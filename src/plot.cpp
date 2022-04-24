#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <string>
#include <utility>

#include <blend2d.h>

#include "plot.hpp"


constexpr int        default_image_size          = 1000;
constexpr color_spec default_background_color    = {1, 1, 1, 0};
constexpr color_spec default_arrow_color         = {0, 0, 0, 1};
constexpr double     default_stem_to_shaft_ratio = 3;
constexpr double     default_head_aspect_ratio   = 1.4;


class quiver_plot
{
public:
    explicit quiver_plot(quiver_spec const& spec);
    void     run();

private:
    void setup_geometry();
    void setup_style();
    void setup_image();
    void init_context();
    void draw_background();
    void draw_arrows();
    void save();

    std::pair<range_spec, range_spec> estimate_data_range() const;

private:
    // Drawing
    BLImage     _image;
    BLContext   _context;
    std::string _output;

    // Raw spec
    quiver_spec _spec;

    // Canvas geometry
    double     _pixels_per_length;
    range_spec _x_range;
    range_spec _y_range;

    // Style
    color_spec _background_color;
    color_spec _arrow_color;
    double     _shaft_width         = 0;
    double     _stem_to_shaft_ratio = 0;
    double     _head_aspect_ratio   = 0;
};


template<typename T>
static T        round(double x);
static void     validate_spec(bool condition, std::string const& message);
static BLRgba32 make_color(color_spec const& spec);
static BLPath   make_arrow_path(double length, double shaft_width, double stem_to_shaft_ratio, double head_aspect_ratio);


void
produce_quiver_plot(quiver_spec const& spec)
{
    quiver_plot{spec}.run();
}


quiver_plot::quiver_plot(quiver_spec const& spec)
: _spec{spec}
{
    setup_geometry();
    setup_style();
    setup_image();
}


void
quiver_plot::setup_geometry()
{
    auto const [data_x_range, data_y_range] = estimate_data_range();
    _x_range = _spec.rendering.x_range.value_or(data_x_range);
    _y_range = _spec.rendering.y_range.value_or(data_y_range);

    validate_spec(_x_range.lower < _x_range.upper, "x_range must be a valid interval");
    validate_spec(_y_range.lower < _y_range.upper, "y_range must be a valid interval");

    auto const max_span = std::max(
        _x_range.upper - _x_range.lower,
        _y_range.upper - _y_range.lower
    );
    _pixels_per_length = _spec.rendering.pixels_per_length.value_or(default_image_size / max_span);

    validate_spec(_pixels_per_length > 0, "pixels_per_length must be positive");
}


void
quiver_plot::setup_style()
{
    auto const pixel_width = 1 / _pixels_per_length;

    _background_color = _spec.style.background_color.value_or(default_background_color);
    _arrow_color = _spec.style.arrow_color.value_or(default_arrow_color);
    _shaft_width = _spec.style.shaft_width.value_or(pixel_width);
    _stem_to_shaft_ratio = _spec.style.stem_to_shaft_ratio.value_or(default_stem_to_shaft_ratio);
    _head_aspect_ratio = _spec.style.head_aspect_ratio.value_or(default_head_aspect_ratio);

    validate_spec(_shaft_width > 0, "shaft_width must be positive");
    validate_spec(_stem_to_shaft_ratio >= 1, "stem_to_shaft_ratio must be >= 1");
    validate_spec(_head_aspect_ratio > 0, "head_aspect_ratio must be positive");
}


void
quiver_plot::setup_image()
{
    auto const width = round<int>((_x_range.upper - _x_range.lower) * _pixels_per_length);
    auto const height = round<int>((_y_range.upper - _y_range.lower) * _pixels_per_length);
    _image = BLImage{width, height, BL_FORMAT_PRGB32};

    if (!_spec.rendering.output) {
        throw std::runtime_error{"output image is not specified"};
    }
    _output = *_spec.rendering.output;
}


void
quiver_plot::run()
{
    _context = BLContext{_image};
    init_context();
    draw_background();
    draw_arrows();
    _context.end();

    save();
}


void
quiver_plot::init_context()
{
    // Blend2D works with the top-to-bottom display coordinate system measured
    // in pixels. To use mathematical xy coordinate system with custom range,
    // we set up a transformation matrix.
    BLMatrix2D view_matrix;
    view_matrix.reset();
    view_matrix.scale(_pixels_per_length);
    view_matrix.translate(-_x_range.lower, _y_range.upper);
    view_matrix.scale(1, -1);
    _context.setMatrix(view_matrix);
    _context.userToMeta();
}


void
quiver_plot::draw_background()
{
    _context.setCompOp(BL_COMP_OP_SRC_COPY);
    _context.setFillStyle(make_color(_background_color));
    _context.fillAll();
}


void
quiver_plot::draw_arrows()
{
    _context.setCompOp(BL_COMP_OP_SRC_OVER);

    for (auto const& arrow : _spec.arrows) {
        auto const length = std::hypot(arrow.dx, arrow.dy);
        auto const angle = std::atan2(arrow.dy, arrow.dx);

        auto path = make_arrow_path(
            length, arrow.w.value_or(_shaft_width), _stem_to_shaft_ratio, arrow.a.value_or(_head_aspect_ratio)
        );

        BLMatrix2D posing;
        posing.reset();
        posing.translate(arrow.x, arrow.y);
        posing.rotate(angle);
        path.transform(posing);

        _context.setFillStyle(make_color(arrow.c.value_or(_arrow_color)));
        _context.fillPath(path);
    }
}


void
quiver_plot::save()
{
    BLImageCodec codec;
    codec.findByName("PNG");

    auto const status = _image.writeToFile(_output.c_str(), codec);
    if (status != BL_SUCCESS) {
        throw std::runtime_error{"failed to save image to file " + _output};
    }
}


std::pair<range_spec, range_spec>
quiver_plot::estimate_data_range() const
{
    if (_spec.arrows.empty()) {
        return std::make_pair(range_spec{0, 1}, range_spec{0, 1});
    }

    auto const x_enclosure = [](arrow_spec const& arrow) {
        auto const x_min = std::min(arrow.x, arrow.x + arrow.dx);
        auto const x_max = std::max(arrow.x, arrow.x + arrow.dx);
        return range_spec{x_min, x_max};
    };

    auto const y_enclosure = [](arrow_spec const& arrow) {
        auto const y_min = std::min(arrow.y, arrow.y + arrow.dy);
        auto const y_max = std::max(arrow.y, arrow.y + arrow.dy);
        return range_spec{y_min, y_max};
    };

    auto max_x_range = x_enclosure(_spec.arrows.front());
    auto max_y_range = y_enclosure(_spec.arrows.front());

    for (auto const& arrow: _spec.arrows) {
        auto const x_range = x_enclosure(arrow);
        auto const y_range = y_enclosure(arrow);

        max_x_range.lower = std::min(max_x_range.lower, x_range.lower);
        max_x_range.upper = std::max(max_x_range.upper, x_range.upper);
        max_y_range.lower = std::min(max_y_range.lower, y_range.lower);
        max_y_range.upper = std::max(max_y_range.upper, y_range.upper);
    }

    return std::make_pair(max_x_range, max_y_range);
}


template<typename T>
T
round(double x)
{
    return static_cast<T>(std::nearbyint(x));
}


void
validate_spec(bool condition, std::string const& message)
{
    if (!condition) {
        throw std::runtime_error{"bad spec: " + message};
    }
}


BLRgba32
make_color(color_spec const& spec)
{
    constexpr double byte_scale = 255;
    return BLRgba32{
        round<unsigned>(byte_scale * spec.red),
        round<unsigned>(byte_scale * spec.green),
        round<unsigned>(byte_scale * spec.blue),
        round<unsigned>(byte_scale * spec.alpha)
    };
}


BLPath
make_arrow_path(double length, double shaft_width, double stem_to_shaft_ratio, double head_aspect_ratio)
{
    auto const head_length = std::min(shaft_width * stem_to_shaft_ratio / head_aspect_ratio, length);
    auto const head_width = head_length * head_aspect_ratio;
    auto const shaft_length = length - head_length;

    auto const x_stem = shaft_length;
    auto const x_head = length;
    auto const y_shaft = shaft_width / 2;
    auto const y_stem = head_width / 2;

    // Arrow-shaped path pointing from the origin to the east.
    BLPath path;
    path.moveTo(0, y_shaft);
    path.lineTo(x_stem, y_shaft);
    path.lineTo(x_stem, y_stem);
    path.lineTo(x_head, 0);
    path.lineTo(x_stem, -y_stem);
    path.lineTo(x_stem, -y_shaft);
    path.lineTo(0, -y_shaft);
    path.close();

    return path;
}

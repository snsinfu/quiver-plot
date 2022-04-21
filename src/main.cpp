#include <cmath>
#include <random>

#include <blend2d.h>


int main()
{
    double const pixels_per_length = 200;
    double const canvas_width = 30;
    double const canvas_height = 30;

    auto const image_width = static_cast<int>(canvas_width * pixels_per_length);
    auto const image_height = static_cast<int>(canvas_height * pixels_per_length);

    BLImage image{image_width, image_height, BL_FORMAT_PRGB32};
    BLContext context{image};

    context.setCompOp(BL_COMP_OP_SRC_COPY);
    context.fillAll();

    BLMatrix2D meta_matrix;
    meta_matrix.reset();
    meta_matrix.translate(canvas_width / 2, canvas_width / 2);
    meta_matrix.postScale(pixels_per_length);
    context.setMatrix(meta_matrix);
    context.userToMeta();

    std::mt19937_64 random;

    for (long i = 0; i < 100'000; i++) {
        BLPath arrow;
        arrow.moveTo(0, 0.5);
        arrow.lineTo(5, 0.5);
        arrow.lineTo(5, 2);
        arrow.lineTo(7, 0);
        arrow.lineTo(5, -2);
        arrow.lineTo(5, -0.5);
        arrow.lineTo(0, -0.5);
        arrow.close();

        std::uniform_real_distribution<double> x_coord{-canvas_width / 2, canvas_width / 2};
        std::uniform_real_distribution<double> y_coord{-canvas_height / 2, canvas_height / 2};
        auto const x = x_coord(random);
        auto const y = y_coord(random);
        auto const dx = y;
        auto const dy = -x;
        auto const angle = std::atan2(dy, dx);

        BLMatrix2D model_matrix;
        model_matrix.reset();
        model_matrix.scale(0.01);
        model_matrix.rotate(angle);
        model_matrix.postTranslate(x, y);
        arrow.transform(model_matrix);

        auto const s = 0.5 + x / canvas_width;
        auto const t = 0.5 + y / canvas_height;
        auto const red = unsigned((0.5 + 0.5 * s) * 255);
        auto const green = unsigned((1.0 - 0.5 * t) * 255);
        auto const blue = unsigned((0.5 + 0.5 * t) * 255);

        context.setCompOp(BL_COMP_OP_SRC_OVER);
        context.setFillStyle(BLRgba32(red, green, blue));
        context.fillPath(arrow);
    }

    context.end();

    BLImageCodec codec;
    codec.findByName("PNG");
    image.writeToFile("_output.png", codec);

    return 0;
}

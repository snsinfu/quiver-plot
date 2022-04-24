#pragma once

#include <optional>
#include <string>
#include <vector>


struct range_spec
{
    double lower = 0;
    double upper = 0;
};


struct color_spec
{
    double red   = 0;
    double green = 0;
    double blue  = 0;
    double alpha = 1;
};


struct rendering_spec
{
    std::optional<double>      pixels_per_length;
    std::optional<range_spec>  x_range;
    std::optional<range_spec>  y_range;
    std::optional<std::string> output;
};


struct style_spec
{
    std::optional<color_spec> background_color;
    std::optional<color_spec> arrow_color;
    std::optional<double>     shaft_width;
    std::optional<double>     stem_to_shaft_ratio;
    std::optional<double>     head_aspect_ratio;
};


struct arrow_spec
{
    double                    x  = 0;
    double                    y  = 0;
    double                    dx = 0;
    double                    dy = 0;
    std::optional<double>     w;
    std::optional<double>     a;
    std::optional<color_spec> c;
};


struct quiver_spec
{
    rendering_spec          rendering;
    style_spec              style;
    std::vector<arrow_spec> arrows;
};


quiver_spec parse_quiver_spec(std::string const& str);

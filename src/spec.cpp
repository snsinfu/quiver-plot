#include <string>

#include <jsoncons/json.hpp>

#include "spec.hpp"


// range_spec as a JSON array of two numbers.
template<class Json>
struct jsoncons::json_type_traits<Json, range_spec>
{
    using allocator_type = typename Json::allocator_type;
    using value_type     = range_spec;

    static bool is(const Json& j) noexcept
    {
        return j.is_array() && j.size() == 2;
    }

    static value_type as(const Json& j)
    {
        return {
            j.at(0).template as<double>(),
            j.at(1).template as<double>(),
        };
    }

    static Json to_json(value_type const& value, allocator_type alloc = {})
    {
        Json j{jsoncons::json_array_arg_t{}, jsoncons::semantic_tag::none, alloc};
        j.push_back(value.lower);
        j.push_back(value.upper);
        return j;
    }
};


// color_spec as a JSON array of three numbers.
template<class Json>
struct jsoncons::json_type_traits<Json, color_spec>
{
    using allocator_type = typename Json::allocator_type;
    using value_type     = color_spec;

    static bool is(const Json& j) noexcept
    {
        return j.is_array() && j.size() == 3;
    }

    static value_type as(const Json& j)
    {
        return {
            j.at(0).template as<double>(),
            j.at(1).template as<double>(),
            j.at(2).template as<double>(),
        };
    }

    static Json to_json(value_type const& value, allocator_type alloc = {})
    {
        Json j{jsoncons::json_array_arg_t{}, jsoncons::semantic_tag::none, alloc};
        j.push_back(value.red);
        j.push_back(value.green);
        j.push_back(value.blue);
        return j;
    }
};


JSONCONS_N_MEMBER_TRAITS(
    rendering_spec,

    // Required fields
    0,

    // Optional fields
    pixels_per_length,
    x_range,
    y_range,
    output
)


JSONCONS_N_MEMBER_TRAITS(
    style_spec,

    // Required fields
    0,

    // Optional fields
    background_color,
    arrow_color,
    shaft_width,
    stem_to_shaft_ratio,
    head_aspect_ratio
)


JSONCONS_N_MEMBER_TRAITS(
    arrow_spec,

    // Required fields
    4,
    x,
    y,
    dx,
    dy,

    // Optional fields
    w,
    a,
    c
)


JSONCONS_N_MEMBER_TRAITS(
    quiver_spec,

    // Required fields
    0,

    // Optional fields
    rendering,
    style,
    arrows
)


quiver_spec
parse_quiver_spec(std::string const& str)
{
    return jsoncons::decode_json<quiver_spec>(str);
}

import json
import math
import random


n_arrows = 10000
pixel_density = 500
view_radius = 2
v_min = 0.01
radial_accel = 0.001
phase_shift = 0.5
thickness_factor = 0.1

random.seed(0)


def main():
    spec = make_spec()
    with open("sample.json", "w") as file:
        json.dump(spec, file, cls=SuccinctEncoder, indent=4, indent_upto=2)


def make_spec():
    arrows = []

    def clip(x, lo, hi):
        return min(max(x, lo), hi)

    for i in range(n_arrows):
        x = random.uniform(-view_radius, view_radius)
        y = random.uniform(-view_radius, view_radius)
        r = math.hypot(x, y)
        ph = math.atan2(y, x)

        v = v_min + math.sqrt(radial_accel * r)
        dx = v * math.sin(ph + phase_shift)
        dy = -v * math.cos(ph + phase_shift)

        s = clip((view_radius + x) / (2 * view_radius), 0, 1)
        t = clip((view_radius + y) / (2 * view_radius), 0, 1)
        red = 0.5 + 0.5 * s
        green = 1.0 - 0.5 * t
        blue = 0.5 + 0.5 * t
        color = [red, green, blue]

        arrows.append({
            "x": x,
            "y": y,
            "dx": dx,
            "dy": dy,
            "w": thickness_factor * v,
            "c": color,
        })


    spec = {
        "rendering": {
            "pixels_per_length": pixel_density,
            "x_range": [-view_radius, view_radius],
            "y_range": [-view_radius, view_radius]
        },

        "style": {
            "background_color": [0, 0, 0],
            "stem_to_shaft_ratio": 5
        },

        "arrows": arrows
    }
    return spec


class SuccinctEncoder(json.JSONEncoder):
    def __init__(self, indent_upto=0, indent=4, **kwargs):
        self._indent_upto = indent_upto
        self._indent = indent
        super().__init__(**kwargs)

    def iterencode(self, value, **kwargs):
        if isinstance(value, float):
            return self._encode_float(value, **kwargs)
        if isinstance(value, list):
            return self._encode_list(value, **kwargs)
        if isinstance(value, dict):
            return self._encode_dict(value, **kwargs)
        return super().iterencode(value)

    def _encode_float(self, value, **kwargs):
        yield f"{value:g}"

    def _encode_list(self, lst, *, nest_level=0, **kwargs):
        return self._encode_structure(lst, ListTraits, nest_level=nest_level, **kwargs)

    def _encode_dict(self, lst, *, nest_level=0, **kwargs):
        return self._encode_structure(lst, DictTraits, nest_level=nest_level, **kwargs)

    def _encode_structure(self, struct, traits, *, nest_level, **kwargs):
        kwargs["nest_level"] = nest_level + 1
        do_indent = nest_level < self._indent_upto
        yield traits.opener
        if do_indent:
            yield "\n"
        for i, item in enumerate(traits.items(struct)):
            if i != 0:
                yield traits.delimiter
                yield "\n" if do_indent else " "
            if do_indent:
                yield self._make_indent(nest_level + 1)
            yield from traits.iterencode_item(self, item, **kwargs)
        if do_indent:
            yield "\n"
            yield self._make_indent(nest_level)
        yield traits.closer

    def _make_indent(self, level):
        return " " * (self._indent * level)


class ListTraits:
    opener = "["
    closer = "]"
    delimiter = ","

    @classmethod
    def items(self, lst):
        return iter(lst)

    @classmethod
    def iterencode_item(self, encoder, item, **kwargs):
        return encoder.iterencode(item, **kwargs)


class DictTraits:
    opener = "{"
    closer = "}"
    delimiter = ","

    @classmethod
    def items(self, dic):
        return dic.items()

    @classmethod
    def iterencode_item(self, encoder, item, **kwargs):
        key, value = item
        yield from encoder.iterencode(key, **kwargs)
        yield ": "
        yield from encoder.iterencode(value, **kwargs)


main()

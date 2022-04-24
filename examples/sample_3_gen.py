import json
import math

from sample_2_gen import SuccinctEncoder


def main():
    spec = make_spec()
    with open("sample_3.json", "w") as file:
        json.dump(spec, file, cls=SuccinctEncoder, indent=4, indent_upto=2)


def make_spec():
    arrows = []

    vortices = [
        [20, 7.5, -1.1, 20],
        [50, 7.5,  0.9, 20],
        [80, 7.5, -1.1, 20],
    ]

    v_unit = 0.9
    phase_shift = 0.6

    def clip(x):
        return min(max(x, 0), 1)

    for i in range(1500):

        x = 0.5 + i // 15
        y = 0.5 + i % 15

        dx = 0
        dy = 0
        for cx, cy, v, w in vortices:
            r = math.hypot(x - cx, y - cy)
            ph = math.atan2(y - cy, x - cx) + phase_shift
            dx +=  v * math.sin(ph) * math.exp(-r / w)
            dy += -v * math.cos(ph) * math.exp(-r / w)

        s = clip((dx + v_unit) / (2 * v_unit))
        t = clip((dy + v_unit) / (2 * v_unit))
        r = 0.5 + 0.5 * s
        g = 1.0 - 0.5 * t
        b = 0.5 + 0.5 * t

        l = math.hypot(dx, dy)
        m = math.sqrt(l / v_unit)
        w = 0.2 * m
        a = 1.4 * m**2

        arrows.append({
            "x": x,
            "y": y,
            "dx": dx,
            "dy": dy,
            "w": w,
            "a": a,
            "c": [r, g, b],
        })

    spec = {
        "rendering": {
            "pixels_per_length": 10,
            "x_range": [0, 100],
            "y_range": [0, 15],
        },

        "style": {
            "background_color": [0, 0, 0],
            "shaft_width": 0.2,
            "stem_to_shaft_ratio": 3,
            "head_aspect_ratio": 1.4,
        },

        "arrows": arrows,
    }
    return spec


main()
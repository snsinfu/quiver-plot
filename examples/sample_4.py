import io
import json
import subprocess

import matplotlib.pyplot as plt
import PIL.Image


def main():
    spec = {
        "rendering": {
            "pixels_per_length": 500,
            "x_range": [-1.5, 1.5],
            "y_range": [-1.5, 1.5]
        },

        "style": {
            "background_color": [1, 1, 1],
            "shaft_width": 0.2
        },

        "arrows": [
            {"x": -1, "y":  1, "dx":  2, "dy": -2, "c": [1, 0, 0, 0.5]},
            {"x":  1, "y":  1, "dx": -2, "dy": -2, "c": [0, 0, 1, 0.5]}
        ]
    }

    with subprocess.Popen(["quiver", "-o", "/dev/stdout", "/dev/stdin"], stdin=subprocess.PIPE, stdout=subprocess.PIPE) as proc:
        json.dump(spec, io.TextIOWrapper(proc.stdin))
        image = PIL.Image.open(proc.stdout)
        image.load()
    
    extent = spec["rendering"]["x_range"] + spec["rendering"]["y_range"]
    plt.imshow(image, extent=extent, interpolation="kaiser")
    plt.show()


main()
# Examples

## [sample_0.json](sample_0.json)

The up & down arrow example shown in [readme](../README.md).


## [sample_1.json](sample_0.json)

Two hexagonal cycles in different style.


## [sample_2_gen.py](sample_2_gen.py)

Generates the quiver spec for a swirl made of 10000 arrows.


## [sample_3_gen.py](sample_3_gen.py)

Generates the quiver spec for the vortex image shown in the top of
[readme](../README.md).


## [sample_4.py](sample_4.py)

Drawing a quiver plot on a matplotlib figure. The Python script constructs
a quiver spec and passes it to **quiver** program via `stdin`. The resulting
image is written to `stdout` and loaded from the script as a `PIL.Image`. The
image is then rendered on a matplotlib figure. No temporary file is created.

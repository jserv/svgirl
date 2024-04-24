# SVGirl

## Overview

SVGirl stands for SVG Instant Rendering Library.
It is a library for parsing SVG files into a simplified internal representation suitable for display or manipulation.
This library is derived from [libsvgtiny](https://www.netsurf-browser.org/projects/libsvgtiny/) and focuses on minimal dependencies and embedded systems.

The overall idea of the library is to take SVG files as input and return a list of paths and texts that can be rendered easily.
The library does not perform the actual rendering.

All supported SVG objects, such as circles, lines, and gradient-filled shapes, are converted into flat-filled paths or fragments of text.
All coordinates are converted and transformed to pixels.

SVGirl is freely redistributable under the [MIT License](https://opensource.org/license/mit).

## SVG support

SVGirl is initially aiming to implement SVG Tiny, as defined in
[Scalable Vector Graphics (SVG) Tiny Specification](https://www.w3.org/TR/SVGMobile/).

SVG Tiny elements supported: defs, g, svg, circle, line, path, polygon,
polyline, rect, text

SVG Tiny elements not yet supported: desc, metadata, title, use, a,
switch, ellipse, image, font, font-face, font-face-name, font-face-src,
glyph, hkern, missing-glyph, animate, animateColor, animateMotion,
animateTransform, mpath, set, foreignObject

Additional elements supported: linearGradient, stop

Text support is incomplete: no fonts. linearGradient support is
incomplete: They parse, but the internal data structure doesn't (yet)
represent the gradient.

The style attribute is supported.

## Building SVGirl

Compile all sources along with the test programs:
```shell
$ make
```

Launch test suite:
```shell
make check
```

Build reference SVG viewer on X Window System:
```shell
$ make bin/display_x11
```

## Using SVGirl

The interface is in the header `svgtiny.h`
```c
#include "svgtiny.h"
```

First create a `svgtiny_diagram` using `svgtiny_create()`:
```c
struct svgtiny_diagram *diagram;
diagram = svgtiny_create();
```

This will return a pointer to a new diagram, or NULL if there was not enough memory available.

SVGs are parsed from memory using `svgtiny_parse()`:
```c
svgtiny_code code = svgtiny_parse(diagram, buffer, size, url, 1000, 1000);
```

The arguments include the pointer returned by `svgtiny_create()`,
a buffer containing the SVG data, the size of the SVG in bytes,
the URL from which the SVG was sourced, and the target viewport width and height in pixels.

The function returns `svgtiny_OK` if there are no problems, and the diagram is updated.
The diagram can then be rendered by looping through the array `diagram->shape[0..diagram->shape_count]`:
```c
for (unsigned int i = 0; i != diagram->shape_count; i++) {
```

Path shapes have a non-`NULL` path pointer.
The path consists of an array of floats with a length defined by `path_length`.
This array contains segment type codes followed by 0 to 3 pairs of coordinates,
depending on the segment type:
- `svgtiny_PATH_MOVE x y`
- `svgtiny_PATH_CLOSE`
- `svgtiny_PATH_LINE x y`
- `svgtiny_PATH_BEZIER x1 y1 x2 y2 x3 y3`

A path always starts with a `MOVE`.

The fill and stroke attributes specify the colors of the path,
or `svgtiny_TRANSPARENT` if the path is neither filled nor stroked.
Colors are in the `0xRRGGBB` format (depending on endianness).
The macros `svgtiny_RED`, `svgtiny_GREEN`, and `svgtiny_BLUE` can be used to extract the color components.

The width of the path is specified in `stroke_width`.

Text shapes have a `NULL` path pointer and a non-`NULL` text pointer.
The text is in UTF-8.
The coordinates for the text are specified in `text_x` and `text_y`.
Text colors and stroke widths are the same as for paths.

If memory runs out during parsing, `svgtiny_parse()` returns `svgtiny_OUT_OF_MEMORY`.
However, the diagram remains valid up to the point where memory was exhausted and can be safely rendered.

If there is an error in the SVG (for example, if an element is missing an attribute required by the specification),
`svgtiny_SVG_ERROR` is returned.
Nevertheless, the diagram is still valid up to the point of the error.
The error is recorded in `diagram->error_message` and the line that caused it in `diagram->error_line`.

`svgtiny_LIBDOM_ERROR` indicates that parsing the XML failed,
resulting in a diagram that will contain no shapes.
`svgtiny_NOT_SVG` means that the XML does not contain a top-level `<svg>` element.

To free memory used by a diagram, use `svgtiny_free()`:
```c
svgtiny_free(diagram);
```

For an example, see `examples/svgtiny_display_x11.c`.

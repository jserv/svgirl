/*
 * This file is part of Libsvgtiny
 * Licensed under the MIT License,
 *                http://opensource.org/licenses/mit-license.php
 * Copyright 2008 James Bursa <james@semichrome.net>
 */

#ifndef SVGTINY_H
#define SVGTINY_H

// To get the definition of size_t
#include <endian.h>
#include <stdlib.h>

typedef unsigned int svgtiny_colour;
#if __BYTE_ORDER == __BIG_ENDIAN
#define svgtiny_RGB(r, g, b) (0xff << 24 | (b) << 16 | (g) << 8 | (r))
#define svgtiny_RED(c) ((c) &0xff)
#define svgtiny_GREEN(c) (((c) >> 8) & 0xff)
#define svgtiny_BLUE(c) (((c) >> 16) & 0xff)
#elif __BYTE_ORDER == __LITTLE_ENDIAN
#define svgtiny_RGB(r, g, b) (0xff << 24 | (r) << 16 | (g) << 8 | (b))
#define svgtiny_RED(c) (((c) >> 16) & 0xff)
#define svgtiny_GREEN(c) (((c) >> 8) & 0xff)
#define svgtiny_BLUE(c) ((c) &0xff)
#else
#error "unable to determine endianess!"
#endif
#define svgtiny_ALPHA(c) (((c) >> 24) & 0xff)

// Use these colors to indicate special.
// They are legal, but unlikely, black that's almost transparent.
// TODO: in a future commit, we'll malloc the gradient description into the
// _internal_extensions
// field of the svgtiny_shape, and we'll lopk at that to decide if an object has
// a gradient.
#define svgtiny_LINEAR_GRADIENT 0x2000000
#define svgtiny_TRANSPARENT 0x1000000


struct svgtiny_shape {
    float *path;
    unsigned int path_length;
    char *text;
    float text_x, text_y;
    svgtiny_colour fill;
    svgtiny_colour stroke;
    float stroke_width;
    void *_internal_extensions;  // TODO: if non-NULL, points to an allocated on
                                 // the heap extension block. (gradients, fonts)
};

struct svgtiny_diagram {
    int width, height;

    struct svgtiny_shape *shape;
    unsigned int shape_count;

    unsigned short error_line;
    const char *error_message;
};

typedef enum {
    svgtiny_OK,
    svgtiny_OUT_OF_MEMORY,
    svgtiny_LIBDOM_ERROR,
    svgtiny_NOT_SVG,
    svgtiny_SVG_ERROR,
} svgtiny_code;

enum {
    svgtiny_PATH_MOVE,
    svgtiny_PATH_CLOSE,
    svgtiny_PATH_LINE,
    svgtiny_PATH_BEZIER
};

struct svgtiny_named_color {
    const char *name;
    svgtiny_colour color;
};


struct svgtiny_diagram *svgtiny_create(void);

svgtiny_code svgtiny_parse(struct svgtiny_diagram *diagram,
                           const char *buffer,
                           size_t size,
                           const char *url,
                           int width,
                           int height);

// Gets the width and size from the buffer.
svgtiny_code svgtiny_parse0(struct svgtiny_diagram *diagram,
                            const char *buffer,
                            size_t size);


void svgtiny_free(struct svgtiny_diagram *svg);

#endif

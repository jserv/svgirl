/*
 * This file is part of Libsvgtiny
 * Released under the MIT License, https://opensource.org/license/mit
 * Copyright 2008 James Bursa <james@semichrome.net>
 */

#include "svgtiny.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define INPUT_FILE "tests/apple.svg"

int main(int argc, char *argv[])
{
    FILE *fd;
    float scale = 1.0;
    struct stat sb;
    char *buffer;
    size_t size;
    size_t n;
    struct svgtiny_diagram *diagram;
    svgtiny_code code;

    /* load file into memory buffer */
    fd = fopen(INPUT_FILE, "rb");
    if (!fd) {
        return 1;
    }

    if (stat(INPUT_FILE, &sb)) {
        return 1;
    }
    size = sb.st_size;

    buffer = malloc(size);
    if (!buffer) {
        fprintf(stderr, "Unable to allocate %lld bytes\n", (long long) size);
        return 1;
    }

    n = fread(buffer, 1, size, fd);
    if (n != size)
        return 1;

    fclose(fd);

    /* create svgtiny object */
    diagram = svgtiny_create();
    if (!diagram) {
        fprintf(stderr, "svgtiny_create failed\n");
        return 1;
    }

    /* parse */
    code = svgtiny_parse(diagram, buffer, size, INPUT_FILE, 320, 240);
    if (code != svgtiny_OK) {
        fprintf(stderr, "svgtiny_parse failed: ");
        switch (code) {
        case svgtiny_OUT_OF_MEMORY:
            fprintf(stderr, "svgtiny_OUT_OF_MEMORY");
            break;
        case svgtiny_LIBDOM_ERROR:
            fprintf(stderr, "svgtiny_LIBDOM_ERROR");
            break;
        case svgtiny_NOT_SVG:
            fprintf(stderr, "svgtiny_NOT_SVG");
            break;
        case svgtiny_SVG_ERROR:
            fprintf(stderr, "svgtiny_SVG_ERROR: line %i: %s",
                    diagram->error_line, diagram->error_message);
            break;
        default:
            fprintf(stderr, "unknown svgtiny_code %i", code);
            break;
        }
        fprintf(stderr, "\n");
    }

    free(buffer);

    printf("viewbox 0 0 %g %g\n", scale * diagram->width,
           scale * diagram->height);

    for (unsigned int i = 0; i != diagram->shape_count; i++) {
        if (diagram->shape[i].fill == svgtiny_TRANSPARENT)
            printf("fill none ");
        else
            printf("fill #%.6x ", diagram->shape[i].fill);
        if (diagram->shape[i].stroke == svgtiny_TRANSPARENT)
            printf("stroke none ");
        else
            printf("stroke #%.6x ", diagram->shape[i].stroke);
        printf("stroke-width %g ", scale * diagram->shape[i].stroke_width);
        if (diagram->shape[i].path) {
            printf("path '");
            for (unsigned int j = 0; j != diagram->shape[i].path_length;) {
                switch ((int) diagram->shape[i].path[j]) {
                case svgtiny_PATH_MOVE:
                    printf("M %g %g ", scale * diagram->shape[i].path[j + 1],
                           scale * diagram->shape[i].path[j + 2]);
                    j += 3;
                    break;
                case svgtiny_PATH_CLOSE:
                    printf("Z ");
                    j += 1;
                    break;
                case svgtiny_PATH_LINE:
                    printf("L %g %g ", scale * diagram->shape[i].path[j + 1],
                           scale * diagram->shape[i].path[j + 2]);
                    j += 3;
                    break;
                case svgtiny_PATH_BEZIER:
                    printf("C %g %g %g %g %g %g ",
                           scale * diagram->shape[i].path[j + 1],
                           scale * diagram->shape[i].path[j + 2],
                           scale * diagram->shape[i].path[j + 3],
                           scale * diagram->shape[i].path[j + 4],
                           scale * diagram->shape[i].path[j + 5],
                           scale * diagram->shape[i].path[j + 6]);
                    j += 7;
                    break;
                default:
                    printf("error ");
                    j += 1;
                }
            }
            printf("' ");
        } else if (diagram->shape[i].text) {
            printf("text %g %g '%s' ", scale * diagram->shape[i].text_x,
                   scale * diagram->shape[i].text_y, diagram->shape[i].text);
        }
        printf("\n");
    }

    svgtiny_free(diagram);

    return 0;
}

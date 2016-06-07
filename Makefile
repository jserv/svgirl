# A simple makefile to make or test the display_x11.
# part of libsvgtiny. M.I.T. license

SVGTINY_C= libsvgtiny/src/svgColor2.c \
  libsvgtiny/src/svgtiny.c \
  libsvgtiny/src/svgtiny_gradient.c \
  libsvgtiny/src/svgtiny_list.c \
  libsvgtiny/src/xml2dom.c \
  libsvgtiny/src/ezxml.c

SVGTINY_H= libsvgtiny/inc/svgtiny.h libsvgtiny/src/svgtiny_internal.h \
  libsvgtiny/src/svgtiny_strings.h \
  libsvgtiny/src/xml2dom.h \
  libsvgtiny/src/ezxml.h

SVGDIRECTORIES= -Ilibsvgtiny/inc -Ilibsvgtiny/src -I.
CFLAGS= $(SVGDIRECTORIES) -DDEBUG_MODE
all: bin/display_x11

# bin/libsvgtiny.a : $(SVGTINY_O) $(SVGTINY_H)
# 	- mkdir bin
# 	ar rs bin/libsvgtiny.a $(SVGTINY_O)

bin/display_x11 : examples/svgtiny_display_x11.c $(SVGTINY_C)
	- mkdir bin
	cc $(CFLAGS) -g -o bin/display_x11 examples/svgtiny_display_x11.c $(SVGTINY_C) -lm -I/usr/include/cairo -L/usr/local/lib/ -lcairo -lX11

test: bin/display_x11
	bin/display_x11 img/apple.svg

clean:
	rm -rf bin


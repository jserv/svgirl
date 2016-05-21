# A simple makefile to make or test the display_x11.
# part of libsvgtiny. M.I.T. license

SVGTINY_C= libsvgtiny/src/svgColor2.c \
  libsvgtiny/src/svgtiny.c \
  libsvgtiny/src/svgtiny_gradient.c \
  libsvgtiny/src/svgtiny_list.c \
  libsvgtiny/src/xml2dom.c

SVGTINY_O= libsvgtiny/src/svgColor2.o \
  libsvgtiny/src/svgtiny.o \
  libsvgtiny/src/svgtiny_gradient.o \
  libsvgtiny/src/svgtiny_list.o \
  libsvgtiny/src/xml2dom.o


SVGTINY_H= libsvgtiny/inc/svgtiny.h libsvgtiny/src/svgtiny_internal.h \
  libsvgtiny/src/svgtiny_strings.h \
  libsvgtiny/src/xml2dom.h

SVGDIRECTORIES= -I/usr/include/libxml2 -Ilibsvgtiny/inc -I.
CFLAGS= $(SVGDIRECTORIES)
all: bin/display_x11

bin/libsvgtiny.a : $(SVGTINY_O) $(SVGTINY_H)
	- mkdir bin
	ar rs bin/libsvgtiny.a $(SVGTINY_O)

bin/display_x11 : bin/libsvgtiny.a  examples/svgtiny_display_x11.c
	cc $(CFLAGS) -o bin/display_x11 examples/svgtiny_display_x11.c bin/libsvgtiny.a -lxml2 -lm -I/usr/include/cairo -L/usr/local/lib/ -lcairo -lX11

test: bin/display_x11
	bin/display_x11 img/apple.svg

clean:
	rm -rf $(SVGTINY_O) $(SVGTINYWRITER_O) bin


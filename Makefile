CFLAGS := \
    -Wall -std=gnu99 -O2 \
    -I./include \
    -I. -MMD

OBJS = \
    src/svgtiny_color.o \
    src/svgtiny.o \
    src/svgtiny_gradient.o \
    src/svgtiny_list.o \
    src/xml2dom.o \
    src/ezxml.o

deps := $(OBJS:%.o=%.d)

all: bin/display_x11

bin/display_x11 : examples/svgtiny_display_x11.c $(OBJS)
	mkdir -p bin
	$(CC) $(CFLAGS) -g \
		-o bin/display_x11 examples/svgtiny_display_x11.c \
		$(OBJS) \
		`pkg-config --cflags --libs cairo` -lm -lX11

test: bin/display_x11
	bin/display_x11 img/apple.svg

clean:
	$(RM) bin/display_x11 $(OBJS) $(deps)

-include $(deps)

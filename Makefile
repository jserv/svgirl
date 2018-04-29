CFLAGS := \
    -Wall -std=gnu99 -O2 \
    -I./include \
    -I. -MMD

LDFLAGS := \
    -lm

OBJS = \
    src/svgtiny_color.o \
    src/svgtiny.o \
    src/svgtiny_gradient.o \
    src/svgtiny_list.o \
    src/dom.o \
    src/ezxml.o

deps := $(OBJS:%.o=%.d)

TESTS := \
    tests/ezxml \
    tests/svgtiny \
    tests/decode_svg

tests/%: tests/%.o
	$(CC) $(CFLAGS) -o $@ $^ $(OBJS) $(LDFLAGS)
deps += $(TESTS:%=%.d)

all: $(OBJS) $(TESTS)

EXAMPLES := \
	bin/display_x11
bin/display_x11: examples/svgtiny_display_x11.c $(OBJS)
	mkdir -p bin
	$(CC) $(CFLAGS) -g \
		-o $@ $< $(OBJS) \
		$(LDFLAGS) `pkg-config --cflags --libs cairo` -lX11

check: $(OBJS) $(TESTS)
	@tests/runner.sh $(PWD) || echo "Check tests/log for details"

clean:
	$(RM) $(EXAMPLES) $(TESTS) $(TESTS:%=%.o) tests/log $(OBJS) $(deps)

-include $(deps)

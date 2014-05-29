lib_x86_ref := libx86ref.so
sources := intel_x86_ref.c
objects := $(subst .c,.o,$(sources))

include_dirs := ./
CFLAGS += $(addprefix -I ,$(include_dirs))
CFLAGS += -fPIC
CFLAGS += -g -Wall -pedantic
LDFLAGS  += -shared -lsqlite3

.PHONY: all

all: $(lib_x86_ref)

$(lib_x86_ref): $(objects)
	$(CC) $< $(LDFLAGS) -o $@

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

clean:
	rm -f $(lib_x86_ref)

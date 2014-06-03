include ../../mktools/show_make_rules.mk

lib_x86_ref_dyn_shared  := libx86ref.so
lib_x86_ref_static		:= libx86ref.a
sources					:= intel_x86_ref.c
objects					:= $(subst .c,.o,$(sources))

include_dirs := ./

CFLAGS  += $(addprefix -I ,$(include_dirs))
CFLAGS  += -fPIC
CFLAGS  += -g -Wall -pedantic
LDFLAGS += -shared -lsqlite3
RANLIB  := ranlib

.PHONY: all dyn_shared_lib static_lib

all: dyn_shared_lib static_lib
dyn_shared_lib: $(lib_x86_ref_dyn_shared)
static_lib: $(lib_x86_ref_static)

$(lib_x86_ref_dyn_shared): $(objects)
	$(CC) $< $(LDFLAGS) -o $@

$(lib_x86_ref_static): $(objects)
	$(RM) $@
	$(AR) $(ARFLAGS) $@ $?
	$(RANLIB) $@

%.o: %.c intel_x86_ref.h
	$(CC) -c $(CFLAGS) -o $@ $<

clean:
	rm -f $(lib_x86_ref_static) $(lib_x86_ref_dyn_shared) *.o

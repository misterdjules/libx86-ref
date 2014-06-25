include deps/mktools/show_make_rules.mk

lib_x86_ref_dyn_shared  := libx86ref.so
lib_x86_ref_static		:= libx86ref.a
sources					:= intel_x86_ref.c
objects					:= $(subst .c,.o,$(sources))

sqlite_helper_dir 	:= ./deps/sql-query-generator
jstring_lib_dir 	:= ./deps/jstring
debug_lib_dir 		:= ./deps/debug

jstring_static_lib := $(jstring_lib_dir)/libjstring.a
debug_static_lib := $(debug_lib_dir)/libdebug.a

include_dirs := ./ $(sqlite_helper_dir) $(debug_lib_dir) $(jstring_lib_dir)

sqlite_helper_static_lib := $(sqlite_helper_dir)/libsqlitehelper.a

CFLAGS  += $(addprefix -I, $(include_dirs))
CFLAGS  += -fPIC
CFLAGS  += -g -Wall -pedantic
LDFLAGS += -shared -lsqlite3
RANLIB  := ranlib

.PHONY: all dyn_shared_lib static_lib test clean

all: dyn_shared_lib static_lib
dyn_shared_lib: $(lib_x86_ref_dyn_shared)
static_lib: $(lib_x86_ref_static)

$(lib_x86_ref_dyn_shared): $(objects) $(sqlite_helper_static_lib) $(jstring_static_lib) $(debug_static_lib)
	$(CC) $^ $(LDFLAGS) -o $@

$(lib_x86_ref_static): $(objects) $(sqlite_helper_static_lib) $(jstring_static_lib) $(debug_static_lib)
	$(RM) $@
	$(AR) $(ARFLAGS) $@ $^
	$(RANLIB) $@

$(sqlite_helper_static_lib):
	$(MAKE) --directory=$(dir $@)

$(jstring_static_lib):
	$(MAKE) --directory=$(dir $@)

$(debug_static_lib):
	$(MAKE) --directory=$(dir $@)

%.o: %.c intel_x86_ref.h
	$(CC) -c $(CFLAGS) -o $@ $<

test: all
	$(MAKE) --directory=$@

clean: clean_test
	rm -f $(lib_x86_ref_static) $(lib_x86_ref_dyn_shared) *.o

clean_test:
	$(MAKE) --directory=test clean

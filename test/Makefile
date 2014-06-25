include ../deps/mktools/show_make_rules.mk

LIBX86_REF_DIR  := ..
LIBX86_REF_STATIC_LIB := $(LIBX86_REF_DIR)/libx86ref.so

GTEST_DIR 		:= gtest-1.7.0
GTEST_MAIN_LIB 	:= ./$(GTEST_DIR)/make/gtest_main.a
# Google Test uses thread for part of its implementation
GTEST_LDFLAGS   := -lpthread $(LIBX86_REF_STATIC_LIB)

INCLUDE_DIRS := $(LIBX86_REF_DIR) $(GTEST_DIR)/include

TESTS_EXE_DIR := ./tests_exe
ALL_TESTS := get_all_instructions_test
ALL_TESTS := $(addprefix $(TESTS_EXE_DIR)/,$(ALL_TESTS))
CXXFLAGS += $(addprefix -I,$(INCLUDE_DIRS))

.PHONY: all

all: $(ALL_TESTS)
	# run all tests' binary executable
	$(foreach test, $(ALL_TESTS), \
		./$(test) \
	)

define TEST_TEMPLATE
$1: $(GTEST_MAIN_LIB) $(notdir $(join $1,.cc))
	mkdir -p $(TESTS_EXE_DIR)
	$(CXX) $(CXXFLAGS) $(GTEST_MAIN_LIB) $(notdir $(join $1,.cc)) $(GTEST_LDFLAGS) -o $1
endef

# Generates each test's binary executable using the template "TEST_TEMPLATE"
# defined above
$(foreach test, $(ALL_TESTS), \
	$(eval $(call TEST_TEMPLATE,$(test))) \
)

# Builds Google Test static lib
$(GTEST_MAIN_LIB):
	$(MAKE) --directory=$(dir $@)

# Removes all tests' binary executable file
clean:
	rm -f *test

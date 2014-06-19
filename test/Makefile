include ../../../mktools/show_make_rules.mk

GTEST_DIR 		:= gtest-1.7.0
GTEST_MAIN_LIB 	:= ./$(GTEST_DIR)/make/gtest_main.a
# Google Test uses thread for part of its implementation
GTEST_LDFLAGS   := -lpthread

ALL_TESTS := get_all_instructions_test

.PHONY: all

all: $(ALL_TESTS)
	# run all tests' binary executable
	$(foreach test, $(ALL_TESTS), \
		./$(test) \
	)

define TEST_TEMPLATE
$1: $(GTEST_MAIN_LIB) $(join $1,.cc)
	$(CXX) -I ./$(GTEST_DIR)/include $(GTEST_MAIN_LIB) $(join $1,.cc) $(GTEST_LDFLAGS) -o $1
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

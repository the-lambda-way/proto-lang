# Practical makefiles, by example: http://nuclear.mutantstargoat.com/articles/make/
# Automatic variables: https://www.gnu.org/software/make/manual/html_node/Automatic-Variables.html

CXX      := /usr/local/gcc-10.2.0/bin/g++-10.2
CXXFLAGS := -O3 -MMD
CPPFLAGS := -std=c++20

ROOT     := /home/mike/projects/languages/proto/repo
INCLUDES := -I/usr/local/gcc-10.2.0/include/c++/10.2.0/ -I$(ROOT)/external -I$(ROOT)/source

COMPILE  := $(CXX) $(CXXFLAGS) $(CPPFLAGS) $(INCLUDES)


# ======================================================================================================================
# Tests
# ======================================================================================================================
TEST_SRCS := $(filter-out tests/main.test.cpp,$(shell find tests/ -name "*.test.cpp"))
TEST_EXES := $(addprefix build/,$(TEST_SRCS:.cpp=.out))


.PHONY: tests
tests: build/tests/main.test.o $(TEST_EXES)


build/%.test.out: %.test.cpp
	@echo "building $(@F) ..."
	@mkdir -p $(@D)
	@$(COMPILE) build/tests/main.test.o $< -o $@


build/tests/main.test.o: tests/main.test.cpp
	@echo "building $(@F) ..."
	@mkdir -p $(@D)
	@$(COMPILE) $< -c -o $@


# Dependency rules included at bottom of file

# Recompiles on change, but doesn't run, so the test harness can autorun. Pass src= on the command line.
.PHONY: watch-test
watch-test: exe=$($(src:/tests/=/build/tests/):.cpp=.out)
watch-test:
	@while true; do                            \
		clear;                                \
		$(MAKE) $(exe) --no-print-directory;  \
		                                      \
		echo "watching $(notdir $(src)) ..."; \
		inotifywait -qq -e modify $(src);     \
	done


# ======================================================================================================================
# Examples
# ======================================================================================================================
.PHONY: examples
examples:


# ======================================================================================================================
# Docs
# ======================================================================================================================
.PHONY: docs
docs:
	$(MAKE) -C docs html


# ======================================================================================================================
# Misc
# ======================================================================================================================
.PHONY: clean-all clean-tests clean-examples clean-docs
clean-all:
	rm -rf build/


clean-tests:
	rm -rf build/tests


clean-examples:
	rm -rf build/examples


clean-docs:
	rm -rf build/docs


.PHONY: troubleshoot
troubleshoot:
	@echo $(TEST_SRCS)
	@echo $(TEST_EXES)
	@echo $(TEST_DEPS)



TEST_DEPS := $(TEST_EXES:.out=.d)
-include $(TEST_DEPS)

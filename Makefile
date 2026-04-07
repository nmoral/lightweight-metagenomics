CXX      = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -Isrc -Itests
CATCH    = -lCatch2Main -lCatch2
COVERAGE = --coverage -fprofile-abs-path

# Sources communes (sans aucun main)
COMMON_SRCS = $(filter-out src/mains/%, $(wildcard src/**/*.cpp))
COMMON_OBJS = $(patsubst src/%.cpp, output/%.o, $(COMMON_SRCS))

# Sources de tests
TEST_SRCS = $(wildcard tests/**/*.cpp)
TEST_OBJS = $(patsubst tests/%.cpp, output/tests/%.o, $(TEST_SRCS))

# Dossiers de sortie necessaires
OUTDIRS = $(sort $(dir $(COMMON_OBJS))) \
          output/mains \
          $(sort $(dir $(TEST_OBJS)))

.PHONY: all clean index bits benchmark tests run_tests

all: index bits benchmark

# --- Executables ---

index: .create_dirs $(COMMON_OBJS) output/mains/main_index.o
	$(CXX) $(CXXFLAGS) $(COMMON_OBJS) output/mains/main_index.o -o output/index

bits: .create_dirs $(COMMON_OBJS) output/mains/main_bits.o
	$(CXX) $(CXXFLAGS) $(COMMON_OBJS) output/mains/main_bits.o -o output/bits

benchmark: .create_dirs $(COMMON_OBJS) output/mains/main_benchmark.o
	$(CXX) $(CXXFLAGS) $(COMMON_OBJS) output/mains/main_benchmark.o -o output/benchmark

# --- Tests ---

tests: .create_dirs $(COMMON_OBJS) $(TEST_OBJS)
	$(CXX) $(CXXFLAGS) $(COMMON_OBJS) $(TEST_OBJS) $(CATCH) -o output/tests/runner

# --- Compilation des objets src ---

output/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# --- Compilation des objets tests ---

output/tests/%.o: tests/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# --- Utilitaires ---

.create_dirs:
	mkdir -p $(OUTDIRS)

clean:
	rm -rf output

run_index: index
	./output/index

run_bits: bits
	./output/bits

run_benchmark: benchmark
	./output/benchmark

run_tests: tests
	./output/tests/runner

coverage: CXXFLAGS += $(COVERAGE)
coverage: clean tests
	./output/tests/runner
	lcov --capture --directory output --output-file output/coverage.info
	lcov --remove output/coverage.info '/usr/*' --output-file output/coverage.info
	genhtml output/coverage.info --output-directory output/coverage_html
	@echo "Coverage report: output/coverage_html/index.html"
	xdg-open output/coverage_html/index.html

clean_coverage:
	find output -name "*.gcda" -delete
	find output -name "*.gcno" -delete
	rm -f output/coverage.info
	rm -rf output/coverage_html
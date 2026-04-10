EXTRACTION_MODE ?= 1
EXTRACTION_TYPE ?= 0
THROW_EXCEPTION ?= 1
KMER_SIZE ?=2

CXX      = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -Isrc -Itests \
           -DKMER_SIZE=$(KMER_SIZE) \
           -DEXTRACTION_MODE=$(EXTRACTION_MODE) \
           -DEXTRACTION_TYPE=$(EXTRACTION_TYPE) \
		   -DTHROW_EXCEPTION=$(THROW_EXCEPTION)

CATCH    = -lCatch2Main -lCatch2
COVERAGE = --coverage -fprofile-abs-path

BENCHMARK_FLAGS = -L/usr/lib/x86_64-linux-gnu -lbenchmark -lbenchmark_main -lpthread

BENCHMARK_RESULT_DIR ?= $(shell date +%Y-%m-%d_%H-%M)


BENCHMARK_OUTPUT = benchmarks/results/$(BENCHMARK_RESULT_DIR)/mode$(EXTRACTION_MODE)_type$(EXTRACTION_TYPE)_k$(KMER_SIZE)
MEMORY_OUTPUT = benchmarks/results/$(BENCHMARK_RESULT_DIR)/memory_mode$(EXTRACTION_MODE)_type$(EXTRACTION_TYPE)_k$(KMER_SIZE)

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


.PHONY: all clean index bits benchmark tests run_tests report report_open

all: index bits benchmark

# --- Executables ---

index: .create_dirs $(COMMON_OBJS) output/mains/main_index.o
	$(CXX) $(CXXFLAGS) $(COMMON_OBJS) output/mains/main_index.o -o output/index

bits: .create_dirs $(COMMON_OBJS) output/mains/main_bits.o
	$(CXX) $(CXXFLAGS) $(COMMON_OBJS) output/mains/main_bits.o -o output/bits

benchmark: .create_dirs $(COMMON_OBJS) output/mains/main_benchmark.o
	$(CXX) $(CXXFLAGS) $(COMMON_OBJS) output/mains/main_benchmark.o $(BENCHMARK_FLAGS) -o output/benchmark

memory: .create_dirs $(COMMON_OBJS) output/mains/main_memory.o
	$(CXX) $(CXXFLAGS) $(COMMON_OBJS) output/mains/main_memory.o -o output/memory


# --- Tests ---

tests: clean .create_dirs $(COMMON_OBJS) $(TEST_OBJS)
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

run_benchmark: CXXFLAGS += -O2 -DNDEBUG
run_benchmark: benchmark
	mkdir -p benchmarks/results/$(BENCHMARK_RESULT_DIR)
	./output/benchmark \
	    --benchmark_format=json \
	    --benchmark_out=$(BENCHMARK_OUTPUT).json
	./output/benchmark \
	    --benchmark_format=console

run_tests:
	./output/tests/runner

tests_debug: CXXFLAGS += -g -O0
tests_debug: clean tests

tests_strict:
	$(MAKE) tests EXTRACTION_MODE=0

tests_tolerant:
	$(MAKE) tests EXTRACTION_MODE=1

tests_all: tests_strict tests_tolerant

coverage: CXXFLAGS += $(COVERAGE)
coverage: clean tests
	./output/tests/runner
	lcov --capture --directory output --output-file output/coverage.info --ignore-errors mismatch
	lcov --extract output/coverage.info "$(PWD)/src/*" --output-file output/coverage.info --ignore-errors mismatch
	genhtml output/coverage.info --output-directory output/coverage_html
	@echo "Coverage report: output/coverage_html/index.html"
	xdg-open output/coverage_html/index.html

clean_coverage:
	find output -name "*.gcda" -delete
	find output -name "*.gcno" -delete
	rm -f output/coverage.info
	rm -rf output/coverage_html

report:
	python3 benchmarks/scripts/benchmark_report.py benchmarks/results/$(BENCHMARK_RESULT_DIR)

report_open: report
	xdg-open benchmarks/results/$(BENCHMARK_RESULT_DIR)/report.html


run_memory: CXXFLAGS += -O2 -DNDEBUG
run_memory: memory
	mkdir -p benchmarks/results/$(BENCHMARK_RESULT_DIR)
	valgrind --tool=massif --pages-as-heap=yes \
	    --massif-out-file=$(MEMORY_OUTPUT).massif \
	    ./output/memory
	ms_print $(MEMORY_OUTPUT).massif > $(MEMORY_OUTPUT).txt
PRJ_ROOT := $(shell pwd)
INCLUDEDIR := $(PRJ_ROOT)/include
OBJECTDIR := obj
SOURCEDIR := src
SOURCES := $(shell find $(SOURCEDIR) -name '*.c')
OBJECTS = $(addprefix $(OBJECTDIR)/,$(notdir $(SOURCES:%.c=%.o)))

BUILD_ARGS := -std=c++11 
LINK_ARGS := -lgtest_main -lgtest -lpthread


unittest: test_bin/hash_map_test
	./test_bin/hash_map_test

test_bin/%: $(OBJECTS) obj_test/%.o
	g++ $(BUILD_ARGS) $(LINK_ARGS) $? -o $@

obj_test/%.o: test/%.cpp 
	g++ $(BUILD_ARGS) -I$(INCLUDEDIR) $? -c -o $@

obj/%.o: src/%.c
	gcc -I$(INCLUDEDIR) $? -c -o $@

run_shared: src/shared_mem_starter.cpp
	g++ -Wall -Ilib/phf -std=c++11 $? -o run_shared -lphf -L/home/dariuslee/shm_mem_spike/c_src/lib/phf -lrt -lpthread

run: run_shared
	export LD_LIBRARY_PATH=/home/dariuslee/shm_mem_spike/c_src/lib/phf:/usr/lib/
	./run_shared

shared: src/file_mem_utils.cpp
	g++ -Ilib/phf -fpic -std=c++11 -c $? -o obj/file_mem_utils.o -lphf -L/home/dariuslee/shm_mem_spike/c_src/lib/phf -lrt -lpthread

	g++ -shared -o file_mem.so obj/file_mem_utils.o

cereal: src/cereal_spike.cpp
	g++ -Ilib/cereal-1.2.2/include -std=c++11 $? -o cereal_spike 

.PHONY: all
all: ; $(info $$BUILD_ARGS is [${BUILD_ARGS}])

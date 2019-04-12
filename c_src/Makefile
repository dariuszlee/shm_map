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

.PHONY: all
all: ; $(info $$BUILD_ARGS is [${BUILD_ARGS}])

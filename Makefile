unittest: test_bin/hash_map_test
	./test_bin/hash_map_test

test_bin/%: test/%.cpp
	g++ -std=c++11 -Wall -lgtest_main -lgtest hash_map_test.cpp -lpthread -o hash_map_test



CXX=g++
CXXFLAGS=-std=c++11 -O3

all:
	make clean
	$(CXX) $(CXXFLAGS) -o main_old -I include fptree.cpp main.cpp
	./main

clean:
	rm -rf main

run_second:
	$(CXX) $(CXXFLAGS) main.cpp FP_Tree_handler.cpp -o run 
	./run

run_all:
	bash compile.sh
	bash interface.sh C A1_datasets/D_medium.dat output.dat

compare:
	diff -u A1_datasets/D_small.dat decompressed.dat > differences.txt

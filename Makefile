
CXX = g++
CXXFLAGS = -std=c++11 -O3 -pthread

Lab4 : main.cpp src/abacus.o
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ $^ 

src/abacus.o : src/abacus.cpp header/abacus.hpp


clean:
	rm -f Lab4 src/abacus.o

.phony : clean

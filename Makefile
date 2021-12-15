
CXX = g++
CXXFLAG = -std=c++11

Lab4 : main.cpp src/abacus.o
	$(CXX) $(CXXFLAG) -o $@ $^ 

src/abacus.o : src/abacus.cpp header/abacus.hpp


clean:
	rm -f Lab4 src/abacus.o

.phony : clean
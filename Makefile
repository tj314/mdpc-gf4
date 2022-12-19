CXX_STANDARD := -std=c++17
CXX_FLAGS := -Wall -Wextra -pedantic

all:
	${CXX} ${CXX_STANDARD} ${CXX_FLAGS} main.cpp -o main

debug:
	${CXX} ${CXX_STANDARD} ${CXX_FLAGS} -g main.cpp -o main
	gdb -tui main

clean:
	rm main

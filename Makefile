CXX_STANDARD := -std=c++17
CXX_FLAGS := -Wall -Wextra -Wpedantic -Wfatal-errors

all:
	${CXX} ${CXX_STANDARD} ${CXX_FLAGS} main.cpp -o main

compile-debug:
	${CXX} ${CXX_STANDARD} ${CXX_FLAGS} -g -D NOPRINT main.cpp -o main

debug: compile-debug
	gdb -tui main

debug-rr: compile-debug
	rr record -o ./main
	rr replay

release:
	${CXX} ${CXX_STANDARD} ${CXX_FLAGS} -D NOPRINT main.cpp -o main

clean:
	rm main

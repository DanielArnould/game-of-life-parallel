main: main.cpp
	g++ -fopenmp -Wl,--stack,8388608 -Isrc/include -Lsrc/lib -o main main.cpp -lmingw32 -lSDL2main -lSDL2
	./main.exe
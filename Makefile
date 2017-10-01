all: a.out

a.out: main.cpp utf8.h
	g++ -std=c++17 main.cpp

clean:
	rm a.out

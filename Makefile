
all: src/*.cpp include/*.hpp
	clang++ -o main src/*.cpp -lraylib -Iinclude -Wno-c99-designator

run:
	./main

clean:
	-rm main
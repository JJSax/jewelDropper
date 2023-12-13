
all:
	g++ -o main main.cpp -lraylib

run:
	./main

clean:
	rm main
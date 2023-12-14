
all:
	g++ -o main src/main.cpp src/jeweldrop.cpp -lraylib -Iinclude

run:
	./main

clean:
	rm main
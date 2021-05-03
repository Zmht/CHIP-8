CC=clang++
CFLAGS=-std=c++17
PROJECT=chip8
OUT=bin/$(PROJECT)

chip8: chip8.cpp
	$(CC) -o $(OUT) chip8.cpp $(CFLAGS)

clean:
	rm -f $(OUT)
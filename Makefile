CC=clang++
CFLAGS=-std=c++17
WFLAGS=-Wall -Werror
DFLAGS=-g
PROJECT=chip8
OUT=bin

chip8: chip8.cpp
	$(CC) -o $(OUT)/$(PROJECT) chip8.cpp $(CFLAGS) $(WFLAGS)

debug: chip8.cpp
	$(CC) -o $(OUT)/$(PROJECT) chip8.cpp $(CFLAGS) $(DFLAGS) $(WFLAGS)



clean:
	rm -f $(OUT)/*
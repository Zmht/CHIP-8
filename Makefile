CC=clang++
CFLAGS=-std=c++17
WFLAGS=-Wall 
DFLAGS=-g
PROJECT=chip8
OUT=bin

default: debug


optimize: chip8.cpp
	$(CC) -o $(OUT)/$(PROJECT) chip8.cpp $(CFLAGS) --optimize

debug: chip8.cpp
	$(CC) -o $(OUT)/$(PROJECT) chip8.cpp $(CFLAGS) $(DFLAGS) $(WFLAGS)



clean:
	rm -fr $(OUT)/*
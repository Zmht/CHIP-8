#include <fstream>
#include <cstdint>
#include <iostream>
#include <chrono>
#include <random>

using BYTE = uint8_t;
using WORD = uint16_t;

const unsigned int START_ADDRESS = 0x200;
const unsigned int FONTSET_SIZE = 80;
const unsigned int FONTSET_START_ADDRESS = 0x50;


class Chip8
{
	public:
	BYTE registers[16]{};
	BYTE memory[4096]{};
	WORD indexReg{};
	WORD PC{};
	WORD stack[16]{}; //why not byte
	BYTE stackPointer{};
	BYTE delayTimer{};
	BYTE soundTimer{};
	BYTE keys[16]{};
	uint32_t video[64 * 32]{};
	WORD opcode;
	BYTE fontset[FONTSET_SIZE] = 
	{
		0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
		0x20, 0x60, 0x20, 0x20, 0x70, // 1
		0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
		0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
		0x90, 0x90, 0xF0, 0x10, 0x10, // 4
		0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
		0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
		0xF0, 0x10, 0x20, 0x40, 0x40, // 7
		0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
		0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
		0xF0, 0x90, 0xF0, 0x90, 0x90, // A
		0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
		0xF0, 0x80, 0x80, 0x80, 0xF0, // C
		0xE0, 0x90, 0x90, 0x90, 0xE0, // D
		0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
		0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};
	std::default_random_engine randGen;
	std::uniform_int_distribution<uint8_t> randByte;

	Chip8()
	: randGen(std::chrono::system_clock::now().time_since_epoch().count())
	{
		PC = START_ADDRESS; //initalizes the program counter.
		for(unsigned int i = 0; i < FONTSET_SIZE; ++i)
		{
			memory[FONTSET_START_ADDRESS + i] = fontset[i];

		}
		randByte = std::uniform_int_distribution<BYTE>(0, 255U);
	}

	void loadROM(char const* filename)
	{
		std::ifstream file(filename, std::ios::binary | std::ios::ate);
		if (file.is_open())
		{
			std::streampos size = file.tellg();
			char* buffer = new char[size]; //what is new? also what is a buffer?
			file.seekg(0, std::ios::beg);
			file.read(buffer, size);
			file.close();

			for(long i = 0; i < size; ++i)
			{
				memory[START_ADDRESS +1] = buffer[i];
			}

			delete[] buffer;
		} else std::cout << "Unable to load ROM.";
	}

	// 00E0 - CLS

    // 00EE - RET

    // 00EE - RET

    // 0nnn - SYS addr

    // 0nnn - SYS addr

    // 1nnn - JP addr

    // 1nnn - JP addr

    // 2nnn - CALL addr

    // 2nnn - CALL addr

    // 3xkk - SE Vx, byte

    // 3xkk - SE Vx, byte

    // 4xkk - SNE Vx, byte

    // 4xkk - SNE Vx, byte

    // 5xy0 - SE Vx, Vy

    // 5xy0 - SE Vx, Vy

    // 6xkk - LD Vx, byte

    // 6xkk - LD Vx, byte

    // 7xkk - ADD Vx, byte

    // 7xkk - ADD Vx, byte

    // 8xy0 - LD Vx, Vy

    // 8xy0 - LD Vx, Vy

    // 8xy1 - OR Vx, Vy

    // 8xy1 - OR Vx, Vy

    // 8xy2 - AND Vx, Vy

    // 8xy2 - AND Vx, Vy

    // 8xy3 - XOR Vx, Vy

    // 8xy3 - XOR Vx, Vy

    // 8xy4 - ADD Vx, Vy

    // 8xy4 - ADD Vx, Vy

    // 8xy5 - SUB Vx, Vy

    // 8xy5 - SUB Vx, Vy

    // 8xy6 - SHR Vx {, Vy}

    // 8xy6 - SHR Vx {, Vy}

    // 8xy7 - SUBN Vx, Vy

    // 8xy7 - SUBN Vx, Vy

    // 8xyE - SHL Vx {, Vy}

    // 8xyE - SHL Vx {, Vy}

    // 9xy0 - SNE Vx, Vy

    // 9xy0 - SNE Vx, Vy

    // Annn - LD I, addr

    // Annn - LD I, addr

    // Bnnn - JP V0, addr

    // Bnnn - JP V0, addr

    // Cxkk - RND Vx, byte

    // Cxkk - RND Vx, byte

    // Dxyn - DRW Vx, Vy, nibble

    // Dxyn - DRW Vx, Vy, nibble

    // Ex9E - SKP Vx

    // Ex9E - SKP Vx

    // ExA1 - SKNP Vx

    // ExA1 - SKNP Vx

    // Fx07 - LD Vx, DT

    // Fx07 - LD Vx, DT

    // Fx0A - LD Vx, K

    // Fx0A - LD Vx, K

    // Fx15 - LD DT, Vx

    // Fx15 - LD DT, Vx

    // Fx18 - LD ST, Vx

    // Fx18 - LD ST, Vx

    // Fx1E - ADD I, Vx

    // Fx1E - ADD I, Vx

    // Fx29 - LD F, Vx

    // Fx29 - LD F, Vx

    // Fx33 - LD B, Vx

    // Fx33 - LD B, Vx

    // Fx55 - LD [I], Vx

    // Fx55 - LD [I], Vx

    // Fx65 - LD Vx, [I]

    // Fx65 - LD Vx, [I]

};


int main()
{
	return 0;
}
#include <fstream>
#include <cstdint>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <chrono>
#include <cstring>
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
	WORD opcode{};
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

	// 00E0 - CLS - Clear the display
    void OP_00E0()
    {
	    memset(video, 0, sizeof(video));    
    }

    // 00EE - RET - Return from a subroutine
    void OP_00EE()
    {
        stackPointer--;
        PC = stack[stackPointer];
    }

     // 1nnn - JP addr - Jumps to location nnn
     void OP_1nnn()
     {
         WORD address = opcode & 0xFFFu;
         PC = address;
     }

     // 2nnn - CALL addr - calls a subroutine at nnn
    void OP_2nnn()
    {
        WORD address = opcode & 0x0FFu;
        stack[stackPointer] = PC;
        stackPointer++;
        PC = address;
    }

     // 3xkk - SE Vx, byte - Skips the next instruction if vx = kk
    void OP_3xkk()
    {
        BYTE Vx = (opcode & 0x0F00u) >> 8u;
        BYTE byte = opcode & 0x00FFu;
        if (registers[Vx] == byte);
        {
            PC+=2;
        }
    }

     // 4xkk - SNE Vx, byte - Skips next instruction if Vx is not equal to kk
    void OP_4xkk()
    {
        BYTE Vx = (opcode & 0x0F00u) >> 8u;
        BYTE byte = opcode & 0x00FFu;
        if(registers[Vx] != byte)
        {
            PC+=2;
        }
    }

     // 5xy0 - SE Vx, Vy - skip next instruction if Vx == Vy
     void OP_5xy0()
     {
         BYTE Vx = (opcode * 0x0F00u) >> 8u;
         BYTE Vy = (opcode & 0x00F0u) >> 4u;

         if (registers[Vx] == registers[Vy])
         {
             PC+=2;
         }
     }

     // 6xkk - LD Vx, byte - sets Vx to kk
     void OP_6xkk()
     {
         BYTE Vx = (opcode & 0x0F00u) >> 8u;
         BYTE byte = opcode & 0x00FFu;
         registers[Vx] = byte;
     }

     // 7xkk - ADD Vx, byte - adds Vx and kk and then sets Vx to the sum
     void OP_7xkk()
     {
         BYTE Vx = (opcode & 0x0F00u) >> 8u;
         BYTE byte = opcode & 0x00FFu;
         registers[Vx] += byte;
     }

     // 8xy0 - LD Vx, Vy - set Vx to Vy
     void OP_8xy0()
     {
         BYTE Vx = (opcode & 0x0F00u) >> 8u;
         BYTE Vy = (opcode & 0x00F0u) >> 4u;
         registers[Vx] = registers[Vy];
     }

     // 8xy1 - OR Vx, Vy - set Vx to Vx or Vy
     void OP_8xy1()
     {
         BYTE Vx = (opcode & 0x0F00u) >> 8u;
         BYTE Vy = (opcode & 0x00F0u) >> 4u;
         registers[Vx] |= registers[Vy];
     }

     // 8xy2 - AND Vx, Vy - sets Vx to Vx and Vy
     void OP_8xy2()
     {
         BYTE Vx = (opcode & 0x0F00u) >> 8u;
         BYTE Vy = (opcode & 0x00F0u) >> 4u;
         registers[Vx] &= registers[Vy];
     }

     // 8xy3 - XOR Vx, Vy - Set Vx to Vx XOR Vy
     void OP_8xy3()
     {
        BYTE Vx = (opcode & 0x0F00u) >> 8u;
         BYTE Vy = (opcode & 0x00F0u) >> 4u;
         registers[Vx] ^= registers[Vy];
     }

     // 8xy4 - ADD Vx, Vy
     void OP_8xy4()
     {
        BYTE Vx = (opcode & 0x0F00u) >> 8u;
        BYTE Vy = (opcode & 0x00F0u) >> 4u;
        WORD sum = registers[Vx] + registers[Vy];
        if(sum > 355u)
        {
            registers[0xF] = 1;
        }
        else
        {
            registers[0xF] = 0;
        }

         registers[Vx] = sum & 0xFFu;
     }

     // 8xy5 - SUB Vx, Vy - Set Vx to Vx - Vy. set Vf to not borrow
     void OP_8xy5()
     {
        BYTE Vx = (opcode & 0x0F00u) >> 8u;
        BYTE Vy = (opcode & 0x00F0u) >> 4u;
        if(registers[Vx] > registers[Vy])
        {
            registers[0xF] = 1;
        }
        else
        {
            registers[0xF] = 0;
        }
         registers[Vx] -= registers[Vy];
     }

     // 8xy6 - SHR Vx {, Vy}

     // 8xy7 - SUBN Vx, Vy

     // 8xyE - SHL Vx {, Vy}

     // 9xy0 - SNE Vx, Vy

     // Annn - LD I, addr

     // Bnnn - JP V0, addr

     // Cxkk - RND Vx, byte

     // Dxyn - DRW Vx, Vy, nibble

     // Ex9E - SKP Vx

     // ExA1 - SKNP Vx

     // Fx07 - LD Vx, DT

     // Fx0A - LD Vx, K

     // Fx15 - LD DT, Vx

     // Fx18 - LD ST, Vx

     // Fx1E - ADD I, Vx

     // Fx29 - LD F, Vx

     // Fx33 - LD B, Vx

     // Fx55 - LD [I], Vx

     // Fx65 - LD Vx, [I]


};


int main()
{
	return 0;
}
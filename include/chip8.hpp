#include <array>
#include <cstdint>
#include <stack>
#include <string>
#include <fstream>
#include <vector>
#include <random>
#include <chrono>
#include <algorithm>

#include <iostream>
#include <bitset>

const unsigned int START_ADDRESS = 0x200;
const unsigned int VIDEO_HEIGHT = 32;
const unsigned int VIDEO_WIDTH = 64;

class Chip8 {
public:
    Chip8();

    void LoadROM(std::string filename);
    void Cycle();
private:
    std::array<uint8_t, 16> V;
    std::array<uint8_t, 0x1000> memory;
    uint16_t IR;
    uint16_t PC;
    std::array<uint8_t, 16> stack;
    uint8_t SP;
    uint8_t DT;
    uint8_t ST;
    std::array<std::array<uint8_t, VIDEO_WIDTH>, VIDEO_HEIGHT> DM;
    uint16_t opcode;

    std::default_random_engine rand_eng;
    std::uniform_int_distribution<uint8_t> random_byte;

    void OP_00E0();
    void OP_00EE();
    void OP_1NNN();
    void OP_6XNN();
    void OP_7XNN();
    void OP_ANNN();
    void OP_DXYN();

};
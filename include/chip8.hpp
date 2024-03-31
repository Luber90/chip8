#include <array>
#include <cstdint>
#include <stack>
#include <string>
#include <fstream>
#include <vector>
#include <random>
#include <chrono>
#include <algorithm>
#include <functional>

#include <iostream>
#include <bitset>

const unsigned int START_ADDRESS = 0x200;
const unsigned int VIDEO_HEIGHT = 32;
const unsigned int VIDEO_WIDTH = 64;

class Chip8 {
public:

    std::array<std::array<uint32_t, VIDEO_WIDTH>, VIDEO_HEIGHT> DM;

    Chip8();

    void LoadROM(std::string filename);
    void Cycle(const std::array<uint8_t, 16>& keys);
private:
    std::array<uint8_t, 16> V;
    std::array<uint8_t, 16> keys;
    std::array<uint8_t, 0x1000> memory;
    uint16_t IR;
    uint16_t PC;
    std::array<uint16_t, 16> stack;
    uint8_t SP;
    uint8_t DT;
    uint8_t ST;
    uint16_t opcode;

    std::default_random_engine rand_eng;
    std::uniform_int_distribution<uint8_t> random_byte;

    std::chrono::time_point<std::chrono::system_clock> timers_point;

    typedef std::function<void(void)> Chip8F;
    std::array<Chip8F, 16> table;
    std::array<Chip8F, 16> table0;
    std::array<Chip8F, 16> table8;
    std::array<Chip8F, 16> tableE;
    std::array<Chip8F, 102> tableF;

    void Table0();
    void Table8();
    void TableE();
    void TableF();

    void ShowStack();

    void OP_NULL();
    void OP_00E0();
    void OP_00EE();
    void OP_0NNN();
    void OP_1NNN();
    void OP_2NNN();
    void OP_3XKK();
    void OP_4XKK();
    void OP_5XY0();
    void OP_6XKK();
    void OP_7XKK();
    void OP_8XY0();
    void OP_8XY1();
    void OP_8XY2();
    void OP_8XY3();
    void OP_8XY4();
    void OP_8XY5();
    void OP_8XY6();
    void OP_8XY7();
    void OP_8XYE();
    void OP_9XY0();
    void OP_ANNN();
    void OP_BNNN();
    void OP_CXKK();
    void OP_DXYN();
    void OP_EX9E();
    void OP_EXA1();
    void OP_FX07();
    void OP_FX0A();
    void OP_FX15();
    void OP_FX18();
    void OP_FX1E();
    void OP_FX29();
    void OP_FX33();
    void OP_FX55();
    void OP_FX65();

};
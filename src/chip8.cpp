#include "chip8.hpp"

Chip8::Chip8() : V({}), memory({}), stack({}), DM({}),
                 PC(START_ADDRESS), IR(0), SP(0), rand_eng(std::chrono::system_clock::now().time_since_epoch().count()),
                 DT(0), ST(0) { 
    std::array<uint8_t, 80> fontset =
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

    for(int i = 0; i < 80; i++) {
        memory[0x50 + i] = fontset[i];
    }

    random_byte = std::uniform_int_distribution<uint8_t>(0, 255U);

    timers_point = std::chrono::system_clock::now();

    table[0x0] = std::bind(&Chip8::Table0, this);
    table[0x1] = std::bind(&Chip8::OP_1NNN, this);
    table[0x2] = std::bind(&Chip8::OP_2NNN, this);
    table[0x3] = std::bind(&Chip8::OP_3XKK, this);
    table[0x4] = std::bind(&Chip8::OP_4XKK, this);
    table[0x5] = std::bind(&Chip8::OP_5XY0, this);
    table[0x6] = std::bind(&Chip8::OP_6XKK, this);
    table[0x7] = std::bind(&Chip8::OP_7XKK, this);
    table[0x8] = std::bind(&Chip8::Table8, this);
    table[0x9] = std::bind(&Chip8::OP_9XY0, this);
    table[0xA] = std::bind(&Chip8::OP_ANNN, this);
    table[0xB] = std::bind(&Chip8::OP_BNNN, this);
    table[0xC] = std::bind(&Chip8::OP_CXKK, this);
    table[0xD] = std::bind(&Chip8::OP_DXYN, this);
    table[0xE] = std::bind(&Chip8::TableE, this);
    table[0xF] = std::bind(&Chip8::TableF, this);

    for(size_t i = 0; i < 16; i++) {
        table0[i] = std::bind(&Chip8::OP_NULL, this);
        table8[i] = std::bind(&Chip8::OP_NULL, this);
        tableE[i] = std::bind(&Chip8::OP_NULL, this);
    }

    table0[0x0] = std::bind(&Chip8::OP_00E0, this);
    table0[0xE] = std::bind(&Chip8::OP_00EE, this);

    table8[0x0] = std::bind(&Chip8::OP_8XY0, this);
    table8[0x1] = std::bind(&Chip8::OP_8XY1, this);
    table8[0x2] = std::bind(&Chip8::OP_8XY2, this);
    table8[0x3] = std::bind(&Chip8::OP_8XY3, this);
    table8[0x4] = std::bind(&Chip8::OP_8XY4, this);
    table8[0x5] = std::bind(&Chip8::OP_8XY5, this);
    table8[0x6] = std::bind(&Chip8::OP_8XY6, this);
    table8[0x7] = std::bind(&Chip8::OP_8XY7, this);
    table8[0xE] = std::bind(&Chip8::OP_8XYE, this);

    tableE[0x1] = std::bind(&Chip8::OP_EXA1, this);
    tableE[0xE] = std::bind(&Chip8::OP_EX9E, this);

    for(size_t i = 0; i < tableF.size(); i++) {
        tableF[i] = std::bind(&Chip8::OP_NULL, this);
    }

    tableF[0x07] = std::bind(&Chip8::OP_FX07, this);
    tableF[0x0A] = std::bind(&Chip8::OP_FX0A, this);
    tableF[0x15] = std::bind(&Chip8::OP_FX15, this);
    tableF[0x18] = std::bind(&Chip8::OP_FX18, this);
    tableF[0x1E] = std::bind(&Chip8::OP_FX1E, this);
    tableF[0x29] = std::bind(&Chip8::OP_FX29, this);
    tableF[0x33] = std::bind(&Chip8::OP_FX33, this);
    tableF[0x55] = std::bind(&Chip8::OP_FX55, this);
    tableF[0x65] = std::bind(&Chip8::OP_FX65, this);
}

void Chip8::LoadROM(std::string filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);

    if(file.is_open()) {
        std::cout << "opened a file" << std::endl;
        unsigned int file_size = file.tellg();
        std::vector<char> buffer(file_size);

        file.seekg(0, std::ios::beg);
        file.read(buffer.data(), file_size);
        file.close();

        for(unsigned int i = 0; i < file_size; i++) {
            memory[START_ADDRESS + i] = buffer[i];
        }
    }
}

void Chip8::Cycle(const std::array<uint8_t, 16>& keys) {
    this->keys = keys;
    using namespace std::literals::chrono_literals;

    opcode = (memory[PC] << 8u) | (memory[PC+1]);

    //std::bitset<16> a(opcode);
    //std::cout << std::hex << opcode << std::endl;
    //std::cout <<"line: " << PC << std::endl;

    PC += 2;

    table[(opcode & 0xF000u) >> 12u]();

    if(std::chrono::system_clock::now() - timers_point > 16667us) {
        if(DT > 0) {
            DT--;
        }

        if(ST > 0) {
            ST--;
        }
    }
}

void Chip8::ShowStack() {
    std::cout << +SP << std::endl;
    for(size_t i = 0; i < stack.size(); i++) {
        std::cout << " " << std::hex << stack[i];
    }
    std::cout << std::endl;
}

void Chip8::OP_00E0() {
    for(auto& line : DM) {
        std::fill(line.begin(), line.end(), 0u);
    }
}

void Chip8::OP_00EE() {
    //ShowStack();
    SP--;
    //std::cout << "RETURN TO " << stack[SP] << std::endl;
    PC = stack[SP];
}

void Chip8::OP_0NNN() {

}

void Chip8::OP_1NNN() {
    uint16_t address = opcode & 0x0FFFu;
    PC = address;
}

void Chip8::OP_2NNN() {
    //ShowStack();
    uint16_t address = opcode & 0x0FFFu;

    //std::cout << "JUMP FROM " << PC << " TO " << address << " SP " << +SP << std::endl;
    stack[SP] = PC;
    //std::cout << "SANITY CHECK " << stack[SP] << std::endl;
    SP++;
    PC = address;
}

void Chip8::OP_3XKK() {
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    uint8_t value = opcode & 0x00FFu;

    if(V[x] == value) PC += 2;
}

void Chip8::OP_4XKK() {
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    uint8_t value = opcode & 0x00FFu;

    if(V[x] != value) PC += 2;
}

void Chip8::OP_5XY0() {
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    uint8_t y = (opcode & 0x00F0u) >> 4u;

    if(V[x] ==  V[y]) PC += 2;
}

void Chip8::OP_6XKK() {
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    uint8_t value = opcode & 0x00FFu;

    V[x] = value;
}

void Chip8::OP_7XKK() {
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    uint8_t value = opcode & 0x00FFu;
    V[x] += value;
    // uint16_t sum = V[x] + value;
    // if(sum <= 255u) {
    //     V[x] = sum;
    // } else {
    //     V[x] = sum & 0xFFFFu;
    // }
}

void Chip8::OP_8XY0() {
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    uint8_t y = (opcode & 0x00F0u) >> 4u;

    V[x] = V[y];
}

void Chip8::OP_8XY1() {
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    uint8_t y = (opcode & 0x00F0u) >> 4u;

    V[x] = V[x] | V[y];
}

void Chip8::OP_8XY2() {
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    uint8_t y = (opcode & 0x00F0u) >> 4u;

    V[x] = V[x] & V[y];
}

void Chip8::OP_8XY3() {
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    uint8_t y = (opcode & 0x00F0u) >> 4u;

    V[x] = V[x] ^ V[y];
}

void Chip8::OP_8XY4() {
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    uint8_t y = (opcode & 0x00F0u) >> 4u;
    uint16_t sum = V[x] + V[y];
    V[x] = sum & 0xFFFFu;
    V[0xFu] = !(sum <= 255u);
}

void Chip8::OP_8XY5() {
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    uint8_t y = (opcode & 0x00F0u) >> 4u;
    uint8_t result = V[x] - V[y];
    bool flag = V[x] >= V[y];
    V[x] = result;
    V[0xFu] = flag;
}

void Chip8::OP_8XY6() {
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    uint8_t y = (opcode & 0x00F0u) >> 4u;
    bool flag = (V[x]&0b1u) == 0b1u;

    V[x] = V[x]>>1;
    V[0xFu] = flag;
}

void Chip8::OP_8XY7() {
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    uint8_t y = (opcode & 0x00F0u) >> 4u;

    uint8_t result = V[y] - V[x];
    bool flag = V[y] >= V[x];

    V[x] = result;
    V[0xFu] = flag;
}

void Chip8::OP_8XYE() {
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    bool flag = (V[x]&0x80u) == 0x80u;
    V[x] = V[x]<<1;
    V[0xFu] = flag;
}

void Chip8::OP_9XY0() {
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    uint8_t y = (opcode & 0x00F0u) >> 4u;

    if(V[x] != V[y]) PC += 2;
}

void Chip8::OP_ANNN() {
    uint16_t address = opcode & 0x0FFFu;

    IR = address;
}

void Chip8::OP_BNNN() {
    uint16_t address = opcode & 0x0FFFu;

    IR = V[0] + address;
}

void Chip8::OP_CXKK() {
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;

    V[x] = random_byte(rand_eng) & byte;
}

void Chip8::OP_DXYN() {
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    uint8_t y = (opcode & 0x00F0u) >> 4u;
    uint8_t height = opcode & 0x000Fu;

    uint8_t x_pos = V[x] % VIDEO_WIDTH;
    uint8_t y_pos = V[y] % VIDEO_HEIGHT;

    V[0xF] = 0;

    for(unsigned int row =  0; row < height; row++) {
        uint8_t sprite_byte = memory[IR + row];

        for(unsigned int col = 0; col < 8; col++) {
            uint8_t sprite_pixel = sprite_byte & (0x80u >> col);
            uint32_t& screen_pixel = DM[(y_pos + row)][(x_pos + col)];

            if(sprite_pixel) {
                if(screen_pixel == 0xFFFFFFFF) {
                    V[0xF] = 1;
                }

                screen_pixel ^= 0xFFFFFFFF;
            }
        }
    }
}

void Chip8::OP_EX9E() {
    uint8_t x = (opcode & 0x0F00u) >> 8u;

    if(keys[V[x]]) PC += 2;
}

void Chip8::OP_EXA1() {
    uint8_t x = (opcode & 0x0F00u) >> 8u;

    if(!keys[V[x]]) PC += 2;
}

void Chip8::OP_FX07() {
    uint8_t x = (opcode & 0x0F00u) >> 8u;

    V[x] = DT;
}

void Chip8::OP_FX0A() {
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    
    bool pressed = false;
    for(uint8_t i = 0; i < 16; i++) {
        if(keys[i]) {
            V[x] = keys[i];
            pressed = true;
            break;
        }
    }
    if(!pressed) PC -= 2;
}

void Chip8::OP_FX15() {
    uint8_t x = (opcode & 0x0F00u) >> 8u;

    DT = V[x];
}

void Chip8::OP_FX18() {
    uint8_t x = (opcode & 0x0F00u) >> 8u;

    ST = V[x];
}

void Chip8::OP_FX1E() {
    uint8_t x = (opcode & 0x0F00u) >> 8u;

    IR += V[x];
}

void Chip8::OP_FX29() {
    uint8_t x = (opcode & 0x0F00u) >> 8u;

    IR = 0x50u + (5*V[x]);
}

void Chip8::OP_FX33() {
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    uint8_t value = V[x];

    memory[IR + 2] = value % 10u;
    value /= 10;
    memory[IR+1] = value % 10u;
    value /= 10;
    memory[IR] = value % 10u;
}

void Chip8::OP_FX55() {
    uint8_t x = (opcode & 0x0F00u) >> 8u;

    for(uint8_t i = 0; i <= x; i++) {
        memory[IR + i] = V[i];
    }
}

void Chip8::OP_FX65() {
    uint8_t x = (opcode & 0x0F00u) >> 8u;

    for(uint8_t i = 0; i <= x; i++) {
        V[i] = memory[IR + i];
    }
}

void Chip8::OP_NULL() {

}

void Chip8::Table0() {
    table0[opcode & 0x000Fu]();
}

void Chip8::Table8() {
    table8[opcode & 0x000Fu]();
}

void Chip8::TableE() {
    tableE[opcode & 0x000Fu]();
}

void Chip8::TableF() {
    tableF[opcode & 0x00FFu]();
}


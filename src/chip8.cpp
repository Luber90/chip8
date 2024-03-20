#include "../include/chip8.hpp"

Chip8::Chip8() : V({}), memory({}), stack({}), DM({}),
                 PC(START_ADDRESS), rand_eng(std::chrono::system_clock::now().time_since_epoch().count()){ 
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

void Chip8::Cycle() {
    opcode = (memory[PC] << 8u) | (memory[PC+1]);

    std::bitset<16> a(opcode);
    std::cout << a << std::endl;
    std::cout << PC << std::endl;

    PC += 2;

    if((opcode & 0xF000u) == 0x0u) {
        if(opcode  == 0x00E0) {
            OP_00E0();
        } else {
            OP_00EE();
        }
    } else if((opcode & 0xF000u) == 0x1000u) {
        OP_1NNN();
    } else if((opcode & 0xF000u) == 0x6000u) {
        OP_6XNN();
    } else if((opcode & 0xF000u) == 0x7000u) {
        OP_7XNN();
    } else if((opcode & 0xF000u) == 0xA000u) {
        OP_ANNN();
    } else if((opcode & 0xF000u) == 0xD000u) {
        OP_DXYN();
    } else {
        throw std::runtime_error("Unknown opcode");
    }

    if(DT > 0) {
        DT--;
    }

    if(ST > 0) {
        ST--;
    }
}

void Chip8::OP_00E0() {
    for(auto& line : DM) {
        std::fill(line.begin(), line.end(), 0u);
    }
}

void Chip8::OP_00EE() {
    SP--;
    PC = stack[SP];
}

void Chip8::OP_1NNN() {
    uint16_t address = opcode & 0x0FFFu;
    PC = address;
}

void Chip8::OP_6XNN() {
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    uint8_t value = opcode & 0x00FFu;

    V[x] = value;
}

void Chip8::OP_7XNN() {
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    uint8_t value = opcode & 0x00FFu;
    uint16_t sum = V[x] + value;
    if(sum <= 255u) {
        V[x] = sum;
    } else {
        V[x] = sum & 0xFFFFu;
    }
}

void Chip8::OP_ANNN() {
    uint16_t address = opcode & 0x0FFFu;

    IR = address;
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
            uint8_t& screen_pixel = DM[(y_pos + row)][(x_pos + col)];

            if(sprite_pixel) {
                if(screen_pixel) {
                    V[0xF] = 1;
                }

                screen_pixel ^= 0xFF;
            }
        }
    }

    for(size_t i = 0; i < DM.size(); i++) {
        for(size_t j = 0; j < DM[i].size(); j++) {
            std::cout << std::bitset<8>(DM[i][j]).any();
        }
        std::cout << std::endl;
    }
}
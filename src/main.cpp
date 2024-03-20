#include <iostream>
#include <thread>
#include "../include/chip8.hpp"

int main(int argc, char** argv) {
    Chip8 chip;

    if(argc < 2) {
        std::cout << "Specify the filename!" << std::endl;
        return -1;
    }

    chip.LoadROM(argv[1]);
    while(true) {
        chip.Cycle();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    
    return 0;
}
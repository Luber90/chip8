#include <iostream>
#include <SDL2/SDL.h>

#include "interpreter.hpp"

int main(int argc, char** argv) {
    if(argc < 3) {
        std::cout << "Specify the cycle duration and a filename!" << std::endl;
        return -1;
    }


    Interpreter inter(static_cast<double>(std::stoi(argv[1])));
    inter.chip.LoadROM(argv[2]);
    inter.loop();
    
    return 0;
}
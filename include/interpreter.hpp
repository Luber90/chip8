#include "chip8.hpp"
#include <SDL2/SDL.h>
#include <thread>

const unsigned int SCALE = 5;

class Interpreter {
private:
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Surface *window_surface;
    std::array<uint8_t, 16> keys;
    std::chrono::time_point<std::chrono::system_clock> time_point;
    const double cycle;

public:
    Chip8 chip;

    Interpreter(double _cycle);
    Interpreter(Interpreter &i) = delete;
    ~Interpreter();

    void draw();
    void draw2(void const* buffer, int pitch);
    void loop();
    void GetInputs(SDL_Event& event, bool& quit);
};
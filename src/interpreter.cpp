#include "interpreter.hpp"

Interpreter::Interpreter(double _cycle) : cycle(_cycle){
    bool quit = false, first_frame = true;
    window = nullptr;
    renderer = nullptr;
    window_surface = nullptr;

    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_CreateWindowAndRenderer(VIDEO_WIDTH*SCALE, VIDEO_HEIGHT*SCALE, 0, &window, &renderer);
    SDL_RenderSetScale(renderer, SCALE, SCALE);
    window_surface = SDL_GetWindowSurface(window);


    time_point = std::chrono::system_clock::now();
}

Interpreter::~Interpreter() {
    SDL_DestroyWindowSurface(window);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}

void Interpreter::draw() {
    SDL_LockSurface(window_surface);
    for(unsigned int x = 0; x < VIDEO_WIDTH; x++) {
        for(unsigned int y = 0; y < VIDEO_HEIGHT; y++) {
            for(int k = 0; k < SCALE; k++) {
                for(int l = 0; l < SCALE; l++){
                    static_cast<uint32_t*>(window_surface->pixels)[(y*SCALE+k)*SCALE*VIDEO_WIDTH+x*SCALE+l] = chip.DM[y][x] ? 0xFFFFFFu : 0u;
                }    
            }
            //static_cast<uint32_t*>(window_surface->pixels)[y*VIDEO_WIDTH+x] = chip.DM[y][x] ? 0xFFFFFFu : 0u;
        }
    }
    SDL_UnlockSurface(window_surface);
    SDL_UpdateWindowSurface(window);
}

void Interpreter::GetInputs(SDL_Event& event, bool& quit) {
    while(SDL_PollEvent(&event) != 0) {
            switch(event.type) {
                case SDL_QUIT:
                    quit = true;
                    break;
                case SDL_KEYDOWN:
                    switch(event.key.keysym.sym) {
                        case SDLK_x:
                            keys[0] = 1;
                            break;
                        case SDLK_1:
                            keys[1] = 1;
                            break;
                        case SDLK_2:
                            keys[2] = 1;
                            break;
                        case SDLK_3:
                            keys[3] = 1;
                            break;
                        case SDLK_q:
                            keys[4] = 1;
                            break;
                        case SDLK_w:
                            keys[5] = 1;
                            break;
                        case SDLK_e:
                            keys[6] = 1;
                            break;
                        case SDLK_a:
                            keys[7] = 1;
                            break;
                        case SDLK_s:
                            keys[8] = 1;
                            break;
                        case SDLK_d:
                            keys[9] = 1;
                            break;
                        case SDLK_z:
                            keys[10] = 1;
                            break;
                        case SDLK_c:
                            keys[11] = 1;
                            break;
                        case SDLK_4:
                            keys[12] = 1;
                            break;
                        case SDLK_r:
                            keys[13] = 1;
                            break;
                        case SDLK_f:
                            keys[14] = 1;
                            break;
                        case SDLK_v:
                            keys[15] = 1;
                            break;
                    }
                    break;
                case SDL_KEYUP:
                    switch(event.key.keysym.sym) {
                        case SDLK_x:
                            keys[0] = 0;
                            break;
                        case SDLK_1:
                            keys[1] = 0;
                            break;
                        case SDLK_2:
                            keys[2] = 0;
                            break;
                        case SDLK_3:
                            keys[3] = 0;
                            break;
                        case SDLK_q:
                            keys[4] = 0;
                            break;
                        case SDLK_w:
                            keys[5] = 0;
                            break;
                        case SDLK_e:
                            keys[6] = 0;
                            break;
                        case SDLK_a:
                            keys[7] = 0;
                            break;
                        case SDLK_s:
                            keys[8] = 0;
                            break;
                        case SDLK_d:
                            keys[9] = 0;
                            break;
                        case SDLK_z:
                            keys[10] = 0;
                            break;
                        case SDLK_c:
                            keys[11] = 0;
                            break;
                        case SDLK_4:
                            keys[12] = 0;
                            break;
                        case SDLK_r:
                            keys[13] = 0;
                            break;
                        case SDLK_f:
                            keys[14] = 0;
                            break;
                        case SDLK_v:
                            keys[15] = 0;
                            break;
                    }
                    break;
            }
        }
}

void Interpreter::loop() {
    bool quit = false;
    SDL_Event event;

    while(!quit) {
        //auto t1 = std::chrono::system_clock::now();
        GetInputs(event, quit);
        //std::cout << "polling time: " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - t1).count() << std::endl;

        auto current_time = std::chrono::system_clock::now();
        double dt = std::chrono::duration<double, std::chrono::milliseconds::period>(current_time - time_point).count();
        if(dt > cycle) {
            std::cout << "dt: " << dt << std::endl;
            time_point = current_time;
            chip.Cycle(keys);
            draw();
        }
    }
}

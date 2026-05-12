#include <SDL2/SDL.h>
#include "../display.h"

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
static int running = 1;

int display_init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return 0;
    window = SDL_CreateWindow("Radish OS Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                              WIDTH * SCALE, HEIGHT * SCALE, 0);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    }
    if (!renderer) return 0;
    return 1;
}

void display_update(Color* buffer) { // Mudou de matriz para ponteiro
    SDL_Event ev;
    while(SDL_PollEvent(&ev)) {
        if(ev.type == SDL_QUIT) running = 0;
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            // Acesso linear ao buffer
            Color c = buffer[y * WIDTH + x]; 

            // Conversão RGB565 -> RGB888
            uint8_t r = (c >> 11) << 3;
            uint8_t g = ((c >> 5) & 0x3F) << 2;
            uint8_t b = (c & 0x1F) << 3;

            SDL_SetRenderDrawColor(renderer, r, g, b, 255);
            SDL_Rect rect = { x * SCALE, y * SCALE, SCALE, SCALE };
            SDL_RenderFillRect(renderer, &rect);
        }
    }

    SDL_RenderPresent(renderer);
}

int display_is_running() { return running; }

void display_close() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

#include "display.h"
#include "gfx.h"
#include "object_manager.h"
#include <SDL2/SDL.h>

#define NUM_GAMES 6

int main(int argc, char* argv[]) {
    if(!display_init()) return 1;

    Color game_colors[NUM_GAMES] = {
        0xF800, 0x07E0, 0xFFE0, 0x8010, 0x07FF, 0xF81F
    };

    int running = 1;
    Objeto* games[NUM_GAMES];
    float spacing = 52.0f;
    float start_x = -((NUM_GAMES - 1) * spacing) / 2.0f;

    for (int i = 0; i < NUM_GAMES; i++) {
        games[i] = OM_create_object(start_x + i * spacing, 0, 0.5f, game_colors[i]);
    }

    int selected = 0;
    games[selected]->target_sx = 1.5f;
    games[selected]->target_sy = 1.5f;

    while (running) {
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT) running = 0;
            if (ev.type == SDL_KEYDOWN) {
                if (ev.key.keysym.sym == SDLK_LEFT || ev.key.keysym.sym == SDLK_a) {
                    games[selected]->target_sx = 1.0f;
                    games[selected]->target_sy = 1.0f;
                    selected = (selected - 1 + NUM_GAMES) % NUM_GAMES;
                    games[selected]->target_sx = 1.5f;
                    games[selected]->target_sy = 1.5f;
                }
                if (ev.key.keysym.sym == SDLK_RIGHT || ev.key.keysym.sym == SDLK_d) {
                    games[selected]->target_sx = 1.0f;
                    games[selected]->target_sy = 1.0f;
                    selected = (selected + 1) % NUM_GAMES;
                    games[selected]->target_sx = 1.5f;
                    games[selected]->target_sy = 1.5f;
                }
            }
        }
        if (!running) break;

        GFX_clear(0x0008);

        GFX_set_color(0x14A5);
        GFX_draw_rectangle(-160, -120, 160, -120, 160, -85, -160, -85, 1.0f);
        GFX_draw_rectangle(-160, 75, 160, 75, 160, 120, -160, 120, 1.0f);

        OM_update_all();
        OM_render_all();

        Objeto* sel = games[selected];
        float ind_w = 8.0f * sel->sx;
        float ind_y = sel->y + 15.0f * sel->sy;
        GFX_set_color(0xFFFF);
        GFX_draw_rectangle(
            sel->x - ind_w, ind_y,
            sel->x + ind_w, ind_y,
            sel->x + ind_w, ind_y + 5.0f,
            sel->x - ind_w, ind_y + 5.0f,
            0.2f
        );

        GFX_set_color(0xFFFF);
        GFX_begin(GFX_TRIANGLES);
            GFX_vertex(-145, -2, 0.2f, 0, 0);
            GFX_vertex(-135, -8, 0.2f, 0, 0);
            GFX_vertex(-135, 4, 0.2f, 0, 0);
        GFX_end();

        GFX_begin(GFX_TRIANGLES);
            GFX_vertex(145, -2, 0.2f, 0, 0);
            GFX_vertex(135, -8, 0.2f, 0, 0);
            GFX_vertex(135, 4, 0.2f, 0, 0);
        GFX_end();

        display_update(GFX_buffer);
        SDL_Delay(16);
    }

    display_close();
    return 0;
}

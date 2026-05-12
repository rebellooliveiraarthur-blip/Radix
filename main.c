#include "display.h"
#include "gfx.h"
#include "object_manager.h"
#include <SDL2/SDL.h>
#include <math.h>

#define ____ 0x0000
#define BRAN 0xFFFF
#define VERD 0x07E0
#define VERM 0xF800

Color pixels_teste[] = { 
    BRAN, ____, ____, BRAN, 
    ____, VERD, VERD, ____, 
    ____, VERD, VERD, ____, 
    BRAN, ____, ____, BRAN 
};
Sprite meuSprite = { 4, 4, pixels_teste };

int main(int argc, char* argv[]) {
    if(!display_init()) return 1;

    Objeto* green = OM_create_object(0, 20, 2.0f, VERD);
    float time = 0.0f;

    while(display_is_running()) {
        time += 0.05f;

        green->target_sx = 1.0f + 3.0f * (0.5f + 0.5f * sin(time * 2.0f));
        green->target_sy = green->target_sx;
        green->x = 50.0f * sin(time * 1.5f);

        GFX_clear(0x1082);

        OM_update_all();
        OM_render_all();

        GFX_set_color(VERM);
        GFX_begin(GFX_TRIANGLES);
            GFX_vertex(0, -50, 1.0f, 0, 0);
            GFX_vertex(60, 40, 1.0f, 0, 0);
            GFX_vertex(-60, 40, 1.0f, 0, 0);
        GFX_end();

        GFX_bind_sprite(&meuSprite);
        GFX_draw_rectangle(20, -40, 80, -40, 80, 20, 20, 20, 1.5f);
        GFX_bind_sprite(NULL);

        GFX_draw_sprite_scaled(&meuSprite, -100, -50, 0.5f, 64, 64);

        display_update(GFX_buffer);
        SDL_Delay(16);
    }

    display_close();
    return 0;
}

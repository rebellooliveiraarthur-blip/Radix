#include "display.h"
#include "gfx.h"
#include "object_manager.h"
#include <SDL2/SDL.h>
int main(int argc, char* argv[]) {
    if(!display_init()) return 1;
    OM_create_object(OBJ_RECTANGLE, 0, 0, 100, 60, 0x07E0, 1);
    Uint32 last = SDL_GetTicks();
    while (display_is_running()) {
        Uint32 now = SDL_GetTicks();
        float dt = (now - last) / 1000.0f;
        if (dt > 0.05f) dt = 0.05f;
        last = now;
        OM_update_all(dt);
        GFX_clear(0x0000);
        OM_render_all();
        display_update(GFX_buffer);
        SDL_Delay(16);
    }
    display_close();
    return 0;
}
#include "display.h"
#include "gfx.h"
#include "assets/background/background_sprite.h"
#include <SDL2/SDL.h>

typedef enum { 
    ESTADO_LOADING, 
    ESTADO_MENU,
    ESTADO_CONFIG, 
    ESTADO_JOGO 
} Estado;

int timerLoading = 0;
Estado estadoAtual = ESTADO_MENU;

#define ____ 0x0000
#define BRAN 0xFFFF
#define ROSA 0xF810
#define VERD 0x07E0
#define CAST 0x8200

// Um sprite 4x4 simples (ex: uma carinha ou padrão)
Color pixels_teste[] = {
    BRAN, ____, ____, BRAN,
    ____, VERD, VERD, ____,
    ____, VERD, VERD, ____,
    BRAN, ____, ____, BRAN
};

Sprite meuSprite = { 4, 4, pixels_teste };


/*
 * TODO: Vertex já definido em gfx.h — não redefinir aqui.
 * Quando o batch system estiver pronto:
 *
 *   GFX_begin(GFX_TRIANGLES);
 *       GFX_set_color(0xF800);
 *       GFX_vertex(50, 50, 0);
 *       GFX_vertex(90, 50, 0);
 *       GFX_vertex(50, 90, 0);
 *   GFX_end();
 *
 * Com matrix stack (Fase 2):
 *
 *   GFX_matrix_push();
 *       GFX_matrix_translate(160, 120, 0);
 *       GFX_matrix_rotate(time * 0.001f, 0, 0, 1);
 *       GFX_begin(GFX_TRIANGLES);
 *           GFX_set_color(0x07E0);
 *           GFX_vertex(-20, -20, 0);
 *           GFX_vertex( 20, -20, 0);
 *           GFX_vertex(  0,  20, 0);
 *       GFX_end();
 *   GFX_matrix_pop();
 *
 * Com depth buffer (Fase 6):
 *
 *   GFX_clear_color_and_depth(0x0000, 1.0f);
 *   GFX_enable(GFX_DEPTH_TEST);
 *   ... desenhar objetos em qualquer ordem ...
 */

int main(int argc, char* argv[]) {
    if(!display_init()) return 1;

    while(display_is_running()) {
        GFX_clear(____); // Limpa o buffer linear

        if (estadoAtual == ESTADO_MENU) {
            GFX_set_background_color(BRAN);
        }
        // O display_update agora recebe o buffer linear que você definiu
        display_update(GFX_buffer); 
    }

    display_close();
    return 0;
}

#include "display.h"
#include "gfx.h"
#include "object_manager.h"
#include <SDL2/SDL.h>

// Cores e Sprite de teste
#define VERD 0x07E0
#define VERM 0xF800
#define BRAN 0xFFFF
#define ____ 0x0000

Color pixels_teste[] = { 
    BRAN, ____, ____, BRAN, 
    ____, VERD, VERD, ____, 
    ____, VERD, VERD, ____, 
    BRAN, ____, ____, BRAN 
};
Sprite meuSprite = { 4, 4, pixels_teste };

int main(int argc, char* argv[]) {
    if(!display_init()) return 1;

    // --- CONFIGURAÇÃO INICIAL DOS OBJETOS ---
    
    // 1. Um quadrado vermelho fixo ao fundo (Z = 2.0)
    OM_create_object(-50, -50, 2.0f, VERM);

    // 2. Um objeto que usará imagem (Sprite) e vai "pulsar" (Z = 1.0)
    Objeto* objAnimado = OM_create_object(0, 0, 1.0f, 0x0000);
    objAnimado->sprite = &meuSprite;

    int frame_count = 0;

    while(display_is_running()) {
        // 1. Limpa tudo
        GFX_clear(0x1082); // Fundo azul

        // 2. Lógica de animação simples para o teste
        frame_count++;
        if (frame_count % 60 == 0) {
            // A cada 1 segundo, alterna o tamanho alvo
            if (objAnimado->target_sx == 1.0f) {
                objAnimado->target_sx = 5.0f; // Cresce 5x
                objAnimado->target_sy = 5.0f;
            } else {
                objAnimado->target_sx = 1.0f; // Volta ao normal
                objAnimado->target_sy = 1.0f;
            }
        }

        // 3. ATUALIZAÇÃO (Calcula o LERP das escalas)
        OM_update_all();

        // 4. RENDERIZAÇÃO (Desenha todos os objetos do pool)
        OM_render_all();

        // Atualiza o hardware/janela
        display_update(GFX_buffer);
        SDL_Delay(16);
    }

    display_close();
    return 0;
}

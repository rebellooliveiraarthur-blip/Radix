#include "object_manager.h"
#include "gfx.h"

#define LERP_SPEED 0.15f

static Objeto objetos[100];
static int num_objetos = 0;

Objeto* OM_create_object(float x, float y, float z, Color cor) {
    if (num_objetos >= 100) return NULL;

    Objeto* obj = &objetos[num_objetos++];
    obj->x = x; obj->y = y; obj->z = z;
    obj->sx = 1.0f; obj->sy = 1.0f;
    obj->target_sx = 1.0f; obj->target_sy = 1.0f;
    obj->cor = cor;
    obj->sprite = NULL;
    obj->ativo = true;
    return obj;
}

void OM_update_all() {
    for (int i = 0; i < num_objetos; i++) {
        if (!objetos[i].ativo) continue;

        // Suaviza a escala (O efeito que você queria)
        objetos[i].sx += (objetos[i].target_sx - objetos[i].sx) * LERP_SPEED;
        objetos[i].sy += (objetos[i].target_sy - objetos[i].sy) * LERP_SPEED;
    }
}

void OM_render_all() {
    for (int i = 0; i < num_objetos; i++) {
        Objeto* obj = &objetos[i];
        if (!obj->ativo) continue;

        if (obj->sprite) {
            // Desenha imagem
            int dw = (int)(obj->sprite->largura * obj->sx);
            int dh = (int)(obj->sprite->altura * obj->sy);
            GFX_draw_sprite_scaled(obj->sprite, (int)obj->x, (int)obj->y, obj->z, dw, dh);
        } else {
            // Desenha retângulo colorido usando os triângulos do motor
            GFX_set_color(obj->cor);
            
            // Define o tamanho base (ex: 20x20) e aplica a escala
            float halfW = 10.0f * obj->sx;
            float halfH = 10.0f * obj->sy;

            GFX_draw_rectangle(
                obj->x - halfW, obj->y - halfH, // Superior Esquerdo
                obj->x + halfW, obj->y - halfH, // Superior Direito
                obj->x + halfW, obj->y + halfH, // Inferior Direito
                obj->x - halfW, obj->y + halfH, // Inferior Esquerdo
                obj->z
            );
        }
    }
}

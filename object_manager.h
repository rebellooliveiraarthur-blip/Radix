#include"gfx.h"

typedef struct {
    Sprite* sprite;
    float x, y, z;
    float sx, sy;        // Escala atual
    float target_sx, target_sy;
    Color cor;           // Cor para formas simples
    bool ativo;
} Objeto;

// Função para "dar vida" a um objeto
Objeto* OM_create_object(float x, float y, float z, Color cor);
void OM_update_all();
void OM_render_all();

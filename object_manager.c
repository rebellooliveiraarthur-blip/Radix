#include "object_manager.h"
#include <stdlib.h>
/*

recebe o time pelo main

*/

static Objeto lista_objetos[MAX_OBJECTS] = {0}; // Inicializa a lista de objetos
int total_objetos = 0;
int total_dead_objects = 0;
static int dead_object_indices[MAX_OBJECTS] = {0}; // Armazena os índices dos objetos mortos

void OM_clear() {
    total_objetos = 0;
    for (int i = 0; i < MAX_OBJECTS; i++) {
        lista_objetos[i].ativo = false;
    }
}

void OM_list_empty() {
    total_dead_objects = 0;
    for (int i = 0; i < total_objetos; i++) {
        if (!lista_objetos[i].ativo) {
            dead_object_indices[total_dead_objects] = i;
            total_dead_objects += 1;
        }
    }
}

Objeto* OM_create_object(ObjectType type, float x, float y, int w, int h, Color cor, int layer) {
    int index = -1;

    // 1. Decisão de vaga usando a sua lista de mortos
    if (total_dead_objects > 0) {
        index = dead_object_indices[total_dead_objects - 1];
        total_dead_objects -= 1;
    } 
    // 2. Se não houver mortos, expande o array real
    else if (total_objetos < MAX_OBJECTS) {
        index = total_objetos;
        total_objetos += 1;
    } 
    // 3. Sem vagas físicas disponíveis
    else {
        return NULL; 
    }

    Objeto* obj = &lista_objetos[index];
    obj->type = type;
    obj->ativo = true;
    obj->layer = layer;
    obj->color = cor;
    obj->animation_speed = 0.1f;
    obj->x = x;
    obj->y = y;
    obj->width = w;
    obj->height = h;
    obj->sx = 1.0f;
    obj->sy = 1.0f;
    obj->target_sx = 1.0f;
    obj->target_sy = 1.0f;
    obj->sprite = NULL;

    return obj; 
}

void OM_kill_object(Objeto* obj) {
    if (obj == NULL || !obj->ativo) return;
    obj->ativo = false;

    int index_morto = obj - lista_objetos;

    dead_object_indices[total_dead_objects] = index_morto;
    total_dead_objects += 1;
}

//  Funçoes de render e update
void OM_update_all(float dt) {
    for (int i =0; i < total_objetos; i++) {
        Objeto* obj = &lista_objetos[i];
        if (obj->ativo) {
            // calcula a velocidade de animação baseada no delta time (dt)
            float fator_suave = obj->animation_speed * 10.0f * dt;
            if (fator_suave > 1.0f) fator_suave = 1.0f;

            // atualiza a animação
            obj->sx = obj->sx + (obj->target_sx - obj->sx) * fator_suave;
            obj->sy = obj->sy + (obj->target_sy - obj->sy) * fator_suave;
        }
    }
}

void OM_render_all() {
    Objeto* fila_desenho[MAX_OBJECTS];
    int qtde_desenho = 0;

    // procura quem deve ser desenhado e coloca na fila de desenho
    for (int i = 0; i < total_objetos; i++) {
        if (lista_objetos[i].ativo) {
            fila_desenho[qtde_desenho] = &lista_objetos[i];
            qtde_desenho++;
        }
    }

    // ordena a fila de desenho por camada 
    for (int i = 0; i < qtde_desenho -1; i++) {
        for (int j = 0; j < qtde_desenho - i - 1; j++) {
            
            if (fila_desenho[j]->layer > fila_desenho[j + 1]->layer) {
                // Estrutura de troca (Swap) de ponteiros
                Objeto* temp = fila_desenho[j];
                fila_desenho[j] = fila_desenho[j + 1];
                fila_desenho[j + 1] = temp;
            }
        }
    }

    // desenha pela ordem no buffer
    for (int i = 0; i < qtde_desenho; i++) {
        Objeto* obj = fila_desenho[i];

        // Aplica as escalas atuais geradas pelo OM_update_all
        float largura_final = obj->width * obj->sx;
        float altura_final = obj->height * obj->sy;

        switch (obj->type) {
            case OBJ_RECTANGLE:
                GFX_set_color(obj->color);
                GFX_draw_rectangle(
                    obj->x - largura_final / 2, obj->y - altura_final / 2,
                    obj->x + largura_final / 2, obj->y - altura_final / 2,
                    obj->x + largura_final / 2, obj->y + altura_final / 2,
                    obj->x - largura_final / 2, obj->y + altura_final / 2
                );
                break;
            case OBJ_TRIANGLE:
                // Implementar desenho de triângulo usando GFX
                break;
            case OBJ_CIRCLE:
                // Implementar desenho de círculo usando GFX
                break;
            case OBJ_SPRITE:
                if (obj->sprite != NULL) {
                    GFX_draw_sprite_scaled(obj->sprite, obj->x - largura_final / 2, obj->y - altura_final / 2, largura_final, altura_final);
                }
                break;
        }
    }
}


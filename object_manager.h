#ifndef OBJECT_MANAGER_H
#define OBJECT_MANAGER_H

#include <stdbool.h>
#include "gfx.h" // Garante que os tipos Color e Sprite existam aqui

#define MAX_OBJECTS 256

typedef enum {
    OBJ_RECTANGLE,
    OBJ_TRIANGLE,
    OBJ_CIRCLE,
    OBJ_SPRITE
} ObjectType;

typedef struct {
    ObjectType type;
    bool ativo;
    int layer;
    float x, y;                 // Posição central do objeto
    int width, height;          // Dimensões base (raio será baseado em width)
    float sx, sy;               // Escala atual (animação)
    float target_sx, target_sy; // Escala alvo
    float animation_speed;      // Velocidade de animação
    Color color;
    Sprite* sprite;
} Objeto;

// --- COMANDOS DE CICLO DE VIDA ---
void OM_clear(void);
void OM_list_empty(void); // Mantida caso queira forçar uma recontagem de lixo externa
void OM_update_all(float dt);
void OM_render_all(void);

// --- COMANDOS DE MANIPULAÇÃO (O que o main.c vai chamar) ---

// Ajustado para receber ObjectType e Color exatamente igual ao seu arquivo .c
Objeto* OM_create_object(ObjectType type, float x, float y, int w, int h, Color cor, int layer);

// Adicionada a assinatura do matador de objetos que consome o ponteiro
void OM_kill_object(Objeto* obj);

#endif

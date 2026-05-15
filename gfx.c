#include "gfx.h"
#include "gfx.h"
#include <stdlib.h>
#include <math.h> 


// Projeto altamnete inpirado no opengl //

// DEFINIÇÕES GLOBAIS //
Color GFX_buffer[WIDTH * HEIGHT]; 
static Sprite* _gfx_current_texture = NULL;


Vertex vertexBuffer[MAX_VERTICES];
int vertexCount = 0;

static GFX_Primitive current_mode = GFX_POINTS;
static Color _gfx_current_color = 0xFFFF;


// HELPERS MATEMÁTICOS //
static float _edge_function(float ax, float ay, float bx, float by, float px, float py) {
    return (bx - ax) * (py - ay) - (by - ay) * (px - ax);
}

void _GFX_project_vertex(Vertex in, int *sx, int *sy) {
    *sx = (int)in.x + (WIDTH / 2);
    *sy = (int)in.y + (HEIGHT / 2);
}

static int _min3(int a, int b, int c) {
    int m = a < b ? a : b;
    return m < c ? m : c;
}

static int _max3(int a, int b, int c) {
    int m = a > b ? a : b;
    return m > c ? m : c;
}




// FUNÇÔES //
void GFX_set_color(Color cor) {
    _gfx_current_color = cor;
}

void GFX_begin(GFX_Primitive mode) {
    current_mode = mode; // Define se é Ponto, Linha ou Triângulo
    vertexCount = 0;     // "Limpa" o buffer para começar um novo lote
}

void GFX_bind_sprite(Sprite* sprite) {
    _gfx_current_texture = sprite;
}

void GFX_clear(Color cor) {
    for (int i = 0; i < WIDTH * HEIGHT; i++) {
        GFX_buffer[i] = cor;
    }
}

void GFX_draw_pixel(int x, int y, Color cor) {
    if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
        int index = y * WIDTH + x;
        GFX_buffer[index] = cor;   // <--- linha faltando
    }
}


// PRIMITIVES //

void _GFX_draw_line(float x0, float y0, float x1, float y1) {
    GFX_begin(GFX_LINES);
        // Linhas simples não usam textura, passamos 0,0 nos UVs
        GFX_vertex(x0, y0, 0.0f, 0.0f);
        GFX_vertex(x1, y1, 0.0f, 0.0f);
    GFX_end();
}

void GFX_draw_rectangle(float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3) {
    // Um retângulo é formado por dois triângulos
    GFX_begin(GFX_TRIANGLES);
        // Primeiro triângulo (V0, V1, V2) - Mapeando UVs padrão
        GFX_vertex(x0, y0, 0.0f, 0.0f); // Topo-Esquerda
        GFX_vertex(x1, y1, 1.0f, 0.0f); // Topo-Direita
        GFX_vertex(x2, y2, 1.0f, 1.0f); // Baixo-Direita
        
        // Segundo triângulo (V0, V2, V3)
        GFX_vertex(x0, y0, 0.0f, 0.0f); // Topo-Esquerda
        GFX_vertex(x2, y2, 1.0f, 1.0f); // Baixo-Direita
        GFX_vertex(x3, y3, 0.0f, 1.0f); // Baixo-Esquerda
    GFX_end();
}


void GFX_draw_sprite_scaled(Sprite* sprite, int dx, int dy, int dw, int dh) {
    for (int y = 0; y < dh; y++) {
        for (int x = 0; x < dw; x++) {
            // Mapeia o pixel da tela de volta para o pixel do sprite (Nearest Neighbor)
            int sx = (x * sprite->largura) / dw;
            int sy = (y * sprite->altura) / dh;
            
            Color cor = sprite->dados[sy * sprite->largura + sx];
            
            // Desenha apenas se não for preto (transparência simples)
            if (cor != 0x0000) {
                GFX_draw_pixel(dx + x, dy + y, cor);
            }
        }
    }
}


void GFX_set_background_color(Color cor) {
    #define ____ cor

    // Um sprite 4x4 simples (ex: uma carinha ou padrão)
    Color pixels_teste[] = {
        ____
    };
    Sprite meuSprite = { 1, 1, pixels_teste };

    GFX_draw_sprite_scaled(&meuSprite, 0, 0, WIDTH, HEIGHT);
}

void GFX_vertex(float x, float y, float u, float v) {
    if (vertexCount >= MAX_VERTICES) return;

    Vertex* vert = &vertexBuffer[vertexCount];
    vert->x = x;
    vert->y = y;
    vert->u = u; // Coordenada horizontal da textura
    vert->v = v; // Coordenada vertical da textura
    vert->color = _gfx_current_color;
    vertexCount++;
}



// RASTER //
static void _GFX_rasterize_point(Vertex v) {
    int x, y;
    _GFX_project_vertex(v, &x, &y);
    GFX_draw_pixel(x, y, v.color);
}

static void _GFX_rasterize_line(Vertex v0, Vertex v1) {
    int x0, y0, x1, y1;
    _GFX_project_vertex(v0, &x0, &y0);
    _GFX_project_vertex(v1, &x1, &y1);

    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2;

    while (1) {
        // Para linhas, usamos um Z constante ou interpolado simples
        GFX_draw_pixel(x0, y0, v0.color);
        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

static void _GFX_rasterize_triangle(Vertex v0, Vertex v1, Vertex v2) {
    int x0, y0, x1, y1, x2, y2;
    _GFX_project_vertex(v0, &x0, &y0);
    _GFX_project_vertex(v1, &x1, &y1);
    _GFX_project_vertex(v2, &x2, &y2);

    float area = _edge_function(x0, y0, x1, y1, x2, y2);
    if (area == 0) return;

    // Determina o sinal correto para a orientação do triângulo
    float sign = 1.0f;
    if (area < 0) {
        area = -area;
        sign = -1.0f;
    }

    // Bounding Box
    int minX = _min3(x0, x1, x2);
    int minY = _min3(y0, y1, y2);
    int maxX = _max3(x0, x1, x2);
    int maxY = _max3(y0, y1, y2);

    // Clipagem de tela
    minX = (minX < 0) ? 0 : minX;
    minY = (minY < 0) ? 0 : minY;
    maxX = (maxX >= WIDTH) ? WIDTH - 1 : maxX;
    maxY = (maxY >= HEIGHT) ? HEIGHT - 1 : maxY;

    // Fator inverso da área para evitar divisões dentro do loop aninhado
    float inv_area = 1.0f / area;

    for (int py = minY; py <= maxY; py++) {
        for (int px = minX; px <= maxX; px++) {
            // Calcula as funções de borda multiplicando pelo sinal de orientação
            float w0 = _edge_function(x1, y1, x2, y2, px, py) * sign;
            float w1 = _edge_function(x2, y2, x0, y0, px, py) * sign;
            float w2 = _edge_function(x0, y0, x1, y1, px, py) * sign;

            // Se o ponto está dentro do triângulo (ou nas bordas se w == 0)
            if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
                Color final_c = v0.color;

                if (_gfx_current_texture) {
                    // Pesos baricêntricos normais calculados por multiplicação
                    float b0 = w0 * inv_area;
                    float b1 = w1 * inv_area;
                    float b2 = w2 * inv_area;

                    // Interpolação das coordenadas UV da textura
                    float u = (b0 * v0.u) + (b1 * v1.u) + (b2 * v2.u);
                    float v = (b0 * v0.v) + (b1 * v1.v) + (b2 * v2.v);

                    int tx = (int)(u * (_gfx_current_texture->largura - 1));
                    int ty = (int)(v * (_gfx_current_texture->altura - 1));

                    final_c = _gfx_current_texture->dados[ty * _gfx_current_texture->largura + tx];
                    
                    // Ignora pixels transparentes (chave de cor preta)
                    if (final_c == 0x0000) continue; 
                }

                // Desenha diretamente na tela sem passar o Z
                GFX_draw_pixel(px, py, final_c);
            }
        }
    }
}




//  GFX End  //
void GFX_end(void) {
    for (int i = 0; i < vertexCount; ) {
        switch (current_mode) {
            case GFX_POINTS:
                _GFX_rasterize_point(vertexBuffer[i]);
                i += 1;
                break;
            case GFX_LINES:
                _GFX_rasterize_line(vertexBuffer[i], vertexBuffer[i+1]);
                i += 2;
                break;
            case GFX_TRIANGLES:
                if (i + 2 < vertexCount) {
                    _GFX_rasterize_triangle(vertexBuffer[i], vertexBuffer[i+1], vertexBuffer[i+2]);
                }
                i += 3;
                break;
            default: i++; break;
        }
    }
    vertexCount = 0;
    _gfx_current_texture = NULL; // Reset opcional da textura
}

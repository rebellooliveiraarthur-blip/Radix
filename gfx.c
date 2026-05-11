#include "gfx.h"

// Projeto altamnete inpirado no opengl //

Color GFX_buffer[WIDTH * HEIGHT]; 
float GFX_zbuffer[WIDTH * HEIGHT];
static Sprite* _gfx_current_texture = NULL;


Vertex vertexBuffer[MAX_VERTICES];
int vertexCount = 0;

static GFX_Primitive current_mode = GFX_POINTS;
static Color _gfx_current_color = 0xFFFF;

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
        GFX_zbuffer[i] = 1000.0f; // Inicializa com um valor "muito longe"
    }
}


void GFX_draw_pixel(int x, int y, float z, Color cor) {
    if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
        int index = y * WIDTH + x;
        // Teste de profundidade: só desenha se o novo Z for menor (mais perto)
        if (z < GFX_zbuffer[index]) {
            GFX_buffer[index] = cor;
            GFX_zbuffer[index] = z;
        }
    }
}



static float _edge_function(float ax, float ay, float bx, float by, float px, float py) {
    return (px - ax) * (by - ay) - (py - ay) * (bx - ax);
}


void GFX_draw_sprite_scaled(Sprite* sprite, int dx, int dy, float dz, int dw, int dh) {
    for (int y = 0; y < dh; y++) {
        for (int x = 0; x < dw; x++) {
            // Mapeia o pixel da tela de volta para o pixel do sprite (Nearest Neighbor)
            int sx = (x * sprite->largura) / dw;
            int sy = (y * sprite->altura) / dh;
            
            Color cor = sprite->dados[sy * sprite->largura + sx];
            
            // Desenha apenas se não for preto (transparência simples)
            if (cor != 0x0000) {
                GFX_draw_pixel(dx + x, dy + y, dz, cor);
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

    GFX_draw_sprite_scaled(&meuSprite, 0, 0, 1, WIDTH, HEIGHT);
}

// --- Manipulação de vértice --- //
void GFX_vertex(float x, float y, float z) {
    if (vertexCount >= MAX_VERTICES) {
        // Opcional: GFX_end(); GFX_begin(current_mode); 
        // Isso faria um "auto-flush" se o buffer lotasse
        return; 
    }

    Vertex* v = &vertexBuffer[vertexCount];
    v->x = x;
    v->y = y;
    v->z = z;

    v->color = _gfx_current_color;     
    v->u = 0.0f; v->v = 0.0f;

    vertexCount++;
}

// --- Funçoes internas --- //
void _GFX_project_vertex(Vertex in, int *sx, int *sy) {
    // FASE ATUAL: 2D simples
    *sx = (int)in.x + (WIDTH / 2);
    *sy = (int)in.y + (HEIGHT / 2);

    /* 
    FASE FUTURA (3D):
    Vector4 v = {in.x, in.y, in.z, 1.0f};
    v = Mat4_Multiply(mvp_matrix, v); // Transforma
    v.x /= v.w; v.y /= v.w;           // Projeção perspectiva
    *sx = (v.x + 1.0f) * 0.5f * WIDTH;
    *sy = (v.y + 1.0f) * 0.5f * HEIGHT;
    */
}

static int _min3(int a, int b, int c) {
    int m = a < b ? a : b;
    return m < c ? m : c;
}

static int _max3(int a, int b, int c) {
    int m = a > b ? a : b;
    return m > c ? m : c;
}

// --- GFX End --- //
void GFX_end(void) {
    int x0, y0, x1, y1, x2, y2;

    for (int i = 0; i < vertexCount; ) { // O incremento acontece dentro do switch
        switch (current_mode) {
            case GFX_POINTS:
                _GFX_project_vertex(vertexBuffer[i], &x0, &y0);
                GFX_draw_pixel(x0, y0, vertexBuffer[i].z, vertexBuffer[i].color);
                i++; 
                break;

            case GFX_LINES:
                if (i + 1 < vertexCount) {
                    _GFX_project_vertex(vertexBuffer[i], &x0, &y0);
                    _GFX_project_vertex(vertexBuffer[i+1], &x1, &y1);
                    // _GFX_draw_line_raster(x0, y0, x1, y1, color);
                    i += 2;
                } else i++;
                break;

            case GFX_TRIANGLES:
                if (i + 2 < vertexCount) {
                    int x0, y0, x1, y1, x2, y2;
                    _GFX_project_vertex(vertexBuffer[i],   &x0, &y0);
                    _GFX_project_vertex(vertexBuffer[i+1], &x1, &y1);
                    _GFX_project_vertex(vertexBuffer[i+2], &x2, &y2);

                    float z_camada = vertexBuffer[i].z; // Pega o Z definido no GFX_vertex

                    // 1. Achar o Bounding Box (limites do triângulo)
                    int minX = _min3(x0, x1, x2);
                    int minY = _min3(y0, y1, y2);
                    int maxX = _max3(x0, x1, x2);
                    int maxY = _max3(y0, y1, y2);

                    // 2. Clipar contra os limites da tela (evita crash)
                    minX = (minX < 0) ? 0 : minX;
                    minY = (minY < 0) ? 0 : minY;
                    maxX = (maxX >= WIDTH) ? WIDTH - 1 : maxX;
                    maxY = (maxY >= HEIGHT) ? HEIGHT - 1 : maxY;

                    float area = _edge_function(x0, y0, x1, y1, x2, y2);

                    // Se a área for <= 0, o triângulo está "de costas" ou é inválido
                    if (area <= 0) { 
                        i += 3; 
                        continue; 
                    }

                    // 3. Iterar sobre o Bounding Box
                    for (int py = minY; py <= maxY; py++) {
                        for (int px = minX; px <= maxX; px++) {
                            // Testa se o pixel (px, py) está dentro das 3 arestas
                            float w0 = _edge_function(x1, y1, x2, y2, px, py);
                            float w1 = _edge_function(x2, y2, x0, y0, px, py);
                            float w2 = _edge_function(x0, y0, x1, y1, px, py);

                            if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
                                // Calcula o peso de cada vértice para este pixel específico
                                float bary0 = w0 / area;
                                float bary1 = w1 / area;
                                float bary2 = w2 / area;

                                // Interpola o Z: cada vértice contribui com sua profundidade
                                float pixel_z = (bary0 * vertexBuffer[i].z) + 
                                                (bary1 * vertexBuffer[i+1].z) + 
                                                (bary2 * vertexBuffer[i+2].z);

                                GFX_draw_pixel(px, py, pixel_z, vertexBuffer[i].color);
                            }
                        }
                    }
                    i += 3;
                } else i++;
                break;

        }
    }
    vertexCount = 0; 
}

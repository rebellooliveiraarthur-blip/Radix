/* ============================================================
 * MINI-OPENGL ROADMAP (comente/descomente conforme implementar)
 * ============================================================
 *
 * FASE 1 — Primitive Batching (glBegin/glEnd)
 *   [x] Batch buffer (array de vértices + count)
 *   [x] GFX_begin(GFX_POINTS | GFX_LINES | GFX_TRIANGLES)
 *   [x] GFX_vertex(x, y, z) — empilha no batch
 *   [x] GFX_end()           — rasteriza tudo de uma vez
 *   [ ] Rasterizador de linhas  (Bresenham)
 *   [ ] Rasterizador de triângulos (scanline + edge functions)
 *
 * FASE 2 — Matrix Stack (glMatrixMode / glTranslate / glRotate)
 *   [ ] Matriz 4x4 (typedef float Mat4[4][4])
 *   [ ] GFX_matrix_identity()
 *   [ ] GFX_matrix_translate(tx, ty, tz)
 *   [ ] GFX_matrix_rotate(angle, ax, ay, az)
 *   [ ] GFX_matrix_scale(sx, sy, sz)
 *   [ ] push / pop (pilha de matrizes)
 *   [ ] Aplicar matriz ao vértice: v_out = M * v_in
 *
 * FASE 3 — Projection & Camera (gluPerspective / glOrtho / gluLookAt)
 *   [ ] GFX_ortho(left, right, bottom, top, near, far)
 *   [ ] GFX_perspective(fov, aspect, near, far)
 *   [ ] GFX_look_at(eye, target, up)
 *   [ ] Pipeline: Model → View → Projection → Clip → NDC → Screen
 *
 * FASE 4 — Vertex Pipeline (glVertexPointer / glEnableClientState)
 *   [ ] Vertex struct com posição, cor, UV opcional
 *   [ ] Per-vertex color: GFX_color(r, g, b)
 *   [ ] Per-vertex UV (para textura futura)
 *   [ ] Clipping contra o viewport (Cohen-Sutherland ou Sutherland-Hodgman)
 *
 * FASE 5 — State Machine (glEnable / glDisable / glColor)
 *   [ ] GFX_set_color(c)           — cor de estado atual
 *   [ ] GFX_enable_wireframe(bool) — desenha apenas arestas
 *   [ ] GFX_enable_depth_test(bool) — Z-buffer
 *   [ ] GFX_enable_blend(mode)     — alpha, additive, etc.
 *   [ ] GFX_set_depth_func(cmp)    — LESS, LEQUAL, ALWAYS
 *
 * FASE 6 — Depth Buffer (GL_DEPTH_TEST)
 *   [ ] uint16_t depth_buffer[WIDTH][HEIGHT]
 *   [ ] GFX_clear_depth(float d)
 *   [ ] Teste z antes de escrever pixel
 *
 * FASE 7 — Texturing (glBindTexture / glTexImage2D)
 *   [ ] Texture2D struct (dados, w, h, format)
 *   [ ] GFX_bind_texture(tex)
 *   [ ] UV → sample texture (nearest / bilinear)
 *
 * FASE 8 — Mesh / VAO (glGenBuffers / glVertexAttribPointer)
 *   [ ] Mesh struct (vertices[], count, index_buffer[])
 *   [ ] GFX_create_mesh(vertices, count)
 *   [ ] GFX_draw_mesh(mesh) — reusa sem reenviar dados
 *   [ ] Indexed drawing: GFX_draw_indexed(mesh)
 *
 * FASE 9 — Shader-like Pipeline (programável, tipo GLSL mas em C)
 *   [ ] Vertex shader callback: fn(Vertex* in, Vertex* out)
 *   [ ] Fragment shader callback: fn(FragData* frag) -> Color
 *   [ ] Pipeline: VS → Clip → Raster → FS → FB
 *
 * FASE 10 — Performance (ESP32 real)
 *   [ ] Buffer linear (1D array) ao invés de 2D
 *   [ ] DMA para enviar frame ao display
 *   [ ] PSRAM para buffers grandes
 *   [ ] Fixed-point ao invés de float (se precisar de speed)
 *   [ ] display_update: usar textura SDL em vez de pixel-per-rect
 * ============================================================ */

/* ============================================================
 * TIPOS
 * ============================================================ */


#ifndef GFX_H
#define GFX_H

#define MAX_VERTICES 1024

#include "display.h" // Puxa WIDTH, HEIGHT e Color

/* 2. Definições de Tipos */
typedef enum { 
    GFX_POINTS, 
    GFX_LINES, 
    GFX_TRIANGLES 
} GFX_Primitive;

typedef struct {
    float x, y, z; 
    float u, v;     
    Color color;    
} Vertex;

typedef struct {
    int largura;
    int altura; 
    Color* dados;
} Sprite;


extern Color GFX_buffer[WIDTH * HEIGHT]; 
extern Vertex vertexBuffer[MAX_VERTICES]; // Agora o compilador sabe o tamanho!
extern int vertexCount;

void GFX_begin(GFX_Primitive mode);
void GFX_vertex(float x, float y, float z, float u, float v);
void GFX_end(void);
void GFX_set_color(Color cor);
void GFX_clear(Color cor);
void GFX_draw_sprite_scaled(Sprite* sprite, int dx, int dy, float dz, int dw, int dh);
void GFX_set_background_color(Color cor);
void GFX_draw_rectangle(float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3, float z);
void _GFX_draw_line(float x0, float y0, float x1, float y1, float z);
void GFX_bind_sprite(Sprite* sprite);

#endif

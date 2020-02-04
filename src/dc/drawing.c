//
// Created by cpasjuste on 28/01/2020.
//

#include <kos.h>
#include "drawing.h"

static pvr_ptr_t font_texture = NULL;

pvr_init_params_t params = {
        {PVR_BINSIZE_16, PVR_BINSIZE_0, PVR_BINSIZE_32, PVR_BINSIZE_0, PVR_BINSIZE_0},
        512 * 1024
};

static void draw_init_font() {

    uint16 *vram;
    int x, y;

    font_texture = pvr_mem_malloc(256 * 256 * 2);
    vram = (uint16 *) font_texture;

    for (y = 0; y < 8; y++) {
        for (x = 0; x < 16; x++) {
            bfont_draw(vram, 256, 0, y * 16 + x);
            vram += 16;
        }
        vram += 23 * 256;
    }
}

/* The following funcs blatantly ripped from libconio */
/* Draw one font character (12x24) */
static void draw_char(float x1, float y1, float z1, Color color, int c) {

    pvr_vertex_t vert;
    int ix, iy;
    float u1, v1, u2, v2;

    if (c == ' ')
        return;

    if (!(c > ' ' && c < 127))
        return;

    ix = (c % 16) * 16;
    iy = (c / 16) * 24;
    u1 = ix * 1.0f / 256.0f;
    v1 = iy * 1.0f / 256.0f;
    u2 = (ix + 12) * 1.0f / 256.0f;
    v2 = (iy + 24) * 1.0f / 256.0f;

    vert.flags = PVR_CMD_VERTEX;
    vert.x = x1;
    vert.y = y1 + DRAW_FONT_HEIGHT;
    vert.z = z1;
    vert.u = u1;
    vert.v = v2;
    vert.argb = DRAW_PACK_COLOR(color.a, color.r, color.g, color.b);
    vert.oargb = 0;
    pvr_prim(&vert, sizeof(vert));

    vert.x = x1;
    vert.y = y1;
    vert.u = u1;
    vert.v = v1;
    pvr_prim(&vert, sizeof(vert));

    vert.x = x1 + DRAW_FONT_WIDTH;
    vert.y = y1 + DRAW_FONT_HEIGHT;
    vert.u = u2;
    vert.v = v2;
    pvr_prim(&vert, sizeof(vert));

    vert.flags = PVR_CMD_VERTEX_EOL;
    vert.x = x1 + DRAW_FONT_WIDTH;
    vert.y = y1;
    vert.u = u2;
    vert.v = v1;
    pvr_prim(&vert, sizeof(vert));
}

/* draw len chars at string */
void draw_string(float x, float y, float z, Color color, char *str) {

    int i, len;
    pvr_poly_cxt_t cxt;
    pvr_poly_hdr_t poly;

    pvr_poly_cxt_txr(&cxt, PVR_LIST_TR_POLY, PVR_TXRFMT_ARGB1555 | PVR_TXRFMT_NONTWIDDLED,
                     256, 256, font_texture, PVR_FILTER_NONE);
    pvr_poly_compile(&poly, &cxt);
    pvr_prim(&poly, sizeof(poly));

    len = strlen(str);
    for (i = 0; i < len; i++) {
        draw_char(x, y, z, color, str[i]);
        x += DRAW_FONT_WIDTH;
    }
}

/* draw a box (used by cursor and border, etc) (at 1.0f z coord) */
void draw_box(float x, float y, float w, float h, float z, Color color) {
    pvr_poly_cxt_t cxt;
    pvr_poly_hdr_t poly;
    pvr_vertex_t vert;

    pvr_poly_cxt_col(&cxt, PVR_LIST_TR_POLY);
    pvr_poly_compile(&poly, &cxt);
    pvr_prim(&poly, sizeof(poly));

    vert.flags = PVR_CMD_VERTEX;
    vert.x = x;
    vert.y = y + h;
    vert.z = z;
    vert.u = vert.v = 0.0f;
    vert.argb = DRAW_PACK_COLOR(color.a, color.r, color.g, color.b);
    vert.oargb = 0;
    pvr_prim(&vert, sizeof(vert));

    vert.y -= h;
    pvr_prim(&vert, sizeof(vert));

    vert.y += h;
    vert.x += w;
    pvr_prim(&vert, sizeof(vert));

    vert.flags = PVR_CMD_VERTEX_EOL;
    vert.y -= h;
    pvr_prim(&vert, sizeof(vert));
}

void draw_box_outline(float x, float y, float w, float h, float z, Color color,
                      Color outline_color, float outline_size) {

    draw_box(x - outline_size, y - outline_size, w + (outline_size * 2), h + (outline_size * 2), z - 1, outline_color);
    draw_box(x, y, w, h, z, color);
}

void draw_init() {
    pvr_init(&params);
    draw_init_font();
}

void draw_exit() {
    if (font_texture != NULL) {
        pvr_mem_free(font_texture);
    }
}

void draw_start() {
    pvr_wait_ready();
    pvr_scene_begin();
    pvr_list_begin(PVR_LIST_TR_POLY);
}

void draw_end() {
    pvr_list_finish();
    pvr_scene_finish();
}

Vec2 draw_get_screen_size() {
    return (Vec2) {640, 480};
}

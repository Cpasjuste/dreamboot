//
// Created by cpasjuste on 28/01/2020.
//

#include <png/png.h>
#include "retrodream.h"
#include "drawing.h"
#include "bmfont.h"

static BMFont bmf_font;
static pvr_ptr_t bmf_tex = NULL;

pvr_init_params_t params = {
        {PVR_BINSIZE_16, PVR_BINSIZE_0, PVR_BINSIZE_32, PVR_BINSIZE_0, PVR_BINSIZE_0},
        512 * 1024
};

typedef struct {
    char id[4];
    short width;
    short height;
    int type;
    int size;
} tex_header_t;

static void draw_init_font() {

    FILE *fp;
    tex_header_t hdr;

    // parse BMFont font information
    if (bmf_parse(ROMDISK_PATH"/future.fnt", &bmf_font) != 0) {
        return;
    }

    // load "texconv" texture
    fp = fopen(ROMDISK_PATH"/future_0.tex", "r");
    if (fp == NULL) {
        return;
    }
    // read "texconv" texture header
    fread(&hdr, sizeof(hdr), 1, fp);
    // allocate pvr mem
    bmf_tex = pvr_mem_malloc(hdr.size);
    // read "texconv" texture to pvr mem
    fread(bmf_tex, hdr.size, 1, fp);

    // all done
    fclose(fp);
}

static void draw_char(float x1, float y1, float z1, Color color, BMFontChar *c) {

    pvr_vertex_t vert;

    vert.flags = PVR_CMD_VERTEX;
    vert.x = x1 + (float) c->xoffset;
    vert.y = y1 + (float) c->height + (float) c->yoffset;
    vert.z = z1;
    vert.u = (float) c->x / (float) bmf_font.common.scaleW;
    vert.v = (float) (c->y + c->height) / (float) bmf_font.common.scaleH;
    vert.argb = DRAW_PACK_COLOR(color.a, color.r, color.g, color.b);
    vert.oargb = 0;
    pvr_prim(&vert, sizeof(vert));

    vert.x = x1 + (float) c->xoffset;
    vert.y = y1 + (float) c->yoffset;
    vert.u = (float) c->x / (float) bmf_font.common.scaleW;
    vert.v = (float) c->y / (float) bmf_font.common.scaleH;
    pvr_prim(&vert, sizeof(vert));

    vert.x = x1 + (float) (c->width + c->xoffset);
    vert.y = y1 + (float) (c->height + c->yoffset);
    vert.u = (float) (c->x + c->width) / (float) bmf_font.common.scaleW;
    vert.v = (float) (c->y + c->height) / (float) bmf_font.common.scaleH;
    pvr_prim(&vert, sizeof(vert));

    vert.flags = PVR_CMD_VERTEX_EOL;
    vert.x = x1 + (float) (c->width + c->xoffset);
    vert.y = y1 + (float) c->yoffset;
    vert.u = (float) (c->x + c->width) / (float) bmf_font.common.scaleW;
    vert.v = (float) c->y / (float) bmf_font.common.scaleH;
    pvr_prim(&vert, sizeof(vert));
}

/* draw len chars at string */
void draw_string(float x, float y, float z, Color color, char *str) {

    int i, len;
    pvr_poly_cxt_t cxt;
    pvr_poly_hdr_t poly;

    pvr_poly_cxt_txr(&cxt, PVR_LIST_TR_POLY, PVR_TXRFMT_ARGB1555 | PVR_TXRFMT_VQ_ENABLE,
                     256, 256, bmf_tex, PVR_FILTER_NONE);
    pvr_poly_compile(&poly, &cxt);
    pvr_prim(&poly, sizeof(poly));

    len = strlen(str);
    for (i = 0; i < len; i++) {
        char c = str[i];
        if (!(c > 31 && c < 127)) {
            continue;
        }
        BMFontChar bmfChar = bmf_font.chars[(int) c];
        draw_char(x, y, z, color, &bmfChar);
        x += (float) (bmfChar.xadvance + bmfChar.xoffset);
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
    if (bmf_tex != NULL) {
        pvr_mem_free(bmf_tex);
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

int draw_printf(int level, const char *fmt, ...) {

    if (bmf_tex == NULL) {
        return 0;
    }

    char buff[512];
    va_list args;
    Color color = COL_WHITE;
    Vec2 screenSize = draw_get_screen_size();

    memset(buff, 0, 512);
    va_start(args, fmt);
    int ret = vsnprintf(buff, 512, fmt, args);
    va_end(args);

    switch (level) {
        case DBG_DEAD:
        case DBG_ERROR:
        case DBG_CRITICAL:
            color = COL_RED;
            break;
        case DBG_WARNING:
            color = COL_YELLOW;
        default:
            break;
    }

    draw_start();
    draw_string(16, screenSize.y - DRAW_FONT_HEIGHT - 16, 200, color, buff);
    draw_end();

    return ret;
}

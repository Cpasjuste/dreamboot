//
// Created by cpasjuste on 28/01/2020.
//

#ifndef LOADER_DRAWING_H
#define LOADER_DRAWING_H

#define DRAW_FONT_WIDTH 12
#define DRAW_FONT_HEIGHT 24
#define DRAW_FONT_LINE_SPACING 4

#define DRAW_PACK_COLOR(a, r, g, b) ( \
    a << 24 | \
    r << 16 | \
    g << 8 | \
    b << 0 )

typedef struct color_t {
    uint8 r;
    uint8 g;
    uint8 b;
    uint8 a;
} Color;

typedef struct rect_t {
    float left;
    float top;
    float width;
    float height;
} Rect;

typedef struct vec2_t {
    float x;
    float y;
} Vec2;

#define COL_WHITE       (Color) {255, 255, 255, 255}
#define COL_BLUE        (Color) {54, 70, 93, 255}
#define COL_RED         (Color) {255, 81, 72, 255}
#define COL_BLUE_LIGHT  (Color) {178, 226, 249, 255}
#define COL_YELLOW      (Color) {240, 226, 107, 255}

void draw_init();

void draw_exit();

void draw_start();

void draw_end();

void draw_string(float x, float y, float z, Color color, char *str);

void draw_box(float x, float y, float w, float h, float z, Color color);

void draw_box_outline(float x, float y, float w, float h, float z,
                      Color color, Color outline_color, float outline_size);

Vec2 draw_get_screen_size();

#endif //LOADER_DRAWING_H

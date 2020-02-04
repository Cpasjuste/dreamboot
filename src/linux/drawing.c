//
// Created by cpasjuste on 03/02/2020.
//

#include <SDL2/SDL.h>
#include "SDL_FontCache.h"
#include "retrodream.h"
#include "drawing.h"

static SDL_Window *window;
static SDL_Renderer *renderer;
static FC_Font *font;
static int drawing = 0;

void draw_init() {

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE);

    window = SDL_CreateWindow(
            "LOADER", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            640, 480, SDL_WINDOW_OPENGL);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    font = FC_CreateFont();
    FC_LoadFont(font, renderer, "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
                DRAW_FONT_HEIGHT, FC_MakeColor(0, 0, 0, 255), TTF_STYLE_NORMAL);
}

void draw_exit() {
    FC_FreeFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}

void draw_start() {
    if (drawing == 0) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        drawing = 1;
    }
}

void draw_end() {
    if (drawing == 1) {
        SDL_RenderPresent(renderer);
        drawing = 0;
    }
}

Vec2 draw_get_screen_size() {
    return (Vec2) {640, 480};
}

void draw_string(float x, float y, float z, Color color, char *str) {
    SDL_Color c = {color.r, color.g, color.b, color.a};
    FC_DrawColor(font, renderer, x, y, c, str);
}

void draw_box(float x, float y, float w, float h, float z, Color color) {
    SDL_Rect r = {(int) x, (int) y, (int) w, (int) h};
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &r);
}

void draw_box_outline(float x, float y, float w, float h, float z,
                      Color color, Color outline_color, float outline_size) {
    draw_box(x - outline_size, y - outline_size, w + (outline_size * 2), h + (outline_size * 2), z - 1, outline_color);
    draw_box(x, y, w, h, z, color);
}

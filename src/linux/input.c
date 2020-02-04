//
// Created by cpasjuste on 03/02/2020.
//

#include <SDL2/SDL_events.h>
#include "retrodream.h"
#include "input.h"

uint32 get_input() {

    uint32 input = 0;
    SDL_Event ev;

    while (SDL_PollEvent(&ev)) {
        if (ev.type == SDL_QUIT) {
            return INPUT_QUIT;
        }

        if (ev.type == SDL_KEYDOWN) {
            switch (ev.key.keysym.scancode) {
                case SDL_SCANCODE_LEFT:
                    input |= INPUT_LEFT;
                    break;
                case SDL_SCANCODE_RIGHT:
                    input |= INPUT_RIGHT;
                    break;
                case SDL_SCANCODE_UP:
                    input |= INPUT_UP;
                    break;
                case SDL_SCANCODE_DOWN:
                    input |= INPUT_DOWN;
                    break;
                case SDL_SCANCODE_A:
                    input |= INPUT_A;
                    break;
                case SDL_SCANCODE_Z:
                    input |= INPUT_B;
                    break;
                case SDL_SCANCODE_Q:
                    input |= INPUT_X;
                    break;
                case SDL_SCANCODE_S:
                    input |= INPUT_Y;
                    break;
                case SDL_SCANCODE_RETURN:
                    input |= INPUT_START;
                    break;
                default:
                    break;
            }
        }
    }

    return input;
}

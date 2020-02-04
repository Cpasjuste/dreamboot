//
// Created by cpasjuste on 28/01/2020.
//

#include "cross.h"
#include "menu.h"
#include "input.h"
#include "utility.h"
#include "drawing.h"

int main(int argc, char **argv) {

    uint32 keys = get_input();
    if (keys & INPUT_A && keys & INPUT_B) {
        dc_load_serial();
    }

    draw_init();
    loader_init();

    if (keys & INPUT_START) {
        menu_run();
    } else {
        try_boot();
        // if we didn't found anything, run menu
        menu_run();
    }

    draw_exit();

    return 0;
}

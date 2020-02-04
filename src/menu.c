//
// Created by cpasjuste on 28/01/2020.
//

#include "cross.h"
#include "input.h"
#include "drawing.h"
#include "utility.h"

FileList *fileList;
int filer_line_height;
int filer_line_max;
int filer_index = 0;
int filer_highlight_index = 0;

void menu_draw_bg(Rect *rect) {
    draw_box_outline(rect->left, rect->top, rect->width, rect->height,
                     100, COL_BLUE, COL_YELLOW, 4);
}

void menu_draw_filer(Rect *rect) {

    for (int i = 0; i < (unsigned int) filer_line_max; i++) {

        if ((int) i == filer_highlight_index) {
            draw_box_outline(rect->left - 2, rect->top + ((float) (i * filer_line_height)) - 2, rect->width + 4,
                             (float) filer_line_height + 4, 100, COL_RED, COL_WHITE, 2);
        }

        if (filer_index + i < fileList->count) {
            File *file = get_file(filer_index + i);
            if (file == NULL) {
                break;
            }
            if ((int) i == filer_highlight_index) {
                draw_string(rect->left, rect->top + ((float) (i * filer_line_height)), 101, COL_BLUE, file->name);
            } else {
                draw_string(rect->left, rect->top + ((float) (i * filer_line_height)), 101, COL_WHITE, file->name);
            }
        }
    }
}

void menu_run() {

    uint32 input = 0;

    draw_init();

    Vec2 screenSize = draw_get_screen_size();
    Rect menuRect = {32, 32, screenSize.x - 64, screenSize.y - 64};
    Rect filerRect = {menuRect.left + 8, menuRect.top + 8,
                      menuRect.width - 16, menuRect.height - 16};

    // init filer
    fileList = get_dir("/");
    filer_line_height = DRAW_FONT_HEIGHT + 4;
    filer_line_max = (int) filerRect.height / filer_line_height;
    if (filer_line_max * filer_line_height < (int) filerRect.height) {
        filer_line_height = (int) filerRect.height / filer_line_max;
    }

    while (1) {

        input = get_input();
        if (input & INPUT_QUIT) {
            break;
        } else if (input & INPUT_LEFT) {
        } else if (input & INPUT_RIGHT) {
        }

        draw_start();

        menu_draw_bg(&menuRect);
        menu_draw_filer(&filerRect);

        draw_end();
    }

    draw_exit();
}

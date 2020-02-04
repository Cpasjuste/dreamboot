//
// Created by cpasjuste on 28/01/2020.
//

#include <string.h>
#include "cross.h"
#include "input.h"
#include "drawing.h"
#include "utility.h"

static FileList *fileList;
static int filer_line_height = 0;
static int filer_line_max = 0;
static int filer_index = 0;
static int filer_highlight_index = 0;
static Rect menuRect;
static Rect filerRect;

void menu_draw_bg(Rect *rect) {
    draw_box_outline(rect->left, rect->top, rect->width, rect->height,
                     100, COL_BLUE, COL_YELLOW, 4);
}

void menu_filer_get_dir(char *path) {

    if (filer_line_height == 0) {

        filer_line_height = DRAW_FONT_HEIGHT + 4;
        filer_line_max = (int) filerRect.height / filer_line_height;
        if (filer_line_max * filer_line_height < (int) filerRect.height) {
            filer_line_height = (int) filerRect.height / filer_line_max;
        }
    }

    filer_index = 0;
    filer_highlight_index = 0;

    fileList = get_dir(path);
}

void menu_filer_draw(Rect *rect) {

    for (int i = 0; i < (unsigned int) filer_line_max; i++) {

        if (filer_index + i < fileList->count) {

            if (i == filer_highlight_index) {
                draw_box_outline(rect->left - 2, rect->top + ((float) (i * filer_line_height)) - 2, rect->width + 4,
                                 (float) filer_line_height + 4, 101, COL_RED, COL_WHITE, 2);
            }

            File *file = get_file(filer_index + i);
            if (file != NULL) {
                if (file->type == TYPE_DIR) {
                    draw_string(rect->left, rect->top + ((float) (i * filer_line_height)), 102, COL_YELLOW, file->name);
                } else {
                    draw_string(rect->left, rect->top + ((float) (i * filer_line_height)), 102, COL_WHITE, file->name);
                }
            }
        }
    }
}

int menu_filer_input() {

    uint32 input = get_input();

    if (input & INPUT_QUIT) {
        return INPUT_QUIT;
    } else if (input & INPUT_UP) {
        if (filer_highlight_index <= filer_line_max / 2 && filer_index > 0) {
            filer_index--;
        } else {
            filer_highlight_index--;
            if (filer_highlight_index < 0) {
                filer_highlight_index = filer_line_max / 2;
                if (filer_highlight_index >= fileList->count) {
                    filer_highlight_index = fileList->count - 1;
                    filer_index = 0;
                } else {
                    filer_index = (fileList->count - 1) - filer_highlight_index;
                }
            }
        }
    } else if (input & INPUT_DOWN) {
        if (filer_highlight_index >= filer_line_max / 2) {
            filer_index++;
            if (filer_index + filer_highlight_index >= fileList->count) {
                filer_index = 0;
                filer_highlight_index = 0;
            }
        } else {
            filer_highlight_index++;
            if (filer_highlight_index >= fileList->count) {
                filer_highlight_index = 0;
            }
        }
    } else if (input & INPUT_A) {
        File *file = get_file(filer_index + filer_highlight_index);
        if (file != NULL) {
            if (file->type == TYPE_DIR) {
                menu_filer_get_dir(file->path);
            } else if (file->type == TYPE_BIN) {
                // TODO: exec

            }
        }
    } else if (input & INPUT_B) {
        if (strlen(fileList->path) > 1) {
            char *pos = strrchr(fileList->path, '/');
            if (pos != NULL) {
                if (strlen(pos) == 1) {
                    menu_filer_get_dir("/");
                } else {
                    char prev[MAX_PATH];
                    memset(prev, 0, MAX_PATH);
                    strncpy(prev, fileList->path, strlen(fileList->path) - strlen(pos) + 1);
                    menu_filer_get_dir(prev);
                }
            }
        }
    }

    return 0;
}

void menu_run() {

    uint32 input = 0;

    draw_init();

    Vec2 screenSize = draw_get_screen_size();
    menuRect = (Rect) {32, 32, screenSize.x - 64, screenSize.y - 64};
    filerRect = (Rect) {menuRect.left + 8, menuRect.top + 8,
                        menuRect.width - 16, menuRect.height - 16};

    // init filer
    menu_filer_get_dir("/");

    while (1) {

        input = menu_filer_input();
        if (input & INPUT_QUIT) {
            break;
        }

        draw_start();

        menu_draw_bg(&menuRect);

        menu_filer_draw(&filerRect);

        draw_end();
    }

    draw_exit();
}

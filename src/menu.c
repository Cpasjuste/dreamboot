//
// Created by cpasjuste on 28/01/2020.
//

#include <string.h>
#include "uthash/utlist.h"
#include "retrodream.h"
#include "retrolog.h"
#include "input.h"
#include "drawing.h"
#include "utility.h"
#include "menu.h"

static int menu_id = MENU_FILER;
static int line_height = 0;
static int line_max = 0;
static int list_index = 0;
static int highlight_index = 0;
static Rect menuRect;
static Rect pathRect;
static Rect filerRect;
static List fileList;
extern List logList;

void menu_init_rects() {

    Vec2 screenSize = draw_get_screen_size();

    menuRect = (Rect) {32, 32, screenSize.x - 64, screenSize.y - 64};

    pathRect = (Rect) {menuRect.left + 8, menuRect.top + 8,
                       menuRect.width - 16, DRAW_FONT_HEIGHT + DRAW_FONT_LINE_SPACING};

    filerRect = (Rect) {menuRect.left + 8, pathRect.top + pathRect.height + 10,
                        menuRect.width - 16, menuRect.height - pathRect.height - 26};
}

void menu_get_dir(char *path) {

    if (menu_id != MENU_FILER) {
        return;
    }

    if (line_height == 0) {
        line_height = DRAW_FONT_HEIGHT + DRAW_FONT_LINE_SPACING;
        line_max = (int) filerRect.height / line_height;
        if (line_max * line_height < (int) filerRect.height) {
            line_height = (int) filerRect.height / line_max;
        }
    }

    list_index = 0;
    highlight_index = 0;

    free_dir(&fileList);
    get_dir(&fileList, path);
}

void menu_draw() {

    List *list = menu_id == MENU_FILER ? &fileList : &logList;

    draw_box_outline(pathRect.left, pathRect.top, pathRect.width, pathRect.height,
                     100, COL_BLUE, COL_YELLOW, 4);
    draw_string(pathRect.left + 4, pathRect.top + (DRAW_FONT_LINE_SPACING / 2), 103, COL_YELLOW, list->path);

    draw_box_outline(filerRect.left, filerRect.top, filerRect.width, filerRect.height,
                     100, COL_BLUE, COL_YELLOW, 4);

    for (int i = 0; i < (unsigned int) line_max; i++) {

        if (list_index + i < list->size) {

            if (menu_id != MENU_LOGS && i == highlight_index) {
                draw_box_outline(filerRect.left, filerRect.top + ((float) (i * line_height)),
                                 filerRect.width, (float) line_height,
                                 102, COL_RED, COL_WHITE, 2);
            }

            ListItem *item = get_item(list, list_index + i);
            if (item != NULL) {
                Color color = COL_WHITE;
                if (menu_id == MENU_FILER && item->type == TYPE_DIR) {
                    color = COL_YELLOW;
                }
                if (menu_id == MENU_LOGS && item->type == TYPE_DIR) {
                    color = COL_RED;
                }
                draw_string(filerRect.left + 4,
                            filerRect.top + (DRAW_FONT_LINE_SPACING / 2) + ((float) (i * line_height)),
                            103, color, item->name);
            }
        }
    }
}

int menu_input() {

    uint32 input = get_input();

    if (input & INPUT_QUIT) {
        return INPUT_QUIT;
    }

    if (input & INPUT_UP) {
        if (highlight_index <= line_max / 2 && list_index > 0) {
            list_index--;
        } else {
            highlight_index--;
            if (highlight_index < 0) {
                highlight_index = line_max / 2;
                if (highlight_index >= fileList.size) {
                    highlight_index = fileList.size - 1;
                    list_index = 0;
                } else {
                    list_index = (fileList.size - 1) - highlight_index;
                }
            }
        }
    } else if (input & INPUT_DOWN) {
        if (highlight_index >= line_max / 2) {
            list_index++;
            if (list_index + highlight_index >= fileList.size) {
                list_index = 0;
                highlight_index = 0;
            }
        } else {
            highlight_index++;
            if (highlight_index >= fileList.size) {
                highlight_index = 0;
            }
        }
    } else if (input & INPUT_A) {
        ListItem *file = get_item(&fileList, list_index + highlight_index);
        if (file != NULL) {
            if (file->type == TYPE_DIR) {
                menu_get_dir(file->path);
            } else if (file->type == TYPE_BIN) {
                exec(file->path);
            }
        }
    } else if (input & INPUT_B) {
        if (strlen(fileList.path) > 1) {
            char *pos = strrchr(fileList.path, '/');
            if (pos != NULL) {
                if (strlen(pos) == 1) {
                    menu_get_dir("/");
                } else {
                    char prev[MAX_PATH];
                    memset(prev, 0, MAX_PATH);
                    strncpy(prev, fileList.path, strlen(fileList.path) - strlen(pos) + 1);
                    menu_get_dir(prev);
                }
            }
        }
    }

    return 0;
}

void menu_run() {

    uint32 input = 0;

    menu_init_rects();

    // init filer
    menu_get_dir("/");

    while (1) {

        input = menu_input();
        if (input & INPUT_QUIT) {
            break;
        }

        draw_start();

        menu_draw();

        draw_end();
    }
}

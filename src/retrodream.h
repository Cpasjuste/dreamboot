//
// Created by cpasjuste on 03/02/2020.
//

#ifndef LOADER_CROSS_H
#define LOADER_CROSS_H

#ifdef __LINUX__

#include <stdio.h>
#include <zlib.h>
#include <stdint.h>

typedef uint8_t uint8;
typedef uint32_t uint32;

#define DBG_INFO 0
#define dbglog(f_, ...) printf((f_), __VA_ARGS__)
#else

#include <kos.h>
#include <stdarg.h>
#include <stdio.h>
#include "menu.h"

#define dbglog(lv, fmt, ...) menu_draw_printf(lv, fmt, ##__VA_ARGS__)

#endif

#define ROMDISK_PATH "/rd"

#endif //LOADER_CROSS_H

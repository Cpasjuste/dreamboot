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

#define DBG_DEAD        0
#define DBG_CRITICAL    1
#define DBG_ERROR       2
#define DBG_WARNING     3
#define DBG_NOTICE      4
#define DBG_INFO        5
#define DBG_DEBUG       6
#define DBG_KDEBUG      7

#else

#include <kos.h>
#include <stdarg.h>
#include <stdio.h>

#endif

#define ROMDISK_PATH "/rd"

#endif //LOADER_CROSS_H

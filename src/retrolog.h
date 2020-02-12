//
// Created by cpasjuste on 05/02/2020.
//

#ifndef DREAMBOOT_RETROLOG_H
#define DREAMBOOT_RETROLOG_H

void retro_log(int level, const char *fmt, ...);

#ifndef __DEBUG_EMU__
#define dbglog(lv, fmt, ...) retro_log(lv, fmt, ##__VA_ARGS__)
#endif

#endif //DREAMBOOT_RETROLOG_H

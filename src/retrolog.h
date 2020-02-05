//
// Created by cpasjuste on 05/02/2020.
//

#ifndef DREAMBOOT_RETROLOG_H
#define DREAMBOOT_RETROLOG_H

void retro_log(int level, const char *fmt, ...);

#define dbglog(lv, fmt, ...) retro_log(lv, fmt, ##__VA_ARGS__)

#endif //DREAMBOOT_RETROLOG_H

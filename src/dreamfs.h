//
// Created by cpasjuste on 11/02/2020.
//

#ifndef DREAMBOOT_DREAMFS_H
#define DREAMBOOT_DREAMFS_H

#ifdef __DREAMCAST__

enum DeviceType {
    Sd = 0,
    Ide = 1
};

enum DeviceFormat {
    Fat = 0,
    Ext2 = 1
};

typedef struct device_t {
    char path[16];
    int type;
    int format;
    int inited;
    int mounted;
} Device;

int dream_fs_mount(int dev);

void dream_fs_unmount(int dev);

#endif

void dream_fs_init();

void dream_fs_exit();

#endif //DREAMBOOT_DREAMFS_H

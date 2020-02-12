//
// Created by cpasjuste on 11/02/2020.
//

#if 0
#ifdef __DREAMCAST__

#include <kos.h>
#include <fat/fs_fat.h>
#include <ext2/fs_ext2.h>
#include "dreamfs.h"

static Device devices[2] = {
        (Device) {"/sd", Sd, 0, -1, -1},
        (Device) {"/ide", Ide, 0, -1, -1}
};

void dream_fs_init() {

    fs_ext2_init();
    fs_fat_init();

    //devices[Sd].inited = sd_init();
    //dream_fs_mount(Sd);

    devices[Ide].inited = g1_ata_init();
    dream_fs_mount(Ide);
}

int dream_fs_mount(int dev) {

    kos_blockdev_t deviceBlock;
    uint8 deviceType;

    Device *device = &devices[dev];
    if (device->inited != 0) {
        return -1;
    }

    if (dev == Sd) {
        if (sd_blockdev_for_partition(0, &deviceBlock, &deviceType) != 0) {
            return -1;
        }
    } else {
        if (g1_ata_blockdev_for_partition(0, 1, &deviceBlock, &deviceType) != 0) {
            if (g1_ata_blockdev_for_partition(0, 0, &deviceBlock, &deviceType) != 0) {
                return -1;
            }
        }
    }

    if (deviceType == 0x83) {
        if (fs_ext2_mount(device->path, &deviceBlock, FS_EXT2_MOUNT_READONLY) != 0) {
            return -1;
        }
        device->format = Ext2;
    } else if (deviceType == 0x04 || deviceType == 0x06 || deviceType == 0x0B || deviceType == 0x0C) {
        if (fs_fat_mount(device->path, &deviceBlock, FS_FAT_MOUNT_READONLY) != 0) {
            return -1;
        }
        device->format = Fat;
    }

    device->mounted = 0;

    return 0;
}

void dream_fs_unmount(int dev) {

    Device *device = &devices[dev];
    if (device->inited == 0 && device->mounted == 0) {
        if (device->format == Ext2) {
            fs_ext2_sync(device->path);
            fs_ext2_unmount(device->path);
        } else {
            fs_fat_sync(device->path);
            fs_fat_unmount(device->path);
        }
        device->mounted = -1;
    }
}

void dream_fs_exit() {

    if (devices[Sd].inited == 0) {
        dream_fs_unmount(Sd);
        sd_shutdown();
    }

    if (devices[Ide].inited == 0) {
        dream_fs_unmount(Ide);
        g1_ata_shutdown();
    }

    fs_fat_shutdown();
    fs_ext2_shutdown();
}

#endif
#endif

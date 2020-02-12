//
// Created by cpasjuste on 28/01/2020.
//

#include "uthash/utlist.h"
#include "retrodream.h"
#include "drawing.h"
#include "utility.h"
#include "fs.h"
#include "dreamfs.h"

KOS_INIT_FLAGS(INIT_DEFAULT);
extern uint8 romdisk[];
KOS_INIT_ROMDISK(romdisk);

void get_dir(List *list, const char *path) {

    dirent_t *ent;
    file_t fd;
    ListItem *entry;

    memset(list, 0, sizeof(List));
    strncpy(list->path, path, MAX_PATH - 1);

    if ((fd = fs_open(path, O_RDONLY | O_DIR)) != FILEHND_INVALID) {
        while ((ent = fs_readdir(fd)) != NULL) {

            // skip "."
            if (ent->name[0] == '.') {
                continue;
            }

            if (strncmp(ent->name, "pty", 3) == 0 || strncmp(ent->name, "ram", 3) == 0
                || strncmp(ent->name, "pc", 2) == 0 || strncmp(ent->name, "cd", 2) == 0) {
                continue;
            }

            entry = (ListItem *) malloc(sizeof(ListItem));
            memset(entry, 0, sizeof(ListItem));

            strncpy(entry->name, ent->name, MAX_PATH - 1);
            if (list->path[strlen(list->path) - 1] != '/') {
                snprintf(entry->path, MAX_PATH - 1, "%s/%s", list->path, ent->name);
            } else {
                snprintf(entry->path, MAX_PATH - 1, "%s%s", list->path, ent->name);
            }

            entry->type = ent->attr == O_DIR ? TYPE_DIR : TYPE_FILE;
            if (entry->type == TYPE_FILE) {
                if (strstr(entry->name, ".bin") != NULL || strstr(entry->name, ".BIN") != NULL ||
                    strstr(entry->name, ".elf") != NULL || strstr(entry->name, ".ELF") != NULL) {
                    entry->type = TYPE_BIN;
                }
            }

            DL_APPEND(list->head, entry);
            list->size++;
        }

        DL_SORT(list->head, list_cmp);
        fs_close(fd);
    }
}

int file_exists(const char *fn) {
    file_t f;

    f = fs_open(fn, O_RDONLY);

    if (f == FILEHND_INVALID) {
        return 0;
    }

    fs_close(f);
    return 1;
}

int dir_exists(const char *dir) {
    file_t f;

    f = fs_open(dir, O_DIR | O_RDONLY);

    if (f == FILEHND_INVALID) {
        return 0;
    }

    fs_close(f);
    return 1;
}

char *read_file(const char *file) {

    file_t fd;
    ssize_t size;
    char *buffer = NULL;

    fd = fs_open(file, O_RDONLY);
    if (fd == FILEHND_INVALID) {
        printf("read_file: can't open %s\n", file);
        return NULL;
    }

    size = fs_total(fd);
    buffer = (char *) malloc(size);
    memset(buffer, 0, size);

    if (fs_read(fd, buffer, size) != size) {
        fs_close(fd);
        free(buffer);
        printf("read_file: can't read %s\n", file);
        return NULL;
    }

    fs_close(fd);

    return buffer;
}

int flash_get_region() {

    int start, size;
    uint8 region[6] = {0};
    region[2] = *(uint8 *) 0x0021A002;

    /* Find the partition */
    if (flashrom_info(FLASHROM_PT_SYSTEM, &start, &size) < 0) {
        dbglog(DBG_ERROR, "%s: can't find partition %d\n", __func__, FLASHROM_PT_SYSTEM);
    } else {
        /* Read the first 5 characters of that partition */
        if (flashrom_read(start, region, 5) < 0) {
            dbglog(DBG_ERROR, "%s: can't read partition %d\n", __func__, FLASHROM_PT_SYSTEM);
        }
    }

    if (region[2] == 0x58 || region[2] == 0x30) {
        return FLASHROM_REGION_JAPAN;
    } else if (region[2] == 0x59 || region[2] == 0x31) {
        return FLASHROM_REGION_US;
    } else if (region[2] == 0x5A || region[2] == 0x32) {
        return FLASHROM_REGION_EUROPE;
    } else {
        dbglog(DBG_ERROR, "%s: Unknown region code %02x\n", __func__, region[2]);
        return FLASHROM_REGION_UNKNOWN;
    }
}

int is_hacked_bios() {
    return (*(uint16 *) 0xa0000000) == 0xe6ff;
}

int is_custom_bios() {
    return (*(uint16 *) 0xa0000004) == 0x4318;
}

int is_no_syscalls() {
    return (*(uint16 *) 0xac000100) != 0x2f06;
}

int setup_syscalls() {

    file_t fd;
    size_t fd_size;
    int (*sc)(int, int, int, int);

    dbglog(DBG_INFO, "Loading and setup syscalls...\n");
    fd = fs_open(ROMDISK_PATH"/syscalls.bin", O_RDONLY);

    if (fd != FILEHND_INVALID) {

        fd_size = fs_total(fd);
        dbgio_dev_select("null");
        dcache_flush_range(0x8c000000, fd_size);
        icache_flush_range(0x8c000000, fd_size);

        if (fs_read(fd, (uint8 *) 0x8c000000, fd_size) < 0) {
            fs_close(fd);
            //dbgio_dev_select("fb");
            dbglog(DBG_ERROR, "Error at loading syscalls\n");
            return -1;

        } else {
            fs_close(fd);
            dcache_flush_range(0x8c000000, fd_size);
            icache_flush_range(0x8c000000, fd_size);
            //dbgio_dev_select("fb");
            *((uint32 *) &sc) = *((uint32 *) 0x8c0000b0);
            if (sc(0, 0, 0, 0)) {
                dbglog(DBG_ERROR, "Error in sysinfo syscall\n");
                return -1;
            }
            dbglog(DBG_INFO, "Syscalls successfully installed\n");
            return 0;
        }
    } else {
        dbglog(DBG_ERROR, "Can't open file with syscalls\n");
    }

    return -1;
}

void exec(const char *path) {

    file_t f = fs_open(path, O_RDONLY);
    if (f == FILEHND_INVALID) {
        dbglog(DBG_INFO, "error: could not find dcload.bin\n");
        return;
    }

    void *bin = fs_mmap(f);
    arch_exec(bin, fs_total(f));
}

void dc_load_serial(void) {

    /*
    if (fs_romdisk_mount(ROMDISK_PATH, (const uint8 *) romdisk, 0) != 0) {
        dbglog(DBG_INFO, "error: could not mount romdisk\n");
        return;
    }
    */

    exec(ROMDISK_PATH"/dcload-serial.bin");

    //fs_romdisk_unmount(ROMDISK_PATH);
}

void dc_load_ip(void) {

    /*
    if (fs_romdisk_mount(ROMDISK_PATH, (const uint8 *) romdisk, 0) != 0) {
        dbglog(DBG_INFO, "error: could not mount romdisk\n");
        return;
    }
    */

    exec(ROMDISK_PATH"/dcload-ip.bin");

    //fs_romdisk_unmount(ROMDISK_PATH);
}

void loader_init() {

    /*
    if (fs_romdisk_mount(ROMDISK_PATH, (const uint8 *) romdisk, 0) == 0) {
        if (is_custom_bios()) {
            setup_syscalls();
        }
        fs_romdisk_unmount(ROMDISK_PATH);
    }
    */
    if (is_custom_bios()) {
        setup_syscalls();
    }

#ifndef __DEBUG_EMU__
    //dream_fs_init();
    InitIDE();
    InitSDCard();
#endif
}

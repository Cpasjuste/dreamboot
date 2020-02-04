//
// Created by cpasjuste on 28/01/2020.
//

#include <kos.h>
#include "utility.h"

extern FileList s_file_list;

extern void add_file(FileList *list, File *entry, int sort);

extern FileList *get_dir(const char *path);

extern void free_dir(FileList *list);

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

    if (fs_read(fd, buffer, size) != size) {
        fs_close(fd);
        free(buffer);
        printf("read_file: can't read %s\n", file);
        return NULL;
    }

    fs_close(fd);

    return buffer;
}

FileList *get_dir(const char *path) {

    dirent_t *ent;
    file_t fd;
    File *entry;

    free_dir(&s_file_list);
    memset(&s_file_list, 0, sizeof(FileList));
    strncpy(s_file_list.path, path, MAX_PATH);

    if ((fd = fs_open(path, O_RDONLY | O_DIR)) != FILEHND_INVALID) {
        while ((ent = fs_readdir(fd)) != NULL) {

            // skip "."
            if (ent->name[0] == '.') {
                continue;
            }

            entry = (File *) malloc(sizeof(File));
            strncpy(entry->name, ent->name, MAX_PATH);
            if (path[strlen(path) - 1] != '/') {
                snprintf(entry->path, MAX_PATH - 1, "%s/%s", path, ent->name);
            } else {
                snprintf(entry->path, MAX_PATH - 1, "%s%s", path, ent->name);
            }

            struct stat st;
            if (stat(entry->path, &st) == 0) {
                entry->type = S_ISDIR(st.st_mode) ? TYPE_DIR : TYPE_FILE;
            }

            add_file(&s_file_list, entry, SORT_BY_NAME_AND_FOLDER);
        }
        fs_close(fd);
    }

    return &s_file_list;
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

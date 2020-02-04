//
// Created by cpasjuste on 28/01/2020.
//

#ifndef LOADER_UTILITY_H
#define LOADER_UTILITY_H

#define MAX_PATH 512
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

enum FileSortFlags {
    SORT_NONE,
    SORT_BY_NAME_AND_FOLDER,
};

enum FileType {
    TYPE_DIR,
    TYPE_FILE,
    TYPE_BIN
};

typedef struct file_t {
    struct File *next;
    struct File *previous;
    char name[MAX_PATH];
    char path[MAX_PATH];
    int type;
} File;

typedef struct file_list_t {
    File *head;
    File *tail;
    int count;
    char path[MAX_PATH];
} FileList;

FileList *get_dir(const char *path);

File *get_file(int index);

int file_exists(const char *fn);

int dir_exists(const char *dir);

void try_boot();

char *read_file(const char *file);

void exec(const char *path);

int is_hacked_bios();

int is_custom_bios();

int is_no_syscalls();

int flash_get_region();

void descramble(uint8 *source, uint8 *dest, uint32 size);

int setup_syscalls();

void dc_load_serial();

void loader_init();

#endif //LOADER_UTILITY_H

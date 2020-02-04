//
// Created by cpasjuste on 03/02/2020.
//

#include "cross.h"
#include "drawing.h"

#ifdef __LINUX__

void loader_init() {

}

void dc_load_callback(void) {

}

#else

KOS_INIT_FLAGS(INIT_DEFAULT);


void dc_load_callback(void) {

    if (fs_romdisk_mount(RES_PATH, (const uint8 *) romdisk, 0) != 0) {
        //dbgio_dev_select("fb");
        dbglog(DBG_INFO, "error: could not mount romdisk\n");
        return;
    }

    file_t f = fs_open(RES_PATH"/dcload.bin", O_RDONLY);
    if (f == FILEHND_INVALID) {
        //dbgio_dev_select("fb");
        dbglog(DBG_INFO, "error: could not find dcload.bin\n");
        return;
    }

    draw_exit();
    void *bin = fs_mmap(f);
    arch_exec(bin, fs_total(f));
}

void loader_init() {
    //cont_btn_callback(0, CONT_START, (cont_btn_callback_t) start_callback);
    cont_btn_callback(0, CONT_A | CONT_B, (cont_btn_callback_t) dc_load_callback);
}

#endif

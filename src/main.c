/**
 * DreamShell boot loader
 * Main
 * (c)2011-2016 SWAT <http://www.dc-swat.ru>
 */

#include "cross.h"
#include "menu.h"

int main(int argc, char **argv) {

    loader_init();
    menu_run();

    return 0;
}

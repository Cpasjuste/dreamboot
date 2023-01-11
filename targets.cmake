cmake_minimum_required(VERSION 3.1)
#set(CMAKE_VERBOSE_MAKEFILE ON)

if (NOT PLATFORM_LINUX)
    # romdisk handling (tricky..)
    add_custom_target(
            ${PROJECT_NAME}.romdisk ALL
            DEPENDS dummy_romdisk
    )
    add_custom_command(OUTPUT
            dummy_romdisk ${CMAKE_BINARY_DIR}/romdisk.o
            COMMAND ${KOS_BASE}/utils/genromfs/genromfs -f ${CMAKE_BINARY_DIR}/romdisk.img -d ${CMAKE_CURRENT_SOURCE_DIR}/romdisk -v
            COMMAND KOS_ARCH=${KOS_ARCH} KOS_AS=${KOS_AS} KOS_AFLAGS=${KOS_AFLAGS} KOS_LD=${KOS_LD} KOS_OBJCOPY=${KOS_OBJCOPY}
            /bin/bash ${KOS_BASE}/utils/bin2o/bin2o ${CMAKE_BINARY_DIR}/romdisk.img romdisk ${CMAKE_BINARY_DIR}/romdisk.o
            )
    target_sources(${PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR}/romdisk.o)
    add_custom_target(${PROJECT_NAME}.bin
            DEPENDS ${PROJECT_NAME}
            COMMAND ${CMAKE_OBJCOPY} -R .stack -O binary ${PROJECT_NAME}${CMAKE_EXECUTABLE_SUFFIX} ${PROJECT_NAME}.bin
            )
    add_custom_target(${PROJECT_NAME}.bios
            DEPENDS ${PROJECT_NAME}.bin
            COMMAND cp -f ${CMAKE_SOURCE_DIR}/res/boot_loader_retail.bios ${PROJECT_NAME}.bios
            COMMAND dd if=${PROJECT_NAME}.bin of=${PROJECT_NAME}.bios bs=1 seek=65536 conv=notrunc
            )
    add_custom_target(${PROJECT_NAME}-nogdrom.bios
            DEPENDS ${PROJECT_NAME}.bin
            COMMAND cp -f ${CMAKE_SOURCE_DIR}/res/boot_loader_retail_nogdrom.bios ${PROJECT_NAME}-nogdrom.bios
            COMMAND dd if=${PROJECT_NAME}.bin of=${PROJECT_NAME}-nogdrom.bios bs=1 seek=65536 conv=notrunc
            )
    add_custom_target(${PROJECT_NAME}-devkit.bios
            DEPENDS ${PROJECT_NAME}.bin
            COMMAND cp -f ${CMAKE_SOURCE_DIR}/res/boot_loader_devkit.bios ${PROJECT_NAME}-devkit.bios
            COMMAND dd if=${PROJECT_NAME}.bin of=${PROJECT_NAME}-devkit.bios bs=1 seek=65536 conv=notrunc
            )
    add_custom_target(${PROJECT_NAME}-devkit-nogdrom.bios
            DEPENDS ${PROJECT_NAME}.bin
            COMMAND cp -f ${CMAKE_SOURCE_DIR}/res/boot_loader_devkit_nogdrom.bios ${PROJECT_NAME}-devkit-nogdrom.bios
            COMMAND dd if=${PROJECT_NAME}.bin of=${PROJECT_NAME}-devkit-nogdrom.bios bs=1 seek=65536 conv=notrunc
            )
    add_custom_target(${PROJECT_NAME}-32mb.bios
            DEPENDS ${PROJECT_NAME}.bin
            COMMAND cp -f ${CMAKE_SOURCE_DIR}/res/boot_loader_retail_32mb.bios ${PROJECT_NAME}-32mb.bios
            COMMAND dd if=${PROJECT_NAME}.bin of=${PROJECT_NAME}-32mb.bios bs=1 seek=65536 conv=notrunc
            )
    add_custom_target(${PROJECT_NAME}-nogdrom-32mb.bios
            DEPENDS ${PROJECT_NAME}.bin
            COMMAND cp -f ${CMAKE_SOURCE_DIR}/res/boot_loader_retail_nogdrom_32mb.bios ${PROJECT_NAME}-nogdrom-32mb.bios
            COMMAND dd if=${PROJECT_NAME}.bin of=${PROJECT_NAME}-nogdrom-32mb.bios bs=1 seek=65536 conv=notrunc
            )
    add_custom_target(${PROJECT_NAME}-devkit-32mb.bios
            DEPENDS ${PROJECT_NAME}.bin
            COMMAND cp -f ${CMAKE_SOURCE_DIR}/res/boot_loader_devkit_32mb.bios ${PROJECT_NAME}-devkit-32mb.bios
            COMMAND dd if=${PROJECT_NAME}.bin of=${PROJECT_NAME}-devkit-32mb.bios bs=1 seek=65536 conv=notrunc
            )
    add_custom_target(${PROJECT_NAME}-devkit-nogdrom-32mb.bios
            DEPENDS ${PROJECT_NAME}.bin
            COMMAND cp -f ${CMAKE_SOURCE_DIR}/res/boot_loader_devkit_nogdrom_32mb.bios ${PROJECT_NAME}-devkit-nogdrom-32mb.bios
            COMMAND dd if=${PROJECT_NAME}.bin of=${PROJECT_NAME}-devkit-nogdrom-32mb.bios bs=1 seek=65536 conv=notrunc
            )
    add_custom_target(${PROJECT_NAME}-nogdrom.bios-lx
            DEPENDS ${PROJECT_NAME}-nogdrom.bios
            COMMAND cp ${PROJECT_NAME}-nogdrom.bios ~/.lxdream/dcboot.rom
            )
    add_custom_target(${PROJECT_NAME}-nogdrom.bios-reicast
            DEPENDS ${PROJECT_NAME}-nogdrom.bios
            COMMAND cp ${PROJECT_NAME}-nogdrom.bios ~/.local/share/reicast/data/dc_boot.bin
            )
endif ()

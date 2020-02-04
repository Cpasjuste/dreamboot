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
            COMMAND ${CMAKE_OBJCOPY} -R .stack -O binary ${PROJECT_NAME}.elf ${PROJECT_NAME}.bin
            )
    add_custom_target(${PROJECT_NAME}.bios
            DEPENDS ${PROJECT_NAME}.bin
            COMMAND rm -f ${PROJECT_NAME}.bios
            COMMAND dd if=${CMAKE_SOURCE_DIR}/res/boot_loader_retail.bios of=${PROJECT_NAME}.bios bs=1 count=65536
            COMMAND dd if=${PROJECT_NAME}.bin of=${PROJECT_NAME}.bios bs=1 seek=65536
            COMMAND dd if=${CMAKE_SOURCE_DIR}/res/boot_loader_retail.bios of=${PROJECT_NAME}.bios bs=1 skip=`stat -c %s ${PROJECT_NAME}.bios` seek=`stat -c %s ${PROJECT_NAME}.bios`
            )
    add_custom_target(${PROJECT_NAME}-nogdrom.bios
            DEPENDS ${PROJECT_NAME}.bin
            COMMAND rm -f ${PROJECT_NAME}-nogdrom.bios
            COMMAND dd if=${CMAKE_SOURCE_DIR}/res/boot_loader_retail_nogdrom.bios of=${PROJECT_NAME}-nogdrom.bios bs=1 count=65536
            COMMAND dd if=${PROJECT_NAME}.bin of=${PROJECT_NAME}-nogdrom.bios bs=1 seek=65536
            COMMAND dd if=${CMAKE_SOURCE_DIR}/res/boot_loader_retail_nogdrom.bios of=${PROJECT_NAME}-nogdrom.bios bs=1 skip=`stat -c %s ${PROJECT_NAME}-nogdrom.bios` seek=`stat -c %s ${PROJECT_NAME}-nogdrom.bios`
            )
endif ()

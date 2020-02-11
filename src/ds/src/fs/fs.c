/** 
 * \file      fs.c
 * \brief     Filesystem
 * \date      2013-2015
 * \author    SWAT
 * \copyright	http://www.dc-swat.ru
 */

//#include "ds.h"
#include <string.h>
#include <kos.h>
#include <stdlib.h>
#include <utility.h>
#include "fs.h"
#include "drivers/sd.h"
#include "drivers/g1_ide.h"

typedef struct romdisk_hdr {
    char    magic[8];           /* Should be "-rom1fs-" */
    uint32  full_size;          /* Full size of the file system */
    uint32  checksum;           /* Checksum */
    char    volume_name[16];    /* Volume name (zero-terminated) */
} romdisk_hdr_t;

typedef struct blockdev_devdata {
    uint64_t block_count;
    uint64_t start_block;
} sd_devdata_t;

typedef struct ata_devdata {
    uint64_t block_count;
    uint64_t start_block;
    uint64_t end_block;
} ata_devdata_t;

#define MAX_PARTITIONS 4

static kos_blockdev_t sd_dev[MAX_PARTITIONS];
static kos_blockdev_t g1_dev[MAX_PARTITIONS];

static uint32 ntohl_32(const void *data) {
    const uint8 *d = (const uint8*)data;
    return (d[0] << 24) | (d[1] << 16) | (d[2] << 8) | (d[3] << 0);
}


static int check_partition(uint8 *buf, int partition) {
	int pval;
	
	if(buf[0x01FE] != 0x55 || buf[0x1FF] != 0xAA) {
//		dbglog(DBG_DEBUG, "Device doesn't appear to have a MBR\n");
		return -1;
	}
	
	pval = 16 * partition + 0x01BE;

	if(buf[pval + 4] == 0) {
//		dbglog(DBG_DEBUG, "Partition empty: 0x%02x\n", buf[pval + 4]);
		return -1;
	}
	
	return 0;
}


int InitSDCard() {
	
	dbglog(DBG_INFO, "Checking for SD card...\n");
	uint8 partition_type;
	int part = 0, fat_part = 0;
	char path[8];
	uint32 sd_block_count = 0;
	uint64 sd_capacity = 0;
	uint8 buf[512];
	
	if(sdc_init()) {
		dbglog(DBG_INFO, "SD card not found.\n");
		return -1;
	}
	
	sd_capacity = sdc_get_size();
	sd_block_count = (uint32)(sd_capacity / 512);
	
	dbglog(DBG_INFO, "SD card initialized, capacity %" PRIu32 " MB\n", (uint32)(sd_capacity / 1024 / 1024));
	
//	if(sdc_print_ident()) {
//		dbglog(DBG_INFO, "SD card read CID error\n");
//		return -1;
//	}
	
	if(sdc_read_blocks(0, 1, buf)) {
		dbglog(DBG_ERROR, "Can't read MBR from SD card\n");
		return -1;
	}
	
	for(part = 0; part < MAX_PARTITIONS; part++) {
		
		if(!check_partition(buf, part) && !sdc_blockdev_for_partition(part, &sd_dev[part], &partition_type)) {
			
			if(!part) {
				strcpy(path, "/sd");
				path[3] = '\0';
			} else {
				sprintf(path, "sd%d", part);
				path[strlen(path)] = '\0';
			}
			
			/* Check to see if the MBR says that we have a Linux partition. */
			if(is_ext2_partition(partition_type)) {
				
				dbglog(DBG_INFO, "Detected EXT2 filesystem on partition %d\n", part);
				
				if(fs_ext2_init()) {
					dbglog(DBG_INFO, "Could not initialize fs_ext2!\n");
					sd_dev[part].shutdown(&sd_dev[part]);
				} else {
					dbglog(DBG_INFO, "Mounting filesystem...\n");

					if(fs_ext2_mount(path, &sd_dev[part], FS_EXT2_MOUNT_READWRITE)) {
						dbglog(DBG_INFO, "Could not mount device as ext2fs.\n");
						sd_dev[part].shutdown(&sd_dev[part]);
					}
				}
				
			} else if((fat_part = is_fat_partition(partition_type))) {
			
				dbglog(DBG_INFO, "Detected FAT%d filesystem on partition %d\n", fat_part, part);
				
				sd_devdata_t *ddata = (sd_devdata_t *)sd_dev[part].dev_data;
//				ddata->block_count += ddata->start_block;
				ddata->block_count = sd_block_count;
				ddata->start_block = 0;
				
				if(fs_fat_init()) {
					dbglog(DBG_INFO, "Could not initialize fs_fat!\n");
					sd_dev[part].shutdown(&sd_dev[part]);
				} else {
				
					dbglog(DBG_INFO, "Mounting filesystem...\n");

					if(fs_fat_mount(path, &sd_dev[part], 0, part)) {
						dbglog(DBG_INFO, "Could not mount device as fatfs.\n");
						sd_dev[part].shutdown(&sd_dev[part]);
					}
				}
				
			} else {
				dbglog(DBG_INFO, "Unknown filesystem: 0x%02x\n", partition_type);
				sd_dev[part].shutdown(&sd_dev[part]);
			}
			
		} else {
//			dbglog(DBG_ERROR, "Could not make blockdev for partition %d, error: %d\n", part, errno);
		}
	}

	return 0;
}


int InitIDE() {
	
	dbglog(DBG_INFO, "Checking for G1 ATA devices...\n");
	uint8 partition_type;
	int part = 0, fat_part = 0;
	char path[8];
	uint64 ide_block_count = 0;
	uint8 buf[512];
	int use_dma = 1;
	
	if(g1_ata_init()) {
		//dbglog(DBG_INFO, "G1 ATA device not found.\n");
		return -1;
	}
	
//	dbglog(DBG_INFO, "G1 ATA device initialized\n");
	
	/* Read the MBR from the disk */
	if(g1_ata_max_lba() > 0) {
		if(g1_ata_read_lba(0, 1, (uint16_t *)buf) < 0) {
			dbglog(DBG_ERROR, "Can't read MBR from IDE by LBA\n");
			return -1;
		}
	} else {
		
		use_dma = 0;
		
		if(g1_ata_read_chs(0, 0, 1, 1, (uint16_t *)buf) < 0) {
			dbglog(DBG_ERROR, "Can't read MBR from IDE by CHS\n");
			return -1;
		}
	}
	
	for(part = 0; part < MAX_PARTITIONS; part++) {
		
		if(!check_partition(buf, part) && !g1_ata_blockdev_for_partition(part, use_dma, &g1_dev[part], &partition_type)) {
			
			if(!part) {
				strcpy(path, "/ide");
				path[4] = '\0';
			} else {
				sprintf(path, "/ide%d", part);
				path[strlen(path)] = '\0';
			}
			
			/* Check to see if the MBR says that we have a EXT2 or FAT partition. */
			if(is_ext2_partition(partition_type)) {
				
				dbglog(DBG_INFO, "Detected EXT2 filesystem on partition %d\n", part);
				
				if(fs_ext2_init()) {
					dbglog(DBG_INFO, "Could not initialize fs_ext2!\n");
					g1_dev[part].shutdown(&g1_dev[part]);
				} else {
					
					/* Only PIO for EXT2 */
					g1_dev[part].shutdown(&g1_dev[part]);
					if(g1_ata_blockdev_for_partition(part, 0, &g1_dev[part], &partition_type)) {
						continue;
					}
					
					dbglog(DBG_INFO, "Mounting filesystem...\n");
					if(fs_ext2_mount(path, &g1_dev[part], FS_EXT2_MOUNT_READWRITE)) {
						dbglog(DBG_INFO, "Could not mount device as ext2fs.\n");
					}
				}
				
			}  else if((fat_part = is_fat_partition(partition_type))) {
			
				dbglog(DBG_INFO, "Detected FAT%d filesystem on partition %d\n", fat_part, part);
				
				if(!ide_block_count) {
					ide_block_count = g1_ata_max_lba();
				}
				
				ata_devdata_t *ddata = (ata_devdata_t *)g1_dev[part].dev_data;
				
				if(ide_block_count > 0) {
					ddata->block_count = ide_block_count;
					ddata->end_block = ide_block_count - 1;
				} else {
					ddata->block_count += ddata->start_block;
					ddata->end_block += ddata->start_block;
				}
				
				ddata->start_block = 0;
				
				if(fs_fat_init()) {
					dbglog(DBG_INFO, "Could not initialize fs_fat!\n");
					g1_dev[part].shutdown(&g1_dev[part]);
				} else {
				
					dbglog(DBG_INFO, "Mounting filesystem...\n");

					if(fs_fat_mount(path, &g1_dev[part], use_dma, part)) {
						dbglog(DBG_INFO, "Could not mount device as fatfs.\n");
						g1_dev[part].shutdown(&g1_dev[part]);
					}
				}
				
			} else {
				dbglog(DBG_INFO, "Unknown filesystem: 0x%02x\n", partition_type);
				g1_dev[part].shutdown(&g1_dev[part]);
			}
		}
	}

	return 0;
}

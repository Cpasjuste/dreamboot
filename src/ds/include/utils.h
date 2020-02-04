/** 
 * \file    utils.h
 * \brief   DreamShell utils
 * \date    2004-2016
 * \author  SWAT www.dc-swat.ru
 */


#ifndef _DS_UTILS_H
#define _DS_UTILS_H

/* Optimized memory utils */
void *memcpy_sh4(void *dest, const void *src, size_t count);
void *memmove_sh4(void *dest, const void *src, size_t count);
void *memset_sh4(void *dest, uint32 val, size_t count);

#define memcpy  memcpy_sh4
#define memmove memmove_sh4
#define memset  memset_sh4

#endif /*_DS_UTILS_H */

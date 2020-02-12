//
// Created by cpasjuste on 12/02/2020.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bmfont.h"

int bmf_parse(const char *fntPath, BMFont *bmFont) {

    FILE *fd = fopen(fntPath, "r");
    if (fd == NULL) {
        return -1;
    }

    char *lineBuf = (char *) malloc(BMF_MAX_PATH);

    /// parse info
    if (fgets(lineBuf, BMF_MAX_PATH, fd) == NULL) {
        fclose(fd);
        free(lineBuf);
        return -1;
    }
    // TODO: fix empty "charset"
    if (sscanf(lineBuf,
               "info face=\"%[^\"]\" size=%d bold=%d italic=%d charset=\"%[^\"]\" unicode=%d stretchH=%d smooth=%d aa=%d padding=%d,%d,%d,%d spacing=%d,%d outline=%d\n",
               &bmFont->info.face, &bmFont->info.size, &bmFont->info.bold, &bmFont->info.italic, &bmFont->info.charset,
               &bmFont->info.unicode, &bmFont->info.stretchH, &bmFont->info.smooth, &bmFont->info.aa,
               &bmFont->info.padding[0], &bmFont->info.padding[1], &bmFont->info.padding[2], &bmFont->info.padding[3],
               &bmFont->info.spacing[0], &bmFont->info.spacing[1], &bmFont->info.outline) != 16) {
        fclose(fd);
        free(lineBuf);
        printf("sscanf failed on info\n");
        return -1;
    }

    /// parse common
    if (fgets(lineBuf, BMF_MAX_PATH, fd) == NULL) {
        fclose(fd);
        free(lineBuf);
        return -1;
    }
    if (sscanf(lineBuf,
               "common lineHeight=%d base=%d scaleW=%d scaleH=%d pages=%d packed=%d alphaChnl=%d redChnl=%d greenChnl=%d blueChnl=%d\n",
               &bmFont->common.lineHeight, &bmFont->common.base,
               &bmFont->common.scaleW, &bmFont->common.scaleH,
               &bmFont->common.pages, &bmFont->common.packed,
               &bmFont->common.alphaChnl,
               &bmFont->common.redChnl, &bmFont->common.greenChnl, &bmFont->common.blueChnl) != 10) {
        fclose(fd);
        free(lineBuf);
        printf("sscanf failed on common\n");
        return -1;
    }

    /// parse page
    if (fgets(lineBuf, BMF_MAX_PATH, fd) == NULL) {
        fclose(fd);
        free(lineBuf);
        return -1;
    }
    if (sscanf(lineBuf, "page id=%d file=\"%[^\"]\"\n", &bmFont->page.id, &bmFont->page.file) != 2) {
        fclose(fd);
        free(lineBuf);
        printf("sscanf failed on page\n");
        return -1;
    }

    /// parse chars count
    if (fgets(lineBuf, BMF_MAX_PATH, fd) == NULL) {
        fclose(fd);
        free(lineBuf);
        return -1;
    }
    if (sscanf(lineBuf, "chars count=%d\n", &bmFont->charsCount) != 1) {
        fclose(fd);
        free(lineBuf);
        printf("sscanf failed on chars count\n");
        return -1;
    }

    /// parse chars
    char str_id[3];
    int id;

    if (bmFont->charsCount > BMF_MAX_CHAR) {
        bmFont->charsCount = BMF_MAX_CHAR;
    }

    for (int i = 0; i < bmFont->charsCount; i++) {
        if (fgets(lineBuf, BMF_MAX_PATH, fd) == NULL) {
            fclose(fd);
            free(lineBuf);
            return -1;
        }
        // extract for id
        char *pos = strchr(lineBuf, '=') + 1;
        if (pos == NULL) {
            continue;
        }

        memset(str_id, 0, 3);
        strncpy(str_id, pos, 3);
        id = atoi(str_id);
        if (id >= BMF_MAX_CHAR) {
            continue;
        }

        if (sscanf(lineBuf,
                   "char id=%d    x=%d    y=%d    width=%d     height=%d     xoffset=%d    yoffset=%d    xadvance=%d     page=%d  chnl=%d\n",
                   &bmFont->chars[id].id, &bmFont->chars[id].x, &bmFont->chars[id].y,
                   &bmFont->chars[id].width, &bmFont->chars[id].height,
                   &bmFont->chars[id].xoffset, &bmFont->chars[id].yoffset, &bmFont->chars[id].xadvance,
                   &bmFont->chars[id].page, &bmFont->chars[id].chnl) != 10) {
            fclose(fd);
            free(lineBuf);
            printf("sscanf failed on char[%i]\n", i);
            return -1;
        }
    }

    fclose(fd);
    free(lineBuf);

    return 0;
}

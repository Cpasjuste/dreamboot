//
// Created by cpasjuste on 12/02/2020.
//

#ifndef BMFONTLIB_BMFONT_H
#define BMFONTLIB_BMFONT_H

#define BMF_MAX_PATH 512
#define BMF_MAX_CHAR 128

typedef struct bmf_font_info_s {
    char face[BMF_MAX_PATH];
    int size;
    int bold;
    int italic;
    char charset[BMF_MAX_PATH];
    int unicode;
    int stretchH;
    int smooth;
    int aa;
    int padding[4];
    int spacing[2];
    int outline;
} BMFontInfo;

typedef struct bmf_font_common_s {
    int lineHeight;
    int base;
    int scaleW;
    int scaleH;
    int pages;
    int packed;
    int alphaChnl;
    int redChnl;
    int greenChnl;
    int blueChnl;
} BMFontCommon;

typedef struct bmf_font_page_s {
    int id;
    char file[BMF_MAX_PATH];

} BMFontPage;

typedef struct bmf_font_char_s {
    int id;
    int x;
    int y;
    int width;
    int height;
    int xoffset;
    int yoffset;
    int xadvance;
    int page;
    int chnl;
} BMFontChar;

typedef struct bmf_font_kerning_s {
    int first;
    int second;
    int amount;
} BMFontKerning;

typedef struct bmf_font_s {
    BMFontInfo info;
    BMFontCommon common;
    BMFontPage page;
    int charsCount;
    BMFontChar chars[BMF_MAX_CHAR];
    //BMFontKerning kernings[4096];
} BMFont;

int bmf_parse(const char *fntPath, BMFont *bmFont);

#endif //BMFONTLIB_BMFONT_H

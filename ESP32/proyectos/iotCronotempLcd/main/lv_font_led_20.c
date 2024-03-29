#include "lvgl/lvgl.h"

/*******************************************************************************
 * Size: 20 px
 * Bpp: 4
 * Opts: 
 ******************************************************************************/

#ifndef LV_FONT_LED_20
#define LV_FONT_LED_20 1
#endif

#if LV_FONT_LED_20

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t gylph_bitmap[] = {
    /* U+25 "%" */
    0x0, 0xff, 0xe0, 0x96, 0xfa, 0x80, 0xf, 0x44,
    0x3, 0x4f, 0x7d, 0x80, 0x2c, 0x90, 0x3, 0xff,
    0x8b, 0x26, 0x1a, 0x1, 0xfd, 0x26, 0x1a, 0x0,
    0xe2, 0x40, 0xf, 0xe6, 0x9e, 0x10, 0xd, 0x3d,
    0xcb, 0xe, 0x6, 0x0, 0xe2, 0xde, 0x50, 0xe,
    0x5e, 0xd2, 0x0, 0xee, 0x1, 0xb, 0xee, 0x40,
    0x6, 0x5b, 0xa4, 0x4, 0x1, 0xd0, 0xd, 0xe4,
    0x20, 0x1f, 0xfc, 0x34, 0x1, 0xd0, 0xd, 0xa0,
    0xc0, 0x8, 0xee, 0x40, 0x6, 0x7e, 0x20, 0x3,
    0x76, 0x90,

    /* U+2D "-" */
    0xb, 0xff, 0xda, 0x20, 0xe0, 0x1e,

    /* U+2E "." */
    0xa, 0xd5, 0x6, 0x23,

    /* U+30 "0" */
    0x0, 0x47, 0xfd, 0x60, 0x13, 0x88, 0xf4, 0x80,
    0xc1, 0x77, 0x44, 0xc8, 0x60, 0x1f, 0x8, 0x7,
    0xf1, 0x13, 0x0, 0x2c, 0x17, 0xc, 0x70, 0x9,
    0x3c, 0x83, 0x20, 0x2, 0x5f, 0x22, 0x8c, 0x2,
    0xd1, 0x70, 0xf, 0xe3, 0x0, 0xf8, 0x45, 0x5,
    0xdd, 0x13, 0xa0, 0x38, 0x8f, 0x40, 0x0,

    /* U+31 "1" */
    0x1d, 0xf3, 0x0, 0x86, 0xc, 0x77, 0x49, 0x0,
    0x1f, 0xfc, 0x1, 0x2, 0x0, 0x16, 0x48, 0x0,
    0x79, 0x80, 0x4, 0x4d, 0x0, 0xfe, 0x10, 0x0,
    0x81, 0x80, 0xa, 0xa4,

    /* U+32 "2" */
    0x9, 0xff, 0xcc, 0x0, 0x11, 0xef, 0x50, 0x8e,
    0xec, 0x94, 0x1, 0xff, 0xce, 0xcf, 0xf9, 0x2c,
    0x69, 0xc4, 0x6d, 0x47, 0x5d, 0xee, 0x90, 0x4,
    0x3, 0xe1, 0x0, 0xf9, 0x99, 0xbd, 0xd6, 0x98,
    0x4b, 0x88, 0xc4, 0x0,

    /* U+33 "3" */
    0x1d, 0xff, 0xc6, 0x0, 0x11, 0xc5, 0x46, 0x39,
    0xdd, 0x63, 0x78, 0x7, 0xff, 0x24, 0x41, 0xc0,
    0xd, 0xff, 0x3c, 0x80, 0xc, 0x44, 0x44, 0x10,
    0x2, 0xf7, 0x31, 0x7c, 0x3, 0xff, 0x88, 0x39,
    0xdd, 0x63, 0x78, 0x8, 0xe2, 0xa3,

    /* U+34 "4" */
    0x1, 0x0, 0xe1, 0x5e, 0x30, 0xa, 0xf7, 0x40,
    0xc0, 0x12, 0x98, 0x7, 0xff, 0x13, 0xc0, 0x3c,
    0x2c, 0xce, 0xeb, 0x5e, 0xc6, 0x18, 0x44, 0x40,
    0x20, 0x3b, 0xff, 0x3d, 0x0, 0x7c, 0x20, 0x1f,
    0xfc, 0xa2, 0x30, 0xf, 0x6e, 0x80,

    /* U+35 "5" */
    0x6c, 0xc7, 0xfe, 0x30, 0x55, 0x38, 0x8c, 0x42,
    0xa, 0xdb, 0xdd, 0x69, 0x0, 0x42, 0x1, 0xff,
    0xc3, 0x30, 0x10, 0xf, 0x2c, 0xd7, 0xfc, 0xc0,
    0x13, 0x30, 0x44, 0x5e, 0xe0, 0x15, 0x77, 0x35,
    0x20, 0x3, 0xff, 0x91, 0x1d, 0xd9, 0x24, 0x0,
    0x23, 0xde, 0xc0,

    /* U+36 "6" */
    0x0, 0x67, 0xfc, 0xc0, 0x14, 0xb8, 0x8c, 0x60,
    0x6, 0x66, 0x77, 0x4a, 0x1, 0xff, 0xc8, 0x30,
    0x10, 0xf, 0x2c, 0xd7, 0xfc, 0xe0, 0x17, 0xb8,
    0x88, 0xb5, 0xc1, 0x99, 0x9d, 0xcd, 0x78, 0x0,
    0xff, 0xe3, 0xb3, 0x33, 0xba, 0x49, 0x0, 0x4b,
    0x88, 0xde, 0xc0,

    /* U+37 "7" */
    0x4f, 0xfd, 0x20, 0x7, 0x21, 0x18, 0xe0, 0xf,
    0x7b, 0xa3, 0x74, 0x0, 0xe3, 0x0, 0xff, 0xe1,
    0x98, 0x7, 0x8c, 0xcc, 0x80, 0x1a, 0x16, 0xc0,
    0x3f, 0xfa, 0x66, 0x1, 0xd3, 0x21, 0x0,

    /* U+38 "8" */
    0x0, 0x57, 0xfd, 0x0, 0x14, 0x0, 0x8e, 0x70,
    0x47, 0x2e, 0xe8, 0xa0, 0x80, 0x2, 0x1, 0xff,
    0xc5, 0x10, 0x8, 0x40, 0xd2, 0x4f, 0xfe, 0xa,
    0x10, 0xf0, 0x11, 0x83, 0x41, 0x18, 0xfb, 0xa1,
    0x72, 0x0, 0x8, 0x7, 0xe1, 0x0, 0xf2, 0x31,
    0x77, 0x44, 0xe4, 0x12, 0x2, 0x3a, 0x0,

    /* U+39 "9" */
    0x0, 0x5f, 0xfc, 0xe0, 0x14, 0x80, 0x8d, 0xcc,
    0xc, 0xcc, 0xee, 0x96, 0x40, 0x3e, 0x10, 0x10,
    0xf, 0xfe, 0x18, 0x80, 0xb3, 0xdf, 0xfc, 0xd4,
    0x0, 0x86, 0x11, 0x11, 0x98, 0x2, 0xae, 0xe6,
    0xac, 0x80, 0x7f, 0x8, 0x7, 0xe1, 0x0, 0x57,
    0x74, 0xb2, 0x1, 0x30, 0x8d, 0xcc, 0x0,

    /* U+43 "C" */
    0x6, 0xff, 0xd6, 0xd, 0xe2, 0x38, 0x82, 0x53,
    0xbb, 0x48, 0x7, 0xff, 0x15, 0x10, 0x1, 0xea,
    0x90, 0xf, 0x6d, 0x0, 0x78, 0xdc, 0x3, 0xff,
    0x95, 0x49, 0xdd, 0xa4, 0x17, 0xc4, 0x71, 0x0,

    /* U+63 "c" */
    0x0, 0x57, 0xfb, 0x88, 0x25, 0x80, 0x6, 0x64,
    0x66, 0x7f, 0xb4, 0x58, 0x34, 0x3, 0x16, 0x38,
    0x6, 0x1d, 0x60, 0xc, 0xa3, 0x80, 0x19, 0x1d,
    0xbf, 0xda, 0x21, 0xc, 0x0, 0x33, 0x0,

    /* U+BA "º" */
    0x9, 0xf8, 0x2, 0xf, 0x11, 0x0, 0x80, 0x44,
    0x3e, 0x22
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 218, .box_w = 13, .box_h = 15, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 74, .adv_w = 128, .box_w = 8, .box_h = 2, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 80, .adv_w = 68, .box_w = 4, .box_h = 2, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 84, .adv_w = 147, .box_w = 9, .box_h = 13, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 131, .adv_w = 93, .box_w = 5, .box_h = 13, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 159, .adv_w = 141, .box_w = 8, .box_h = 13, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 195, .adv_w = 139, .box_w = 8, .box_h = 13, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 233, .adv_w = 144, .box_w = 8, .box_h = 14, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 271, .adv_w = 142, .box_w = 9, .box_h = 13, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 314, .adv_w = 143, .box_w = 9, .box_h = 13, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 357, .adv_w = 131, .box_w = 8, .box_h = 13, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 388, .adv_w = 148, .box_w = 9, .box_h = 13, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 435, .adv_w = 142, .box_w = 9, .box_h = 13, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 482, .adv_w = 134, .box_w = 8, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 514, .adv_w = 110, .box_w = 7, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 545, .adv_w = 81, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 9}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/

static const uint16_t unicode_list_0[] = {
    0x0, 0x8, 0x9, 0xb, 0xc, 0xd, 0xe, 0xf,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x1e, 0x3e, 0x95
};

/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 37, .range_length = 150, .glyph_id_start = 1,
        .unicode_list = unicode_list_0, .glyph_id_ofs_list = NULL, .list_length = 16, .type = LV_FONT_FMT_TXT_CMAP_SPARSE_TINY
    }
};



/*--------------------
 *  ALL CUSTOM DATA
 *--------------------*/

/*Store all the custom data of the font*/
static lv_font_fmt_txt_dsc_t font_dsc = {
    .glyph_bitmap = gylph_bitmap,
    .glyph_dsc = glyph_dsc,
    .cmaps = cmaps,
    .kern_dsc = NULL,
    .kern_scale = 0,
    .cmap_num = 1,
    .bpp = 4,
    .kern_classes = 0,
    .bitmap_format = 1
};


/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
lv_font_t lv_font_led_20 = {
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 15,          /*The maximum line height required by the font*/
    .base_line = 1,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0)
    .underline_position = -3,
    .underline_thickness = 0,
#endif
    .dsc = &font_dsc           /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
};



#endif /*#if LV_FONT_LED_20*/


#if defined(LV_LVGL_H_INCLUDE_SIMPLE)
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif


#ifndef LV_ATTRIBUTE_MEM_ALIGN
#define LV_ATTRIBUTE_MEM_ALIGN
#endif

#ifndef LV_ATTRIBUTE_IMG_HUMIDITY
#define LV_ATTRIBUTE_IMG_HUMIDITY
#endif

const LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_LARGE_CONST LV_ATTRIBUTE_IMG_HUMIDITY uint8_t humidity_map[] = {
  0x04, 0x02, 0x04, 0xff, 	/*Color of index 0*/
  0xc9, 0x8a, 0x4a, 0xff, 	/*Color of index 1*/
  0xfc, 0x82, 0x84, 0xff, 	/*Color of index 2*/
  0xf8, 0xca, 0x98, 0xff, 	/*Color of index 3*/
  0xfc, 0xfe, 0x04, 0xff, 	/*Color of index 4*/
  0xda, 0xa3, 0x69, 0xff, 	/*Color of index 5*/
  0xf9, 0xe9, 0xd7, 0xff, 	/*Color of index 6*/
  0xcd, 0x8d, 0x54, 0xff, 	/*Color of index 7*/
  0xec, 0xd1, 0xb6, 0xff, 	/*Color of index 8*/
  0xec, 0xba, 0x87, 0xff, 	/*Color of index 9*/
  0xfc, 0xf2, 0xdf, 0xff, 	/*Color of index 10*/
  0xfa, 0xdf, 0xbb, 0xff, 	/*Color of index 11*/
  0xc0, 0x96, 0x40, 0xff, 	/*Color of index 12*/
  0xe5, 0xb1, 0x7a, 0xff, 	/*Color of index 13*/
  0xd0, 0x96, 0x59, 0xff, 	/*Color of index 14*/
  0xfc, 0xfe, 0xfb, 0xff, 	/*Color of index 15*/

  0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x11, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x11, 0x77, 0xe5, 0xe7, 0x11, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x11, 0xed, 0x33, 0x33, 0x33, 0x39, 0xe1, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x01, 0x7d, 0x33, 0x9d, 0x5e, 0x55, 0xd3, 0x39, 0x71, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x1e, 0x33, 0xde, 0x96, 0xaf, 0xa6, 0x35, 0xd3, 0x35, 0x10, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x01, 0xe3, 0x35, 0x3f, 0xff, 0xff, 0xff, 0xff, 0x8e, 0x93, 0x51, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x17, 0x33, 0xe6, 0xff, 0x8a, 0xa6, 0xaa, 0xff, 0xfa, 0x59, 0x3e, 0x10, 0x00, 0x00, 0x00, 
  0x00, 0x1d, 0x35, 0x6f, 0xf6, 0x1d, 0xbb, 0xbb, 0xba, 0xff, 0xae, 0x39, 0x14, 0x00, 0x00, 0x00, 
  0x01, 0xe3, 0xd3, 0xff, 0xab, 0x51, 0xaf, 0xfa, 0xbb, 0x6f, 0xfb, 0x53, 0x51, 0x00, 0x00, 0x00, 
  0x01, 0xd3, 0xef, 0xfa, 0xb6, 0xb1, 0x3f, 0xff, 0xf6, 0xba, 0xff, 0x59, 0x97, 0x00, 0x00, 0x00, 
  0x01, 0x39, 0x9f, 0xfb, 0xbf, 0xf5, 0x7f, 0xff, 0xff, 0xbb, 0xff, 0x8d, 0x37, 0x10, 0x00, 0x00, 
  0x27, 0x3d, 0x6f, 0xab, 0xaf, 0xfb, 0x13, 0xff, 0xff, 0xab, 0xaf, 0xf5, 0x35, 0x10, 0x00, 0x00, 
  0x1e, 0x35, 0xaf, 0xab, 0xff, 0xff, 0xe1, 0xdf, 0xff, 0xfb, 0x6f, 0xfe, 0x35, 0x10, 0x00, 0x00, 
  0x15, 0x35, 0xff, 0x6b, 0xff, 0xff, 0x73, 0x5a, 0xff, 0xfb, 0xbf, 0xf5, 0x3d, 0x10, 0x00, 0x00, 
  0x1e, 0x35, 0xaf, 0xab, 0xff, 0xff, 0xde, 0x7f, 0xff, 0xfb, 0x6f, 0xfe, 0x35, 0x10, 0x00, 0x00, 
  0xc7, 0x35, 0x6f, 0xab, 0xaf, 0xff, 0xf8, 0x18, 0xff, 0xab, 0xaf, 0xfe, 0x35, 0x10, 0x00, 0x00, 
  0x01, 0x3d, 0x3f, 0xfb, 0xbf, 0xff, 0xff, 0xee, 0xff, 0xbb, 0xff, 0xbd, 0x37, 0x1c, 0x00, 0x00, 
  0x01, 0x93, 0x5f, 0xfa, 0xba, 0xff, 0xff, 0x6a, 0xfa, 0xba, 0xff, 0xd9, 0x97, 0xd7, 0x00, 0x00, 
  0x01, 0xe3, 0xd8, 0xff, 0xaf, 0xff, 0xff, 0xff, 0xff, 0xaf, 0xf6, 0x53, 0xe3, 0xad, 0x10, 0x00, 
  0x00, 0x19, 0x3e, 0xaf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf5, 0x9d, 0x5a, 0xa6, 0xe1, 0x00, 
  0x00, 0x17, 0x39, 0x5a, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x59, 0x37, 0x6a, 0xaa, 0x87, 0x00, 
  0x00, 0x01, 0x53, 0x9e, 0xbf, 0xff, 0xff, 0xff, 0xff, 0x65, 0x93, 0xe3, 0xaa, 0xaa, 0xa5, 0x10, 
  0x00, 0x00, 0x15, 0x33, 0x55, 0x8f, 0xff, 0xff, 0xbd, 0x59, 0x35, 0x7a, 0xaa, 0xaa, 0xa6, 0x72, 
  0x00, 0x00, 0x01, 0xe9, 0x39, 0xd5, 0xe5, 0xee, 0xd9, 0x33, 0x57, 0x3a, 0xaa, 0xaa, 0xaa, 0xd1, 
  0x00, 0x00, 0x00, 0x11, 0x59, 0x33, 0x33, 0x33, 0x33, 0x51, 0x17, 0xaa, 0xaa, 0xaa, 0xaa, 0xbe, 
  0x00, 0x00, 0x00, 0x04, 0x17, 0x75, 0x5d, 0x55, 0xe7, 0x11, 0x15, 0xaa, 0xaa, 0xaa, 0xaa, 0xae, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0xc1, 0x11, 0x11, 0x10, 0x00, 0x15, 0xaa, 0xaa, 0xaa, 0xaa, 0xae, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1e, 0xaa, 0xaa, 0xaa, 0xaa, 0xae, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xba, 0xaa, 0xaa, 0xaa, 0x37, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xea, 0xaa, 0xaa, 0xa6, 0x71, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x8a, 0xaa, 0x87, 0x10, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x77, 0xe7, 0x71, 0x00, 
};

const lv_img_dsc_t humidity = {
  .header.always_zero = 0,
  .header.w = 32,
  .header.h = 32,
  .data_size = 576,
  .header.cf = LV_IMG_CF_INDEXED_4BIT,
  .data = humidity_map,
};


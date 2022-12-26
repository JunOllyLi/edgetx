/**
 * @file lv_templ.h
 *
 */

#ifndef SH1107_H
#define SH1107_H

/*********************
 *      INCLUDES
 *********************/
#include <stdbool.h>

#define OLED_W 128
#define OLED_H 64

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif
#include "../lvgl_helpers.h"

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void sh1107_init(void);
void sh1107_flush(lv_disp_drv_t * drv, const lv_area_t * area, lv_color_t * color_map);

/**********************
 *      MACROS
 **********************/

#endif /*SH1107_H*/

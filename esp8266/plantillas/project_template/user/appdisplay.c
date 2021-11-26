/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "appdisplay.h"



void displayBorrarLcd() {
    wlcd_rect_fill(0,0, WLCD_WIDTH, WLCD_HEIGHT, 0);
}

uint8 displayInicializarLcd() {
    
    return wlcd_init();
}


enum OP_DISPLAY displayPintarWifi(enum OP_DISPLAY op) {
    
    uint8_t *datos;
    

    wlcd_rect_fill(WLCD_WIDTH - ICON_WIDTH,BAR_ICON_HEIGHT, ICON_WIDTH, ICON_HEIGHT, 0);
    
    
    switch(op) {
        case PAINT:
            datos = os_malloc(WFOF_SIZE_WIFI_PNG_BMP_16BPP);
            wfof_get_file_data_fast(WFOF_IDX_WIFI_PNG_BMP_16BPP, (uint32_t*) datos, 0, WFOF_SIZE_WIFI_PNG_BMP_16BPP);
            wlcd_img_draw(datos, WLCD_WIDTH - ICON_WIDTH, BAR_ICON_HEIGHT);
            free(datos);
            return PAINTED;
            break;
        case NO_PAINT:
            datos = os_malloc(WFOF_SIZE_WIFI_OFF_PNG_BMP_16BPP);
            wfof_get_file_data_fast(WFOF_IDX_WIFI_OFF_PNG_BMP_16BPP, (uint32_t*) datos, 0, WFOF_SIZE_WIFI_OFF_PNG_BMP_16BPP);
            //wlcd_rect_fill(WLCD_WIDTH - ICON_WIDTH,BAR_ICON_HEIGHT, ICON_WIDTH, ICON_HEIGHT, 0);
            wlcd_img_draw(datos, WLCD_WIDTH - ICON_WIDTH, BAR_ICON_HEIGHT);
            free(datos);
            return NO_PAINTED;
            break;
        case NO_ACTION:
            return NO_ACTION;
            break;
        case TRANSIENT:
            return TRANSIENT;
            break;
            
    }
    
    
    
}

enum OP_DISPLAY displayPintarBroker(struct DATOS_APLICACION *datosApp, enum OP_DISPLAY op) {
    
    uint8_t *datos;
    
    if (op == PAINT) datos = os_malloc(WFOF_SIZE_LINK_PNG_BMP_16BPP);
    
    
    switch(op) {
        case PAINT:
            wfof_get_file_data_fast(WFOF_IDX_LINK_PNG_BMP_16BPP, (uint32_t*) datos, 0, WFOF_SIZE_LINK_PNG_BMP_16BPP);
            wlcd_img_draw(datos, WLCD_WIDTH - (ICON_WIDTH * 2 ) - SPACE_ICON, BAR_ICON_HEIGHT);
            free(datos);
            return PAINTED;
            break;
        case NO_PAINT:
            wlcd_rect_fill(WLCD_WIDTH - (ICON_WIDTH * 2 ) - SPACE_ICON,BAR_ICON_HEIGHT, ICON_WIDTH, ICON_HEIGHT, 0);
            return NO_PAINTED;
            break;
        case NO_ACTION:
            return NO_ACTION;
            break;
        case TRANSIENT:
            return TRANSIENT;
            break;
            
    }
    
    
    
}

enum OP_DISPLAY displayPintarReloj(struct DATOS_APLICACION *datosApp, enum OP_DISPLAY op) {
    
    uint8_t *datos;
    char hora[8];
    uint16_t width, height;
    
    //printf("pintando reloj\n");
    wlcd_text_draw_settings_struct formatoTexto;
    
    formatoTexto.Color.u32t = WLCD_RGB_TO_COLOR(0xFF, 0x5C, 0x20);
    formatoTexto.BoldAdd = 0;
    formatoTexto.FontIdx = 3;
    formatoTexto.FontZoomAdd = 0;
    formatoTexto.MaxH = 50;
    formatoTexto.HSpc = 0;
    formatoTexto.MaxW = WLCD_WIDTH;
    formatoTexto.VSpc = 0;
    formatoTexto.WrapStyle = WLCD_WRAP_NONE;
    formatoTexto.X= WLCD_WIDTH/4;
    formatoTexto.Y = BAR_ICON_HEIGHT;
    
    
    
    
    switch(op) {
        case PAINT:
            if (datosApp->clock.estado == EN_HORA) {
                sprintf(hora, "%02d:%02d:%02d", datosApp->clock.date.tm_hour, datosApp->clock.date.tm_min, datosApp->clock.date.tm_sec);
                formatoTexto.Color.u32t = WLCD_RGB_TO_COLOR(0x00, 0xFF, 0x2B);
            } else {
                if (datosApp->clock.estado != EN_HORA) {
                    if (datosApp->clock.timeValid == true) {
                        sprintf(hora, "%02d:%02d:%02d", datosApp->clock.date.tm_hour, datosApp->clock.date.tm_min, datosApp->clock.date.tm_sec);
                        formatoTexto.Color.u32t = WLCD_RGB_TO_COLOR(0xFF, 0xF91, 0x00);
                    } else {
                        strcpy(hora, "--------");
                        formatoTexto.Color.u32t = WLCD_RGB_TO_COLOR(0xFF, 0xF91, 0x00);
                    }
                }
            }
            //Borramos la parte de texto que vamos a ocupar
            //wlcd_rect_fill(formatoTexto.X, formatoTexto.Y, wlcd_text_nchars_width(12, &formatoTexto), wlcd_text_nrows_height(1, &formatoTexto), 0);
            
            wlcd_text_measure(hora, &formatoTexto, &width, &height);
            wlcd_rect_fill(formatoTexto.X, formatoTexto.Y, width, height,0);
            wlcd_text_draw(hora, &formatoTexto);
            
            return PAINTED;
            break;
        case NO_PAINT:
            wlcd_rect_fill(WLCD_WIDTH - (ICON_WIDTH * 2 ) - SPACE_ICON,BAR_ICON_HEIGHT, ICON_WIDTH, ICON_HEIGHT, 0);
            return NO_PAINTED;
            break;
        case NO_ACTION:
            return NO_ACTION;
            break;
        case TRANSIENT:
            return TRANSIENT;
            break;
            
    }
    
    
    
}

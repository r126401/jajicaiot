/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   appdisplay.h
 * Author: emilio
 *
 * Created on 19 de octubre de 2017, 21:34
 */

#ifndef APPDISPLAY_H
#define APPDISPLAY_H

#ifdef __cplusplus
extern "C" {
#endif

#include "user_config.h"    
#include "wfof.h"
#include "wfof_idxs.h"
#include "wlcd.h"

#define ICON_WIDTH 32
#define ICON_HEIGHT 32
#define SPACE_ICON 3
#define BAR_ICON_HEIGHT 0
    
    
enum OP_DISPLAY {
    NO_ACTION,
    PAINT,
    NO_PAINT,
    TRANSIENT,
    PAINTED,
    NO_PAINTED
    
}OP_DISPLAY; 

uint8 displayInicializarLcd();
enum OP_DISPLAY displayPintarWifi(enum OP_DISPLAY op);
enum OP_DISPLAY displayPintarBroker(struct DATOS_APLICACION *datosApp, enum OP_DISPLAY op);
enum OP_DISPLAY displayPintarReloj(struct DATOS_APLICACION *datosApp, enum OP_DISPLAY op);

void displayBorrarLcd();


#ifdef __cplusplus
}
#endif

#endif /* APPDISPLAY_H */


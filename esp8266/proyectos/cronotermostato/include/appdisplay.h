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
#include "wifi.h"
#include "configuracion.h"

#define ICON_WIDTH 32
#define ICON_HEIGHT 32
#define SPACE_ICON 3
#define BAR_ICON_HEIGHT 0
#define ICON_SETTINGS_WIDTH 72
#define ICON_SETTINGS_HEIGHT 72
    
#define VERDE WLCD_RGB_TO_COLOR(0x00, 0xFF, 0x2B)
#define ROJO WLCD_RGB_TO_COLOR(0xCC, 0x00, 0x00)
#define AMARILLO WLCD_RGB_TO_COLOR(0xFF, 0xFF, 0x00)
#define AZUL WLCD_RGB_TO_COLOR(0x00, 0x00, 0xFF)
#define BLANCO WLCD_RGB_TO_COLOR(0xFF, 0xFF, 0xFF)    

#define ICON_SETTINGS_X     30
#define ICON_SETTINGS_Y     40
#define TITULO_X 70
#define TITULO_Y 5
#define OPCIONES_X 20
#define OPCIONES_Y 200
#define IMAGEN_SETTINGS_X 130
#define IMAGEN_SETTINGS_Y 40
    
    

    
    
enum OP_DISPLAY {
    NO_ACTION,
    PAINT,
    NO_PAINT,
    TRANSIENT,
    PAINTED,
    NO_PAINTED,
    
}OP_DISPLAY; 


enum ELEMENTOS_DISPLAY {
    WIFI_ON,
    WIFI_OFF,
    WIFI_CONNECTING,
    BROKER_ON,
    BROKER_CONNECTING,
    BROKER_OFF,
    CLOCK,
    TRANSMITING,
    RECEIVING,
    THERMOMETER,
    HUMIDITY,
    TEMPERATURE,
    HEATING,
    MODE_WORK,
    UMBRAL_TEMP,
    CLOCK_SETTINGS,
    CLOCK_SETTINGS_DAY,
    CLOCK_SETTINGS_MONTH,
    CLOCK_SETTINGS_YEAR,
    CLOCK_SETTINGS_HOUR,
    CLOCK_SETTINGS_MIN,
    CLOCK_SETTINGS_CONFIRM,
    SMARTCONFIG_SETTINGS,
    SMARTCONFIG_FIND_CHANNEL,
    RESET_SETTINGS,
    FACTORY_RESET_SETTINGS,
    FACTORY_RESET_OK,
    NORMAL_LCD
    
};

typedef struct MSG_LCD {
    enum ELEMENTOS_DISPLAY lcdStat;
    enum OP_DISPLAY opLcd;
    char* data;
} MSG_LCD;

enum ESTADO_LCD {LCD_NORMAL, LCD_BLOCKED};

// Cola para la publicacion de mensajes.
xQueueHandle lcd_queue;




uint8 displayInicializarLcd();
enum OP_DISPLAY displayPintarWifi(enum OP_DISPLAY op);
enum OP_DISPLAY displayPintarBroker(enum OP_DISPLAY op);
enum OP_DISPLAY displayPintarReloj(struct DATOS_APLICACION *datosApp, enum OP_DISPLAY op);
void  lcd_task(void * pvParameters);
void displayBorrarLcd();
void pintarLcd(enum ELEMENTOS_DISPLAY elem, enum OP_DISPLAY op);
 enum OP_DISPLAY displayPintarSend(struct DATOS_APLICACION *datosApp, enum OP_DISPLAY op);
void displaySendData(struct DATOS_APLICACION *datosApp);
 void displayRecData(struct DATOS_APLICACION *datosApp);
enum OP_DISPLAY displayTemperatura(struct DATOS_APLICACION *datosApp, enum OP_DISPLAY op);
 enum OP_DISPLAY displayPintarRele(enum OP_DISPLAY op);
void displayPintarModo(struct DATOS_APLICACION *datosApp);
void displayPintarUmbrales(struct DATOS_APLICACION *datosApp, enum OP_DISPLAY op);
 enum OP_DISPLAY displayPintarReceived(struct DATOS_APLICACION *datosApp, enum OP_DISPLAY op);
 void displayCuadrado(uint16_t x, uint16_t y, uint16_t longitud, uint16_t altura, uint32_t color, uint8 grosor );
 void displayCirculo(uint8 x0, uint8 y0, uint8 r, uint32_t color);
  void displayPintarLcd(struct DATOS_APLICACION *datosApp, enum OP_DISPLAY op);
  void displayPintarPonerHora() ;
   void displayPintarSmartConfig();
   void displayPintarFactoryReset();
  void displaySetDay(struct DATOS_APLICACION *datosApp);
  void displaySetMonth(struct DATOS_APLICACION *datosApp);
  void displaySetYear(struct DATOS_APLICACION *datosApp);
  void displaySetHour(struct DATOS_APLICACION *datosApp);
  void displaySetMin(struct DATOS_APLICACION *datosApp);
  void displayClockConfirm(struct DATOS_APLICACION *datosApp);
void displayPintarSmartConfigFindChannel(struct DATOS_APLICACION *datosApp, enum OP_DISPLAY op);
void displayEscribirTexto(uint16_t x, uint16_t y, char *texto, uint16_t color, uint8 fuente, uint16_t *width, uint16_t *height, uint8 zoom);
void displayPintarImagen(uint16_t x, uint16_t y, int index, int size);
void displayPintarReset();
#ifdef __cplusplus
}
#endif

#endif /* APPDISPLAY_H */


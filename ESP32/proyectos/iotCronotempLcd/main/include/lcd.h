/*
 * lcd.h
 *
 *  Created on: 23 dic. 2020
 *      Author: t126401
 */

#ifndef MAIN_INCLUDE_LCD_H_
#define MAIN_INCLUDE_LCD_H_

/* Littlevgl specific */
#include "lvgl/src/lv_font/lv_symbol_def.h"
#include "lvgl.h"
#include "lvgl_helpers.h"
#include "configuracion_usuario.h"





/**
 * Funciones del lcd
 */

/*********************
 *      DEFINES
 *********************/

#define LV_COLOR_FONDO		LV_COLOR_BLACK
#define LV_COLOR_TEXTO		LV_COLOR_WHITE
#define LV_COLOR_FALLO		LV_COLOR_RED
#define TAMANO_BOTON 		40
#define MENSAJE_SMARTWIFI	"***** Puesta en marcha ******\n\nEs la primera vez que usas el dispositivo o has restaurado de fabrica.\nEjecuta la aplicacion movil y elije la opcion instalar dispositivo para enlazarlo a la red wifi.\nCuando este enlazado, el dispositivo arrancara normalmente."

#define LV_TICK_PERIOD_MS 1
#define COLOR_FONDO LV_COLOR_BLACK
#define COLOR_ICONO_OK LV_COLOR_TEXTO
#define COLOR_ICONO_NOK LV_COLOR_RED
#define COLOR_ICONO_INTENTANDO LV_COLOR_YELLOW
#define TAMANO_NOTIFICACIONES 30
#define SEPARADOR_ICONO 5
#define TAMANO_BOTON_UMBRAL 70
#define TAMANO_CONTENEDOR_UMBRAL 200


/**********************
 *  STATIC PROTOTYPES
 **********************/
void lv_tick_task(void *arg);
void tarea_lcd(void *pvParameter);
void lv_actualizar_modo_aplicacion_lcd(enum ESTADO_APP estado);

/*
void abrir_popup_inicializando();
void esconder_pantalla_principal(bool esconder);
void abrir_popup_reporte_alarmas(lv_obj_t * obj, lv_event_t event);
void abrir_popup_smartwifi();
void cerrar_popup_smart_wifi();
void crear_pantalla_principal();
void abrir_popup_generico(char * texto);
void cerrar_popup_generico(uint16_t delay);
*/


void lv_inicio_aplicacion_normal();
void lv_cargar_pantalla_principal();
void cerrar_popup_inicializando();







#endif /* MAIN_INCLUDE_LCD_H_ */

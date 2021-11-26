/*
 * cronotermostato.c
 *
 *  Created on: 5 abr. 2021
 *      Author: t126401
 */





#include "lv_cronotermostato.h"
#include "lvgl/lvgl.h"
#include "time.h"
#include "unistd.h"
#include "stdio.h"

/**
 * ESTILOS
 */

lv_style_t estilo_aplicacion;
lv_style_t estilo_area_temperatura;
lv_style_t estilo_arco;
lv_style_t estilo_botones_umbral;

/**
 * OBJETOS
 *
 */

static lv_obj_t *primera_pantalla;
static lv_obj_t *pantalla_principal;
static lv_obj_t *pantalla_smartconfig;
static lv_obj_t *popup;

static lv_obj_t *contenedor_iconos_notificacion;
static lv_obj_t *hora;
static lv_obj_t *wifi;
static lv_obj_t *reloj;
static lv_obj_t *conexion;

static lv_obj_t *contenedor_temperatura;
static lv_obj_t *rele_termostato;
static lv_obj_t *icono_temperatura;
static lv_obj_t *texto_temperatura;
static lv_obj_t *icono_humedad;
static lv_obj_t *texto_humedad;

static lv_obj_t *arco_temperatura;


static lv_obj_t *contenedor_botones;
static lv_obj_t *boton_mas;
static lv_obj_t *boton_menos;
static lv_obj_t *boton_modo;

static lv_obj_t *umbral_temperatura;
static lv_obj_t *boton_alarma;

static lv_obj_t *intervalo_anterior;
static lv_obj_t *intervalo_posterior;
/**
 * DEFINES
 */

#define TAMANO_HORIZONTAL	480
#define TAMANO_VERTICAL		320
#define LV_COLOR_FONDO		LV_COLOR_BLACK
#define LV_COLOR_TEXTO		LV_COLOR_CYAN
#define LV_COLOR_FALLO		LV_COLOR_RED
#define TAMANO_BOTON 		40
#define MENSAJE_SMARTWIFI	"***** Puesta en marcha ******\n\nEs la primera vez que usas el dispositivo o has restaurado de fabrica.\n\nEjecuta la aplicacion movil y elije la opcion instalar dispositivo para enlazarlo a la red wifi.\n\nCuando este enlazado, el dispositivo arrancara normalmente."


LV_IMG_DECLARE(heating);
LV_IMG_DECLARE(humidity);
LV_IMG_DECLARE(termometro);
LV_FONT_DECLARE(lv_font_led);
LV_FONT_DECLARE(lv_font_led_30);
LV_FONT_DECLARE(lv_font_led_20);
LV_IMG_DECLARE(automatico);
LV_IMG_DECLARE(automan);
LV_IMG_DECLARE(manual);
LV_IMG_DECLARE(boton_arriba);
LV_IMG_DECLARE(boton_abajo);


char* pintar_hora() {

	static char hora_actual[12] = {0};

	time_t now;
	struct tm fecha;
	//ESP_LOGI(TAG, ""TRAZAR"ACTUALIZAR_HORA", INFOTRAZA);
    time(&now);
    localtime_r(&now, &fecha);


	sprintf(hora_actual, "%02d:%02d",fecha.tm_hour,fecha.tm_min);

	return hora_actual;

}

void lv_crear_estilo_arco() {

	lv_style_reset(&estilo_arco);
	lv_style_set_bg_color(&estilo_arco, LV_STATE_DEFAULT, LV_COLOR_FONDO);
	lv_style_set_border_color(&estilo_arco, LV_STATE_DEFAULT, LV_COLOR_FONDO);

	lv_style_set_radius(&estilo_arco, LV_STATE_DEFAULT, 275);
	lv_style_set_outline_color(&estilo_arco, LV_STATE_DEFAULT, LV_COLOR_GREEN);
	lv_style_set_shadow_color(&estilo_arco, LV_STATE_DEFAULT, LV_COLOR_YELLOW);
	lv_style_set_value_color(&estilo_arco, LV_STATE_DEFAULT, LV_COLOR_GREEN);


}

void lv_crear_estilo_aplicacion() {

	lv_style_reset(&estilo_aplicacion);
	lv_style_set_bg_color(&estilo_aplicacion, LV_STATE_DEFAULT, LV_COLOR_FONDO);
	lv_style_set_text_color(&estilo_aplicacion, LV_STATE_DEFAULT, LV_COLOR_TEXTO);
	lv_style_set_text_color(&estilo_aplicacion, LV_STATE_DISABLED, LV_COLOR_FALLO);
	lv_style_set_border_color(&estilo_aplicacion, LV_STATE_DEFAULT, LV_COLOR_FONDO);
	lv_style_set_scale_border_width(&estilo_aplicacion, LV_STATE_DEFAULT, 0);

}

void lv_crear_estilo_area_temperatura() {

	lv_style_reset(&estilo_area_temperatura);
	lv_style_set_bg_color(&estilo_area_temperatura, LV_STATE_DEFAULT, LV_COLOR_FONDO);
	lv_style_set_text_color(&estilo_area_temperatura, LV_STATE_DEFAULT, LV_COLOR_TEXTO);
	lv_style_set_border_color(&estilo_area_temperatura, LV_STATE_DEFAULT, LV_COLOR_FONDO);
	lv_style_set_line_color(&estilo_arco, LV_STATE_DEFAULT, LV_COLOR_RED);

}

void lv_crear_estilo_botones_umbral() {

	lv_style_reset(&estilo_botones_umbral);
	lv_style_set_bg_color(&estilo_botones_umbral, LV_STATE_DEFAULT, LV_COLOR_FONDO);
	lv_style_set_bg_color(&estilo_botones_umbral, LV_STATE_PRESSED, LV_COLOR_RED);
	lv_style_set_border_color(&estilo_botones_umbral, LV_STATE_DEFAULT, LV_COLOR_FONDO);
	lv_style_set_border_color(&estilo_botones_umbral, LV_STATE_FOCUSED, LV_COLOR_GREEN);
	lv_style_set_line_color(&estilo_botones_umbral, LV_STATE_DEFAULT, LV_COLOR_FONDO);
	lv_style_set_outline_width(&estilo_botones_umbral, LV_STATE_DEFAULT, 0);

	//lv_style_set_border_post(&estilo_botones_umbral, LV_STATE_DEFAULT, true);
	//lv_style_set_border_side(&estilo_botones_umbral, LV_STATE_DEFAULT, 0);

	lv_style_set_border_color(&estilo_botones_umbral, LV_STATE_PRESSED, LV_COLOR_CYAN);
	lv_style_set_text_color(&estilo_botones_umbral, LV_STATE_DEFAULT, LV_COLOR_TEXTO);
    lv_style_set_line_rounded(&estilo_botones_umbral, LV_STATE_DEFAULT, false);
    lv_style_set_radius(&estilo_botones_umbral, LV_STATE_DEFAULT, 0);


}



void lv_crear_msgbox(lv_obj_t *padre, char* mensaje) {

	popup = lv_msgbox_create(padre, NULL);
	lv_obj_add_style(popup, LV_MSGBOX_PART_BG, &estilo_aplicacion);
	lv_obj_set_style_local_value_align(popup, LV_MSGBOX_PART_BG, LV_STATE_DEFAULT, LV_ALIGN_IN_TOP_LEFT);
	lv_msgbox_set_text(popup, mensaje);
	lv_obj_align(popup, pantalla_smartconfig, LV_ALIGN_CENTER, 0, 0);
	//lv_obj_set_pos(popup, 100, 20);

}



void lv_crear_pantalla_inicializando(char* mensaje){

	primera_pantalla = lv_obj_create(NULL, NULL);
	lv_scr_load(primera_pantalla);
	lv_obj_add_style(primera_pantalla, LV_OBJ_PART_MAIN, &estilo_aplicacion);
	lv_crear_msgbox(primera_pantalla, mensaje);

}


lv_obj_t * lv_crear_icono_notificacion(lv_obj_t *padre, char* texto) {

	lv_obj_t *objeto;
	objeto = lv_label_create(padre, NULL);
	lv_label_set_text(objeto, texto);

	return objeto;

}


void lv_crear_barra_notificaciones(lv_obj_t *padre, lv_coord_t x, lv_coord_t y ) {


	lv_coord_t margen = 5;
	contenedor_iconos_notificacion = lv_cont_create(padre, NULL);
	lv_obj_add_style(contenedor_iconos_notificacion, LV_CONT_PART_MAIN, &estilo_aplicacion);
	conexion = lv_crear_icono_notificacion(contenedor_iconos_notificacion, LV_SYMBOL_SHUFFLE);
	reloj = lv_crear_icono_notificacion(contenedor_iconos_notificacion, LV_SYMBOL_LOOP);
	wifi = lv_crear_icono_notificacion(contenedor_iconos_notificacion, LV_SYMBOL_WIFI);
	hora = lv_crear_icono_notificacion(contenedor_iconos_notificacion, pintar_hora());

	lv_obj_set_pos(conexion, x, y);
	x += lv_obj_get_width(conexion) + margen;
	lv_obj_set_pos(reloj, x, y);
	x += lv_obj_get_width(reloj) + margen;
	lv_obj_set_pos(wifi, x, y);
	x += lv_obj_get_width(wifi) + margen;
	lv_obj_set_pos(hora, x, y);
	lv_obj_set_height(contenedor_iconos_notificacion, lv_obj_get_height(conexion));
	x += lv_obj_get_width(hora) + margen;
	lv_obj_set_size(contenedor_iconos_notificacion, x, lv_obj_get_height(conexion));




}

void lv_crear_icono_heating(lv_obj_t *padre) {

	rele_termostato = lv_img_create(padre, NULL);
	lv_img_set_src(rele_termostato, &heating);


}


void lv_crear_area_temperatura(lv_obj_t *padre) {


	lv_coord_t margen = 5;
	lv_coord_t y;
	lv_coord_t x;
	contenedor_temperatura = lv_cont_create(padre, NULL);
	lv_crear_estilo_area_temperatura();
	lv_obj_add_style(contenedor_temperatura, LV_CONT_PART_MAIN, &estilo_area_temperatura);

	/* icono de heating */

	/** icono temperatura y temperatura */
	icono_temperatura = lv_img_create(contenedor_temperatura, NULL);
	lv_img_set_src(icono_temperatura, &termometro);
	lv_obj_set_pos(icono_temperatura, 0, 0);
	texto_temperatura = lv_label_create(contenedor_temperatura, NULL);
	lv_obj_set_pos(texto_temperatura, lv_obj_get_width(icono_temperatura) + margen, 0);
	lv_label_set_text(texto_temperatura, "22.5 ºC");
	lv_obj_set_style_local_text_font(texto_temperatura, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, &lv_font_led);
	lv_obj_set_width(contenedor_temperatura, lv_obj_get_width(icono_temperatura) + margen + lv_obj_get_width(texto_temperatura));

	y = lv_obj_get_height(texto_temperatura);
	/* icono humedad y humedad */

	icono_humedad = lv_img_create(contenedor_temperatura, NULL);
	lv_img_set_src(icono_humedad, &humidity);
	x = 50 + margen;
	lv_obj_set_pos(icono_humedad, x,lv_obj_get_height(texto_temperatura));
	lv_obj_set_height(contenedor_temperatura, lv_obj_get_height(texto_temperatura) + lv_obj_get_height(icono_humedad));

	texto_humedad = lv_label_create(contenedor_temperatura, NULL);
	lv_label_set_text(texto_humedad, "25.4%");
	lv_obj_set_pos(texto_humedad, lv_obj_get_width(icono_humedad) + x + margen,y+ margen );








}

void lv_crear_arco(lv_obj_t *padre) {


	lv_crear_estilo_arco();
	arco_temperatura = lv_arc_create(padre, NULL);
	lv_obj_add_style(arco_temperatura, LV_ARC_PART_BG, &estilo_arco);
	lv_obj_set_style_local_line_color(arco_temperatura, LV_ARC_PART_INDIC, LV_STATE_DEFAULT, LV_COLOR_CYAN);
	lv_obj_set_style_local_line_color(arco_temperatura, LV_ARC_PART_BG, LV_STATE_DEFAULT, LV_COLOR_GRAY);
	lv_obj_set_style_local_bg_grad_color(arco_temperatura, LV_ARC_PART_INDIC, LV_STATE_DEFAULT, LV_COLOR_BLUE);




	lv_arc_set_end_angle(arco_temperatura, 200);
	lv_obj_set_size(arco_temperatura, 275, 275);
	lv_arc_set_range(arco_temperatura, 0, 50);
	lv_arc_set_value(arco_temperatura, 45);



}

void manejador_boton_mas(lv_obj_t *objeto, lv_event_t evento) {


	switch (evento) {

	case LV_EVENT_PRESSED:
		printf("has pulsado el boton mas\n");

		break;
	case LV_EVENT_LONG_PRESSED:
		printf("has hecho una pulsacion larga en el boton mas");
		break;

	default:
		break;

	}

}

void manejador_boton_menos(lv_obj_t *objeto, lv_event_t evento) {


	switch (evento) {

	case LV_EVENT_PRESSED:
		printf("has pulsado el boton menos\n");

		break;
	case LV_EVENT_LONG_PRESSED:
		printf("has hecho una pulsacion larga en el boton menos");
		break;
	default:
		break;

	}

}




void lv_crear_botones_umbral(lv_obj_t *padre) {

	contenedor_botones = lv_cont_create(padre, NULL);
	lv_crear_estilo_botones_umbral();
	lv_obj_add_style(contenedor_botones, LV_CONT_PART_MAIN, &estilo_botones_umbral);
	boton_mas = lv_imgbtn_create(contenedor_botones, NULL);
	lv_imgbtn_set_src(boton_mas, LV_STATE_DEFAULT, &boton_arriba);
	lv_obj_set_event_cb(boton_mas, manejador_boton_mas);

	lv_btn_set_checkable(boton_mas, false);
	lv_obj_set_pos(boton_mas, 0,0);
	lv_obj_set_size(boton_mas, 60, 60);
	lv_obj_add_style(boton_mas, LV_CONT_PART_MAIN, &estilo_botones_umbral);

	lv_obj_set_style_local_value_str(boton_mas, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_SYMBOL_UP);
	lv_obj_set_style_local_value_align(boton_mas, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_ALIGN_CENTER);
	lv_obj_set_size(contenedor_botones, lv_obj_get_width(boton_mas), (lv_obj_get_height(boton_mas)*2) + 60);

	//boton_menos = lv_btn_create(contenedor_botones, boton_mas);
	boton_menos = lv_imgbtn_create(contenedor_botones, NULL);
	lv_imgbtn_set_src(boton_menos, LV_STATE_DEFAULT, &boton_abajo);

	lv_obj_set_event_cb(boton_menos, manejador_boton_menos);
	lv_btn_set_checkable(boton_menos, false);
	lv_obj_set_style_local_value_str(boton_menos, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_SYMBOL_DOWN);
	lv_obj_set_pos(boton_menos, 0, lv_obj_get_height(boton_menos) + 60);

}

void lv_crear_umbral_temperatura(lv_obj_t *padre) {


	umbral_temperatura = lv_label_create(padre, NULL);
	lv_obj_add_style(umbral_temperatura, LV_LABEL_PART_MAIN, &estilo_area_temperatura);
	lv_obj_set_style_local_text_font(umbral_temperatura, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_led_30);
	lv_label_set_text_fmt(umbral_temperatura, "21.7 ºC");


}

void abrir_popup_reporte_alarmas(lv_obj_t *objeto, lv_event_t evento) {

	switch (evento) {

	case LV_EVENT_CLICKED:
		printf("has pulsado el boton alarmas\n");

		break;
	default:
		break;

	}


}


void lv_crear_boton_alarma(lv_obj_t *padre) {

	boton_alarma = lv_btn_create(padre, NULL);
	lv_obj_set_event_cb(boton_alarma, abrir_popup_reporte_alarmas);
	lv_obj_add_style(boton_alarma, LV_BTN_PART_MAIN, &estilo_aplicacion);
	lv_obj_set_size(boton_alarma, TAMANO_BOTON, TAMANO_BOTON);
	lv_obj_set_style_local_value_str(boton_alarma, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_SYMBOL_WARNING);
	lv_obj_set_style_local_value_color(boton_alarma, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_YELLOW);
	lv_obj_set_style_local_bg_color(boton_alarma, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_RED);


}

void lv_crear_intervalos(lv_obj_t *padre) {

	intervalo_anterior = lv_label_create(padre, texto_humedad);
	lv_label_set_text_fmt(intervalo_anterior, "18:00");
	intervalo_posterior = lv_label_create(padre, intervalo_anterior);
	lv_label_set_text_fmt(intervalo_posterior, "21:00");

}

void manejador_boton_modo(lv_obj_t *objeto, lv_event_t evento) {

	switch (evento) {

	case LV_EVENT_CLICKED:
		printf("has pulsado el boton modo\n");

		break;
	default:
		break;

	}


}

void lv_crear_boton_modo(lv_obj_t *padre) {

	boton_modo = lv_imgbtn_create(padre, NULL);
	lv_obj_set_event_cb(boton_modo, manejador_boton_modo);
	lv_obj_add_style(boton_modo, LV_STATE_DEFAULT, &estilo_aplicacion);
	lv_imgbtn_set_src(boton_modo, LV_STATE_DEFAULT, &automatico);
	lv_imgbtn_set_src(boton_modo, LV_STATE_PRESSED, &automatico);


}


void lv_cerrar_pantalla_smartconfig() {

	lv_obj_del(pantalla_smartconfig);
}

void lv_crear_pantalla_smartconfig() {

	pantalla_smartconfig = lv_obj_create(NULL, NULL);
	lv_obj_add_style(pantalla_smartconfig, LV_OBJ_PART_MAIN, &estilo_aplicacion);
	lv_crear_msgbox(pantalla_smartconfig, MENSAJE_SMARTWIFI);
}


void lv_inicio_aplicacion_normal() {

	lv_area_t pos;
	//lv_obj_del(primera_pantalla);
	pantalla_principal = lv_obj_create(NULL, NULL);
	lv_scr_load(pantalla_principal);
	/* Creacion de la barra de notificaciones */
	lv_crear_barra_notificaciones(pantalla_principal, 0, 0);
	lv_obj_add_style(pantalla_principal, LV_OBJ_PART_MAIN, &estilo_aplicacion);
	lv_obj_set_pos(contenedor_iconos_notificacion, TAMANO_HORIZONTAL - lv_obj_get_width(contenedor_iconos_notificacion), 10);

	/* Creamos el area de temperatura */

	/* Creamos el arco */
	lv_crear_arco(pantalla_principal);
	lv_obj_set_pos(arco_temperatura, 100,45);

	/* Creamos el icono heating */
	lv_crear_icono_heating(pantalla_principal);
	lv_crear_area_temperatura(pantalla_principal);
	/* Posicionamos el icono y el area de temperatura */
	lv_obj_get_coords(arco_temperatura, &pos);
	lv_obj_set_pos(rele_termostato, pos.x1 + ((pos.x2 - pos.x1)/2) - lv_obj_get_width(rele_termostato)/2, pos.y1 + (pos.y1 * 0.8));
	lv_obj_set_pos(contenedor_temperatura, pos.x1 + (pos.x1 /2), pos.y1 + (pos.y1 * 2.7));

	/* Creamos los botones del umbral */
	lv_crear_botones_umbral(pantalla_principal);
	lv_obj_set_pos(contenedor_botones, TAMANO_HORIZONTAL - lv_obj_get_width(contenedor_botones) - 10, 80);

	/* Creamos el umbral */
	lv_crear_umbral_temperatura(pantalla_principal);
	lv_obj_set_pos(umbral_temperatura, 2 * pos.x1, pos.y1 + (pos.y1 * 4.8));

	/* Creamos el boton alarma */
	lv_crear_boton_alarma(pantalla_principal);
	//lv_obj_set_pos(boton_alarma, pos.x1 + 50, pos.y1 + 80);
	lv_obj_set_pos(boton_alarma,50,50);

	/* Creamos los intervalos */
	lv_crear_intervalos(pantalla_principal);
	lv_obj_set_pos(intervalo_anterior, pos.x1 + 40, pos.y2 - 40);
	lv_obj_set_pos(intervalo_posterior, pos.x2 - 70, pos.y2 -40);
	/* Creamos el boton modo */

	lv_crear_boton_modo(pantalla_principal);
	//lv_obj_set_pos(boton_modo,pos.x2 - 110, pos.y1 + 70 );
	lv_obj_set_pos(boton_modo,50, 220);


}


void lv_pantalla_cronotermostato_app() {


	int estado_app = 0;

	lv_crear_estilo_aplicacion();
	/*
	if (estado_app == 0) {

		lv_crear_pantalla_smartconfig();
		lv_scr_load(pantalla_smartconfig);
	} else {
		lv_crear_pantalla_inicializando("Inicializando\nEspere un momento...");
		lv_msgbox_start_auto_close(popup, 5000);
		lv_scr_load(primera_pantalla);

	}
*/
	lv_inicio_aplicacion_normal();


}

void lv_actualizar_estado_wifi_lcd(DATOS_APLICACION *datosApp, lv_color_t color) {

	ESP_LOGI(TAG, ""TRAZAR" ACTUALIZAMOS EL ESTADO WIFI DEL LCD", INFOTRAZA);
	if (datosApp->datosGenerales->estadoApp == NORMAL_ARRANCANDO) {
		lv_cargar_pantalla_principal();

		//lv_inicio_aplicacion_normal();
	} else {
		lv_obj_set_style_local_text_color(wifi, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, color);
	}


}






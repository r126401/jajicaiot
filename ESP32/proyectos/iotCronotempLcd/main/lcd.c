/*
 * lcd.c
 *
 *  Created on: 23 dic. 2020
 *      Author: t126401
 */


#include "funciones_usuario.h"
#include "lcd.h"
#include "esp_err.h"
#include "esp_log.h"
#include "interfaz_usuario.h"
#include "conexiones_mqtt.h"


static lv_obj_t *primera_pantalla;
static lv_obj_t *pantalla_principal;
static lv_obj_t *pantalla_smartconfig;
static lv_obj_t *popup;

static lv_obj_t *contenedor_iconos_notificacion;
static lv_obj_t *contenedor_temperatura;
static lv_obj_t *contenedor_botones;
static lv_obj_t *arco_temperatura;
static lv_obj_t *intervalo_anterior;
static lv_obj_t *intervalo_posterior;
static lv_obj_t *icono_temperatura;
static lv_obj_t *texto_temperatura;
static lv_obj_t *icono_humedad;
static lv_obj_t *texto_humedad;
static lv_obj_t *barra_temperatura;

static const char *TAG = "LCD";
/**
 * DEFINESSemaphoreHandle_t xGuiSemaphore;
 */

//#define TAMANO_HORIZONTAL	CONFIG_LV_DISPLAY_WIDTH
//#define TAMANO_VERTICAL		CONFIG_LV_DISPLAY_HEIGHT

/**
 * ESTILOS
 */

lv_style_t estilo_aplicacion;
lv_style_t estilo_area_temperatura;
lv_style_t estilo_arco;
lv_style_t estilo_barra;
lv_style_t estilo_botones_umbral;







lv_obj_t * grupo_iconos_notificacion;
lv_obj_t *area_umbral_temperatura;
lv_obj_t *area_temperatura;


#define LV_SYMBOL_THERMOMETER_EMPTY     "\xef\x8b\x8b"
#define LV_SYMBOL_TINT                  "\xef\x81\x83"
#define LV_SYMBOL_BALANCE_SCALE         "\xef\x89\x8e"
#define LV_SYMBOL_BARS                  "\xef\x83\x89"
#define LV_SYMBOL_MICROCHIP             "\xef\x8b\x9b"

//#define MENSAJE_SMARTWIFI "Utiliza la App para enlazar el dispositivo a tu red wifi de 2.4Ghz"


static lv_obj_t *pantalla_1;
static lv_obj_t *contenedor_botones;
static lv_obj_t *boton_mas;
static lv_obj_t *boton_menos;
static lv_obj_t *boton_modo;

static lv_obj_t *umbral_temperatura;
//static lv_obj_t *temperatura;
//static lv_obj_t *icono_termometro;
//static lv_obj_t *humedad;
static lv_obj_t *icono_humedad;
//static lv_obj_t *modo_termostato;

static lv_obj_t *hora;
static lv_obj_t *wifi;
static lv_obj_t *reloj;
static lv_obj_t *conexion;

static lv_obj_t *rele_termostato;
static lv_obj_t *boton_alarma;
static lv_obj_t *popup;



#define TAG "LCD"
SemaphoreHandle_t xGuiSemaphore;
ETSTimer temporizador_pulsador_umbral;
extern DATOS_APLICACION datosApp;

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


lv_obj_t* poner_icono(lv_obj_t *grupo, char* texto, lv_coord_t x, lv_coord_t y, lv_coord_t *size) {


	lv_obj_t *icono;
	//lv_coord_t tam;

	icono = lv_label_create(grupo, NULL);
	lv_obj_set_pos(icono, x, y);
	lv_label_set_text(icono, texto);
	*size = lv_obj_get_width(icono);
	*size += SEPARADOR_ICONO;
	ESP_LOGE(TAG, ""TRAZAR" tamano icono :%d", INFOTRAZA, *size);


	return icono;


}


void crear_iconos_notificacion(DATOS_APLICACION *datosApp, lv_style_t *estilo, lv_coord_t x, lv_coord_t y) {



	lv_coord_t tamano_grupo = 0;
	lv_coord_t tamano=0;
	lv_obj_add_style(pantalla_1, LV_OBJ_PART_MAIN, estilo);
	grupo_iconos_notificacion = lv_cont_create(pantalla_1, NULL);
	//lv_obj_set_size(grupo_iconos_notificacion, tamano_grupo, TAMANO_NOTIFICACIONES);
	// Creamos fecha
	conexion = poner_icono(grupo_iconos_notificacion, LV_SYMBOL_SHUFFLE, tamano,0, &tamano);
	tamano_grupo += tamano;
	reloj = poner_icono(grupo_iconos_notificacion, LV_SYMBOL_LOOP, tamano_grupo,0, &tamano);
	tamano_grupo += tamano;
	wifi = poner_icono(grupo_iconos_notificacion, LV_SYMBOL_WIFI, tamano_grupo, 0, &tamano);
	tamano_grupo += tamano;
	hora = poner_icono(grupo_iconos_notificacion, pintar_hora(), tamano_grupo, 0, &tamano);
	tamano_grupo += tamano;
	lv_obj_set_size(grupo_iconos_notificacion, tamano_grupo, TAMANO_NOTIFICACIONES);
	lv_obj_set_pos(grupo_iconos_notificacion, CONFIG_LV_DISPLAY_WIDTH - tamano_grupo, 10);
	lv_obj_add_style(grupo_iconos_notificacion, LV_OBJ_PART_MAIN, estilo);

}

static void temporizacion_pulsacion_umbral() {

	cJSON *informe;
    enum TIPO_ACCION_TERMOSTATO accion_termostato;
    enum ESTADO_RELE accion_rele;
	//appuser_cambiar_modo_aplicacion(&datosApp, NORMAL_AUTOMAN);
	//leer_temperatura(&datosApp);
    if (((accion_termostato = calcular_accion_termostato(&datosApp, &accion_rele)) == ACCIONAR_TERMOSTATO)) {
    	operacion_rele(&datosApp, TEMPORIZADA, accion_rele);
    }


	ESP_LOGI(TAG, ""TRAZAR" umbral cambiado a %lf", INFOTRAZA, datosApp.termostato.tempUmbral);
	lv_obj_set_style_local_text_color(umbral_temperatura, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_TEXTO);
	informe = appuser_generar_informe_espontaneo(&datosApp, CAMBIO_UMBRAL_TEMPERATURA, NULL);
	publicar_mensaje_json(&datosApp, informe, NULL);

}


static void manejador_boton_umbrales() {

	lv_obj_set_style_local_text_color(umbral_temperatura, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_YELLOW);
	lv_actualizar_umbral_temperatura_lcd(&datosApp);
	if (datosApp.datosGenerales->estadoApp == NORMAL_AUTO) {
		appuser_cambiar_modo_aplicacion(&datosApp, NORMAL_AUTOMAN);
	}

    ets_timer_disarm(&temporizador_pulsador_umbral);
    ets_timer_setfn(&temporizador_pulsador_umbral, (ETSTimerFunc*) temporizacion_pulsacion_umbral, NULL);
    ets_timer_arm(&temporizador_pulsador_umbral, 2000,0);
    ESP_LOGI(TAG, ""TRAZAR" el umbral sigue valiendo : %.02f", INFOTRAZA, datosApp.termostato.tempUmbral);



}

static void manejador_boton_mas(lv_obj_t * obj, lv_event_t event) {



    if(event == LV_EVENT_CLICKED) {
    	ESP_LOGI(TAG, ""TRAZAR" HE PULSADO EL BOTON MAS incrementando en %.02lf", INFOTRAZA, datosApp.termostato.incdec);
    	datosApp.termostato.tempUmbral += datosApp.termostato.incdec;
    	manejador_boton_umbrales();

    }
}

static void manejador_boton_menos(lv_obj_t * obj, lv_event_t event) {


    if(event == LV_EVENT_CLICKED) {
    	ESP_LOGI(TAG, ""TRAZAR" HE PULSADO EL BOTON MENOS incrementando en %.02lf", INFOTRAZA, datosApp.termostato.incdec);
    	datosApp.termostato.tempUmbral -= datosApp.termostato.incdec;
    	manejador_boton_umbrales();
    }
}

static void manejador_boton_modo(lv_obj_t * obj, lv_event_t event) {

	ESP_LOGI(TAG, ""TRAZAR" HE PULSADO EL BOTON MODO Y EL EVENTO ES %d", INFOTRAZA, event);
	if(event == LV_EVENT_CLICKED) {
	    	ESP_LOGI(TAG, ""TRAZAR" HE PULSADO EL BOTON modo", INFOTRAZA);
	    	pulsacion_modo_app(&datosApp);

	    }

}
void cerrar_popup_inicializando() {


	if (popup != NULL) {
		ESP_LOGW(TAG, ""TRAZAR" CERRAMOS EL POPUP", INFOTRAZA);
		lv_msgbox_start_auto_close(popup, 0);
	}


	//lv_obj_del(popup);
}

void abrir_popup_inicializando() {


	popup = lv_msgbox_create(lv_layer_top(), NULL);
	lv_obj_set_size(popup, CONFIG_LV_DISPLAY_WIDTH, CONFIG_LV_DISPLAY_HEIGHT);
	lv_msgbox_set_text(popup, "Inicializando.\nEspere un momento por favor...");
	lv_obj_align_mid(popup, pantalla_1, LV_ALIGN_CENTER, 0, 0);
}


lv_obj_t* mostrar_popup(DATOS_APLICACION *datosApp, char* mensaje) {


	lv_coord_t ancho=200, alto=20;
	xSemaphoreTake(xGuiSemaphore, portMAX_DELAY);
	if (datosApp->popup == NULL) {
	    datosApp->popup = lv_msgbox_create(lv_layer_top(), NULL);
	    lv_obj_t * scr =  lv_disp_get_scr_act(NULL);
	    lv_obj_set_size(datosApp->popup, ancho, alto);
	    lv_msgbox_set_text(datosApp->popup, mensaje);
	    lv_obj_align_mid(datosApp->popup, scr, LV_ALIGN_CENTER, 0,0 );

	} else {
		ESP_LOGE(TAG, ""TRAZAR" ERROR, YA HAY UN POPUP CREADO", INFOTRAZA);
	}

	xSemaphoreGive(xGuiSemaphore);
    return datosApp->popup;
}

void quitar_popup(DATOS_APLICACION *datosApp) {

	if (datosApp->popup != NULL) {
		lv_obj_del(datosApp->popup);
	}

	datosApp->popup = NULL;


}


void lv_tick_task(void *arg) {
    (void) arg;

    lv_tick_inc(LV_TICK_PERIOD_MS);
}


void lv_actualizar_intervalo_programa(DATOS_APLICACION *datosApp, bool comienzo_programa) {

	time_t t_siguiente_intervalo;
	time_t hora_actual;
	int programa_actual = -1;
	int hora = -1;
	int minuto = -1;
	int duracion;
	TIME_PROGRAM programacion_actual;
	static time_t longitud_intervalo;
	time_t puntero_actual = 0;
	time_t intervalo_superior, intervalo_inferior;
	int progreso;
	struct tm *intervalo_siguiente;


	hora_actual = datosApp->datosGenerales->clock.time;
	buscar_programa(datosApp->datosGenerales->programacion,
			datosApp->datosGenerales->nProgramacion,
			&programa_actual,
			&t_siguiente_intervalo);

	//Calculo del longitud_intervalo del intervalo.
	if (comienzo_programa) {
		//_arc_set_range(arco_temperatura, 0, 100);
		lv_bar_set_range(barra_temperatura, 0, 100);


	}


	if (datosApp->datosGenerales->programacion != NULL) {
		programacion_actual = datosApp->datosGenerales->programacion[programa_actual];
		hora = programacion_actual.programacion.tm_hour;
		minuto = programacion_actual.programacion.tm_min;
		intervalo_inferior = programacion_actual.programa;
		ESP_LOGE(TAG, ""TRAZAR" INTERVALO INFERIOR: %d:%d", INFOTRAZA, hora, minuto);
		intervalo_superior = intervalo_inferior + programacion_actual.duracion;
		longitud_intervalo = intervalo_superior - intervalo_inferior;
		puntero_actual = hora_actual - intervalo_inferior;
		progreso = (puntero_actual*100)/longitud_intervalo;
		intervalo_siguiente = localtime(&intervalo_superior);
		ESP_LOGE(TAG, ""TRAZAR" INTERVALO SUPERIOR: %d:%d", INFOTRAZA, intervalo_siguiente->tm_hour, intervalo_siguiente->tm_min);
		ESP_LOGE(TAG, ""TRAZAR" puntero actual: %ld, progreso :%d", INFOTRAZA, puntero_actual, progreso);

		lv_label_set_text_fmt(intervalo_anterior, "%02d:%02d", hora, minuto);
		lv_label_set_text_fmt(intervalo_posterior, "%02d:%02d", intervalo_siguiente->tm_hour, intervalo_siguiente->tm_min);
		lv_bar_set_value(barra_temperatura, progreso, LV_ANIM_OFF);


		ESP_LOGE(TAG, ""TRAZAR" Llevamos el %d %% del total del intervalo que es 100%%", INFOTRAZA, progreso);

	} else {

		ESP_LOGW(TAG, ""TRAZAR" NO SE PINTA NADA PORQUE NO HAY PROGRAMACION", INFOTRAZA);
	}






	//longitud_intervalo = t_siguiente_intervalo - programacion_actual.programa;
	//puntero_actual = hora_actual- programacion_actual.programa;
}





void lv_actualizar_intervalo_programa2(DATOS_APLICACION *datosApp, bool comienzo_programa) {

	time_t t_siguiente_intervalo;
	time_t hora_actual;
	int programa_actual = -1;
	int hora = -1;
	int minuto = -1;
	struct tm *intervalo_siguiente;
	struct tm *hoy;
	time_t hora_hoy;
	static time_t longitud_intervalo;
	time_t puntero_actual = 0;
	TIME_PROGRAM programacion_actual;
	ESP_LOGI(TAG, ""TRAZAR" entramos en lv_actualizar_intervalo_programa", INFOTRAZA);
	programacion_actual.programa = 0;

	hora_actual = datosApp->datosGenerales->clock.time;
	buscar_programa(datosApp->datosGenerales->programacion,
			datosApp->datosGenerales->nProgramacion,
			&programa_actual,
			&t_siguiente_intervalo);



	//Calculo del longitud_intervalo del intervalo.
	if (comienzo_programa) {
		//_arc_set_range(arco_temperatura, 0, 100);
		lv_bar_set_range(barra_temperatura, 0, 100);


	}

	if (programa_actual < 0) {
		hora = 0;
		minuto = 0;
		hoy = localtime(&hora_actual);
		hoy->tm_hour = 0;
		hoy->tm_min = 0;
		hoy->tm_sec = 0;
		hora_hoy = mktime(hoy);
		longitud_intervalo = t_siguiente_intervalo - hora_hoy;
		puntero_actual = hora_actual - hora_hoy;

	} else {
		programacion_actual = datosApp->datosGenerales->programacion[programa_actual];
		hora = programacion_actual.programacion.tm_hour;
		minuto = programacion_actual.programacion.tm_min;
		longitud_intervalo = programacion_actual.programa + programacion_actual.duracion;;
		puntero_actual = hora_actual- programacion_actual.programa;
	}




		ESP_LOGE(TAG, ""TRAZAR" INTERVALO INFERIOR: %d:%d", INFOTRAZA, hora, minuto);
		lv_label_set_text_fmt(intervalo_anterior, "%02d:%02d", hora, minuto);

		//intervalo_siguiente = (struct tm *) calloc(1, sizeof(struct tm));
		intervalo_siguiente = localtime(&t_siguiente_intervalo);
		lv_label_set_text_fmt(intervalo_posterior, "%02d:%02d", intervalo_siguiente->tm_hour, intervalo_siguiente->tm_min);

		//Calculo del intervalo.


/*

		if (programa_actual < 0) {
			puntero_actual = hora_actual - *hora_hoy;
		} else {
			puntero_actual = hora_actual- programacion_actual.programa;
		}
*/
		ESP_LOGE(TAG, ""TRAZAR" LONGITUD DEL INTERVALO %ld", INFOTRAZA, longitud_intervalo);
		puntero_actual = (puntero_actual * 100)/longitud_intervalo;
		//lv_arc_set_value(arco_temperatura, puntero_actual );
		lv_bar_set_value(barra_temperatura, puntero_actual, LV_ANIM_OFF);


		ESP_LOGE(TAG, ""TRAZAR" Llevamos el %ld %% del total del intervalo que es 100%%", INFOTRAZA, puntero_actual);

}


void lv_actualizar_hora_lcd(DATOS_APLICACION *datosApp) {

	ESP_LOGI(TAG, ""TRAZAR" entramos en actualizar_hora_lcd", INFOTRAZA);
	lv_obj_set_style_local_text_color(hora, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_TEXTO);
	lv_label_set_text_fmt(hora, "%02d:%02d", datosApp->datosGenerales->clock.date.tm_hour, datosApp->datosGenerales->clock.date.tm_min);
}




void lv_actualizar_estado_ntp_lcd(DATOS_APLICACION *datosApp, lv_color_t color) {

	ESP_LOGI(TAG, ""TRAZAR" entramos en lv_actualizar_estado_ntp_lcd", INFOTRAZA);
	lv_obj_set_style_local_text_color(reloj, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, color);

}
void lv_actualizar_estado_broker_lcd(DATOS_APLICACION *datosApp, lv_color_t color) {

	ESP_LOGI(TAG, ""TRAZAR" entramos en lv_actualizar_estado_broker_lcd", INFOTRAZA);
	lv_obj_set_style_local_text_color(conexion, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, color);

}

void lv_actualizar_temperatura_lcd(DATOS_APLICACION *datosApp) {


	//char temperatura[10];
	//sprintf(temperatura, "%.1f ºC", datosApp->termostato.tempActual);
	ESP_LOGI(TAG, ""TRAZAR" entramos en lv_actualizar_temperatura_lcd %.1f", INFOTRAZA, datosApp->termostato.tempActual);
	//lv_label_set_text(texto_temperatura, temperatura);
	lv_label_set_text_fmt(texto_temperatura, "%.1f ºC", datosApp->termostato.tempActual);
	ESP_LOGI(TAG, ""TRAZAR" entramos en lv_actualizar_temperatura_lcd despues", INFOTRAZA);
	lv_obj_set_style_local_text_color(texto_temperatura, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, COLOR_ICONO_OK);
	ESP_LOGI(TAG, ""TRAZAR" entramos en lv_actualizar_temperatura_lcd despuessssssss", INFOTRAZA);
}

void lv_actualizar_humedad_lcd(DATOS_APLICACION *datosApp) {

	ESP_LOGI(TAG, ""TRAZAR" entramos en lv_actualizar_humedad_lcd", INFOTRAZA);
	//lv_label_set_text_fmt(texto_humedad, "%.01lf %%", datosApp->termostato.humedad);

}

void lv_actualizar_rele_lcd( enum ESTADO_RELE estado) {

	bool escondido = false;
	ESP_LOGI(TAG, ""TRAZAR" entramos en lv_actualizar_rele_lcd", INFOTRAZA);

	switch (estado) {
	case ON:
		escondido = false;
	    break;
	case OFF:
		escondido = true;
		break;
	default:
		escondido = true;
		break;
	}
	if (rele_termostato != NULL) {
		lv_obj_set_hidden(rele_termostato, escondido);
	}


}

void lv_actualizar_umbral_temperatura_lcd(DATOS_APLICACION *datosApp) {

	ESP_LOGE(TAG, ""TRAZAR" ACTUALIZANDO UMBRAL EN LCD", INFOTRAZA);
	lv_label_set_text_fmt(umbral_temperatura, "%.01lf ºC", datosApp->termostato.tempUmbral);
	//lv_style_set_text_color(&estilo_temperatura, LV_STATE_DEFAULT, LV_COLOR_TEXTO);

}

void lv_actualizar_estado_alarma_lcd(DATOS_APLICACION *datosApp) {

	int i;
	bool escondido;
	ESP_LOGI(TAG, ""TRAZAR" entramos en lv_actualizar_estado_alarma_lcd", INFOTRAZA);
	for (i=0;i< NUM_TIPOS_ALARMAS; i++) {

		if (datosApp->alarmas[i].estado_alarma == ALARMA_OFF) {
			escondido = true;

		} else {
			escondido = false;
			break;
		}

	}
	lv_obj_set_hidden(boton_alarma, escondido);


}

void lv_actualizar_modo_aplicacion_lcd(enum ESTADO_APP estado) {

	bool esconder;
	ESP_LOGI(TAG, ""TRAZAR" entramos en lv_actualizar_modo_aplicacion_lcd", INFOTRAZA);
	switch (estado) {

	case NORMAL_AUTO:
		//lv_label_set_text(modo_termostato, "AUTO");
		lv_imgbtn_set_src(boton_modo, LV_BTN_STATE_RELEASED, &automatico);
		esconder = false;
		break;
	case NORMAL_AUTOMAN:
		//lv_label_set_text(modo_termostato, "AUTOMAN");
		lv_imgbtn_set_src(boton_modo, LV_BTN_STATE_RELEASED, &automan);
		esconder = false;
		break;
	case NORMAL_MANUAL:
		//lv_label_set_text(modo_termostato, "MANUAL");
		lv_imgbtn_set_src(boton_modo, LV_BTN_STATE_RELEASED, &manual);
		esconder = true;
		break;
	case NORMAL_SIN_PROGRAMACION:
		//lv_label_set_text(modo_termostato, "SIN PROGRAMAS");
		lv_imgbtn_set_src(boton_modo, LV_BTN_STATE_RELEASED, &manual);
		esconder = true;
		break;
	default:
		//lv_label_set_text(modo_termostato, "...");
		lv_imgbtn_set_src(boton_modo, LV_BTN_STATE_RELEASED, &manual);
		esconder = false;
		break;

	}
	lv_obj_set_hidden(intervalo_anterior, esconder);
	lv_obj_set_hidden(intervalo_posterior, esconder);
	lv_obj_set_hidden(barra_temperatura, esconder);
	//lv_obj_set_hidden(arco_temperatura, esconder);



}

void lv_crear_msgbox(lv_obj_t *padre, char* mensaje) {

	popup = lv_msgbox_create(padre, NULL);
	lv_obj_add_style(popup, LV_MSGBOX_PART_BG, &estilo_aplicacion);
	lv_obj_set_style_local_value_align(popup, LV_MSGBOX_PART_BG, LV_STATE_DEFAULT, LV_ALIGN_IN_TOP_LEFT);
	lv_msgbox_set_text(popup, mensaje);
	lv_obj_align(popup, pantalla_smartconfig, LV_ALIGN_CENTER, 0, 0);
	//lv_obj_set_pos(popup, 100, 20);

}


void abrir_popup_reporte_alarmas(lv_obj_t * obj, lv_event_t event) {

	int i;
	char texto[500] = {0};
	char parcial[100] = {0};
	char estado[4] = {0};

	ESP_LOGE(TAG, ""TRAZAR" REPORTE DE ALARMAS*", INFOTRAZA);


	if(event == LV_EVENT_CLICKED) {
		ESP_LOGE(TAG, ""TRAZAR" REPORTE DE ALARMAS", INFOTRAZA);

		//popup = lv_msgbox_create(lv_layer_top(), NULL);

		for (i=0;i<NUM_TIPOS_ALARMAS;i++) {
			if (datosApp.alarmas[i].estado_alarma == ALARMA_ON) {
				strcpy(estado, "NO");
			} else {
				strcpy(estado, "SI");
			}
			sprintf(parcial, "%s........%s\n", datosApp.alarmas[i].nemonico, estado);
			strcat(texto, parcial);
		}
		ESP_LOGE(TAG, ""TRAZAR" REPORTE DE ALARMAS %s", INFOTRAZA, texto);
		lv_crear_msgbox(pantalla_principal, texto);
		lv_msgbox_start_auto_close(popup, 10000);
		//abrir_popup_generico(texto);
		//cerrar_popup_generico(10000);


	}

}

void cerrar_popup_smart_wifi() {

	if (popup != NULL) {
		lv_obj_del(popup);
		popup = NULL;
	}
}

void abrir_popup_smartwifi() {

	popup = lv_msgbox_create(lv_layer_top(), NULL);
	lv_obj_set_pos(popup, 0,0);
	lv_msgbox_set_text(popup, MENSAJE_SMARTWIFI);
	lv_obj_align_mid(popup, pantalla_1, LV_ALIGN_CENTER, 0, 0);
}

void abrir_popup_generico(char *texto) {

	ESP_LOGW(TAG, ""TRAZAR" el  texto del popup es :%s", INFOTRAZA, texto);
    popup = lv_msgbox_create(lv_layer_top(), NULL);
	lv_obj_set_pos(popup, 0,0);
	lv_msgbox_set_text(popup, texto);
	lv_obj_align_mid(popup, pantalla_1, LV_ALIGN_CENTER, 0, 0);

}
/*
void cerrar_popup_generico(uint16_t delay) {

	if (popup != NULL) {
		ESP_LOGW(TAG, ""TRAZAR" Se va a cerrar el popup", INFOTRAZA);
		lv_msgbox_start_auto_close(popup, delay);
		//lv_obj_del(popup);
		//popup = NULL;
		if (popup == NULL) {
			ESP_LOGW(TAG, ""TRAZAR" popup es nulo", INFOTRAZA);


		}
	}


}
*/
/**********************************************************************************/


void lv_crear_estilo_barra() {

	lv_style_init(&estilo_barra);
    /*Set a background color and a radius*/
    lv_style_set_radius(&estilo_barra, LV_STATE_DEFAULT, 5);
    lv_style_set_bg_opa(&estilo_barra, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_style_set_bg_color(&estilo_barra, LV_STATE_DEFAULT, LV_COLOR_FONDO);

    /*Add a repeating pattern*/
    lv_style_set_pattern_image(&estilo_barra, LV_STATE_DEFAULT, LV_SYMBOL_RIGHT);
    lv_style_set_pattern_recolor(&estilo_barra, LV_STATE_DEFAULT, LV_COLOR_TEXTO);
    lv_style_set_pattern_opa(&estilo_barra, LV_STATE_DEFAULT, LV_OPA_50);
    lv_style_set_pattern_repeat(&estilo_barra, LV_STATE_DEFAULT, true);





}


void lv_crear_estilo_arco() {

	lv_style_reset(&estilo_arco);
	lv_style_set_bg_color(&estilo_arco, LV_STATE_DEFAULT, LV_COLOR_FONDO);
	lv_style_set_border_color(&estilo_arco, LV_STATE_DEFAULT, LV_COLOR_FONDO);

	lv_style_set_radius(&estilo_arco, LV_STATE_DEFAULT, 275);
	lv_style_set_outline_color(&estilo_arco, LV_STATE_DEFAULT, LV_COLOR_GREEN);
	lv_style_set_shadow_color(&estilo_arco, LV_STATE_DEFAULT, LV_COLOR_YELLOW);
	lv_style_set_value_color(&estilo_arco, LV_STATE_DEFAULT, LV_COLOR_GREEN);
	lv_style_set_line_width(&estilo_arco, LV_STATE_DEFAULT, 3);
	lv_style_set_scale_end_line_width(&estilo_arco, LV_STATE_DEFAULT, 4);


}



void lv_crear_estilo_area_temperatura() {

	lv_style_init(&estilo_area_temperatura);
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

	lv_style_set_border_color(&estilo_botones_umbral, LV_STATE_PRESSED, LV_COLOR_TEXTO);
	lv_style_set_text_color(&estilo_botones_umbral, LV_STATE_DEFAULT, LV_COLOR_TEXTO);
    lv_style_set_line_rounded(&estilo_botones_umbral, LV_STATE_DEFAULT, false);
    lv_style_set_radius(&estilo_botones_umbral, LV_STATE_DEFAULT, 0);


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
	lv_obj_add_style(objeto, LV_LABEL_PART_MAIN, &estilo_aplicacion);
	lv_obj_set_style_local_text_color(objeto, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_TEXTO);


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
	lv_actualizar_rele_lcd(OFF);



}


void lv_crear_area_temperatura(lv_obj_t *padre) {


	lv_coord_t margen = 5;
	lv_coord_t y;
	lv_coord_t x;
	contenedor_temperatura = lv_cont_create(padre, NULL);
	lv_crear_estilo_area_temperatura();
	lv_obj_add_style(contenedor_temperatura, LV_CONT_PART_MAIN, &estilo_area_temperatura);

	lv_obj_set_state(contenedor_temperatura, LV_STATE_DISABLED);
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
/*
	icono_humedad = lv_img_create(contenedor_temperatura, NULL);
	lv_img_set_src(icono_humedad, &humidity);
	x = 50 + margen;
	lv_obj_set_pos(icono_humedad, x,lv_obj_get_height(texto_temperatura));
	lv_obj_set_height(contenedor_temperatura, lv_obj_get_height(texto_temperatura) + lv_obj_get_height(icono_humedad));

	texto_humedad = lv_label_create(contenedor_temperatura, NULL);
	lv_label_set_text(texto_humedad, "25.4%");
	lv_obj_set_pos(texto_humedad, lv_obj_get_width(icono_humedad) + x + margen,y+ margen );
*/







}

void lv_crear_barra_temperatura(lv_obj_t *padre) {

	//lv_crear_estilo_arco();
	lv_crear_estilo_barra();
	barra_temperatura = lv_bar_create(padre,  NULL);
	lv_bar_set_range(barra_temperatura, 0, 100);
	//lv_obj_add_style(barra_temperatura, LV_BAR_PART_BG , &estilo_barra);
	lv_obj_add_style(barra_temperatura, LV_ARC_PART_INDIC, &estilo_barra);
	lv_obj_set_style_local_bg_color(barra_temperatura, LV_BAR_PART_BG, LV_STATE_DEFAULT, LV_COLOR_FONDO);
	//lv_obj_set_style_local_bg_color(barra_temperatura, LV_ARC_PART_INDIC, LV_STATE_DEFAULT, LV_COLOR_TEXTO);
	//lv_obj_set_style_local_pattern_image(barra_temperatura, LV_ARC_PART_INDIC, LV_STATE_DEFAULT, "E");












}




void lv_crear_arco(lv_obj_t *padre) {


	lv_crear_estilo_arco();
	arco_temperatura = lv_arc_create(padre, NULL);
	lv_obj_add_style(arco_temperatura, LV_ARC_PART_BG, &estilo_arco);
	lv_obj_set_state(arco_temperatura, LV_STATE_DISABLED);
	lv_obj_set_style_local_line_color(arco_temperatura, LV_ARC_PART_INDIC, LV_STATE_DEFAULT, LV_COLOR_TEXTO);
	lv_obj_set_style_local_line_color(arco_temperatura, LV_ARC_PART_BG, LV_STATE_DEFAULT, LV_COLOR_GRAY);
	lv_obj_set_style_local_bg_grad_color(arco_temperatura, LV_ARC_PART_INDIC, LV_STATE_DEFAULT, LV_COLOR_BLUE);
	lv_obj_set_style_local_line_width(arco_temperatura, LV_ARC_PART_INDIC, LV_STATE_DEFAULT, 3);
	lv_obj_set_style_local_line_width(arco_temperatura, LV_ARC_PART_KNOB, LV_STATE_DEFAULT, 3);




	lv_arc_set_end_angle(arco_temperatura, 200);
	lv_obj_set_size(arco_temperatura, 220, 220);
	//lv_arc_set_range(arco_temperatura, 0, 50);
	//lv_arc_set_value(arco_temperatura, 45);



}



void lv_crear_estilo_aplicacion() {

	lv_style_init(&estilo_aplicacion);
	lv_style_set_bg_color(&estilo_aplicacion, LV_STATE_DEFAULT, LV_COLOR_FONDO);
	lv_style_set_bg_color(&estilo_aplicacion, LV_STATE_PRESSED, LV_COLOR_FONDO);
	lv_style_set_bg_color(&estilo_aplicacion, LV_STATE_FOCUSED, LV_COLOR_FONDO);
	lv_style_set_bg_color(&estilo_aplicacion, LV_STATE_CHECKED, LV_COLOR_FONDO);
	lv_style_set_text_color(&estilo_aplicacion, LV_STATE_DEFAULT, LV_COLOR_TEXTO);
	lv_style_set_text_color(&estilo_aplicacion, LV_STATE_DISABLED, LV_COLOR_TEXTO);
	lv_style_set_border_color(&estilo_aplicacion, LV_STATE_DEFAULT, LV_COLOR_FONDO);
	lv_style_set_scale_border_width(&estilo_aplicacion, LV_STATE_DEFAULT, 0);
	lv_style_set_line_color(&estilo_aplicacion, LV_STATE_DEFAULT, LV_COLOR_FONDO);
	lv_style_set_outline_color(&estilo_aplicacion, LV_STATE_DEFAULT, LV_COLOR_FONDO);
	lv_style_set_shadow_color(&estilo_aplicacion, LV_STATE_DEFAULT, LV_COLOR_FONDO);


}

void lv_crear_botones_umbral(lv_obj_t *padre) {

	//contenedor_botones = lv_cont_create(padre, NULL);
	lv_crear_estilo_botones_umbral();
	lv_obj_add_style(padre, LV_CONT_PART_MAIN, &estilo_botones_umbral);
	boton_mas = lv_imgbtn_create(padre, NULL);
	lv_imgbtn_set_src(boton_mas, LV_STATE_DEFAULT, &boton_arriba);
	lv_imgbtn_set_src(boton_mas, LV_STATE_PRESSED, &boton_arriba);
	lv_imgbtn_set_src(boton_mas, LV_STATE_CHECKED, &boton_arriba);

	lv_obj_set_event_cb(boton_mas, manejador_boton_mas);

	lv_btn_set_checkable(boton_mas, false);
	//lv_obj_set_size(boton_mas, 60, 60);
	lv_obj_add_style(boton_mas, LV_CONT_PART_MAIN, &estilo_botones_umbral);

	//lv_obj_set_style_local_value_str(boton_mas, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_SYMBOL_UP);
	//lv_obj_set_style_local_value_align(boton_mas, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_ALIGN_CENTER);
	//lv_obj_set_size(contenedor_botones, lv_obj_get_width(boton_mas), (lv_obj_get_height(boton_mas)*2) + 60);

	//boton_menos = lv_btn_create(contenedor_botones, boton_mas);
	boton_menos = lv_imgbtn_create(padre, NULL);
	lv_imgbtn_set_src(boton_menos, LV_STATE_DEFAULT, &boton_abajo);
	lv_imgbtn_set_src(boton_menos, LV_STATE_CHECKED, &boton_abajo);
	lv_obj_set_event_cb(boton_menos, manejador_boton_menos);
	lv_btn_set_checkable(boton_menos, false);
	//lv_obj_set_size(boton_menos, 60, 60);
	//lv_obj_set_style_local_value_str(boton_menos, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_SYMBOL_DOWN);

}

void lv_crear_umbral_temperatura(lv_obj_t *padre) {


	umbral_temperatura = lv_label_create(padre, NULL);
	lv_obj_add_style(umbral_temperatura, LV_LABEL_PART_MAIN, &estilo_area_temperatura);
	lv_obj_set_style_local_text_font(umbral_temperatura, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_led_30);
	lv_label_set_text_fmt(umbral_temperatura, "21.7 ºC");


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



void lv_crear_boton_modo(lv_obj_t *padre) {

	boton_modo = lv_imgbtn_create(padre, NULL);
	lv_obj_set_event_cb(boton_modo, manejador_boton_modo);
	lv_obj_add_style(boton_modo, LV_STATE_DEFAULT, &estilo_aplicacion);
	lv_imgbtn_set_src(boton_modo, LV_STATE_DEFAULT, &automatico);
	lv_imgbtn_set_src(boton_modo, LV_STATE_CHECKED, &automatico);


}


void lv_cerrar_pantalla_smartconfig() {

	lv_obj_del(pantalla_smartconfig);
}

void lv_crear_pantalla_smartconfig() {

	pantalla_smartconfig = lv_obj_create(NULL, NULL);
	lv_obj_add_style(pantalla_smartconfig, LV_OBJ_PART_MAIN, &estilo_aplicacion);
	lv_crear_msgbox(pantalla_smartconfig, MENSAJE_SMARTWIFI);
}

lv_coord_t calcular_mitad_horizontal(lv_obj_t *objeto) {

	lv_coord_t mitad;


	mitad = LV_HOR_RES_MAX / 2 - (lv_obj_get_width(objeto) / 2);

	return mitad;
}

static void manejador_pantalla_principal(lv_obj_t * obj, lv_event_t event) {

    if(event == LV_EVENT_CLICKED) {
    	ESP_LOGI(TAG, ""TRAZAR" He pulsado fuera de ningun boton", INFOTRAZA);
    	lv_obj_set_style_local_bg_color(pantalla_principal, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_FONDO);

    }



}


void lv_inicio_aplicacion_normal() {

	lv_area_t pos;
	//lv_obj_del(primera_pantalla);
	pantalla_principal = lv_obj_create(NULL, NULL);
	lv_obj_set_event_cb(pantalla_principal, manejador_pantalla_principal);
	lv_obj_add_style(pantalla_principal, LV_OBJ_PART_MAIN, &estilo_aplicacion);
	lv_obj_set_state(pantalla_principal, LV_STATE_DISABLED);

	//lv_scr_load(pantalla_principal);
	/* Creacion de la barra de notificaciones */
	lv_crear_barra_notificaciones(pantalla_principal, 0, 0);


	/* Creamos el area de temperatura */


	/* Creamos la barra de temperatura */

	lv_crear_barra_temperatura(pantalla_principal);

	/* Creamos el arco */
	//lv_crear_arco(pantalla_principal);

	/* Creamos el icono heating */
	lv_crear_icono_heating(pantalla_principal);
	lv_crear_area_temperatura(pantalla_principal);
	/* Posicionamos el icono y el area de temperatura */

	/* Creamos los botones del umbral */
	lv_crear_botones_umbral(pantalla_principal);

	/* Creamos el umbral */
	lv_crear_umbral_temperatura(pantalla_principal);

	/* Creamos el boton alarma */
	lv_crear_boton_alarma(pantalla_principal);
	//lv_obj_set_pos(boton_alarma, pos.x1 + 50, pos.y1 + 80);

	/* Creamos los intervalos */
	lv_crear_intervalos(pantalla_principal);
	/* Creamos el boton modo */

	lv_crear_boton_modo(pantalla_principal);
	//lv_obj_set_pos(boton_modo,pos.x2 - 110, pos.y1 + 70 );


	/*posicionamos los objetos */
	lv_obj_set_pos(contenedor_iconos_notificacion, LV_HOR_RES_MAX - lv_obj_get_width(contenedor_iconos_notificacion), 30);
	//lv_obj_set_pos(arco_temperatura, (LV_HOR_RES/2) - lv_obj_get_width(arco_temperatura)/1.9,30);
	//lv_obj_get_coords(arco_temperatura, &pos);

	pos.x1 = 10;
	pos.y1 = 10;
	pos.x2 = 200;
	pos.y2 = 100;




	//lv_obj_set_pos(rele_termostato, (pos.x2 - pos.x1)/2 + lv_obj_get_width(rele_termostato)/2, pos.y1 + lv_obj_get_height(arco_temperatura)/10);
	/* contenedor temperatura */

	lv_obj_set_pos(contenedor_temperatura, calcular_mitad_horizontal(contenedor_temperatura) - lv_obj_get_width(icono_temperatura), 80);
	//lv_obj_set_pos(contenedor_temperatura, pos.x1 + (pos.x1 /2), pos.y1 + (lv_obj_get_height(arco_temperatura) * 0.5) - (lv_obj_get_height(contenedor_temperatura)/2));
	/* contenedor botones */

	//lv_obj_set_pos(contenedor_botones,
		//	LV_HOR_RES_MAX - lv_obj_get_width(contenedor_botones) - 10,
			//(LV_VER_RES_MAX/2) - lv_obj_get_height(contenedor_botones)/2);
	//lv_obj_set_pos(umbral_temperatura, ((pos.x2 - pos.x1)/2) + lv_obj_get_width(umbral_temperatura)/5,pos.y2 - (lv_obj_get_height(arco_temperatura)/3.5));
	/* intervalos */

	//lv_obj_set_pos(intervalo_anterior, pos.x1 + lv_obj_get_width(arco_temperatura)/10, pos.y2 - 40);
	//lv_obj_set_pos(intervalo_posterior, pos.x2 - lv_obj_get_width(arco_temperatura)/5, pos.y2 -40);
	/* rele termostato */

	lv_obj_get_coords(contenedor_temperatura, &pos);
	lv_obj_set_pos(rele_termostato, pos.x2 + 20, pos.y1);
	/* boton alarma */

	lv_obj_set_pos(boton_alarma,5,LV_VER_RES_MAX/18);
	lv_obj_set_pos(boton_modo,5, LV_VER_RES_MAX - (LV_VER_RES_MAX/18) - lv_obj_get_height(boton_modo));

	/* barra temperatura */

	lv_coord_t pos_ver_barra = LV_VER_RES_MAX - 20;
	lv_coord_t pos_hor_barra = 5;
	lv_obj_set_size(barra_temperatura, LV_HOR_RES_MAX - lv_obj_get_width(intervalo_anterior) - lv_obj_get_width(intervalo_posterior) - 40 ,10);

	lv_obj_set_pos(intervalo_anterior, pos_hor_barra, pos_ver_barra - 5);
	lv_obj_set_pos(barra_temperatura, pos_hor_barra + lv_obj_get_width(intervalo_anterior) + 5, pos_ver_barra);
	lv_obj_set_pos(intervalo_posterior, pos_hor_barra + lv_obj_get_width(intervalo_anterior) + lv_obj_get_width(barra_temperatura) + 10, pos_ver_barra - 5 );
	/* umbral de temperatura */

	lv_obj_set_pos(umbral_temperatura, calcular_mitad_horizontal(umbral_temperatura) - 10, pos_ver_barra - lv_obj_get_height(barra_temperatura) - 50);

	/* boton modo */
	lv_obj_get_coords(contenedor_temperatura, &pos);
	lv_obj_set_pos(boton_modo,pos.x1 + 30, pos.y1 - 50);

	/* botones umbral */
	lv_obj_get_coords(umbral_temperatura, &pos);
	ESP_LOGW(TAG, ""TRAZAR" x1: %d y1%d", INFOTRAZA, pos.x1, pos.y1);
	lv_obj_set_pos(boton_menos, pos.x1 - 60, pos.y1 - 10);
	lv_obj_set_pos(boton_mas, pos.x2 + 20, pos.y1 - 10);




}


void lv_pantalla_cronotermostato_app(DATOS_APLICACION *datosApp) {




	ESP_LOGE(TAG, "COMENZAMOS LA TAREA LCD");
	lv_crear_estilo_aplicacion();

	lv_inicio_aplicacion_normal();
	ESP_LOGE(TAG, "Hemos cargado la pantalla principal pero no la enseñamos aun");
	if (datosApp->datosGenerales->estadoApp == ARRANQUE_FABRICA) {
		ESP_LOGI(TAG, "creamos la pantalla smartconfig");
		lv_crear_pantalla_smartconfig();
		lv_scr_load(pantalla_smartconfig);
	} else {
		ESP_LOGE(TAG, "");


		lv_crear_pantalla_inicializando("Inicializando\nEspere un momento...");
		ESP_LOGW(TAG, ""TRAZAR" ESTAMOS EN INICIALIZANDO", INFOTRAZA);
		lv_scr_load(primera_pantalla);
		ESP_LOGW(TAG, ""TRAZAR" DESPUES ESTAMOS EN INICIALIZANDO", INFOTRAZA);

	}


}


void tarea_lcd(void *pvParameter) {

    (void) pvParameter;
    DATOS_APLICACION *datosApp;



    datosApp = (DATOS_APLICACION*) pvParameter;

    xGuiSemaphore = xSemaphoreCreateMutex();

    lv_init();

    /* Initialize SPI or I2C bus used by the drivers */
    lvgl_driver_init();

    static lv_color_t buf1[DISP_BUF_SIZE];

    /* Use double buffered when not working with monochrome displays */
#ifndef CONFIG_LV_TFT_DISPLAY_MONOCHROME
    static lv_color_t buf2[DISP_BUF_SIZE];
#else
    static lv_color_t *buf2 = NULL;
#endif

    static lv_disp_buf_t disp_buf;

    uint32_t size_in_px = DISP_BUF_SIZE;

#if defined CONFIG_LV_TFT_DISPLAY_CONTROLLER_IL3820 \
    || defined CONFIG_LV_TFT_DISPLAY_CONTROLLER_JD79653A \
    || defined CONFIG_LV_TFT_DISPLAY_CONTROLLER_UC8151D

    /* Actual size in pixels, not bytes. */
    size_in_px *= 8;
#endif

    /* Initialize the working buffer depending on the selected display.
     * NOTE: buf2 == NULL when using monochrome displays. */
    lv_disp_buf_init(&disp_buf, buf1, buf2, size_in_px);

    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.flush_cb = disp_driver_flush;

    /* When using a monochrome display we need to register the callbacks:
     * - rounder_cb
     * - set_px_cb */
#ifdef CONFIG_LV_TFT_DISPLAY_MONOCHROME
    disp_drv.rounder_cb = disp_driver_rounder;
    disp_drv.set_px_cb = disp_driver_set_px;
#endif

    disp_drv.buffer = &disp_buf;
    lv_disp_drv_register(&disp_drv);

    /* Register an input device when enabled on the menuconfig */
#if CONFIG_LV_TOUCH_CONTROLLER != TOUCH_CONTROLLER_NONE
    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.read_cb = touch_driver_read;
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    lv_indev_drv_register(&indev_drv);
#endif

    /* Create and start a periodic timer interrupt to call lv_tick_inc */
    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &lv_tick_task,
        .name = "periodic_gui"
    };
    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, LV_TICK_PERIOD_MS * 1000));

    /* Create the demo application */
    //create_demo_application();
    lv_pantalla_cronotermostato_app(datosApp);
    //inicio_lcd(datosApp);
    //msgbox_create();

    while (1) {
        /* Delay 1 tick (assumes FreeRTOS tick is 10ms */
        vTaskDelay(pdMS_TO_TICKS(10));

        /* Try to take the semaphore, call lvgl related function on success */
        if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY)) {
            lv_task_handler();
            xSemaphoreGive(xGuiSemaphore);
       }
    }

    /* A task should NEVER return */
    vTaskDelete(NULL);
}

void lv_cargar_pantalla_principal() {
	if (primera_pantalla != NULL) {
		lv_obj_del(primera_pantalla);
		primera_pantalla = NULL;
	}

	lv_scr_load(pantalla_principal);


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




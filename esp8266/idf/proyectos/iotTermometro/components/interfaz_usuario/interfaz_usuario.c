/*
 * interfaz_usuario.c
 *
 *  Created on: 21 sept. 2020
 *      Author: t126401
 */

#include "dialogos_json.h"
#include "interfaz_usuario.h"
#include "configuracion_usuario.h"
#include "esp_err.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "api_json.h"
#include "driver/gpio.h"
#include "mqtt.h"
#include "programmer.h"
#include "conexiones.h"
#include "nvs.h"
#include "alarmas.h"
#include "funciones_usuario.h"
#include "configuracion.h"
#include "dht22.h"


#define CADENCIA_WIFI 250
#define CADENCIA_BROKER 300
#define CADENCIA_SMARTCONFIG 80
#define CADENCIA_SNTP 1000
#define CADENCIA_ALARMA 1500






static const char *TAG = "INTERFAZ_USUARIO";


static ETSTimer temporizador_general;





void parapadeo_led() {

	static bool luz = false;

	if (luz == false) {
		gpio_set_level(CONFIG_GPIO_PIN_LED, ON);
		luz = true;
	} else {
		gpio_set_level(CONFIG_GPIO_PIN_LED, OFF);
		luz = false;
	}


}

esp_err_t appuser_configuracion_defecto(DATOS_APLICACION *datosApp) {

	ESP_LOGI(TAG, ""TRAZAR"Ejecutando configuraciones adicionales de la aplicacion por defecto...", INFOTRAZA);
	datosApp->datosGenerales->tipoDispositivo = CRONOTERMOSTATO;
    //Escribe aqui el codigo de inicializacion por defecto de la aplicacion.
	// Esta funcion es llamada desde el componente configuracion defaultConfig.
	// Aqui puedes establecer los valores por defecto para tu aplicacion.

    datosApp->termostato.reintentosLectura = 5;
    datosApp->termostato.intervaloReintentos = 3;
    datosApp->termostato.margenTemperatura = 0.5;
    datosApp->termostato.intervaloLectura = 10;
    datosApp->termostato.calibrado = 0;

	return ESP_OK;
}

esp_err_t appUser_ejecutar_accion_programa_defecto(DATOS_APLICACION *datosApp) {

	ESP_LOGI(TAG, ""TRAZAR"Ejecutando acciones de programa por defecto", INFOTRAZA);
	//Escribe aquí lo que quieres que tu aplicacion haga tenga que ejecutar una accion por defecto
    //GPIO_OUTPUT_SET(GPIO_ID_PIN(GPIO_RELE), OFF);

    return ESP_OK;
}


esp_err_t appuser_notificar_smartconfig() {

	ets_timer_disarm(&temporizador_general);
	ets_timer_disarm(&temporizador_general);
	ets_timer_setfn(&temporizador_general, (ETSTimerFunc*) parapadeo_led, NULL);
	ets_timer_arm(&temporizador_general, CADENCIA_SMARTCONFIG, true);


	return ESP_OK;
}


esp_err_t appuser_arranque_aplicacion(DATOS_APLICACION *datosApp) {

	ESP_LOGI(TAG, ""TRAZAR" Ejecutando acciones finales de arranque", INFOTRAZA);

	leer_temperatura_local(datosApp);

	notificar_fin_arranque(datosApp);

	return ESP_OK;
}

esp_err_t appuser_acciones_ota(DATOS_APLICACION *datosApp) {


	//datosApp->datosGenerales->ota.puerto = 80;
	ESP_LOGI(TAG, ""TRAZAR"PUERTO: %d", INFOTRAZA, datosApp->datosGenerales->ota.puerto);
	//ESP_LOGI(TAG, ""TRAZAR"PUERTO OTRA VEZ: %d", datosApp->datosGenerales->ota.puerto);
	ESP_LOGI(TAG, ""TRAZAR"servidor ota: %s\n, puerto: %d\n, url: %s, version %d", INFOTRAZA,
			datosApp->datosGenerales->ota.server, datosApp->datosGenerales->ota.puerto, datosApp->datosGenerales->ota.url, datosApp->datosGenerales->ota.swVersion);


	return ESP_OK;
}

esp_err_t appuser_obteniendo_sntp() {

	ets_timer_disarm(&temporizador_general);
	ets_timer_setfn(&temporizador_general, (ETSTimerFunc*) parapadeo_led, NULL);
	ets_timer_arm(&temporizador_general, CADENCIA_SNTP, true);

	return ESP_OK;
}
esp_err_t app_user_error_obteniendo_sntp() {

	ets_timer_disarm(&temporizador_general);
	ets_timer_setfn(&temporizador_general, (ETSTimerFunc*) parapadeo_led, NULL);
	ets_timer_arm(&temporizador_general, CADENCIA_SNTP, true);

	return ESP_OK;
}

esp_err_t appuser_sntp_ok() {

	ets_timer_disarm(&temporizador_general);
	gpio_set_level(CONFIG_GPIO_PIN_LED, ON);
	return ESP_OK;
}



esp_err_t appuser_wifi_conectando() {


	ets_timer_disarm(&temporizador_general);
	ets_timer_setfn(&temporizador_general, (ETSTimerFunc*) parapadeo_led, NULL);
	ets_timer_arm(&temporizador_general, CADENCIA_WIFI, true);


	return ESP_OK;
}

esp_err_t appuser_wifi_conectado() {


	ets_timer_disarm(&temporizador_general);
	gpio_set_level(CONFIG_GPIO_PIN_LED, ON);


	return ESP_OK;
}

esp_err_t appuser_broker_conectando() {

	ESP_LOGI(TAG, ""TRAZAR"BROKER CONECTANDO", INFOTRAZA);
	ets_timer_disarm(&temporizador_general);
	ets_timer_setfn(&temporizador_general, (ETSTimerFunc*) parapadeo_led, NULL);
	ets_timer_arm(&temporizador_general, CADENCIA_BROKER, true);

	return ESP_OK;
}
esp_err_t appuser_broker_conectado() {

	ets_timer_disarm(&temporizador_general);
	gpio_set_level(CONFIG_GPIO_PIN_LED, ON);

	return ESP_OK;
}
esp_err_t appuser_broker_desconectado() {

	ets_timer_disarm(&temporizador_general);
	ets_timer_setfn(&temporizador_general, (ETSTimerFunc*) parapadeo_led, NULL);
	ets_timer_arm(&temporizador_general, CADENCIA_BROKER, true);

	return ESP_OK;
}





void appuser_ejecucion_accion_temporizada(void *datosApp) {

}

esp_err_t appuser_temporizador_cumplido(DATOS_APLICACION *datosApp) {

	return ESP_OK;
}

esp_err_t appuser_notificar_alarma_localmente(DATOS_APLICACION *datosApp, uint8_t indice) {

	if (datosApp->alarmas[indice].estado_alarma == ALARMA_OFF) {
		ets_timer_disarm(&temporizador_general);
		ESP_LOGI(TAG, ""TRAZAR" ALARMA OFF NOTIFICADA", INFOTRAZA);
	} else {
		ets_timer_disarm(&temporizador_general);
		ets_timer_setfn(&temporizador_general, (ETSTimerFunc*) parapadeo_led, NULL);
		ets_timer_arm(&temporizador_general, CADENCIA_ALARMA, true);
		ESP_LOGI(TAG, ""TRAZAR" ALARMA ON NOTIFICADA", INFOTRAZA);

	}


	return ESP_OK;
}

cJSON* appuser_generar_informe_espontaneo(DATOS_APLICACION *datosApp, enum TIPO_INFORME tipoInforme, cJSON *comandoOriginal) {


    cJSON *respuesta = NULL;


    respuesta = cabecera_espontaneo(datosApp, tipoInforme);
    switch(tipoInforme) {
    case ARRANQUE_APLICACION:
        cJSON_AddNumberToObject(respuesta, DEVICE_STATE, datosApp->datosGenerales->estadoApp);
        cJSON_AddNumberToObject(respuesta, TEMPERATURA, datosApp->termostato.tempActual);
        cJSON_AddNumberToObject(respuesta, HUMEDAD, datosApp->termostato.humedad);
        escribir_programa_actual(datosApp, respuesta);
        break;
    case ACTUACION_RELE_LOCAL:


        break;
    case CAMBIO_DE_PROGRAMA:

        break;
    case CAMBIO_TEMPERATURA:
        cJSON_AddNumberToObject(respuesta, DEVICE_STATE, datosApp->datosGenerales->estadoApp);
        cJSON_AddNumberToObject(respuesta, TEMPERATURA, datosApp->termostato.tempActual);
        cJSON_AddNumberToObject(respuesta, HUMEDAD, datosApp->termostato.humedad);
        escribir_programa_actual(datosApp, respuesta);
        break;

        default:
            codigoRespuesta(respuesta, RESP_NOK);
            printf("enviarReporte--> Salida no prevista\n");
            break;
    }

    return respuesta;

}
esp_err_t appuser_cargar_programa_especifico(DATOS_APLICACION *datosApp, TIME_PROGRAM *programa_actual, cJSON *nodo) {



	return ESP_OK;
}

esp_err_t appuser_configuracion_a_json(DATOS_APLICACION *datosApp, cJSON *conf) {


	ESP_LOGI(TAG, ""TRAZAR"SE CARGAN DATOS PARTICULARES DE LA APLICACION", INFOTRAZA);
    cJSON_AddNumberToObject(conf, MARGEN_TEMPERATURA, datosApp->termostato.margenTemperatura);
    cJSON_AddNumberToObject(conf, INTERVALO_LECTURA, datosApp->termostato.intervaloLectura);
    cJSON_AddNumberToObject(conf, INTERVALO_REINTENTOS, datosApp->termostato.intervaloReintentos);
    cJSON_AddNumberToObject(conf, REINTENTOS_LECTURA, datosApp->termostato.reintentosLectura);
    cJSON_AddNumberToObject(conf, CALIBRADO, datosApp->termostato.calibrado);

	return ESP_OK;
}

esp_err_t appuser_json_a_datos(DATOS_APLICACION *datosApp, cJSON *datos) {

	extraer_dato_double(datos, MARGEN_TEMPERATURA, &datosApp->termostato.margenTemperatura);
	extraer_dato_uint8(datos, INTERVALO_LECTURA, &datosApp->termostato.intervaloLectura);
	extraer_dato_uint8(datos, INTERVALO_REINTENTOS, &datosApp->termostato.intervaloReintentos);
	extraer_dato_uint8(datos, REINTENTOS_LECTURA, &datosApp->termostato.reintentosLectura);
	extraer_dato_double(datos, CALIBRADO, &datosApp->termostato.calibrado);

	return ESP_OK;
}

esp_err_t appuser_cargar_programas_defecto(DATOS_APLICACION *datosApp, cJSON *array) {



	return ESP_OK;
}

esp_err_t appuser_extraer_datos_programa(TIME_PROGRAM *programa_actual, cJSON *nodo) {




	return ESP_OK;
}

esp_err_t appuser_modificar_dato_programa(TIME_PROGRAM *programa_actual,cJSON *nodo) {



	return ESP_OK;
}

esp_err_t appuser_visualizar_dato_programa(TIME_PROGRAM *programa_actual, cJSON *nodo) {


	return ESP_OK;
}

esp_err_t appuser_modificarConfApp(cJSON *root, DATOS_APLICACION *datosApp, cJSON *respuesta) {

   cJSON *nodo;

   nodo = cJSON_GetObjectItem(root, APP_PARAMS);
   if(nodo == NULL) {
       codigoRespuesta(respuesta, RESP_NOK);
       return ESP_FAIL;
   }

   extraer_dato_double(nodo, MARGEN_TEMPERATURA, &datosApp->termostato.margenTemperatura);
   extraer_dato_uint8(nodo, INTERVALO_LECTURA, &datosApp->termostato.intervaloLectura);
   extraer_dato_uint8(nodo, INTERVALO_REINTENTOS, &datosApp->termostato.intervaloReintentos);
   extraer_dato_uint8(nodo, REINTENTOS_LECTURA, &datosApp->termostato.reintentosLectura);
   extraer_dato_double(nodo, CALIBRADO, &datosApp->termostato.calibrado);
   codigoRespuesta(respuesta, RESP_OK);

   return salvar_configuracion_general(datosApp);

}

esp_err_t app_user_mensaje_recibido_especifico(DATOS_APLICACION *datosApp) {


	return ESP_OK;
}

esp_err_t notificar_fin_arranque(DATOS_APLICACION *datosApp) {

	cJSON *informe;
	if (datosApp->datosGenerales->estadoApp == NORMAL_ARRANCANDO) {
		datosApp->datosGenerales->estadoApp = NORMAL_AUTO;
		informe = appuser_generar_informe_espontaneo(datosApp, ARRANQUE_APLICACION, NULL);

		if (informe != NULL) {
			publicar_mensaje_json(datosApp, informe, NULL);
		}

	}

	return ESP_OK;

}

esp_err_t appuser_cambiar_modo_aplicacion(DATOS_APLICACION *datosApp, enum ESTADO_APP estado) {

	return ESP_OK;
}

void nemonicos_alarmas(DATOS_APLICACION *datosApp, int i) {

	switch(i) {
	case 0:
		strncpy(datosApp->alarmas[i].nemonico, "WiFi................", 50);
		break;
	case 1:
		strncpy(datosApp->alarmas[i].nemonico, "Servidor Mqtt.......", 50);
		break;
	case 2:
		strncpy(datosApp->alarmas[i].nemonico, "Reloj...............", 50);
		break;
	case 3:
		strncpy(datosApp->alarmas[i].nemonico, "Nvs.................", 50);
		break;
	case 4:
		strncpy(datosApp->alarmas[i].nemonico, "Sensor local........", 50);
		break;
	case 5:
		strncpy(datosApp->alarmas[i].nemonico, "sensor remoto.......", 50);
		break;
	default:
		strncpy(datosApp->alarmas[i].nemonico, "alarma no registrada", 50);
		break;

	}


}

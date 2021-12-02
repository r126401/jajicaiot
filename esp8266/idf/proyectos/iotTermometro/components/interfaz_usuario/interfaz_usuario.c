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
#include "conexiones_mqtt.h"
#include "programmer.h"
#include "conexiones.h"
#include "nvslib.h"
#include "alarmas.h"
#include "funciones_usuario.h"
#include "esp_timer.h"
#include "configuracion.h"

#define CADENCIA_WIFI 250
#define CADENCIA_BROKER 300
#define CADENCIA_SMARTCONFIG 80
#define CADENCIA_SNTP 1000
#define CADENCIA_ALARMA 1500



static const char *TAG = "INTERFAZ_USUARIO";


//static os_timer_t temporizador_general;
static esp_timer_handle_t temporizador_nuevo = NULL;




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

void eliminar_temporizacion(char* nombre) {

	if (temporizador_nuevo != NULL) {
		ESP_LOGW(TAG, ""TRAZAR" elminando temporizador %s ", INFOTRAZA, nombre);
		esp_timer_stop(temporizador_nuevo);
		esp_timer_delete(temporizador_nuevo);
		temporizador_nuevo = NULL;
	}

}

void aplicar_temporizacion(int cadencia, esp_timer_cb_t funcion, char* nombre) {

	esp_timer_create_args_t temporizador;

	ESP_LOGW(TAG, ""TRAZAR" aplicando temporizador %s ", INFOTRAZA, nombre);
	eliminar_temporizacion(nombre);
	temporizador.arg = NULL;
	temporizador.callback = funcion;
	temporizador.name = nombre;

	esp_timer_create(&temporizador, &temporizador_nuevo);
	esp_timer_start_periodic(temporizador_nuevo, cadencia * 1000);


}

esp_err_t appuser_configuracion_defecto(DATOS_APLICACION *datosApp) {

	ESP_LOGI(TAG, ""TRAZAR"Ejecutando configuraciones adicionales de la aplicacion por defecto...", INFOTRAZA);
	datosApp->datosGenerales->tipoDispositivo = INTERRUPTOR;
    //Escribe aqui el codigo de inicializacion por defecto de la aplicacion.
	// Esta funcion es llamada desde el componente configuracion defaultConfig.
	// Aqui puedes establecer los valores por defecto para tu aplicacion.

	return ESP_OK;
}




esp_err_t appUser_ejecutar_accion_programa_defecto(DATOS_APLICACION *datosApp) {

	ESP_LOGI(TAG, ""TRAZAR"Ejecutando acciones de programa por defecto", INFOTRAZA);
	//Escribe aquí lo que quieres que tu aplicacion haga tenga que ejecutar una accion por defecto
    //GPIO_OUTPUT_SET(GPIO_ID_PIN(GPIO_RELE), OFF);

    return ESP_OK;
}


esp_err_t appuser_notificar_smartconfig() {


	aplicar_temporizacion(CADENCIA_SMARTCONFIG, parapadeo_led, "smartconfig");

	return ESP_OK;
}

esp_err_t appuser_arranque_aplicacion(DATOS_APLICACION *datosApp) {

	cJSON *informe;

	informe = appuser_generar_informe_espontaneo(datosApp, ARRANQUE_APLICACION, NULL);

	ESP_LOGI(TAG, ""TRAZAR" vamos a publicar el arranque del dispositivo", INFOTRAZA);
	if (informe != NULL) {
		publicar_mensaje_json(datosApp, informe, NULL);
		ESP_LOGI(TAG, ""TRAZAR" PUBLICADO", INFOTRAZA);
	}
	datosApp->datosGenerales->estadoApp = NORMAL_AUTO;

	return ESP_OK;
}

esp_err_t appuser_acciones_ota(DATOS_APLICACION *datosApp) {

	esp_err_t error;

	//inicializar_nvs(CONFIG_NAMESPACE, &datosApp->handle);
	error = ota_a_json(datosApp);
	if (error == ESP_OK) {
		ESP_LOGW(TAG, ""TRAZAR" PREPARADO PARA UPGRADE...", INFOTRAZA);
		esp_restart();
	}



	//escribir_dato_string_nvs(datosApp->handle, CONFIG_CLAVE_PROGRAMACION, texto);

	return ESP_OK;
}

esp_err_t appuser_obteniendo_sntp() {

	aplicar_temporizacion(CADENCIA_SNTP, parapadeo_led, "sntp");
	return ESP_OK;
}
esp_err_t app_user_error_obteniendo_sntp() {

	aplicar_temporizacion(CADENCIA_SNTP, parapadeo_led, "sntp_error");
	return ESP_OK;
}

esp_err_t appuser_sntp_ok() {

	eliminar_temporizacion("sntp");
	gpio_set_level(CONFIG_GPIO_PIN_LED, ON);
	return ESP_OK;
}



esp_err_t appuser_wifi_conectando() {


	aplicar_temporizacion(CADENCIA_WIFI, parapadeo_led, "wifi");


	return ESP_OK;
}

esp_err_t appuser_wifi_conectado() {


	eliminar_temporizacion("wifi");
	//os_timer_disarm(&temporizador_general);
	gpio_set_level(CONFIG_GPIO_PIN_LED, ON);


	return ESP_OK;
}

esp_err_t appuser_broker_conectando() {

	ESP_LOGI(TAG, ""TRAZAR"BROKER CONECTANDO", INFOTRAZA);
	aplicar_temporizacion(CADENCIA_BROKER, parapadeo_led, "mqtt");
	return ESP_OK;
}
esp_err_t appuser_broker_conectado() {

	eliminar_temporizacion("mqtt");
	gpio_set_level(CONFIG_GPIO_PIN_LED, ON);

	return ESP_OK;
}
esp_err_t appuser_broker_desconectado() {

	aplicar_temporizacion(CADENCIA_BROKER, parapadeo_led, "mqtt_error");
	return ESP_OK;
}





void appuser_ejecucion_accion_temporizada(void *datosApp) {


}

esp_err_t appuser_temporizador_cumplido(DATOS_APLICACION *datosApp) {



	return ESP_OK;
}

esp_err_t appuser_notificar_alarma_localmente(DATOS_APLICACION *datosApp, uint8_t indice) {

	if (datosApp->alarmas[indice].estado_alarma == ALARMA_OFF) {
		//eliminar_temporizacion();
		//os_timer_disarm(&temporizador_general);
		ESP_LOGI(TAG, ""TRAZAR" ALARMA OFF NOTIFICADA", INFOTRAZA);
	} else {
		//aplicar_temporizacion(CADENCIA_ALARMA, parapadeo_led);
		/*
		os_timer_disarm(&temporizador_general);
		os_timer_setfn(&temporizador_general, (os_timer_func_t*) parapadeo_led, NULL);
		os_timer_arm(&temporizador_general, CADENCIA_ALARMA, true);
		*/
		ESP_LOGI(TAG, ""TRAZAR" ALARMA ON NOTIFICADA", INFOTRAZA);

	}


	return ESP_OK;
}

cJSON* appuser_generar_informe_espontaneo(DATOS_APLICACION *datosApp, enum TIPO_INFORME tipoInforme, cJSON *comandoOriginal) {


    cJSON *respuesta = NULL;


    respuesta = cabecera_espontaneo(datosApp, tipoInforme);
    switch(tipoInforme) {
        case ARRANQUE_APLICACION:
            printf("generarReporte--> enviando arranqueAplicacion");
            cJSON_AddNumberToObject(respuesta, APP_COMAND_ESTADO_RELE, gpio_get_level(CONFIG_GPIO_PIN_RELE));
            cJSON_AddNumberToObject(respuesta, PROGRAMMER_STATE, datosApp->datosGenerales->estadoProgramacion);
            cJSON_AddNumberToObject(respuesta, DEVICE_STATE, datosApp->datosGenerales->estadoApp);
            escribir_programa_actual(datosApp, respuesta);
            codigoRespuesta(respuesta,RESP_OK);
            break;
        case ACTUACION_RELE_LOCAL:
            cJSON_AddNumberToObject(respuesta, APP_COMAND_ESTADO_RELE, gpio_get_level(CONFIG_GPIO_PIN_RELE));
            cJSON_AddNumberToObject(respuesta, PROGRAMMER_STATE, datosApp->datosGenerales->estadoProgramacion);
            cJSON_AddNumberToObject(respuesta, DEVICE_STATE, datosApp->datosGenerales->estadoApp);
            escribir_programa_actual(datosApp, respuesta);
            codigoRespuesta(respuesta,RESP_OK);

            break;
        case CAMBIO_DE_PROGRAMA:
            cJSON_AddNumberToObject(respuesta, PROGRAMMER_STATE, datosApp->datosGenerales->estadoProgramacion);
            cJSON_AddNumberToObject(respuesta, APP_COMAND_ESTADO_RELE, gpio_get_level(CONFIG_GPIO_PIN_RELE));
            cJSON_AddNumberToObject(respuesta, DEVICE_STATE, datosApp->datosGenerales->estadoApp);
            escribir_programa_actual(datosApp, respuesta);
            codigoRespuesta(respuesta,RESP_OK);
            break;
        case RELE_TEMPORIZADO:
            cJSON_AddNumberToObject(respuesta, PROGRAMMER_STATE, datosApp->datosGenerales->estadoProgramacion);
            cJSON_AddNumberToObject(respuesta, APP_COMAND_ESTADO_RELE, gpio_get_level(CONFIG_GPIO_PIN_RELE));
            cJSON_AddNumberToObject(respuesta, DEVICE_STATE, datosApp->datosGenerales->estadoApp);
            escribir_programa_actual(datosApp, respuesta);
            codigoRespuesta(respuesta,RESP_OK);
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

	//cJSON_AddNumberToObject(conf, DEVICE , INTERRUPTOR);
	ESP_LOGI(TAG, ""TRAZAR" CONFIGURACION A JSON DEL DISPOSITIVO...", INFOTRAZA);
	return ESP_OK;
}

esp_err_t appuser_json_a_datos(DATOS_APLICACION *datosApp, cJSON *datos) {

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

	return ESP_OK;

}

esp_err_t app_user_mensaje_recibido_especifico(DATOS_APLICACION *datosApp) {


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

esp_err_t appuser_cambiar_modo_aplicacion(DATOS_APLICACION *datosApp, enum ESTADO_APP estado) {



	return ESP_OK;
}

esp_err_t appuser_actualizar_gestion_programas(DATOS_APLICACION *datosApp) {


	return ESP_OK;
}



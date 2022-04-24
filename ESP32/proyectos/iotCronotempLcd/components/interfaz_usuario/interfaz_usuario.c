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
#include "configuracion.h"
#include "dht22.h"



#define CADENCIA_WIFI 250
#define CADENCIA_BROKER 300
#define CADENCIA_SMARTCONFIG 80
#define CADENCIA_SNTP 1000
#define CADENCIA_ALARMA 1500






static const char *TAG = "INTERFAZ_USUARIO";


static ETSTimer temporizador_general;
extern ETSTimer temporizador_lectura_remota;




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
    datosApp->termostato.tempUmbral = 21.5;
    datosApp->termostato.tempUmbralDefecto = 21.5;
    datosApp->termostato.calibrado = -3.5;
    datosApp->termostato.master = true;
    datosApp->termostato.incdec = 0.1;
    memset(datosApp->termostato.sensor_remoto, 0, sizeof(datosApp->termostato.sensor_remoto));

	return ESP_OK;
}

esp_err_t appUser_ejecutar_accion_programa_defecto(DATOS_APLICACION *datosApp) {

	ESP_LOGI(TAG, ""TRAZAR"Ejecutando acciones de programa por defecto", INFOTRAZA);
	//Escribe aquí lo que quieres que tu aplicacion haga tenga que ejecutar una accion por defecto
    //GPIO_OUTPUT_SET(GPIO_ID_PIN(GPIO_RELE), OFF);

    return ESP_OK;
}


esp_err_t appuser_notificar_smartconfig(DATOS_APLICACION *datosApp) {

	ets_timer_disarm(&temporizador_general);
	ets_timer_setfn(&temporizador_general, (ETSTimerFunc*) parapadeo_led, NULL);
	ets_timer_arm(&temporizador_general, CADENCIA_SMARTCONFIG, true);
	ESP_LOGI(TAG, ""TRAZAR" EJECUTANDO SMARTCONFIG", INFOTRAZA);
	//esconder_pantalla_principal(true);
	//cerrar_popup_generico(0);
	//abrir_popup_smartwifi();


	return ESP_OK;
}

esp_err_t suscribir_sensor_remoto(DATOS_APLICACION *datosApp) {

	char topic[55] = {0};
	strcpy(topic, "/pub_");
	strcat(topic, datosApp->termostato.sensor_remoto);
	if (esp_mqtt_client_subscribe(datosApp->handle_mqtt->client, topic,datosApp->datosGenerales->parametrosMqtt.qos) >= 0) {
		ESP_LOGW(TAG, ""TRAZAR" suscrito al topic %s", INFOTRAZA, topic);
		return ESP_OK;
	}
	return ESP_FAIL;
}

esp_err_t appuser_arranque_aplicacion(DATOS_APLICACION *datosApp) {

	ESP_LOGI(TAG, ""TRAZAR" Ejecutando acciones finales de arranque", INFOTRAZA);
	if (!datosApp->termostato.master) {
		if ((suscribir_sensor_remoto(datosApp) != ESP_OK)) {
			registrar_alarma(datosApp, NOTIFICACION_ALARMA_SENSOR_REMOTO, ALARMA_SENSOR_REMOTO, ALARMA_ON, true);
		}
	}
	notificar_fin_arranque(datosApp);
	leer_temperatura(datosApp);
/*
	if (datosApp->termostato.master) {
		datosApp->datosGenerales->estadoApp = NORMAL_AUTO;
		informe = appuser_generar_informe_espontaneo(datosApp, ARRANQUE_APLICACION, NULL);

		if (informe != NULL) {
			publicar_mensaje_json(datosApp, informe, NULL);
		}

	}
*/

	return ESP_OK;
}

esp_err_t appuser_acciones_ota(DATOS_APLICACION *datosApp) {


	//datosApp->datosGenerales->ota.puerto = 80;
	ESP_LOGI(TAG, ""TRAZAR"PUERTO: %d", INFOTRAZA, datosApp->datosGenerales->ota.puerto);
	//ESP_LOGI(TAG, ""TRAZAR"PUERTO OTRA VEZ: %d", datosApp->datosGenerales->ota.puerto);
	ESP_LOGI(TAG, ""TRAZAR"servidor ota: %s\n, puerto: %d\n, url: %s, version %s", INFOTRAZA,
			datosApp->datosGenerales->ota.server, datosApp->datosGenerales->ota.puerto, datosApp->datosGenerales->ota.url, datosApp->datosGenerales->ota.newVersion);


	return ESP_OK;
}

esp_err_t appuser_obteniendo_sntp(DATOS_APLICACION *datosApp) {

	ESP_LOGI(TAG, ""TRAZAR" obteniendo sntp", INFOTRAZA);
	ets_timer_disarm(&temporizador_general);
	ets_timer_setfn(&temporizador_general, (ETSTimerFunc*) parapadeo_led, NULL);
	ets_timer_arm(&temporizador_general, CADENCIA_SNTP, true);



	return ESP_OK;
}
esp_err_t app_user_error_obteniendo_sntp(DATOS_APLICACION *datosApp) {

	ets_timer_disarm(&temporizador_general);
	ets_timer_setfn(&temporizador_general, (ETSTimerFunc*) parapadeo_led, NULL);
	ets_timer_arm(&temporizador_general, CADENCIA_SNTP, true);


	return ESP_OK;
}

esp_err_t appuser_sntp_ok(DATOS_APLICACION *datosApp) {

	ets_timer_disarm(&temporizador_general);
	gpio_set_level(CONFIG_GPIO_PIN_LED, ON);
	lv_actualizar_hora_lcd(datosApp);
	lv_actualizar_estado_ntp_lcd(datosApp, COLOR_ICONO_OK);
	return ESP_OK;
}



esp_err_t appuser_wifi_conectando(DATOS_APLICACION *datosApp) {


	ets_timer_disarm(&temporizador_general);
	ets_timer_setfn(&temporizador_general, (ETSTimerFunc*) parapadeo_led, NULL);
	ets_timer_arm(&temporizador_general, CADENCIA_WIFI, true);



	return ESP_OK;
}

esp_err_t appuser_wifi_conectado(DATOS_APLICACION *datosApp) {


	ets_timer_disarm(&temporizador_general);
	gpio_set_level(CONFIG_GPIO_PIN_LED, ON);
	lv_actualizar_estado_wifi_lcd(datosApp, COLOR_ICONO_OK);
	datosApp->datosGenerales->estadoApp = NORMAL_ARRANCANDO;
	//cerrar_popup_smart_wifi();




	return ESP_OK;
}

esp_err_t appuser_broker_conectando(DATOS_APLICACION *datosApp) {

	ESP_LOGI(TAG, ""TRAZAR"BROKER CONECTANDO", INFOTRAZA);
	ets_timer_disarm(&temporizador_general);
	ets_timer_setfn(&temporizador_general, (ETSTimerFunc*) parapadeo_led, NULL);
	ets_timer_arm(&temporizador_general, CADENCIA_BROKER, true);
	lv_actualizar_estado_broker_lcd(datosApp, COLOR_ICONO_INTENTANDO);

	return ESP_OK;
}
esp_err_t appuser_broker_conectado(DATOS_APLICACION *datosApp) {

	ets_timer_disarm(&temporizador_general);
	gpio_set_level(CONFIG_GPIO_PIN_LED, ON);
	lv_actualizar_estado_broker_lcd(datosApp, COLOR_ICONO_OK);

	return ESP_OK;
}
esp_err_t appuser_broker_desconectado(DATOS_APLICACION *datosApp) {

	ets_timer_disarm(&temporizador_general);
	ets_timer_setfn(&temporizador_general, (ETSTimerFunc*) parapadeo_led, NULL);
	ets_timer_arm(&temporizador_general, CADENCIA_BROKER, true);
	lv_actualizar_estado_broker_lcd(datosApp, COLOR_ICONO_NOK);

	return ESP_OK;
}




/**
 * Esta funcion se ejecuta cuando vence el temporizador de duracion cuando este es mayor que 0
 */
void appuser_ejecucion_accion_temporizada(DATOS_APLICACION *datosApp) {

    cJSON * respuesta = NULL;
    ESP_LOGI(TAG, ""TRAZAR"FIN DE LA TEMPORIZACION. SE PASA A TEMPERATURA DE DEFECTO", INFOTRAZA);
    datosApp->termostato.tempUmbral = datosApp->termostato.tempUmbralDefecto;
    lv_actualizar_umbral_temperatura_lcd(datosApp);
    respuesta = appuser_generar_informe_espontaneo(datosApp, RELE_TEMPORIZADO, NULL);
    if (respuesta != NULL) {
    	publicar_mensaje_json(datosApp, respuesta, NULL);
    }
    ESP_LOGI(TAG, ""TRAZAR"FIN DE LA TEMPORIZACION. SE PASA A LA TEMPERATURA DE DEFECTO", INFOTRAZA);
}

esp_err_t appuser_temporizador_cumplido(DATOS_APLICACION *datosApp) {

	cJSON *respuesta;
	enum ESTADO_RELE accion;

	ESP_LOGI(TAG, ""TRAZAR"ejecutamos la accion del programa de aplicacion", INFOTRAZA);
	datosApp->termostato.tempUmbral = datosApp->datosGenerales->programacion[datosApp->datosGenerales->nProgramaCandidato].temperatura;

    if (calcular_accion_termostato(datosApp, &accion) == ACCIONAR_TERMOSTATO) {
    	operacion_rele(datosApp, TEMPORIZADA, accion);

    }
	respuesta = appuser_generar_informe_espontaneo(datosApp, CAMBIO_DE_PROGRAMA, NULL);
	if (respuesta != NULL) {
		publicar_mensaje_json(datosApp, respuesta, NULL);
	}
	lv_actualizar_umbral_temperatura_lcd(datosApp);
	// actualizar los intervalos del lcd
	lv_actualizar_intervalo_programa(datosApp, true);



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
	lv_actualizar_estado_alarma_lcd(datosApp);



	return ESP_OK;
}

cJSON* appuser_generar_informe_espontaneo(DATOS_APLICACION *datosApp, enum TIPO_INFORME tipoInforme, cJSON *comandoOriginal) {


    cJSON *respuesta = NULL;


    respuesta = cabecera_espontaneo(datosApp, tipoInforme);
    switch(tipoInforme) {
    case ARRANQUE_APLICACION:
        cJSON_AddNumberToObject(respuesta, APP_COMAND_ESTADO_RELE, gpio_get_level(CONFIG_GPIO_PIN_RELE));
        cJSON_AddNumberToObject(respuesta, PROGRAMMER_STATE, datosApp->datosGenerales->estadoProgramacion);
        cJSON_AddNumberToObject(respuesta, DEVICE_STATE, datosApp->datosGenerales->estadoApp);
        cJSON_AddNumberToObject(respuesta, TEMPERATURA, datosApp->termostato.tempActual);
        cJSON_AddNumberToObject(respuesta, HUMEDAD, datosApp->termostato.humedad);
        cJSON_AddNumberToObject(respuesta, UMBRAL_TEMPERATURA, datosApp->termostato.tempUmbral);
        cJSON_AddBoolToObject(respuesta, MASTER, datosApp->termostato.master);
        cJSON_AddStringToObject(respuesta, SENSOR_REMOTO, datosApp->termostato.sensor_remoto);
        escribir_programa_actual(datosApp, respuesta);
        break;
    case ACTUACION_RELE_LOCAL:
        cJSON_AddNumberToObject(respuesta, APP_COMAND_ESTADO_RELE, gpio_get_level(CONFIG_GPIO_PIN_RELE));
        cJSON_AddNumberToObject(respuesta, PROGRAMMER_STATE, datosApp->datosGenerales->estadoProgramacion);
        cJSON_AddNumberToObject(respuesta, DEVICE_STATE, datosApp->datosGenerales->estadoApp);
        cJSON_AddNumberToObject(respuesta, TEMPERATURA, datosApp->termostato.tempActual);
        cJSON_AddNumberToObject(respuesta, HUMEDAD, datosApp->termostato.humedad);
        cJSON_AddNumberToObject(respuesta, UMBRAL_TEMPERATURA, datosApp->termostato.tempUmbral);
        cJSON_AddBoolToObject(respuesta, MASTER, datosApp->termostato.master);
        cJSON_AddStringToObject(respuesta, SENSOR_REMOTO, datosApp->termostato.sensor_remoto);
        escribir_programa_actual(datosApp, respuesta);

        break;
    case CAMBIO_DE_PROGRAMA:
        cJSON_AddNumberToObject(respuesta, APP_COMAND_ESTADO_RELE, gpio_get_level(CONFIG_GPIO_PIN_RELE));
        cJSON_AddNumberToObject(respuesta, PROGRAMMER_STATE, datosApp->datosGenerales->estadoProgramacion);
        cJSON_AddNumberToObject(respuesta, DEVICE_STATE, datosApp->datosGenerales->estadoApp);
        cJSON_AddNumberToObject(respuesta, TEMPERATURA, datosApp->termostato.tempActual);
        cJSON_AddNumberToObject(respuesta, HUMEDAD, datosApp->termostato.humedad);
        cJSON_AddNumberToObject(respuesta, UMBRAL_TEMPERATURA, datosApp->termostato.tempUmbral);
        cJSON_AddBoolToObject(respuesta, MASTER, datosApp->termostato.master);
        cJSON_AddStringToObject(respuesta, SENSOR_REMOTO, datosApp->termostato.sensor_remoto);
        escribir_programa_actual(datosApp, respuesta);
        break;
    case RELE_TEMPORIZADO:
    case CAMBIO_TEMPERATURA:
    case CAMBIO_UMBRAL_TEMPERATURA:
    case CAMBIO_ESTADO_APLICACION:
        cJSON_AddNumberToObject(respuesta, APP_COMAND_ESTADO_RELE, gpio_get_level(CONFIG_GPIO_PIN_RELE));
        cJSON_AddNumberToObject(respuesta, PROGRAMMER_STATE, datosApp->datosGenerales->estadoProgramacion);
        cJSON_AddNumberToObject(respuesta, DEVICE_STATE, datosApp->datosGenerales->estadoApp);
        cJSON_AddNumberToObject(respuesta, TEMPERATURA, datosApp->termostato.tempActual);
        cJSON_AddNumberToObject(respuesta, HUMEDAD, datosApp->termostato.humedad);
        cJSON_AddNumberToObject(respuesta, UMBRAL_TEMPERATURA, datosApp->termostato.tempUmbral);
        cJSON_AddBoolToObject(respuesta, MASTER, datosApp->termostato.master);
        cJSON_AddStringToObject(respuesta, SENSOR_REMOTO, datosApp->termostato.sensor_remoto);
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

	cJSON *item;
	char* dato;

	item = cJSON_GetObjectItem(nodo, PROGRAM_ID);
	dato = cJSON_GetStringValue(item);
	if (item != NULL) {
		switch(programa_actual->tipo) {
		case DIARIA:
			programa_actual->accion = extraer_dato_tm(dato, 11, 1);
			break;
		case SEMANAL:
			break;
		case FECHADA:
			programa_actual->accion = extraer_dato_tm(dato, 17, 1);
			break;
		}

	}

	item = cJSON_GetObjectItem(nodo, DURATION_PROGRAM);
	if (item != NULL) {
		programa_actual->duracion = item->valueint;
		ESP_LOGI(TAG, ""TRAZAR"DURACION = %d", INFOTRAZA, programa_actual->duracion);
	} else {
		programa_actual->duracion = 0;
		ESP_LOGI(TAG, ""TRAZAR"NO SE GUARDA DURACION: %d", INFOTRAZA, programa_actual->duracion);
	}

	ESP_LOGI(TAG, ""TRAZAR" VAMOS A CALCULAR LA TEMPERATURA UMBRAL", INFOTRAZA);
	extraer_dato_double(nodo, UMBRAL_TEMPERATURA, &programa_actual->temperatura);
	datosApp->termostato.tempUmbral = programa_actual->temperatura;
	ESP_LOGI(TAG, ""TRAZAR" UMBRAL :%lf", INFOTRAZA, programa_actual->temperatura);
	lv_actualizar_umbral_temperatura_lcd(datosApp);

	return ESP_OK;
}

esp_err_t appuser_configuracion_a_json(DATOS_APLICACION *datosApp, cJSON *conf) {


	ESP_LOGI(TAG, ""TRAZAR"SE CARGAN DATOS PARTICULARES DE LA APLICACION", INFOTRAZA);
    cJSON_AddNumberToObject(conf, MARGEN_TEMPERATURA, datosApp->termostato.margenTemperatura);
    cJSON_AddNumberToObject(conf, INTERVALO_LECTURA, datosApp->termostato.intervaloLectura);
    cJSON_AddNumberToObject(conf, INTERVALO_REINTENTOS, datosApp->termostato.intervaloReintentos);
    cJSON_AddNumberToObject(conf, REINTENTOS_LECTURA, datosApp->termostato.reintentosLectura);
    cJSON_AddNumberToObject(conf, CALIBRADO, datosApp->termostato.calibrado);
    cJSON_AddBoolToObject(conf, MASTER, datosApp->termostato.master);
    cJSON_AddStringToObject(conf, SENSOR_REMOTO, datosApp->termostato.sensor_remoto);
    cJSON_AddNumberToObject(conf, UMBRAL_DEFECTO, datosApp->termostato.tempUmbralDefecto);
    cJSON_AddNumberToObject(conf, INCDEC, datosApp->termostato.incdec);


    ESP_LOGI(TAG, ""TRAZAR" INTERVALO LECTURA VALE %d", INFOTRAZA, datosApp->termostato.intervaloLectura);
	return ESP_OK;
}

esp_err_t appuser_json_a_datos(DATOS_APLICACION *datosApp, cJSON *datos) {

	extraer_dato_double(datos, MARGEN_TEMPERATURA, &datosApp->termostato.margenTemperatura);
	extraer_dato_uint8(datos, INTERVALO_LECTURA, &datosApp->termostato.intervaloLectura);
	extraer_dato_uint8(datos, INTERVALO_REINTENTOS, &datosApp->termostato.intervaloReintentos);
	extraer_dato_uint8(datos, REINTENTOS_LECTURA, &datosApp->termostato.reintentosLectura);
	extraer_dato_double(datos, CALIBRADO, &datosApp->termostato.calibrado);
	extraer_dato_uint8(datos,  MASTER, (uint8_t*) &datosApp->termostato.master);
	extraer_dato_string(datos, SENSOR_REMOTO, datosApp->termostato.sensor_remoto);
	extraer_dato_float(datos,  UMBRAL_DEFECTO, &datosApp->termostato.tempUmbralDefecto);
	extraer_dato_float(datos, INCDEC, &datosApp->termostato.incdec);

	return ESP_OK;
}

esp_err_t appuser_cargar_programas_defecto(DATOS_APLICACION *datosApp, cJSON *array) {

	cJSON *item = NULL;

	ESP_LOGW(TAG, ""TRAZAR" VAMOS A INSERTAR PROGRAMAS DE DEFECTO EN EL CRONOTERMOSTATO", INFOTRAZA);
	cJSON_AddItemToArray(array, item = cJSON_CreateObject());
	cJSON_AddStringToObject(item, PROGRAM_ID, "000000006700");
	cJSON_AddNumberToObject(item, UMBRAL_TEMPERATURA, 21.5);
	cJSON_AddItemToArray(array, item = cJSON_CreateObject());
	cJSON_AddStringToObject(item, PROGRAM_ID, "001200007f10");
	cJSON_AddNumberToObject(item, UMBRAL_TEMPERATURA, 22.5);
	cJSON_AddItemToArray(array, item = cJSON_CreateObject());
	cJSON_AddStringToObject(item, PROGRAM_ID, "001555007f11");
	cJSON_AddNumberToObject(item, UMBRAL_TEMPERATURA, 23.5);
	cJSON_AddItemToArray(array, item = cJSON_CreateObject());
	cJSON_AddStringToObject(item, PROGRAM_ID, "001038007f11");
	cJSON_AddNumberToObject(item, UMBRAL_TEMPERATURA, 24.5);
	cJSON_AddItemToArray(array, item = cJSON_CreateObject());
	cJSON_AddStringToObject(item, PROGRAM_ID, "021715000120081611");
	cJSON_AddNumberToObject(item, UMBRAL_TEMPERATURA, 25.5);

	return ESP_OK;
}

esp_err_t appuser_extraer_datos_programa(TIME_PROGRAM *programa_actual, cJSON *nodo) {


	extraer_dato_double(nodo, UMBRAL_TEMPERATURA, &programa_actual->temperatura);
	extraer_dato_double(nodo, HUMEDAD, &programa_actual->humedad);
    if(extraer_dato_int(nodo, DURATION_PROGRAM, (int*) &programa_actual->duracion) != ESP_OK) {
    	programa_actual->duracion = 0;
    }


	return ESP_OK;
}

esp_err_t appuser_modificar_dato_programa(TIME_PROGRAM *programa_actual,cJSON *nodo) {

	extraer_dato_double(nodo, UMBRAL_TEMPERATURA, &programa_actual->temperatura);
	extraer_dato_double(nodo, HUMEDAD, &programa_actual->humedad);
	extraer_dato_uint32(nodo, DURATION_PROGRAM, &programa_actual->duracion);

	return ESP_OK;
}

esp_err_t appuser_visualizar_dato_programa(TIME_PROGRAM *programa_actual, cJSON *nodo) {

    cJSON_AddNumberToObject(nodo, UMBRAL_TEMPERATURA, programa_actual->temperatura);
    if (programa_actual->duracion > 0) {
        cJSON_AddNumberToObject(nodo, DURATION_PROGRAM, programa_actual->duracion);

    }

	return ESP_OK;
}

esp_err_t appuser_modificarConfApp(cJSON *root, DATOS_APLICACION *datosApp, cJSON *respuesta) {

   cJSON *nodo;

   ESP_LOGI(TAG, ""TRAZAR" Modificando configuracion App", INFOTRAZA);
   nodo = cJSON_GetObjectItem(root, APP_PARAMS);
   if(nodo == NULL) {
       codigoRespuesta(respuesta, RESP_NOK);
       return ESP_FAIL;
   }

   extraer_dato_uint8(nodo, MQTT_TLS, (uint8_t*) &datosApp->datosGenerales->parametrosMqtt.tls);
   if (datosApp->datosGenerales->parametrosMqtt.tls == false) {
	   datosApp->datosGenerales->parametrosMqtt.port = 1883;
	   strcpy(datosApp->datosGenerales->parametrosMqtt.broker, (const char*) "mqtt://jajicaiot.ddns.net");
   } else {
	   datosApp->datosGenerales->parametrosMqtt.port = 8883;
	  	   strcpy(datosApp->datosGenerales->parametrosMqtt.broker, (const char*) "mqtts://jajicaiot.ddns.net");
   }
   extraer_dato_double(nodo, MARGEN_TEMPERATURA, &datosApp->termostato.margenTemperatura);
   extraer_dato_uint8(nodo, INTERVALO_LECTURA, &datosApp->termostato.intervaloLectura);
   extraer_dato_uint8(nodo, INTERVALO_REINTENTOS, &datosApp->termostato.intervaloReintentos);
   extraer_dato_uint8(nodo, REINTENTOS_LECTURA, &datosApp->termostato.reintentosLectura);
   extraer_dato_double(nodo, CALIBRADO, &datosApp->termostato.calibrado);
   extraer_dato_float(nodo, UMBRAL_DEFECTO, &datosApp->termostato.tempUmbralDefecto);
   extraer_dato_float(nodo, INCDEC, &datosApp->termostato.incdec);
   if ((extraer_dato_uint8(nodo,  MASTER, (uint8_t*) &datosApp->termostato.master)) == ESP_OK) {
	   extraer_dato_string(nodo, SENSOR_REMOTO, datosApp->termostato.sensor_remoto);
	   cJSON_AddBoolToObject(respuesta, MASTER, datosApp->termostato.master);
	   cJSON_AddStringToObject(respuesta, SENSOR_REMOTO, datosApp->termostato.sensor_remoto);

   }
   codigoRespuesta(respuesta, RESP_OK);

   ESP_LOGI(TAG, ""TRAZAR" parametros de configuracion  modificados", INFOTRAZA);
   return salvar_configuracion_general(datosApp);

}

esp_err_t app_user_mensaje_recibido_especifico(DATOS_APLICACION *datosApp) {

	cJSON *respuesta;
	char* texto_respuesta;
	double dato;
	char topic[55] = {0};
	bool flag_envio = false;
	ets_timer_disarm(&temporizador_lectura_remota);
	strncpy(topic, datosApp->handle_mqtt->topic, datosApp->handle_mqtt->topic_len);
	texto_respuesta = (char*) calloc((datosApp->handle_mqtt->data_len + 1), sizeof(char));
	strncpy(texto_respuesta,datosApp->handle_mqtt->data, datosApp->handle_mqtt->data_len);
	respuesta = cJSON_Parse(texto_respuesta);
	if (respuesta != NULL) {
		extraer_dato_double(respuesta, TEMPERATURA, &dato);
		datosApp->termostato.tempActual = (float) dato;
		extraer_dato_double(respuesta, HUMEDAD, &dato);
		datosApp->termostato.humedad = (float) dato;
		free(texto_respuesta);

	}
	ESP_LOGI(TAG, ""TRAZAR" temperatura remota :%lf, humedad remota:%lf", INFOTRAZA, datosApp->termostato.tempActual,datosApp->termostato.humedad);
	cJSON_Delete(respuesta);
	if(datosApp->alarmas[ALARMA_SENSOR_REMOTO].estado_alarma > ALARMA_OFF) {
		flag_envio = true;

	}
	registrar_alarma(datosApp, NOTIFICACION_ALARMA_SENSOR_REMOTO, ALARMA_SENSOR_REMOTO, ALARMA_OFF, flag_envio);

	notificar_fin_arranque(datosApp);



	return ESP_OK;
}

esp_err_t notificar_fin_arranque(DATOS_APLICACION *datosApp) {

	cJSON *informe;


	if (datosApp->datosGenerales->estadoApp == ESPERA_FIN_ARRANQUE) {

		lv_cargar_pantalla_principal();
		calcular_estado_aplicacion(datosApp);
		//appuser_cambiar_modo_aplicacion(datosApp, estado_app);
		informe = appuser_generar_informe_espontaneo(datosApp, ARRANQUE_APLICACION, NULL);

		if (informe != NULL) {
			publicar_mensaje_json(datosApp, informe, NULL);
		}

	}






/*

	if (datosApp->datosGenerales->estadoApp == ESPERA_FIN_ARRANQUE) {
		//cerrar_popup_generico(0);
		lv_cargar_pantalla_principal();
		//crear_pantalla_principal();
		//esconder_pantalla_principal(false);

		if (datosApp->datosGenerales->nProgramacion == 0) {
			appuser_cambiar_modo_aplicacion(datosApp, NORMAL_SIN_PROGRAMACION);
		} else {
			appuser_cambiar_modo_aplicacion(datosApp, NORMAL_AUTO);
		}
		//datosApp->datosGenerales->estadoApp = NORMAL_AUTO;
		informe = appuser_generar_informe_espontaneo(datosApp, ARRANQUE_APLICACION, NULL);

		if (informe != NULL) {
			publicar_mensaje_json(datosApp, informe, NULL);
		}

	}*/

	return ESP_OK;

}

esp_err_t appuser_actualizar_gestion_programas(DATOS_APLICACION *datosApp) {

	//actualizamos la hora en el lcd

	if (datosApp->datosGenerales->clock.date.tm_sec == 0) {
		ESP_LOGI(TAG, ""TRAZAR" ACTUALIZAMOS LA HORA EN EL LCD", INFOTRAZA);
		lv_actualizar_hora_lcd(datosApp);
		lv_actualizar_intervalo_programa(datosApp, false);
	}



	return ESP_OK;
}

esp_err_t appuser_cambiar_modo_aplicacion(DATOS_APLICACION *datosApp, enum ESTADO_APP estado) {


    time_t t_siguiente_intervalo;
	switch(estado) {

	case NORMAL_AUTO:
	case NORMAL_AUTOMAN:
		calcular_programa_activo(datosApp, &t_siguiente_intervalo);
		//lv_actualizar_umbral_temperatura_lcd(datosApp);
		//lv_actualizar_modo_aplicacion_lcd(estado);
		break;

	case NORMAL_MANUAL:
		datosApp->termostato.tempUmbral = datosApp->termostato.tempUmbralDefecto;
		//lv_actualizar_umbral_temperatura_lcd(datosApp);
		//lv_actualizar_modo_aplicacion_lcd(estado);
		break;

	case NORMAL_SIN_PROGRAMACION:
		ESP_LOGE(TAG, ""TRAZAR" PONEMOS EL UMBRAL A %lf", INFOTRAZA, datosApp->termostato.tempUmbralDefecto);
		datosApp->termostato.tempUmbral = datosApp->termostato.tempUmbralDefecto;
		//lv_actualizar_umbral_temperatura_lcd(datosApp);
		//lv_actualizar_modo_aplicacion_lcd(estado);
		break;

	default:
		break;


	}
	datosApp->datosGenerales->estadoApp = estado;
	lv_actualizar_modo_aplicacion_lcd(estado);
	lv_actualizar_umbral_temperatura_lcd(datosApp);





	ESP_LOGI(TAG, ""TRAZAR" CAMBIADA LA APLICACION A ESTADO %d", INFOTRAZA, datosApp->datosGenerales->estadoApp);



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

enum ESTADO_APP calcular_estado_aplicacion(DATOS_APLICACION *datosApp) {

	enum ESTADO_APP estado_final;

	switch (datosApp->datosGenerales->estadoApp) {

	case ESPERA_FIN_ARRANQUE:
		if (datosApp->datosGenerales->nProgramacion == 0) {
			estado_final = NORMAL_SIN_PROGRAMACION;
		} else {
			estado_final = NORMAL_AUTO;
		}
	break;
	default:
		estado_final = NORMAL_AUTO;
		break;
	}

	appuser_cambiar_modo_aplicacion(datosApp, estado_final);



	return estado_final;
}






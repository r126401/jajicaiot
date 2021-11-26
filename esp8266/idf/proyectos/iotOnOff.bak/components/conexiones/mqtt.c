/*
 * mqtt.c
 *
 *  Created on: 22 sept. 2020
 *      Author: t126401
 */


#include "esp_err.h"
#include "esp_log.h"
#include "conexiones_mqtt.h"
#include "mqtt_client.h"
#include "datos_comunes.h"
#include "api_json.h"
#include "alarmas.h"
#include "interfaz_usuario.h"
#include "dialogos_json.h"


static const char *TAG = "MQTT";
extern DATOS_APLICACION datosApp;

xQueueHandle cola_mqtt = NULL;



static esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event)
{
    esp_mqtt_client_handle_t client = event->client;
    int msg_id = -1;
    // your_context_t *context = event->context;
    datosApp.handle_mqtt = event;
    static bool arranque = false;

    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, ""TRAZAR"MQTT_EVENT_CONNECTED: CONECTADO AL BROKER", INFOTRAZA);
            appuser_broker_conectado(&datosApp);
            msg_id = esp_mqtt_client_subscribe(client, datosApp.datosGenerales->parametrosMqtt.subscribe,datosApp.datosGenerales->parametrosMqtt.qos);
            ESP_LOGI(TAG, ""TRAZAR"ACCION PARA SUBSCRIBIR AL TOPIC :%s msg_id=%d", INFOTRAZA, datosApp.datosGenerales->parametrosMqtt.subscribe, msg_id);
            if (datosApp.alarmas[ALARMA_MQTT].estado_alarma == ALARMA_ON) {
            	registrar_alarma(&datosApp, NOTIFICACION_ALARMA_MQTT, ALARMA_MQTT, ALARMA_OFF, true);
            }

            break;

        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGW(TAG, ""TRAZAR"MQTT_EVENT_DISCONNECTED: Desconectado del broker :%s msg_id=%d", INFOTRAZA, datosApp.datosGenerales->parametrosMqtt.broker, msg_id);
            registrar_alarma(&datosApp, NOTIFICACION_ALARMA_MQTT, ALARMA_MQTT, ALARMA_ON, false);
            appuser_broker_desconectado(&datosApp);
            break;

        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, ""TRAZAR"MQTT_EVENT_SUBSCRIBED: SUBSCRITOS CON EXITO AL TOPIC :%s msg_id=%d", INFOTRAZA, datosApp.datosGenerales->parametrosMqtt.subscribe, msg_id);
            if (arranque == false ){
            	appuser_arranque_aplicacion(&datosApp);
            	arranque = true;
            }
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGW(TAG, ""TRAZAR"MQTT_EVENT_UNSUBSCRIBED: YA NO ESTAS SUBSCRITO AL TOPIC :%s msg_id=%d", INFOTRAZA, datosApp.datosGenerales->parametrosMqtt.subscribe, msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, ""TRAZAR"MQTT_EVENT_PUBLISHED: CONFIRMACION DE EVENTO PUBLICADO. TOPIC :%s msg_id=%d", INFOTRAZA, datosApp.datosGenerales->parametrosMqtt.subscribe, msg_id);

            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, ""TRAZAR"MQTT_EVENT_DATA: RECIBIDO MENSAJE", INFOTRAZA);

            char topic[55] = {0};
            strncpy(topic, event->topic, event->topic_len);

            if (strcmp(datosApp.datosGenerales->parametrosMqtt.subscribe, topic) == 0) {
            	mensaje_recibido(&datosApp);
            } else {
            	app_user_mensaje_recibido_especifico(&datosApp);
            }

            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGE(TAG, ""TRAZAR"MQTT_EVENT_ERROR", INFOTRAZA);
            registrar_alarma(&datosApp, NOTIFICACION_ALARMA_MQTT, ALARMA_MQTT, ALARMA_ON, false);
            break;
        case MQTT_EVENT_BEFORE_CONNECT:
        	ESP_LOGE(TAG, ""TRAZAR"MQTT_EVENT_BEFORE_CONNECT. EVENTO NUEVO DESPUES DE CONECTAR", INFOTRAZA);
        	break;
        default:
        	break;
    }
    return ESP_OK;
}

void mqtt_task(void *arg) {

	//DATOS_APLICACION *datosApp = (DATOS_APLICACION*) arg;
	COLA_MQTT cola;
	cola_mqtt = xQueueCreate(10, sizeof(COLA_MQTT));

	ESP_LOGI(TAG, ""TRAZAR"COMIENZO MQTT_TASK", INFOTRAZA);
	establecer_conexion_mqtt(&datosApp);


	for(;;) {
		 ESP_LOGI(TAG, ""TRAZAR"ESPERANDO MENSAJE...Memoria libre: %d\n", INFOTRAZA, esp_get_free_heap_size());
		if (xQueueReceive(cola_mqtt, &cola, portMAX_DELAY) == pdTRUE) {
			publicar_mensaje(&datosApp, &cola);

		} else {
			ESP_LOGE(TAG, ""TRAZAR"NO SE HA PODIDO PROCESAR LA PETICION", INFOTRAZA);
		}

	}
	vTaskDelete(NULL);


}



esp_err_t establecer_conexion_mqtt(DATOS_APLICACION *datosApp) {
	esp_err_t error;

    esp_mqtt_client_config_t mqtt_cfg = {
		.uri = datosApp->datosGenerales->parametrosMqtt.broker,
		.port = datosApp->datosGenerales->parametrosMqtt.port,
        .event_handle = mqtt_event_handler,
		.username = datosApp->datosGenerales->parametrosMqtt.user,
		.password = datosApp->datosGenerales->parametrosMqtt.password
    };
    ESP_LOGI(TAG, ""TRAZAR"Nos conectamos al broker %s", INFOTRAZA, mqtt_cfg.uri);
    appuser_broker_conectando(datosApp);
    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    error = esp_mqtt_client_start(client);


    return error;
}
esp_err_t publicar_mensaje(DATOS_APLICACION *datosApp, COLA_MQTT *cola) {

	int msg_id;

	//esp_mqtt_client_handle_t client = cliente->client;
	if (datosApp->handle_mqtt == NULL) {
		ESP_LOGE(TAG, ""TRAZAR"NO HAY CONEXION CON EL BROKER Y NO SE PUEDE ENVIAR EL MENSAJE", INFOTRAZA);
		return ESP_FAIL;
	}

	if (datosApp->datosGenerales->estadoApp == NORMAL_ARRANCANDO) {
		ESP_LOGW(TAG, "NO SE ENVIA NADA PORQUE EL DISPOSITIVO ESTA AUN EN FASE DE ARRANQUE");
		return ESP_OK;
	}

	msg_id = esp_mqtt_client_publish(datosApp->handle_mqtt->client,
			cola->topic,
			cola->buffer,
			0,
			datosApp->datosGenerales->parametrosMqtt.qos,
			0);
	ESP_LOGI(TAG, ""TRAZAR" MENSAJE %d PUBLICADO.%s \n %s", INFOTRAZA, msg_id, cola->topic, cola->buffer);
	return ESP_OK;
}

esp_err_t publicar_mensaje_json(DATOS_APLICACION *datosApp, cJSON *mensaje, char *topic) {

	char* texto;
	COLA_MQTT cola;
	memset(&cola, 0, sizeof(COLA_MQTT));
	if (datosApp->handle_mqtt == NULL) {
		ESP_LOGE(TAG, ""TRAZAR"NO HAY CONEXION CON EL BROKER Y NO SE PUEDE ENVIAR EL MENSAJE", INFOTRAZA);
		return ESP_FAIL;
	}
	texto = cJSON_Print(mensaje);
	if (topic != NULL) {
		strcpy(cola.topic, topic);
	} else {
		strcpy(cola.topic, datosApp->datosGenerales->parametrosMqtt.publish);
	}
	strcpy(cola.buffer, texto);
	ESP_LOGE(TAG, ""TRAZAR" tamano de cola %d", INFOTRAZA, strlen(cola.topic) + strlen(cola.buffer));
	if (texto != NULL) {
		xQueueSend(cola_mqtt, &cola,0);
		free(texto);
	}
	cJSON_Delete(mensaje);
	return ESP_OK;
}


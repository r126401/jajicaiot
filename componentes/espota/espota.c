/*
 * espota.c
 *
 *  Created on: 16 feb. 2021
 *      Author: t126401
 */

#include "stdint.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_ota_ops.h"
#include "esp_http_client.h"
#include "esp_https_ota.h"
#include "esp_system.h"
#include "esp_event.h"
#include "datos_comunes.h"
#include "configuracion_usuario.h"
#include <sys/socket.h>
#include <netdb.h>
#include "espota.h"
#include "api_json.h"






static const char *TAG = "OTAESP";
extern const uint8_t server_cert_pem_start[] asm("_binary_ca_cert_pem_start");
extern const uint8_t server_cert_pem_end[] asm("_binary_ca_cert_pem_end");

#define OTA_URL_SIZE 256

char* name_to_ip(char* url) {


static char ip[16];

    const struct addrinfo hints = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM,
    };
    struct addrinfo *res;
    struct in_addr *addr;
    int s;
    char puerto[6] = {0};
    itoa(8070, puerto, 10);

    int err = getaddrinfo(url, puerto, &hints, &res);

     if(err != 0 || res == NULL) {
         ESP_LOGE(TAG, ""TRAZAR"DNS lookup error err=%d res=%p", INFOTRAZA, err, res);
         vTaskDelay(1000 / portTICK_PERIOD_MS);
         return NULL;
     }

     /* Code to print the resolved IP.

        Note: inet_ntoa is non-reentrant, look at ipaddr_ntoa_r for "real" code */
     addr = &((struct sockaddr_in *)res->ai_addr)->sin_addr;
     ESP_LOGI(TAG, ""TRAZAR"DNS lookup con exito. IP=%s", INFOTRAZA, inet_ntoa(*addr));
     s = socket(res->ai_family, res->ai_socktype, 0);
     if(s < 0) {
         ESP_LOGE(TAG, ""TRAZAR"... Fallo al crear el socket.", INFOTRAZA);
         freeaddrinfo(res);
         vTaskDelay(1000 / portTICK_PERIOD_MS);
         return NULL;
     }
     strcpy(ip, inet_ntoa(*addr));
     return ip;

}



esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    switch (evt->event_id) {
    case HTTP_EVENT_ERROR:
        ESP_LOGI(TAG, "HTTP_EVENT_ERROR");
        break;
    case HTTP_EVENT_ON_CONNECTED:
        ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
        break;
    case HTTP_EVENT_HEADER_SENT:
        ESP_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");
        break;
    case HTTP_EVENT_ON_HEADER:
        ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
        break;
    case HTTP_EVENT_ON_DATA:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
        break;
    case HTTP_EVENT_ON_FINISH:
        ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
        break;
    case HTTP_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
        break;
    }
    return ESP_OK;
}

void otaesp_task(void *pvParameter)
{
	DATOS_APLICACION *datosApp;
	datosApp = (DATOS_APLICACION *) pvParameter;
	char*ip;
	char url[120];
	const esp_app_desc_t *aplicacion;
	aplicacion = esp_ota_get_app_description();
	ESP_LOGW(TAG, ""TRAZAR"Comienzo upgrade firmware", INFOTRAZA);
    ip = name_to_ip(datosApp->datosGenerales->ota.server);
    if (ip == NULL) {
    	ESP_LOGE(TAG, "Error al traducir de nombre a ip");
    }

    sprintf(url, "https://%s:%d/firmware/%s/%s", ip, datosApp->datosGenerales->ota.puerto,
    		aplicacion->project_name,datosApp->datosGenerales->ota.file);

    ESP_LOGI(TAG, ""TRAZAR"url definitiva %s", INFOTRAZA, url);
    esp_http_client_config_t config = {
        .url = url,
		.port = datosApp->datosGenerales->ota.puerto,
        .cert_pem = (char *)server_cert_pem_start,
        .event_handler = _http_event_handler,
    };

    config.skip_cert_common_name_check = true;

    ESP_LOGI(TAG, ""TRAZAR"url definitiva2 %s", INFOTRAZA, config.url);
    ESP_LOGW(TAG, ""TRAZAR" memoria antes %d", INFOTRAZA,esp_get_free_heap_size ());
    esp_err_t ret = esp_https_ota(&config);
    ESP_LOGW(TAG, ""TRAZAR" memoria despues %d", INFOTRAZA,esp_get_free_heap_size ());
    if (ret == ESP_OK) {
    	notificar_evento_ota(datosApp, OTA_UPGRADE_FINALIZADO);
        esp_restart();
    } else {
        ESP_LOGE(TAG, "Firmware upgrade failed, error: %d", ret);
        ESP_LOGE(TAG, ""TRAZAR" memoria error %d", INFOTRAZA,esp_get_free_heap_size ());
        notificar_evento_ota(datosApp, OTA_ERROR);
    }
    /*
    while (1) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    */
    notificar_evento_ota(datosApp, OTA_ERROR);
    vTaskDelete(NULL);
}



void tarea_upgrade_firmware(DATOS_APLICACION *datosApp) {
	xTaskCreate(otaesp_task, "ota_example_task", 8192, datosApp, 5, NULL);

}






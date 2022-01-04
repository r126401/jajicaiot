/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/



#include "funciones_usuario.h"
#include <stdio.h>
//#include "freertos/FreeRTOS.h"
//#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "nvslib.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "mqtt_client.h"
#include "conexiones.h"
#include "configuracion.h"
#include "programmer.h"
#include "ntp.h"
#include "configuracion_usuario.h"
#include "datos_comunes.h"
#include "api_json.h"
#include "conexiones_mqtt.h"
#include "espota.h"
//#include "lwip/dns.h"
#include "alarmas.h"
#include "interfaz_usuario.h"

#include <sys/socket.h>
#include <netdb.h>
#include "lcd.h"
#include "esp_app_format.h"
#include "esp_ota_ops.h"
#include "esp_wifi.h"

#include "esp_http_client.h"


DATOS_APLICACION datosApp;
static const char *TAG = "IOTCRONOTEMPLCD";
TaskHandle_t handle;





void app_main()
{




	const esp_app_desc_t *aplicacion;
	esp_err_t error = ESP_OK;
	ESP_LOGI(TAG, ""TRAZAR"COMIENZO DE LA APLICACION version", INFOTRAZA);
	//ESP_LOGI(TAG, ""TRAZAR"VERSION ORIGINAL DE COMPILACION: %d", INFOTRAZA, VERSION_SW);
	DATOS_GENERALES *datosGenerales;
	datosGenerales = (DATOS_GENERALES*) calloc(1, sizeof(DATOS_GENERALES));
	datosApp.datosGenerales = datosGenerales;
	//uart_set_baudrate(UART_NUM_0, 115200);




	aplicacion = calloc(1, sizeof(esp_app_desc_t));
	aplicacion = esp_ota_get_app_description();
	ESP_LOGW(TAG, ""TRAZAR" app:%s, version: %s, hora: %s, dia:%s, idfver:%s", INFOTRAZA,
			aplicacion->project_name, aplicacion->version, aplicacion->time, aplicacion->date, aplicacion->idf_ver);

	error = inicializar_nvs(CONFIG_NAMESPACE, &datosApp.handle);
	if (error != ESP_OK) {
		ESP_LOGE(TAG, ""TRAZAR" ERROR AL INICIALIZAR NVS", INFOTRAZA);
		error = ESP_FAIL;
	} else {
		error = ESP_OK;
	}



	if(configurado_de_fabrica() == ESP_OK) {

		datosApp.datosGenerales->estadoApp = ARRANQUE_FABRICA;

	}

	if (appuser_inicializar_aplicacion(&datosApp) != ESP_OK) {
		ESP_LOGE(TAG, ""TRAZAR" FALLO LA INICIALIZACION DE LA APLICACION", INFOTRAZA);
	}

	ESP_LOGI(TAG, ""TRAZAR" vamos a conectar al wifi", INFOTRAZA);

	conectar_dispositivo_wifi();

	error = inicializacion(&datosApp, CONFIG_CARGA_CONFIGURACION);
	if (error == ESP_OK) {
		ESP_LOGI(TAG, ""TRAZAR"INICIALIZACION CORRECTA", INFOTRAZA);
	} else {
		ESP_LOGE(TAG, ""TRAZAR"NO SE HA PODIDO INICIALIZAR EL DISPOSITIVO", INFOTRAZA);
	}

	handle = NULL;
	crear_tarea_mqtt(&datosApp);


    iniciar_gestion_programacion(&datosApp);
    xTaskCreate(tarea_lectura_temperatura, "tarea_lectura_temperatura", 4096, (void*) &datosApp, 4, NULL);

    pintar_fecha();


}

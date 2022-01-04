/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/


#include <funciones_usuario.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
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
#include "lwip/dns.h"
#include "alarmas.h"
#include "interfaz_usuario.h"

#include <sys/socket.h>
#include <netdb.h>
#include "esp_wifi.h"
#include "esp_timer.h"



DATOS_APLICACION datosApp;
static const char *TAG = "IOTONOFF";

/*
void preparar_datos_ota(DATOS_APLICACION *datosApp) {


	strcpy(datosApp->datosGenerales->ota.file,"newVersion_2103181249.ota.bin");
	strcpy(datosApp->datosGenerales->ota.server, "jajicaiot.ddns.net");
	strcpy(datosApp->datosGenerales->ota.url, "/firmware/iotOnOff/");
	datosApp->datosGenerales->ota.puerto= 8070;
	datosApp->datosGenerales->ota.swVersion = 2103181249;
	tarea_upgrade_firmware(datosApp);


}
*/
esp_err_t leer_upgrade_ota(DATOS_APLICACION *datosApp) {

	if (json_a_ota(datosApp) == ESP_OK) {
		obtener_certificado(datosApp);
		tarea_upgrade_firmware(datosApp);
		borrar_clave(&datosApp->handle, "UPGRADE");
		return ESP_OK;
	}


	return ESP_FAIL;
}


void app_main()
{


    //static os_timer_t temporizador;
	esp_timer_init();
	esp_err_t error = ESP_OK;
	ESP_LOGI(TAG, ""TRAZAR"COMIENZO DE LA APLICACION version", INFOTRAZA);
	//ESP_LOGI(TAG, ""TRAZAR"VERSION ORIGINAL DE COMPILACION: %d", INFOTRAZA, VERSION_SW);
	DATOS_GENERALES *datosGenerales;
	datosGenerales = (DATOS_GENERALES*) calloc(1, sizeof(DATOS_GENERALES));
	datosApp.datosGenerales = datosGenerales;
	//uart_set_baudrate(UART_NUM_0, 115200);

	//prueba_json();
	error = inicializar_nvs(CONFIG_NAMESPACE, &datosApp.handle);
	if (error != ESP_OK) {
		ESP_LOGE(TAG, ""TRAZAR" ERROR AL INICIALIZAR NVS", INFOTRAZA);
		error = ESP_FAIL;
	} else {
		error = ESP_OK;
	}

	if (appuser_inicializar_aplicacion(&datosApp) != ESP_OK) {
		ESP_LOGE(TAG, ""TRAZAR" FALLO LA INICIALIZACION DE LA APLICACION", INFOTRAZA);
	}

	ESP_LOGI(TAG, ""TRAZAR" vamos a conectar al wifi", INFOTRAZA);
	conectar_dispositivo_wifi();



	if (leer_upgrade_ota(&datosApp) == ESP_OK) {

	} else {
		error = inicializacion(&datosApp, CONFIG_CARGA_CONFIGURACION);
		if (error == ESP_OK) {
			ESP_LOGI(TAG, ""TRAZAR"INICIALIZACION CORRECTA", INFOTRAZA);
		} else {
			ESP_LOGE(TAG, ""TRAZAR"NO SE HA PODIDO INICIALIZAR EL DISPOSITIVO", INFOTRAZA);
		}
		iniciar_gestion_programacion(&datosApp);
	    xTaskCreate(mqtt_task, "mqtt_task", 4096, (void*) &datosApp, 2, NULL);

	}









}

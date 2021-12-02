/*
 * iotOnoff.c
 *
 *  Created on: 6 sept. 2020
 *      Author: t126401
 */



#include <dialogos_json.h>
#include "esp_err.h"
#include "esp_log.h"
#include "datos_comunes.h"
#include "programmer.h"
#include "api_json.h"
#include "driver/gpio.h"
//#include "mqtt.h"
#include "conexiones.h"
#include "funciones_usuario.h"
#include "interfaz_usuario.h"
#include "configuracion_usuario.h"
#include "dht22.h"
#include "configuracion.h"
#include "alarmas.h"
#include "conexiones_mqtt.h"


//static os_timer_t temporizadorTermometro;
//os_timer_t temporizador_lectura_remota;

static const char *TAG = "IOTTERMOMETRO";
xQueueHandle cola_gpio = NULL;

#define GPIO_OUTPUT_PIN_SEL  ((1ULL<<CONFIG_GPIO_PIN_RELE) | (1ULL<<CONFIG_GPIO_PIN_LED)| (1ULL<<CONFIG_GPIO_PIN_DHT))
#define GPIO_INPUT_PIN_SEL  ((1ULL<<CONFIG_GPIO_PIN_BOTON) | (1ULL<<CONFIG_GPIO_PIN_RELE))

#define NUM_REPETICIONES    3
#define TIMEOUT_LECTURA_REMOTA 5000 //ms





void consultarEstadoAplicacion(DATOS_APLICACION *datosApp, cJSON *respuesta) {

	ESP_LOGI(TAG, ""TRAZAR"Consultamos el estado de la aplicacion", INFOTRAZA);
    //cJSON_AddNumberToObject(respuesta, APP_COMAND_ESTADO_RELE, gpio_get_level(CONFIG_GPIO_PIN_RELE));
    cJSON_AddNumberToObject(respuesta, DEVICE_STATE, datosApp->datosGenerales->estadoApp);
    cJSON_AddNumberToObject(respuesta, TEMPERATURA, datosApp->termostato.tempActual);
    cJSON_AddNumberToObject(respuesta, HUMEDAD, datosApp->termostato.humedad);
    cJSON_AddNumberToObject(respuesta, MARGEN_TEMPERATURA, datosApp->termostato.margenTemperatura);
    cJSON_AddNumberToObject(respuesta, INTERVALO_LECTURA, datosApp->termostato.intervaloLectura);
    cJSON_AddNumberToObject(respuesta, INTERVALO_REINTENTOS, datosApp->termostato.intervaloReintentos);
    cJSON_AddNumberToObject(respuesta, REINTENTOS_LECTURA, datosApp->termostato.reintentosLectura);
    cJSON_AddNumberToObject(respuesta, CALIBRADO, datosApp->termostato.calibrado);


    //appUser_parametrosAplicacion(datosApp, respuesta);
    escribir_programa_actual(datosApp, respuesta);
    codigoRespuesta(respuesta,RESP_OK);



}



esp_err_t appUser_analizarComandoAplicacion(cJSON *peticion, int nComando, DATOS_APLICACION *datosApp, cJSON *respuesta) {

	ESP_LOGI(TAG, ""TRAZAR"Analizamos el comando de aplicacion especifico al dispositivo...", INFOTRAZA);

    switch(nComando) {
        case STATUS_DISPOSITIVO:
            consultarEstadoAplicacion(datosApp, respuesta);
            break;
        default:
            visualizar_comando_desconocido(datosApp, respuesta);
            break;
    }


	return ESP_OK;
}
/*
void pulsacion(DATOS_APLICACION *datosApp) {

    static os_timer_t repeticion;
    static uint8_t rep=0;
    ESP_LOGI(TAG, ""TRAZAR"RUTINA QUE TRATA LAS INTERRUPCIONES", INFOTRAZA);
    if (gpio_get_level(CONFIG_GPIO_PIN_BOTON) == OFF) {
        os_timer_disarm(&repeticion);
        os_timer_setfn(&repeticion, (os_timer_func_t*) pulsacion, datosApp);
        os_timer_arm(&repeticion, 500,0);
        rep++;
        ESP_LOGI(TAG, ""TRAZAR"repeticion %d", INFOTRAZA, rep);

    } else {
        datosApp->datosGenerales->botonPulsado = false;
        if (rep > NUM_REPETICIONES) {

            ESP_LOGI(TAG, ""TRAZAR"pulsacion larga", INFOTRAZA);



            if (tcpip_adapter_is_netif_up(ESP_IF_WIFI_STA) == true) {
            	ESP_LOGI(TAG, ""TRAZAR"EJECUTAMOS RESTART", INFOTRAZA);
                esp_restart();
            } else {
                ESP_LOGI(TAG, ""TRAZAR"AQUI LANZARIAMOS LA RUTINA DE SMARTCONFIG", INFOTRAZA);
                appuser_notificar_smartconfig();
                xTaskCreate(tarea_smartconfig, "tarea_smart", 2048, (void*)&datosApp, tskIDLE_PRIORITY + 0, NULL);


            }

        } else {
            ESP_LOGI(TAG, ""TRAZAR"pulsacion corta", INFOTRAZA);



        }

        rep=0;
    }
}
*/
/*

static void tratarInterrupcionesPulsador(DATOS_APLICACION *datosApp) {

    static os_timer_t temporizador;
    if (datosApp->datosGenerales->botonPulsado == false) {
        datosApp->datosGenerales->botonPulsado = true;
        os_timer_disarm(&temporizador);
        os_timer_setfn(&temporizador, (os_timer_func_t*) pulsacion, datosApp);
        os_timer_arm(&temporizador, 250,0);

    }

}

static void tratar_interrupciones(void *arg)
{
    DATOS_APLICACION datosApp;

    ESP_LOGI(TAG, ""TRAZAR"COMIENZO DE LA TAREA DE ATENCION A LA INTERRUPCION", INFOTRAZA);

    for (;;) {
        if (xQueueReceive(cola_gpio, &datosApp, portMAX_DELAY)) {
            tratarInterrupcionesPulsador(&datosApp);
        }
    }
}


static void isr_handler(void *dato) {

	DATOS_APLICACION *datosApp = dato;
	xQueueSendFromISR(cola_gpio, datosApp, NULL);

}
*/

/**
 * @brief Esta funcion inicializa los parametros iniciales especificos para la aplicacion.
 * @param datosApp. Estructura completa de la aplicacion
 */
esp_err_t appuser_inicializar_aplicacion(DATOS_APLICACION *datosApp) {

	/*
	esp_err_t error = ESP_OK;

    datosApp->datosGenerales->botonPulsado = false;

    ESP_LOGI(TAG, ""TRAZAR"INICIALIZAR PARAMETROS PARTICULARES DE LA APLICACION", INFOTRAZA);

    gpio_config_t io_conf;
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set,e.g.GPIO15/16
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    //disable pull-down mode
    io_conf.pull_down_en = 0;
    //disable pull-up mode
    io_conf.pull_up_en = 0;
    //configure GPIO with the given settings
    gpio_config(&io_conf);

    //interrupt of rising edge
    io_conf.intr_type = GPIO_INTR_POSEDGE;
    //bit mask of the pins, use GPIO4/5 here
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    //set as input mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //enable pull-up mode
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);

    gpio_set_level(CONFIG_GPIO_PIN_RELE, OFF);
    //change gpio intrrupt type for one pin
    gpio_set_intr_type(CONFIG_GPIO_PIN_BOTON, GPIO_INTR_ANYEDGE);



    gpio_set_level(CONFIG_GPIO_PIN_RELE, OFF);
    //change gpio intrrupt type for one pin
    gpio_set_intr_type(CONFIG_GPIO_PIN_BOTON, GPIO_INTR_ANYEDGE);


    //create a queue to handle gpio event from isr
    cola_gpio = xQueueCreate(3, sizeof(DATOS_APLICACION));

    xTaskCreate(tratar_interrupciones, "tratar_interrupciones", 4096, NULL, 10, NULL);
    //install gpio isr service
    gpio_install_isr_service(0);
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(CONFIG_GPIO_PIN_BOTON, isr_handler, (void *) datosApp);

    return error;
*/
	return ESP_OK;

}



esp_err_t leer_temperatura_local(DATOS_APLICACION *datosApp) {

    esp_err_t error = ESP_FAIL;
    static uint8_t contador = 0;


    ESP_LOGI(TAG, ""TRAZAR" Leyendo desde el sensor dht", INFOTRAZA);
    while (error != ESP_OK) {
    	error = dht_read_float_data(
    			DHT_TYPE_AM2301,
    			CONFIG_GPIO_PIN_DHT,
    			&datosApp->termostato.humedad,
    			&datosApp->termostato.tempActual);


    	if ((error == ESP_OK) && ((datosApp->termostato.humedad != 0) && (datosApp->termostato.tempActual != 0))) {
    		ESP_LOGI(TAG, ""TRAZAR" Lectura local correcta!. ", INFOTRAZA);
    		datosApp->termostato.tempActual = datosApp->termostato.tempActual + datosApp->termostato.calibrado;
    		contador = 0;
    	} else {
    		contador++;
    		if (contador == 4)  {
    			registrar_alarma(datosApp, NOTIFICACION_ALARMA_SENSOR_DHT, ALARMA_SENSOR_DHT, ALARMA_WARNING, true);

    		}
    		if (contador == 10) {
    			registrar_alarma(datosApp, NOTIFICACION_ALARMA_SENSOR_DHT, ALARMA_SENSOR_DHT, ALARMA_ON, true);

    		}
    		ESP_LOGE(TAG, ""TRAZAR" ERROR AL TOMAR LA LECTURA. REINTENTAMOS EN %d SEGUNDOS", INFOTRAZA, datosApp->termostato.intervaloReintentos);
        	vTaskDelay(datosApp->termostato.intervaloReintentos * 1000 / portTICK_RATE_MS);
    	}

    }

    if (datosApp->alarmas[ALARMA_SENSOR_DHT].estado_alarma > ALARMA_OFF) {
    	registrar_alarma(datosApp, NOTIFICACION_ALARMA_SENSOR_DHT, ALARMA_SENSOR_DHT, ALARMA_OFF, true);
    }


    return ESP_OK;

}


void tarea_lectura_temperatura(void *parametros) {
    cJSON* informe = NULL;
    DATOS_APLICACION *datosApp = (DATOS_APLICACION*) parametros;
    static float lecturaAnterior = -1000;


    while(1) {

    	leer_temperatura_local(datosApp);

    	ESP_LOGI(TAG, ""TRAZAR" temperatura :%lf, humedad:%lf, temperatura anterior :%lf", INFOTRAZA, datosApp->termostato.tempActual, datosApp->termostato.humedad, lecturaAnterior);

	    if (lecturaAnterior != datosApp->termostato.tempActual) {
            informe = appuser_generar_informe_espontaneo(datosApp, CAMBIO_TEMPERATURA, NULL);
            publicar_mensaje_json(datosApp, informe, NULL);
	    }
	    lecturaAnterior = datosApp->termostato.tempActual;
	    vTaskDelay(datosApp->termostato.intervaloLectura * 1000 / portTICK_RATE_MS);


    }

}

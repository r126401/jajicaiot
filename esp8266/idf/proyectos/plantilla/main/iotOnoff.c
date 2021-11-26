/*
 * iotOnoff.c
 *
 *  Created on: 6 sept. 2020
 *      Author: t126401
 */



#include <dialogos_json.h>
#include "configuracion_usuario.h"
#include "esp_err.h"
#include "esp_log.h"
#include "datos_comunes.h"
#include "programmer.h"
#include "api_json.h"
#include "driver/gpio.h"
#include "mqtt.h"
#include "conexiones.h"
#include "funciones_usuario.h"
#include "interfaz_usuario.h"


static const char *TAG = "IOTONOFF";
xQueueHandle cola_gpio = NULL;

#define GPIO_OUTPUT_PIN_SEL  ((1ULL<<CONFIG_GPIO_PIN_RELE) | (1ULL<<CONFIG_GPIO_PIN_LED))
#define GPIO_INPUT_PIN_SEL  ((1ULL<<CONFIG_GPIO_PIN_BOTON) | (1ULL<<CONFIG_GPIO_PIN_RELE))

#define NUM_REPETICIONES    3





void consultarEstadoAplicacion(DATOS_APLICACION *datosApp, cJSON *respuesta) {

	ESP_LOGI(TAG, ""TRAZAR"Consultamos el estado de la aplicacion", INFOTRAZA);
    cJSON_AddNumberToObject(respuesta, APP_COMAND_ESTADO_RELE, gpio_get_level(CONFIG_GPIO_PIN_RELE));
    cJSON_AddNumberToObject(respuesta, DEVICE_STATE, datosApp->datosGenerales->estadoApp);
    cJSON_AddNumberToObject(respuesta, PROGRAMMER_STATE, datosApp->datosGenerales->estadoProgramacion);
    escribir_programa_actual(datosApp, respuesta);
    codigoRespuesta(respuesta,RESP_OK);



}


void respuesta_actuacion_rele_remoto(DATOS_APLICACION *datosApp, cJSON *respuesta) {

    cJSON_AddNumberToObject(respuesta, DLG_COD_RESPUESTA, DLG_OK_CODE);
    cJSON_AddNumberToObject(respuesta, APP_COMAND_ESTADO_RELE, gpio_get_level(CONFIG_GPIO_PIN_RELE));
    cJSON_AddNumberToObject(respuesta, DEVICE_STATE, datosApp->datosGenerales->estadoApp);
    cJSON_AddNumberToObject(respuesta, PROGRAMMER_STATE, datosApp->datosGenerales->estadoProgramacion);
    escribir_programa_actual(datosApp, respuesta);
    codigoRespuesta(respuesta,DLG_OK_CODE);
}


enum ESTADO_RELE operacion_rele(DATOS_APLICACION *datosApp, enum TIPO_ACTUACION_RELE tipo, enum ESTADO_RELE operacion) {





    switch(tipo) {
        case MANUAL:
        case REMOTA:
            if (gpio_get_level(CONFIG_GPIO_PIN_RELE) == OFF) {
            	ESP_LOGI(TAG, ""TRAZAR"SE ENCIENDE", INFOTRAZA);
                gpio_set_level(CONFIG_GPIO_PIN_RELE, ON);

            } else {
            	ESP_LOGI(TAG, ""TRAZAR"SE APAGA", INFOTRAZA);
            	gpio_set_level(CONFIG_GPIO_PIN_RELE, OFF);

            }
            ESP_LOGI(TAG, ""TRAZAR"EL RELE SE HA PUESTO A %d", INFOTRAZA, gpio_get_level(CONFIG_GPIO_PIN_RELE));
            break;
        case TEMPORIZADA:
        	gpio_set_level(CONFIG_GPIO_PIN_RELE, operacion);
            break;
        default:
            break;

    }

    return gpio_get_level(CONFIG_GPIO_PIN_RELE);


}




esp_err_t appUser_analizarComandoAplicacion(cJSON *peticion, int nComando, DATOS_APLICACION *datosApp, cJSON *respuesta) {

	ESP_LOGI(TAG, ""TRAZAR"Analizamos el comando de aplicacion especifico al dispositivo...", INFOTRAZA);

    switch(nComando) {
        case OPERAR_RELE:
            operacion_rele(datosApp, MANUAL, INDETERMINADO);
            respuesta_actuacion_rele_remoto(datosApp, respuesta);
            break;

        case STATUS_RELE:
            consultarEstadoAplicacion(datosApp, respuesta);
            break;
        default:
            visualizar_comando_desconocido(datosApp, respuesta);
            break;
    }


	return ESP_OK;
}

void pulsacion(DATOS_APLICACION *datosApp) {

    static os_timer_t repeticion;
    static uint8_t rep=0;
    cJSON *informe;




    ESP_LOGI(TAG, ""TRAZAR"RUTINA QUE TRATA LAS INTERRUPCIONES", INFOTRAZA);
    if (gpio_get_level(CONFIG_GPIO_PIN_BOTON) == OFF) {
        os_timer_disarm(&repeticion);
        os_timer_setfn(&repeticion, (os_timer_func_t*) pulsacion, datosApp);
        os_timer_arm(&repeticion, 500,0);
        rep++;
        ESP_LOGI(TAG, ""TRAZAR"repeticion %d", INFOTRAZA, rep);

    } else {
        datosApp->datosGenerales->botonPulsado = false;
        //*rebote = false;
        //printf("Rebote cancelado, rep = %d\n", rep);
        if (rep > NUM_REPETICIONES) {

            ESP_LOGI(TAG, ""TRAZAR"pulsacion larga", INFOTRAZA);



            if (tcpip_adapter_is_netif_up(ESP_IF_WIFI_STA) == true) {
            	ESP_LOGI(TAG, ""TRAZAR"EJECUTAMOS RESTART", INFOTRAZA);
                esp_restart();
            } else {
                //smartconfig
                ESP_LOGI(TAG,"sin ip, entramos en smartconfig...");
                //smartconfig_set_type(SC_TYPE_ESPTOUCH);
                ESP_LOGI(TAG, ""TRAZAR"AQUI LANZARIAMOS LA RUTINA DE SMARTCONFIG", INFOTRAZA);
                appuser_notificar_smartconfig();
                xTaskCreate(tarea_smartconfig, "tarea_smart", 2048, (void*)&datosApp, tskIDLE_PRIORITY + 0, NULL);


            }

        } else {
            ESP_LOGI(TAG, ""TRAZAR"pulsacion corta", INFOTRAZA);
            operacion_rele(datosApp, MANUAL, INDETERMINADO);
            ESP_LOGI(TAG, ""TRAZAR"AQUI ENVIAREMOS EL REPORTE DE QUE SE HA PULSADO EL BOTON", INFOTRAZA);
            informe = appuser_generar_informe_espontaneo(datosApp, ACTUACION_RELE_LOCAL, NULL);

            ESP_LOGI(TAG, ""TRAZAR"REPORTE GENERADO Y DISPUESTO PARA ENVIAR", INFOTRAZA);
            if (informe != NULL) {
            	publicar_mensaje_json(datosApp, informe);

            } else {
            	ESP_LOGE(TAG, ""TRAZAR"MENSAJE DE PULSACION CORTA INVALIDO", INFOTRAZA);
            }



        }

        rep=0;
    }
}



static void tratarInterrupcionesPulsador(DATOS_APLICACION *datosApp) {




    //uint32 gpio_status = GPIO_REG_READ(GPIO_STATUS_ADDRESS);
    static os_timer_t temporizador;
    //static bool rebote = false;
    //printf("han pulsado un boton....\n");

    if (datosApp->datosGenerales->botonPulsado == false) {
        datosApp->datosGenerales->botonPulsado = true;
        //os_timer_disarm(&notificacionWifi);
        os_timer_disarm(&temporizador);
        os_timer_setfn(&temporizador, (os_timer_func_t*) pulsacion, datosApp);
        os_timer_arm(&temporizador, 250,0);

    }




    //GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, gpio_status );
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


/**
 * @brief Esta funcion inicializa los parametros iniciales especificos para la aplicacion.
 * @param datosApp. Estructura completa de la aplicacion
 */
esp_err_t appuser_inicializar_aplicacion(DATOS_APLICACION *datosApp) {

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


    //create a queue to handle gpio event from isr
    cola_gpio = xQueueCreate(3, sizeof(DATOS_APLICACION));

    xTaskCreate(tratar_interrupciones, "tratar_interrupciones", 4096, NULL, 10, NULL);
    //install gpio isr service
    gpio_install_isr_service(0);
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(CONFIG_GPIO_PIN_BOTON, isr_handler, (void *) datosApp);

    return error;


}






/*
 * iotOnoff.c
 *
 *  Created on: 6 sept. 2020
 *      Author: t126401
 */


#include <math.h>
#include <dialogos_json.h>
#include "esp_err.h"
#include "esp_log.h"
#include "datos_comunes.h"
#include "programmer.h"
#include "api_json.h"
#include "driver/gpio.h"
#include "conexiones_mqtt.h"
#include "conexiones.h"
#include "funciones_usuario.h"
#include "interfaz_usuario.h"
#include "configuracion_usuario.h"
#include "dht22.h"
#include "configuracion.h"
#include "alarmas.h"
#include "lcd.h"
#include "ds18x20.h"


//static ETSTimer temporizadorTermometro;
ETSTimer temporizador_lectura_remota;
ETSTimer temporizador_refresco_led;

static const char *TAG = "IOTCRONOTEMP";
xQueueHandle cola_gpio = NULL;


#define GPIO_OUTPUT_PIN_SEL  ((1ULL<<CONFIG_GPIO_PIN_LED)| (1ULL<<CONFIG_GPIO_PIN_DHT) | (1ULL<<CONFIG_GPIO_PIN_LED_LCD) | (1ULL<<CONFIG_GPIO_PIN_DS18B20))
#define GPIO_INPUT_PIN_SEL  ( (1ULL<<CONFIG_GPIO_PIN_BOTON))

#define NUM_REPETICIONES    3
#define TIMEOUT_LECTURA_REMOTA 5000 //ms
#define TASA_REFRESCO_LED 5 //ms




void gpio_rele_in() {

	gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //bit mask of the pins, use GPIO4/5 here
    io_conf.pin_bit_mask = 1ULL<<CONFIG_GPIO_PIN_RELE;
    //set as input mode
    io_conf.mode = GPIO_MODE_INPUT;
    //enable pull-up mode
    io_conf.pull_up_en = 1;
    io_conf.pull_down_en = 1;
    gpio_config(&io_conf);
    ESP_LOGW(TAG, ""TRAZAR" gpio rele en entrada 11", INFOTRAZA);






}

void gpio_rele_out() {

	gpio_config_t io_conf;
	io_conf.intr_type = GPIO_INTR_DISABLE;
	io_conf.pin_bit_mask = 1ULL<<CONFIG_GPIO_PIN_RELE;
	io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pull_down_en = 0;
    //disable pull-up mode
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);
    ESP_LOGW(TAG, ""TRAZAR" gpio rele en salida", INFOTRAZA);

}

void gpio_rele_in_out() {
	gpio_config_t io_conf;
	io_conf.intr_type = GPIO_INTR_DISABLE;
	io_conf.pin_bit_mask = 1ULL<<CONFIG_GPIO_PIN_RELE;
	io_conf.mode = GPIO_MODE_INPUT_OUTPUT;
    io_conf.pull_down_en = 0;
    //disable pull-up mode
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);
    ESP_LOGW(TAG, ""TRAZAR" gpio rele en E/S", INFOTRAZA);

}



void accionar_termostato(DATOS_APLICACION *datosApp) {

	enum ESTADO_RELE accion_rele;
	enum TIPO_ACCION_TERMOSTATO accion_termostato;
	cJSON* informe = NULL;
	static float lecturaAnterior = -1000;

    if (((accion_termostato = calcular_accion_termostato(datosApp, &accion_rele)) == ACCIONAR_TERMOSTATO)) {
    	operacion_rele(datosApp, TEMPORIZADA, accion_rele);
    }

    if ((accion_termostato == ACCIONAR_TERMOSTATO) || (lecturaAnterior != datosApp->termostato.tempActual)) {
    	lv_actualizar_temperatura_lcd(datosApp);
        informe = appuser_generar_informe_espontaneo(datosApp, CAMBIO_TEMPERATURA, NULL);
        publicar_mensaje_json(datosApp, informe, NULL);
    }
    lecturaAnterior = datosApp->termostato.tempActual;
}


void consultarEstadoAplicacion(DATOS_APLICACION *datosApp, cJSON *respuesta) {

	ESP_LOGI(TAG, ""TRAZAR"Consultamos el estado de la aplicacion", INFOTRAZA);
	//gpio_rele_in();
    cJSON_AddNumberToObject(respuesta, APP_COMAND_ESTADO_RELE, gpio_get_level(CONFIG_GPIO_PIN_RELE));
    cJSON_AddNumberToObject(respuesta, DEVICE_STATE, datosApp->datosGenerales->estadoApp);
    cJSON_AddNumberToObject(respuesta, PROGRAMMER_STATE, datosApp->datosGenerales->estadoProgramacion);
    cJSON_AddNumberToObject(respuesta, TEMPERATURA, datosApp->termostato.tempActual);
    cJSON_AddNumberToObject(respuesta, HUMEDAD, datosApp->termostato.humedad);
    cJSON_AddNumberToObject(respuesta, UMBRAL_TEMPERATURA, datosApp->termostato.tempUmbral);
    cJSON_AddNumberToObject(respuesta, MARGEN_TEMPERATURA, datosApp->termostato.margenTemperatura);
    cJSON_AddNumberToObject(respuesta, INTERVALO_LECTURA, datosApp->termostato.intervaloLectura);
    cJSON_AddNumberToObject(respuesta, INTERVALO_REINTENTOS, datosApp->termostato.intervaloReintentos);
    cJSON_AddNumberToObject(respuesta, REINTENTOS_LECTURA, datosApp->termostato.reintentosLectura);
    cJSON_AddNumberToObject(respuesta, CALIBRADO, datosApp->termostato.calibrado);
    cJSON_AddBoolToObject(respuesta, MASTER, datosApp->termostato.master);
    cJSON_AddStringToObject(respuesta, SENSOR_REMOTO, datosApp->termostato.sensor_remoto);
    cJSON_AddNumberToObject(respuesta, UMBRAL_DEFECTO, datosApp->termostato.tempUmbralDefecto);


    //appUser_parametrosAplicacion(datosApp, respuesta);
    escribir_programa_actual(datosApp, respuesta);
    codigoRespuesta(respuesta,RESP_OK);



}


void respuesta_actuacion_rele_remoto(DATOS_APLICACION *datosApp, cJSON *respuesta) {

    cJSON_AddNumberToObject(respuesta, DLG_COD_RESPUESTA, DLG_OK_CODE);
    //gpio_rele_in();
    cJSON_AddNumberToObject(respuesta, APP_COMAND_ESTADO_RELE, gpio_get_level(CONFIG_GPIO_PIN_RELE));
    cJSON_AddNumberToObject(respuesta, DEVICE_STATE, datosApp->datosGenerales->estadoApp);
    cJSON_AddNumberToObject(respuesta, PROGRAMMER_STATE, datosApp->datosGenerales->estadoProgramacion);
    cJSON_AddNumberToObject(respuesta, TEMPERATURA, datosApp->termostato.tempActual);
    cJSON_AddNumberToObject(respuesta, HUMEDAD, datosApp->termostato.humedad);
    cJSON_AddNumberToObject(respuesta, UMBRAL_TEMPERATURA, datosApp->termostato.tempUmbral);
    escribir_programa_actual(datosApp, respuesta);
    codigoRespuesta(respuesta,DLG_OK_CODE);
}


enum ESTADO_RELE operacion_rele(DATOS_APLICACION *datosApp, enum TIPO_ACTUACION_RELE tipo, enum ESTADO_RELE operacion) {

	enum ESTADO_RELE rele;


	//gpio_reset_pin(CONFIG_GPIO_PIN_RELE);
	//gpio_rele_in();
	switch (tipo) {
	case MANUAL:
		if (gpio_get_level(CONFIG_GPIO_PIN_RELE) == OFF) {
			ESP_LOGW(TAG, ""TRAZAR" ESTABA A OFF Y SE ENCIENDE", INFOTRAZA);
			rele = ON;
		} else {
			ESP_LOGW(TAG, ""TRAZAR" ESTABA A On Y SE APAGA", INFOTRAZA);
			rele = OFF;
		}
		break;
	default:
		rele = operacion;
		break;
	}
	//gpio_rele_out();
	gpio_set_level(CONFIG_GPIO_PIN_RELE, rele);
	ESP_LOGW(TAG, ""TRAZAR"EL RELE SE HA PUESTO A %d", INFOTRAZA, rele);
	lv_actualizar_rele_lcd(rele);
	//gpio_set_direction(CONFIG_GPIO_PIN_RELE, GPIO_MODE_INPUT);

	return rele;
}


/*
enum ESTADO_RELE operar_rele(DATOS_APLICACION *datosApp, enum TIPO_ACTUACION_RELE tipo, enum ESTADO_RELE operacion) {


    gpio_reset_pin(CONFIG_GPIO_PIN_RELE);
    gpio_set_direction(CONFIG_GPIO_PIN_RELE, GPIO_MODE_INPUT);
    switch(tipo) {
        case MANUAL:
        case REMOTA:
            if (gpio_get_level(CONFIG_GPIO_PIN_RELE) == OFF) {
                gpio_reset_pin(CONFIG_GPIO_PIN_RELE);
                gpio_set_direction(CONFIG_GPIO_PIN_RELE, GPIO_MODE_OUTPUT);

            	ESP_LOGI(TAG, ""TRAZAR"SE ENCIENDE", INFOTRAZA);

                gpio_set_level(CONFIG_GPIO_PIN_RELE, ON);
                ESP_LOGW(TAG, ""TRAZAR"EL RELE SE HA PUESTO A 1", INFOTRAZA);

            } else {
            	ESP_LOGI(TAG, ""TRAZAR"SE APAGA", INFOTRAZA);
                gpio_reset_pin(CONFIG_GPIO_PIN_RELE);
                gpio_set_direction(CONFIG_GPIO_PIN_RELE, GPIO_MODE_OUTPUT);

            	gpio_set_level(CONFIG_GPIO_PIN_RELE, OFF);
            	ESP_LOGW(TAG, ""TRAZAR"EL RELE SE HA PUESTO A 0", INFOTRAZA);

            }
            break;
        case TEMPORIZADA:
        	gpio_set_level(CONFIG_GPIO_PIN_RELE, operacion);
            break;
        default:
            break;

    }


    ESP_LOGI(TAG, ""TRAZAR"EL RELE SE HA PUESTO A %d", INFOTRAZA, gpio_get_level(CONFIG_GPIO_PIN_RELE));

    return gpio_get_level(CONFIG_GPIO_PIN_RELE);


}
*/
bool modificarUmbralTemperatura(cJSON *peticion, DATOS_APLICACION *datosApp, cJSON *respuesta) {

    cJSON *nodo = NULL;
    cJSON *campo = NULL;




    nodo = cJSON_GetObjectItem(peticion, MODIFICAR_APP);
   if(nodo == NULL) {
       return NULL;
   }

    printf("modificarUmbralTemperatura-->comienzo\n");
    campo = cJSON_GetObjectItem(nodo, UMBRAL_TEMPERATURA);
       if((campo != NULL) && (campo->type == cJSON_Number)) {
           printf("modificando umbral\n");
           datosApp->termostato.tempUmbral = campo->valuedouble;
           /*
           if (datosApp->datosGenerales->estadoApp == NORMAL_AUTO) {
        	   appuser_cambiar_modo_aplicacion(datosApp, NORMAL_AUTOMAN);
           }
           */
           lv_actualizar_umbral_temperatura_lcd(datosApp);
           //datosApp->datosGenerales->estadoApp = NORMAL_AUTOMAN;
           //guardarConfiguracion(datosApp, 0);
           cJSON_AddNumberToObject(respuesta, UMBRAL_TEMPERATURA, datosApp->termostato.tempUmbral);
           codigoRespuesta(respuesta, RESP_OK);
       } else {
           codigoRespuesta(respuesta, RESP_NOK);
       }
    return true;
}


esp_err_t appUser_analizarComandoAplicacion(cJSON *peticion, int nComando, DATOS_APLICACION *datosApp, cJSON *respuesta) {

	ESP_LOGI(TAG, ""TRAZAR"Analizamos el comando de aplicacion especifico al dispositivo...", INFOTRAZA);

    switch(nComando) {
        case OPERAR_RELE:
            operacion_rele(datosApp, MANUAL, INDETERMINADO);
            respuesta_actuacion_rele_remoto(datosApp, respuesta);
            break;

        case STATUS_DISPOSITIVO:
            consultarEstadoAplicacion(datosApp, respuesta);
            break;
        case MODIFICAR_UMBRAL:
            if ((modificarUmbralTemperatura(peticion, datosApp, respuesta) == true)) {
            	accionar_termostato(datosApp);
            }
            break;
        case SELECCIONAR_SENSOR:
        	seleccionarSensorTemperatura(peticion, datosApp, respuesta);
        	break;

        default:
            visualizar_comando_desconocido(datosApp, respuesta);
            break;
    }


	return ESP_OK;
}



void pulsacion_modo_app(DATOS_APLICACION *datosApp) {

	time_t t_siguiente_intervalo;
	ESP_LOGI(TAG, ""TRAZAR" PONER QUITAR MODO MANUAL :%d", INFOTRAZA, datosApp->datosGenerales->estadoApp );
	switch (datosApp->datosGenerales->estadoApp) {

	case NORMAL_AUTO:
	case NORMAL_AUTOMAN:
		//datosApp->termostato.tempUmbral = datosApp->termostato.tempUmbralDefecto;
		appuser_cambiar_modo_aplicacion(datosApp, NORMAL_MANUAL);
		break;
	case NORMAL_MANUAL:
		//ESP_LOGE(TAG, ""TRAZAR" pasamos a modo auto", INFOTRAZA);
		appuser_cambiar_modo_aplicacion(datosApp, NORMAL_AUTO);
		calcular_programa_activo(datosApp, &t_siguiente_intervalo);
		break;
	default:
		//ESP_LOGE(TAG, ""TRAZAR" NO SE PUEDE CAMBIAR EL ESTADO POR NO SER CONSISTENTE. ESTADO %d", INFOTRAZA, datosApp->datosGenerales->estadoApp);
		break;

	}
	leer_temperatura(datosApp);


}

void pulsacion(DATOS_APLICACION *datosApp) {

    static ETSTimer repeticion;
    static uint8_t rep=0;
    cJSON *informe;




    ESP_LOGW(TAG, "pulsacion  %p", datosApp);
    ESP_LOGI(TAG, ""TRAZAR"RUTINA QUE TRATA LAS INTERRUPCIONES", INFOTRAZA);
    //gpio_rele_in();
    if (gpio_get_level(CONFIG_GPIO_PIN_BOTON) == OFF) {
        ets_timer_disarm(&repeticion);
        ets_timer_setfn(&repeticion, (ETSTimerFunc*) pulsacion, datosApp);
        ets_timer_arm(&repeticion, 500,0);
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
                appuser_notificar_smartconfig(datosApp);
                xTaskCreate(tarea_smartconfig, "tarea_smart", 2048, (void*)&datosApp, tskIDLE_PRIORITY + 0, NULL);


            }

        } else {
            //ESP_LOGI(TAG, ""TRAZAR"pulsacion corta 0x%08x", INFOTRAZA, datosApp);
            ESP_LOGW(TAG, "pulsacion corta %p", datosApp);
            pulsacion_modo_app(datosApp);
            /*
            if (datosApp->datosGenerales->estadoApp < 2) {
            	appuser_cambiar_modo_aplicacion(datosApp, NORMAL_MANUAL);

            } else {
                if (datosApp->datosGenerales->estadoApp == NORMAL_MANUAL) {
            		appuser_cambiar_modo_aplicacion(datosApp, NORMAL_AUTO);


                }
            }*/
            informe = appuser_generar_informe_espontaneo(datosApp, CAMBIO_ESTADO_APLICACION, NULL);
            publicar_mensaje_json(datosApp, informe, NULL);
            //poner_quitar_modo_manual(datosApp);

            //operacion_rele(datosApp, MANUAL, INDETERMINADO);

        }

        rep=0;
    }
}



static void tratarInterrupcionesPulsador(DATOS_APLICACION *datosApp) {




    //uint32 gpio_status = GPIO_REG_READ(GPIO_STATUS_ADDRESS);
    static ETSTimer temporizador;
    //static bool rebote = false;
    //printf("han pulsado un boton....\n");

    if (datosApp->datosGenerales->botonPulsado == false) {
        datosApp->datosGenerales->botonPulsado = true;
        //ets_timer_disarm(&notificacionWifi);
        ets_timer_disarm(&temporizador);
        ets_timer_setfn(&temporizador, (ETSTimerFunc*) pulsacion, datosApp);
        ets_timer_arm(&temporizador, 250,0);

    }




    //GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, gpio_status );
}
/*
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
*/
/*
static void isr_handler(void *dato) {

	DATOS_APLICACION *datosApp = dato;
	xQueueSendFromISR(cola_gpio, dato, NULL);

}
*/
/**
 * Esta funcion inicaliza el lcd y dispara la tarea para el lcd
 */
esp_err_t appuser_inicializar_lcd(DATOS_APLICACION *datosApp) {


	//xTaskCreatePinnedToCore(tarea_lcd, "gui", 4096*2, datosApp, 5, NULL, 1);
	xTaskCreate(tarea_lcd, "gui", 2048*2, datosApp, 5, NULL);

    //xTaskCreate(tarea_lcd, "gui", 4096*2, datosApp, 10, NULL);


	return ESP_OK;
}


static void refresco_lcd(void) {


	if (gpio_get_level(CONFIG_GPIO_PIN_LED_LCD) == OFF) {
		gpio_set_level(CONFIG_GPIO_PIN_LED_LCD, ON);

	} else {
		gpio_set_level(CONFIG_GPIO_PIN_LED_LCD, OFF);
	}


}


/**
 * @brief Esta funcion inicializa los parametros iniciales especificos para la aplicacion.
 * @param datosApp. Estructura completa de la aplicacion
 */
esp_err_t appuser_inicializar_aplicacion(DATOS_APLICACION *datosApp) {

	esp_err_t error = ESP_OK;
	ESP_LOGW(TAG, "appuser_inicializar_aplicacion %p", datosApp);
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
    //ESP_LOGI(TAG, ""TRAZAR" VAMOS (0)", INFOTRAZA);
    gpio_config(&io_conf);
    //ESP_LOGI(TAG, ""TRAZAR" VAMOS (1)", INFOTRAZA);
    //interrupt of rising edge
    io_conf.intr_type = GPIO_INTR_POSEDGE;
    //bit mask of the pins, use GPIO4/5 here
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    //set as input mode
    io_conf.mode = GPIO_MODE_INPUT;
    //enable pull-up mode
    io_conf.pull_up_en = 1;

    gpio_config(&io_conf);

    //gpio_set_level(CONFIG_GPIO_PIN_RELE, OFF);
    //change gpio intrrupt type for one pin
    //gpio_set_intr_type(CONFIG_GPIO_PIN_BOTON, GPIO_INTR_ANYEDGE);


    gpio_rele_in_out();
//    gpio_set_level(CONFIG_GPIO_PIN_LED_LCD, ON);
    gpio_set_pull_mode(CONFIG_GPIO_PIN_DHT, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(CONFIG_GPIO_PIN_DS18B20, GPIO_PULLUP_ONLY);
    ets_timer_disarm(&temporizador_refresco_led);
    ets_timer_setfn(&temporizador_refresco_led, (ETSTimerFunc*) refresco_lcd, NULL);
    ets_timer_arm(&temporizador_refresco_led, TASA_REFRESCO_LED,true);



    gpio_set_level(CONFIG_GPIO_PIN_RELE, OFF);
    //change gpio intrrupt type for one pin
    gpio_set_intr_type(CONFIG_GPIO_PIN_BOTON, GPIO_INTR_ANYEDGE);


    ESP_LOGI(TAG, ""TRAZAR" VAMOS A CREAR LA COLA GPIO", INFOTRAZA);
    //create a queue to handle gpio event from isr
    cola_gpio = xQueueCreate(3, sizeof(DATOS_APLICACION));
    ESP_LOGI(TAG, ""TRAZAR" COLA GPIO CREADA", INFOTRAZA);
    //xTaskCreate(tratar_interrupciones, "tratar_interrupciones", 4096, NULL, 10, NULL);
    ESP_LOGI(TAG, ""TRAZAR" RUTINA DE TRATAR INTERRUPCIONES LANZADA", INFOTRAZA);
    //install gpio isr service
    gpio_install_isr_service(0);
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(CONFIG_GPIO_PIN_BOTON, tratarInterrupcionesPulsador,datosApp);
    ESP_LOGI(TAG, ""TRAZAR"RUTINA ISR_HANDLER LANZADA", INFOTRAZA);

    appuser_inicializar_lcd(datosApp);

    return error;


}

float redondear_temperatura(float temperatura) {

	float redondeado;
	float diferencia;
	float resultado = 0;
	float valor_absoluto;

	redondeado = lround(temperatura);
	diferencia = temperatura - redondeado;
	valor_absoluto = fabs(redondeado);
	if (diferencia < 0.25) resultado = valor_absoluto;
	if ((diferencia > 0.25 ) && (diferencia < 0.5)) resultado = valor_absoluto + 0.5;

	if ((diferencia < -0.25)) resultado = valor_absoluto - 0.5;

	return resultado;

}


enum TIPO_ACCION_TERMOSTATO calcular_accion_termostato(DATOS_APLICACION *datosApp, enum ESTADO_RELE *accion) {

    // El termostato esta apagado y la temperatura actual es menor o igual que la umbral.


	//gpio_rele_in();
    if (gpio_get_level(CONFIG_GPIO_PIN_RELE) == OFF) {
               if (datosApp->termostato.tempActual <= (datosApp->termostato.tempUmbral - datosApp->termostato.margenTemperatura)) {
            	   ESP_LOGI(TAG, ""TRAZAR"RELE OFF Y SE ENCIENDE. tempMedida: %.2f, tempUmbral: %.02f", INFOTRAZA, datosApp->termostato.tempActual, datosApp->termostato.tempUmbral);
                   *accion = ON;
                   return ACCIONAR_TERMOSTATO;
               } else {
            	   ESP_LOGI(TAG, ""TRAZAR"RELE OFF Y SIGUE OFF. tempMedida: %.2f, tempUmbral: %.02f", INFOTRAZA, datosApp->termostato.tempActual, datosApp->termostato.tempUmbral);
                   *accion = OFF;
                   return NO_ACCIONAR_TERMOSTATO;

               }
           } else {
               if (datosApp->termostato.tempActual >= (datosApp->termostato.tempUmbral + datosApp->termostato.margenTemperatura) ) {
            	   ESP_LOGI(TAG, ""TRAZAR"RELE ON Y SE ENCIENDE. tempMedida: %.2f, tempUmbral: %.02f", INFOTRAZA, datosApp->termostato.tempActual, datosApp->termostato.tempUmbral);
                   *accion = OFF;
                   return ACCIONAR_TERMOSTATO;
               } else {
            	   ESP_LOGI(TAG, ""TRAZAR"RELE ON Y SIGUE ON. tempMedida: %.2f, tempUmbral: %.02f", INFOTRAZA, datosApp->termostato.tempActual, datosApp->termostato.tempUmbral);
                   *accion = ON;
                   return NO_ACCIONAR_TERMOSTATO;

               }
           }

}


esp_err_t leer_temperatura_local(DATOS_APLICACION *datosApp) {

    esp_err_t error = ESP_FAIL;
    static uint8_t contador = 0;
    char temp[15]={0};
	float temperatura_a_redondear;


    ESP_LOGI(TAG, ""TRAZAR" Leyendo desde el sensor dht", INFOTRAZA);
    while (error != ESP_OK) {
#ifdef CONFIG_DHT22

    	error = dht_read_float_data(
    			DHT_TYPE_AM2301,
    			CONFIG_GPIO_PIN_DHT,
    			&datosApp->termostato.humedad,
    			&datosApp->termostato.tempActual);

    	float dato;
    	lectura_temperatura_ds18x20(&dato);
    	ESP_LOGI(TAG, ""TRAZAR" lectura ds18x20: %lf", INFOTRAZA, dato);

#endif
#ifdef CONFIG_DS18B20

    	error = lectura_temperatura_ds18x20(&datosApp->termostato.tempActual);


    	datosApp->termostato.humedad = 5;

#endif



    	if ((error == ESP_OK) && ((datosApp->termostato.humedad != 0) && (datosApp->termostato.tempActual != 0))) {
    		ESP_LOGI(TAG, ""TRAZAR" Lectura local correcta!. ", INFOTRAZA);
    		datosApp->termostato.tempActual = datosApp->termostato.tempActual + datosApp->termostato.calibrado;
    		temperatura_a_redondear = datosApp->termostato.tempActual;
          	datosApp->termostato.tempActual = redondear_temperatura(temperatura_a_redondear);
            	ESP_LOGI(TAG, ""TRAZAR" Temp sin redondeo %.01lf, Temp redondeada %.01lf ", INFOTRAZA, temperatura_a_redondear,datosApp->termostato.tempActual );
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
    sprintf(temp,"%.02lf ºC", datosApp->termostato.tempActual);
    notificar_fin_arranque(datosApp);

    return ESP_OK;

}

esp_err_t leer_temperatura(DATOS_APLICACION *datosApp) {

	if ((datosApp->termostato.master == false)) {
		if (datosApp->alarmas[ALARMA_SENSOR_REMOTO].estado_alarma == ALARMA_ON) {
			ESP_LOGW(TAG, ""TRAZAR" termostato en remoto. ADEMAS LA ALARMA ESTA A ON", INFOTRAZA);
			leer_temperatura_local(datosApp);
		}
		leer_temperatura_remota(datosApp);

	} else {
		leer_temperatura_local(datosApp);
	}




	return ESP_OK;
}

static void temporizacion_lectura_remota(DATOS_APLICACION *datosApp) {

	static int contador = 0;

	contador++;
	switch(datosApp->alarmas[ALARMA_SENSOR_REMOTO].estado_alarma) {

	case ALARMA_INDETERMINADA:
		registrar_alarma(datosApp, NOTIFICACION_ALARMA_SENSOR_REMOTO, ALARMA_SENSOR_REMOTO, ALARMA_WARNING, true);
		break;
	case ALARMA_OFF:
		contador = 0;
		break;
	case ALARMA_WARNING:
		ESP_LOGE(TAG, ""TRAZAR" ALARMA WARNING EN SENSOR REMOTO. CONTADOR %d ", INFOTRAZA, contador);
		if (contador == 5) {
			if (datosApp->alarmas[ALARMA_SENSOR_REMOTO].estado_alarma != ALARMA_ON) {
				registrar_alarma(datosApp, NOTIFICACION_ALARMA_SENSOR_REMOTO, ALARMA_SENSOR_REMOTO, ALARMA_ON, true);
			}
		}
		break;
	case ALARMA_ON:
		ESP_LOGE(TAG, ""TRAZAR" ALARMA ON EN SENSOR REMOTO. LEEMOS EN LOCAL", INFOTRAZA);
		break;
	}




}

esp_err_t leer_temperatura_remota(DATOS_APLICACION *datosApp) {

	ESP_LOGI(TAG, ""TRAZAR" Leyendo desde el sensor remoto %s", INFOTRAZA, datosApp->termostato.sensor_remoto);
	cJSON *objeto = NULL;
	cJSON *comando;
	comando = cJSON_CreateObject();
	char topic[50];
	strcpy(topic, "/sub_");
	strcat(topic, datosApp->termostato.sensor_remoto);
	objeto = cabeceraGeneral(datosApp);
	if (objeto != NULL) {


		cJSON_AddNumberToObject(objeto, DLG_COMANDO, STATUS_DISPOSITIVO);
		cJSON_AddItemToObject(comando, COMANDO, objeto);
		publicar_mensaje_json(datosApp, comando, topic);
	}


    if (datosApp->alarmas[ALARMA_SENSOR_REMOTO].estado_alarma == ALARMA_OFF) {
    	registrar_alarma(datosApp, NOTIFICACION_ALARMA_SENSOR_REMOTO, ALARMA_SENSOR_REMOTO, ALARMA_INDETERMINADA, false);
    }
    ets_timer_disarm(&temporizador_lectura_remota);
    ets_timer_setfn(&temporizador_lectura_remota, (ETSTimerFunc*) temporizacion_lectura_remota, datosApp);
    ets_timer_arm(&temporizador_lectura_remota, TIMEOUT_LECTURA_REMOTA,0);




	return ESP_OK;
}






void tarea_lectura_temperatura(void *parametros) {
    enum ESTADO_RELE accion_rele;
    cJSON* informe = NULL;
    DATOS_APLICACION *datosApp = (DATOS_APLICACION*) parametros;
    static float lecturaAnterior = -1000;
    enum TIPO_ACCION_TERMOSTATO accion_termostato;


    while(1) {

    	ESP_LOGE(TAG, ""TRAZAR" tempUmbral %.02f", INFOTRAZA, datosApp->termostato.tempUmbral);
    	leer_temperatura(datosApp);
    	ESP_LOGE(TAG, ""TRAZAR" tempUmbral %.02f", INFOTRAZA, datosApp->termostato.tempUmbral);
    	accionar_termostato(datosApp);
    	/*
	    if (((accion_termostato = calcular_accion_termostato(datosApp, &accion_rele)) == ACCIONAR_TERMOSTATO)) {
	    	operacion_rele(datosApp, TEMPORIZADA, accion_rele);
	    }

	    if ((accion_termostato == ACCIONAR_TERMOSTATO) || (lecturaAnterior != datosApp->termostato.tempActual)) {
	    	lv_actualizar_temperatura_lcd(datosApp);
            informe = appuser_generar_informe_espontaneo(datosApp, CAMBIO_TEMPERATURA, NULL);
            publicar_mensaje_json(datosApp, informe, NULL);

            //lv_actualizar_humedad_lcd(datosApp);
	    }

	    lecturaAnterior = datosApp->termostato.tempActual;
	    */
	    vTaskDelay(datosApp->termostato.intervaloLectura * 1000 / portTICK_RATE_MS);


    }

}

esp_err_t seleccionarSensorTemperatura(cJSON *peticion, DATOS_APLICACION *datosApp, cJSON *respuesta) {

	cJSON *nodo = NULL;
    bool master = true;


    nodo = cJSON_GetObjectItem(peticion, MODIFICAR_SENSOR_TEMPERATURA);
   if(nodo == NULL) {
	   ESP_LOGE(TAG, ""TRAZAR" NO SE ENCUENTRA EL PATRON DE SENSOR EN EL COMANDO", INFOTRAZA);
	   codigoRespuesta(respuesta, RESP_NOK);
	   return ESP_FAIL;
   }



   if (extraer_dato_uint8(nodo,  MASTER, (uint8_t*) &master) != ESP_OK) {
	   ESP_LOGE(TAG, ""TRAZAR" NO VIENE EL CAMPO MASTER EN LA PETICION", INFOTRAZA);
	   codigoRespuesta(respuesta, RESP_NOK);
	   return ESP_FAIL;
   }

   if (master) {
	   ESP_LOGI(TAG, ""TRAZAR" Se modifica el sensor para que el master sea el dispositivo", INFOTRAZA);
	   memset(datosApp->termostato.sensor_remoto, 0,sizeof(datosApp->termostato.sensor_remoto));
   } else {
	   ESP_LOGE(TAG, ""TRAZAR"ante: %d\n", INFOTRAZA, esp_get_free_heap_size());
	   //strcpy(datosApp->termostato.sensor_remoto, cJSON_GetObjectItem(nodo, SENSOR_REMOTO)->valuestring);
	   extraer_dato_string(nodo, SENSOR_REMOTO, datosApp->termostato.sensor_remoto);
	   ESP_LOGE(TAG, ""TRAZAR"despues: %d\n", INFOTRAZA, esp_get_free_heap_size());
	   ESP_LOGW(TAG, ""TRAZAR" Se selecciona el sensor remoto a :%s", INFOTRAZA, datosApp->termostato.sensor_remoto);
   }
   datosApp->termostato.master = master;
   salvar_configuracion_general(datosApp);
   codigoRespuesta(respuesta, RESP_OK);

	return ESP_OK;
}






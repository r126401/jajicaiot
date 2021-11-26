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
#include "mqtt.h"
#include "conexiones.h"
#include "funciones_usuario.h"
#include "interfaz_usuario.h"
#include "configuracion_usuario.h"
#include "dht22.h"
#include "configuracion.h"
#include "alarmas.h"


//static ETSTimer temporizadorTermometro;
ETSTimer temporizador_lectura_remota;

static const char *TAG = "IOTCRONOTEMP";
xQueueHandle cola_gpio = NULL;

#define GPIO_OUTPUT_PIN_SEL  ((1ULL<<CONFIG_GPIO_PIN_RELE) | (1ULL<<CONFIG_GPIO_PIN_LED)| (1ULL<<CONFIG_GPIO_PIN_DHT))
#define GPIO_INPUT_PIN_SEL  ((1ULL<<CONFIG_GPIO_PIN_BOTON) | (1ULL<<CONFIG_GPIO_PIN_RELE))

#define NUM_REPETICIONES    3
#define TIMEOUT_LECTURA_REMOTA 5000 //ms





void consultarEstadoAplicacion(DATOS_APLICACION *datosApp, cJSON *respuesta) {

	ESP_LOGI(TAG, ""TRAZAR"Consultamos el estado de la aplicacion", INFOTRAZA);
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


    //appUser_parametrosAplicacion(datosApp, respuesta);
    escribir_programa_actual(datosApp, respuesta);
    codigoRespuesta(respuesta,RESP_OK);



}


void respuesta_actuacion_rele_remoto(DATOS_APLICACION *datosApp, cJSON *respuesta) {

    cJSON_AddNumberToObject(respuesta, DLG_COD_RESPUESTA, DLG_OK_CODE);
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
           datosApp->datosGenerales->estadoApp = NORMAL_AUTOMAN;
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
            modificarUmbralTemperatura(peticion, datosApp, respuesta);
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

void pulsacion(DATOS_APLICACION *datosApp) {

    static ETSTimer repeticion;
    static uint8_t rep=0;
    cJSON *informe;




    ESP_LOGI(TAG, ""TRAZAR"RUTINA QUE TRATA LAS INTERRUPCIONES", INFOTRAZA);
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
            	publicar_mensaje_json(datosApp, informe, NULL);

            } else {
            	ESP_LOGE(TAG, ""TRAZAR"MENSAJE DE PULSACION CORTA INVALIDO", INFOTRAZA);
            }



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

enum TIPO_ACCION_TERMOSTATO calcular_accion_termostato(DATOS_APLICACION *datosApp, enum ESTADO_RELE *accion) {

    // El termostato esta apagado y la temperatura actual es menor o igual que la umbral.
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

/*
esp_err_t tomar_lectura_dht(struct DATOS_APLICACION *datosApp) {

    int res = -1;
    uint8_t i;


    for(i=0; i < datosApp->termostato.reintentosLectura; i++) {

    	if (((res = dht_read_float_data(DHT_TYPE_AM2301, CONFIG_GPIO_PIN_DHT, &datosApp->termostato.humedad, &datosApp->termostato.tempActual)) >= 0) &&
    			((datosApp->termostato.humedad != 0) && (datosApp->termostato.tempActual != 0))){
        //if ((res = readDHT(lectura)) >= 0) {
    		ESP_LOGI(TAG, ""TRAZAR" TEMPERATURA: %lf, humedad %lf", INFOTRAZA, datosApp->termostato.tempActual, datosApp->termostato.humedad);
            datosApp->termostato.tempActual = datosApp->termostato.tempActual + datosApp->termostato.calibrado;
            return ESP_OK;
            break;
        } else {

        	ESP_LOGE(TAG, ""TRAZAR" LECTURA INCORRECTA, REINTENTO EN %d SEGUNDOS", INFOTRAZA,datosApp->termostato.intervaloReintentos);
            vTaskDelay(datosApp->termostato.intervaloReintentos * 1000 / portTICK_RATE_MS);
        }
    }
	ESP_LOGE(TAG, ""TRAZAR" AGOTADOS TODOS LOS %d REINTENTOS", INFOTRAZA, datosApp->termostato.reintentosLectura);
	return ESP_FAIL;

}


*/
/*
esp_err_t lectura_temperatura(DATOS_APLICACION *datosApp) {

    enum ESTADO_RELE accion;
    cJSON* informe = NULL;
    static float lecturaAnterior = -1000;
    //DATOS_APLICACION *datosApp = (DATOS_APLICACION*) parametros;

    if (tomar_lectura_dht(datosApp) == ESP_OK) {
        // ejecutamos logica de lectura
    	ESP_LOGI(TAG, ""TRAZAR" CALCULANDO ACCION TERMOMETRO. Estadoapp: %d", INFOTRAZA, datosApp->datosGenerales->estadoApp);
        if (calcular_accion_termostato(datosApp, &accion) == ACCIONAR_TERMOSTATO) {
            operacion_rele(datosApp, TEMPORIZADA, accion);
            informe = appuser_generar_informe_espontaneo(datosApp, CAMBIO_TEMPERATURA, NULL);
            publicar_mensaje_json(datosApp, informe);
            return ESP_OK;
        } else {
            //Si la temperatura ha variado desde la ultima medida lo notificamos.
            printf("lecturaTemperatura--> no hay que modificar el rele del termostato\n");
            if(lecturaAnterior != datosApp->termostato.tempActual) {
                informe = appuser_generar_informe_espontaneo(datosApp,CAMBIO_TEMPERATURA,NULL );
                publicar_mensaje_json(datosApp, informe);
            }
            lecturaAnterior = datosApp->termostato.tempActual;
            return ESP_OK;
        }


    } else {
        printf("bucleLecturaTemperatura--> Error en la lectura de temperatura\n");
    }
    return ESP_FAIL;



}
*/
/*
void ejecutar_lectura_termometro(DATOS_APLICACION *datosApp) {

    ets_timer_disarm(&temporizadorTermometro);
    ets_timer_setfn(&temporizadorTermometro, (void*) lectura_temperatura, datosApp);
    ets_timer_arm(&temporizadorTermometro, (datosApp->termostato.intervaloLectura)*1000, 1);
}
*/

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

    	leer_temperatura(datosApp);
	    if (((accion_termostato = calcular_accion_termostato(datosApp, &accion_rele)) == ACCIONAR_TERMOSTATO)) {
	    	operacion_rele(datosApp, TEMPORIZADA, accion_rele);
	    }

	    if ((accion_termostato == ACCIONAR_TERMOSTATO) || (lecturaAnterior != datosApp->termostato.tempActual)) {
            informe = appuser_generar_informe_espontaneo(datosApp, CAMBIO_TEMPERATURA, NULL);
            publicar_mensaje_json(datosApp, informe, NULL);
	    }
	    lecturaAnterior = datosApp->termostato.tempActual;
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



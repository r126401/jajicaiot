/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */


#include "user_config.h"
#include "gpio.h"
#include "iotCronoTemp.h"
#include "cJSON.h"
#include "dialogoJson.h"
#include "dht22.h"




// macros para que funcionen las interrupciones.
#define ETS_GPIO_INTR_ENABLE() _xt_isr_unmask ((1 << ETS_GPIO_INUM))
#define ETS_GPIO_INTR_DISABLE() _xt_isr_mask(1 << ETS_GPIO_INUM) 
extern xTaskHandle manejadorBeat;
extern xTaskHandle manejadorMqtt;
static os_timer_t temporizadorTermometro;
ALARMAS_APLICACION alarmasAplicacion;


void appUser_ConfiguracionDefecto(DATOS_APLICACION *datosApp) {
    //Escribe aqui el codigo de inicializacion por defecto de la aplicacion.
    datosApp->reintentosLectura = 5;
    datosApp->intervaloReintentos = 3;
    datosApp->margenTemperatura = 0.5;
    datosApp->intervaloLectura = 10;
    datosApp->tempUmbral = 21.5;
    datosApp->tempUmbralDefecto = 21;
    datosApp->calibrado = 0;
    printf ("appUser_ConfiguracionDefecto--> cargada la configuracion del termostato");

    
}

/**
 *  Calcula la accion del termostato segun la temperatura actual y la umbral
 * @param datosApp
 * @param accion es un parametro de vuelta que indica que hacer con el rele
 * @return true si hay cambio de estado.
 */
bool calcularAccionTermostato(DATOS_APLICACION *datosApp, enum ESTADO_RELE *accion) {
    
    // El termostato esta apagado y la temperatura actual es menor o igual que la umbral.
    if (GPIO_INPUT_GET(GPIO_RELE) == OFF) {
               if (datosApp->tempActual <= (datosApp->tempUmbral - datosApp->margenTemperatura)) {
                   printf("tempMedida: %d.%d, tempUmbral: %d.%d. El rele estaba apagado y hay que encenderlo\n", 
                            (int) datosApp->tempActual, parteDecimal(datosApp->tempActual), (int)  datosApp->tempUmbral, parteDecimal(datosApp->tempUmbral));
                   *accion = ON;
                   return true; // Se notifica el cambio de estado
                  
                   
               } else {
                   printf ("tempMedida: %d.%d, tempUmbral: %d.%d. El rele estaba apagado y debe seguir apagado\n", 
                           (int) datosApp->tempActual, parteDecimal(datosApp->tempActual), (int)  datosApp->tempUmbral, parteDecimal(datosApp->tempUmbral));
                   //operarRele(datosApp, OFF);
                   *accion = OFF;
                   return false; // no se notifica el cambio de estado
                   
               }
               //El termostato esta encendido y la temperatura actual es mayor que la umbral.
           } else {
               if (datosApp->tempActual >= (datosApp->tempUmbral + datosApp->margenTemperatura) ) {
                   printf("tempMedida: %d.%d, tempUmbral: %d.%d. El rele estaba encendido y hay que apagarlo\n",
                           (int) datosApp->tempActual, parteDecimal(datosApp->tempActual), (int)  datosApp->tempUmbral, parteDecimal(datosApp->tempUmbral));
                   *accion = OFF;
                   return true; // Se notirica el cambio de estado.
               } else {
                   printf("tempMedida: %d.%d, tempUmbral: %d.%d. El rele estaba encendido y debe estar encendido\n",
                           (int) datosApp->tempActual, parteDecimal(datosApp->tempActual), (int)  datosApp->tempUmbral, parteDecimal(datosApp->tempUmbral));
                   *accion = ON;
                   //datosApp->rele = ENCENDIDO;
                   //operarRele(datosApp, ON);
                   return false; // no se notifica el cambio de estado.

               }
           }

}


bool tomarLecturaDht(struct DATOS_APLICACION *datosApp) {
    
    bool res;
    uint8 i;
    uint8 lectura[5];
    DHTType sensor;
    
    sensor = DHT22;
    
    for(i=0; i < datosApp->reintentosLectura; i++) {
        
        if (res = DHTRead(lectura) == true) {
            printf("tomarLecturaDht-->Lectura correcta\n");
            break;
        } else {
            vTaskDelay(datosApp->intervaloReintentos * 1000 / portTICK_RATE_MS); 
        }
        
    }
    if(res == false) {
        printf("Agotados los reintentos: %d", i);
        return false;
    } else {
        datosApp->tempActual = scale_temperature(sensor, lectura);
        // aplicamos corrector de caja
        datosApp->tempActual = datosApp->tempActual + datosApp->calibrado;
        datosApp->humedad = scale_humidity(sensor, lectura);
        return true;
    }
    
    
    
    
    
}


void lecturaTemperatura(DATOS_APLICACION *datosApp) {
    
    enum ESTADO_RELE accion;
    cJSON* root = NULL;
    static float lecturaAnterior = -1000;
    bool accionTermostato;
    
    if (tomarLecturaDht(datosApp) == true) {
        // ejecutamos logica de lectura
        printf("lecturaTemperatura-->Calculando accion termometro. EstadoApp: %d\n", datosApp->estadoApp);
        printf("lecturaTemperatura-->intervalo de lectura %d\n", datosApp->intervaloLectura);
            
        if (calcularAccionTermostato(datosApp, &accion) == true) {
            operacionRele(datosApp, POR_TEMPERATURA, accion);
            root = generarReporte(datosApp,CAMBIO_TEMPERATURA, NULL );
            //root = actuarRele(root, datosApp, POR_TEMPERATURA, accion);
            notificarAccion(root, datosApp);
            return;
        } else {
            //Si la temperatura ha variado desde la ultima medida lo notificamos.
            printf("lecturaTemperatura--> no hay que modificar el rele del termostato\n");
            if(lecturaAnterior != datosApp->tempActual) {
                root = generarReporte(datosApp,CAMBIO_TEMPERATURA,NULL );
                //root = generarRespuesta(root, POR_TEMPERATURA, datosApp, CAMBIO_TEMPERATURA);
                notificarAccion(root, datosApp);
            }
            lecturaAnterior = datosApp->tempActual;           
        }

        
    } else {
        printf("bucleLecturaTemperatura--> Error en la lectura de temperatura\n");
    }
    
    
}







void parapadeoLed() {
    
    if (GPIO_INPUT_GET(GPIO_LED) == 0) {
        GPIO_OUTPUT_SET(GPIO_ID_PIN(GPIO_LED), 1);
    } else {
        GPIO_OUTPUT_SET(GPIO_ID_PIN(GPIO_LED), 0);
    }
    
    
    
}


void appUser_notificarWifiConectando() {
    
    
    os_timer_disarm(&notificacionWifi);
    os_timer_setfn(&notificacionWifi, (os_timer_func_t*)parapadeoLed,NULL);
    os_timer_arm(&notificacionWifi, PARPADEO_WIFI_CONECTANDO, 1);


}

void appUser_notificarWifiConectado() {

    
    os_timer_disarm(&notificacionWifi);
    os_timer_setfn(&notificacionWifi, (os_timer_func_t*)parapadeoLed,NULL);
    os_timer_arm(&notificacionWifi, PARPADEO_WIFI_CONECTADO, 1);


    
}

void appUser_notificarWifiDesconectado() {

    os_timer_disarm(&notificacionWifi);
    GPIO_OUTPUT_SET(GPIO_ID_PIN(GPIO_LED), 0);
}


void appUser_notificarBrokerDesconectado() {

    os_timer_disarm(&notificacionWifi);
    os_timer_setfn(&notificacionWifi, (os_timer_func_t*)parapadeoLed,NULL);
    os_timer_arm(&notificacionWifi, PARPADEO_BROKER_DESCONECTADO, 1);
    
}

void appUser_notificarBrokerConectado() {
 
    printf("appUser_notificarBrokerConectado--> conectado!\n");
    os_timer_disarm(&notificacionWifi);
    GPIO_OUTPUT_SET(GPIO_ID_PIN(GPIO_LED), 1);
    
    
}
/*
cJSON* actuarRele(cJSON* root, DATOS_APLICACION *datosApp, enum TIPO_ACTUACION_RELE tipo, int releTemporizado) {
    
    cJSON *nodo = NULL;
    cJSON *campo;
    //cJSON *nodoRespuesta;
    //uint8 nParametros = 0;
    bool exito = true;
    int accion;

    if (root == NULL) {
       root = cJSON_CreateObject(); 
       printf("actuarRele--> Se crea json porque es nulo\n");
       
    }

    
   nodo = cJSON_GetObjectItem(root, APP_COMAND_RELE);
   
   if(nodo == NULL) {
       nodo = root;
   }
   

   campo = cJSON_GetObjectItem(nodo, APP_COMAND_OPERACION_RELE);
   if((campo != NULL) && (campo->type == cJSON_Number)) {
       accion = campo->valueint;
   }
   
   switch(tipo) {
       
       case NOTIFICACION_ESTADO:
           root = generarRespuesta(root, tipo, datosApp, CAMBIO_TEMPERATURA);
           break;
           
       case POR_TEMPERATURA:
           GPIO_OUTPUT_SET(GPIO_ID_PIN(GPIO_RELE), releTemporizado);
           root = generarRespuesta(root, tipo, datosApp, CAMBIO_TEMPERATURA);
           break;
       case TEMPORIZADA:
           printf ("actuarRele-->accion temporizada vale %d\n", releTemporizado);
           GPIO_OUTPUT_SET(GPIO_ID_PIN(GPIO_RELE), releTemporizado);
           if (datosApp->estadoApp == NORMAL_AUTO) {
               datosApp->estadoApp = NORMAL_AUTOMAN;
               DBG("activarRele--> EstadoApp pasa a AUTOMAN\n");
           }
           root = generarRespuesta(root, tipo, datosApp, CAMBIO_DE_PROGRAMA);
           break;
       case REMOTA:
           if (accion == 0) {
               //se apaga el rele
               GPIO_OUTPUT_SET(GPIO_ID_PIN(GPIO_RELE), OFF);
               DBG("actuarRele-->rele desactivado\n");

           } else {
               GPIO_OUTPUT_SET(GPIO_ID_PIN(GPIO_RELE), ON);
               DBG("actuarRele-->rele activado\n");
           }  
           root = generarRespuesta(root, tipo, datosApp, ACTUACION_RELE_LOCAL);
           break;
       case MANUAL:
           if (GPIO_INPUT_GET(GPIO_RELE) == 0) {
               GPIO_OUTPUT_SET(GPIO_ID_PIN(GPIO_RELE), 1);
           } else {
               GPIO_OUTPUT_SET(GPIO_ID_PIN(GPIO_RELE), 0);
           }
           datosApp->estadoApp = NORMAL_MANUAL;
           root = generarRespuesta(root, tipo, datosApp, ACTUACION_RELE_LOCAL);
           
           break;

           
           
   }

   DBG("activarRele--> memoria libre:  %d\n", system_get_free_heap_size());
   return root;
 /*  
   switch(tipo) {
       
       case POR_TEMPERATURA:
           GPIO_OUTPUT_SET(GPIO_ID_PIN(GPIO_RELE), releTemporizado);
           printf("actuarRele-->accion por temperatura vale %d\n", releTemporizado);
           root = generarRespuesta(root, tipo, datosApp, CAMBIO_TEMPERATURA);
           return root;
           //notificarAccion(root, datosApp); 
             
           
           break;
           
       case TEMPORIZADA:
           printf ("actuarRele-->accion temporizada vale %d\n", releTemporizado);
           GPIO_OUTPUT_SET(GPIO_ID_PIN(GPIO_RELE), releTemporizado);
           if (datosApp->estadoApp == NORMAL_AUTO) {
               datosApp->estadoApp = NORMAL_AUTOMAN;
               DBG("activarRele--> EstadoApp pasa a AUTOMAN\n");
           }
           root = generarRespuesta(root, tipo, datosApp, CAMBIO_PROGRAMA);
           break;

           
       case REMOTA:
       
           if (accion == 0) {
               //se apaga el rele
               GPIO_OUTPUT_SET(GPIO_ID_PIN(GPIO_RELE), OFF);
               DBG("actuarRele-->rele desactivado\n");

           } else {
               GPIO_OUTPUT_SET(GPIO_ID_PIN(GPIO_RELE), ON);
               DBG("actuarRele-->rele activado\n");
           }
           switch(tipo) {
               case TEMPORIZADA:
                   root = generarRespuesta(root, tipo, datosApp, CAMBIO_DE_PROGRAMA);
                   break;
               case REMOTA:
                   root = generarRespuesta(root, tipo, datosApp, ACTUACION_RELE_REMOTO);
                   break;
               default:
                   root = generarRespuesta(root, tipo, datosApp, ACTUACION_RELE_LOCAL);
           }
            
           root = generarRespuesta(root, tipo, datosApp);
           //notificarAccion(root, datosApp);     
           return root;
           break;
           
       case MANUAL:
           if (GPIO_INPUT_GET(GPIO_RELE) == 0) {
               GPIO_OUTPUT_SET(GPIO_ID_PIN(GPIO_RELE), 1);
           } else {
               GPIO_OUTPUT_SET(GPIO_ID_PIN(GPIO_RELE), 0);
           }
           datosApp->estadoApp = NORMAL_MANUAL;
           root = generarRespuesta(root, tipo, datosApp);
           //root = cJSON_GetObjectItem(root,DLG_RESPUESTA );
           
           return root;

           break;
           
            
    }

   
/*  
   if (tipo == TEMPORIZADA) {
       accion = releTemporizado;
       printf ("actuarRele-->accion temporizada vale %d\n", accion);
   }
   
   
   if ( (tipo == REMOTA) || (tipo == TEMPORIZADA)) {
       
       if (accion == 0) {
           //se apaga el rele
           GPIO_OUTPUT_SET(GPIO_ID_PIN(GPIO_RELE), OFF);
           DBG("actuarRele-->rele desactivado\n");
           
       } else {
           GPIO_OUTPUT_SET(GPIO_ID_PIN(GPIO_RELE), ON);
           DBG("actuarRele-->rele activado\n");
       }
   } else {
       
   }
   
   if (tipo == REMOTA) {
       if (datosApp->estadoApp == NORMAL_AUTO) {
           datosApp->estadoApp = NORMAL_AUTOMAN;
           DBG("activarRele--> EstadoApp pasa a AUTOMAN\n");
       }
   }
   
   root = generarRespuesta(root, tipo, datosApp);
   DBG("activarRele--> memoria libre:  %d\n", system_get_free_heap_size());
   
   return root;
  */ 
   
    
//}

bool appUser_modificarConfApp(cJSON *root, DATOS_APLICACION *datosApp, cJSON *respuesta) {
    cJSON *nodo;
   cJSON *campo;
   cJSON *nodoRespuesta;
   uint8 i=0;
   char* ipNumber;
   uint8 nModificaciones=0;
   
   nodo = cJSON_GetObjectItem(root, APP_PARAMS);
   if(nodo == NULL) {
       codigoRespuesta(respuesta, RESP_NOK);
       return FALSE;
   }
   
   campo = cJSON_GetObjectItem(nodo, MARGEN_TEMPERATURA);
   if((campo != NULL) && (campo->type == cJSON_Number)) {
       datosApp->margenTemperatura = campo->valuedouble;
       nModificaciones++;
   }
   
   
   
   campo = cJSON_GetObjectItem(nodo, INTERVALO_LECTURA);
   if((campo != NULL) && (campo->type == cJSON_Number)) {
       datosApp->intervaloLectura = campo->valueint;
       nModificaciones++;
       printf("intervalo de lectura modificado a %d\n", datosApp->intervaloLectura);
   }
   campo = cJSON_GetObjectItem(nodo, INTERVALO_REINTENTOS);
   if((campo != NULL) && (campo->type == cJSON_Number)) {
       datosApp->intervaloReintentos = campo->valueint;
       nModificaciones++;
   }
   campo = cJSON_GetObjectItem(nodo, REINTENTOS_LECTURA);
   if((campo != NULL) && (campo->type == cJSON_Number)) {
       datosApp->reintentosLectura = campo->valueint;
       nModificaciones++;
   }
 
   campo = cJSON_GetObjectItem(nodo, CALIBRADO);
   if((campo != NULL) && (campo->type == cJSON_Number)) {
       datosApp->calibrado = campo->valueint;
       nModificaciones++;
   }
   

   
   if (nModificaciones == 0) {
       codigoRespuesta(respuesta, RESP_NOK);
       return FALSE;
   } else {
       grabarConfiguracion(datosApp, respuesta);
       return TRUE;
       
   }
    return root;

    
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
           datosApp->tempUmbral = campo->valuedouble;
           datosApp->estadoApp = NORMAL_AUTOMAN;
           guardarConfiguracion(datosApp, 0);
           cJSON_AddNumberToObject(respuesta, UMBRAL_TEMPERATURA, datosApp->tempUmbral);
           codigoRespuesta(respuesta, RESP_OK);
       } else {
           codigoRespuesta(respuesta, RESP_NOK);
       }
    return TRUE;
}

bool appUser_analizarComandoAplicacion(cJSON *peticion, int nComando, DATOS_APLICACION *datosApp, cJSON *respuesta) {
    

    enum ESTADO_RELE estadoRele;
    switch(nComando) {
        case OPERAR_RELE:
            //cJSON_Delete(root);
            estadoRele = operacionRele(datosApp, MANUAL, INDETERMINADO);
            actuacionReleRemoto(datosApp, respuesta);

            break;

        case STATUS_RELE:
            //estadoRele();
            consultarEstadoAplicacion(datosApp, respuesta);
            //respuesta = generarReporte(datosApp, ESTADO, respuesta);
            break;
            
        case MODIFICAR_UMBRAL:
            modificarUmbralTemperatura(peticion, datosApp, respuesta);
            //root = generarReporte(datosApp, CAMBIO_UMBRAL_TEMPERATURA, root);
            break;
        default:
            respuesta = cabeceraRespuestaComando(datosApp, respuesta);
            comandoDesconocido(datosApp, respuesta);
            
            //comando desconocido
            break;
    }
    
    return TRUE;
    
}

void pulsacion(DATOS_APLICACION *datosApp) {
    
    static os_timer_t repeticion;
    static uint8 rep=0;
    cJSON *root = NULL;
    
    
    if (GPIO_INPUT_GET(GPIO_BOTON) == PULSADO) {
        os_timer_disarm(&repeticion);
        os_timer_setfn(&repeticion, (os_timer_func_t*) pulsacion, datosApp);
        os_timer_arm(&repeticion, 500,0);
        rep++;
        printf("repeticion %d\n", rep);
        
    } else {
        datosApp->botonPulsado = false;
        //*rebote = false;
        //printf("Rebote cancelado, rep = %d\n", rep);
        if (rep > NUM_REPETICIONES) {
            
            printf("pulsacion larga\n");
            if (wifi_station_get_connect_status() == STATION_GOT_IP) {
                system_restart();
            } else {
                //smartconfig
                printf("sin ip, entramos en smartconfig...");
                smartconfig_start(smartconfig_done);

            }
            
        } else {
            printf("pulsacion corta\n");
            datosApp->estadoApp = NORMAL_MANUAL;
            operacionRele(datosApp, MANUAL, INDETERMINADO);
            root = generarReporte(datosApp, ACTUACION_RELE_LOCAL, NULL);
            notificarAccion(root, datosApp);
        }
        
        rep=0;
    }
}


void tratarInterrupcionesPulsador(DATOS_APLICACION *datosApp) {
    

    uint32 gpio_status = GPIO_REG_READ(GPIO_STATUS_ADDRESS);
    static os_timer_t temporizador;
    static bool rebote = false;
    //printf("han pulsado un boton....\n");
    
    
    
    if (gpio_status & (BIT(GPIO_BOTON))) {
        
        if (datosApp->botonPulsado == false) {
            datosApp->botonPulsado = true;
            os_timer_disarm(&notificacionWifi);
            os_timer_disarm(&temporizador);
            os_timer_setfn(&temporizador, (os_timer_func_t*) pulsacion, datosApp);
            os_timer_arm(&temporizador, 250,0);        
                    
        }
    }
    

    

 
    
    GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, gpio_status );
}


void appUser_inicializarAplicacion(DATOS_APLICACION *datosApp) {
    
    datosApp->botonPulsado = false;
    // LED
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTCK_U, FUNC_GPIO13);
    // RELE
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U, FUNC_GPIO12);
    GPIO_OUTPUT_SET(GPIO_ID_PIN(GPIO_RELE), 0);
    GPIO_OUTPUT_SET(GPIO_ID_PIN(GPIO_LED), 0);

    
    // BOTON
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0);
    GPIO_OUTPUT_SET(GPIO_ID_PIN(GPIO_BOTON), NO_PULSADO);

    ETS_GPIO_INTR_DISABLE();
    
    //Rregistro la rutina que lo va a tratar
    gpio_intr_handler_register(tratarInterrupcionesPulsador, datosApp);
    
    
    //Boton +
    gpio_pin_intr_state_set (GPIO_ID_PIN(GPIO_BOTON), GPIO_PIN_INTR_NEGEDGE);
    ETS_GPIO_INTR_ENABLE();
    



    
}

void notificarAccion(cJSON *root, DATOS_APLICACION *datosApp) {
    
    char* respuesta;
    char* topic;
    
    respuesta = cJSON_Print(root);
    if (root == NULL) {
        printf("error al enviar el json\n");
    } else {
        topic = zalloc(75*sizeof(char));
        strcpy(topic, datosApp->parametrosMqtt.prefix);
        strcat(topic, datosApp->parametrosMqtt.publish);
        sendMqttMessage(topic, respuesta);
        free(respuesta);
        free(topic);
        cJSON_Delete(root);
        root = NULL;
    }
    
    printf("notificarAccion-->Notificado el evento del rele\n");

    
    
}




cJSON* generarRespuesta(cJSON *root, enum TIPO_ACTUACION_RELE tipo, DATOS_APLICACION *datosApp, enum TIPO_INFORME tipoInforme) {
  
    
    /*
    cJSON *nodoRespuesta = NULL;
    
        if (root == NULL) {
       root = cJSON_CreateObject(); 
       printf("generarRespuesta--> Se crea json porque es nulo\n");
       
    }

    
    cJSON_AddItemToObject(root, DLG_RESPUESTA, nodoRespuesta = cJSON_CreateObject());

    
    
    if (tipo == REMOTA) {
        cJSON_AddNumberToObject(nodoRespuesta, DLG_COD_RESPUESTA, DLG_OK_CODE);
    } else {
        nodoRespuesta = ponerElementosRespuesta(nodoRespuesta, datosApp);
        if((tipoInforme != ACTUACION_RELE_REMOTO) && (tipoInforme != COMANDO_APLICACION) ) {
            cJSON_AddNumberToObject(nodoRespuesta, TIPO_REPORT, tipoInforme);
        }      
    }
    printf("(1) generarRespuesta: %s\n", cJSON_Print(root));
    
    cJSON_AddNumberToObject(nodoRespuesta, APP_COMAND_ESTADO_RELE, GPIO_INPUT_GET(GPIO_RELE));
    cJSON_AddNumberToObject(nodoRespuesta, DEVICE_STATE, datosApp->estadoApp);
    cJSON_AddNumberToObject(nodoRespuesta, DEVICE, datosApp->tipoDispositivo);
    cJSON_AddNumberToObject(nodoRespuesta, TEMPERATURA, datosApp->tempActual);
    cJSON_AddNumberToObject(nodoRespuesta, PROGRAMMER_STATE, datosApp->estadoProgramacion);
    cJSON_AddNumberToObject(nodoRespuesta, HUMEDAD, datosApp->humedad);
    cJSON_AddNumberToObject(nodoRespuesta, UMBRAL_TEMPERATURA, datosApp->tempUmbral);
    if (tipo == NOTIFICACION_ESTADO) {
        printf("(3) generarRespuesta: %s\n", cJSON_Print(root));
        cJSON_AddNumberToObject(nodoRespuesta, REINTENTOS_LECTURA, datosApp->reintentosLectura);
        cJSON_AddNumberToObject(nodoRespuesta, INTERVALO_REINTENTOS, datosApp->intervaloReintentos);
        cJSON_AddNumberToObject(nodoRespuesta, INTERVALO_LECTURA, datosApp->intervaloLectura);
        cJSON_AddNumberToObject(nodoRespuesta, MARGEN_TEMPERATURA, datosApp->margenTemperatura);
        cJSON_AddNumberToObject(nodoRespuesta, CALIBRADO, datosApp->calibrado);
    }
    
    printf("(4) generarRespuesta: %s\n", cJSON_Print(root));
    nodoRespuesta = escribirProgramaActual(datosApp, nodoRespuesta);
    printf("(5) generarRespuesta: %s\n", cJSON_Print(root));
    /*
    if (tipo != MANUAL) {
        nodoRespuesta = escribirProgramaActual(datosApp, nodoRespuesta);
        
    }
      
    switch (tipo) {
        case MANUAL:
            
            cJSON_AddNumberToObject(nodoRespuesta, APP_COMAND_ESTADO_RELE, GPIO_INPUT_GET(GPIO_RELE));
            cJSON_AddNumberToObject(nodoRespuesta, PROGRAMMER_STATE, datosApp->estadoProgramacion);
            cJSON_AddNumberToObject(nodoRespuesta, DEVICE_STATE, datosApp->estadoApp);
            cJSON_AddNumberToObject(nodoRespuesta, DEVICE, datosApp->tipoDispositivo);
            cJSON_AddNumberToObject(nodoRespuesta, TEMPERATURA, datosApp->tempActual);
            cJSON_AddNumberToObject(nodoRespuesta, HUMEDAD, datosApp->humedad);
            cJSON_AddNumberToObject(nodoRespuesta, UMBRAL_TEMPERATURA, datosApp->tempUmbral);

            
            
            break;
        case REMOTA:
      

               cJSON_AddNumberToObject(nodoRespuesta, DLG_COD_RESPUESTA, DLG_OK_CODE);
               
               cJSON_AddNumberToObject(nodoRespuesta, APP_COMAND_ESTADO_RELE, GPIO_INPUT_GET(GPIO_RELE));
               cJSON_AddNumberToObject(nodoRespuesta, DEVICE_STATE, datosApp->estadoApp);
               cJSON_AddNumberToObject(nodoRespuesta, PROGRAMMER_STATE, datosApp->estadoProgramacion);          
               cJSON_AddNumberToObject(nodoRespuesta, DEVICE, datosApp->tipoDispositivo);
               cJSON_AddNumberToObject(nodoRespuesta, TEMPERATURA, datosApp->tempActual);
               cJSON_AddNumberToObject(nodoRespuesta, HUMEDAD, datosApp->humedad);
               cJSON_AddNumberToObject(nodoRespuesta, UMBRAL_TEMPERATURA, datosApp->tempUmbral);
               
               nodoRespuesta = escribirProgramaActual(datosApp, nodoRespuesta);
           break;
           
        case TEMPORIZADA:
        case POR_TEMPERATURA:
        case CAMBIO_TEMPERATURA:
               //cJSON_AddNumberToObject(nodoRespuesta, DLG_COD_RESPUESTA, DLG_OK_CODE);
            
               cJSON_AddNumberToObject(nodoRespuesta, PROGRAMMER_STATE, datosApp->estadoProgramacion);
               cJSON_AddNumberToObject(nodoRespuesta, APP_COMAND_ESTADO_RELE, GPIO_INPUT_GET(GPIO_RELE));               
               cJSON_AddNumberToObject(nodoRespuesta, DEVICE_STATE, datosApp->estadoApp);
               cJSON_AddNumberToObject(nodoRespuesta, DEVICE, datosApp->tipoDispositivo);
               cJSON_AddNumberToObject(nodoRespuesta, TEMPERATURA, datosApp->tempActual);
               cJSON_AddNumberToObject(nodoRespuesta, HUMEDAD, datosApp->humedad);
               cJSON_AddNumberToObject(nodoRespuesta, UMBRAL_TEMPERATURA, datosApp->tempUmbral);
             
               nodoRespuesta = escribirProgramaActual(datosApp, nodoRespuesta);
           break;
            

    }
    
    
    return root;
    */
        cJSON *nodoRespuesta = NULL;
    
    cJSON_AddItemToObject(root, DLG_RESPUESTA, nodoRespuesta = cJSON_CreateObject());

    
    nodoRespuesta = ponerElementosRespuesta(nodoRespuesta, datosApp);
    if((tipoInforme != ACTUACION_RELE_REMOTO) && (tipoInforme != COMANDO_APLICACION) ) {
        cJSON_AddNumberToObject(nodoRespuesta, TIPO_REPORT, tipoInforme);
    }
    
    switch (tipo) {
        case MANUAL:
            cJSON_AddNumberToObject(nodoRespuesta, APP_COMAND_ESTADO_RELE, GPIO_INPUT_GET(GPIO_RELE));
            cJSON_AddNumberToObject(nodoRespuesta, PROGRAMMER_STATE, datosApp->estadoProgramacion);
            cJSON_AddNumberToObject(nodoRespuesta, DEVICE_STATE, datosApp->estadoApp);
            cJSON_AddNumberToObject(nodoRespuesta, DEVICE, datosApp->tipoDispositivo);
            
            
            break;
        case REMOTA:
        case COMANDO_APLICACION:

               cJSON_AddNumberToObject(nodoRespuesta, DLG_COD_RESPUESTA, DLG_OK_CODE);
               cJSON_AddNumberToObject(nodoRespuesta, APP_COMAND_ESTADO_RELE, GPIO_INPUT_GET(GPIO_RELE));
               cJSON_AddNumberToObject(nodoRespuesta, DEVICE_STATE, datosApp->estadoApp);
               cJSON_AddNumberToObject(nodoRespuesta, PROGRAMMER_STATE, datosApp->estadoProgramacion);          
               cJSON_AddNumberToObject(nodoRespuesta, DEVICE, datosApp->tipoDispositivo);
                cJSON_AddNumberToObject(nodoRespuesta, APP_COMAND_ESTADO_RELE, GPIO_INPUT_GET(GPIO_RELE));
                cJSON_AddNumberToObject(nodoRespuesta, DEVICE_STATE, datosApp->estadoApp);
                cJSON_AddNumberToObject(nodoRespuesta, DEVICE, datosApp->tipoDispositivo);
                cJSON_AddNumberToObject(nodoRespuesta, TEMPERATURA, datosApp->tempActual);
                cJSON_AddNumberToObject(nodoRespuesta, PROGRAMMER_STATE, datosApp->estadoProgramacion);
                cJSON_AddNumberToObject(nodoRespuesta, HUMEDAD, datosApp->humedad);
                cJSON_AddNumberToObject(nodoRespuesta, UMBRAL_TEMPERATURA, datosApp->tempUmbral);
                if (tipo == NOTIFICACION_ESTADO) {
                    printf("(3) generarRespuesta: %s\n", cJSON_Print(root));
                    cJSON_AddNumberToObject(nodoRespuesta, REINTENTOS_LECTURA, datosApp->reintentosLectura);
                    cJSON_AddNumberToObject(nodoRespuesta, INTERVALO_REINTENTOS, datosApp->intervaloReintentos);
                    cJSON_AddNumberToObject(nodoRespuesta, INTERVALO_LECTURA, datosApp->intervaloLectura);
                    cJSON_AddNumberToObject(nodoRespuesta, MARGEN_TEMPERATURA, datosApp->margenTemperatura);
                    cJSON_AddNumberToObject(nodoRespuesta, CALIBRADO, datosApp->calibrado);
                }
               
               nodoRespuesta = escribirProgramaActual(datosApp, nodoRespuesta);
           break;
           
        case TEMPORIZADA:
               //cJSON_AddNumberToObject(nodoRespuesta, DLG_COD_RESPUESTA, DLG_OK_CODE);
               cJSON_AddNumberToObject(nodoRespuesta, PROGRAMMER_STATE, datosApp->estadoProgramacion);
               cJSON_AddNumberToObject(nodoRespuesta, APP_COMAND_ESTADO_RELE, GPIO_INPUT_GET(GPIO_RELE));               
               cJSON_AddNumberToObject(nodoRespuesta, DEVICE_STATE, datosApp->estadoApp);
               cJSON_AddNumberToObject(nodoRespuesta, DEVICE, datosApp->tipoDispositivo);
               nodoRespuesta = escribirProgramaActual(datosApp, nodoRespuesta);
           break;
            

    }
    
    
    return root;

    
}

void appUser_notificarArranque(DATOS_APLICACION *datosApp, int tipoArranque) {
    
    cJSON *respuesta = NULL;
    lecturaTemperatura(datosApp);
    respuesta = generarReporte(datosApp, ARRANQUE_APLICACION, NULL);
    notificarAccion(respuesta, datosApp);
    
}

void appUser_notificarSmartconfig() {
    
    
    printf("notificacion smartconfig\n");
    os_timer_disarm(&noficacionSmartConfig);
    os_timer_setfn(&noficacionSmartConfig, (os_timer_func_t*)parapadeoLed,NULL);
    os_timer_arm(&noficacionSmartConfig, PARPADEO_SMARTCONFIG, 1);

    
}

void appUser_notificarSmartconfigStop() {
    os_timer_disarm(&noficacionSmartConfig);
}


/**
 * funcion de usuario para apagar el interruptor cuando vence el temporizador de duracion
 * @param datosApp
 */
void appUser_ejecucionAccionTemporizada(void *datosApp) {
    
    cJSON * root = NULL;
    printf("ejecucionAccionTemporizada-->Fin de la accion de la duracion\n");
    //GPIO_OUTPUT_SET(GPIO_ID_PIN(GPIO_RELE), OFF);
    operacionRele(datosApp, TEMPORIZADA, OFF);
    root = generarReporte(datosApp, RELE_TEMPORIZADO, NULL);
    
    //root = actuarRele(datosApp, TEMPORIZADA,0);
    notificarAccion(root, datosApp);
    printf("ejecucionAccionTemporizada-->Rele desactivado\n");
    
}



int appUser_ejecutarAccionPrograma(DATOS_APLICACION *datosApp, int programaActual) {
    
    cJSON *root = NULL;
    char *respuesta;
    char *topic;
    enum ESTADO_RELE accion;

    printf("appUser_ejecutarAccionPrograma-->ejecutando accion programa\n");
    if (datosApp->estadoApp != NORMAL_MANUAL) {
       //Ejecutamos la accion que proceda dentro de la aplicacion
        datosApp->estadoApp = NORMAL_AUTO;
        datosApp->tempUmbral = datosApp->programacion[programaActual].temperatura;
        calcularAccionTermostato(datosApp, &accion);
        //operacionRele(datosApp, TEMPORIZADA, datosApp->programacion[programaActual].accion);
        root = generarReporte(datosApp, CAMBIO_DE_PROGRAMA, NULL);
        //root = actuarRele(datosApp, TEMPORIZADA, datosApp->programacion[programaActual].accion); 
        notificarAccion(root, datosApp);
        
        //root = actuarRele(root, datosApp, TEMPORIZADA, accion);
        //notificarAccion(root, datosApp); 
        
    } else {
        printf("appUser_ejecutarAccionPrograma--> dispositivo en modo manual, nos se hace nada");
    }
        
    
     
    
    return 0;

}


int appUser_ejecutarAccionProgramaDefecto(DATOS_APLICACION *datosApp) {
    
    printf("appUser_ejecutarAccionProgramaDefecto--> Accion por defecto al no encontrarse programacion valida\n");
    printf("appUser_ejecutarAccionProgramaDefecto-->por defecto rele apagado\n");
    GPIO_OUTPUT_SET(GPIO_ID_PIN(GPIO_RELE), OFF);
    
}

void appUser_accionesOta(DATOS_APLICACION *datosApp) {
    //vTaskDelete(manejadorBeat);
}

void appUser_notificarUpgradeOta() {

    printf("notificacion smartconfig\n");
    os_timer_disarm(&noficacionUpgradeOta);
    os_timer_setfn(&noficacionUpgradeOta, (os_timer_func_t*)parapadeoLed,NULL);
    os_timer_arm(&noficacionUpgradeOta, PARPADEO_UPGRADE_OTA, 1);


    
}

void ejecutarLecturaTermometro(DATOS_APLICACION *datosApp) {
    
    os_timer_disarm(&temporizadorTermometro);
    os_timer_setfn(&temporizadorTermometro, (os_timer_func_t*) lecturaTemperatura, datosApp);
    os_timer_arm(&temporizadorTermometro, (datosApp->intervaloLectura)*1000, 1);
}

     
void appUser_parametrosAplicacion(DATOS_APLICACION *datosApp, cJSON *respuesta) {
    

    cJSON_AddNumberToObject(respuesta, MARGEN_TEMPERATURA, datosApp->margenTemperatura);
    cJSON_AddNumberToObject(respuesta, INTERVALO_LECTURA, datosApp->intervaloLectura);
    cJSON_AddNumberToObject(respuesta, INTERVALO_REINTENTOS, datosApp->intervaloReintentos);
    cJSON_AddNumberToObject(respuesta, REINTENTOS_LECTURA, datosApp->reintentosLectura);
    cJSON_AddNumberToObject(respuesta, CALIBRADO, datosApp->calibrado);

}

void evaluarAccion(DATOS_APLICACION *datosApp, enum ESTADO_RELE *accion) {
    

    
    
    
}

bool appUser_modificarProgramacionDispositivo(DATOS_APLICACION *datosApp, int nPrograma, cJSON *peticion, cJSON *respuesta) {
    
   

    cJSON* nodo = NULL;

    
    nodo = cJSON_GetObjectItem(peticion, PROGRAM);
    if (nodo != NULL) {
        datosApp->programacion[nPrograma].temperatura = extraerDatoDouble(nodo, UMBRAL_TEMPERATURA);
        cJSON_AddNumberToObject(respuesta, UMBRAL_TEMPERATURA, datosApp->programacion[nPrograma].temperatura);
        return TRUE;
    } else {
        return FALSE;
    }
}


cJSON* generarReporte(DATOS_APLICACION *datosApp, enum TIPO_INFORME tipoInforme, cJSON *comandoOriginal) {
    

    cJSON *respuesta = NULL;
    

    respuesta = cabeceraEspontaneo(datosApp, tipoInforme);
    switch(tipoInforme) {
        case ARRANQUE_APLICACION:
            cJSON_AddNumberToObject(respuesta, APP_COMAND_ESTADO_RELE, GPIO_INPUT_GET(GPIO_RELE));
            cJSON_AddNumberToObject(respuesta, PROGRAMMER_STATE, datosApp->estadoProgramacion);
            cJSON_AddNumberToObject(respuesta, DEVICE_STATE, datosApp->estadoApp);
            cJSON_AddNumberToObject(respuesta, TEMPERATURA, datosApp->tempActual);
            cJSON_AddNumberToObject(respuesta, HUMEDAD, datosApp->humedad);
            cJSON_AddNumberToObject(respuesta, UMBRAL_TEMPERATURA, datosApp->tempUmbral);
            escribirProgramaActual(datosApp, respuesta);
            break;
        case ACTUACION_RELE_LOCAL:
            cJSON_AddNumberToObject(respuesta, APP_COMAND_ESTADO_RELE, GPIO_INPUT_GET(GPIO_RELE));
            cJSON_AddNumberToObject(respuesta, PROGRAMMER_STATE, datosApp->estadoProgramacion);
            cJSON_AddNumberToObject(respuesta, DEVICE_STATE, datosApp->estadoApp);
            cJSON_AddNumberToObject(respuesta, TEMPERATURA, datosApp->tempActual);
            cJSON_AddNumberToObject(respuesta, HUMEDAD, datosApp->humedad);
            cJSON_AddNumberToObject(respuesta, UMBRAL_TEMPERATURA, datosApp->tempUmbral);            
            escribirProgramaActual(datosApp, respuesta);

            break;
        case CAMBIO_DE_PROGRAMA:
            cJSON_AddNumberToObject(respuesta, PROGRAMMER_STATE, datosApp->estadoProgramacion);
            cJSON_AddNumberToObject(respuesta, APP_COMAND_ESTADO_RELE, GPIO_INPUT_GET(GPIO_RELE));               
            cJSON_AddNumberToObject(respuesta, DEVICE_STATE, datosApp->estadoApp);
            cJSON_AddNumberToObject(respuesta, TEMPERATURA, datosApp->tempActual);
            cJSON_AddNumberToObject(respuesta, HUMEDAD, datosApp->humedad);
            cJSON_AddNumberToObject(respuesta, UMBRAL_TEMPERATURA, datosApp->tempUmbral);
            escribirProgramaActual(datosApp, respuesta);
            break;
        case CAMBIO_TEMPERATURA:
            cJSON_AddNumberToObject(respuesta, APP_COMAND_ESTADO_RELE, GPIO_INPUT_GET(GPIO_RELE));
            cJSON_AddNumberToObject(respuesta, PROGRAMMER_STATE, datosApp->estadoProgramacion);
            cJSON_AddNumberToObject(respuesta, DEVICE_STATE, datosApp->estadoApp);
            cJSON_AddNumberToObject(respuesta, TEMPERATURA, datosApp->tempActual);
            cJSON_AddNumberToObject(respuesta, HUMEDAD, datosApp->humedad);
            cJSON_AddNumberToObject(respuesta, UMBRAL_TEMPERATURA, datosApp->tempUmbral);
            escribirProgramaActual(datosApp, respuesta);            
            break;
        default:
            codigoRespuesta(respuesta, RESP_NOK);
            printf("enviarReporte--> Salida no prevista\n");
            break;
    }
    
    return respuesta;
    
}
void appUser_AlarmasActivas(DATOS_APLICACION *datosApp, cJSON *respuesta) {
    //Escribe aquí las alarmas especificas de tu aplicacion.
    
    cJSON_AddBoolToObject(respuesta, APP_ALARMA_SENSOR_TEMPERATURA, alarmasAplicacion.sensorTemperatura);
    
}


enum ESTADO_RELE operacionRele(DATOS_APLICACION *datosApp, enum TIPO_ACTUACION_RELE tipo, enum ESTADO_RELE operacion) {
    

    
    switch(tipo) {
        case MANUAL:
        case REMOTA:
            if (GPIO_INPUT_GET(GPIO_RELE) == OFF) {
                GPIO_OUTPUT_SET(GPIO_ID_PIN(GPIO_RELE), ON);
            } else {
               GPIO_OUTPUT_SET(GPIO_ID_PIN(GPIO_RELE), OFF); 
            }
            break;
        case TEMPORIZADA:
            GPIO_OUTPUT_SET(GPIO_ID_PIN(GPIO_RELE), operacion);
            break;
        default:
            break;
        
    }
    
    return GPIO_INPUT_GET(GPIO_RELE);
    
    
    
}
void actuacionReleRemoto(DATOS_APLICACION *datosApp, cJSON *respuesta) {

    cJSON_AddNumberToObject(respuesta, DLG_COD_RESPUESTA, DLG_OK_CODE);
    cJSON_AddNumberToObject(respuesta, APP_COMAND_ESTADO_RELE, GPIO_INPUT_GET(GPIO_RELE));
    cJSON_AddNumberToObject(respuesta, DEVICE_STATE, datosApp->estadoApp);
    cJSON_AddNumberToObject(respuesta, PROGRAMMER_STATE, datosApp->estadoProgramacion);          
    cJSON_AddNumberToObject(respuesta, TEMPERATURA, datosApp->tempActual);
    cJSON_AddNumberToObject(respuesta, HUMEDAD, datosApp->humedad);
    cJSON_AddNumberToObject(respuesta, UMBRAL_TEMPERATURA, datosApp->tempUmbral);
    escribirProgramaActual(datosApp, respuesta);
    codigoRespuesta(respuesta,RESP_OK);
}

void consultarEstadoAplicacion(DATOS_APLICACION *datosApp, cJSON *respuesta) {

    cJSON_AddNumberToObject(respuesta, APP_COMAND_ESTADO_RELE, GPIO_INPUT_GET(GPIO_RELE));
    cJSON_AddNumberToObject(respuesta, DEVICE_STATE, datosApp->estadoApp);
    cJSON_AddNumberToObject(respuesta, PROGRAMMER_STATE, datosApp->estadoProgramacion);
    cJSON_AddNumberToObject(respuesta, TEMPERATURA, datosApp->tempActual);
    cJSON_AddNumberToObject(respuesta, HUMEDAD, datosApp->humedad);
    cJSON_AddNumberToObject(respuesta, UMBRAL_TEMPERATURA, datosApp->tempUmbral);
    escribirProgramaActual(datosApp, respuesta);
    appUser_parametrosAplicacion(datosApp, respuesta);
    codigoRespuesta(respuesta,RESP_OK);
    
    
}

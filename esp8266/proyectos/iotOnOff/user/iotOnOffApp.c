/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "user_config.h"
#include "gpio.h"
#include "iotOnOffApp.h"
#include "cJSON.h"
#include"dialogoJson.h"
#include "ota_config.h"

ALARMAS_APLICACION alarmasAplicacion;

// macros para que funcionen las interrupciones.
#define ETS_GPIO_INTR_ENABLE() _xt_isr_unmask ((1 << ETS_GPIO_INUM))
#define ETS_GPIO_INTR_DISABLE() _xt_isr_mask(1 << ETS_GPIO_INUM) 
extern xTaskHandle manejadorBeat;
extern xTaskHandle manejadorMqtt;

void appUser_ConfiguracionDefecto(DATOS_APLICACION *datosApp) {
    //Escribe aqui el codigo de inicializacion por defecto de la aplicacion.
    
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
    printf("appUser_notificarWifiConectando\n");


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
 
    os_timer_disarm(&notificacionWifi);
    GPIO_OUTPUT_SET(GPIO_ID_PIN(GPIO_LED), 1);
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

bool appUser_analizarComandoAplicacion(cJSON *peticion, int nComando, DATOS_APLICACION *datosApp, cJSON *respuesta) {
    

    enum ESTADO_RELE estadoRele;
    switch(nComando) {
        case OPERAR_RELE:
            //cJSON_Delete(root);
            estadoRele = operacionRele(datosApp, MANUAL, INDETERMINADO);
            actuacionReleRemoto(datosApp, respuesta);
            //respuesta = generarReporte(datosApp, ACTUACION_RELE_REMOTO, root);
            break;

        case STATUS_RELE:
            //cJSON_Delete(root);
            consultarEstadoAplicacion(datosApp, respuesta);
            //respuesta = generarReporte(datosApp, ESTADO, root);
            break;
        default:
            //respuesta = cabeceraRespuestaComando(datosApp, root);
            comandoDesconocido(datosApp, respuesta);
            //comando desconocido
            break;
    }
    
    return TRUE;
    
}

void pulsacion(DATOS_APLICACION *datosApp) {
    
    static os_timer_t repeticion;
    static uint8 rep=0;
    cJSON *root;
    
    
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
            /*
            if ( smartconfig_set_type(SC_TYPE_ESPTOUCH) == TRUE) {
                printf("Entramos en smartconfig\n");
                //esptouch_set_timeout(45);
                smartconfig_start(smartconfig_done);
                
            }*/
            
            
            if (wifi_station_get_connect_status() == STATION_GOT_IP) {
                system_restart();
            } else {
                //smartconfig
                printf("sin ip, entramos en smartconfig...");
                //smartconfig_set_type(SC_TYPE_ESPTOUCH);
                smartconfig_start(smartconfig_done);

            }
            
        } else {
            printf("pulsacion corta\n");
            operacionRele(datosApp, MANUAL, INDETERMINADO);
            root = generarReporte(datosApp, ACTUACION_RELE_LOCAL, NULL);
            notificarAccion(root, datosApp);
            //operarRele(datosApp);
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
    alarmasAplicacion.sensorTemperatura = TRUE;
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
    //Ponemos el rele apagado por defecto
    GPIO_OUTPUT_SET(GPIO_ID_PIN(GPIO_RELE), 0);
    GPIO_OUTPUT_SET(GPIO_ID_PIN(GPIO_LED), 0);



    
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
    }
    
    printf("notificarAccion-->Notificado el evento del rele\n");

    
    
}


cJSON* generarReporte(DATOS_APLICACION *datosApp, enum TIPO_INFORME tipoInforme, cJSON *comandoOriginal) {
    

    cJSON *respuesta = NULL;


    respuesta = cabeceraEspontaneo(datosApp, tipoInforme);
    switch(tipoInforme) {
        case ARRANQUE_APLICACION:
            printf("generarReporte--> enviando arranqueAplicacion");            
            cJSON_AddNumberToObject(respuesta, APP_COMAND_ESTADO_RELE, GPIO_INPUT_GET(GPIO_RELE));
            cJSON_AddNumberToObject(respuesta, PROGRAMMER_STATE, datosApp->estadoProgramacion);
            cJSON_AddNumberToObject(respuesta, DEVICE_STATE, datosApp->estadoApp);
            escribirProgramaActual(datosApp, respuesta);
            codigoRespuesta(respuesta,RESP_OK);
            break;
        case ACTUACION_RELE_LOCAL:
            cJSON_AddNumberToObject(respuesta, APP_COMAND_ESTADO_RELE, GPIO_INPUT_GET(GPIO_RELE));
            cJSON_AddNumberToObject(respuesta, PROGRAMMER_STATE, datosApp->estadoProgramacion);
            cJSON_AddNumberToObject(respuesta, DEVICE_STATE, datosApp->estadoApp);
            escribirProgramaActual(datosApp, respuesta);
            codigoRespuesta(respuesta,RESP_OK);

            break;
        case CAMBIO_DE_PROGRAMA:
            cJSON_AddNumberToObject(respuesta, PROGRAMMER_STATE, datosApp->estadoProgramacion);
            cJSON_AddNumberToObject(respuesta, APP_COMAND_ESTADO_RELE, GPIO_INPUT_GET(GPIO_RELE));               
            cJSON_AddNumberToObject(respuesta, DEVICE_STATE, datosApp->estadoApp);
            escribirProgramaActual(datosApp, respuesta);
            codigoRespuesta(respuesta,RESP_OK);
            
            break;
        default:
            codigoRespuesta(respuesta, RESP_NOK);
            printf("enviarReporte--> Salida no prevista\n");
            break;
    }
    
    return respuesta;
    
}


void appUser_notificarArranque(DATOS_APLICACION *datosApp, int tipoArranque) {
    
    cJSON *root = NULL;

    
    
    root = generarReporte(datosApp, ARRANQUE_APLICACION, NULL);
    notificarAccion(root, datosApp);
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


    //Ejecutamos la accion que proceda dentro de la aplicacion
    printf("appUser_ejecutarAccionPrograma-->ejecutando accion programa\n");
    datosApp->estadoApp = NORMAL_AUTO;
    operacionRele(datosApp, TEMPORIZADA, datosApp->programacion[programaActual].accion);
    root = generarReporte(datosApp, CAMBIO_DE_PROGRAMA, NULL);
    //root = actuarRele(datosApp, TEMPORIZADA, datosApp->programacion[programaActual].accion); 
    notificarAccion(root, datosApp);
    
    return 0;

}


int appUser_ejecutarAccionProgramaDefecto(DATOS_APLICACION *datosApp) {
    
    printf("appUser_ejecutarAccionProgramaDefecto--> Accion por defecto al no haber programacion hay un solo prog a futuro\n");
    printf("appUser_ejecutarAccionProgramaDefecto-->por defecto rele apagado\n");
    GPIO_OUTPUT_SET(GPIO_ID_PIN(GPIO_RELE), OFF);
    
}

void appUser_accionesOta(DATOS_APLICACION *datosApp) {
    printf("vamos a ejecutar OTA\n");
    //vTaskDelete(manejadorBeat);
}

void appUser_notificarUpgradeOta() {

    printf("notificacion UpgradeOta\n");
    os_timer_disarm(&noficacionUpgradeOta);
    os_timer_setfn(&noficacionUpgradeOta, (os_timer_func_t*)parapadeoLed,NULL);
    os_timer_arm(&noficacionUpgradeOta, PARPADEO_UPGRADE_OTA, 1);


    
}

bool appUser_modificarConfApp(cJSON *root, DATOS_APLICACION *datosApp, cJSON *respuesta) {
   
    //Escribe aqui los parametros especificos para modificar la aplicacion
    return TRUE;

    
}

void appUser_parametrosAplicacion(DATOS_APLICACION *datosApp, cJSON *respuesta) {
    
    //Escribe aqui los parametros especificos de la aplicacion para pintarlos en el informe
    


}


bool appUser_modificarProgramacionDispositivo(DATOS_APLICACION *datosApp, int nPrograma, cJSON *peticion, cJSON *respuesta) {
    

    cJSON *nodo;

    printf("appUser_modificarProgramacionDispositivo--> inicio\n");
    nodo = cJSON_GetObjectItem(peticion, PROGRAM);    
    //campo = cJSON_GetObjectItem(nodo, DURATION_PROGRAM);
    datosApp->programacion[nPrograma].duracion = extraerDatoInt(nodo, DURATION_PROGRAM);
    if (datosApp->programacion[nPrograma].duracion <= 0) {
        datosApp->programacion[nPrograma].duracion = 0;
    } else {
        cJSON_AddNumberToObject(respuesta, DURATION_PROGRAM, datosApp->programacion[nPrograma].duracion);
    }
   

    return TRUE;
   
}

void appUser_AlarmasActivas(DATOS_APLICACION *datosApp, cJSON *respuesta) {
    //Escribe aquí las alarmas especificas de tu aplicacion.
    
    //cJSON_AddBoolToObject(nodoRespuesta, APP_ALARMA_SENSOR_TEMPERATURA, alarmasAplicacion.sensorTemperatura);
    
    return ;
}

void actuacionReleRemoto(DATOS_APLICACION *datosApp, cJSON *respuesta) {

    cJSON_AddNumberToObject(respuesta, DLG_COD_RESPUESTA, DLG_OK_CODE);
    cJSON_AddNumberToObject(respuesta, APP_COMAND_ESTADO_RELE, GPIO_INPUT_GET(GPIO_RELE));
    cJSON_AddNumberToObject(respuesta, DEVICE_STATE, datosApp->estadoApp);
    cJSON_AddNumberToObject(respuesta, PROGRAMMER_STATE, datosApp->estadoProgramacion);          
    escribirProgramaActual(datosApp, respuesta);
    codigoRespuesta(respuesta,RESP_OK);
}

void consultarEstadoAplicacion(DATOS_APLICACION *datosApp, cJSON *respuesta) {

    cJSON_AddNumberToObject(respuesta, APP_COMAND_ESTADO_RELE, GPIO_INPUT_GET(GPIO_RELE));
    cJSON_AddNumberToObject(respuesta, DEVICE_STATE, datosApp->estadoApp);
    cJSON_AddNumberToObject(respuesta, PROGRAMMER_STATE, datosApp->estadoProgramacion);          
    escribirProgramaActual(datosApp, respuesta);
    codigoRespuesta(respuesta,RESP_OK);
    
    
}

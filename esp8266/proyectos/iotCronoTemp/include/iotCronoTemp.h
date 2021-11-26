/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   iotOnOffApp.h
 * Author: t126401
 *
 * Created on 20 de julio de 2018, 12:56
 */


#ifndef IOTONOFFAPP_H
#define IOTONOFFAPP_H

#ifdef __cplusplus
extern "C" {
#endif
#define APP_NAME "iotCronoTemp"
#define DEVICE_TYPE 1


#define GPIO_LED    13
#define GPIO_RELE   12
#define GPIO_BOTON  0
#define PULSADO             0
#define NO_PULSADO          1
    
#define PARPADEO_WIFI_CONECTANDO 100
#define PARPADEO_WIFI_CONECTADO 300
#define PARPADEO_BROKER_DESCONECTADO 500
#define PARPADEO_SMARTCONFIG         60
#define PARPADEO_UPGRADE_OTA         50
#define APP_COMAND_RELE                 "rele"
#define APP_COMAND_OPERACION_RELE       "opRele"
#define APP_COMAND_ESTADO_RELE          "estadoRele"

    
#define NUM_REPETICIONES    3

    
#define DLG_OBSERVACIONES_ACTUAR_RELE_NOK "Error al actuar sobre el rele"

#define MODIFICAR_APP "modificarApp"
#define DLG_OBSERVACIONES_OK "Ejecucion realizada con exito"    
#define DLG_OBSERVACIONES_NOK "Ejecucion no realizada"
#define APP_ALARMA_SENSOR_TEMPERATURA   "sensorTemperatura"    

typedef struct ALARMAS_APLICACION {
    bool sensorTemperatura;

}ALARMAS_APLICACION;
    
    
enum COMANDOS_APP {
    OPERAR_RELE = 50,
    STATUS_RELE, 
    MODIFICAR_UMBRAL
};    

enum ESTADO_RELE {
    OFF,
    ON,
    INDETERMINADO
} ESTADO_RELE;

enum TIPO_ACTUACION_RELE {
    MANUAL,
    REMOTA,
    TEMPORIZADA,
    ARRANQUE_RELE,
    EXPIRADA,
    POR_TEMPERATURA,
    NOTIFICACION_ESTADO,
    ERROR_LECTURA
};




/**
 * @brief Inicializa los datos de la aplicacion.
 * @param datosApp es el objeto de la aplicacion
 */
void appUser_inicializarAplicacion(DATOS_APLICACION *datosApp);

/**
 * @brief Funcion para tratar las interrupciones del boton.
 * @param datosApp
 */
void tratarInterrupcionesPulsador(DATOS_APLICACION *datosApp);


/**
 * @brief Funcion axilicar de tratarInterrupcionesPulsador para las pulsaciones cortas y largas del boton.
 * @param datosApp
 */
void pulsacion(DATOS_APLICACION *datosApp);

/**
 * @brief funcion para la gestion de dialogos json de la aplicacion
 * @param root es el objeto json de la peticion
 * @param nComando el la id del comando
 * @param datosApp objeto de la aplicacion
 * @return se devuelve el objeto json como respuesta a la ejecucion del comando.
 */
//cJSON* appUser_analizarComandoAplicacion(cJSON *root, int nComando, DATOS_APLICACION *datosApp);






cJSON* generarRespuesta(cJSON *root,enum TIPO_ACTUACION_RELE tipo, DATOS_APLICACION *datosApp, enum TIPO_INFORME tipoInforme);


void ejecutarLecturaTermometro(DATOS_APLICACION *datosApp);

void notificarAccion(cJSON *root, DATOS_APLICACION *datosApp);

cJSON* actuarRele(cJSON* root, DATOS_APLICACION *datosApp, enum TIPO_ACTUACION_RELE tipo, int releTemporizado);


bool modificarUmbralTemperatura(cJSON *peticion, DATOS_APLICACION *datosApp, cJSON *respuesta);

cJSON* generarReporte(DATOS_APLICACION *datosApp, enum TIPO_INFORME tipoInforme, cJSON *comandoOriginal);

enum ESTADO_RELE operacionRele(DATOS_APLICACION *datosApp, enum TIPO_ACTUACION_RELE tipo, enum ESTADO_RELE operacion);

void actuacionReleRemoto(DATOS_APLICACION *datosApp, cJSON *respuesta);

void consultarEstadoAplicacion(DATOS_APLICACION *datosApp, cJSON *respuesta);


#ifdef __cplusplus
}
#endif

#endif /* IOTONOFFAPP_H */


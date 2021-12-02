/**
Proyecto realizado por Emilio Jimenez Prieto
*/

/*
 * configuracion_usuario.h
 *
 *  Created on: 10 sept. 2020
 *      Author: t126401
 */





#ifndef COMPONENTS_COMUNES_INCLUDE_CONFIGURACION_USUARIO_H_
#define COMPONENTS_COMUNES_INCLUDE_CONFIGURACION_USUARIO_H_
#include "datos_comunes.h"
#include "cJSON.h"
#include "mqtt_client.h"



#define NUM_TIPOS_ALARMAS 6
#define ALARMA_WIFI 0
#define ALARMA_MQTT 1
#define ALARMA_NTP 2
#define ALARMA_NVS 3
#define ALARMA_SENSOR_DHT 4
#define ALARMA_SENSOR_REMOTO 5

#define NOTIFICACION_ALARMA_SENSOR_REMOTO "alarmaDhtRemoto"
#define NOTIFICACION_ALARMA_SENSOR_DHT "alarmaDht"
#define NOTIFICACION_ALARMA_WIFI	"alarmaWifi"
#define NOTIFICACION_ALARMA_MQTT	"alarmaMqtt"
#define NOTIFICACION_ALARMA_NTP		"alarmaNtp"
#define NOTIFICACION_ALARMA_NVS		"alarmaNvs"




typedef struct DATOS_TERMOSTATO {
    float tempActual;
    float humedad;
    double margenTemperatura;
    uint8_t reintentosLectura;
    uint8_t intervaloReintentos;
    uint8_t intervaloLectura;
    double calibrado;


}DATOS_TERMOSTATO;




/**
 * @struct DATOS_APLICACION
 * @brief Estructura general de la aplicacion.
 *
 */
typedef struct DATOS_APLICACION {

	DATOS_GENERALES *datosGenerales;
    nvs_handle handle;
    esp_mqtt_event_handle_t handle_mqtt;
    ALARMA alarmas[NUM_TIPOS_ALARMAS];
    DATOS_TERMOSTATO termostato;

} DATOS_APLICACION;

typedef struct COLA_INTERRUPCION {

	DATOS_APLICACION *datosApp;
	bool reporte;

}COLA_INTERRUPCION;

#endif /* COMPONENTS_COMUNES_INCLUDE_CONFIGURACION_USUARIO_H_ */

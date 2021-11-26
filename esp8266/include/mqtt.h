/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   mqtt.h
 * Author: emilio
 *
 * Created on 19 de julio de 2017, 10:07
 */

//#include "esp_common.h"
#include "MQTTESP8266.h"
#include "MQTTClient.h"
#include "wifi.h"

// maxima longitud del paquete de datos
#define MAXPAYLOAD 735
//maxima longitud del topic
#define LENTOPIC 90
//maxima longitud del buffer a reservar para enviar y recibir paquets mqtt
#define LENBUFFER  MAXPAYLOAD + LENTOPIC

// Cola para la publicacion de mensajes.
xQueueHandle publish_queue;
xQueueHandle colaOta;

//Estructura para el envio de publicaciones.
/**
 * @brief estructura para el envio de datos desde la aplicacion.
 * En datos escribimos lo que queremos enviar.
 * En topic escribimos el topic por donde queremos enviar los datos.
 */
typedef struct MQTT_PACKET {
    char datos[MAXPAYLOAD];
    char topic[LENTOPIC];
} MQTT_PACKET;

//Estructura de datos a incorporar en el proyectos para mqtt
/**
 * Estructura de datos mqtt para la aplicacion
 * el topic de la aplicacion concatena prefix y publish
 */
typedef struct MQTT_PARAM {
    char broker[100];
    int port;
    char user[25];
    char password[25];
    char prefix[25];
    char publish[40];
    char subscribe[40];

} MQTT_PARAM;

#ifndef MQTT_H
#define MQTT_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Tarea que se lanza desde user_main para activar el protocolo mqtt
 * Es necesario que previamente tengamos conexion wifi
 * 
 * @param pvParameters serían los datos de la aplicacion datosApp
 * xTaskCreate(mqtt_task, "mqtt", 2048, (void*) &datosApp.parametrosMqtt, tskIDLE_PRIORITY + 2, NULL);
 */    
 void  mqtt_task(void *pvParameters);
 /**
  * Funcion que recibe los datos del topic subscrito. En nuestro caso /comandos.
  * @param md
  */
LOCAL void ICACHE_FLASH_ATTR topic_received(MessageData* md);
bool sendMqttMessage(char* topic, char *message);




#ifdef __cplusplus
}
#endif

#endif /* MQTT_H */


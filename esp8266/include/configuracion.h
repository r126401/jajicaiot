/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   configuracion.h
 * Author: emilio
 *
 * Created on 20 de julio de 2017, 18:31
 */

/**
 * @brief libreria de configuracion comun a todas los proyectos base
 * proporciona la funcionalidad para grabar datos en la memoria de usuario y cargarla
 * 
 * Se han definido dos segmentos de memoria para guardar la configuracion.
 * Segmento 0: Toda la estructura de la aplicacion.
 * Segmento 1: Estructura de programacion que se linka a la estructura de la aplicacion.
 * Se habilitan dos segmentos mas de backup semejantes a la anterior.
 * Por lo tanto, el segmento 2 y 3 son los homologos.
 * 
 */


#ifndef CONFIGURACION_H
#define CONFIGURACION_H

//#include "esp_common.h"
#include "dialogoJson.h"
//#include "mqtt.h"
#include "ntp.h"

/**
 * @brief Segmento de usuario para grabar la configuracion
 */
#define SEGMENTO_USUARIO 0X7C //0X7C
/**
 * @brief Valor que se graba en la estructura de aplicacion para considerar que es valida
 */
#define VALIDO      0X1000
/**
 * @brief valor invalido de la memoria. Usada para el factory reset.
 */
#define INVALIDO    0XFFFF
/**
 * @brief Definicion de la memoria principal
 */
#define MEMORIA_PRINCIPAL   0
/**
 * @brief Definicion de la memoria backup
 */
#define MEMORIA_BACKUP      2

#define MEMORIA_INVALIDA    3






#ifdef __cplusplus
extern "C" {
#endif

bool salvarConfiguracionMemoria(DATOS_APLICACION *configuracion);
bool cargarDesdeMemoria(DATOS_APLICACION *configuracion);


/**
 * @brief Funcion que carga la configuracion por defecto cuando se enciende la primera vez
 * o cuando la estructura de datos no es valida.
 * @param datosApp es la estrcutura de aplicacion.
 * @return true si la operacion se realiza con exito.
 */
bool defaultConfig(DATOS_APLICACION *datosApp);
/**
 * @brief Funcion para la carga de configuracion. Si no es valida, se carga la de defecto.
 * @param datosApp es la estructura de la aplicacion
 * @param offset, 0 para la memoria principal y 2 para la memoria de backup
 * @return true si la operacion se realiza con exito.
 */
bool cargarConfiguracion(DATOS_APLICACION *datosApp, uint8 offset);

/**
 * @brief funcion para guardar la configuracion
 * @param datosApp es la estructura de la aplicacion
 * @param offset 0 para la memoria principal y 1 para backup.
 * @return true si la operacion se realiza con exito.
 */
bool guardarConfiguracion(struct DATOS_APLICACION *datosApp, uint8 offset);
/**
 * @brief es el punto de entrada de la aplicacion de usuario.
 * Carga la configuracion para que funcione la aplicacion y la guarda en memoria cuando
 * es de defecto.
 * @param datosApp es la estructura de la aplicacion.
 */
void Inicializacion(struct DATOS_APLICACION *datosApp, bool forzado);


/**
 * Funcion para redefinir por la aplicacion en la que se inicializan valores por defecto
 * en el caso de primera instalacion o factory reset
 * @param datosApp Datos de la aplicacion
 */
void appUser_ConfiguracionDefecto(DATOS_APLICACION *datosApp);

#ifdef __cplusplus
}
#endif

#endif /* CONFIGURACION_H */


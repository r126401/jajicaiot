/**
Proyecto realizado por Emilio Jimenez Prieto
*/

/*
 * IOTTERMOMETRO.h
 *
 *  Created on: 10 sept. 2020
 *      Author: t126401
 */

#include "configuracion_usuario.h"
#include "datos_comunes.h"

#ifndef MAIN_INCLUDE_IOTTERMOMETRO_H_
#define MAIN_INCLUDE_IOTTERMOMETRO_H_


enum COMANDOS_APP {
    OPERAR_RELE = 50,
    STATUS_DISPOSITIVO,
	MODIFICAR_UMBRAL,
	SELECCIONAR_SENSOR
};

#define MODIFICAR_APP "modificarApp"
#define APP_PARAMS "configApp"


//void ejecutar_lectura_termometro(DATOS_APLICACION *datosApp);
//esp_err_t lectura_temperatura(DATOS_APLICACION *datosApp);
esp_err_t leer_temperatura_local(DATOS_APLICACION *datosApp);
void tarea_lectura_temperatura(void *parametros);
esp_err_t notificar_fin_arranque(DATOS_APLICACION *datosApp);

#endif /* MAIN_INCLUDE_IOTTERMOMETRO_H_ */

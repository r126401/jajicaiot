/**
Proyecto realizado por Emilio Jimenez Prieto
*/

/*
 * iotOnOff.h
 *
 *  Created on: 10 sept. 2020
 *      Author: t126401
 */

#include "configuracion_usuario.h"
#include "datos_comunes.h"

#ifndef MAIN_INCLUDE_IOTOTERMOMETRO_H_
#define MAIN_INCLUDE_IOTTERMOMETRO_H_

enum COMANDOS_APP {
	STATUS_DISPOSITIVO = 51
};

#define MODIFICAR_APP "modificarApp"
#define APP_PARAMS "configApp"


void tarea_lectura_temperatura(void *parametros);
esp_err_t leer_temperatura_local(DATOS_APLICACION *datosApp);



#endif /* MAIN_INCLUDE_IOTONOFF_H_ */

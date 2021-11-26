/*
 * ota.h
 *
 *  Created on: 20 sept. 2020
 *      Author: t126401
 */

//#include <sys/socket.h>
//#include <netdb.h>
#include "configuracion_usuario.h"
#include "datos_comunes.h"
#ifndef COMPONENTS_OTA_INCLUDE_OTA_H_
#define COMPONENTS_OTA_INCLUDE_OTA_H_

typedef enum ESTADO_OTA {

    OTA_ERROR,
            OTA_DESCARGA_FIRMWARE,
            OTA_BORRANDO_SECTORES,
            OTA_COPIANDO_SECTORES,
            OTA_UPGRADE_FINALIZADO,
            OTA_FALLO_CONEXION,
            OTA_DATOS_CORRUPTOS,
            OTA_PAQUETES_ERRONEOS,
            OTA_CRC_ERRONEO,
            OTA_ERROR_PARTICION,
			OTA_CABECERA_MAL_FORMADA,
			OTA_ERROR_MEMORIA_DISPONIBLE,
			OTA_ERROR_OTA_BEGIN,
			OTA_ERROR_AL_ESCRIBIR_EN_MEMORIA
} ESTADO_OTA;

/**
 * @fn void tarea_upgrade_firmware(DATOS_APLICACION*)
 * @brief Funcion principal para la gestion de upgrade firmware
 *
 * @pre
 * @post
 * @param datosApp
 */
void tarea_upgrade_firmware(DATOS_APLICACION *datosApp);
/*
int  resolver_host(const char *hostname , struct in_addr *in);
*/
/**
 * @fn int conexion_servidor_http(DATOS_APLICACION*)
 * @brief Funcion para conectarse al servidor ota via http.
 *
 * @pre
 * @post
 * @param datosApp estructura de la aplicacion.
 * @return
 */
int conexion_servidor_http(DATOS_APLICACION *datosApp);

#endif /* COMPONENTS_OTA_INCLUDE_OTA_H_ */

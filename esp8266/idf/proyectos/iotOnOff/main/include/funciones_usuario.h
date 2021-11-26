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

#ifndef MAIN_INCLUDE_IOTONOFF_H_
#define MAIN_INCLUDE_IOTONOFF_H_

enum COMANDOS_APP {
    OPERAR_RELE = 50,
    STATUS_RELE
};


enum ESTADO_RELE operacion_rele(DATOS_APLICACION *datosApp, enum TIPO_ACTUACION_RELE tipo, enum ESTADO_RELE operacion);


void programacion_task(void *parametro);


/*
int appUser_ejecutarAccionPrograma(DATOS_APLICACION *datosApp, int programaActual);
void appUser_ejecucionAccionTemporizada(void *datosApp);
int appUser_ejecutarAccionProgramaDefecto(DATOS_APLICACION *datosApp);
void appUser_notificarWifiConectando();
void appUser_notificarWifiConectado();
void appUser_notificarWifiDesconectado();
void appUser_notificarBrokerDesconectado();
void appUser_notificarBrokerConectado();
void appUser_notificarSmartconfig();
void appUser_notificarSmartconfigStop();
void appUser_inicializarAplicacion(DATOS_APLICACION *datosApp);
void appUser_accionesOta(DATOS_APLICACION *datosApp);
void appUser_notificarUpgradeOta();

*/
#endif /* MAIN_INCLUDE_IOTONOFF_H_ */

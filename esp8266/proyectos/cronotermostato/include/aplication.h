/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   aplication.h
 * Author: emilio
 *
 * Created on 21 de octubre de 2017, 8:34
 */

#ifndef APLICATION_H
#define APLICATION_H

#ifdef __cplusplus
extern "C" {
#endif

#include "user_config.h"
#include "esp_common.h"
#include "dht22.h"
#include "dialogoJson.h"
#include "gpio.h"
#include "appdisplay.h"



 

    

    
    
/**
 * Toma la lectura del sensor de temperatura y guarda en la aplicacion los valores
 * de temperatura y humedad. En caso de fallos en la lectura realiza n intentos con un 
 * intervalo de tiempo configurables desde la aplicacion.
 * @param datosApp Estructura de la apliacion.
 * @return devuelve true en caso de lectura correcta.
 */    
bool tomarLecturaDht(struct DATOS_APLICACION *datosApp);

bool calcularAccionTermostato(struct DATOS_APLICACION *datosApp);
void operarRele(bool rele, struct DATOS_APLICACION *datosApp);
void lecturaSensores(struct DATOS_APLICACION *datosApp);
void ejecutarLectura(struct DATOS_APLICACION *datosApp);
cJSON* jsonLectura(struct DATOS_APLICACION *datosApp, cJSON *root);
cJSON* modificarJsonAplicacion(cJSON* root, struct DATOS_APLICACION *datosApp);
bool enviarLectura(struct DATOS_APLICACION *datosApp, cJSON *lectura);
void ICACHE_FLASH_ATTR InicializarGpios(DATOS_APLICACION *datosApp);
void tratarInterrupcionesPulsadores(DATOS_APLICACION *datosApp);
void pulsacionMenu(DATOS_APLICACION *datosApp);
void cancelarReboteMasMenos(bool *rebote);
void botonMenu(DATOS_APLICACION *datosApp);
void botonMas(DATOS_APLICACION *datosApp);
void botonMenos(DATOS_APLICACION *datosApp);
void modoNormal(struct DATOS_APLICACION *datosApp);
void ejecutarSmartconfig();

#ifdef __cplusplus
}
#endif

#endif /* APLICATION_H */


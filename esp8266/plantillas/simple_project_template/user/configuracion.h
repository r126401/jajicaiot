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

#ifndef CONFIGURACION_H
#define CONFIGURACION_H

#include "esp_common.h"
#define SEGMENTO_USUARIO 0XFC





#ifdef __cplusplus
extern "C" {
#endif

bool SalvarConfiguracionMemoria(void *configuracion);
bool CargarDesdeMemoria(void *configuracion);

#ifdef __cplusplus
}
#endif

#endif /* CONFIGURACION_H */


/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   wifi.h
 * Author: emilio
 *
 * Created on 18 de julio de 2017, 12:26
 */

#ifndef WIFI_H
#define WIFI_H

#ifdef __cplusplus
extern "C" {
#endif
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/portmacro.h"    
#include "freertos/semphr.h"
#include "smartconfig.h"
#include "esp_wifi.h"

    
xSemaphoreHandle wifi_alive;   
void  wifi_task(void *pvParameters);
void ICACHE_FLASH_ATTR  smartconfig_done(sc_status status, void *pdata);
LOCAL const char * ICACHE_FLASH_ATTR get_my_id(void);

#ifdef __cplusplus
}
#endif

#endif /* WIFI_H */


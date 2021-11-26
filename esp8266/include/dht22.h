/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   dht.h
 * Author: t126401
 *
 * Created on 19 de diciembre de 2016, 21:25
 */

#ifndef DHT_H
#define DHT_H

#ifdef __cplusplus
extern "C" {
#endif

    
#include "esp_common.h"
    
#define PINDHT 14
typedef enum {
	DHT11,
	DHT22
} DHTType;

typedef struct {
  float temperature;
  float humidity;
} DHT_Sensor_Data;

typedef struct {
  uint8_t pin;
  DHTType type;
} DHT_Sensor;

#define DHT_MAXTIMINGS	10000
#define DHT_BREAKTIME	20
#define DHT_MAXCOUNT	32000



    
//Comienzo de dialogo con DHT11/DHT22
bool ICACHE_FLASH_ATTR StartDHT();
// Lectura de datos.
bool ICACHE_FLASH_ATTR LecturaSensorDht(uint8 *data);
bool DHTRead(uint8 *data);
bool DHTRead16(uint8 *data);
float calcularTemperaturaAm2302( uint8 tempInt, uint8 tempDec);
int parteDecimal(float numero);
float scale_temperature(DHTType sensor_type, uint8 *data);
float scale_humidity(DHTType sensor_type, uint8 *data);


#ifdef __cplusplus
}
#endif

#endif /* DHT_H */


/*
 * ESPRSSIF MIT License
 *
 * Copyright (c) 2015 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP8266 only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include "esp_common.h"


#include "c_types.h"
#include "esp8266/ets_sys.h"
//#include "gpio.h"
#include "esp8266/esp8266.h" // include que faltaba para compilar la libreria.
#include "uart.h"
#include "wifi.h"
#include "user_config.h"
#include "dht22.h"



void temperaturaTask(void* pvparameters) {
    
    float lectura1=0, lectura2=0, humedad;
    uint8 data[5];
    DHTType sensor;

        
    sensor = DHT22;
    
    
    while (1) {
        vTaskDelay(5000 / portTICK_RATE_MS);
        printf("Comienzo de bucle\n");
        
        DHTRead(data);
        lectura1 = scale_temperature(sensor, data);
        lectura2 = calcularTemperaturaAm2302(data[2], data[3]);
        humedad = scale_humidity(sensor, data);
        if (lectura1 < 100) printf("lectura menor que 100.\n");
        printf("lectura1: %02d.%02d\n", (int) lectura1, parteDecimal(lectura1));
        printf("lectura2: %02d.%02d\n", (int) lectura2, parteDecimal(lectura2));
        printf("Humedad: %02d.%02d%%\n", (int) humedad, parteDecimal(humedad));
        //printf("lectura3: %s\n", p);
        
    }
    
    
}


/******************************************************************************
 * FunctionName : user_rf_cal_sector_set
 * Description  : SDK just reversed 4 sectors, used for rf init data and paramters.
 *                We add this function to force users to set rf cal sector, since
 *                we don't know which sector is free in user's application.
 *                sector map for last several sectors : ABCCC
 *                A : rf cal
 *                B : rf init data
 *                C : sdk parameters
 * Parameters   : none
 * Returns      : rf cal sector
*******************************************************************************/
uint32 user_rf_cal_sector_set(void)
{
    flash_size_map size_map = system_get_flash_size_map();
    uint32 rf_cal_sec = 0;

    switch (size_map) {
        case FLASH_SIZE_4M_MAP_256_256:
            rf_cal_sec = 128 - 5;
            break;

        case FLASH_SIZE_8M_MAP_512_512:
            rf_cal_sec = 256 - 5;
            break;

        case FLASH_SIZE_16M_MAP_512_512:
        case FLASH_SIZE_16M_MAP_1024_1024:
            rf_cal_sec = 512 - 5;
            break;

        case FLASH_SIZE_32M_MAP_512_512:
        case FLASH_SIZE_32M_MAP_1024_1024:
            rf_cal_sec = 1024 - 5;
            break;

        default:
            rf_cal_sec = 0;
            break;
    }

    return rf_cal_sec;
}




/******************************************************************************
 * FunctionName : user_init
 * Description  : entry of user application, init user function here
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void user_init(void)
{
    double prueba;
    
    prueba=12.5;

    // Colocamos el puerto serie a 115200 bps
    UART_SetBaudrate(UART0, 115200);
    DBG("SDK version:%s\n", system_get_sdk_version());
    wifi_set_opmode(STATION_MODE);
    
    DBG("Esto es una prueba\n");
    DBG("El valor de prueba es %d.%d\n", (int) prueba, parteDecimal((float) prueba));
    xTaskCreate(wifi_task, "wifi", 512, NULL, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(temperaturaTask, "Leer tempertarura", 512, NULL, tskIDLE_PRIORITY + 2, NULL);

}

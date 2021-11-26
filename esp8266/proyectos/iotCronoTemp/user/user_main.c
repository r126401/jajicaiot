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
#include "uart.h"
#include "user_config.h"
#include "dialogoJson.h"
#include "wfof.h"
#include "wfof_idxs.h"
#include "wlcd.h"
#include "gpio.h"
//#include "appdisplay.h"

xTaskHandle manejadorBeat;
xTaskHandle manejadorMqtt;

extern NTP_CLOCK reloj;
#define CRONOTERMOSTATO 1

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

    MQTT_PARAM* datosMqtt;
    datosMqtt = (struct MQTT_PARAM*) zalloc(sizeof(struct MQTT_PARAM));
        //Creacion de la cola para mqtt
    publish_queue = xQueueCreate(20, sizeof(MQTT_PACKET));
    if ((colaOta = xQueueCreate(5, sizeof(int))) == NULL) {
    printf("Error al crear la cola ota\n");
    return;
}

    // Colocamos el puerto serie a 115200 bps
    UART_SetBaudrate(UART0, 115200);
    wifi_set_opmode(STATION_MODE);
    //Creacion del Semaforo entre mqtt_task y wifi_task
    vSemaphoreCreateBinary(wifi_alive);
     //Toma el semaforo la Tarea Wifi
    xSemaphoreTake(wifi_alive, 0);  // take the default semaphore
    Inicializacion(&datosApp, false);
    appUser_inicializarAplicacion(&datosApp);
    realClock(&datosApp.clock);
    datosApp.ota.swVersion = VERSION_SW;
    datosApp.tipoDispositivo = CRONOTERMOSTATO;
    xTaskCreate(wifi_task, "wifi", 256, NULL, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(mqtt_task, "mqtt", 2048, (void*) &datosApp, tskIDLE_PRIORITY + 2, &manejadorMqtt);
    tareaFota(&datosApp);
    printf("Version aplicacion:%d\n", datosApp.ota.swVersion);
    printf("memoria disponible: %d\n", system_get_free_heap_size());
    printf("boot: %u\n", system_get_boot_version());
    printf("direccion usada: %x\n", system_get_userbin_addr());
    printf("modo boot:%u\n", system_get_boot_mode());
    printf("flash:%u\n", system_get_flash_size_map());
    //printf("Modo arranque: %d\n", system_get_rst_info());
    printf("datosSPI: %u\n", spi_flash_get_id());
    printf("fin de user_init\n");

    
}


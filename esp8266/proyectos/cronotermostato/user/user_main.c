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
#include "appdisplay.h"
#include "wlcd_defs.h"

//Ok, revisado
LOCAL void ICACHE_FLASH_ATTR sensorTask(void * pvParameters) {
    

    struct DATOS_APLICACION *datosApp;
    
    datosApp = (struct DATOS_APLICACION*) pvParameters;
    while (1) {
        printf("sensorTask-->lectura\n");
        //wlcd_wakeup();
        
        vTaskDelay(datosApp->intervaloLectura * 1000 / portTICK_RATE_MS); // This is executed every 10000ms
        if  (datosApp->estadoApp == NORMAL) {
            ejecutarLectura(datosApp);
        } else {
            printf("sensorTask-->No estamos en modo normal\n");
        }
        
        printf("sensorTask-->dormimos %d segundos\n", datosApp->intervaloLectura);
        
        
        if (datosApp->parametrosMqtt.estadoBroker == ON_LINE) {
            printf("sensorTask-->dormimos\n");
            //wlcd_sleep();
            wifi_set_sleep_type(LIGHT_SLEEP_T);
            printf("sensorTask-->despertamos\n");
        }
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
/*
void ejecutarAccionPrograma(DATOS_APLICACION *datosApp) {
    printf("Ejecuto lo que quiero\n");
    
    
}
*/

/******************************************************************************
 * FunctionName : user_init
 * Description  : entry of user application, init user function here
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
//oK, revisado
void user_init(void)
{


    enum OP_DISPLAY op;

    publish_queue = xQueueCreate(20, sizeof(MQTT_PACKET));
    lcd_queue = xQueueCreate(20, sizeof(MSG_LCD));

    

    // Colocamos el puerto serie a 115200 bps
    UART_SetBaudrate(UART0, 115200);
    DBG("user_init-->SDK version:%s\n", system_get_sdk_version());
    DBG("user_init-->Heap: %d\n", system_get_free_heap_size());
    system_print_meminfo();
    wifi_set_opmode(STATION_MODE);
    
    //Creacion del Semaforo entre mqtt_task y wifi_task
    vSemaphoreCreateBinary(wifi_alive);
     //Toma el semaforo la Tarea Wifi
    xSemaphoreTake(wifi_alive, 0);  // take the default semaphore

#ifdef LCD    
    displayInicializarLcd();
    displayBorrarLcd();
#endif    
    // Inicializacion de la aplicacion
    Inicializacion(&datosApp);    
    //Inicializacion de todos los GPIOS menos los del lcd
    datosApp.estadoApp = NORMAL;
    datosApp.botonPulsado = false;
    datosApp.clock.estado = SIN_HORA;
    datosApp.clock.timeValid = false;
    datosApp.estadoProgramacion = INVALID_PROG;
    datosApp.modo = MANUAL;
    //Activacion del reloj en tiempo real
    realClock(&datosApp.clock);
    
    InicializarGpios(&datosApp);
    //Aplicacion en modo normal.

    //datosApp.estadoApp = NORMAL;
    
    
    calcularAccionTermostato(&datosApp);
    


    //Activacion de las tareas.
    //wifi
    xTaskCreate(wifi_task, "wifi", 256, (void*) &datosApp, tskIDLE_PRIORITY + 1, NULL);
    //mqtt
    xTaskCreate(mqtt_task, "mqtt", 1720, (void*) &datosApp, tskIDLE_PRIORITY + 2, NULL);
    //Sensor de temperatura
    xTaskCreate(sensorTask, "beat", 320, (void*) &datosApp, tskIDLE_PRIORITY + 3, NULL);
    //Operaciones sobre el display
    xTaskCreate(lcd_task, "lcd", 320, (void*) &datosApp, tskIDLE_PRIORITY, NULL);
    
#ifdef LCD
    pintarLcd(NORMAL_LCD, PAINT);
    pintarLcd(MODE_WORK, PAINT);
    
    
#endif
    
    
    

}

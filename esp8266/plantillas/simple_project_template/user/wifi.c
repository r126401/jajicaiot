/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "esp_common.h"
#include "wifi.h"
#include "user_config.h"






/******************************************************************************
 * FunctionName : wifi_task
 * Description  : Gestiona la conexion wifi y los reintentos.
 * Parameters   : none
 * Returns      : none
*******************************************************************************/

 void  wifi_task(void *pvParameters)
{
    uint8_t status;

    if (wifi_get_opmode() != STATION_MODE)
    {
        wifi_set_opmode(STATION_MODE);
        vTaskDelay(1000 / portTICK_RATE_MS);
        system_restart();
    }

    while (1)
    {
        printf("WiFi: Connecting to WiFi\n");
        wifi_station_connect();
        struct station_config *config = (struct station_config *)zalloc(sizeof(struct station_config));
        wifi_station_get_config_default(config);
        //sprintf(config->ssid, "Salon");
        //sprintf(config->password, "calbdtre");
        wifi_station_set_config(config);
        free(config);
        status = wifi_station_get_connect_status();
        int8_t retries = 30;
        while ((status != STATION_GOT_IP) && (retries > 0))
        {
            status = wifi_station_get_connect_status();
            if (status == STATION_WRONG_PASSWORD)
            {
                printf("WiFi: Wrong password\n");
                break;
            }
            else if (status == STATION_NO_AP_FOUND)
            {
                printf("WiFi: AP not found\n");
                break;
            }
            else if (status == STATION_CONNECT_FAIL)
            {
                printf("WiFi: Connection failed\n");
                break;
            }
            vTaskDelay(1000 / portTICK_RATE_MS);
            --retries;
        }
        if (status == STATION_GOT_IP)
        {
            printf("WiFi: Connected\n");
            vTaskDelay(1000 / portTICK_RATE_MS);
        }
        while ((status = wifi_station_get_connect_status()) == STATION_GOT_IP)
        {
            vTaskDelay(1000 / portTICK_RATE_MS);
        }
        printf("WiFi: Disconnected\n");
        wifi_station_disconnect();
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}


 void ICACHE_FLASH_ATTR  smartconfig_done(sc_status status, void *pdata)
{
    
    char* intermedio;
    //char* broker;
    //intermedio = malloc(150);
    
    
    switch(status) {
        case SC_STATUS_WAIT:
            DBG("smartconfig_done: C_STATUS_WAIT\n");
            break;
        case SC_STATUS_FIND_CHANNEL:
            DBG("smartconfig_done: SC_STATUS_FIND_CHANNEL\n");
            break;
        case SC_STATUS_GETTING_SSID_PSWD:
            DBG("smartconfig_done: SC_STATUS_GETTING_SSID_PSWD\n");
            sc_type *type = pdata;
            if (*type == SC_TYPE_ESPTOUCH) {
                DBG("smartconfig_done: SC_TYPE:SC_TYPE_ESPTOUCH\n");
            } else {
                DBG("smartconfig_done: SC_TYPE:SC_TYPE_AIRKISS\n");
            }
            break;
        case SC_STATUS_LINK:
            DBG("smartconfig_done: SC_STATUS_LINK\n");
            struct station_config *sta_conf = pdata;
/*            
            strcpy(intermedio, sta_conf->password);
            
            strcpy(sta_conf->password,strtok(intermedio,":"));
            broker = strtok(NULL, ":");
            if (broker != NULL) {
                strcpy( confGlobal.broker, broker);
                DBG("Se copia nuevo broker: %s", confGlobal.broker);
            }
            
            free(intermedio);
*/            
            DBG("smartconfig_done: essid = %s, ", sta_conf->ssid);
            DBG("smartconfig_done: contrasena = %s\n", sta_conf->password);
	        wifi_station_set_config(sta_conf);
	        wifi_station_disconnect();
	        if (wifi_station_connect() == FALSE) {
                    DBG("smartconfig_done: Fallo al conectar por smartconfig\n");
                    smartconfig_stop();
                    return;
                }
                    
                
            break;
        case SC_STATUS_LINK_OVER:
            DBG("smartconfig_done: SC_STATUS_LINK_OVER\n");
            if (pdata != NULL) {
				//SC_TYPE_ESPTOUCH
                uint8 phone_ip[4] = {0};

                memcpy(phone_ip, (uint8*)pdata, 4);
                DBG("smartconfig_done: Phone ip: %d.%d.%d.%d\n",phone_ip[0],phone_ip[1],phone_ip[2],phone_ip[3]);
            } /*else {
            	//SC_TYPE_AIRKISS - support airkiss v2.0
	airkiss_start_discover();
			}*/
            smartconfig_stop();
            break;
    }
	
}

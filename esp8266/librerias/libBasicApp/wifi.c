/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "esp_common.h"
#include "wifi.h"
#include "user_config.h"
#include "configuracion.h"
#ifdef LCD

#include "appdisplay.h"

#endif





/******************************************************************************
 * FunctionName : wifi_task
 * Description  : Gestiona la conexion wifi y los reintentos.
 * Parameters   : none
 * Returns      : none
*******************************************************************************/

 void  wifi_task(void *pvParameters)
{
    uint8_t status;
    DATOS_APLICACION *datosApp;
    
    datosApp = (DATOS_APLICACION*) pvParameters;

#ifdef LCD
    displayPintarWifi(NO_PAINT);
#endif
    if (wifi_get_opmode() != STATION_MODE)
    {
        wifi_set_opmode(STATION_MODE);
        vTaskDelay(1000 / portTICK_RATE_MS);
        system_restart();
    }
    

    
    //notificarAlarma(datosApp, ALARMA_WIFI, ALARMA_INDETERMINADA, NO);
    while (1)
    {
       
        printf("wifi_task--> Conectandose a la red wifi\n");
        wifi_station_connect();
        struct station_config *config = (struct station_config *)zalloc(sizeof(struct station_config));
        wifi_station_get_config_default(config);
        printf("wifi_task--> Red configurada: %s con password: %s\n", config->ssid, config->password);
        //sprintf(config->ssid, "Salon");
        //sprintf(config->password, "calbdtre");
        wifi_station_set_config(config);
        free(config);
        
        status = wifi_station_get_connect_status();
        int8_t retries = 100;
        while ((status != STATION_GOT_IP) && (retries > 0))
        {
            appUser_notificarWifiConectando();
            status = wifi_station_get_connect_status();
            printf("status es %d\n", status);
            if (status == STATION_WRONG_PASSWORD)
            {
                printf("wifi_task--> Password erronea\n");
                break;
            }
            else if (status == STATION_NO_AP_FOUND)
            {
                printf("wifi_task--> : No se encuentra la red\n");
                break;
            }
            else if (status == STATION_CONNECT_FAIL)
            {
                printf("wifi_task--> : Fallo al conectarse a la red\n");
                appUser_notificarWifiDesconectado();
                break;
            }
            vTaskDelay(1000 / portTICK_RATE_MS);
            --retries;
        }
        if (status == STATION_GOT_IP)
        {
            printf("wifi_task--> : Conectado...\n");
            appUser_notificarWifiConectado();
            //notificarAlarma(datosApp, ALARMA_WIFI, ALARMA_NO_ACTIVA, NO);
            //wifi_station_set_config(config);

            
            //free(config);
            //config = NULL;

            vTaskDelay(1000 / portTICK_RATE_MS);
            
        }
        while ((status = wifi_station_get_connect_status()) == STATION_GOT_IP)
        {
            xSemaphoreGive(wifi_alive);
            ////notificarAlarma(ALARMA_WIFI, ALARMA_NO_ACTIVA, NO);
            // printf("WiFi: Alive\n");
            vTaskDelay(1000 / portTICK_RATE_MS);
        }
        //notificarAlarma(datosApp, ALARMA_WIFI, ALARMA_ACTIVA, NO);
        printf("wifi_task--> Desconectado\n");
        //appUser_notificarWifiDesconectado();
        wifi_station_disconnect();
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}


 void ICACHE_FLASH_ATTR  smartconfig_done(sc_status status, void *pdata)
{
    
    char* intermedio;
    uint8 len;
    char* cadena;
    char *ptr;
    
    appUser_notificarSmartconfig();
    printf("smartconfig_done--> estado %d\n", status);
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
            // codigo para meter los datos del broker y demas
            len = strlen(sta_conf->password);
            intermedio = (char*) calloc(len, sizeof(char));
            
            
            ptr = strtok(sta_conf->password, ":");
            strcpy (sta_conf->password, ptr);
            printf("password: %s\n", sta_conf->password);
            
            if ((ptr = strtok(NULL, ":")) != NULL) {
                strcpy(datosApp.parametrosMqtt.broker, ptr);
                printf ("broker:%s\n", datosApp.parametrosMqtt.broker);
            }
            
            
            
            if ((ptr = strtok(NULL, ":")) != NULL) {
                datosApp.parametrosMqtt.port = atoi(ptr);
                printf("puerto: %d\n", datosApp.parametrosMqtt.port);
            }
            if ((ptr = strtok(NULL, ":")) != NULL) {
                strcpy(datosApp.parametrosMqtt.user, ptr);
                printf("user: %s\n", datosApp.parametrosMqtt.user);
            }
            if ((ptr = strtok(NULL, ":")) != NULL) {
                strcpy(datosApp.parametrosMqtt.password, ptr);
                printf("password: %s\n", datosApp.parametrosMqtt.password);
            }
            cadena = NULL;
            strcpy(datosApp.parametrosMqtt.prefix, "/");
            strcpy(datosApp.parametrosMqtt.publish, "pub_");
            strcat(datosApp.parametrosMqtt.publish, get_my_id());
            strcpy(datosApp.parametrosMqtt.subscribe, "sub_");
            strcat(datosApp.parametrosMqtt.subscribe, get_my_id());
            
            printf("topic_pub: %s\n",datosApp.parametrosMqtt.publish );
            printf("topic_sub: %s\n", datosApp.parametrosMqtt.subscribe);
            guardarConfiguracion(&datosApp, MEMORIA_PRINCIPAL);
            free(cadena);
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
                DBG("smartconfig_done--> direccion ip: %d.%d.%d.%d\n",phone_ip[0],phone_ip[1],phone_ip[2],phone_ip[3]);
            } /*else {
            	//SC_TYPE_AIRKISS - support airkiss v2.0
	airkiss_start_discover();
			}*/
            smartconfig_stop();
            appUser_notificarSmartconfigStop();
            break;
        default:
            printf("smartconfig_done-->Estado no previsto\n");
            break;
                   
    }
    
    printf("smartconfig_done-->Saliendo...\n");
	
}

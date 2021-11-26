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
#include "ota_config.h"
#include "user_config.h"
#include "esp_common.h"
#include "lwip/mem.h"
#include "lwip/sockets.h"
#include "lwip/inet.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "upgrade.h"
#include "uart.h"
#include "lwip/netdb.h"
#include "lwip/sys.h"



/*********************global param define start ******************************/
LOCAL os_timer_t upgrade_timer;
LOCAL uint32 totallength = 0;
LOCAL uint32 sumlength = 0;
LOCAL bool flash_erased = false;
LOCAL xTaskHandle *ota_task_handle = NULL;
/*********************global param define end *******************************/

/******************************************************************************
 * FunctionName : upgrade_recycle
 * Description  : recyle upgrade task, if OTA finish switch to run another bin
 * Parameters   :
 * Returns      : none
 *******************************************************************************/
LOCAL void upgrade_recycle(uint8 tipo)
{
    totallength = 0;
    sumlength = 0;
    flash_erased = false;

    if (tipo == 0) {

        printf("upgrade_recycle--> vamos a finalizar\n");
        system_upgrade_deinit();
        vTaskDelete(ota_task_handle);
        ota_task_handle = NULL;
        printf("finalizando...\n");
        if (system_upgrade_flag_check() == UPGRADE_FLAG_FINISH) {
            printf("upgrade finalizado\n");
            system_upgrade_reboot(); // if need
        } else {
            printf("upgrade_recycle--> aun no ha finalizado\n");
        }
        
    } else {
        printf("upgrade_recycle-->SE PUEDE REINTENTAR OTRA VEZ\n");
        //system_restore();
        //system_restart();
    }
    //notificarOta(datosApp, OTA_UPGRADE_FINALIZADO);

}

LOCAL void upgradeTemporizado() {

    totallength = 0;
    sumlength = 0;
    flash_erased = false;

        printf("upgradeTemporizado--> vamos a finalizar\n");
        system_upgrade_deinit();
        vTaskDelete(ota_task_handle);
        ota_task_handle = NULL;
        printf("upgradeTemporizado-->finalizando...\n");
        if (system_upgrade_flag_check() == UPGRADE_FLAG_FINISH) {
            printf("upgrade_recycle-->upgrade finalizado\n");
            system_upgrade_reboot(); // if need

    }
     
}


/******************************************************************************
 * FunctionName : upgrade_download
 * Description  : parse http response ,and download remote data and write in flash
 * Parameters   : int sta_socket : ota client socket fd
 *                char *pusrdata : remote data
 *                length         : data length
 * Returns      : none
 *******************************************************************************/
void upgrade_download(int sta_socket, char *pusrdata, unsigned short length, DATOS_APLICACION *datosApp)
{
    char *ptr = NULL;
    char *ptmp2 = NULL;
    char lengthbuffer[32];
    if (totallength == 0 && (ptr = (char *)strstr(pusrdata, "\r\n\r\n")) != NULL &&
    (ptr = (char *)strstr(pusrdata, "Content-Length")) != NULL) {
        ptr = (char *) strstr(pusrdata, "\r\n\r\n");
        length -= ptr - pusrdata;
        length -= 4;
        printf("upgrade file download start.\n");
        

        ptr = (char *) strstr(pusrdata, "Content-Length: ");
        if (ptr != NULL) {
            ptr += 16;
            ptmp2 = (char *) strstr(ptr, "\r\n");

            if (ptmp2 != NULL) {
                memset(lengthbuffer, 0, sizeof(lengthbuffer));
                memcpy(lengthbuffer, ptr, ptmp2 - ptr);
                sumlength = atoi(lengthbuffer);
                if (sumlength > 0) {
                    if (false == system_upgrade(pusrdata, sumlength, datosApp)) {
                        system_upgrade_flag_set(UPGRADE_FLAG_IDLE);
                        notificarOta(datosApp, OTA_PAQUETES_ERRONEOS);
                        close(sta_socket);
                        upgrade_recycle(1);
                        return;
                        
//                        goto ota_recycle;
                    }
                    flash_erased = true;
                    ptr = (char *) strstr(pusrdata, "\r\n\r\n");
                    if (false == system_upgrade(ptr + 4, length, datosApp)) {
                        system_upgrade_flag_set(UPGRADE_FLAG_IDLE);
                        notificarOta(datosApp, OTA_PAQUETES_ERRONEOS);
                        close(sta_socket);
                        upgrade_recycle(1);
                        return;
                        //goto ota_recycle;
                    }
                    totallength += length;
                    printf("sumlength = %d\n", sumlength);
                    if (sumlength > 512) {
                        return;
                    } else {
                        printf("error en fichero\n");
                        system_upgrade_flag_set(UPGRADE_FLAG_IDLE);
                        notificarOta(datosApp, OTA_PAQUETES_ERRONEOS);
                        close(sta_socket);
                        upgrade_recycle(1);
                        return;

//                        goto ota_recycle;
                    }
                    //return;
                }
            } else {
                printf("sumlength failed\n");
                system_upgrade_flag_set(UPGRADE_FLAG_IDLE);
                notificarOta(datosApp, OTA_PAQUETES_ERRONEOS);
                close(sta_socket);
                upgrade_recycle(1);
                return;
                
//                goto ota_recycle;
            }
        } else {
            printf("Content-Length: failed\n");
            system_upgrade_flag_set(UPGRADE_FLAG_IDLE);
            notificarOta(datosApp, OTA_PAQUETES_ERRONEOS);
            close(sta_socket);
            upgrade_recycle(1);
            return;
            
//            goto ota_recycle;
        }
    } else {
        totallength += length;
        printf("totallen = %d\n", totallength);
        if (false == system_upgrade(pusrdata, length, datosApp)) {
            system_upgrade_flag_set(UPGRADE_FLAG_IDLE);
            notificarOta(datosApp, OTA_PAQUETES_ERRONEOS);
            close(sta_socket);
            upgrade_recycle(1);
            return;
            
//            goto ota_recycle;
        }
        if (totallength == sumlength) {
            printf("upgrade file download finished.\n");

            if (upgrade_crc_check(system_get_fw_start_sec(), sumlength) != true) {
                printf("upgrade crc check failed !\n");
                system_upgrade_flag_set(UPGRADE_FLAG_IDLE);
                notificarOta(datosApp, OTA_CRC_ERRONEO);
                close(sta_socket);
                upgrade_recycle(1);
                return;

//                goto ota_recycle;
            }
            system_upgrade_flag_set(UPGRADE_FLAG_FINISH);
            notificarOta(datosApp, OTA_UPGRADE_FINALIZADO);  
            close(sta_socket);
            printf("upgrade_download--> voy a llamar a upgrade_recycle 0\n");
            upgrade_recycle(0);
            
//            goto ota_recycle;
        } else {
            return;
        }
    }

//    ota_recycle: printf("go to ota recycle\n");
//    close(sta_socket);
//    upgrade_recycle(1);

}


 int  resolverHost(const char *hostname , struct in_addr *in)
{
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_in *h;
    int rv;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    rv = getaddrinfo(hostname, 0 , &hints , &servinfo);
    if (rv != 0)
    {
        return rv;
    }

    // loop through all the results and get the first resolve
    for (p = servinfo; p != 0; p = p->ai_next)
    {
        h = (struct sockaddr_in *)p->ai_addr;
        in->s_addr = h->sin_addr.s_addr;
    }
    freeaddrinfo(servinfo); // all done with this structure
    return 0;
}
 
/******************************************************************************
 * FunctionName : fota_begin
 * Description  : ota_task function
 * Parameters   : task param
 * Returns      : none
 *******************************************************************************/
void fota_begin(void *pvParameters)
{

    int recbytes;
    int sta_socket;
    char recv_buf[1460];
    struct sockaddr_in remote_ip;
    DATOS_APLICACION* datosApp;
    char* datosOta;
    int dato;
    struct sockaddr_in addr;
    xQueueReset(colaOta);
    
    datosApp = (DATOS_APLICACION*) pvParameters;
    while (1) {
        printf("fota_begin-->Esperando peticion de upgrade\n");
        if (xQueueReceive(colaOta, &dato, portMAX_DELAY) == pdTRUE) {
            printf("dato vale :%d\n", dato);
            if (dato != 5555) {
                printf("fota_begin--> Esto no es un mensaje real\n");
                continue;
            }
            system_upgrade_flag_set(UPGRADE_FLAG_START);
            os_timer_disarm(&upgrade_timer);
            os_timer_setfn(&upgrade_timer, (os_timer_func_t *) upgradeTemporizado, NULL);
            os_timer_arm(&upgrade_timer, OTA_TIMEOUT, 0);
            
            vTaskDelay(ESPERA);

            datosApp->estadoApp = UPGRADE_EN_PROGRESO;
            printf("fota_begin--> Servidor : %s\n", datosApp->ota.server);
            resolverHost(datosApp->ota.server, &(addr.sin_addr));
            printf("fota_begin--> ip OTA: %s\n", ipaddr_ntoa( (ip_addr_t*) &addr.sin_addr.s_addr));
            //Abrimos la conexion
            sta_socket = socket(PF_INET, SOCK_STREAM, 0);
            if (-1 == sta_socket) {

                close(sta_socket);
                printf("fota_begin-->Fallo al abrir el socket !\r\n");
                continue;
            }
            printf("fota_begin-->socket ok!\r\n");
            bzero(&remote_ip, sizeof(struct sockaddr_in));
            remote_ip.sin_family = AF_INET;
            remote_ip.sin_addr.s_addr = addr.sin_addr.s_addr;
            remote_ip.sin_port = htons(datosApp->ota.puerto);
            if(0 != connect(sta_socket,(struct sockaddr *)(&remote_ip),sizeof(struct sockaddr)))
            {
                close(sta_socket);
                printf("fota_begin-->connect fail!\r\n");
                notificarOta(datosApp, OTA_FALLO_CONEXION);
                system_upgrade_flag_set(UPGRADE_FLAG_IDLE);
                upgrade_recycle(1);
                continue;
            }
            printf("fota_begin-->connect ok!\r\n");
            char *pbuf = (char *) zalloc(512);
            if (pbuf == NULL) {
                printf("no hay memoria para asignar al buffer\n");
                notificarOta(datosApp, OTA_ERROR_MEMORIA);
                system_upgrade_flag_set(UPGRADE_FLAG_IDLE);
                upgrade_recycle(1);
                continue;
            }
            datosOta = (char*) zalloc(512);

            if (datosOta == NULL) {
                printf("No hay memoria para asignar a OTA\n");
                system_upgrade_flag_set(UPGRADE_FLAG_IDLE);
                free(pbuf);
                upgrade_recycle(1);
                return;
            }
            if (system_upgrade_userbin_check() == UPGRADE_FW_BIN1) {
                sprintf(datosOta, "http://%s%s2-%d.bin", datosApp->ota.url, datosApp->ota.file, datosApp->ota.swVersion);
            } else if (system_upgrade_userbin_check() == UPGRADE_FW_BIN2) {
                sprintf(datosOta, "http://%s%s1-%d.bin", datosApp->ota.url,datosApp->ota.file, datosApp->ota.swVersion);
            }
            
            printf("fota_begin--> longitud de la url :%d", strlen(datosOta));
            sprintf(pbuf,"GET %s HTTP/1.0\r\nHost: \"%s\":%d\r\n"pheadbuffer"", datosOta, datosApp->ota.server, datosApp->ota.puerto);
            printf("fota_begin--> longitud de pbuf :%d\n", strlen(datosOta));
            free(datosOta);
            printf("fota_begin--> %s\n", pbuf);
            if (write(sta_socket,pbuf,strlen(pbuf)+1) < 0) {
                close(sta_socket);
                printf("send fail\n");
                notificarOta(datosApp, OTA_FALLO_CONEXION);
                free(pbuf);
                system_upgrade_flag_set(UPGRADE_FLAG_IDLE);
                upgrade_recycle(1);
                continue;
            }
            printf("send success\n");
            free(pbuf);

            // Notificamos la descarga desde el servidor
            notificarOta(datosApp, OTA_DESCARGA_FIRMWARE);
            vTaskDelay(ESPERA);

            while ((recbytes = read(sta_socket, recv_buf, 1460)) >= 0) {
                if (recbytes != 0) {
                    //printf("fota_begin--> leyendo...\n");
                    upgrade_download(sta_socket, recv_buf, recbytes, datosApp);
                }
            }
            printf("recbytes = %d\n", recbytes);
            if (recbytes < 0) {
                printf("read data fail!\r\n");
                notificarOta(datosApp, OTA_DATOS_CORRUPTOS);
                //os_timer_disarm(&noficacionUpgradeOta);
                close(sta_socket);
                system_upgrade_flag_set(UPGRADE_FLAG_IDLE);
                upgrade_recycle(1);
                continue;
            }
        } else {
            printf("fota_begin--> mensaje mal procesado\n");
            continue;
        }
    }
    
}

/******************************************************************************
 * FunctionName : fota_begin
 * Description  : ota_task function
 * Parameters   : task param
 * Returns      : none
 *******************************************************************************/
void tareaFota(DATOS_APLICACION *datosApp)
{
            //printf("tareaFota--> Se crea tarea\n");
            if (ota_task_handle == NULL) {
                system_upgrade_init();
                xTaskCreate(fota_begin, "fota_task", 1024, (void*) datosApp, 1, ota_task_handle);
                
            }
            
            
}

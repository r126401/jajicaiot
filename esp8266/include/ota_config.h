#ifndef __OTA_CONFIG_H__
#define __OTA_CONFIG_H__

#define DEMO_SERVER "192.168.1.55"
#define DEMO_SERVER_PORT 80

#define DEMO_WIFI_SSID     "Xiaomi_6324"
#define DEMO_WIFI_PASSWORD  "calbdtre"

#define OTA_TIMEOUT 120000  //120000 ms
//#define ESPERA 10 / portTICK_RATE_MS
#define ESPERA 200
typedef enum ESTADO_OTA {
    
    OTA_ERROR,
            OTA_DESCARGA_FIRMWARE,
            OTA_BORRANDO_SECTORES,
            OTA_COPIANDO_SECTORES,
            OTA_UPGRADE_FINALIZADO,
            OTA_FALLO_CONEXION,
            OTA_DATOS_CORRUPTOS,
            OTA_PAQUETES_ERRONEOS,
            OTA_CRC_ERRONEO,
            OTA_ERROR_MEMORIA
} ESTADO_OTA;

#define pheadbuffer "Connection: keep-alive\r\n\
Cache-Control: no-cache\r\n\
User-Agent: Mozilla/5.0 (Windows NT 5.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/30.0.1599.101 Safari/537.36 \r\n\
Accept: */*\r\n\
Accept-Encoding: gzip,deflate,sdch\r\n\
Accept-Language: zh-CN,zh;q=0.8\r\n\r\n"


#endif


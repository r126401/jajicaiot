#ifndef __USER_CONFIG_H__
#define __USER_CONFIG_H__

#define DO_DEBUG						//uncomment this to output basic debug level msgs on TxD

#ifdef DO_DEBUG
#define DBG(...)			printf( __VA_ARGS__ )
#define NOTIF(...)		        printf( __VA_ARGS__ )
//#define LCD
#else
#define DBG printf
#endif

#include "wifi.h"
#include "mqtt.h"
#include "ntp.h"
#include "programmer.h"

//#define RAND_0_TO_X(x)			( (uint32_t)os_random() / ((~(uint32_t)0)/(x)) ) //get uint32_t random number 0..x (including)

enum ESTADO_APP {
    CLOCK_ERROR,
    NORMAL_AUTO,
    NORMAL_MANUAL,
    
    
};

typedef struct OTADATA {
    char server[25];
    char url[75];
    uint8 puerto;
    char file[25];
    
    
}OTADATA;


typedef struct DATOS_APLICACION {
    
    int salvado;
    uint8 ActiveConf;
    MQTT_PARAM parametrosMqtt;
    struct NTP_CLOCK clock;
    struct TIME_PROGRAM *programacion;
    uint8 nProgramacion;
    PROG_STATE estadoProgramacion;
    OTADATA ota;
    
    
}DATOS_APLICACION;


DATOS_APLICACION datosApp;


void ejecutarAccionPrograma(DATOS_APLICACION *datosApp);







#endif

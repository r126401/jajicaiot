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
#include "cJSON.h"

xTaskHandle manejadorBeat;
xTaskHandle manejadorMqtt;
//#define DEVICE_TYPE 0

enum TIPO_ALARMA {
    ALARMA_MQTT,
    ALARMA_WIFI,
    ALARMA_NTP,
    ALARMA_HORA,
    ALARMA_OTA
}TIPO_ALARMA;

enum ESTADO_ALARMA {
    ALARMA_INDETERMINADA = -1,
    ALARMA_NO_ACTIVA,
    ALARMA_ACTIVA,
    
}ESTADO_ALARMA;

enum NOTIFICAR_ALARMA {
    SI,
    NO
}NOTIFICAR_ALARMA;

typedef struct ALARMAS {
    
    enum ESTADO_ALARMA mqtt;
    enum ESTADO_ALARMA ntp;
    enum ESTADO_ALARMA hora;
    enum ESTADO_ALARMA wifi;
    enum ESTADO_ALARMA ota;
    
}ALARMAS;

ALARMAS alarmas;
//#define RAND_0_TO_X(x)			( (uint32_t)os_random() / ((~(uint32_t)0)/(x)) ) //get uint32_t random number 0..x (including)
/*
#define APP_NAME "iotOnOffApp"
#define DEVICE_TYPE 0
*/
enum ESTADO_APP {
    NORMAL_AUTO,
    NORMAL_AUTOMAN,
    NORMAL_MANUAL,
    NORMAL_ARRANCANDO,
    NORMAL_SIN_PROGRAMACION,
    UPGRADE_EN_PROGRESO
    
    
    
} ESTADO_APP;

enum MOMENTO { ARRANQUE, CAMBIO_PROGRAMA, MEDIA_NOCHE, INDIFERENTE, OPERACION_COMANDO, A_PASADO=-1, DEFECTO=-2 };

typedef struct OTADATA {
    char server[100];
    char url[150];
    uint8 puerto;
    char file[50];
    int swVersion;
    
    
}OTADATA;


enum TIPO_INFORME {
    ARRANQUE_APLICACION,
    ACTUACION_RELE_LOCAL,
    ACTUACION_RELE_REMOTO,
    UPGRADE_FIRMWARE_FOTA,
    CAMBIO_DE_PROGRAMA,
    COMANDO_APLICACION,
    CAMBIO_TEMPERATURA,
    ESTADO,
    RELE_TEMPORIZADO,
    INFORME_ALARMA,
    CAMBIO_UMBRAL_TEMPERATURA,
    ERROR
}TIPO_INFORME;


typedef struct DATOS_APLICACION {
    
    uint8 tipoDispositivo;
    int salvado;
    uint8 ActiveConf;
    MQTT_PARAM parametrosMqtt;
    struct NTP_CLOCK clock;
    struct TIME_PROGRAM *programacion;
    uint8 nProgramacion;
    uint8 nProgramaCandidato;
    PROG_STATE estadoProgramacion;
    OTADATA ota;
    bool botonPulsado;
    //Estado de la aplicacion
    enum ESTADO_APP estadoApp;
    
#ifdef TERMOSTATO
    
    float tempActual;
    float tempUmbral;
    float tempUmbralDefecto;
    float humedad;
    float margenTemperatura;
    uint8 reintentosLectura;
    uint8 intervaloReintentos;
    uint8 intervaloLectura; 
    float calibrado;    

#endif
    
}DATOS_APLICACION;


DATOS_APLICACION datosApp;
int uptime;


static os_timer_t notificacionWifi;
static os_timer_t noficacionSmartConfig;
static os_timer_t noficacionUpgradeOta;



int appUser_ejecutarAccionPrograma(DATOS_APLICACION *datosApp, int programaActual);
void appUser_ejecucionAccionTemporizada(void *datosApp);
int appUser_ejecutarAccionProgramaDefecto(DATOS_APLICACION *datosApp);
void appUser_notificarWifiConectando();
void appUser_notificarWifiConectado();
void appUser_notificarWifiDesconectado();
void appUser_notificarBrokerDesconectado();
void appUser_notificarBrokerConectado();
void appUser_notificarSmartconfig();
void appUser_notificarSmartconfigStop();
void appUser_inicializarAplicacion(DATOS_APLICACION *datosApp);
void appUser_accionesOta(DATOS_APLICACION *datosApp);
void appUser_notificarUpgradeOta();






#endif

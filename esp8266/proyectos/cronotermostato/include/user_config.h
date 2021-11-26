#ifndef __USER_CONFIG_H__
#define __USER_CONFIG_H__

#define DO_DEBUG						//uncomment this to output basic debug level msgs on TxD

#ifdef DO_DEBUG
#define DBG(...)			printf( __VA_ARGS__ )
#define NOTIF(...)		        printf( __VA_ARGS__ )

#else
#define DBG printf
#endif
#define LCD
#include "wifi.h"
#include "mqtt.h"
#include "ntp.h"
#include "programmer.h"

//#define RAND_0_TO_X(x)			( (uint32_t)os_random() / ((~(uint32_t)0)/(x)) ) //get uint32_t random number 0..x (including)

// Cola para la publicacion de mensajes.
xQueueHandle publish_queue;


enum ESTADO_APP {
    CLOCK_ERROR,
    NORMAL,
    SETTINGS_CLOCK,
    SETTINGS_CLOCK_DAY,
    SETTINGS_CLOCK_MONTH,
    SETTINGS_CLOCK_YEAR,
    SETTINGS_CLOCK_HOUR,
    SETTINGS_CLOCK_MIN,
    SETTINGS_CLOCK_CONFIRM,
    SETTINGS_SMARTCONFIG,
    SETTINGS_SMARTCONFIG_FIND_CHANNEL,
    SETTINGS_RESET,
    SETTINGS_FACTORY

    
    
    
    
};

enum ESTADO_RELE { APAGADO=0, ENCENDIDO=1 };
enum ENVIO_LECTURA { SIEMPRE, CUANDO_CAMBIE, NUNCA };
enum MODO_FUNCIONAMIENTO { AUTO, AUTOMAN, MANUAL, ON, OFF};
enum MOMENTO { ARRANQUE, CAMBIO_PROGRAMA, MEDIA_NOCHE, INDIFERENTE, A_PASADO=-1, DEFECTO=-2 };

typedef struct DATOS_APLICACION {
    
    int salvado;
    uint8 ActiveConf;
    MQTT_PARAM parametrosMqtt;
    struct NTP_CLOCK clock;
    struct TIME_PROGRAM *programacion;
    uint8 nProgramacion;
    PROG_STATE estadoProgramacion;
    
    float tempActual;
    float tempUmbral;
    float tempUmbralDefecto;
    float humedad;
    float margenTemperatura;
    uint8 reintentosLectura;
    uint8 intervaloReintentos;
    uint8 intervaloLectura;
    enum ESTADO_RELE rele;
    enum ESTADO_APP estadoApp;
    enum ENVIO_LECTURA condicionesEnvio;
    time_t fechaLectura;
    enum MODO_FUNCIONAMIENTO modo;
    float incrementoUmbral;
    bool botonPulsado;
    struct tm fechaTemporal;
    float calibrado;
    
    
    
    
}DATOS_APLICACION;


DATOS_APLICACION datosApp;


int ejecutarAccionPrograma(DATOS_APLICACION *datosApp, enum MOMENTO momento);







#endif

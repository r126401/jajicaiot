/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   dialogoJson.h
 * Author: emilio
 *
 * Created on 16 de septiembre de 2017, 21:39
 */

/**
 * @brief Libreria que implementa todo el dialogo con el usuario.
 * La comunicacion se produce por el mismo protocolo mqtt.
 * El cliente envia las peticiones por el topic definido (/comandos) y el servidor responde
 * por el topic /publicacion.
 * El cliente ha de enviar una peticion con el fichero json previsto.
 * El servidor responde con la misma clave a la peticion del cliente.
 */



#ifndef DIALOGOJSON_H
#define DIALOGOJSON_H

#include "esp_common.h"
#include <stdio.h>
#include <stdlib.h>
#include "cJSON.h"
#include <time.h>
#include <stdint.h>
#include <string.h>
//#include "mqtt.h"
//#include "ntp.h"
//#include "programmer.h"
#include "user_config.h"
#define uint8 uint8_t


/***************Etiquetas****************/





/**
 @brief Etiquetas que componen todo el dialogo json.
 */

#define REINTENTOS_LECTURA        "intentosLectura"
#define INTERVALO_REINTENTOS           "intervaloReintentos"
#define INTERVALO_LECTURA         "intervaloLectura"
#define MARGEN_TEMPERATURA          "margenTemperatura"
#define RELE                        "rele"    
#define TEMPERATURA                 "temperatura"
#define HUMEDAD                     "humedad"
#define UMBRAL_TEMPERATURA          "temperaturaUmbral"
#define UMBRAL_SIGUIENTE            "temperaturaUmbralSig"
#define FECHA_ULTIMA_LECTURA        "ultimaLectura"
#define REPORTE_LECTURA             "reporteLectura"
#define UMBRAL_DEFECTO              "umbralDefecto"
#define MODO                        "modoFuncionamiento"
#define LECTURA                     "lectura"
#define CALIBRAR_SENSOR             "calibrar"
#define CALIBRADO                   "valorCalibrado"


#define YEAR        "year"
#define MONTH       "month"
#define DAY         "day"
#define HOUR        "hour"
#define MINUTE      "minute"
#define SECOND      "second"
#define WEEKDAY     "weekDay"
#define TIME_VALID  "timeValid"
#define TIME_T      "time_t"
#define NTP_TIMEZONE   "timeZone"
#define NTP_SUMMER_TIME "summerTime"
#define NTP_SERVER_ACTIVE   "ntpServerActive"
#define NTP_SERVER  "ntpServer"
#define SPARES_NTP  "sparesNtp"
#define SPARE       "spare"
#define SPARE_INDEX "spareIndex"
#define CLOCK_STATE "clock_state"
#define NTP_TIME_EXP    "ntpTimeExp"
#define NTP_TIMEOUT "ntpTimeout"
#define NTP_SYNC    "ntpSync"
#define DATE        "date"

#define PROGRAM         "program"
#define PROGRAM_ID      "programId"
#define NEXT_PROGRAM_ID "nextProgramId"
#define NEW_PROGRAM_ID  "newProgramId"
#define PROGRAM_TYPE    "programType"
#define PROGRAM_STATE   "programState"
#define PROGRAM_TIME    "programTime"
#define PROGRAMMER_STATE  "programmerState"
#define NUMBER_PROGRAMS   "programNumbers"  

#define MQTT_PARAMETER  "mqttParam"
#define MQTT_BROKER     "mqttBroker"
#define MQTT_PORT       "mqttPort"
#define MQTT_USER       "mqttUser"
#define MQTT_PASS       "mqttPass"
#define MQTT_PREFIX     "mqttPrefix"
#define MQTT_PUBLISH    "mqttPublish"
#define MQTT_SUBSCRIBE  "mqttSubscribe"

#define COMANDO             "comando"
#define DLG_COMANDO         "dlgComando"
#define DLG_FECHA           "dlgFecha"
#define DLG_KEY             "dlgKey"
#define DLG_PARAMETRO       "dlgParametro"
#define DLG_RESPUESTA       "dlgRespuesta"
#define DLG_COD_RESPUESTA   "dlgResultCode"
#define DLG_OBSERVACIONES   "dlgObservaciones"
#define DLG_OK              "ok"
#define DLG_NOK             "nok"
#define DLG_OBSERVACIONES_PROGRAMACION_OK           "Programacion insertada"
#define DLG_OBSERVACIONES_PROGRAMACION_NOK          "Programacion erronea o duplicada"
#define DLG_OBSERVACIONES_BORRAR_PROGRAMACION_OK    "Programacion borrada ok"
#define DLG_OBSERVACIONES_BORRAR_PROGRAMACION_NOK   "Programacion no encontrada"
#define DLG_OBSERVACIONES_MODIFICAR_PROGRAMACION_OK    "Programacion modificada"
#define DLG_OBSERVACIONES_MODIFICAR_PROGRAMACION_NOK    "Programacion no modificada"
#define DLG_OBSERVACIONES_MOD_CLOCK_OK                  "Configuracion de reloj modificada"
#define DLG_OBSERVACIONES_MOD_CLOCK_NOK                 "Error en configuracion de reloj"
#define DLG_OBSERVACIONES_CALIBRADO_OK                  "Sensor de temperatura calibrado"
#define DLG_OBSERVACIONES_CALIBRADO_NOK                  "No se ha podido calibrar el sensor"

#define DLG_OK_CODE     200
#define DLG_NOK_CODE    400

#define CONF_WIFI           "confWifi"
#define CONF_WIFI_ESSID     "essidWifi"
#define CONF_WIFI_PASS      "passwordWifi"

#define DELAY_TIME_RESET 3000
#define NORMAL_RESET            "resetNormal"
#define FACTORY_RESET           "factoryReset"
#define DLG_OBSERVACIONES_RESET "reset normal se ejecutara en unos segundos"

#define SALVAR_CONF    "salvarConf"
#define BACKUP_CONF    "backupConf"
#define CARGAR_CONF    "cargarConf"
#define BACKUP_LOAD  "cargarBackup"

#define DLG_OBSERVACIONES_GUARDAR_CONF_OK "OK, ejecutar reset para aplicar conf."
#define DLG_OBSERVACIONES_GUARDAR_CONF_NOK "NOK, no se ha grabado la configuracion."
#define DLG_OBSERVACIONES_BACKUP_CONF_OK "OK, backup realizado."
#define DLG_OBSERVACIONES_BACKUP_CONF_NOK "NOK, no se ha grabado la configuracion en backup."
#define DLG_OBSERVACIONES_CARGAR_BACKUP_OK "OK, ejecutar reset para aplicar conf."
#define DLG_OBSERVACIONES_CARGAR_BACKUP_NOK "NOK, no se ha grabado la configuracion."
#define DLG_OBSERVACIONES_MODIFICAR_NTP_OK "Servidor modificado correctamente."
#define DLG_OBSERVACIONES_MODIFICAR_NTP_NOK "Error, servidor no modificiado"
#define DLG_OBSERVACIONES_SINCRONIZAR "Sincronizar. Chequear estado consultando el reloj."
#define DLG_OBSERVACIONES_MODIFICAR_APP_OK "Modificaciones en App realizadas con exito"
#define DLG_OBSERVACIONES_MODIFICAR_APP_NOK "Modificaciones en App realizadas sin exito"
#define DLG_OBSERVACIONES_COMANDO_NO_ENCONTRADO "Comando no encontrado"
#define DLG_OBSERVACIONES_COMANDO_NO_JSON "Error en la sintaxis del comando"
#define APP_PARAMS "configApp"
#define ACTIVE_CONF "configActive"





/**
 *@brief enum para definir los diferentes tipos de comandos.
 */
enum COMANDOS { CONSULTAR_CONF_APP, //ok
                CONSULTAR_CONF_WIFI, //ok
                CONSULTAR_CONF_MQTT, //ok
                CONSULTAR_CONF_RELOJ,//ok
                SINCRONIZAR_RELOJ,//ok
                MODIFICAR_SERVIDOR_NTP, //ok
                CONSULTAR_CONF_PROGRAMACION,//ok
                INSERTAR_PROGRAMACION, //ok
                MODIFICAR_PROGRAMACION,//ok
                BORRAR_PROGRAMACION,//ok
                EJECUTAR_RESET, //ok
                EJECUTAR_FACTORY_RESET, // OK
                MODIFICAR_CONF_APP,
                MODIFICAR_CONF_WIFI, //ok
                MODIFICAR_CONF_MQTT, //ok
                MODIFICAR_CONF_RELOJ, // ok
                EJECUTAR_TEST,
                ENVIAR_STATUS,
                ENVIAR_DATOS_APP,
                GRABAR_CONFIGURACION,//ok
                BACKUP_CONFIGURACION,//ok
                CARGAR_BACKUP,//ok
                SELECCIONAR_NTP_SERVER_ACTIVO, // ok
                PONER_EN_HORA, // OK
                SUBIR_UMBRAL,
                BAJAR_UMBRAL
};






/**
 * @brief visualiza en formato json la configuracion mqtt del dispositivo
 * @param mqttParam Estructura mqtt
 * @param root estructura json de la peticion del comando que se enlaza con la respuesta.
 * @param parte true si se quiere eliminar la cabecera json de mqtt.
 * @return puntero a la estructura completa de la peticion mas la respuesta.
 */

cJSON* jsonMqtt(struct MQTT_PARAM *mqttParam, cJSON *root, bool parte);



/**
 * @brief Visualiza la configuracion del reloj en formato json
 * @param clock puntero a la estructura del reloj de la aplicacion.
 * @param root estructura json de la peticion del comando que se enlaza con la respuesta.
 * @param parte true si se quiere eliminar la cabecera json de mqtt.
 * @return puntero a la estructura completa de la peticion mas la respuesta.
 */

cJSON* jsonClock(struct NTP_CLOCK *clock, cJSON* root, bool parte);
/**
  * @brief Construye un fichero json con la hora de la aplicacion
  *                                                    .
  * @attention 
  *
  * @param     *mqttParam: es la estructura de reloj de la aplicacion
  * @param     root: es una estructura json. Si es NULL, la aplicacion crea una nueva
  * estructura json. Si no es NULL, se añade la parte json a la que se pasa en este parametro.
  * @param     parte: Indica si se añade un item cabecera indicando la parte de la estructura o
  * se añade a root como atributos sin mas.  
  * 
  *
  * @return    NULL si no se ha podido crear correctamente la estructura
  * @return    puntero cJSON para usarlo posteriormente.
  * 
  */

cJSON* jsonFecha(struct NTP_CLOCK *clock, cJSON* root, bool parte);


/**
 * @brief Visualiza la configuracion wifi del dispositivo
 * @param root estructura json de la peticion del comando que se enlaza con la respuesta.
 * @param parte true si se quiere eliminar la cabecera json de mqtt.
 * @return puntero a la estructura completa de la peticion mas la respuesta.
 */

cJSON* jsonWifi(cJSON* root, bool parte);

/**
 * @brief Visualiza la estructura de programacion del dispositvo.
 * @param program puntero a la lista de programas
 * @param root estructura json de la peticion del comando que se enlaza con la respuesta.
 * @param parte parte true si se quiere eliminar la cabecera json.
 * @param element es el indice del elemento que se quiere visualizar.
 * @return puntero a la estructura completa de la peticion mas la respuesta.
 */

cJSON* jsonProgramacion(struct TIME_PROGRAM *program, cJSON *root, bool parte, uint8 element);


cJSON* consultarConfProgramacion(cJSON *comando, struct DATOS_APLICACION *datosApp, bool separar);


/**
 * @brief funcion principal que procesa el comando en formato json recibido.
 * La funcion envia la respuesta y libera todos los recursos asociados.
 * @param info es la peticion recibida desde el cliente.
 * @param datosApp es la estructura de la aplicacion.
 * @return puntero a la estructura completa de la peticion mas la respuesta.
 */

cJSON* analizarComando(char* info, struct DATOS_APLICACION *datosApp);

/**
 * @brief Modifica los parametros mqtt de la aplicacion.
 * Para que los datos se guarden permanentemente se ha de invocar a guardarConfiguracion.
 * @param root peticion del cliente en formato json.
 * @param datosApp es la estructura de la aplicacion.
 * @return puntero a la estructura completa de la peticion mas la respuesta.
 */
cJSON* modificarConfMqtt(cJSON* root, struct DATOS_APLICACION *datosApp);
/**
 * @brief Modifica la configuracion de reloj del dispositivo.
 * @param root peticion del cliente en formato json.
 * @param datosApp es la estructura de la aplicacion.
 * @return puntero a la estructura completa de la peticion mas la respuesta.
 */
cJSON* modificarConfClock(cJSON *root, struct DATOS_APLICACION *datosApp);
/**
 * @brief Mofifica un servidor de reserva para consultar ntp numerado en el 
 * fichero json por el indice del servidor a cambiar.
 * @param root peticion del cliente en formato json.
 * @param datosApp es la estructura de la aplicacion.
 * @return puntero a la estructura completa de la peticion mas la respuesta.
 */
cJSON* modificarServidorNtp(cJSON *root, DATOS_APLICACION *datosApp);
/**
 * @brief Cambia el servidor ntp activo para sincronizarse.
 * @param root peticion del cliente en formato json.
 * @param datosApp es la estructura de la aplicacion.
 * @return puntero a la estructura completa de la peticion mas la respuesta.
 */
cJSON* seleccionarNtpServerActive(cJSON *root, DATOS_APLICACION *datosApp);
/**
 * @brief Modifica la configuracion wifi del dispositivo
 * @param root peticion del cliente en formato json.
 * @return puntero a la estructura completa de la peticion mas la respuesta.
 */
cJSON* modificarConfWifi(cJSON* root);
/**
 * @brief Genera una consulta ntp mediante el servidor activo configurado para 
 * resincronizar manualmente el dispositivo.
 * @param root peticion del cliente en formato json.
 * @param datosApp es la estructura de la aplicacion.
 * @return puntero a la estructura completa de la peticion mas la respuesta.
 */
cJSON* sincronizarReloj(cJSON *root, struct DATOS_APLICACION *datosApp);
/**
 * @brief pone en hora el dispositivo manualmente sin necesidad de acceder a un servidor ntp.
 * @param root peticion del cliente en formato json.
 * @param datosApp es la estructura de la aplicacion.
 * @return puntero a la estructura completa de la peticion mas la respuesta.
 */
cJSON* ponerEnHora(cJSON *root, DATOS_APLICACION *datosApp);

/**
 * @brief provoca reset normal del dispositivo.
 * @param root peticion del cliente en formato json.
 * @param datosApp es la estructura de la aplicacion.
 * @return puntero a la estructura completa de la peticion mas la respuesta.
 * Se devuelve respuesta al cliente y se ejecuta el reset en 3 segundos.
 */
cJSON* ejecutarReset(cJSON *root, struct DATOS_APLICACION *datosApp);

/**
 * @brief provoca la carga de configuracion de defecto del dispositivo como puesta a nuevo.
 * @param root peticion del cliente en formato json.
 * @param datosApp es la estructura de la aplicacion.
 * @return puntero a la estructura completa de la peticion mas la respuesta.
 */
cJSON* ejecutarFactoryReset(cJSON *root, struct DATOS_APLICACION *datosApp);

/**
 * @brief Inserta un nuevo programa en la estructura TIME_PROGRAM.
 * @param root peticion del cliente en formato json.
 * @param datosApp es la estructura de la aplicacion.
 * @return puntero a la estructura completa de la peticion mas la respuesta.
 */
cJSON* insertarProgramacion(cJSON *root,struct DATOS_APLICACION *datosApp);
/**
 * @brief Borra un programa existente de la estructura TIME_PROGRAM
 * @param root peticion del cliente en formato json.
 * @param datosApp es la estructura de la aplicacion.
 * @return puntero a la estructura completa de la peticion mas la respuesta.
 */
cJSON* borrarProgramacion(cJSON *root,struct DATOS_APLICACION *datosApp);
/**
 * @brief Modifica un programa existente
 * @param root peticion del cliente en formato json.
 * @param datosApp es la estructura de la aplicacion.
 * @return puntero a la estructura completa de la peticion mas la respuesta.
 */
cJSON* modificarProgramacion(cJSON *root,struct DATOS_APLICACION *datosApp);
/**
 * @brief Da orden de grabar la configuraicon a la memoria principal
 * @param root peticion del cliente en formato json.
 * @param datosApp es la estructura de la aplicacion.
 * @return puntero a la estructura completa de la peticion mas la respuesta.
 */
cJSON* grabarConfiguracion(cJSON *root, struct DATOS_APLICACION *datosApp);
/**
 * @brief Realiza un backup de la configuracion de memoria en la zona de backup
 * @param root peticion del cliente en formato json.
 * @param datosApp es la estructura de la aplicacion.
 * @return puntero a la estructura completa de la peticion mas la respuesta.
 */
cJSON* backupConfiguracion(cJSON *root, struct DATOS_APLICACION *datosApp);
/**
 * @brief Carga la configuracion desde la memoria de backup
 * @param root peticion del cliente en formato json.
 * @param datosApp es la estructura de la aplicacion.
 * @return puntero a la estructura completa de la peticion mas la respuesta.
 */
cJSON* cargarBackup(cJSON *root, struct DATOS_APLICACION *datosApp);

/**
 * @brief Visualiza en formato json los parametros mas importantes de la aplicacion
 * que son comunes a cualquier aplicacion.
 * @param root peticion del cliente en formato json.
 * @param datosApp es la estructura de la aplicacion.
 * @return puntero a la estructura completa de la peticion mas la respuesta.
 */
cJSON* AppJsonComun(struct DATOS_APLICACION *datosApp, cJSON* root, bool parte);
/**
 * @brief visualiza la configuracion activa de la aplicacion
 * @param comando peticion del cliente en formato json.
 * @param datosApp es la estructura de la aplicacion.
 * @return puntero a la estructura completa de la peticion mas la respuesta.
 */
cJSON* consultarConfApp(cJSON *comando, DATOS_APLICACION *datosApp);

/**
 * @brief Ejecuta un reset normal a peticion del cliente sobre el dispositivo.
 * @param parametros
 */
void restartNormal(void* parametros);
char* colocarSeparadorProgramacion(char*respuesta);
cJSON* subirUmbralTemperatura(cJSON *root, DATOS_APLICACION *datosApp);
cJSON* bajarUmbralTemperatura(cJSON *root, DATOS_APLICACION *datosApp);

cJSON* comandoDesconocido(cJSON *root, char *msg);
#endif /* DIALOGOJSON_H */


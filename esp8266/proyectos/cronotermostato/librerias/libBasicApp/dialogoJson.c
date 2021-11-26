/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */


#include "dialogoJson.h"
#include "ntp.h"
#include "configuracion.h"
#include "aplication.h"
#ifdef LCD 
#include "appdisplay.h"
#endif

extern DATOS_APLICACION datosApp;





char* colocarSeparadorProgramacion(char*respuesta) {
    
    char*respSegmentada;
    respSegmentada = zalloc(strlen(respuesta) + 50);
    strcpy(respSegmentada, respuesta);
    strcat(respSegmentada, "\n-------------------------------\n");
    
    return respSegmentada;
    
    
}


cJSON* consultarConfProgramacion(cJSON *comando, DATOS_APLICACION *datosApp, bool separar) {

    char* respuesta;
    uint8 i;
    char *topic;
    cJSON *root;
    cJSON *nodoRespuesta;
    char* respSegmentada;
    cJSON *duplicado;
    
    
    if (datosApp->nProgramacion == 0) {
        cJSON_AddItemToObject(comando, DLG_RESPUESTA, nodoRespuesta = cJSON_CreateObject());
        cJSON_AddStringToObject(nodoRespuesta, DLG_COD_RESPUESTA, DLG_NOK);
        cJSON_AddStringToObject(nodoRespuesta, DLG_OBSERVACIONES, DLG_OBSERVACIONES_BORRAR_PROGRAMACION_NOK);
        return comando;
        //respuesta = cJSON_Print(comando);
        //sendMqttMessage(topic, respuesta);
        //free(respuesta);
    }    
    
    topic = zalloc(75*sizeof(char));
    strcpy(topic, datosApp->parametrosMqtt.prefix);
    strcat(topic, datosApp->parametrosMqtt.publish);

    for(i=0;i<datosApp->nProgramacion; i++) {
        


        //root = duplicarJson(comando);
        root = cJSON_Duplicate(comando, 1);
        if (root == NULL) {
            printf("Error al crear json duplicado");
        }
        printf("json %d duplicado\n", i);


        
        root = jsonProgramacion(datosApp->programacion, root, false,i);
        if (root == NULL) printf("consultarConfProgramacion-->json nulo\n");
        respuesta = cJSON_Print(root);
        cJSON_Delete(root);
        root = NULL;

        
        //Enviamos por mqtt la respuesta
        if(separar = true) {
            respSegmentada = colocarSeparadorProgramacion(respuesta);
            sendMqttMessage(topic, respSegmentada);
            free(respSegmentada);
            free(respuesta);
        } else {
            sendMqttMessage(topic, respuesta);
            //liberamos el texto de la peticion, la respuesta y el objeto cJSON
            free(respuesta);
            respuesta = NULL;
            //cJSON_Delete(root);
            //root=NULL;
            printf("fin de bucle %d\n", i);

        }

        

        
        

       
       
    }
    
    

    printf("liberando recursos\n");
    cJSON_Delete(comando);
    free(topic);    
    comando = NULL;



    
    return comando;
}








cJSON* analizarComando(char* info, DATOS_APLICACION *datosApp) {
    
    cJSON *root = NULL;
    cJSON *element = NULL;
    cJSON *campo = NULL;
    //1.- Analizar la peticion y vemos que comando nos piden.
    

    root = cJSON_Parse(info);
    if (root == NULL) {
        printf("analizarComando-->Error en fichero json antes de...\n %s", cJSON_GetErrorPtr());
        root = comandoDesconocido(root, DLG_OBSERVACIONES_COMANDO_NO_JSON);
        free(root);
        //return NULL;
        return root;
    }
    //2.- Analizamos el comando.
        //Buscamos el nodo comando dentro del json.
    //printf("analizarComando-->vamos a analizar el comando\n");
    element = cJSON_GetObjectItem(root, COMANDO);
    
    if(element == NULL) {
        printf("analizarComando-->No se ha recibido un comando\n");
        free(root);
        return NULL;
    }
    
    
    //Actualizamos el reloj de la respuesta
    //printf("analizarComando-->voy a modificar la fecha\n");
    campo = cJSON_GetObjectItem(element, DATE);
    //printf("analizarComando-->obtenido el elmento\n");
    if((campo != NULL) && (campo->type == cJSON_String) && (datosApp->clock.timeValid == true)) {
       //printf("analizarComando-->modifico la fecha\n");
       sprintf(campo->valuestring, "%02d/%02d/%d %02d:%02d:%02d",
               datosApp->clock.date.tm_mday,datosApp->clock.date.tm_mon+1, datosApp->clock.date.tm_year+1900,
               datosApp->clock.date.tm_hour, datosApp->clock.date.tm_min, datosApp->clock.date.tm_sec);
   }
    
   cJSON_AddNumberToObject(element, "time_t", datosApp->clock.time); 
    
    
    
    //3.- Nos aseguramos que el comando existe
    element = cJSON_GetObjectItem(element, DLG_COMANDO);
    
    if ((element == NULL) || (element->type != cJSON_Number)) {
        printf("analizarComando-->No se encuentra el comando en el json\n");
        root = comandoDesconocido(root, DLG_OBSERVACIONES_COMANDO_NO_ENCONTRADO);
        //free(root);
        //return NULL;
        return root;
    }

    
    //4.- En funcion del comando ejecutamos una cosa u otra
    switch (element->valueint) {
        case CONSULTAR_CONF_APP:
            printf("analizarComando-->Consultar confApp\n");
            root = AppJsonComun(datosApp, root, false);
            break;
        case CONSULTAR_CONF_MQTT: //probada ok
            //Visualizar la configuracion mqtt de aplicacion
            printf("analizarComando-->Consultar configuracion mqtt...\n");
            root = jsonMqtt(&(datosApp->parametrosMqtt), root, false);
            
            break;
        case CONSULTAR_CONF_RELOJ: //probado ok
            //Visualizar la configuracion del reloj
            printf("analizarComando-->Consultar configuracion reloj\n");
            root = jsonClock(&(datosApp->clock), root, false);
            break;
        case CONSULTAR_CONF_WIFI: // probado ok
            //Visualizar la configuracion wifi
            printf("analizarComando-->Consultar configuracion wifi\n");
            root = jsonWifi(root, false);
            break;
        case CONSULTAR_CONF_PROGRAMACION: // probado ok
            printf("analizarComando-->consultar Programacion\n");
            root = consultarConfProgramacion(root, datosApp, true);
            break;
        case MODIFICAR_CONF_MQTT: // probado ok
            root = modificarConfMqtt(root, datosApp);
            //Falta salvar a memoria la configuracion modificada
            break;
        case MODIFICAR_CONF_RELOJ: // probado ok
            root = modificarConfClock(root, datosApp);
            break;
        case SINCRONIZAR_RELOJ:
            root = sincronizarReloj(root, datosApp);
            break;
        case EJECUTAR_RESET: // probado ok
            root = ejecutarReset(root,datosApp);
            break;
        case EJECUTAR_FACTORY_RESET:
            root = ejecutarFactoryReset(root, datosApp);
            break;
        case INSERTAR_PROGRAMACION: // probado ok
            printf("analizarComando-->insertar programacion\n");
            root = insertarProgramacion(root, datosApp);
            ejecutarAccionPrograma(datosApp, INDIFERENTE);
            break;
        case BORRAR_PROGRAMACION: // probado ok
            printf("analizarComando-->borrar programacion\n");
            root = borrarProgramacion(root, datosApp);
            ejecutarAccionPrograma(datosApp, INDIFERENTE);
            break;
        case MODIFICAR_PROGRAMACION: // probado ok
            printf("analizarComando-->modificar programacion\n");
            root = modificarProgramacion(root, datosApp);
            ejecutarAccionPrograma(datosApp, INDIFERENTE);
            break;
        case GRABAR_CONFIGURACION: //probado ok
            printf("analizarComando-->Guardar configuracion\n");
            root = grabarConfiguracion(root, datosApp);
            break;
        case BACKUP_CONFIGURACION: //probado ok
            root = backupConfiguracion(root, datosApp);
            break;
        case CARGAR_BACKUP: //probado ok
            //Liberamos la anterior programacion
            free(datosApp->programacion);
            datosApp->nProgramacion=0;
            root = cargarBackup(root, datosApp);
            //provocamos la reevaluacion de la programacion
            datosApp->estadoProgramacion = INVALID_PROG;
            break;
        case MODIFICAR_SERVIDOR_NTP: //probado ok
            root = modificarServidorNtp(root, datosApp);
            break;
        case SELECCIONAR_NTP_SERVER_ACTIVO: //probado ok
            root = seleccionarNtpServerActive(root,datosApp);
            break;
        case PONER_EN_HORA:
            root = ponerEnHora(root, datosApp);
            break;
        case MODIFICAR_CONF_APP:
            root = modificarJsonAplicacion(root, datosApp);
            break;
        case SUBIR_UMBRAL:
            root = subirUmbralTemperatura(root, datosApp);
            break;
        case BAJAR_UMBRAL:
            root = bajarUmbralTemperatura(root, datosApp);
            break;
        case ENVIAR_DATOS_APP:
            root = jsonLectura(datosApp, root);
            break;
        default:
            comandoDesconocido(root, DLG_OBSERVACIONES_COMANDO_NO_ENCONTRADO);
            break;
            //comando no encontrado
                    
    }
    //Despues de ejecutar el comando devolvemos el objeto json para prepara la respuesta

    
    
    return root;
    
    
}



cJSON* jsonMqtt(struct MQTT_PARAM *mqttParam, cJSON *root, bool parte) {

    cJSON *nodo;
    char texto[20];


    if (root == NULL) {
       root = cJSON_CreateObject(); 
       
    }
    
    if(parte == false) {
        cJSON_AddItemToObject(root, MQTT_PARAMETER, nodo = cJSON_CreateObject());
    } else {
        nodo = root;
    }
    
    
    cJSON_AddStringToObject(nodo, MQTT_BROKER, mqttParam->broker);
    cJSON_AddNumberToObject(nodo, MQTT_PORT, mqttParam->port);
    cJSON_AddStringToObject(nodo, MQTT_USER, mqttParam->user);
    cJSON_AddStringToObject(nodo, MQTT_PASS, mqttParam->password);
    cJSON_AddStringToObject(nodo, MQTT_PREFIX, mqttParam->prefix);
    cJSON_AddStringToObject(nodo, MQTT_PUBLISH, mqttParam->publish);
    cJSON_AddStringToObject(nodo, MQTT_SUBSCRIBE, mqttParam->subscribe);
    
    return root;
    
    
}


cJSON* jsonProgramacion(struct TIME_PROGRAM *program, cJSON *root, bool parte, uint8 element) {
    
    cJSON *nodo;



    if (root == NULL) {
       root = cJSON_CreateObject(); 
       
    }
    
    if(parte == false) {
        cJSON_AddItemToObject(root, PROGRAM, nodo = cJSON_CreateObject());
    }else {
        nodo = root;
    }
  

    cJSON_AddStringToObject(nodo, PROGRAM_ID, program[element].idPrograma);
    cJSON_AddNumberToObject(nodo, PROGRAM_TYPE, program[element].tipo);
    cJSON_AddNumberToObject(nodo, HOUR, program[element].programacion.tm_hour);
    cJSON_AddNumberToObject(nodo, MINUTE, program[element].programacion.tm_min);
    cJSON_AddNumberToObject(nodo, SECOND, program[element].programacion.tm_sec);
    if( program[element].tipo == SEMANAL) {
        cJSON_AddNumberToObject(nodo, WEEKDAY, program[element].programacion.tm_wday);
        
    }
    cJSON_AddBoolToObject(nodo, PROGRAM_STATE, program[element].activo );
    printf("Hola %d\n", element);
    cJSON_AddNumberToObject(nodo, YEAR, program[element].programacion.tm_year);
    cJSON_AddNumberToObject(nodo, MONTH, program[element].programacion.tm_mon);
    cJSON_AddNumberToObject(nodo, DAY, program[element].programacion.tm_mday);
    cJSON_AddNumberToObject(nodo, "diasAno", program[element].programacion.tm_yday);
    cJSON_AddNumberToObject(nodo, WEEKDAY, program[element].programacion.tm_wday);
    cJSON_AddNumberToObject(nodo, "time_t", program[element].programa);
    cJSON_AddNumberToObject(nodo, "diferencia", program[element].programa - datosApp.clock.time);
    cJSON_AddNumberToObject(nodo, UMBRAL_TEMPERATURA, program[element].umbral );
    printf("Adios %d\n", element);
    

    

        
    
    return root;
}



cJSON* jsonFecha(struct NTP_CLOCK *clock, cJSON* root, bool parte) {
    
    //cJSON *root;
    cJSON *nodo;
    char texto[20];

    
    
    if (root == NULL) {
       root = cJSON_CreateObject(); 
       
    }
    
    if(parte == false) {
        cJSON_AddItemToObject(root, DATE, nodo = cJSON_CreateObject());
    }else {
        nodo = root;
    }
    
    sprintf(texto, "%02d/%02d/%d %02d:%02d:%02d",
            clock->date.tm_mday, clock->date.tm_mon+1, clock->date.tm_year + 1900,
            clock->date.tm_hour, clock->date.tm_min, clock->date.tm_sec);
            
    cJSON_AddStringToObject(nodo, DATE,texto );
    
    nodo = jsonClock(clock, nodo, true);
return root;    
    
    
}





cJSON* jsonClock(struct NTP_CLOCK *clock, cJSON* root, bool parte) {

    cJSON *nodo=NULL;
    char *texto = NULL;
    cJSON *arrayServers=NULL;
    cJSON *server=NULL;
    uint8 i;

    

    if (root == NULL) {
       root = cJSON_CreateObject(); 
       
    }
    
    if(parte == false) {
        cJSON_AddItemToObject(root, DATE, nodo = cJSON_CreateObject());
    } else {
        nodo = root;
    }
    
    cJSON_AddBoolToObject(nodo, TIME_VALID, clock->timeValid );
    cJSON_AddNumberToObject(nodo, CLOCK_STATE, clock->estado );
    cJSON_AddNumberToObject(nodo, NTP_TIME_EXP, clock->ntpTimeExp );
    cJSON_AddNumberToObject(nodo, NTP_TIMEZONE, clock->ntpTimeZone );
    cJSON_AddNumberToObject(nodo, NTP_TIMEOUT, clock->ntpTimeOut );
    //visualizamos el servidor activo
    texto = zalloc(20*sizeof(char));
    sprintf(texto, "%d.%d.%d.%d", clock->ntpServerActive[0], clock->ntpServerActive[1],clock->ntpServerActive[2],clock->ntpServerActive[3]);
    cJSON_AddStringToObject(nodo, NTP_SERVER_ACTIVE, texto);
    free(texto);
    //Visualizamos los servidores de reserva

    ;
    cJSON_AddItemToObject(nodo,SPARES_NTP, arrayServers = cJSON_CreateArray());
    for(i=0;i<5;i++) {
        cJSON_AddItemToArray(arrayServers,server = cJSON_CreateObject());
        texto = zalloc(20*sizeof(char));
        sprintf(texto, "%d.%d.%d.%d", clock->ntpServer[i][0],
                
                clock->ntpServer[i][1],
                clock->ntpServer[i][2],
                clock->ntpServer[i][3]);
        cJSON_AddStringToObject(server,SPARE, texto);
        free(texto);
        
    }
    

    
    
    
    //cJSON_AddNumberToObject(nodo, TIME_T, clock->time );
    
  return root;  
    
}

cJSON* jsonWifi(cJSON* root, bool parte) {
   
    cJSON *nodo;
    struct station_config config;



    if (root == NULL) {
       root = cJSON_CreateObject(); 
       
    }
    
    if(parte == false) {
        cJSON_AddItemToObject(root, CONF_WIFI, nodo = cJSON_CreateObject());
    } else {
        nodo = root;
    }
    //wifi_station_get_config_default(config);
    strcpy((char*) config.ssid, "Salon");
    strcpy((char*) config.password, "calbdtre");
    cJSON_AddStringToObject(nodo, CONF_WIFI_ESSID, (char*) config.ssid);
    cJSON_AddStringToObject(nodo, CONF_WIFI_PASS, (char*) config.password);
    
    return root;
    
    
}


cJSON* modificarConfMqtt(cJSON *root, struct DATOS_APLICACION *datosApp) {
      
   cJSON *nodo;
   cJSON *campo;
   cJSON *nodoRespuesta;
   uint8 nModificaciones=0;
   
   nodo = cJSON_GetObjectItem(root, MQTT_PARAMETER);
   if(nodo == NULL) {
       nodo = root;
   }
   
   campo = cJSON_GetObjectItem(nodo, MQTT_BROKER);
   if((campo != NULL) && (campo->type == cJSON_String)) {
       strcpy(datosApp->parametrosMqtt.broker, campo->valuestring);
       nModificaciones++;
   }
   
   campo = cJSON_GetObjectItem(nodo, MQTT_PORT);
   if((campo != NULL ) && (campo->type == cJSON_Number)) {
       datosApp->parametrosMqtt.port = campo->valueint;
       nModificaciones++;
   }

   campo = cJSON_GetObjectItem(nodo, MQTT_USER);
   if((campo != NULL) && (campo->type == cJSON_String)) {
       strcpy(datosApp->parametrosMqtt.user, campo->valuestring);
       nModificaciones++;
   }
   
   campo = cJSON_GetObjectItem(nodo, MQTT_PASS);
   if((campo != NULL) && (campo->type == cJSON_String)) {
       strcpy(datosApp->parametrosMqtt.password, campo->valuestring);
       nModificaciones++;
   }

   campo = cJSON_GetObjectItem(nodo, MQTT_PREFIX);
   if((campo != NULL) && (campo->type == cJSON_String)) {
       strcpy(datosApp->parametrosMqtt.prefix, campo->valuestring);
       nModificaciones++;
   }
   
   campo = cJSON_GetObjectItem(nodo, MQTT_PUBLISH);
   if((campo != NULL) && (campo->type == cJSON_String)) {
       printf("Se modifica el valor de %s a %s", datosApp->parametrosMqtt.publish, campo->valuestring);
       strcpy(datosApp->parametrosMqtt.publish, campo->valuestring);
       nModificaciones++;
       
   }
   campo = cJSON_GetObjectItem(nodo, MQTT_SUBSCRIBE);
   if((campo != NULL) && (campo->type == cJSON_String)) {
       printf("Se modifica el valor de %s a %s", datosApp->parametrosMqtt.subscribe, campo->valuestring);
       strcpy(datosApp->parametrosMqtt.subscribe, campo->valuestring);
       nModificaciones++;
       
   }
   
   cJSON_AddItemToObject(root, DLG_RESPUESTA, nodoRespuesta = cJSON_CreateObject());
   if (nModificaciones == 0) {
       cJSON_AddNumberToObject(nodoRespuesta, DLG_COD_RESPUESTA, DLG_NOK_CODE);
       cJSON_AddStringToObject(nodoRespuesta, DLG_OBSERVACIONES, DLG_OBSERVACIONES_MOD_CLOCK_NOK);
   } else {
       cJSON_AddNumberToObject(nodoRespuesta, DLG_COD_RESPUESTA, DLG_OK_CODE);
       cJSON_AddStringToObject(nodoRespuesta, DLG_OBSERVACIONES, DLG_OBSERVACIONES_MOD_CLOCK_OK);
       
   }




   
   //root = consultarConfMqtt(root, datosApp);
    //Faltarian salvar los datos
    return root;
}

cJSON* modificarConfClock(cJSON *root, struct DATOS_APLICACION *datosApp) {
      
   cJSON *nodo;
   cJSON *campo;
   cJSON *nodoRespuesta;
   uint8 i=0;
   char* ipNumber;
   uint8 nModificaciones=0;
   
   nodo = cJSON_GetObjectItem(root, DATE);
   if(nodo == NULL) {
       nodo = root;
   }
   
   campo = cJSON_GetObjectItem(nodo, YEAR);
   if((campo != NULL) && (campo->type == cJSON_Number)) {
       datosApp->clock.date.tm_year = campo->valueint;
       nModificaciones++;
   }
   
   campo = cJSON_GetObjectItem(nodo, MONTH);
   if((campo != NULL) && (campo->type == cJSON_Number)) {
       datosApp->clock.date.tm_mon = campo->valueint;
       nModificaciones++;
   }
   campo = cJSON_GetObjectItem(nodo, DAY);
   if((campo != NULL) && (campo->type == cJSON_Number)) {
       datosApp->clock.date.tm_mday = campo->valueint;
       nModificaciones++;
   }
   campo = cJSON_GetObjectItem(nodo, HOUR);
   if((campo != NULL) && (campo->type == cJSON_Number)) {
       datosApp->clock.date.tm_hour = campo->valueint;
       nModificaciones++;
   }
   campo = cJSON_GetObjectItem(nodo, MINUTE);
   if((campo != NULL) && (campo->type == cJSON_Number)) {
       datosApp->clock.date.tm_min = campo->valueint;
       nModificaciones++;
   }
   campo = cJSON_GetObjectItem(nodo, TIME_VALID);
   if(campo != NULL)  {
       datosApp->clock.timeValid = campo->valueint;
       nModificaciones++;
   }
   
   campo = cJSON_GetObjectItem(nodo, TIME_T);
   if((campo != NULL) && (campo->type == cJSON_Number)) {
       datosApp->clock.time = campo->valueint;
       nModificaciones++;
   }
   
   campo = cJSON_GetObjectItem(nodo, NTP_TIMEZONE);
   if((campo != NULL) && (campo->type == cJSON_Number)) {
       datosApp->clock.ntpTimeZone = campo->valueint;
       nModificaciones++;
   }

   campo = cJSON_GetObjectItem(nodo, NTP_SERVER_ACTIVE);
   if((campo != NULL) && (campo->type == cJSON_String)) {

       ipNumber= strtok(campo->valuestring, ".");
       strcpy(&(datosApp->clock.ntpServerActive[i]), ipNumber);
       for (i=1;i<4;i++) {
           ipNumber = strtok(NULL,".");
           strcpy(&(datosApp->clock.ntpServerActive[i]), ipNumber);
       }
       nModificaciones++;
           
          
   }
   
   campo = cJSON_GetObjectItem(nodo, CLOCK_STATE);
   if((campo != NULL) && (campo->type == cJSON_Number)) {
       datosApp->clock.estado = campo->valueint;
       nModificaciones++;
   }

   campo = cJSON_GetObjectItem(nodo, NTP_TIME_EXP);
   if((campo != NULL) && (campo->type == cJSON_Number)) {
       datosApp->clock.ntpTimeExp = campo->valueint;
       nModificaciones++;
   }

   campo = cJSON_GetObjectItem(nodo, NTP_TIMEOUT);
   if((campo != NULL) && (campo->type == cJSON_Number)) {
       datosApp->clock.ntpTimeOut = campo->valueint;
       nModificaciones++;
   }
   
   cJSON_AddItemToObject(root, DLG_RESPUESTA, nodoRespuesta = cJSON_CreateObject());
   //root = jsonProgramacion(datosApp->programacion, root, false,datosApp->nProgramacion-1);
   
   if (nModificaciones == 0) {
       cJSON_AddNumberToObject(nodoRespuesta, DLG_COD_RESPUESTA, DLG_NOK_CODE);
       cJSON_AddStringToObject(nodoRespuesta, DLG_OBSERVACIONES, DLG_OBSERVACIONES_MOD_CLOCK_NOK);
   } else {
       cJSON_AddNumberToObject(nodoRespuesta, DLG_COD_RESPUESTA, DLG_OK_CODE);
       cJSON_AddStringToObject(nodoRespuesta, DLG_OBSERVACIONES, DLG_OBSERVACIONES_MOD_CLOCK_OK);
       
   }

   
   //root = consultarConfClock(root, datosApp);
    //Faltarian salvar los datos
    return root;
}


cJSON* modificarConfWifi(cJSON* root) {

   cJSON *nodo;
   cJSON *campo;
   struct station_config *config = (struct station_config *)zalloc(sizeof(struct station_config));
   
   nodo = cJSON_GetObjectItem(root, CONF_WIFI);
   if(nodo == NULL) {
       nodo = root;
   }

   campo = cJSON_GetObjectItem(nodo, CONF_WIFI_ESSID);
   if((campo != NULL) && (campo->type == cJSON_String)) {
       sprintf(config->bssid, campo->valuestring);
   }

   campo = cJSON_GetObjectItem(nodo, CONF_WIFI_PASS);
   if((campo != NULL) && (campo->type == cJSON_String)) {
       sprintf(config->password, campo->valuestring);
   }
   
   wifi_station_set_config(config);
   free(config);
   
   root = jsonWifi(root, false);
   return root;
   
    
}

cJSON* sincronizarReloj(cJSON *root, struct DATOS_APLICACION *datosApp) {



    cJSON *nodo;
    cJSON *campo;
    cJSON *nodoRespuesta;

   if (datosApp->clock.timeValid == false) {
       datosApp->clock.estado == SIN_HORA;
       
   } else {
       datosApp->clock.estado = EXPIRADO;
   }
   
   relojTiempoReal(&datosApp->clock);
   cJSON_AddItemToObject(root, DLG_RESPUESTA, nodoRespuesta = cJSON_CreateObject());
   cJSON_AddNumberToObject(nodoRespuesta, DLG_COD_RESPUESTA, DLG_OK_CODE);
   cJSON_AddStringToObject(nodoRespuesta, DLG_OBSERVACIONES, DLG_OBSERVACIONES_SINCRONIZAR);
   

    
    return root;
    
}

void restartNormal(void* parametros) {
    system_restart();
}


cJSON* ejecutarReset(cJSON *root, struct DATOS_APLICACION *datosApp) {
    
    static os_timer_t restart;
    cJSON *nodo;
    cJSON *campo;
    cJSON *nodoRespuesta;
   
   nodo = cJSON_GetObjectItem(root, NORMAL_RESET);
   if(nodo == NULL) {
       nodo = root;
   }
    
    
    os_timer_disarm(&restart);
    os_timer_setfn(&restart, (os_timer_func_t*) restartNormal, NULL);
    os_timer_arm(&restart, DELAY_TIME_RESET, 0);
    cJSON_AddItemToObject(root, DLG_RESPUESTA, nodoRespuesta = cJSON_CreateObject());
    cJSON_AddNumberToObject(nodoRespuesta, DLG_COD_RESPUESTA, DLG_OK_CODE);
    cJSON_AddStringToObject(nodoRespuesta, DLG_OBSERVACIONES, DLG_OBSERVACIONES_RESET);
    
    
    return root;
}

cJSON* ejecutarFactoryReset(cJSON *root, struct DATOS_APLICACION *datosApp) {
    
    cJSON *nodo;
    cJSON *campo;
    cJSON *nodoRespuesta;
   

    //Se carga la configuracion por defecto
    defaultConfig(datosApp);
    //Se graba en la memoria principal
    guardarConfiguracion(datosApp, MEMORIA_PRINCIPAL);      
    
    root = ejecutarReset(root, datosApp);
    
    return root;
}

cJSON* insertarProgramacion(cJSON *root,struct DATOS_APLICACION *datosApp) {

   cJSON *nodo;
   cJSON *campo;
   cJSON *nodoRespuesta;
   TIME_PROGRAM programaActual;
   uint8 elements;
  
   
   elements= datosApp->nProgramacion;
   
   printf("funcion insertarProgramacion\n");
   nodo = cJSON_GetObjectItem(root, PROGRAM);
   if(nodo == NULL) {
       printf("program no encontrado\n");
       nodo = root;
   }
   
   //Revision de campos para crear la nueva programacion
   
   campo = cJSON_GetObjectItem(nodo, PROGRAM_TYPE);
   if((campo != NULL) && (campo->type == cJSON_Number)) {
       programaActual.tipo = campo->valueint;
   }
   
   campo = cJSON_GetObjectItem(nodo, HOUR);
   if((campo != NULL) && (campo->type == cJSON_Number)) {
       programaActual.programacion.tm_hour = campo->valueint;
   }
   campo = cJSON_GetObjectItem(nodo, MINUTE);
   if((campo != NULL) && (campo->type == cJSON_Number)) {
       programaActual.programacion.tm_min = campo->valueint;
   }
   campo = cJSON_GetObjectItem(nodo, SECOND);
   if((campo != NULL) && (campo->type == cJSON_Number)) {
       programaActual.programacion.tm_sec = campo->valueint;
   }
   campo = cJSON_GetObjectItem(nodo, WEEKDAY);
   if((campo != NULL) && (campo->type == cJSON_Number)) {
       programaActual.programacion.tm_wday = campo->valueint;
   }
   
   campo = cJSON_GetObjectItem(nodo, PROGRAM_STATE);
   if((campo != NULL) && ((campo->type == cJSON_False) || (campo->type == cJSON_True))) {
       printf("bool pillado\n");
       programaActual.activo = campo->type;
   }

       campo = cJSON_GetObjectItem(nodo, UMBRAL_TEMPERATURA);
       if((campo != NULL) && (campo->type == cJSON_Number)) {
           programaActual.umbral = campo->valuedouble;
       }


   
   datosApp->programacion = crearPrograma(programaActual, datosApp->programacion, &datosApp->nProgramacion);
   ordenarListaProgramas(datosApp->programacion, datosApp->nProgramacion, &(datosApp->clock));
   cJSON_AddItemToObject(root, DLG_RESPUESTA, nodoRespuesta = cJSON_CreateObject());
   //root = jsonProgramacion(datosApp->programacion, root, false,datosApp->nProgramacion-1);
   
   if (datosApp->nProgramacion == elements) {
       cJSON_AddNumberToObject(nodoRespuesta, DLG_COD_RESPUESTA, DLG_NOK_CODE);
       cJSON_AddStringToObject(nodoRespuesta, DLG_OBSERVACIONES, DLG_OBSERVACIONES_PROGRAMACION_NOK);
   } else {
       cJSON_AddNumberToObject(nodoRespuesta, DLG_COD_RESPUESTA, DLG_OK_CODE);
       cJSON_AddStringToObject(nodoRespuesta, DLG_OBSERVACIONES, DLG_OBSERVACIONES_PROGRAMACION_OK);
       
   }
   
   
   
    
    return root;
}

cJSON* borrarProgramacion(cJSON *root,struct DATOS_APLICACION *datosApp) {
    
   cJSON *nodo, *campo, *nodoRespuesta;
   TIME_PROGRAM programaActual;
   char idPrograma[10];
   int nPrograma;

   

   //Buscamos el nodo programa
   printf("borrar insertarProgramacion\n");
   nodo = cJSON_GetObjectItem(root, PROGRAM);
   if(nodo == NULL) {
       printf("program no encontrado\n");
       nodo = root;
   }
   
   
   campo = cJSON_GetObjectItem(nodo, PROGRAM_ID);
   if((campo != NULL) && (campo->type == cJSON_String)) {
       printf("idPrograma vale %s\n", campo->valuestring);
       strcpy(idPrograma, campo->valuestring);
   }
   //Localizamos el programa
   nPrograma = localizarProgramaPorId(idPrograma, datosApp->programacion, datosApp->nProgramacion);
   cJSON_AddItemToObject(root, DLG_RESPUESTA, nodoRespuesta = cJSON_CreateObject());
   
   if (nPrograma == -1) {
       
       cJSON_AddNumberToObject(nodoRespuesta, DLG_COD_RESPUESTA, DLG_NOK_CODE);
       cJSON_AddStringToObject(nodoRespuesta, DLG_OBSERVACIONES, DLG_OBSERVACIONES_BORRAR_PROGRAMACION_NOK);

       
   } else {
       //borramos el programa
       datosApp->programacion = borrarPrograma(datosApp->programacion, &datosApp->nProgramacion, nPrograma);
       cJSON_AddNumberToObject(nodoRespuesta, DLG_COD_RESPUESTA, DLG_OK_CODE);
       cJSON_AddStringToObject(nodoRespuesta, DLG_OBSERVACIONES, DLG_OBSERVACIONES_BORRAR_PROGRAMACION_OK);
       
   }
    
    return root;
}

cJSON* modificarProgramacion(cJSON *root,struct DATOS_APLICACION *datosApp) {
    
    
    cJSON *nodo, *campo, *nodoRespuesta;
    TIME_PROGRAM programaActual;
    char idPrograma[10];
    int nPrograma;
    uint8 nEncontrado = 0;
    
    
    
    //Buscamos el nodo programa
    nodo = cJSON_GetObjectItem(root, PROGRAM);
    if(nodo == NULL) {
        printf("program no encontrado\n");
        nodo = root;
    }
   
   
    campo = cJSON_GetObjectItem(nodo, PROGRAM_ID);
    if((campo != NULL) && (campo->type == cJSON_String)) {
        printf("idPrograma vale %s\n", campo->valuestring);
        strcpy(idPrograma, campo->valuestring);
    }
    //Localizamos el programa
    nPrograma = localizarProgramaPorId(idPrograma, datosApp->programacion, datosApp->nProgramacion);
    cJSON_AddItemToObject(root, DLG_RESPUESTA, nodoRespuesta = cJSON_CreateObject());
   
   
    if (nPrograma == -1) {
        cJSON_AddNumberToObject(nodoRespuesta, DLG_COD_RESPUESTA, DLG_NOK_CODE);
        cJSON_AddStringToObject(nodoRespuesta, DLG_OBSERVACIONES, DLG_OBSERVACIONES_MODIFICAR_PROGRAMACION_NOK);

       
    } else {
        
        //Modificamos cada campo del elemento seleccionado

       campo = cJSON_GetObjectItem(nodo, UMBRAL_TEMPERATURA);
       if((campo != NULL) && (campo->type == cJSON_Number)) {
           nEncontrado++;
           printf("modificando umbral a %d\n", campo->valueint);
           datosApp->programacion[nPrograma].umbral = campo->valuedouble;
       }

        
       campo = cJSON_GetObjectItem(nodo, HOUR);
       if((campo != NULL) && (campo->type == cJSON_Number)) {
           nEncontrado++;
           datosApp->programacion[nPrograma].programacion.tm_hour = campo->valueint;
       }
       campo = cJSON_GetObjectItem(nodo, MINUTE);
       if((campo != NULL) && (campo->type == cJSON_Number)) {
           nEncontrado++;
           datosApp->programacion[nPrograma].programacion.tm_min = campo->valueint;
       }
       campo = cJSON_GetObjectItem(nodo, SECOND);
       if((campo != NULL) && (campo->type == cJSON_Number)) {
           nEncontrado++;
           datosApp->programacion[nPrograma].programacion.tm_sec = campo->valueint;
       }
       campo = cJSON_GetObjectItem(nodo, WEEKDAY);
       if((campo != NULL) && (campo->type == cJSON_Number)) {
           nEncontrado++;
           datosApp->programacion[nPrograma].programacion.tm_wday = campo->valueint;
       }

       campo = cJSON_GetObjectItem(nodo, PROGRAM_STATE);
       if((campo != NULL) && ((campo->type == cJSON_False) || (campo->type == cJSON_True))) {
           nEncontrado++;
           printf("bool pillado\n");
           programaActual.activo = campo->type;
       }
       
       generarIdPrograma(&datosApp->programacion[nPrograma]); 
       
       //ordenarListaProgramas(datosApp->programacion, datosApp->nProgramacion, &(datosApp->clock));

        //borramos el programa
       cJSON_AddNumberToObject(nodoRespuesta, DLG_COD_RESPUESTA, DLG_OK_CODE);
       cJSON_AddStringToObject(nodoRespuesta, DLG_OBSERVACIONES, DLG_OBSERVACIONES_MODIFICAR_PROGRAMACION_OK);
       cJSON_AddStringToObject(nodoRespuesta, NEW_PROGRAM_ID, datosApp->programacion[nPrograma].idPrograma);
   }
 
    
    return root;
}

cJSON* grabarConfiguracion(cJSON *root, struct DATOS_APLICACION *datosApp) {

    cJSON *nodo;

    cJSON *nodoRespuesta;
    bool res;
   
   nodo = cJSON_GetObjectItem(root, SALVAR_CONF);
   if(nodo == NULL) {
       nodo = root;
   }
   
   res = guardarConfiguracion(datosApp, MEMORIA_PRINCIPAL);

   cJSON_AddItemToObject(root, DLG_RESPUESTA, nodoRespuesta = cJSON_CreateObject());
   
   
    if (res == true) {
        cJSON_AddNumberToObject(nodoRespuesta, DLG_COD_RESPUESTA, DLG_OK_CODE);
        cJSON_AddStringToObject(nodoRespuesta, DLG_OBSERVACIONES, DLG_OBSERVACIONES_GUARDAR_CONF_OK);

       
    } else {
        cJSON_AddNumberToObject(nodoRespuesta, DLG_COD_RESPUESTA, DLG_NOK_CODE);
        cJSON_AddStringToObject(nodoRespuesta, DLG_OBSERVACIONES, DLG_OBSERVACIONES_GUARDAR_CONF_NOK);
        
    }
        

   
    
    
    return root;
}

cJSON* backupConfiguracion(cJSON *root, struct DATOS_APLICACION *datosApp) {
    
    cJSON *nodo;

    cJSON *nodoRespuesta;
    bool res;
   
   nodo = cJSON_GetObjectItem(root, BACKUP_LOAD);
   if(nodo == NULL) {
       nodo = root;
   }
   
   res = guardarConfiguracion(datosApp, MEMORIA_BACKUP);

   cJSON_AddItemToObject(root, DLG_RESPUESTA, nodoRespuesta = cJSON_CreateObject());
   
   
    if (res == true) {
        cJSON_AddNumberToObject(nodoRespuesta, DLG_COD_RESPUESTA, DLG_OK_CODE);
        cJSON_AddStringToObject(nodoRespuesta, DLG_OBSERVACIONES, DLG_OBSERVACIONES_BACKUP_CONF_OK);

       
    } else {
        cJSON_AddNumberToObject(nodoRespuesta, DLG_COD_RESPUESTA, DLG_NOK_CODE);
        cJSON_AddStringToObject(nodoRespuesta, DLG_OBSERVACIONES, DLG_OBSERVACIONES_BACKUP_CONF_NOK);
        
    }
        
    
    return root;
}

cJSON* cargarBackup(cJSON *root, struct DATOS_APLICACION *datosApp) {
    
    cJSON *nodo;

    cJSON *nodoRespuesta;
    bool res;
   
   nodo = cJSON_GetObjectItem(root, BACKUP_LOAD);
   if(nodo == NULL) {
       nodo = root;
   }
   
   res = cargarConfiguracion(datosApp, MEMORIA_BACKUP);

   cJSON_AddItemToObject(root, DLG_RESPUESTA, nodoRespuesta = cJSON_CreateObject());
   
   
    if (res == true) {
        cJSON_AddNumberToObject(nodoRespuesta, DLG_COD_RESPUESTA, DLG_OK_CODE);
        cJSON_AddStringToObject(nodoRespuesta, DLG_OBSERVACIONES, DLG_OBSERVACIONES_CARGAR_BACKUP_OK);

       
    } else {
        cJSON_AddNumberToObject(nodoRespuesta, DLG_COD_RESPUESTA, DLG_NOK_CODE);
        cJSON_AddStringToObject(nodoRespuesta, DLG_OBSERVACIONES, DLG_OBSERVACIONES_CARGAR_BACKUP_NOK);
        
    }
        
    
    return root;
}

cJSON* AppJsonComun(struct DATOS_APLICACION *datosApp, cJSON* root, bool parte) {
    
    cJSON *nodo;



    if (root == NULL) {
       root = cJSON_CreateObject(); 
       
    } 
    
    if(parte == false) {
        cJSON_AddItemToObject(root, APP_PARAMS, nodo = cJSON_CreateObject());
    } else {
        nodo = root;
    }
    
    cJSON_AddNumberToObject(nodo, PROGRAMMER_STATE, datosApp->estadoProgramacion);
    cJSON_AddNumberToObject(nodo, NUMBER_PROGRAMS, datosApp->nProgramacion);
    cJSON_AddNumberToObject(nodo, ACTIVE_CONF, datosApp->ActiveConf);
    
    anadirJsonAplicacion(nodo, datosApp);
    
    
    
    return root;
}


cJSON* modificarServidorNtp(cJSON *root, DATOS_APLICACION *datosApp) {

   cJSON *nodo;
   cJSON *campo;
   cJSON *nodoRespuesta;
   char *ptr;
   uint8 i;
   uint8 indice;
   bool ok=true;
   
   nodo = cJSON_GetObjectItem(root, SPARES_NTP);
   if(nodo == NULL) {
       nodo = root;
   }


   campo = cJSON_GetObjectItem(nodo, SPARE_INDEX);
   if((campo != NULL) && (campo->type == cJSON_Number)) {
       if (campo->valueint > 4 ) {
           printf("error, indice incorrecto\n");
       }
       indice = campo->valueint;
   } else {
       printf("no se encuentra el campo indice\n");
       ok = false;
       
   }
   
   
   
   campo = cJSON_GetObjectItem(nodo, SPARE);
   if((campo != NULL) && (campo->type == cJSON_String)) {
       
       ptr = strtok(campo->valuestring, ".");
       datosApp->clock.ntpServer[indice][0] = atoi(ptr);
       for (i=1;i<4;i++) {
           ptr = strtok(NULL,".");
           datosApp->clock.ntpServer[indice][i] = atoi(ptr);
       }
       
       
   } else {
       printf("No se encuentra el servidor en la peticion\n");
       ok=false;
   }
   
   cJSON_AddItemToObject(root, DLG_RESPUESTA, nodoRespuesta = cJSON_CreateObject());
   
   if(ok == true) {
        cJSON_AddNumberToObject(nodoRespuesta, DLG_COD_RESPUESTA, DLG_OK_CODE);
        cJSON_AddStringToObject(nodoRespuesta, DLG_OBSERVACIONES, DLG_OBSERVACIONES_MODIFICAR_NTP_OK);
       
   } else {
        cJSON_AddNumberToObject(nodoRespuesta, DLG_COD_RESPUESTA, DLG_NOK_CODE);
        cJSON_AddStringToObject(nodoRespuesta, DLG_OBSERVACIONES, DLG_OBSERVACIONES_MODIFICAR_NTP_NOK);
       
   }
   
    
    
    return root;
}

cJSON* seleccionarNtpServerActive(cJSON *root, DATOS_APLICACION *datosApp) {
    
   cJSON *nodo;
   cJSON *campo;
   cJSON *nodoRespuesta;
   uint8 i;
   uint8 indice;
   bool ok=true;
   char *texto;
   
   nodo = cJSON_GetObjectItem(root, NTP_SERVER);
   if(nodo == NULL) {
       nodo = root;
   }
    
   campo = cJSON_GetObjectItem(nodo, SPARE_INDEX);
   if((campo != NULL) && (campo->type == cJSON_Number)) {
       if (campo->valueint > 4 ) {
           printf("error, indice incorrecto\n");
       }
       indice = campo->valueint;
   } else {
       printf("no se encuentra el campo indice\n");
       ok = false;
       
   }
    
   
   for(i=0;i<4;i++) {
    datosApp->clock.ntpServerActive[i] = datosApp->clock.ntpServer[indice][i];    
   }
   printf("Servidor activo: %d.%d.%d.%d\n", datosApp->clock.ntpServerActive[0], datosApp->clock.ntpServerActive[1],datosApp->clock.ntpServerActive[2],datosApp->clock.ntpServerActive[3]);
   
   cJSON_AddItemToObject(root, DLG_RESPUESTA, nodoRespuesta = cJSON_CreateObject());
   if(ok == true) {
        cJSON_AddNumberToObject(nodoRespuesta, DLG_COD_RESPUESTA, DLG_OK_CODE);
        cJSON_AddStringToObject(nodoRespuesta, DLG_OBSERVACIONES, DLG_OBSERVACIONES_MODIFICAR_NTP_OK);
        texto = zalloc(20*sizeof(char));
        sprintf(texto, "%d.%d.%d.%d", datosApp->clock.ntpServerActive[0], datosApp->clock.ntpServerActive[1],datosApp->clock.ntpServerActive[2],datosApp->clock.ntpServerActive[3]);
        cJSON_AddStringToObject(nodoRespuesta, NTP_SERVER_ACTIVE, texto);
        free(texto);
       
   } else {
        cJSON_AddNumberToObject(nodoRespuesta, DLG_COD_RESPUESTA, DLG_NOK_CODE);
        cJSON_AddStringToObject(nodoRespuesta, DLG_OBSERVACIONES, DLG_OBSERVACIONES_MODIFICAR_NTP_NOK);
       
   }
   
   
   

   
   
   
    
    return root;
}

cJSON* ponerEnHora(cJSON *root, DATOS_APLICACION *datosApp) {

   cJSON *nodo;
   cJSON *campo;
   cJSON *nodoRespuesta;
   uint8 i=0;
   char* ipNumber;
   uint8 nModificaciones=0;
   struct tm fecha;
   uint8 timezone;
   uint8 summertime;
   
   nodo = cJSON_GetObjectItem(root, DATE);
   if(nodo == NULL) {
       nodo = root;
   }
   
   campo = cJSON_GetObjectItem(nodo, YEAR);
   if((campo != NULL) && (campo->type == cJSON_Number)) {
       fecha.tm_year = campo->valueint;
       nModificaciones++;
   }
   
   campo = cJSON_GetObjectItem(nodo, MONTH);
   if((campo != NULL) && (campo->type == cJSON_Number)) {
       fecha.tm_mon = campo->valueint;
       nModificaciones++;
   }
   campo = cJSON_GetObjectItem(nodo, DAY);
   if((campo != NULL) && (campo->type == cJSON_Number)) {
       fecha.tm_mday = campo->valueint;
       nModificaciones++;
   }
   campo = cJSON_GetObjectItem(nodo, HOUR);
   if((campo != NULL) && (campo->type == cJSON_Number)) {
       fecha.tm_hour = campo->valueint;
       nModificaciones++;
   }
   campo = cJSON_GetObjectItem(nodo, MINUTE);
   if((campo != NULL) && (campo->type == cJSON_Number)) {
       fecha.tm_min = campo->valueint;
       nModificaciones++;
   }
   
   campo = cJSON_GetObjectItem(nodo, SECOND);
   if((campo != NULL) && (campo->type == cJSON_Number)) {
       fecha.tm_sec = campo->valueint;
       nModificaciones++;
   }
   
   campo = cJSON_GetObjectItem(nodo, NTP_TIMEZONE);
   if((campo != NULL) && (campo->type == cJSON_Number)) {
       timezone = campo->valueint;
       nModificaciones++;
   }
   campo = cJSON_GetObjectItem(nodo, NTP_SUMMER_TIME);
   if((campo != NULL) && (campo->type == cJSON_Number)) {
       summertime = campo->valueint;
       nModificaciones++;
   }


   
   cJSON_AddItemToObject(root, DLG_RESPUESTA, nodoRespuesta = cJSON_CreateObject());
   //root = jsonProgramacion(datosApp->programacion, root, false,datosApp->nProgramacion-1);
   
   if (nModificaciones == 8) {
       //Aplicamos la hora
       memcpy(&(datosApp->clock.date), &fecha, sizeof(struct tm));
       datosApp->clock.time = mktime(&datosApp->clock.date);
       datosApp->clock.ntpTimeZone = timezone;
       datosApp->clock.ntpSummerTime = summertime;
       datosApp->clock.time = datosApp->clock.time - ((datosApp->clock.ntpTimeZone + datosApp->clock.ntpSummerTime) * 3600);
       datosApp->clock.timeValid;
       datosApp->clock.estado = EN_HORA;
       ordenarListaProgramas(datosApp->programacion, datosApp->nProgramacion, &datosApp->clock);
       cJSON_AddNumberToObject(nodoRespuesta, DLG_COD_RESPUESTA, DLG_OK_CODE);
       cJSON_AddStringToObject(nodoRespuesta, DLG_OBSERVACIONES, DLG_OBSERVACIONES_MOD_CLOCK_OK);
   } else {
       cJSON_AddNumberToObject(nodoRespuesta, DLG_COD_RESPUESTA, DLG_NOK_CODE);
       cJSON_AddStringToObject(nodoRespuesta, DLG_OBSERVACIONES, DLG_OBSERVACIONES_MOD_CLOCK_NOK);
       
   }
   
    
    
    return root;
}

cJSON* subirUmbralTemperatura(cJSON *root, DATOS_APLICACION *datosApp) {


   cJSON *nodoRespuesta;
   uint8 i=0;
   char* ipNumber;

   //printf("el umbral actual vale: %d.%d", (int) datosApp->tempUmbral, parteDecimal(datosApp->tempUmbral));
   datosApp->tempUmbral = datosApp->tempUmbral + datosApp->incrementoUmbral;
   calcularAccionTermostato(datosApp);
   if (datosApp->modo == AUTO)
       datosApp->modo = AUTOMAN;
   
#ifdef LCD
pintarLcd(UMBRAL_TEMP, PAINT);
pintarLcd(MODE_WORK, PAINT);
#endif



   
   cJSON_AddItemToObject(root, DLG_RESPUESTA, nodoRespuesta = cJSON_CreateObject());
       cJSON_AddNumberToObject(nodoRespuesta, DLG_COD_RESPUESTA, DLG_OK_CODE);
       cJSON_AddNumberToObject(nodoRespuesta, UMBRAL_TEMPERATURA, datosApp->tempUmbral);
       cJSON_AddBoolToObject(nodoRespuesta, RELE, datosApp->rele);

   
    

    return root;
}

cJSON* bajarUmbralTemperatura(cJSON *root, DATOS_APLICACION *datosApp) {


   cJSON *nodoRespuesta;
   uint8 i=0;
   char* ipNumber;

   //printf("el umbral actual vale: %d.%d", (int) datosApp->tempUmbral, parteDecimal(datosApp->tempUmbral));
   datosApp->tempUmbral = datosApp->tempUmbral - datosApp->incrementoUmbral;
   calcularAccionTermostato(datosApp);
   if (datosApp->modo == AUTO)
       datosApp->modo = AUTOMAN;
   
#ifdef LCD
pintarLcd(UMBRAL_TEMP, PAINT);
pintarLcd(MODE_WORK, PAINT);
#endif



   
   cJSON_AddItemToObject(root, DLG_RESPUESTA, nodoRespuesta = cJSON_CreateObject());
   cJSON_AddNumberToObject(nodoRespuesta, DLG_COD_RESPUESTA, DLG_OK_CODE);
   cJSON_AddNumberToObject(nodoRespuesta, UMBRAL_TEMPERATURA, datosApp->tempUmbral);
   cJSON_AddBoolToObject(nodoRespuesta, RELE, datosApp->rele);
   
    

    
    
    return root;
}

cJSON* comandoDesconocido(cJSON *root, char* msg) {


    cJSON *nodo = NULL;
    cJSON *nodoRespuesta = NULL;
    


    if (root == NULL) {
       root = cJSON_CreateObject(); 
       
    } 
    
   
    
    
   cJSON_AddItemToObject(root, DLG_RESPUESTA, nodoRespuesta = cJSON_CreateObject());
   cJSON_AddNumberToObject(nodoRespuesta, DLG_COD_RESPUESTA, DLG_NOK_CODE);
   cJSON_AddStringToObject(nodoRespuesta, DLG_OBSERVACIONES, msg);
   
   
   pintarLcd(FACTORY_RESET_SETTINGS, PAINT);
   //smartconfig_start(smartconfig_done);
    
    return root;
}


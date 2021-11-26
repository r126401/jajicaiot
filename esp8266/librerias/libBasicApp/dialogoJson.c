/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */



#include "dialogoJson.h"
#include "ntp.h"
#include "configuracion.h"
#include "user_config.h"
#include "ota_config.h"


#ifdef LCD 
#include "appdisplay.h"
#endif


// El año se pone tal cual y en la visualizacion se le suma 1900
// El mes se pone tal cual y en la visualizacion se le suma 1900

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

    
    
    if (datosApp->nProgramacion == 0) {
        cJSON_AddItemToObject(comando, DLG_RESPUESTA, nodoRespuesta = cJSON_CreateObject());
        cJSON_AddNumberToObject(nodoRespuesta, DLG_COD_RESPUESTA, DLG_NOK_CODE);
        cJSON_AddStringToObject(nodoRespuesta, DLG_OBSERVACIONES, DLG_OBSERVACIONES_BORRAR_PROGRAMACION_NOK);
        return comando;
    }    

    topic = zalloc(75*sizeof(char));
    strcpy(topic, datosApp->parametrosMqtt.prefix);
    strcat(topic, datosApp->parametrosMqtt.publish);

    for(i=0;i<datosApp->nProgramacion; i++) {
        
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
        if(separar == true) {
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
    cJSON *idComando = NULL;
    cJSON *campo = NULL;
    cJSON *peticion = NULL;
    cJSON *respuesta = NULL;
    enum COMANDOS com;
    //1.- Analizar la peticion y vemos que comando nos piden.
    

    peticion = cJSON_Parse(info);
    if (peticion == NULL) {
        com = NO_JSON;
        free(peticion);
    } else {
        idComando = cJSON_GetObjectItem(peticion, COMANDO);
        if (idComando == NULL) {
            com = DESCONOCIDO;
        } else {
            idComando = cJSON_GetObjectItem(idComando, DLG_COMANDO);
            if (idComando == NULL) {
                com = NO_IMPLEMENTADO;
            } else {
                com = idComando->valueint;
            }
        }
        
    }

    respuesta = cabeceraRespuestaComando(datosApp, peticion);
    
    if (com >= 50) {
        appUser_analizarComandoAplicacion(peticion, idComando->valueint, datosApp, respuesta);

        printf("analizarComando-->comando de aplicacion ejecutado!!\n");
    } else {
        
        //4.- En funcion del comando ejecutamos una cosa u otra
        switch (com) {
            case CONSULTAR_CONF_APP: //ok--- con cabecera
                AppJsonComun(datosApp, respuesta);
                break;
            case CONSULTAR_CONF_MQTT: //probada ok--- con cabecera
                //Visualizar la configuracion mqtt de aplicacion
                printf("analizarComando-->Consultar configuracion mqtt...\n");
                //cJSON_Delete(respuesta);
                jsonMqtt(datosApp, respuesta);

                break;
            case CONSULTAR_CONF_RELOJ: //probado ok---  con cabecera
                //Visualizar la configuracion del reloj
                printf("analizarComando-->Consultar configuracion reloj\n");
                respuesta = jsonClock(datosApp, respuesta);
                break;
            case CONSULTAR_CONF_WIFI: // probado ok--- con cabecera
                //Visualizar la configuracion wifi
                printf("analizarComando-->Consultar configuracion wifi\n");
                respuesta = jsonWifi(datosApp, respuesta);
                break;
            case CONSULTAR_CONF_PROGRAMACION: // probado ok--- con cabecera
                printf("analizarComando-->consultar Programacion\n");
                respuesta = jsonProgramacionNuevoFormato(datosApp, respuesta);
                
                break;
            case MODIFICAR_CONF_MQTT: // probado ok--- con cabecera
                respuesta = modificarConfMqtt(peticion, datosApp, respuesta);
                break;
            case MODIFICAR_CONF_RELOJ: // probado ok-- con cabecera
                respuesta = modificarConfClock(peticion, datosApp, respuesta);
                break;
            case SINCRONIZAR_RELOJ: // probado ok-- con cabecera
                //cJSON_Delete(respuesta);
                sincronizarReloj(datosApp, respuesta);
                break;
            case EJECUTAR_RESET: // probado ok-- con cabecera
                //cJSON_Delete(respuesta);
                respuesta = ejecutarReset(datosApp, respuesta);
                break;
            case EJECUTAR_FACTORY_RESET: // probado ok-- con cabecera
                //cJSON_Delete(respuesta);
                respuesta = ejecutarFactoryReset(datosApp, respuesta);
                break;
            case INSERTAR_PROGRAMACION: // probado ok-- con construirCabecera
                printf("analizarComando-->insertar programacion\n");
                if (insertarProgramacion(peticion, datosApp, respuesta)) {
                    ajustarProgramacion(datosApp);
                }

                break;
            case BORRAR_PROGRAMACION: // probado ok-- con construirCabecera
                printf("analizarComando-->borrar programacion\n");
                if(borrarProgramacion(peticion, datosApp, respuesta)) {
                    ajustarProgramacion(datosApp);
                }
                break;
            case MODIFICAR_PROGRAMACION: // probado ok-- con construirCabecera
                printf("analizarComando-->modificar programacion\n");
                if (modificarProgramacion(peticion, datosApp, respuesta)) {
                    ajustarProgramacion(datosApp);
                }
                //ajusteProgramacion(datosApp, OPERACION_COMANDO);
                //ajustarProgramacion(datosApp);
                break;
            case GRABAR_CONFIGURACION: //probado ok-- con construirCabecera
                printf("analizarComando-->Guardar configuracion\n");
                //cJSON_Delete(respuesta);
                grabarConfiguracion(datosApp, respuesta);
                break;
            case BACKUP_CONFIGURACION: //probado ok-- con construirCabecera
                //cJSON_Delete(respuesta);
                backupConfiguracion(datosApp, respuesta);
                break;
            case CARGAR_BACKUP: //probado ok-- con construirCabecera
                //Liberamos la anterior programacion
                free(datosApp->programacion);
                datosApp->nProgramacion=0;
                cargarBackup(datosApp, respuesta);
                //provocamos la reevaluacion de la programacion
                datosApp->estadoProgramacion = INVALID_PROG;
                break;
            case MODIFICAR_SERVIDOR_NTP: //probado ok---con cabecera
                modificarServidorNtp(peticion, datosApp, respuesta);
                break;
            case SELECCIONAR_NTP_SERVER_ACTIVO: //probado ok--- con construirCabecera. Falla la logica
                seleccionarNtpServerActive(peticion,datosApp, respuesta);
                break;
            case PONER_EN_HORA:
                ponerEnHora(peticion, datosApp, respuesta);
                break;
            case MODIFICAR_CONF_APP:
                modificarConfApp(peticion, datosApp, respuesta);
                break;
            case SUBIR_UMBRAL:
                //respuesta = subirUmbralTemperatura(respuesta, datosApp);
                break;
            case BAJAR_UMBRAL:
                //respuesta = bajarUmbralTemperatura(respuesta, datosApp);
                break;
            case ENVIAR_DATOS_APP:
                //respuesta = jsonLectura(datosApp, respuesta);
                break;
            case UPGRADE_FIRMWARE_OTA:
                upgradeOta(peticion, datosApp, respuesta);//probado ok--
                break;
            case ALARMAS_ACTIVAS:
                //cJSON_Delete(respuesta);
                alarmasActivas(datosApp, respuesta);
            default:
                comandoDesconocido(datosApp, respuesta);
                break;
                //comando no encontrado

        }
    }
    //Despues de ejecutar el comando devolvemos el objeto json para prepara la respuesta

    
    cJSON_Delete(peticion);
    return respuesta;
    
    
}



cJSON* jsonMqtt(DATOS_APLICACION *datosApp, cJSON *respuesta) {

    cJSON_AddStringToObject(respuesta, MQTT_BROKER, datosApp->parametrosMqtt.broker);
    cJSON_AddNumberToObject(respuesta, MQTT_PORT, datosApp->parametrosMqtt.port);
    cJSON_AddStringToObject(respuesta, MQTT_USER, datosApp->parametrosMqtt.user);
    cJSON_AddStringToObject(respuesta, MQTT_PASS, datosApp->parametrosMqtt.password);
    cJSON_AddStringToObject(respuesta, MQTT_PREFIX, datosApp->parametrosMqtt.prefix);
    cJSON_AddStringToObject(respuesta, MQTT_PUBLISH, datosApp->parametrosMqtt.publish);
    cJSON_AddStringToObject(respuesta, MQTT_SUBSCRIBE, datosApp->parametrosMqtt.subscribe);
    codigoRespuesta(respuesta, RESP_OK);
  
    return respuesta;
    
    
}

cJSON* jsonProgramacionNuevoFormato(struct DATOS_APLICACION *datosApp, cJSON *respuesta) {
   
     
    cJSON *nodo = NULL;
    cJSON * array;
    cJSON *elemento;
    int i;
    char dato[30];
    cJSON *root;

    escribirProgramaActual(datosApp, respuesta);
    cJSON_AddNumberToObject(respuesta, DEVICE_STATE, datosApp->estadoApp);
    cJSON_AddNumberToObject(respuesta, PROGRAMMER_STATE, datosApp->estadoProgramacion);    
    array = cJSON_CreateArray();
    cJSON_AddItemToObject(respuesta, PROGRAM, array);
    
    for (i=0;i<datosApp->nProgramacion;i++) {
        cJSON_AddItemToArray(array, elemento = cJSON_CreateObject());
        sprintf(dato, "%s%d%d",datosApp->programacion[i].idPrograma,
                datosApp->programacion[i].estadoPrograma, 
                datosApp->programacion[i].accion);
               
        cJSON_AddStringToObject(elemento, PROGRAM_ID, dato);
        //cJSON_AddNumberToObject(elemento, PROGRAM_MASK, datosApp->programacion[i].mascara);
        if (datosApp->programacion[i].duracion > 0) {
            cJSON_AddNumberToObject(elemento,DURATION_PROGRAM, datosApp->programacion[i].duracion );
        }
#ifdef TERMOSTATO
        cJSON_AddNumberToObject(elemento,UMBRAL_TEMPERATURA, datosApp->programacion[i].temperatura );
        //cJSON_DeleteItemFromObject(root,COMANDO );
#endif
    }
    codigoRespuesta(respuesta, RESP_OK);
    
    return respuesta;
    
    
    
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
    cJSON_AddNumberToObject(nodo,PROGRAM_ACTION, program[element].accion );
    cJSON_AddNumberToObject(nodo, "time_t", program[element].programa);
    cJSON_AddNumberToObject(nodo, "diferencia", program[element].programa - datosApp.clock.time);
    printf("Adios %d\n", element);
    

    

        
    
    return root;
}



cJSON* jsonFecha(DATOS_APLICACION *datosApp, cJSON *respuesta) {
    
    //cJSON *root;
    cJSON *nodo;
    char texto[20];
    cJSON *root = NULL;

    
    
    root = cJSON_CreateObject(); 

    cJSON_AddItemToObject(root, DATE, nodo = cJSON_CreateObject());

    
    sprintf(texto, "%02d/%02d/%d %02d:%02d:%02d",
            datosApp->clock.date.tm_mday, datosApp->clock.date.tm_mon+1, datosApp->clock.date.tm_year + 1900,
            datosApp->clock.date.tm_hour, datosApp->clock.date.tm_min, datosApp->clock.date.tm_sec);
            
    cJSON_AddStringToObject(nodo, DATE,texto );
    
    nodo = jsonClock(datosApp, respuesta);
return root;    
    
    
}




cJSON* jsonClock(DATOS_APLICACION *datosApp, cJSON *respuesta) {


    char *texto = NULL;
    cJSON *arrayServers=NULL;
    cJSON *server=NULL;
    uint8 i;


    cJSON_AddBoolToObject(respuesta, TIME_VALID, datosApp->clock.timeValid );
    cJSON_AddNumberToObject(respuesta, CLOCK_STATE, datosApp->clock.estado );
    cJSON_AddNumberToObject(respuesta, NTP_TIME_EXP, datosApp->clock.ntpTimeExp );
    cJSON_AddNumberToObject(respuesta, NTP_TIMEZONE, datosApp->clock.ntpTimeZone );
    cJSON_AddNumberToObject(respuesta, NTP_TIMEOUT, datosApp->clock.ntpTimeOut );
    //visualizamos el servidor activo
    texto = zalloc(20*sizeof(char));
    sprintf(texto, "%d.%d.%d.%d", datosApp->clock.ntpServerActive[0], datosApp->clock.ntpServerActive[1],datosApp->clock.ntpServerActive[2],datosApp->clock.ntpServerActive[3]);
    cJSON_AddStringToObject(respuesta, NTP_SERVER_ACTIVE, texto);
    free(texto);
    //Visualizamos los servidores de reserva

    ;
    cJSON_AddItemToObject(respuesta,SPARES_NTP, arrayServers = cJSON_CreateArray());
    for(i=0;i<5;i++) {
        cJSON_AddItemToArray(arrayServers,server = cJSON_CreateObject());
        texto = zalloc(20*sizeof(char));
        sprintf(texto, "%d.%d.%d.%d", datosApp->clock.ntpServer[i][0],
                
                datosApp->clock.ntpServer[i][1],
                datosApp->clock.ntpServer[i][2],
                datosApp->clock.ntpServer[i][3]);
        cJSON_AddStringToObject(server,SPARE, texto);
        free(texto);
        
    }
    codigoRespuesta(respuesta, RESP_OK);
    

    
    
    

  return respuesta;  
    
}

cJSON* jsonWifi(DATOS_APLICACION *datosApp, cJSON *respuesta) {
   
    struct station_config *config;

    config = (struct station_config *)zalloc(sizeof(struct station_config));
    wifi_station_get_config_default(config);
    cJSON_AddStringToObject(respuesta, CONF_WIFI_ESSID, (char*) config->ssid);
    cJSON_AddStringToObject(respuesta, CONF_WIFI_PASS, (char*) config->password);
    free(config);
    codigoRespuesta(respuesta, RESP_OK);
    return respuesta;
    
    
}


cJSON* modificarConfMqtt(cJSON *root, struct DATOS_APLICACION *datosApp, cJSON *respuesta) {
      
   cJSON *nodo = NULL;
   cJSON *campo;
   uint8 nModificaciones=0;
   nodo = cJSON_GetObjectItem(root, MQTT_PARAMETER);
   if(nodo == NULL) {
       codigoRespuesta(respuesta, RESP_NOK);
       return respuesta;
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

   if (nModificaciones == 0) {
       codigoRespuesta(respuesta, RESP_NOK);

   } else {

       jsonMqtt(datosApp, respuesta);

       printf("modificarConfMqtt(4ok)\n");
       guardarConfiguracion(datosApp, MEMORIA_PRINCIPAL);
   }


    return respuesta;
}

cJSON* modificarConfClock(cJSON *root, struct DATOS_APLICACION *datosApp, cJSON *respuesta) {
      
   cJSON *nodo = NULL;
   cJSON *campo;
   cJSON *nodoRespuesta;
   uint8 i=0;
   char* ipNumber;
   uint8 nModificaciones=0;
   
   nodo = cJSON_GetObjectItem(root, CLOCK);
   if(nodo == NULL) {
       codigoRespuesta(respuesta, RESP_NOK);
       return respuesta;
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
   
   campo = cJSON_GetObjectItem(nodo, PROGRAMMER_STATE);
   if((campo != NULL) && (campo->type == cJSON_Number)) {
       datosApp->estadoProgramacion = campo->valueint;
       printf("modificarConfClock-->Estao de programacion: %d\n", datosApp->estadoProgramacion);
       nModificaciones++;
       
   }
   
    switch(datosApp->estadoProgramacion) {
     case INVALID_PROG:
         datosApp->estadoApp = NORMAL_SIN_PROGRAMACION;
         break;
     case VALID_PROG:
         datosApp->estadoApp = NORMAL_AUTO;
         //ajusteProgramacion(datosApp, OPERACION_COMANDO);
         ajustarProgramacion(datosApp);
         break;
     case INH_PROG:
         datosApp->estadoApp = NORMAL_SIN_PROGRAMACION;
         break;
    }

   
   if (nModificaciones == 0) {
       //construirCabecera(nodoRespuesta, datosApp, MODIFICAR_CONF_RELOJ, FALSE, root);
       codigoRespuesta(respuesta, RESP_NOK);

   } else {
       //construirCabecera(nodoRespuesta, datosApp, MODIFICAR_CONF_RELOJ, FALSE, root);
       //cJSON_AddNumberToObject(nodoRespuesta, DLG_COD_RESPUESTA, DLG_OK_CODE);
       cJSON_AddNumberToObject(respuesta, PROGRAMMER_STATE, datosApp->estadoProgramacion);
       cJSON_AddNumberToObject(respuesta, DEVICE_STATE, datosApp->estadoApp);
       //cJSON_AddStringToObject(nodoRespuesta, DLG_OBSERVACIONES, DLG_OBSERVACIONES_MOD_CLOCK_OK);
       guardarConfiguracion(datosApp, MEMORIA_PRINCIPAL);
       //root = grabarConfiguracion(root, datosApp);
       codigoRespuesta(respuesta, RESP_OK);
       
   }

    return respuesta;
}


cJSON* modificarConfWifi(cJSON* root, DATOS_APLICACION *datosApp) {

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
   
    cJSON_AddItemToObject(root, DLG_RESPUESTA, nodo = cJSON_CreateObject());
    //construirCabecera(nodo, datosApp, CONSULTAR_CONF_WIFI, TRUE, NULL);
    cJSON_AddStringToObject(nodo, CONF_WIFI_ESSID, (char*) config->ssid);
    cJSON_AddStringToObject(nodo, CONF_WIFI_PASS, (char*) config->password);
    cJSON_DeleteItemFromObject(root, COMANDO);
    
 

   
   //root = jsonWifi(datosApp);
   return root;
   
    
}

void sincronizarReloj(DATOS_APLICACION *datosApp, cJSON *respuesta) {



    cJSON *nodo;
    cJSON *campo;
    cJSON *nodoRespuesta;
    cJSON *root = NULL;
    

    

   if (datosApp->clock.timeValid == false) {
       datosApp->clock.estado == SIN_HORA;
       
   } else {
       datosApp->clock.estado = EXPIRADO;
   }
   

   ntp_get_time(&datosApp->clock);
   cJSON_AddNumberToObject(respuesta, CLOCK_STATE, datosApp->clock.estado);
   codigoRespuesta(respuesta, RESP_OK);

    
}

void restartNormal(void* parametros) {
    system_restart();
}


cJSON* ejecutarReset(DATOS_APLICACION *datosApp, cJSON *respuesta) {
    
    static os_timer_t restart;

    os_timer_disarm(&restart);
    os_timer_setfn(&restart, (os_timer_func_t*) restartNormal, NULL);
    os_timer_arm(&restart, DELAY_TIME_RESET, 0);
    codigoRespuesta(respuesta, RESP_OK);
    
    
    return respuesta;
}

cJSON* ejecutarFactoryReset(DATOS_APLICACION *datosApp, cJSON *respuesta) {

    cJSON *nodoRespuesta;
    static os_timer_t restart;
    cJSON *root = NULL;

    guardarConfiguracion(datosApp, MEMORIA_INVALIDA);      
    ejecutarReset(datosApp, respuesta);
    return respuesta;
}

bool insertarProgramacion(cJSON *peticion,DATOS_APLICACION *datosApp, cJSON *respuesta) {

   cJSON *nodo;
   cJSON *campo;
   TIME_PROGRAM *programaActual;
   uint8 elements;
   TIME_PROGRAM *programa;
  
   
   elements= datosApp->nProgramacion;
   
   printf("funcion insertarProgramacion\n");
   nodo = cJSON_GetObjectItem(peticion, PROGRAM);
   if(nodo == NULL) {
       printf("program no encontrado\n");
       codigoRespuesta(respuesta, RESP_NOK);
       return FALSE;
       
   }
   
   //Revision de campos para crear la nueva programacion
   programaActual = (TIME_PROGRAM*) calloc(1, sizeof(TIME_PROGRAM));
   programaActual->tipo = extraerDatoInt(nodo, PROGRAM_TYPE);
   programaActual->programacion.tm_hour = extraerDatoInt(nodo, HOUR);
   programaActual->programacion.tm_min = extraerDatoInt(nodo, MINUTE);
   programaActual->programacion.tm_sec = extraerDatoInt(nodo, SECOND);
   programaActual->programacion.tm_yday = extraerDatoInt(nodo, YEAR);
   programaActual->programacion.tm_mon = extraerDatoInt(nodo, MONTH);
   programaActual->programacion.tm_mday = extraerDatoInt(nodo, DAY);
   if (programaActual->programacion.tm_wday = extraerDatoInt(nodo, WEEKDAY) == -1000) {
       programaActual->programacion.tm_wday = 0;
   }
   programaActual->estadoPrograma = extraerDatoInt(nodo, PROGRAM_STATE);
   programaActual->activo = extraerDatoInt(nodo, PROGRAM_STATE);
   //datosApp->estadoApp = NORMAL_AUTO;
   programaActual->accion = extraerDatoInt(nodo, PROGRAM_ACTION);
   programaActual->mascara = extraerDatoInt(nodo, PROGRAM_MASK);
   if (programaActual->duracion = extraerDatoInt(nodo, DURATION_PROGRAM) == -1000) {
       programaActual->duracion = 0;
   }
#ifdef TERMOSTATO
   campo = cJSON_GetObjectItem(nodo, UMBRAL_TEMPERATURA);
   if((campo != NULL) && (campo->type == cJSON_Number)) {
       programaActual->temperatura = campo->valuedouble;
   }
   
   campo = cJSON_GetObjectItem(nodo, HUMEDAD);
   if((campo != NULL) && (campo->type == cJSON_Number)) {
       programaActual->temperatura = campo->valuedouble;
   }
   
#endif

   generarIdPrograma(programaActual);
   datosApp->programacion = crearPrograma(programaActual, datosApp->programacion, &datosApp->nProgramacion);
   
   
   printf("borrarProgramacion--> (1)\n");
   if (datosApp->nProgramacion == elements) {
       codigoRespuesta(respuesta, RESP_NOK);
         printf("borrarProgramacion--> (1')\n");
       return FALSE;

       
   } else {
       cJSON_AddStringToObject(respuesta, PROGRAM_ID, programaActual->idPrograma);
       cJSON_AddNumberToObject(respuesta, DEVICE_STATE, datosApp->estadoApp);
       cJSON_AddNumberToObject(respuesta, PROGRAMMER_STATE, datosApp->estadoProgramacion);
       escribirProgramaActual(datosApp, respuesta);
       guardarConfiguracion(datosApp, MEMORIA_PRINCIPAL);
       codigoRespuesta(respuesta, RESP_OK);
       return TRUE;
       
   }

}

bool borrarProgramacion(cJSON *root,struct DATOS_APLICACION *datosApp, cJSON *respuesta) {
    
   cJSON *nodo, *campo, *nodoRespuesta;
   char *idPrograma = NULL;
   int nPrograma;
   uint8 i;
   
   
   idPrograma = zalloc(20*sizeof(char));
   for(i=0;i<10;i++) {
       idPrograma[i] = '\0';
   }
   

   //Buscamos el nodo programa
   printf("borrarProgramacion-->borrar programa\n");
   nodo = cJSON_GetObjectItem(root, PROGRAM);
   if(nodo == NULL) {
       codigoRespuesta(respuesta, RESP_NOK);
       return FALSE;
   }
   
   
   campo = cJSON_GetObjectItem(nodo, PROGRAM_ID);
   if((campo != NULL) && (campo->type == cJSON_String)) {
       printf("idPrograma vale %s\n", campo->valuestring);
       strcpy(idPrograma, campo->valuestring);
   }
   //Localizamos el programa
   nPrograma = localizarProgramaPorId(idPrograma, datosApp->programacion, datosApp->nProgramacion);

  
   if (nPrograma == -1) {
       
       codigoRespuesta(respuesta, RESP_NOK);
       return FALSE;

       
   } else {
       
       //borramos el programa
       datosApp->programacion = borrarPrograma(datosApp->programacion, &datosApp->nProgramacion, nPrograma);
       if (datosApp->nProgramacion == 0) {
           datosApp->estadoApp = NORMAL_SIN_PROGRAMACION;
       }    
       cJSON_AddNumberToObject(respuesta, DEVICE_STATE, datosApp->estadoApp);
       cJSON_AddNumberToObject(respuesta, PROGRAMMER_STATE, datosApp->estadoProgramacion);
       printf("idprograma borrado:%s\n", idPrograma);
       cJSON_AddStringToObject(respuesta, PROGRAM_ID, idPrograma);
       guardarConfiguracion(datosApp, MEMORIA_PRINCIPAL);
       
   }
   if (idPrograma != NULL) {
       free(idPrograma);
       idPrograma = NULL;
   }

    
    return TRUE;
}

bool modificarProgramacion(cJSON *root,struct DATOS_APLICACION *datosApp, cJSON *respuesta) {
    
    
    cJSON *nodo, *campo;
    //TIME_PROGRAM programaActual;
    char idPrograma[19];
    int nPrograma;
    uint8 nEncontrado = 0;
    
    
    
    //Buscamos el nodo programa
    nodo = cJSON_GetObjectItem(root, PROGRAM);
    if(nodo == NULL) {
        printf("modificarProgramacion-->program no encontrado\n");
        codigoRespuesta(respuesta, RESP_NOK);
        return FALSE;
    }
   
   
    campo = cJSON_GetObjectItem(nodo, PROGRAM_ID);
    if((campo != NULL) && (campo->type == cJSON_String)) {
        printf("modificarProgramacion-->idPrograma vale %s\n", campo->valuestring);
        strcpy(idPrograma, campo->valuestring);
    }
    //Localizamos el programa
    nPrograma = localizarProgramaPorId(idPrograma, datosApp->programacion, datosApp->nProgramacion);
   
   
    if (nPrograma == -1) {
        codigoRespuesta(respuesta, RESP_NOK);
        return FALSE;
       
    } else {
        campo = cJSON_GetObjectItem(nodo, YEAR);
        if((campo != NULL) && (campo->type == cJSON_Number)) {
           datosApp->programacion[nPrograma].programacion.tm_year = campo->valueint;
           printf("insertarProgramacion-->ano : %d\n", datosApp->programacion[nPrograma].programacion.tm_year);
           nEncontrado++;
        }
        campo = cJSON_GetObjectItem(nodo, MONTH);
        if((campo != NULL) && (campo->type == cJSON_Number)) {
           datosApp->programacion[nPrograma].programacion.tm_mon = campo->valueint;
           printf("insertarProgramacion-->mes : %02d\n", datosApp->programacion[nPrograma].programacion.tm_mon);
           nEncontrado++;
        }
        campo = cJSON_GetObjectItem(nodo, DAY);
        if((campo != NULL) && (campo->type == cJSON_Number)) {
           datosApp->programacion[nPrograma].programacion.tm_mday = campo->valueint;
           printf("insertarProgramacion-->dia : %02d\n", datosApp->programacion[nPrograma].programacion.tm_mday);
           nEncontrado++;
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

       campo = cJSON_GetObjectItem(nodo, PROGRAM_TYPE);
       if((campo != NULL) && (campo->type == cJSON_Number)) {
           //programaActual.tipo = campo->valueint;
           datosApp->programacion[nPrograma].tipo = campo->valueint;
           printf("modificarProgramacion-->tipoPrograma: %d\n", datosApp->programacion[nPrograma].tipo);
       }

       campo = cJSON_GetObjectItem(nodo, PROGRAM_STATE);
       if(campo != NULL)  {
           nEncontrado++;
           datosApp->programacion[nPrograma].estadoPrograma = campo->valueint;
           datosApp->programacion[nPrograma].activo = campo->valueint;
          
       }

       campo = cJSON_GetObjectItem(nodo, PROGRAM_ACTION);
       if((campo != NULL) && (campo->type == cJSON_Number)) {
           nEncontrado++;
           datosApp->programacion[nPrograma].accion = campo->valueint;
       }


       campo = cJSON_GetObjectItem(nodo, PROGRAM_MASK);
       if((campo != NULL) && (campo->type == cJSON_Number)) {
           datosApp->programacion[nPrograma].mascara = campo->valueint;
       }
       campo = cJSON_GetObjectItem(nodo, DURATION_PROGRAM);
       if((campo != NULL) && (campo->type == cJSON_Number)) {
           datosApp->programacion[nPrograma].duracion = campo->valueint;
       }
       
       appUser_modificarProgramacionDispositivo(datosApp, nPrograma, root, respuesta);
       generarIdPrograma(&datosApp->programacion[nPrograma]); 
       cJSON_AddStringToObject(respuesta, PROGRAM_ID, idPrograma);      
       cJSON_AddStringToObject(respuesta, NEW_PROGRAM_ID, datosApp->programacion[nPrograma].idPrograma);
       cJSON_AddNumberToObject(respuesta, DEVICE_STATE, datosApp->estadoApp);
       cJSON_AddNumberToObject(respuesta, PROGRAM_STATE, datosApp->programacion[nPrograma].estadoPrograma);
       cJSON_AddNumberToObject(respuesta, PROGRAM_ACTION, datosApp->programacion[nPrograma].accion);
       guardarConfiguracion(datosApp, MEMORIA_PRINCIPAL);
       ajustarProgramacion(datosApp);
       escribirProgramaActual(datosApp, respuesta);
       codigoRespuesta(respuesta, RESP_OK);
   }


    
    return TRUE;
}

bool grabarConfiguracion(DATOS_APLICACION *datosApp, cJSON *respuesta) {


    bool res;
   res = guardarConfiguracion(datosApp, MEMORIA_PRINCIPAL);
    if (res == true) {
        codigoRespuesta(respuesta, RESP_OK);
    } else {
        codigoRespuesta(respuesta, RESP_NOK);
    }
    return res;
}

bool backupConfiguracion(DATOS_APLICACION *datosApp, cJSON *respuesta) {
    
    bool res;

    if (res = guardarConfiguracion(datosApp, MEMORIA_BACKUP)) {
        codigoRespuesta(respuesta, RESP_OK);
    } else {
        codigoRespuesta(respuesta, RESP_NOK);
    }

    if (res == true) {
        
    }        
    
    return res;
}

bool cargarBackup(DATOS_APLICACION *datosApp, cJSON *respuesta) {
    

    bool res;


   res = cargarConfiguracion(datosApp, MEMORIA_BACKUP);


   
   
    if (res == true) {
        codigoRespuesta(respuesta, RESP_OK);
    } else {
        codigoRespuesta(respuesta, RESP_NOK);
    }
        
    
    return res;
}

cJSON* AppJsonComun(struct DATOS_APLICACION *datosApp, cJSON *respuesta) {
    

    char* cadena;    
    cJSON_AddNumberToObject(respuesta, PROGRAMMER_STATE, datosApp->estadoProgramacion);
    cJSON_AddNumberToObject(respuesta, NUMBER_PROGRAMS, datosApp->nProgramacion);
    cJSON_AddNumberToObject(respuesta, ACTIVE_CONF, datosApp->ActiveConf);
    cJSON_AddStringToObject(respuesta, MQTT_BROKER, datosApp->parametrosMqtt.broker);
    cJSON_AddNumberToObject(respuesta, MQTT_PORT, datosApp->parametrosMqtt.port);
    cJSON_AddStringToObject(respuesta, MQTT_USER, datosApp->parametrosMqtt.user);
    cJSON_AddStringToObject(respuesta, MQTT_PASS, datosApp->parametrosMqtt.password);
    cadena = (char*) calloc(125,sizeof(char));
    sprintf(cadena, "%s%s", datosApp->parametrosMqtt.prefix, datosApp->parametrosMqtt.publish);
    cJSON_AddStringToObject(respuesta, MQTT_PUBLISH, cadena);
    free(cadena);
    cadena = (char*) calloc(125,sizeof(char));
    sprintf(cadena, "%s%s", datosApp->parametrosMqtt.prefix, datosApp->parametrosMqtt.subscribe);
    cJSON_AddStringToObject(respuesta, MQTT_SUBSCRIBE, cadena);
    free(cadena);
    cJSON_AddNumberToObject(respuesta, FREE_MEMORY, system_get_free_heap_size());
    cJSON_AddNumberToObject(respuesta, UPTIME, uptime);
    codigoRespuesta(respuesta, RESP_OK);
    printf("AppJsonComun--> vamos a imprimir\n");

    return respuesta;
}


bool modificarServidorNtp(cJSON *root, DATOS_APLICACION *datosApp, cJSON *respuesta) {

   cJSON *nodo;
   cJSON *campo;
   cJSON *nodoRespuesta;
   char *ptr;
   uint8 i;
   uint8 indice;
   bool ok=true;
   
   nodo = cJSON_GetObjectItem(root, SPARES_NTP);
   if(nodo == NULL) {
       codigoRespuesta(respuesta, RESP_NOK);
       return FALSE;
   }


   campo = cJSON_GetObjectItem(nodo, SPARE_INDEX);
   if((campo != NULL) && (campo->type == cJSON_Number)) {
       if (campo->valueint > 4 ) {
           printf("error, indice incorrecto\n");
           ok = false;
       } else {
          indice = campo->valueint;
          ok = true;
       }
       
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
   
   
   if(ok == true) {
       codigoRespuesta(respuesta, RESP_OK);
   } else {
       codigoRespuesta(respuesta, RESP_NOK);
   }
    return ok;
}

bool seleccionarNtpServerActive(cJSON *root, DATOS_APLICACION *datosApp, cJSON *respuesta) {
    
   cJSON *nodo;
   cJSON *campo;
   uint8 i;
   uint8 indice;
   bool ok=true;
   char *texto;
   
   nodo = cJSON_GetObjectItem(root, NTP_SERVER);
   if(nodo == NULL) {
       codigoRespuesta(respuesta, RESP_NOK);
       return TRUE;
   }
    
   campo = cJSON_GetObjectItem(nodo, SPARE_INDEX);
   if((campo != NULL) && (campo->type == cJSON_Number)) {
       if (campo->valueint > 4 ) {
           printf("error, indice incorrecto\n");
           ok = false;
       } else {
           indice = campo->valueint;
           ok = true;
       }
       
   } else {
       printf("no se encuentra el campo indice\n");
       ok = false;
       
   }
   if (ok == false) {
       codigoRespuesta(respuesta, RESP_NOK);
   } else {
       for(i=indice;i<4;i++) {
           datosApp->clock.ntpServerActive[i] = datosApp->clock.ntpServer[indice][i];
       }
       ntp_get_time(&datosApp->clock);
       printf("Servidor activo: %d.%d.%d.%d\n", datosApp->clock.ntpServerActive[0], datosApp->clock.ntpServerActive[1],datosApp->clock.ntpServerActive[2],datosApp->clock.ntpServerActive[3]);
       texto = zalloc(20*sizeof(char));
       sprintf(texto, "%d.%d.%d.%d", datosApp->clock.ntpServerActive[0], datosApp->clock.ntpServerActive[1],datosApp->clock.ntpServerActive[2],datosApp->clock.ntpServerActive[3]);
       cJSON_AddStringToObject(respuesta, NTP_SERVER_ACTIVE, texto);
       codigoRespuesta(respuesta, RESP_OK);
       free(texto);
   }
    return ok;
}

bool ponerEnHora(cJSON *root, DATOS_APLICACION *datosApp, cJSON *respuesta) {

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
       codigoRespuesta(respuesta, RESP_NOK);
       return FALSE;
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
       codigoRespuesta(respuesta, RESP_OK);
       return TRUE;
   } else {
       //construirCabecera(nodoRespuesta, datosApp, PONER_EN_HORA, FALSE, root);
       cJSON_AddNumberToObject(respuesta, DLG_COD_RESPUESTA, DLG_NOK_CODE);
       cJSON_AddStringToObject(respuesta, DLG_OBSERVACIONES, DLG_OBSERVACIONES_MOD_CLOCK_NOK);
       codigoRespuesta(respuesta, RESP_NOK);
       return FALSE;
       
   }

}

bool comandoDesconocido(DATOS_APLICACION *datosApp, cJSON *respuesta) {
    

    codigoRespuesta(respuesta, RESP_NOK);
   
    
    return TRUE;
}

bool upgradeOta(cJSON *root, struct DATOS_APLICACION *datosApp, cJSON *respuesta) {
    
    static os_timer_t restart;
    cJSON *nodo;
    cJSON *campo;
    uint8 nParametros = 0;
   
   nodo = cJSON_GetObjectItem(root, UPGRADE_FIRMWARE);
   if(nodo == NULL) {
       codigoRespuesta(respuesta, RESP_NOK);
       return TRUE;
   }
   
   campo = cJSON_GetObjectItem(nodo, OTA_SERVER);
   if((campo != NULL) && (campo->type == cJSON_String)) {
       sprintf(datosApp->ota.server, campo->valuestring);
       nParametros++;
   }

   
   campo = cJSON_GetObjectItem(nodo, OTA_PORT);
   if((campo != NULL) && (campo->type == cJSON_Number)) {
       datosApp->ota.puerto = campo->valueint;
       nParametros++;
   }
   
   campo = cJSON_GetObjectItem(nodo, OTA_URL);
   if((campo != NULL) && (campo->type == cJSON_String)) {
       sprintf(datosApp->ota.url, campo->valuestring);
       nParametros++;
   }
   
   campo = cJSON_GetObjectItem(nodo, OTA_FILE);
   if((campo != NULL) && (campo->type == cJSON_String)) {
       sprintf(datosApp->ota.file, campo->valuestring);
       nParametros++;
   }

   campo = cJSON_GetObjectItem(nodo, OTA_SW_VERSION);
   if((campo != NULL) && (campo->type == cJSON_Number)) {
       datosApp->ota.swVersion = campo->valueint;
       nParametros++;
   }
   


   printf("he metido upgrade ota\n");

   int dato = 5555;
   if ((nParametros == 5) && datosApp->estadoApp != (UPGRADE_EN_PROGRESO)){
       codigoRespuesta(respuesta, RESP_OK);
       DBG("upgradeOta--> memoria libre:  %d\n", system_get_free_heap_size());
       appUser_accionesOta(datosApp);
       xQueueSend(colaOta, &dato, portMAX_DELAY);
       return TRUE;
   } else {
       codigoRespuesta(respuesta, RESP_NOK);
       return FALSE;
   }
}



cJSON* escribirProgramaActual(DATOS_APLICACION *datosApp, cJSON *nodoRespuesta) {

    int programaActual = -1, programaFuturo;
    bool res = FALSE;

    
    res = buscarPrograma(datosApp, &programaActual);
    if ((res == TRUE) && (programaActual >= 0)) {
            cJSON_AddStringToObject(nodoRespuesta, CURRENT_PROGRAM_ID, datosApp->programacion[programaActual].idPrograma);
            printf("escribirProgramaActual-->actual: %d, futuro %d, idPrograma: %s\n", programaActual, datosApp->nProgramaCandidato, datosApp->programacion[programaActual].idPrograma);
        
    }

    return nodoRespuesta;
}

cJSON* ponerElementosRespuesta(cJSON* elemCabecera, DATOS_APLICACION *datosApp) {
    

    char fecha[25];

           sprintf(fecha, "%02d/%02d/%d %02d:%02d:%02d",
               datosApp->clock.date.tm_mday,datosApp->clock.date.tm_mon+1, datosApp->clock.date.tm_year+1900,
               datosApp->clock.date.tm_hour, datosApp->clock.date.tm_min, datosApp->clock.date.tm_sec);

    
    cJSON_AddStringToObject(elemCabecera, ID_DEVICE, get_my_id());
    cJSON_AddNumberToObject(elemCabecera, DEVICE, datosApp->tipoDispositivo);
    cJSON_AddNumberToObject(elemCabecera, OTA_SW_VERSION, datosApp->ota.swVersion);
    cJSON_AddStringToObject(elemCabecera, DATE, fecha);
    //cJSON_AddNumberToObject(element, "time_t", datosApp->clock.time); 
    return elemCabecera;
    
}


bool modificarConfApp(cJSON *root, DATOS_APLICACION *datosApp, cJSON *respuesta) {
    
    
    appUser_modificarConfApp(root, datosApp, respuesta);
        return TRUE;

    
}

void notificarOta(DATOS_APLICACION *datosApp, ESTADO_OTA codeOta) {
    
    cJSON *respuesta = NULL;

    respuesta = cabeceraEspontaneo(datosApp, UPGRADE_FIRMWARE_FOTA);
    cJSON_AddNumberToObject(respuesta, CODIGO_OTA, codeOta);
    notificarAccion(respuesta, datosApp);
    printf("notificarOta-->enviado\n");
    
}

void alarmasActivas(DATOS_APLICACION *datosApp, cJSON *respuesta) {
    


    
    cJSON_AddBoolToObject(respuesta, COMANDO, alarmas.mqtt);
    cJSON_AddBoolToObject(respuesta, COMANDO, alarmas.hora);
    cJSON_AddBoolToObject(respuesta, COMANDO, alarmas.ntp);
    cJSON_AddBoolToObject(respuesta, COMANDO, alarmas.wifi);
    cJSON_AddBoolToObject(respuesta, COMANDO, alarmas.ota);
    codigoRespuesta(respuesta, RESP_OK);
    appUser_AlarmasActivas(datosApp, respuesta);

    
    
    return;
}
/*
void notificarAlarma(DATOS_APLICACION *datosApp, enum TIPO_ALARMA tipoAlarma, enum ESTADO_ALARMA estadoAlarma, enum NOTIFICAR_ALARMA notificar) {
    
    
    char alarma[5];
    cJSON *root = NULL;
    cJSON *nodo = NULL;
    char *envio = NULL;
    char* topic = NULL;
    root = cJSON_CreateObject();
    cJSON_AddItemToObject(root, DLG_RESPUESTA, nodo = cJSON_CreateObject());
    
    
    switch (tipoAlarma) {
        case ALARMA_MQTT:
            alarmas.mqtt = estadoAlarma;
            strcpy(alarma, "MQTT");
            break;
        case ALARMA_WIFI:
            alarmas.wifi = estadoAlarma;
            strcpy(alarma, "WIFI");
            break;
        case ALARMA_HORA:
            alarmas.hora = estadoAlarma;
            strcpy(alarma, "HORA");
            break;
        case ALARMA_NTP:
            alarmas.ntp = estadoAlarma;
            strcpy(alarma, "NTP");
            break;
        case ALARMA_OTA:
            alarmas.ota = estadoAlarma;
            strcpy(alarma, "OTA");
            break;
    }
    
    if (notificar == NO) {
        printf("alarma %s, valor %d, notificar NO\n", alarma, estadoAlarma);
        return;
    }
    
    if (estadoAlarma == ALARMA_INDETERMINADA) {
        printf("alarma %s, valor %d, notificar NO\n", alarma, estadoAlarma);
        return;
    }
    
    construirCabeceraEspontaneo(nodo, datosApp, INFORME_ALARMA, TRUE, NULL);
    cJSON_AddNumberToObject(nodo, DLG_TIPO_ALARMA, tipoAlarma);
    cJSON_AddNumberToObject(nodo, DLG_ESTADO_ALARMA, estadoAlarma);
    envio = cJSON_Print(root);
    printf("alarmas:%s\n", envio);
    // preparamos los datos para el envio
    topic = zalloc(75*sizeof(char));
    strcpy(topic, datosApp->parametrosMqtt.prefix);
    strcat(topic, datosApp->parametrosMqtt.publish);

    sendMqttMessage(topic, envio);
    cJSON_Delete(root);
    free(topic);
    free(envio);
    return;
    
    
    
     
    
}
*/




cJSON* cabeceraGeneral(DATOS_APLICACION *datosApp) {
    
    char fecha[25];
    cJSON *respuesta = NULL;
    
    sprintf(fecha, "%02d/%02d/%d %02d:%02d:%02d",
       datosApp->clock.date.tm_mday,datosApp->clock.date.tm_mon+1, datosApp->clock.date.tm_year+1900,
       datosApp->clock.date.tm_hour, datosApp->clock.date.tm_min, datosApp->clock.date.tm_sec);
    
    printf("cabeceraGeneral-- (1)\n");
    respuesta = cJSON_CreateObject();
    cJSON_AddStringToObject(respuesta, ID_DEVICE, get_my_id());
    cJSON_AddNumberToObject(respuesta, DEVICE, datosApp->tipoDispositivo);
    cJSON_AddNumberToObject(respuesta, OTA_SW_VERSION, datosApp->ota.swVersion);
    cJSON_AddStringToObject(respuesta, DATE, fecha);
    printf("cabeceraGeneral-- (2)\n");
    return respuesta;
       
    
}

cJSON *cabeceraRespuestaComando(DATOS_APLICACION *datosApp, cJSON *peticion) {

    cJSON* clave = NULL;
    cJSON *parteComando = NULL;
    cJSON *respuesta = NULL;
    cJSON *com = NULL;
    
    printf("cabeceraRespuestaComando-- (1)\n");
    respuesta = cabeceraGeneral(datosApp);
    printf("cabeceraRespuestaComando-- (2)\n");
    if (peticion != NULL) {
        parteComando = cJSON_GetObjectItem(peticion, COMANDO);
        if (parteComando != NULL) {
            clave = cJSON_GetObjectItem(parteComando, DLG_KEY);
            //cJSON_AddItemToObject(respuesta, DLG_KEY, clave);
            if (clave != NULL) cJSON_AddStringToObject(respuesta, DLG_KEY, clave->valuestring);
        } else {
            return respuesta;
        }
    }else {
        printf("cabeceraRespuestaComando--> json invalido\n");
        printf("cabeceraRespuestaComando--> cabecera defecto_ %s\n", cJSON_Print(respuesta));
        return respuesta;
    }
     printf("cabeceraRespuestaComando-- (3)\n");
    com = cJSON_GetObjectItem(parteComando, DLG_COMANDO);
    if (com != NULL) {
        cJSON_AddNumberToObject(respuesta, DLG_COMANDO, com->valueint);
        
    }
     printf("cabeceraRespuestaComando-- (4)\n");
    
    
    return respuesta;
}

void codigoRespuesta(cJSON *respuesta, enum CODIGO_RESPUESTA codigo) {
    
    cJSON_AddNumberToObject(respuesta, DLG_COD_RESPUESTA, codigo);
    return;
}


cJSON* cabeceraEspontaneo(DATOS_APLICACION *datosApp, enum TIPO_INFORME tipoReport) {
    
    cJSON *respuesta;
    printf("cabeceraEspontaneo(1)\n");
    respuesta = cabeceraGeneral(datosApp);
    printf("cabeceraEspontaneo(2)\n");
    cJSON_AddNumberToObject(respuesta, TIPO_REPORT, tipoReport);
    printf("cabeceraEspontaneo(3)\n");
    return respuesta;
    
}

int extraerDatoInt(cJSON *nodo, char* nombreCampo) {
    
    int dato = -1000;
    cJSON *campo = NULL;
    campo = cJSON_GetObjectItem(nodo, nombreCampo);
    if((campo != NULL) && (campo->type == cJSON_Number)) {
       dato = campo->valueint;
       printf("extraerDatoInt--> campo %s = %d\n", nombreCampo, dato);
    } else {
        printf("extraerDatoInt-->campo %s no aparece\n", nombreCampo);
    }
    return dato;
}

double extraerDatoDouble(cJSON *nodo, char *nombreCampo) {
    
    double dato = -1000;
    cJSON *campo = NULL;
    campo =cJSON_GetObjectItem(nodo, nombreCampo);
    if((campo != NULL) && (campo->type == cJSON_Number)) {
        dato = campo->valuedouble;
        printf("extraerDatoInt--> campo %s = %f\n", nombreCampo, dato);
    }
    return dato;
}

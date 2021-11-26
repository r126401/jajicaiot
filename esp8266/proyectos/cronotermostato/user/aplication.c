/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "aplication.h"


#define PINRELE             3
#define BOTON_MAS           0
#define BOTON_MENOS         2
#define BOTON_MENU          5
#define PULSADO             0
#define NO_PULSADO          1




static os_timer_t temporizadorLectura;
#define ETS_GPIO_INTR_ENABLE() _xt_isr_unmask ((1 << ETS_GPIO_INUM))
#define ETS_GPIO_INTR_DISABLE() _xt_isr_mask(1 << ETS_GPIO_INUM) 


bool tomarLecturaDht(struct DATOS_APLICACION *datosApp) {
    
    bool res;
    uint8 i;
    uint8 lectura[5];
    DHTType sensor;
    
    sensor = DHT22;
    
    for(i=0; i < datosApp->reintentosLectura; i++) {
        
        if (res = DHTRead16(lectura) == true) {
            printf("Lectura correcta\n");
            break;
        } else {
            vTaskDelay(datosApp->intervaloReintentos * 1000 / portTICK_RATE_MS); 
        }
        
    }
    
    if(res == false) {
        printf("Agotados los reintentos: %d", i);
        return false;
    } else {
        datosApp->tempActual = scale_temperature(sensor, lectura);
        // aplicamos corrector de caja
        datosApp->tempActual = datosApp->tempActual + datosApp->calibrado;
        datosApp->humedad = scale_humidity(sensor, lectura);
        return true;
    }
    
    
    
    
    
}

cJSON * anadirJsonAplicacion(cJSON* nodo, struct DATOS_APLICACION *datosApp) {
    
    int programaFuturo, programaActual;
    bool res;
    
    cJSON_AddNumberToObject(nodo, REINTENTOS_LECTURA, datosApp->reintentosLectura);
    cJSON_AddNumberToObject(nodo, INTERVALO_REINTENTOS, datosApp->intervaloReintentos);
    cJSON_AddNumberToObject(nodo, INTERVALO_LECTURA, datosApp->intervaloLectura);    
    cJSON_AddNumberToObject(nodo, MARGEN_TEMPERATURA, datosApp->margenTemperatura);
    //cJSON_AddNumberToObject(nodo, TEMPERATURA, datosApp->tempActual);
    //cJSON_AddNumberToObject(nodo, HUMEDAD, datosApp->humedad);
    cJSON_AddBoolToObject(nodo, RELE, datosApp->rele);
    cJSON_AddNumberToObject(nodo, MODO, datosApp->modo);
    cJSON_AddNumberToObject(nodo, REPORTE_LECTURA, datosApp->condicionesEnvio);
    cJSON_AddNumberToObject(nodo, UMBRAL_DEFECTO, datosApp->tempUmbralDefecto);
    cJSON_AddNumberToObject(nodo, CALIBRADO, datosApp->calibrado);
    //cJSON_AddNumberToObject(nodo, PROGRAMMER_STATE, datosApp->estadoProgramacion);
    //Buscamos el programa activo
    res = buscarProgram(datosApp->programacion, datosApp->nProgramacion, datosApp->clock.time, &programaActual, &programaFuturo);
/*    
 switch (programaFuturo) {
     case 0:
         //Todos los programas son a futuro
         //Se aplicaría el primer programa a futuro
         programaActual = programaFuturo;
         break;
     case -1:
         //Todos los programas son a pasado.
         programaFuturo = datosApp->nProgramacion - 1;
         programaActual = programaFuturo;
         break;
     case -2:
         //Se han borrado todos los programas. Se asigna el de defecto
         datosApp->tempUmbral = datosApp->tempUmbralDefecto;
         datosApp->estadoProgramacion = INVALID_PROG;
     default:
         programaActual = programaFuturo - 1;
         break;
         
 }
 */
 printf("esta es la ultima parte\n");
    cJSON_AddStringToObject(nodo, PROGRAM_ID, datosApp->programacion[programaActual].idPrograma );
    cJSON_AddStringToObject(nodo, NEXT_PROGRAM_ID, datosApp->programacion[programaFuturo].idPrograma);
    cJSON_AddNumberToObject(nodo, UMBRAL_TEMPERATURA, datosApp->programacion[programaActual].umbral);
    printf("Escrita la ultima parte\n");
    
    
    
    //return nodo;
}

bool calcularAccionTermostato(struct DATOS_APLICACION *datosApp) {
    
    enum ESTADO_RELE estado;
    
    if (datosApp->rele == APAGADO) {
               if (datosApp->tempActual <= (datosApp->tempUmbral - datosApp->margenTemperatura)) {
                   printf("tempMedida: %d.%d, tempUmbral: %d.%d. El rele estaba apagado y hay que encenderlo\n", 
                            (int) datosApp->tempActual, parteDecimal(datosApp->tempActual), (int)  datosApp->tempUmbral, parteDecimal(datosApp->tempUmbral));
                   estado = ENCENDIDO;
                   //datosApp->rele = ENCENDIDO;
                   //conf->rele = ENCENDIDO;
                   operarRele(ENCENDIDO, datosApp);
                   
                   return true;
                  
                   
               } else {
                   estado = APAGADO;
                   printf ("tempMedida: %d.%d, tempUmbral: %d.%d. El rele estaba apagado y debe seguir apagado\n", 
                           (int) datosApp->tempActual, parteDecimal(datosApp->tempActual), (int)  datosApp->tempUmbral, parteDecimal(datosApp->tempUmbral));
                   operarRele(APAGADO, datosApp);
                   return true;
                   
               } 
           } else {
               if (datosApp->tempActual >= (datosApp->tempUmbral + datosApp->margenTemperatura) ) {
                   printf("tempMedida: %d.%d, tempUmbral: %d.%d. El rele estaba encendido y hay que apagarlo\n",
                           (int) datosApp->tempActual, parteDecimal(datosApp->tempActual), (int)  datosApp->tempUmbral, parteDecimal(datosApp->tempUmbral));
                   estado = APAGADO;
                   //datosApp->rele = APAGADO;
                   operarRele(APAGADO, datosApp);
                   return true;
               } else {
                   printf("tempMedida: %d.%d, tempUmbral: %d.%d. El rele estaba encendido y debe estar encendido\n",
                           (int) datosApp->tempActual, parteDecimal(datosApp->tempActual), (int)  datosApp->tempUmbral, parteDecimal(datosApp->tempUmbral));
                   estado = ENCENDIDO;
                   //datosApp->rele = ENCENDIDO;
                   operarRele(ENCENDIDO, datosApp);

               }
           }
    
    
    
    return true;
}


void operarRele(bool rele, struct DATOS_APLICACION *datosApp) {
    
    // Si el modo es ON, da igual todo y siempre activamos el rele
    if (datosApp->modo == ON) {
        rele = ENCENDIDO;
#ifdef LCD
        pintarLcd(HEATING, PAINT);
#endif

    }
    // Si el modo es OFF, da igual todo y siempre desactivamos el rele
    if(datosApp->modo == OFF) {
        rele = APAGADO;
#ifdef LCD
        pintarLcd(HEATING, NO_PAINT);
#endif
        
    }
    // No se actua sobre el rele si ya estaba encendido. Transitorios...
    if((GPIO_INPUT_GET(PINRELE) == 1) && (rele == ENCENDIDO)) {
        //printf("operarRele-->no actuo sobre el rele\n");
#ifdef LCD
        pintarLcd(HEATING, PAINT);
#endif
        
        return;
    }
    
    GPIO_OUTPUT_SET(PINRELE,rele);
    datosApp->rele = rele;
    //printf("El estado del rele es %d\n", rele);    

#ifdef LCD    
    if (rele == ENCENDIDO)  pintarLcd(HEATING, PAINT);
    else pintarLcd(HEATING, NO_PAINT);
#endif
    
    
    
}


 void lecturaSensores(struct DATOS_APLICACION *datosApp) {
     
     
     os_timer_disarm(&temporizadorLectura);
     os_timer_setfn(&temporizadorLectura, (os_timer_func_t*) ejecutarLectura, datosApp);
     os_timer_arm(&temporizadorLectura, datosApp->intervaloLectura * 1000, 1);

 }


 void ejecutarLectura(struct DATOS_APLICACION *datosApp) {
  
     //bool res;
     float tempAnterior;
     cJSON *lectura;
     
     
     tempAnterior = datosApp->tempActual;
     // 1.- Tomamos la lectura de temperatura y humedad.
     //res = tomarLecturaDht(datosApp);
     // 2.- Calculamos si el rele hay que encenderlo o no
     if (tomarLecturaDht(datosApp) == true) {
         datosApp->fechaLectura = datosApp->clock.time;
         calcularAccionTermostato(datosApp);
     }
     // Enviamos la lectura por mqtt segun configuracion.
#ifdef LCD
     pintarLcd(TEMPERATURE, PAINT);
     if (datosApp->estadoProgramacion == VALID_PROG) {
         
         pintarLcd(UMBRAL_TEMP, PAINT);
     }
#endif
     
     if(datosApp->parametrosMqtt.estadoBroker = ON_LINE) {
     
         switch (datosApp->condicionesEnvio) {

             case SIEMPRE:
                 lectura = jsonLectura(datosApp, NULL);
                 enviarLectura(datosApp, lectura);
                 //cJSON_Delete(lectura);
                 break;
             case CUANDO_CAMBIE:
                 if(datosApp->tempActual != tempAnterior) {
                     lectura = jsonLectura(datosApp, NULL);
                     enviarLectura(datosApp, lectura);
                     //cJSON_Delete(lectura);
                     break;
                 }
             case NUNCA:
                 printf("por defecto no se envia nada\n");
             break;




         }
     }
     
     
     
 }
 
 
cJSON* jsonLectura(struct DATOS_APLICACION *datosApp, cJSON *root) {
    
    
    cJSON *nodo = NULL;
     char texto[25];
     int idPrograma=0;
     int programaActual, programaFuturo;
     bool res;

    if (root == NULL) {
       root = cJSON_CreateObject(); 

    }


    sprintf(texto, "%02d/%02d/%d %02d:%02d:%02d",
    datosApp->clock.date.tm_mday,datosApp->clock.date.tm_mon+1, datosApp->clock.date.tm_year+1900,
    datosApp->clock.date.tm_hour, datosApp->clock.date.tm_min, datosApp->clock.date.tm_sec);

    cJSON_AddItemToObject(root, LECTURA, nodo = cJSON_CreateObject());
    cJSON_AddStringToObject(nodo, DATE, texto);
    cJSON_AddNumberToObject(nodo, TEMPERATURA, datosApp->tempActual);
    cJSON_AddNumberToObject(nodo, HUMEDAD, datosApp->humedad);
    cJSON_AddBoolToObject(nodo, RELE, datosApp->rele);
    cJSON_AddNumberToObject(nodo, UMBRAL_TEMPERATURA, datosApp->tempUmbral);
    cJSON_AddNumberToObject(nodo, MODO, datosApp->modo);
    //cJSON_AddNumberToObject(nodo,"bateria", system_adc_read());
    
    
    if ((datosApp->modo == AUTO) || (datosApp->modo == AUTOMAN)) {
        //Buscamos el proximo programa
        res = buscarProgram(datosApp->programacion, datosApp->nProgramacion, datosApp->clock.time, &programaActual, &programaFuturo);
        cJSON_AddStringToObject(nodo, PROGRAM_ID, datosApp->programacion[programaActual].idPrograma );
        cJSON_AddNumberToObject(nodo, "indiceActual", programaActual );
        cJSON_AddNumberToObject(nodo, "quedan", datosApp->programacion[programaFuturo].programa - datosApp->clock.time);
        
        cJSON_AddStringToObject(nodo, NEXT_PROGRAM_ID, datosApp->programacion[programaFuturo].idPrograma );
        cJSON_AddNumberToObject(nodo, UMBRAL_SIGUIENTE, datosApp->programacion[programaFuturo].umbral );
        

        
    }
    cJSON_AddNumberToObject(nodo, "heap", system_get_free_heap_size());
    return root;

}
 
bool enviarLectura(struct DATOS_APLICACION *datosApp, cJSON *lectura) {

 char *topic;
 char *texto;
 bool res;

// preparamos los datos para el envio
topic = zalloc(75*sizeof(char));
strcpy(topic, datosApp->parametrosMqtt.prefix);
strcat(topic, datosApp->parametrosMqtt.publish);
texto = cJSON_Print(lectura);
if (texto == NULL) {
    printf("Error al enviar lectura. No vamos a enviar nada\n");
    res = false;
} else {
    res = true;
}
cJSON_Delete(lectura);
if (res == true) {
    res = sendMqttMessage(topic, texto);
}
free(texto);
free(topic);

return res;
}
 
 
int ejecutarAccionPrograma(DATOS_APLICACION *datosApp, enum MOMENTO momento) {

 int programaFuturo, programaActual;
 bool res;
 
 //Si no hay programas, asignamos el umbral de defecto.
 
  if(datosApp->nProgramacion == 0) {
     printf("ejecutarAccionPrograma-->No hay programas en memoria y se aplica temperatura por defecto\n");
     datosApp->tempUmbral = datosApp->tempUmbralDefecto;
     return 0;
 }

 
 // Ordenamos los programas
 ordenarListaProgramas(datosApp->programacion, datosApp->nProgramacion, &(datosApp->clock));
 //Buscamos el proximo programa
 //programaFuturo = buscarPrograma(datosApp->programacion, datosApp->nProgramacion, datosApp->clock.time);
 
 res = buscarProgram(datosApp->programacion, datosApp->nProgramacion,datosApp->clock.time, &programaActual, &programaFuturo );
 
 /*
 switch (programaFuturo) {
     case ARRANQUE:
         //Todos los programas son a futuro
         //Se aplicaría el primer programa a futuro
         programaActual = programaFuturo;
         break;
     case A_PASADO:
         //Todos los programas son a pasado.
         programaFuturo = 0;
         programaActual = datosApp->nProgramacion - 1;
         //programaFuturo = datosApp->nProgramacion - 1;
         //programaActual = programaFuturo;
         break;
     case DEFECTO:
         //Se han borrado todos los programas. Se asigna el de defecto
         datosApp->tempUmbral = datosApp->tempUmbralDefecto;
     default:
         programaActual = programaFuturo - 1;
         break;
         
         
 }
*/
 
 printf("ejecutarAccionPrograma--> programaActual: %d, programaFuturo: %d\n", programaActual, programaFuturo);


 switch (datosApp->modo) {
     case AUTO:
         datosApp->tempUmbral = datosApp->programacion[programaActual].umbral;
         printf("ejecutarAccionPrograma-->modo AUTO, programaActual: %s\n", datosApp->programacion[programaActual].idPrograma);
         printf("ejecutarAccionPrograma-->modo AUTO, falta para el proximo evento:%d segundos\n", datosApp->programacion[programaFuturo].programa - datosApp->clock.time);
         break;
     case AUTOMAN:
         if (momento == CAMBIO_PROGRAMA) {
             datosApp->modo = AUTO;
         }
         datosApp->tempUmbral = datosApp->programacion[programaActual].umbral;
         printf("ejecutarAccionPrograma-->modo AUTOMAN, programaActual: %s\n", datosApp->programacion[programaActual].idPrograma);
         printf("ejecutarAccionPrograma-->modo AUTOMAN, falta para el proximo evento:%d segundos\n", datosApp->programacion[programaFuturo].programa - datosApp->clock.time);

         break;
     case MANUAL:
         printf("ejecutarAccionPrograma-->No se actualiza la temperatura umbral\n");
     default:
         printf("ejecutarAccionPrograma-->Resto de estados, no se actualiza la temperatura umbral\n");
         break;             
         }
         
      return programaFuturo;   

         
 }

 
cJSON* modificarJsonAplicacion(cJSON *root, struct DATOS_APLICACION *datosApp) {
    
   cJSON *nodo;
   cJSON *campo;
   cJSON *nodoRespuesta;
   uint8 modificaciones = 0;
   

   nodo = cJSON_GetObjectItem(root, APP_PARAMS);
   if(nodo == NULL) {
       printf("APP_PARAMS no encontrado\n");
       nodo = root;
   }
   
   campo = cJSON_GetObjectItem(nodo, PROGRAMMER_STATE);
   if((campo != NULL) && (campo->type == cJSON_Number)) {
       datosApp->estadoProgramacion = campo->valueint;
       modificaciones++;
   }
   
   campo = cJSON_GetObjectItem(nodo, REINTENTOS_LECTURA);
   if((campo != NULL) && (campo->type == cJSON_Number)) {
       datosApp->reintentosLectura = campo->valueint;
       modificaciones++;
   }

   campo = cJSON_GetObjectItem(nodo, INTERVALO_REINTENTOS);
   if((campo != NULL) && (campo->type == cJSON_Number)) {
       datosApp->intervaloReintentos = campo->valueint;
       modificaciones++;
   }

   campo = cJSON_GetObjectItem(nodo, INTERVALO_LECTURA);
   if((campo != NULL) && (campo->type == cJSON_Number)) {
       datosApp->intervaloLectura = campo->valueint;
       modificaciones++;
   }
   
   campo = cJSON_GetObjectItem(nodo, MARGEN_TEMPERATURA);
   if((campo != NULL) && (campo->type == cJSON_Number)) {
       datosApp->margenTemperatura = campo->valuedouble;
       modificaciones++;
   }

   
   campo = cJSON_GetObjectItem(nodo, RELE);
   if((campo != NULL) && (campo->type == cJSON_Number)) {
       datosApp->margenTemperatura = campo->valueint;
       modificaciones++;
       //Habria que hacer mas acciones para dejar el termostato coherente
   }
   
   campo = cJSON_GetObjectItem(nodo, UMBRAL_TEMPERATURA);
   if((campo != NULL) && (campo->type == cJSON_Number)) {
       datosApp->tempUmbral = campo->valuedouble;
       //ejecutarAccionPrograma(datosApp, CAMBIO_PROGRAMA);
       calcularAccionTermostato(datosApp);       
       modificaciones++;
       //Habria que hacer mas acciones para dejar el termostato coherente
   }
   
   
   
   
   campo = cJSON_GetObjectItem(nodo, REPORTE_LECTURA);
   if((campo != NULL) && (campo->type == cJSON_Number)) {
       datosApp->condicionesEnvio = campo->valueint;
       modificaciones++;
       //Habria que hacer mas acciones para dejar el termostato coherente
   }
   

   campo = cJSON_GetObjectItem(nodo, UMBRAL_DEFECTO);
   if((campo != NULL) && (campo->type == cJSON_Number)) {
       datosApp->tempUmbralDefecto = campo->valuedouble;
       modificaciones++;
       //Habria que hacer mas acciones para dejar el termostato coherente
   }


   campo = cJSON_GetObjectItem(nodo, CALIBRADO);
   if((campo != NULL) && (campo->type == cJSON_Number)) {
       datosApp->calibrado = campo->valuedouble;
       modificaciones++;
       //Habria que hacer mas acciones para dejar el termostato coherente
   }

   
   campo = cJSON_GetObjectItem(nodo, MODO);
   if((campo != NULL) && (campo->type == cJSON_Number)) {
       datosApp->modo = campo->valueint;
       printf("modificarJsonAplicacion-->calculando la accion del termostato\n");
       ejecutarAccionPrograma(datosApp, CAMBIO_PROGRAMA);
       calcularAccionTermostato(datosApp);
       modificaciones++;
       //Habria que hacer mas acciones para dejar el termostato coherente
#ifdef LCD
       pintarLcd(MODE_WORK, PAINT);
#endif
       
   }
   
   
   cJSON_AddItemToObject(root, DLG_RESPUESTA, nodoRespuesta = cJSON_CreateObject());
   
   if(modificaciones > 0) {
        cJSON_AddNumberToObject(nodoRespuesta, DLG_COD_RESPUESTA, DLG_OK_CODE);
        cJSON_AddStringToObject(nodoRespuesta, DLG_OBSERVACIONES, DLG_OBSERVACIONES_MODIFICAR_APP_OK);
       
   } else {
        cJSON_AddNumberToObject(nodoRespuesta, DLG_COD_RESPUESTA, DLG_NOK_CODE);
        cJSON_AddStringToObject(nodoRespuesta, DLG_OBSERVACIONES, DLG_OBSERVACIONES_MODIFICAR_APP_NOK);
       
   }
   
     
     
     return root;
 }


void ICACHE_FLASH_ATTR InicializarGpios(DATOS_APLICACION *datosApp) {
    

    //configuro el rele
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_U0RXD_U, FUNC_GPIO3);
    GPIO_DIS_OUTPUT(PINRELE);
    // Configuro el pin para el boton +
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0);
    GPIO_OUTPUT_SET(GPIO_ID_PIN(BOTON_MAS), NO_PULSADO);

    // Configuro el pin para el boton -
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO2_U,FUNC_GPIO2);
    GPIO_OUTPUT_SET(GPIO_ID_PIN(BOTON_MENOS), NO_PULSADO);
    
    // Configuro el pin para el boton Menu
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO5_U, FUNC_GPIO5);
    GPIO_OUTPUT_SET(GPIO_ID_PIN(BOTON_MENU), NO_PULSADO);
    
    ETS_GPIO_INTR_DISABLE();
    
    //Rregistro la rutina que lo va a tratar
    gpio_intr_handler_register(tratarInterrupcionesPulsadores, datosApp);
    
    
    //Boton +
    gpio_pin_intr_state_set (GPIO_ID_PIN(BOTON_MAS), GPIO_PIN_INTR_NEGEDGE);
    
    //Boton -
    gpio_pin_intr_state_set (GPIO_ID_PIN(BOTON_MENOS), GPIO_PIN_INTR_NEGEDGE);
    
    // Boton Menu
    gpio_pin_intr_state_set (GPIO_ID_PIN(BOTON_MENU), GPIO_PIN_INTR_POSEDGE);
    ETS_GPIO_INTR_ENABLE();

    
}

void tratarInterrupcionesPulsadores(DATOS_APLICACION *datosApp) {
    

    uint32 gpio_status = GPIO_REG_READ(GPIO_STATUS_ADDRESS);
    static os_timer_t temporizador;
    static bool rebote = false;
    //printf("han pulsado un boton....\n");
    
    
    
    if (gpio_status & (BIT(BOTON_MENU))) {
        
        if (datosApp->botonPulsado == false) {
            datosApp->botonPulsado = true;
            printf("Has pulsado el boton M\n");
            os_timer_disarm(&temporizador);
            os_timer_setfn(&temporizador, (os_timer_func_t*) pulsacionMenu, datosApp);
            os_timer_arm(&temporizador, 250,0);        
                    
        }
    }
    


    if (gpio_status & (BIT(BOTON_MAS))) {
        
        if (rebote == false) {
            rebote = true;
            printf("Has pulsado el boton +\n");
            os_timer_disarm(&temporizador);
            os_timer_setfn(&temporizador, (os_timer_func_t*) cancelarReboteMasMenos, &rebote);
            os_timer_arm(&temporizador, 250,0);        
            printf("Ejecuto la accion del boton +\n");
            botonMas(datosApp);
                    
                    
        }
    }
    
    if (gpio_status & (BIT(BOTON_MENOS))) {
         
        if (rebote == false) {
            rebote = true;
            printf("Has pulsado el boton -\n");
            os_timer_disarm(&temporizador);
            os_timer_setfn(&temporizador, (os_timer_func_t*) cancelarReboteMasMenos, &rebote);
            os_timer_arm(&temporizador, 250,0);        
            printf("Ejecuto la accion del boton -\n");
            botonMenos(datosApp);
                    
                    
        }
    }
    

 
    
    GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, gpio_status );
}

void pulsacionMenu(DATOS_APLICACION *datosApp) {
    
    static os_timer_t repeticion;
    static uint8 rep=0;
    
    
    if (GPIO_INPUT_GET(BOTON_MENU) == PULSADO) {
        os_timer_disarm(&repeticion);
        os_timer_setfn(&repeticion, (os_timer_func_t*) pulsacionMenu, datosApp);
        os_timer_arm(&repeticion, 500,0);
        rep++;
        printf("repeticion %d\n", rep);
        
    } else {
        datosApp->botonPulsado = false;
        //*rebote = false;
        //printf("Rebote cancelado, rep = %d\n", rep);
        if (rep > 5) {
            //printf("pulsacion larga\n");
            datosApp->estadoApp = SETTINGS_CLOCK;
            datosApp->fechaTemporal.tm_mday=1;
            datosApp->fechaTemporal.tm_mon =1;
            datosApp->fechaTemporal.tm_hour=0;
            datosApp->fechaTemporal.tm_min=0;
            datosApp->fechaTemporal.tm_sec=0;
            datosApp->fechaTemporal.tm_year = 2017;
            pintarLcd(CLOCK_SETTINGS, PAINT);
        } else {
            //printf("pulsacion corta\n");
            botonMenu(datosApp);
        }
        
        rep=0;
    }
    
    
    
}

void cancelarReboteMasMenos(bool *rebote) {
    

        *rebote = false;
        //printf("Rebote cancelado\n");
    

    
    
}


void botonMenu(DATOS_APLICACION *datosApp) {
    
    
    printf("botonMenu-->modo %d, estadoApp %d\n", datosApp->modo, datosApp->estadoApp);
    //Si el estado de configuracion es normal, voy cambiando el modo
    switch (datosApp->estadoApp) {
        case NORMAL:
            switch (datosApp->modo) {
                case AUTO:
                case AUTOMAN:
                    datosApp->modo = ON;
                    break;
                case MANUAL:
                    //Si aun no hay hora valida no se puede poner en modo AUTO
                    if (datosApp->clock.timeValid == true) {
                        datosApp->modo = AUTO;
                        ejecutarAccionPrograma(datosApp, CAMBIO_PROGRAMA);
                    } else {
                        datosApp->modo = ON;
                    }
                    
                    
                    //pintarLcd(UMBRAL_TEMP, PAINT);
                    //datosApp->modo = ON;
                    break;
                case ON:
                    datosApp->modo = OFF;
                    break;
                case OFF:
                    datosApp->modo = MANUAL;
                    break;
            }
            pintarLcd(UMBRAL_TEMP, PAINT);
            printf("botonMenu--> Modo seleccionado: %d\n", datosApp->modo);
            calcularAccionTermostato(datosApp);
#ifdef LCD
            pintarLcd(MODE_WORK, PAINT);
#endif
            break;
        case SETTINGS_CLOCK:
            pintarLcd(CLOCK_SETTINGS_DAY, PAINT);
            datosApp->estadoApp = SETTINGS_CLOCK_DAY;
            break;
            
            
        case SETTINGS_CLOCK_DAY:
            pintarLcd(CLOCK_SETTINGS_MONTH, PAINT);
            datosApp->estadoApp = SETTINGS_CLOCK_MONTH;
            break;
        case SETTINGS_CLOCK_MONTH:
            pintarLcd(CLOCK_SETTINGS_YEAR, PAINT);
            datosApp->estadoApp = SETTINGS_CLOCK_YEAR;
            break;
        case SETTINGS_CLOCK_YEAR:
            pintarLcd(CLOCK_SETTINGS_HOUR, PAINT);
            datosApp->estadoApp = SETTINGS_CLOCK_HOUR;
            break;
        case SETTINGS_CLOCK_HOUR:
            pintarLcd(CLOCK_SETTINGS_MIN, PAINT);
            datosApp->estadoApp = SETTINGS_CLOCK_MIN;
            break;
        case SETTINGS_CLOCK_MIN:
            pintarLcd(CLOCK_SETTINGS_CONFIRM, PAINT);
            datosApp->estadoApp = SETTINGS_CLOCK_CONFIRM;
            break;
        case SETTINGS_CLOCK_CONFIRM:
            modoNormal(datosApp);
            break;
        case SETTINGS_SMARTCONFIG:            
            printf("botonMenu-->Activamos smartconfig\n");
            pintarLcd(SMARTCONFIG_FIND_CHANNEL, PAINT);
            datosApp->estadoApp = SETTINGS_SMARTCONFIG_FIND_CHANNEL;
            static os_timer_t tempSmart;
            os_timer_disarm(&tempSmart);
            os_timer_setfn(&tempSmart, (os_timer_func_t*) ejecutarSmartconfig, NULL);
            os_timer_arm(&tempSmart, 1000, false);
            break;
        case SETTINGS_SMARTCONFIG_FIND_CHANNEL:
            if (wifi_station_get_connect_status() == STATION_GOT_IP) {
                datosApp->estadoApp = NORMAL;
                pintarLcd(NORMAL_LCD, PAINT);
            } else {
                smartconfig_stop();
                datosApp->estadoApp = NORMAL;
                pintarLcd(NORMAL_LCD, PAINT);
                
            }
            break;
        case SETTINGS_RESET:
            system_restart();
            break;
        case SETTINGS_FACTORY:
            //Se carga la configuracion por defecto
            defaultConfig(datosApp);
            //Se graba en la memoria principal
            guardarConfiguracion(datosApp, MEMORIA_PRINCIPAL);
            pintarLcd(FACTORY_RESET_OK, PAINT);
            break;
            
            
            
    }
           
}
void ejecutarSmartconfig() {
   smartconfig_start(smartconfig_done);
   printf("ejecutarSmartconfig-->ejecutado smartconfig\n");
} 
    
 

void botonMas(DATOS_APLICACION *datosApp) {
    

    switch (datosApp->estadoApp) {
        case NORMAL:
            switch (datosApp->modo) {
                case AUTO:
                    datosApp->modo = AUTOMAN;
                    datosApp->tempUmbral = datosApp->tempUmbral + datosApp->incrementoUmbral;
                    calcularAccionTermostato(datosApp);                    
                    pintarLcd(MODE_WORK, PAINT);
                    pintarLcd(UMBRAL_TEMP, PAINT);

                    break;
                case AUTOMAN:
                case MANUAL:
                case ON:
                case OFF:
                    datosApp->tempUmbral = datosApp->tempUmbral + datosApp->incrementoUmbral;
                    calcularAccionTermostato(datosApp);                    
                    pintarLcd(MODE_WORK, PAINT);
                    pintarLcd(UMBRAL_TEMP, PAINT);

                    break;

            }
            break;
        case SETTINGS_CLOCK:
           pintarLcd(SMARTCONFIG_SETTINGS, PAINT);
           datosApp->estadoApp = SETTINGS_SMARTCONFIG;
            break;     
        case SETTINGS_SMARTCONFIG:
            //pintarLcd(SMARTCONFIG_SETTINGS_ACTIVATING, PAINT);
            datosApp->estadoApp = SETTINGS_RESET;
            pintarLcd(RESET_SETTINGS, PAINT);
            break;    
        case SETTINGS_RESET:
            datosApp->estadoApp = SETTINGS_FACTORY;
            pintarLcd(FACTORY_RESET_SETTINGS, PAINT);
            break;
        case SETTINGS_FACTORY:
            datosApp->estadoApp = NORMAL;
            pintarLcd(NORMAL_LCD, PAINT);
            break;
        case SETTINGS_CLOCK_DAY:
            if (datosApp->fechaTemporal.tm_mday == 31) datosApp->fechaTemporal.tm_mday = 1;
            else datosApp->fechaTemporal.tm_mday++;
            pintarLcd(CLOCK_SETTINGS_DAY, PAINT);
            break;
        case SETTINGS_CLOCK_MONTH:
            if (datosApp->fechaTemporal.tm_mon == 12) datosApp->fechaTemporal.tm_mon = 1;
            else datosApp->fechaTemporal.tm_mon++;
            pintarLcd(CLOCK_SETTINGS_MONTH, PAINT);
            break;
        case SETTINGS_CLOCK_YEAR:
            datosApp->fechaTemporal.tm_year++;
            pintarLcd(CLOCK_SETTINGS_YEAR, PAINT);
            break;
        case SETTINGS_CLOCK_HOUR:
            if (datosApp->fechaTemporal.tm_hour == 23) datosApp->fechaTemporal.tm_hour = 0;
            else datosApp->fechaTemporal.tm_hour++;
            pintarLcd(CLOCK_SETTINGS_HOUR, PAINT);
            break;
        case SETTINGS_CLOCK_MIN:
            if (datosApp->fechaTemporal.tm_min == 59) datosApp->fechaTemporal.tm_min = 0;
            else datosApp->fechaTemporal.tm_min++;
            pintarLcd(CLOCK_SETTINGS_MIN, PAINT);
            break;
        case SETTINGS_CLOCK_CONFIRM:
            datosApp->estadoApp = NORMAL;
            ejecutarAccionPrograma(datosApp, INDIFERENTE);
            pintarLcd(NORMAL_LCD, PAINT);
            break;
            

    }
    
    
    
}

void botonMenos(DATOS_APLICACION *datosApp) {
    
    switch (datosApp->estadoApp) {
        case NORMAL:
            switch (datosApp->modo) {
                case AUTO:
                    datosApp->modo = AUTOMAN;
                    datosApp->tempUmbral = datosApp->tempUmbral - datosApp->incrementoUmbral;
                    calcularAccionTermostato(datosApp);                    
#ifdef LCD
                    pintarLcd(MODE_WORK, PAINT);
                    pintarLcd(UMBRAL_TEMP, PAINT);
#endif

                    break;
                case AUTOMAN:
                case MANUAL:
                case ON:
                case OFF:
                    datosApp->tempUmbral = datosApp->tempUmbral - datosApp->incrementoUmbral;
                    calcularAccionTermostato(datosApp);                    
#ifdef LCD
                    pintarLcd(MODE_WORK, PAINT);
                    pintarLcd(UMBRAL_TEMP, PAINT);
#endif

                    break;

            }
            break;
        case SETTINGS_CLOCK_DAY:
            if (datosApp->fechaTemporal.tm_mday == 1) datosApp->fechaTemporal.tm_mday = 31;
            else datosApp->fechaTemporal.tm_mday--;
            pintarLcd(CLOCK_SETTINGS_DAY, PAINT);
            break;
        case SETTINGS_CLOCK_MONTH:
            if (datosApp->fechaTemporal.tm_mon == 1) datosApp->fechaTemporal.tm_mon = 12;
            else datosApp->fechaTemporal.tm_mon--;
            pintarLcd(CLOCK_SETTINGS_MONTH, PAINT);
            break;
        case SETTINGS_CLOCK_YEAR:
            datosApp->fechaTemporal.tm_year--;
            pintarLcd(CLOCK_SETTINGS_YEAR, PAINT);
            break;
        case SETTINGS_CLOCK_HOUR:
            if (datosApp->fechaTemporal.tm_hour == 0) datosApp->fechaTemporal.tm_hour = 23;
            else datosApp->fechaTemporal.tm_hour--;
            pintarLcd(CLOCK_SETTINGS_HOUR, PAINT);
            break;
        case SETTINGS_CLOCK_MIN:
            if (datosApp->fechaTemporal.tm_min == 0) datosApp->fechaTemporal.tm_min = 59;
            else datosApp->fechaTemporal.tm_min--;
            pintarLcd(CLOCK_SETTINGS_MIN, PAINT);
            break;
        case SETTINGS_CLOCK_CONFIRM:
            datosApp->estadoApp = NORMAL;
            pintarLcd(NORMAL_LCD, PAINT);
            break;
        case SETTINGS_CLOCK:
           pintarLcd(NORMAL_LCD, PAINT);
           datosApp->estadoApp = NORMAL;
            break;     
        case SETTINGS_SMARTCONFIG:
            //pintarLcd(SMARTCONFIG_SETTINGS_ACTIVATING, PAINT);
            datosApp->estadoApp = SETTINGS_CLOCK;
            pintarLcd(CLOCK_SETTINGS, PAINT);
            break;    
        case SETTINGS_RESET:
            datosApp->estadoApp = SETTINGS_SMARTCONFIG;
            pintarLcd(SMARTCONFIG_SETTINGS, PAINT);
            break;
        case SETTINGS_FACTORY:            
            pintarLcd(RESET_SETTINGS, PAINT);
            datosApp->estadoApp = SETTINGS_RESET;
            break;
            
    }
    
}






void modoNormal(struct DATOS_APLICACION *datosApp) {
    
    printf("modoNormal-->grabamos la hora y volvemos\n");
    // 1.- Copiar la hora en la estructura de datos.
    datosApp->fechaTemporal.tm_year -= 1900;
    datosApp->fechaTemporal.tm_mon--;
    memcpy(&datosApp->clock.date, &datosApp->fechaTemporal, sizeof(struct tm));
       datosApp->clock.time = mktime(&datosApp->clock.date);
       datosApp->clock.ntpTimeZone = 2;
       datosApp->clock.time = datosApp->clock.time - ((datosApp->clock.ntpTimeZone + datosApp->clock.ntpSummerTime) * 3600);
       datosApp->clock.timeValid;
       datosApp->clock.estado = EN_HORA;
       ordenarListaProgramas(datosApp->programacion, datosApp->nProgramacion, &datosApp->clock);
    // poner el display en modo normal.
       datosApp->estadoApp = NORMAL;
       pintarLcd(NORMAL_LCD, PAINT);
    
}
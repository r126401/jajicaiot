/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "programmer.h"
#include "user_config.h"




#define uint8 uint8_t


struct TIME_PROGRAM*  insertarElemento(struct TIME_PROGRAM *listProgram, uint8 *elements) {
    
    TIME_PROGRAM *list;
    uint8 tamano;

    //Si es el primer elemento, se crea la lista
    if ((listProgram == NULL) || (*elements == 0)) {
        list = (TIME_PROGRAM*) calloc(1,sizeof(*listProgram));
        (*elements) ++;
       //printf("reservados : %d\n", sizeof(*list));
    } else {
        //Se añade un elemento mas
        (*elements) ++;
        list = (TIME_PROGRAM*) realloc(listProgram, sizeof(*list) * (*elements) );
        //printf("reservados : %d\n", sizeof(*list) * (*elements));
    }
    
    if (list == NULL) {
        printf("insertarElemento--> Error al crear el elemento\n");
        return NULL;
    } else {
        printf("insertarElemento--> Elemento creado o anadido\n");

        return list;
                
    }
    
    
}

int compararDatos(struct TIME_PROGRAM *dat1, struct TIME_PROGRAM *dat2) {
    
    if (dat1->programa < dat2->programa) 
        return -1;
    else if(dat1->programa > dat2->programa)
        return 1;
    else return 0;
    
}


TIME_PROGRAM* crearPrograma(TIME_PROGRAM peticion, TIME_PROGRAM *listaProgramas, uint8 *nElementos) {
    
    TIME_PROGRAM *list;
    uint8 elem;
    elem = *nElementos;
    generarIdPrograma(&peticion);
    //Si se encuentra duplicado, no se crea el programa
    if (buscarProgramaDuplicado(peticion.idPrograma, listaProgramas, elem) == true) {
        printf("crearPrograma-->Programa no creado\n");
        return listaProgramas;
    }
    
    //Primero insertamos el elemento en la lista
    list = insertarElemento(listaProgramas, (uint8*)nElementos);
    if (list == NULL) return NULL;
    
    
    //Lo actualizamos con la informacion de la peticion.
    memcpy(&list[elem], &peticion, sizeof(*list));
    
    return list;
        
        
    }
    
uint8 InicializarListaProgramas(TIME_PROGRAM *listaProgramas, int nElementos, struct NTP_CLOCK *clock) {

    int i;
    for(i=0;i<nElementos;i++) {
        if (listaProgramas[i].tipo == SEMANAL) {
            listaProgramas[i].programacion.tm_mday = listaProgramas[i].programacion.tm_mday +7;
        }
    } 
    
    
}
    
uint8 ordenarListaProgramas(TIME_PROGRAM *listaProgramas, int nElementos, struct NTP_CLOCK *clock) {
    
    int i;
    struct tm *horaActual;
    horaActual = &clock->date;
    
    // Añado los atributos necesarios en funcion del tipo de programacion para convertirlo en time_t valido.
    
    for(i=0;i<nElementos;i++) {
        listaProgramas[i].programacion.tm_year = horaActual->tm_isdst;
        switch (listaProgramas[i].tipo) {
            case DIARIA:
                listaProgramas[i].programacion.tm_year = horaActual->tm_year;
                listaProgramas[i].programacion.tm_mon = horaActual->tm_mon;
                listaProgramas[i].programacion.tm_mday = horaActual->tm_mday;
                listaProgramas[i].programacion.tm_wday = horaActual->tm_wday;
                listaProgramas[i].programacion.tm_yday = horaActual->tm_yday;
                break;
                
            case SEMANAL:
                listaProgramas[i].programacion.tm_year = horaActual->tm_year;
                
                if (horaActual->tm_wday > listaProgramas[i].programacion.tm_wday) {
                   // printf("dia de la semana actual > programa, %d, %d \n", (horaActual->tm_wday - listaProgramas[i].programacion.tm_wday),
                     //       (horaActual->tm_wday - listaProgramas[i].programacion.tm_wday));
                    listaProgramas[i].programacion.tm_yday = horaActual->tm_yday - (horaActual->tm_wday - listaProgramas[i].programacion.tm_wday);
                    listaProgramas[i].programacion.tm_mday = horaActual->tm_mday - (horaActual->tm_wday - listaProgramas[i].programacion.tm_wday);
                    listaProgramas[i].programacion.tm_mon = horaActual->tm_mon;
                    //Si ya ha pasado la programacion, le sumamos 7 dias
                    listaProgramas[i].programacion.tm_mday = listaProgramas[i].programacion.tm_mday;
                } else if (horaActual->tm_wday < listaProgramas[i].programacion.tm_wday) {
                  //  printf("dia de la semana menor que el programado. Sumamos %d dias\n", (listaProgramas[i].programacion.tm_wday- horaActual->tm_wday));
                    listaProgramas[i].programacion.tm_yday = horaActual->tm_yday + (listaProgramas[i].programacion.tm_wday - horaActual->tm_wday);
                    listaProgramas[i].programacion.tm_mday = horaActual->tm_mday + (listaProgramas[i].programacion.tm_wday - horaActual->tm_wday);
                    listaProgramas[i].programacion.tm_mon = horaActual->tm_mon;
                    
                } else {
                    
                    listaProgramas[i].programacion.tm_yday = horaActual->tm_yday;
                    listaProgramas[i].programacion.tm_mday = horaActual->tm_mday;
                    listaProgramas[i].programacion.tm_mon = horaActual->tm_mon;
                    /*
                    // Si la hora actual es > que la hora programada, sumo 7 dias
                    if (horaActual->tm_hour > listaProgramas[i].programacion.tm_hour) { 
                        listaProgramas[i].programacion.tm_mday = listaProgramas[i].programacion.tm_mday + 7;
                    }
                    //Si las horas son iguales, pero los minutos actuales son mayor que la programada, sumo 7 dias
                    if ((horaActual->tm_hour == listaProgramas[i].programacion.tm_hour) &&
                    (horaActual->tm_min > listaProgramas[i].programacion.tm_min)) {
                        listaProgramas[i].programacion.tm_mday = listaProgramas[i].programacion.tm_mday + 7;
                    }
                    // Si la hora actual es == a la hora programada, y los minutos tambien, 
                    // Si los segundos actuales son mayores tambien les sumamos 7 dias.
                    if (((horaActual->tm_hour == listaProgramas[i].programacion.tm_hour) &&
                    (horaActual->tm_min == listaProgramas[i].programacion.tm_min)) &&
                    (horaActual->tm_sec > listaProgramas[i].programacion.tm_sec )) {
                        
                        listaProgramas[i].programacion.tm_mday = listaProgramas[i].programacion.tm_mday + 7;
                    }
                     */
                    
                }
                break;
                
        }
        // Convierto la fecha calculada en un time_t para ordenarlo despues.
        //Adapto la hora a la zona horaria
        listaProgramas[i].programa = mktime(&listaProgramas[i].programacion);
        listaProgramas[i].programa = listaProgramas[i].programa -((clock->ntpTimeZone + clock->ntpSummerTime) * 3600);
        /*
        printf("ordenarListaProgramas-->%d/%02d/%02d %d %02d %02d:%02d:%02d %d\n",
                listaProgramas[i].programacion.tm_year + 1900,
                listaProgramas[i].programacion.tm_mon +1,
                listaProgramas[i].programacion.tm_mday,
                listaProgramas[i].programacion.tm_yday,
                listaProgramas[i].programacion.tm_wday,
                listaProgramas[i].programacion.tm_hour,
                listaProgramas[i].programacion.tm_min,
                listaProgramas[i].programacion.tm_sec,
                listaProgramas[i].programacion.tm_isdst);*/
        //applyTZ(&listaProgramas[i].programacion);
        
    }
    // Una vez rellena toda la lista de programacion, se ordena de menor a mayor.
    qsort(listaProgramas, nElementos, sizeof(TIME_PROGRAM),(compfn) compararDatos);
    //despues de ordenar
    //printf("ordenarListaProgramas-->Despues de ordenar...\n");
    /*
    for(i=0;i<nElementos;i++) {
        printf("ordenarListaProgramas-->%d/%02d/%02d %d %02d %02d:%02d:%02d %d\n",
                listaProgramas[i].programacion.tm_year + 1900,
                listaProgramas[i].programacion.tm_mon +1,
                listaProgramas[i].programacion.tm_mday,
                listaProgramas[i].programacion.tm_yday,
                listaProgramas[i].programacion.tm_wday,
                listaProgramas[i].programacion.tm_hour,
                listaProgramas[i].programacion.tm_min,
                listaProgramas[i].programacion.tm_sec,
                listaProgramas[i].programacion.tm_isdst);
        
    }
    
    */
    
    return 0;
}

bool buscarProgram(TIME_PROGRAM *listaProgramas, int nElementos, time_t fecha, int *actual, int* futuro) {
    

    int i;
    bool encontrado = FALSE;
    
    //Si no hay programas devolvemos un -1
    //En caso contrario, devuelve el indice del programa
    if (nElementos == 0) {
        *actual = -1;
        *futuro = *actual;
        return false;
    }
    

    //recorremos la lista que debe estar previamente ordenada en sentido ascendente
    for (i=0;i<nElementos;i++) {
        if((fecha < listaProgramas[i].programa) && (listaProgramas[i].activo == true)) {
            //printf("programa %d encontrado\n", i);
            //visualizartiempo(listaProgramas[i]);
            encontrado=true;
            break;
        }
    }
    

    if (encontrado == true) {
        *futuro = i;
        if (i == 0) {
            //Todos los programas son a futuro.
            //Actual lo ponemos a -1 para indicar que se debe aplicar el valor por defecto.
            //Esto solo debe pasar cuando se enciende el dispositivo y todos los valores son a futuro 
            //no encontrandose programacion activa
            *actual = -1;
           //Todos los elementos son a pasado.
        } else if (i == nElementos) {
            //Si hemos llegado al final de la lista, apuntamos al primero si éste es periodico.
            if ((listaProgramas[0].tipo == DIARIA) || (listaProgramas[0].tipo == SEMANAL)) {
                *futuro =0;
                *actual = nElementos - 1;
                
            }
            
                       
        } else {
            //programa mas habitual.
            *actual = *futuro -1;
        }
        printf("buscarProgram-->nregistros:%d, encontrado:%d, actual:%d, futuro %d \n", nElementos, i, *actual, *futuro);
        printf("buscarProgram-->Actual %d ",*actual );
        visualizartiempo(listaProgramas[*actual]);
        printf("buscarProgram-->Futuro %d ", *futuro);
        visualizartiempo(listaProgramas[*futuro]);
        
        return true;
        
    } else {
        if (nElementos == 1) {
            //Solo hay un elemento y es a pasado
            printf("buscarProgram-->Solo un elemento y es a pasado");
            *actual = *futuro;
        printf("buscarProgram-->Actual");
        visualizartiempo(listaProgramas[*actual]);
        printf("buscarProgram-->Futuro");
        visualizartiempo(listaProgramas[*futuro]);
            
            return true;
        }
        if ((listaProgramas[0].tipo == DIARIA) || (listaProgramas[0].tipo == SEMANAL)) {
            *futuro =0;
            *actual = nElementos - 1;

        }
        printf("buscarProgram-->programacion no encontrada. Todos son a pasado\n");
        return false;
    }
    
/*    
     case DEFECTO:
         //Se han borrado todos los programas. Se asigna el de defecto
         datosApp->tempUmbral = datosApp->tempUmbralDefecto;
    
*/    
    
}

int buscarPrograma(TIME_PROGRAM *listaProgramas, int nElementos, time_t fecha) {
    
    //time_t fecha;
    int i;
    //fecha = mktime(horaActual);
    bool encontrado = FALSE;
    
    //Si no hay programas devolvemos un -1
    if (nElementos == 0) return -2;
    //recorremos la lista que debe estar previamente ordenada en sentido ascendente
    for (i=0;i<nElementos;i++) {
        if((fecha < listaProgramas[i].programa) && (listaProgramas[i].activo == true)) {
            printf("programa %d encontrado\n", i);
            visualizartiempo(listaProgramas[i]);
            encontrado=true;
            break;
        }
    }
    
    
    if (encontrado == true) return i;
    else return -1;
    
    
}

void visualizartiempo(TIME_PROGRAM fecha) {
    
    char tipoPrograma[10];
    
    printf("%02d/%02d/%d %02d:%02d:%02d diaSem: %d, dias del ano: %d ", 
            fecha.programacion.tm_mday, fecha.programacion.tm_mon + 1, fecha.programacion.tm_year + 1900, 
            fecha.programacion.tm_hour, fecha.programacion.tm_min, fecha.programacion.tm_sec, fecha.programacion.tm_wday, 
            fecha.programacion.tm_yday);
    
    switch (fecha.tipo) {
        case DIARIA:
            strcpy(tipoPrograma, "DIARIA");
            break;
        case SEMANAL:
            strcpy(tipoPrograma, "SEMANAL");
            break;
        case FECHADA:
            strcpy(tipoPrograma, "FECHADA");
    }
    printf("fecha time_t : %d, activo: %d, tipo: %s \n", fecha.programa, fecha.activo, tipoPrograma);
                              
               
               
}

struct TIME_PROGRAM * borrarPrograma(struct TIME_PROGRAM *listProgram, uint8 *elements, uint8 element) {
    
    uint8 i;
    struct TIME_PROGRAM *list;
    //printf("tamaño antes: %d\n", sizeof(*listProgram));
    //printf("posicion encontrada:%d, totalElementos:%d\n", element, *elements);
    for (i=element; i< *elements - 1; i++) {
        memcpy(listProgram+element, listProgram+element+1, sizeof(*listProgram));
    }
    *elements = *elements - 1;
    
    list = (TIME_PROGRAM*) realloc(listProgram, sizeof(*listProgram) * (*elements) );
    
    listProgram = list;
    return list;
}


uint8 modificarElemento(struct TIME_PROGRAM *listProgram, uint8 elements, struct TIME_PROGRAM *idPrograma) {
    
    uint8 i;
    
    for(i=0;i<elements;i++) {
        if (idPrograma->programa == listProgram[i].programa) {
            memcpy(&(listProgram[i]), idPrograma, sizeof(struct TIME_PROGRAM));
            printf("modificarElemento--> Programacion modificada\n");
            return 0;
        }
    }
    
    
    return -1;
}

bool buscarProgramaDuplicado(char* idPrograma, struct TIME_PROGRAM *program, uint8 elements) {
    
    if(elements == 0) {
        printf("buscarProgramaDuplicado--> primer elemento, se devuelve false\n");
        return false;
    }
    
    if (localizarProgramaPorId(idPrograma, program, elements) < 0 ) {
        return false;
    } else {
        printf("buscarProgramaDuplicado--> Registro %s duplicado. No se inserta\n", idPrograma);
        return true;
    }
/*
    uint8 i;
    
    for(i=0;i<elements;i++) {
        printf("idPrograma: %s, idProgramaInsertado:%s\n", idPrograma, program[i].idPrograma);
        if (strcmp(idPrograma, program[i].idPrograma) == 0) {
            printf("buscarProgramaDuplicado:Registro %s duplicado. No se inserta\n", idPrograma);
            return false;
        }
    }
*/    
    return true;
}

void generarIdPrograma(struct TIME_PROGRAM *program) {
    
    if( program->tipo == DIARIA) {

        sprintf(program->idPrograma, "%02d%02d%02d%02d", 
                program->tipo, program->programacion.tm_hour,
                program->programacion.tm_min,program->programacion.tm_sec );
        
    } else {
        sprintf(program->idPrograma, "%02d%02d%02d%02d%02d", 
                program->tipo, program->programacion.tm_hour,
                program->programacion.tm_min,program->programacion.tm_sec,
                program->programacion.tm_wday);

        
    }
    
}

int localizarProgramaPorId(char *idPrograma, struct TIME_PROGRAM *program, uint8 elements) {

    uint8 i;
    
    //if (elements == 0) return 0;
    for(i=0;i<elements;i++) {
        //printf("idPrograma: %s, idProgramaExistente:%s\n", idPrograma, program[i].idPrograma);
        if (strcmp(idPrograma, program[i].idPrograma) == 0) {

            return i;
        }
    }
    printf("localizarProgramaPorId--> registro no localizado\n");
    return -1;

    
}
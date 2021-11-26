/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "programmer.h"
#include "user_config.h"




//#define uint8 uint8_t


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


TIME_PROGRAM* crearPrograma(TIME_PROGRAM *peticion, TIME_PROGRAM *listaProgramas, uint8 *nElementos) {
    
    TIME_PROGRAM *list;
    uint8 elem;
    elem = *nElementos;
    generarIdPrograma(peticion);
    //Si se encuentra duplicado, no se crea el programa
    if (buscarProgramaDuplicado(peticion->idPrograma, listaProgramas, elem) == true) {
        printf("crearPrograma-->Programa no creado\n");
        return listaProgramas;
    }
    
    //Primero insertamos el elemento en la lista
    list = insertarElemento(listaProgramas, (uint8*)nElementos);
    if (list == NULL) return NULL;
    
    
    //Lo actualizamos con la informacion de la peticion.
    memcpy(&list[elem], peticion, sizeof(*list));
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
    
bool calcularMascara(int diaActual, int mascara) {
    
    int i;
    int j=1;
    int k;
    printf("calcularMascara--> dia actual: %d, mascara : %d\n", diaActual, mascara);
    for (i=0;i<diaActual;i++) {
        
        j = j << 1;      
    }
    
    
    k= j & mascara;
    
    if (k == j) {
        printf("calcularMascara-->programacion activa %d\n", k);
        return true;
    } else {
        printf("calcularMascara-->programacion inactiva %d\n", k);
        return false;
    }

}

uint8 ordenarListaProgramas(TIME_PROGRAM *listaProgramas, int nElementos, struct NTP_CLOCK *clock) {
    
    int i, j;
    struct tm *horaActual;
    horaActual = &clock->date;
    
    // Añado los atributos necesarios en funcion del tipo de programacion para convertirlo en time_t valido.
    
    for(i=0;i<nElementos;i++) {
        //listaProgramas[i].programacion.tm_year = horaActual->tm_isdst;
        printf("ordenarListaProgramas--> El ano es: %d\n", horaActual->tm_year);
        switch (listaProgramas[i].tipo) {
            case DIARIA:
                listaProgramas[i].programacion.tm_year = horaActual->tm_year;
                listaProgramas[i].programacion.tm_mon = horaActual->tm_mon;
                listaProgramas[i].programacion.tm_mday = horaActual->tm_mday;
                listaProgramas[i].programacion.tm_wday = horaActual->tm_wday;
                if (calcularMascara(horaActual->tm_wday, listaProgramas[i].mascara) == true) {
                    listaProgramas[i].activo = 1;
                } else {
                    listaProgramas[i].activo = 0;
                }
                
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

                    
                }
                break;
                
            case FECHADA:
                printf("ordenarListaProgramas--> Pinto la fechada\n");
                //listaProgramas[i].programacion.tm_yday = 0;
                break;
                
        }
        // Convierto la fecha calculada en un time_t para ordenarlo despues.
        //Adapto la hora a la zona horaria
        listaProgramas[i].programa = mktime(&listaProgramas[i].programacion);
        listaProgramas[i].programa = listaProgramas[i].programa -((clock->ntpTimeZone + clock->ntpSummerTime) * 3600);
        printf("ordenarListaProgramas-->tPrograma : %d, tActual: %d, dif: %d\n", listaProgramas[i].programa, clock->time,listaProgramas[i].programa - clock->time );
        
    }
    // Una vez rellena toda la lista de programacion, se ordena de menor a mayor.
    qsort(listaProgramas, nElementos, sizeof(TIME_PROGRAM),(compfn) compararDatos);
    //despues de ordenar
    //printf("ordenarListaProgramas-->Despues de ordenar...\n");
    
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
    
    
    
    return 0;
}

int calcularProgramaActivoActual(TIME_PROGRAM *listaProgramas, int nElementos, int actual) {
    
    int i;

    
    for (i=actual; i > -1; i--) {
        printf("calcularProgramaActivoActual-->actual: %d\n", i);
        if ((listaProgramas[i].activo == 0) || (listaProgramas[i].estadoPrograma == INACTIVO)) {
            visualizartiempo(&listaProgramas[i]);
             actual --;
           
        } else {
            printf("calcularProgramaActivoActual-->encontrado actual: %d\n", i);
            return i;
        }
    }
    printf("calcularProgramaActivoActual--> actual no encontrado. Retornado: %d\n", i);
    return i;
    

    
}

bool esActivoElPrograma(TIME_PROGRAM *listaProgramas, int nElementos, int indice) {
    
    if ((listaProgramas->activo == 1) && (listaProgramas->estadoPrograma == ACTIVO)) {
        printf("esActivoElPrograma--> activo, idPrograma: %s, indice:%d\n", listaProgramas->idPrograma, indice);
        
        return TRUE;
    } else {
        printf("esActivoElPrograma--> inactivo, idPrograma: %s, indice: %d\n", listaProgramas->idPrograma, indice);
        return FALSE;
    }
    
    
}


bool buscarPrograma(DATOS_APLICACION *datosApp, int *actual) {
    

    int i;
    bool encontrado = FALSE;
    int programaActivo = -1;
    
    printf("buscarPrograma-->inicio\n");
    printf("buscarPrograma--> nProgramas: %d\n", datosApp->nProgramacion);
    
    if(datosApp->nProgramacion == 0) {
        *actual = -1;
        datosApp->nProgramaCandidato = -1;
        printf("buscarPrograma--> (1) nProgramas: %d\n", datosApp->nProgramacion);
        return FALSE;
    }
    
    
    if (datosApp->nProgramacion == 1) {
        //Si la fecha actual es menor que la programada. A futuro...
        if((datosApp->clock.time < datosApp->programacion[i].programa)) {
            if (esActivoElPrograma(&datosApp->programacion[i], datosApp->nProgramacion, 0)) {
                programaActivo = 0;
                *actual = -1;
                datosApp->nProgramaCandidato = 0;
                printf("buscarPrograma--> a futuro (2), actual: %d, futuro: %d\n ", *actual, datosApp->nProgramaCandidato);
                return FALSE;
            } else {
                *actual = -1;
                datosApp->nProgramaCandidato = -1;
                printf("buscarPrograma--> (3), actual: %d, futuro: %d\n ", *actual, datosApp->nProgramaCandidato);
                
                return FALSE;
            }
            // Un solo elemento a pasado. Aun no se ha activado porque no ha llegado la fecha
        } else {
            if (esActivoElPrograma(&(datosApp->programacion[i]), datosApp->nProgramacion, 0)) {
                programaActivo = 0;
                *actual = 0;
                datosApp->nProgramaCandidato = 0;
                printf("buscarPrograma--> a pasado (4), actual: %d, futuro: %d\n ", *actual, datosApp->nProgramaCandidato);
                
                return TRUE;
                
            } else {
                *actual = -1;
                datosApp->nProgramaCandidato = -1;
                printf("buscarPrograma--> (5), actual: %d, futuro: %d\n ", *actual, datosApp->nProgramaCandidato);
                
                return FALSE;
            }
        }
    }
    
    // Si encontramos un programa en el medio, el actual apunta al programa activo mas cercano
    // y el futuro al siguiente
    for (i=0;i<datosApp->nProgramacion;i++) {
        printf("buscarProgramas-> bucle: %d\n", i);
        if (esActivoElPrograma(&(datosApp->programacion[i]), datosApp->nProgramacion, i) == TRUE) {
            programaActivo = i;
            printf("buscarPrograma--> programaActivo vale: %d\n", programaActivo);
            if (datosApp->clock.time < datosApp->programacion[i].programa) {
                //*actual = programaActivo;
                datosApp->nProgramaCandidato = programaActivo;
                // Si es el ultimo programa activo, lo dejamos hasta que llegue el siguiente del dia siguiente
                if (programaActivo == 0) {
                    *actual = datosApp->nProgramacion - 1;
                } else {
                    *actual = calcularProgramaActivoActual(datosApp->programacion, datosApp->nProgramacion, i-1);

                }
                printf("buscarPrograma--> (6), actual: %d, futuro: %d\n ", *actual, datosApp->nProgramaCandidato);

                return TRUE;
            }
        }
    }
    i--;
    // Si estamos aqui es porque no se ha encontrado ningun programa a futuro.
    printf("buscarPrograma--> no se ha encontrado ningun programa a futuro: progActivo vale:%d, i vale %d\n", programaActivo, i);
    if (esActivoElPrograma(&(datosApp->programacion[i]), datosApp->nProgramacion, i) == TRUE) {
        //Si el ultimo programa fuera activo, el futuro apunta al primero
        if (programaActivo == i) {
            *actual = programaActivo;
            datosApp->nProgramaCandidato = 0;                
            printf("buscarPrograma--> (7), actual: %d, futuro: %d\n ", *actual, datosApp->nProgramaCandidato);
            return TRUE;
        } else {
            *actual = programaActivo;
            datosApp->nProgramaCandidato = *actual + 1;
            printf("buscarPrograma--> (8), actual: %d, futuro: %d\n ", *actual, datosApp->nProgramaCandidato);
            
            return TRUE;
        }
    } else {
        *actual = programaActivo;
        datosApp->nProgramaCandidato = *actual + 1;
        printf("buscarPrograma--> (8), actual: %d, futuro: %d\n ", *actual, datosApp->nProgramaCandidato);
        return TRUE;
    }
   

}

/*
bool buscarProgram(TIME_PROGRAM *listaProgramas, int nElementos, time_t fecha, int *actual, int* futuro) {
    

    int i;
    bool encontrado = FALSE;
    
    //Si no hay programas devolvemos un -1
    //En caso contrario, devuelve el indice del programa
    if (nElementos == 0) {
        *actual = -1;
        *futuro = *actual;
        printf("buscarProgram--> El numero de programas es 0, actual: %d, futuro: %d\n", *actual, *futuro);
        return FALSE;
    }
    

    //recorremos la lista que debe estar previamente ordenada en sentido ascendente
    for (i=0;i<nElementos;i++) {
        //printf("buscarProgram-->i vale %d y encontrado vale %d. Fecha actual:%d, programada: %d\n", i, encontrado, fecha, listaProgramas[i].programa );
        if((fecha < listaProgramas[i].programa) && (listaProgramas[i].activo == 1) && (listaProgramas[i].estadoPrograma == ACTIVO)) {
            //printf("buscarProgram-->programa %d encontrado\n", i);
            //visualizartiempo(listaProgramas[i]);
            printf("buscarProgram--> El numero de programas es %d, actual: %d, futuro: %d\n", i, *actual, *futuro);

            encontrado=TRUE;
            break;
        }
    }

    //printf("buscarProgram--> Final: Elementos totales : %d, i vale %d y encontrado vale %d. Fecha actual:%d, programada: %d\n", nElementos, i, encontrado, fecha, listaProgramas[i-1].programa );

    //if ((encontrado == FALSE) && (listaProgramas[i-1].activo == 1) && (listaProgramas[i-1].estadoPrograma == ACTIVO)){
    if (encontrado == FALSE) {
        //Probado ok 30/03/2019
        if (i == 1) {
            *futuro =0;
            *futuro = calcularProgramaActivoActual(listaProgramas, nElementos, *futuro);
            *actual = calcularProgramaActivoActual(listaProgramas, nElementos, *futuro);
            //*actual = 0;
            
            //printf("buscarProgram-->Solo un elemento y la fecha actual es mayor que la programada\n");
            //printf("buscarProgram-->encontrado false, i = 1: Programa actual: %d, futuro: %d\n", *actual, *futuro);
            //visualizartiempo(listaProgramas[*actual]);
            printf("buscarProgram--> Solo hay %d programa, actual: %d, futuro: %d\n", i, *actual, *futuro);

            return TRUE;
        }
        if (i == nElementos) {
            //printf("buscarProgram-->Hemos llegado al ultimo y apuntamos al primero porque ya han pasado todos los programas.\n");
            *futuro = nElementos;
            *futuro = calcularProgramaActivoActual(listaProgramas, nElementos, *futuro - 1);
            //*futuro = i - 1;
            *actual = calcularProgramaActivoActual(listaProgramas, nElementos, *futuro);
            //*actual = *futuro;
            //printf("buscarProgram-->encontrado false, i = nElementos: Programa actual: %d, futuro: %d\n", *actual, *futuro);
            //visualizartiempo(listaProgramas[*actual]);
            printf("buscarProgram--> El numero de programas es %d, actual: %d, futuro: %d\n", i, *actual, *futuro);
            return TRUE;
            
        }
        // encontrado == TRUE
    } else {
        
        *futuro = i;
        *actual = calcularProgramaActivoActual(listaProgramas, nElementos, *futuro - 1);
        printf("buscarProgram--> El numero de programas es %d, actual: %d, futuro: %d\n", i, *actual, *futuro);

        //*actual = *futuro -1;
        //printf("buscarProgram-->encontrado true,: Programa actual: %d, futuro: %d\n", *actual, *futuro);
        if (*actual >= 0 ) {
            visualizartiempo(listaProgramas[*actual]);
            
        }
        return TRUE;
    } 
}

    
    
/*    
    
    

    if (encontrado == true) {
        *futuro = i;
        if (i == 0) {
            printf("buscarProgram-->nregistros:%d, encontrado:%d, actual:%d, futuro %d \n", nElementos, i, *actual, *futuro);
            printf("buscarProgram-->Actual %d \n",-1 );
            
            //Todos los programas son a futuro.
            //Actual lo ponemos a -1 para indicar que se debe aplicar el valor por defecto.
            //Esto solo debe pasar cuando se enciende el dispositivo y todos los valores son a futuro 
            //no encontrandose programacion activa
            *actual = -1;
            printf("buscarProgram-->Futuro %d \n", *futuro);
            visualizartiempo(listaProgramas[*futuro]);
           //Todos los elementos son a pasado.
        } else if (i == nElementos) {
            //Si hemos llegado al final de la lista, apuntamos al primero si éste es periodico.
            printf("buscarProgram-->Hemos llegado al ultimo y apuntamos al primero\n");
            if ((listaProgramas[0].tipo == DIARIA) || (listaProgramas[0].tipo == SEMANAL)) {
                *futuro =0;
                *actual = nElementos - 1;
                printf("buscarProgram-->nregistros:%d, encontrado:%d, actual:%d, futuro %d \n", nElementos, i, *actual, *futuro);
                printf("buscarProgram-->Actual %d \n",*actual );
                visualizartiempo(listaProgramas[*actual]);
                printf("buscarProgram-->Futuro %d \n", *futuro);
                visualizartiempo(listaProgramas[*futuro]);
                
            }
            
                       
        } else {
            printf("buscarProgram-->programa mas habitual\n");
            //programa mas habitual.
            *actual = *futuro -1;
            printf("buscarProgram-->nregistros:%d, encontrado:%d, actual:%d, futuro %d \n", nElementos, i, *actual, *futuro);
            printf("buscarProgram-->Actual %d \n",*actual );
            visualizartiempo(listaProgramas[*actual]);
            printf("buscarProgram-->Futuro %d \n", *futuro);
            visualizartiempo(listaProgramas[*futuro]);

        }
        if (nElementos == 1) {
            //Solo hay un elemento y es a pasado
            printf("buscarProgram-->Solo un elemento y es a pasado\n");
            *futuro = 0;
            *actual = *futuro;
            printf("buscarProgram-->Actual\n");
            visualizartiempo(listaProgramas[*actual]);
            printf("buscarProgram-->Futuro\n");
            visualizartiempo(listaProgramas[*futuro]);
        }
            
        return true;

        
    } else {
        if (nElementos == 1) {
            //Solo hay un elemento y es a pasado
            printf("buscarProgram-->Solo un elemento y es a pasado\n");
            *futuro = 0;
            *actual = *futuro;
            printf("buscarProgram-->Actual\n");
            visualizartiempo(listaProgramas[*actual]);
            printf("buscarProgram-->Futuro\n");
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
   
    
}
*/
void visualizartiempo(TIME_PROGRAM *fecha) {
    
    char tipoPrograma[10];
    
    printf("visualizartiempo-->%02d/%02d/%d %02d:%02d:%02d diaSem: %d, dias del ano: %d, ", 
            fecha->programacion.tm_mday, fecha->programacion.tm_mon + 1, (fecha->programacion.tm_year + 1900), 
            fecha->programacion.tm_hour, fecha->programacion.tm_min, fecha->programacion.tm_sec, fecha->programacion.tm_wday, 
            fecha->programacion.tm_yday);
    
    switch (fecha->tipo) {
        case DIARIA:
            strcpy(tipoPrograma, "DIARIA");
            break;
        case SEMANAL:
            strcpy(tipoPrograma, "SEMANAL");
            break;
        case FECHADA:
            strcpy(tipoPrograma, "FECHADA");
    }
    printf("fecha time_t : %d, activo: %d, tipo: %s, estadoPrograma: %d \n", fecha->programa, fecha->activo, tipoPrograma, fecha->estadoPrograma);
                              
               
               
}

struct TIME_PROGRAM * borrarPrograma(struct TIME_PROGRAM *listProgram, uint8 *elements, uint8 element) {
    
    uint8 i;
    struct TIME_PROGRAM *list = NULL;
    
    /*
    uint8 nProgramas=0;
    //printf("tamaño antes: %d\n", sizeof(*listProgram));
    printf("borrarPrograma-->posicion encontrada:%d, totalElementos:%d\n", element, *elements);
    for (i=0;i<*elements;i++) {
        if (i != element) {
            list = crearPrograma(listProgram[i], list, &nProgramas);
        } else {
            printf("borrarPrograma-->elemento %d borrado\n", i);
            
        }
    }
    *elements = nProgramas;
    free(listProgram);
    return list;
    
    
/*   */ 
    
    for (i=element; i< *elements - 1; i++) {
        memcpy(listProgram+i, listProgram+i+1, sizeof(*listProgram));
        printf("pongo el elemento %d en el %d", i, i+1);
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

TIME_PROGRAM* generarIdPrograma(struct TIME_PROGRAM *program) {
    
    char mascara[2];
    uint8 i;
    
    for (i=0;i<19;i++) {
        program->idPrograma[i] = '\0';
    }
    
    
    switch (program->tipo) {
        case DIARIA:
            sprintf(mascara, "%x", program->mascara);
            printf("generarIdPrograma-->Generando diaria\n");
            sprintf(program->idPrograma, "%02d%02d%02d%02d%02s", 
                program->tipo, program->programacion.tm_hour,
                program->programacion.tm_min,program->programacion.tm_sec, mascara );
            break;
            
        case SEMANAL:
            printf("generarIdPrograma-->Generando semanal\n");
            sprintf(program->idPrograma, "%02d%02d%02d%02d%02d", 
                program->tipo, program->programacion.tm_hour,
                program->programacion.tm_min,program->programacion.tm_sec,
                program->programacion.tm_wday);
            break;
            
        case FECHADA:
            printf("generarIdPrograma-->Generando fechada\n");
            sprintf(program->idPrograma, "%02d%02d%02d%02d%04d%02d%02d", 
                program->tipo, program->programacion.tm_hour,
                program->programacion.tm_min,program->programacion.tm_sec,
                program->programacion.tm_year, program->programacion.tm_mon,
                program->programacion.tm_mday);

            break;
            
        default:
            break;
    }
    printf("generarIdPrograma--> Generado id:%s\n", program->idPrograma);
    visualizartiempo(program);
    
    return program;
    
/*    
    if( program->tipo == DIARIA) {
        

        sprintf(mascara, "%x", program->mascara);
        printf("generarIdPrograma-->Generando diaria\n");
        sprintf(program->idPrograma, "%02d%02d%02d%02d%02s", 
                program->tipo, program->programacion.tm_hour,
                program->programacion.tm_min,program->programacion.tm_sec, mascara );
        
    } else {
        printf("generarIdPrograma-->Generando semanal\n");
        sprintf(program->idPrograma, "%02d%02d%02d%02d%02d", 
                program->tipo, program->programacion.tm_hour,
                program->programacion.tm_min,program->programacion.tm_sec,
                program->programacion.tm_wday);

        
    }
    
    printf("generarIdPrograma--> Generado id:%s\n", program->idPrograma);
    
    return program;
 * 
 */
    
}

int localizarProgramaPorId(char *idPrograma, struct TIME_PROGRAM *program, uint8 elements) {

    uint8 i;
    char *cadena1;
    char *cadena2;
    char *tipo;
    cadena1 = (char*) zalloc(19);
    cadena2 = (char*) zalloc(19);
    tipo = (char*) zalloc(3);
    
    strncpy(tipo, idPrograma, 2);
    printf("localizarProgramaPorId--> tipo programa es %s\n", tipo);
    if ((strcmp(tipo, "00") == 0) || (strcmp(tipo, "01") == 0)) {
        //Diaria o semanal
        printf("localizarProgramaPorId--> Programacion diaria o semanal\n");
        strncpy(cadena1, idPrograma, 8);
        for(i=0;i<elements;i++) {
            strncpy(cadena2, program[i].idPrograma, 8);
             printf("localizarProgramaPorId--> comparamos %s con %s\n", cadena1, cadena2);
            if (strcmp(cadena1, cadena2) == 0) {
                free(cadena1);
                free(cadena2);
                free(tipo);
                printf("localizarProgramaPorId--> registro duplicado\n");
                return i;
            }
    }
        
        
    } else {
        //Fechada
        printf("localizarProgramaPorId--> Programacion fechada\n");
        strcpy(cadena1, idPrograma);
        for(i=0;i<elements;i++) {
            strcpy(cadena2, program[i].idPrograma);
             printf("localizarProgramaPorId--> comparamos %s con %s\n", cadena1, cadena2);
            if (strcmp(cadena1, cadena2) == 0) {
                free(cadena1);
                free(cadena2);
                free(tipo);
                printf("localizarProgramaPorId--> registro duplicado\n");
                return i;
            }
    }
        
        
    }

    printf("localizarProgramaPorId--> registro no localizado\n");
    free(cadena1);
    free(cadena2);
    free(tipo);
    return -1;
    
    
    
/*

    for(i=0;i<elements;i++) {
        strcpy(cadena2, program[i].idPrograma);
         printf("localizarProgramaPorId--> comparamos %s con %s\n", cadena1, cadena2);
        if (strcmp(cadena1, cadena2) == 0) {
            free(cadena1);
            free(cadena2);
            printf("localizarProgramaPorId--> registro duplicado\n");
            return i;
        }
    }
    printf("localizarProgramaPorId--> registro no localizado\n");
    free(cadena1);
    free(cadena2);
    return -1;
*/
    
}

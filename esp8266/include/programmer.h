/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   programmer.h
 * Author: emilio
 *
 * Created on 9 de agosto de 2017, 21:36
 */


//#include <time.h>
//#include <stdio.h>
//#include <stdint.h>
#include <time.h>
//#include <stdlib.h>
//#include <string.h>

//#include "esp_common.h"
//#include "user_config.h"
#include "user_config.h"
/**
 * @brief programmer.h
 * libreria para poder crear programaciones que activen eventos.
 * Se pueden realizar programaciones diarias, semanales o de una fecha concreta.
 * 
 *
 * Se utiliza para dar la capacidad de realizar programaciones todos los dias, semanales o para una fecha concreta.
 */





#ifndef PROGRAMMER_H
#define PROGRAMMER_H

//#include "ntp.h"

//#define uint8 uint8_t
/*
typedef struct NTP_CLOCK {
    struct tm date;
    int ntpTimeZone;
    int ntpTimeExp;
    int ntpTimeOut;
    uint8 ntpServer[4];
    
} NTP_CLOCK;                                       
*/
/**
 * @brief TIPO_PROGRAMA 
 * especifica los tipos de programaciones que podemos hacer
 *
 * Se utiliza para dar la capacidad de realizar programaciones todos los dias, semanales o para una fecha concreta.
 */

typedef enum PROG_STATE {
    INVALID_PROG, 
    VALID_PROG,
    INH_PROG

}PROG_STATE;
                    
typedef enum ESTADO_PROGAMA {
    INACTIVO, ACTIVO
            
} ESTADO_PROGAMA;            


typedef enum TIPO_PROGRAMA {
                DIARIA,
                SEMANAL,
                FECHADA
}TIPO_PROGRAMA;

#ifndef TERMOSTATO

typedef struct TIME_PROGRAM {
                char idPrograma[19];
                struct tm programacion;
                enum TIPO_PROGRAMA tipo;
                time_t programa;
                uint8 activo;
                int accion;
                uint8 mascara;
                enum ESTADO_PROGAMA estadoPrograma;
                uint32 duracion;
               
}TIME_PROGRAM;

#else
typedef struct TIME_PROGRAM {
                char idPrograma[19];
                struct tm programacion;
                enum TIPO_PROGRAMA tipo;
                time_t programa;
                uint8 activo;
                int accion;
                float temperatura;
                float humedad;
                uint8 mascara;
                enum ESTADO_PROGAMA estadoPrograma;
                uint32 duracion;
               
}TIME_PROGRAM;

#endif

// Prototipo para la funcion de ordenacion de la lista de programacion.
typedef int (*compfn)(const void*, const void*);

/**
  * @brief     Compara dos elementos del array de programas para ordenar la lista.
  * se ayuda de la funcion qsort que realiza la funcion.
  * @attention 
  *
  * @param     dat1 es un elemento del array a comparar
  * @param     dat2 es el otro elemento a comparar.
  * 
  *
  * @return    Devuelve 1 cuando dat1 > dat2
  * @return    Devuelve -1 cuando dat1 < dat2
  * @return    Devuelve 0 cuando son iguales.
  */

int compararDatos(struct TIME_PROGRAM *dat1, struct TIME_PROGRAM *dat2);


/**
  * @brief     Inserta un elemento en la lista de programas.
  *
  * @attention Si la funciona retorna NULL debera abortarse la ejecucion porque
  * no habra podido asignar memoria
  *
  * @param     *listProgram es el array que contiene la lista de programas
  * @param      *elements indica el numero de elementos del array. Es de vuelta para actualizar el numero de elementos.
  * 
  *
  * @return    Devuelve un puntero a la propia lista
  * @return    NULL cuando no se ha podido crear o aumentar la lista.
  */
struct TIME_PROGRAM* insertarElemento(struct TIME_PROGRAM *listProgram, uint8 *elements);

/**
  * @brief     Inserta un elemento nuevo en la lista de programaciones.
  *
  * @attention Si la funcion devuelve 0 deberá abortarse la ejecucion porque no se ha podido insertar el elemento.
  *
  * @param     *listProgram es el array que contiene la lista de programas
  * @param      nElementos indica el numero de elementos del array. Es de vuelta para actualizar el numero de elementos.
  * 
  *
  * @return    Devuelve NULL si hay un error al insertar el elemento.
  * @return    el puntero a la lista cuando la creacion se ha realizado con exito.
  */
TIME_PROGRAM* crearPrograma(TIME_PROGRAM *peticion, TIME_PROGRAM *listaProgramas, uint8 *nElementos);

/**
  * @brief     Ordena la lista de programacion de menor a mayor con el objetivo de poder
  * saber quien es el elemento de programacion mas proximo al la hora actual.
  *
  * @attention 
  *
  * @param     *listProgram es el array que contiene la lista de programas
  * @param     nElementos indica el numero de elementos del array.
  * @param     horaActual indica la hora actual como referencia para rellenar los datos de programacion
  * y podera asi ordenar la lista.
  * 
  *
  * @return    Devuelve 0 si hay un error al insertar el elemento.
  * @return    > o cuando la creacion se ha realizado con exito.
  */

uint8 ordenarListaProgramas(TIME_PROGRAM *listaProgramas, int nElementos, struct NTP_CLOCK *clock);

/**
  * @brief Busca el elemento inmediatamente superior a la fecha actual. 
  *
  * @attention 
  *
  * @param     *listProgram es el array que contiene la lista de programas
  * @param     nElementos indica el numero de elementos del array.
  * @param     horaActual indica la hora actual como referencia para rellenar los datos de programacion
  * y podera asi ordenar la lista.
  * 
  *
  * @return    Devuelve -1 si hay un error al insertar el elemento.
  * @return    > o cuando la creacion se ha realizado con exito.
  */

//int buscarPrograma(TIME_PROGRAM *listaProgramas, int nElementos, time_t fecha);

/**
  * @brief Borra un elemento del array dado por element y lo redimensiona. 
  *
  * @attention 
  *
  * @param     *listProgram es el array que contiene la lista de programas
  * @param     *elements indica el numero de elementos del array.
  * @param     element es el indice del elemento a borrar.
  * 
  *
  * @return    Devuelve NULL si hay un error al borrar el elemento.
  * @return    putero a la lista  del array despues del borrado.
  */


struct TIME_PROGRAM * borrarPrograma(struct TIME_PROGRAM *listProgram, uint8 *elements, uint8 element);
/**
  * @brief Modifica un elemento del array dado por idPrograma 
  *
  * @attention 
  *
  * @param     *listProgram es el array que contiene la lista de programas
  * @param     *elements indica el numero de elementos del array.
  * @param     idPograma es el indice del elemento a modificar.
  * 
  *
  * @return    Devuelve 0 si hay un error al insertar el elemento.
  * @return    numero de elementos del array despues del borrado.
  */

uint8 modificarElemento(struct TIME_PROGRAM *listProgram, uint8 elements, struct TIME_PROGRAM *idPrograma);
void visualizartiempo(TIME_PROGRAM *fecha);

bool buscarProgramaDuplicado(char* idPrograma, struct TIME_PROGRAM *program, uint8 elements);
TIME_PROGRAM* generarIdPrograma(struct TIME_PROGRAM *program);
int localizarProgramaPorId(char *idPrograma, struct TIME_PROGRAM *program, uint8 elements);
uint8 InicializarListaProgramas(TIME_PROGRAM *listaProgramas, int nElementos, struct NTP_CLOCK *clock);
//bool buscarProgram(TIME_PROGRAM *listaProgramas, int nElementos, time_t fecha, int *actual, int* futuro);

//bool buscarProg(DATOS_APLICACION *datosApp, int *actual);



#endif /* PROGRAMMER_H */


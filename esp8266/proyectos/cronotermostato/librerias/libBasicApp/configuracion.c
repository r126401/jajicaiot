/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "configuracion.h"

#include "user_config.h"

//#define DEFECTO 0X1113

#define TAM_DATOS_APP sizeof(*configuracion) + ((sizeof(TIME_PROGRAM) * configuracion->nProgramacion))

bool salvarConfiguracionMemoria(DATOS_APLICACION *configuracion) {
    
   
    uint8 sectorActivo=0;
    SpiFlashOpResult Resultado;
    uint32 dato;
    
        
    
    
    // 1 recibir configuracion

    dato = 0;
    Resultado = spi_flash_read((SEGMENTO_USUARIO + 3) * SPI_FLASH_SEC_SIZE,
            (uint32*) &dato,
            sizeof(uint32));
    
    
    if (Resultado != SPI_FLASH_RESULT_OK) {
        DBG("salvarConfiguracionMemoria-->Error en la configuracion. No pueden salvarse los datos. Error:%d \n", Resultado);
        return FALSE;
    }
    
    //Compruebo quien es el ultimo sector en el que escribi
    if (dato == 0) {

        // Borro el sector 1 antes de escribir 
        spi_flash_erase_sector(SEGMENTO_USUARIO + 1);
        //Escribo el texto en el  sector 1

        Resultado = spi_flash_write((SEGMENTO_USUARIO + 1) * SPI_FLASH_SEC_SIZE,
                (uint32*) configuracion,
                TAM_DATOS_APP);
        if (Resultado != SPI_FLASH_RESULT_OK) {
        DBG("salvarConfiguracionMemoria-->Error en la configuracion. No pueden salvarse los datos. Error:%d \n", Resultado);
        return FALSE;
    }

        //Actualizo el flag al  sector 1 como el ultimo en el que escribi
        dato = 1;
        // Borro el sector 3 para actualizarlo
        spi_flash_erase_sector(SEGMENTO_USUARIO + 3);
        // Escribo el sector 3 con el flag a 1
        Resultado = spi_flash_write((SEGMENTO_USUARIO + 3)* SPI_FLASH_SEC_SIZE,
                (uint32*) &dato,
                sizeof(uint32));
         if (Resultado != SPI_FLASH_RESULT_OK) {
        DBG("salvarConfiguracionMemoria-->Error en la configuracion. No pueden salvarse los datos. Error:%d \n", Resultado);
        return FALSE;
    }

        DBG("salvarConfiguracionMemoria-->Salvado en el segmento 1\n. El tamaNo de TERMOSTATO es %d\n", sizeof(*configuracion));
        
        
    } else {


         // Borro el sector 0 antes de escribir 
        spi_flash_erase_sector(SEGMENTO_USUARIO + 0);
        //Escribo el texto en el  sector 0


        Resultado = spi_flash_write((SEGMENTO_USUARIO + 0) * SPI_FLASH_SEC_SIZE,
                (uint32*) configuracion,
                TAM_DATOS_APP);
        if (Resultado != SPI_FLASH_RESULT_OK) {
        DBG("Error en la configuracion. No pueden salvarse los datos. Error:%d \n", Resultado);
        return FALSE;
    }

        //Actualizo el flag al  sector 0 como el ultimo en el que escribi
        dato = 0;
        // Borro el sector 3 para actualizarlo
        spi_flash_erase_sector(SEGMENTO_USUARIO + 3);
        // Escribo el sector 3 con el flag a 0
        Resultado=spi_flash_write((SEGMENTO_USUARIO + 3)* SPI_FLASH_SEC_SIZE,
                (uint32*) &dato,
                sizeof(uint32));
        
      DBG("Salvado en el segmento 0\n. El tamaño de TERMOSTATO es %d", sizeof(*configuracion));
        
    }
    
    return true;
    
     
}



bool cargarDesdeMemoria(DATOS_APLICACION *configuracion) {
    
    
    
    uint32 dato;
    SpiFlashOpResult res;

    printf("cargarDesdeMemoria-->Cargando desde memoria...\n");
    
    // Leemos el sector 3 para ver cual es el sector del cual leer
    
    res = spi_flash_read((SEGMENTO_USUARIO +3) * SPI_FLASH_SEC_SIZE,
            (uint32*) &dato,
            sizeof(uint32));
    
    if (res != SPI_FLASH_RESULT_OK) return false;

    
    if (dato == 0) {
        res = spi_flash_read((SEGMENTO_USUARIO +0) * SPI_FLASH_SEC_SIZE,
               ( uint32*) configuracion,
                TAM_DATOS_APP);
        if (res != SPI_FLASH_RESULT_OK) return false;
        printf("cargarDesdeMemoria-->texto leido desde sector 0\n");

        
    } else {
        
         res = spi_flash_read((SEGMENTO_USUARIO +1) * SPI_FLASH_SEC_SIZE,
               ( uint32*) configuracion,
                TAM_DATOS_APP);
         if (res != SPI_FLASH_RESULT_OK) return false;
         printf("cargarDesdeMemoria-->texto leido desde sector 1\n");
         
        
    }

    if(configuracion->salvado == VALIDO) return true;
    else return false;


}


bool defaultConfig(DATOS_APLICACION *datosApp) {
    
    

    cJSON *root=NULL;
    char* texto;
    TIME_PROGRAM programaActual;
    uint8 i;



    //configuracion por defecto de mqtt
    
    strcpy(datosApp->parametrosMqtt.broker, "jajica.ddns.net");
    datosApp->parametrosMqtt.port = 1883;
    strcpy(datosApp->parametrosMqtt.user, "");
    strcpy(datosApp->parametrosMqtt.password, "");
    strcpy(datosApp->parametrosMqtt.prefix, "/");
    strcpy(datosApp->parametrosMqtt.publish, "publicacion");
    strcpy(datosApp->parametrosMqtt.subscribe, "comandos");
    datosApp->parametrosMqtt.estadoBroker = OFF_LINE;
    
    root = jsonMqtt(&datosApp->parametrosMqtt, root, false);
    texto = cJSON_Print(root);
    printf("defaultConfig-->%s\n", texto);
    cJSON_Delete(root);
    free(texto);
    root = NULL;
    
    // configuracion de parametros ntp
    initParNtp(&datosApp->clock);

    
    
    
    root = jsonClock(&datosApp->clock, root, false);
    texto = cJSON_Print(root);
    printf("defaultConfig-->%s\n", texto);
    cJSON_Delete(root);
    free(texto);

    
    
    //configuracion de parametros de programacion
    // La programacion estara desactivada hasta que el reloj sea valido
    datosApp->estadoProgramacion = INVALID_PROG;
    datosApp->nProgramacion=0;
    datosApp->programacion = NULL;
    for (i=0;i<7;i++) {
        root = NULL;
        programaActual.activo = true;
        programaActual.tipo = SEMANAL;
        programaActual.programacion.tm_wday = i;
        programaActual.umbral = 21.5;
        //programaActual.programacion.tm_mday = 24;
        programaActual.programacion.tm_hour = 17;
        //programaActual.programacion.tm_mon = 8;
        programaActual.programacion.tm_min = i;
        programaActual.programacion.tm_sec = 0;
        datosApp->programacion = crearPrograma(programaActual, datosApp->programacion, &datosApp->nProgramacion);
        /*
        texto = cJSON_Print(root);
        printf("defaultConfig-->%s\n", texto);
        free(texto);
        cJSON_Delete(root);
        */
        programaActual.tipo = SEMANAL;
        programaActual.programacion.tm_wday = i;
        programaActual.umbral = 19.5;
        //programaActual.programacion.tm_mday = 24;
        programaActual.programacion.tm_hour = 0;
        //programaActual.programacion.tm_mon = 8;
        programaActual.programacion.tm_min = i;
        programaActual.programacion.tm_sec = 0;
        datosApp->programacion = crearPrograma(programaActual, datosApp->programacion, &datosApp->nProgramacion);
        /*
        texto = cJSON_Print(root);
        printf("defaultConfig-->%s\n", texto);
        free(texto);
        cJSON_Delete(root);
        */
        
    }
    
    
    
    
    datosApp->salvado = VALIDO;
    
    // parametros de defecto de la configuracion de aplicacion.
    datosApp->reintentosLectura = 5;
    datosApp->intervaloReintentos = 3;
    datosApp->rele = false;
    datosApp->margenTemperatura = 0.5;
    datosApp->intervaloLectura = 5;
    datosApp->condicionesEnvio = SIEMPRE;
    datosApp->tempUmbral = 21.5;
    datosApp->tempUmbralDefecto = 21;
    datosApp->modo = AUTO;
    datosApp->incrementoUmbral = 0.5;
    datosApp->botonPulsado = false;
    datosApp->calibrado = 0;
     
        

    
    
    return true;
}

bool guardarConfiguracion(struct DATOS_APLICACION *datosApp, uint8 offset) {
    
    
    SpiFlashOpResult res;
    
    datosApp->salvado = VALIDO;
    
    // Borro el sector de DATOS_APPLICACION antes de escribir 
    spi_flash_erase_sector(SEGMENTO_USUARIO + offset);
    
    //Escribo la structura de aplicacion excepto la programacion dinamica

    res = spi_flash_write((SEGMENTO_USUARIO + offset) * SPI_FLASH_SEC_SIZE,
            (uint32*) datosApp,sizeof(struct DATOS_APLICACION));

    if (res != SPI_FLASH_RESULT_OK) {
        datosApp->salvado = INVALIDO;
        DBG("guardarConfiguracion-->Error en la configuracion. No pueden salvarse los datos. Error:%d \n", res);
    return FALSE;
    }

    // Borro el sector de TIME_PROGRAM antes de escribir 
    spi_flash_erase_sector(SEGMENTO_USUARIO + offset + 1);
    //Escribo la estructura TIME_PROGRAM

    res = spi_flash_write((SEGMENTO_USUARIO + offset + 1) * SPI_FLASH_SEC_SIZE,
            (uint32*) datosApp->programacion,sizeof(struct TIME_PROGRAM) * datosApp->nProgramacion);

    if (res != SPI_FLASH_RESULT_OK) {
       datosApp->salvado = INVALIDO;
       DBG("guardarConfiguracion-->Error en la configuracion. No pueden salvarse los datos. Error:%d \n", res);
    return FALSE;
    }
    
    printf("guardarConfiguracion-->Configuracion guardada en memoria %d\n", offset);
    
    

    
    
    return true;
}

bool cargarConfiguracion(DATOS_APLICACION *datosApp, uint8 offset) {
    
    
    SpiFlashOpResult res;


    //Cargamos en el offset 0 la estructura de la aplicacion
    //Cargamos en el offset +1 la estructura de programacion del putnero datosApp->programacion
    //Cargamos la estructura de aplicacion
    res = spi_flash_read((SEGMENTO_USUARIO + offset) * SPI_FLASH_SEC_SIZE,
           ( uint32*) datosApp,
            sizeof(struct DATOS_APLICACION));
    if (res != SPI_FLASH_RESULT_OK) {
        printf("cargarConfiguracion-->Error al cargar la estructura de aplicacion\n");
        datosApp->salvado = INVALIDO;
        return false;
    }
    
    // Reservamos memoria para la programacion cargada
    
    datosApp->programacion = (TIME_PROGRAM*) calloc(datosApp->nProgramacion,sizeof(struct TIME_PROGRAM));
    //Cargamos la programacion
    res = spi_flash_read((SEGMENTO_USUARIO + offset +1) * SPI_FLASH_SEC_SIZE,
            (uint32*) datosApp->programacion, sizeof(struct TIME_PROGRAM) * datosApp->nProgramacion);
    
    if (res != SPI_FLASH_RESULT_OK) {
        printf("cargarConfiguracion-->Error al cargar la estructura de aplicacion\n");
        return false;
        datosApp->salvado = INVALIDO;
    }
    
    if (datosApp->salvado != VALIDO) {
        datosApp->salvado = INVALIDO;
        printf("cargarConfiguracion-->Configuracion no valida\n");
        return false;
    } else {
        printf ("cargarConfiguracion-->Carga correcta desde memoria %d\n", offset);
        datosApp->ActiveConf = offset;
        return true;
    }
      
    
}

void Inicializacion(struct DATOS_APLICACION *datosApp) {
    

    //Inicializamos el reloj para que intente coger hora
    if (cargarConfiguracion(datosApp, MEMORIA_PRINCIPAL) == true) {
        DBG("Inicializacion-->Carga de configuracion realizada correctamente.\n");
        return;
        
    } else {
        defaultConfig(datosApp);
        guardarConfiguracion(datosApp, MEMORIA_PRINCIPAL);
        
    }
    /*
    datosApp->estadoApp = NORMAL;
    datosApp->botonPulsado = false;
    datosApp->clock.estado = SIN_HORA;
    datosApp->clock.timeValid = false;
    datosApp->estadoProgramacion = INVALID_PROG;
*/
    printf("Inicializacion-->Inicializando el LCD...\n");

    
}
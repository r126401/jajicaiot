/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "configuracion.h"

#include "user_config.h"


#define DEFECTO 0X1113

#define TAM_DATOS_APP sizeof(*configuracion) + ((sizeof(TIME_PROGRAM) * configuracion->nProgramacion))
/*
bool cargarConfiguracion(DATOS_APLICACION *datosApp) {
    
    bool res;
    cJSON *root=NULL;
    char *texto=NULL;
    
    res = cargarDesdeMemoria(datosApp);
    printf("Salvado vale %d", datosApp->salvado);
    if(res == true) {
        printf("Carga desde memoria correcta\n");
    root = jsonProgramacion(datosApp->programacion, root, false,0);
    texto = cJSON_Print(root);
    printf("%s\n", texto);
    cJSON_Delete(root);
    free(texto);

        
    } else {
        defaultConfig(datosApp);
        res = salvarConfiguracionMemoria(datosApp);
        if (res == true) printf("Configuracion salvada 1\n");
        res = salvarConfiguracionMemoria(datosApp);
        if (res == true) printf("Configuracion salvada 2\n");
        printf("Cargada configuracion por defecto\n");
    }
        return res;
    
    
}
*/

bool salvarConfiguracionMemoria(DATOS_APLICACION *configuracion) {
    
   
    uint8 sectorActivo=0;
    SpiFlashOpResult Resultado;
    uint32 dato;
    
        
        printf("testeo: %s\n", configuracion->parametrosMqtt.broker);
        printf("tamano datosApp: %d, *datosApp:%d\n", sizeof(*configuracion->programacion), sizeof(struct TIME_PROGRAM));
    
    
    // 1 recibir configuracion

    dato = 0;
    Resultado = spi_flash_read((SEGMENTO_USUARIO + 3) * SPI_FLASH_SEC_SIZE,
            (uint32*) &dato,
            sizeof(uint32));
    
    
    if (Resultado != SPI_FLASH_RESULT_OK) {
        DBG("Error en la configuracion. No pueden salvarse los datos. Error:%d \n", Resultado);
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
        DBG("Error en la configuracion. No pueden salvarse los datos. Error:%d \n", Resultado);
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
        DBG("Error en la configuracion. No pueden salvarse los datos. Error:%d \n", Resultado);
        return FALSE;
    }

        DBG("Salvado en el segmento 1\n. El tamaNo de TERMOSTATO es %d\n", sizeof(*configuracion));
        
        
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

    printf("Cargando desde memoria...\n");
    
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
        printf("texto leido desde sector 0\n");

        
    } else {
        
         res = spi_flash_read((SEGMENTO_USUARIO +1) * SPI_FLASH_SEC_SIZE,
               ( uint32*) configuracion,
                TAM_DATOS_APP);
         if (res != SPI_FLASH_RESULT_OK) return false;
         printf("texto leido desde sector 1\n");
         
        
    }
    printf("testeo 2 : %s\n", configuracion->parametrosMqtt.broker);
    printf("testeo3 :%d\n", configuracion->nProgramacion);
    if(configuracion->salvado == DEFECTO) return true;
    else return false;


}


bool defaultConfig(DATOS_APLICACION *datosApp) {
    
    
    TIME_PROGRAM *program;
    cJSON *root;
    char* texto;
    TIME_PROGRAM programaActual;
    uint8 i;



    //configuracion por defecto de mqtt
    
    //strcpy(datosApp->parametrosMqtt.broker, "iot.eclipse.org");
    strcpy(datosApp->parametrosMqtt.broker, "jajicaiot.ddns.net");
    datosApp->parametrosMqtt.port = 1883;
    strcpy(datosApp->parametrosMqtt.user, "");
    strcpy(datosApp->parametrosMqtt.password, "");
    strcpy(datosApp->parametrosMqtt.prefix, "/");
    strcpy(datosApp->parametrosMqtt.publish, "pub_");
    strcat(datosApp->parametrosMqtt.publish, get_my_id());
    strcpy(datosApp->parametrosMqtt.subscribe, "sub_");
    strcat(datosApp->parametrosMqtt.subscribe, get_my_id());
    // configuracion de parametros ntp
    initParNtp(&datosApp->clock);
    datosApp->estadoApp = NORMAL_ARRANCANDO;
    datosApp->estadoProgramacion = INVALID_PROG;
    datosApp->nProgramacion=0;
    datosApp->nProgramaCandidato = -1;
    datosApp->programacion = NULL;
    datosApp->salvado = DEFECTO;
    appUser_ConfiguracionDefecto(datosApp);

    

     
        

    
    
    return true;
}

bool guardarConfiguracion(struct DATOS_APLICACION *datosApp, uint8 offset) {
    
    
    SpiFlashOpResult res;
    if (offset == MEMORIA_INVALIDA) {
        
        datosApp->salvado = INVALIDO;
    } else {
        datosApp->salvado = VALIDO;
    }
    offset = MEMORIA_PRINCIPAL;
    
    
    // Borro el sector de DATOS_APPLICACION antes de escribir 
    spi_flash_erase_sector(SEGMENTO_USUARIO + offset);
    
    //Escribo la structura de aplicacion excepto la programacion dinamica

    res = spi_flash_write((SEGMENTO_USUARIO + offset) * SPI_FLASH_SEC_SIZE,
            (uint32*) datosApp,sizeof(struct DATOS_APLICACION));

    if (res != SPI_FLASH_RESULT_OK) {
        datosApp->salvado = INVALIDO;
        DBG("Error en la configuracion. No pueden salvarse los datos. Error:%d \n", res);
    return FALSE;
    }

    if (datosApp->nProgramacion > 0 ) {
        // Borro el sector de TIME_PROGRAM antes de escribir 
        spi_flash_erase_sector(SEGMENTO_USUARIO + offset + 1);
        //Escribo la estructura TIME_PROGRAM

        res = spi_flash_write((SEGMENTO_USUARIO + offset + 1) * SPI_FLASH_SEC_SIZE,
                (uint32*) datosApp->programacion,sizeof(struct TIME_PROGRAM) * datosApp->nProgramacion);

        if (res != SPI_FLASH_RESULT_OK) {
           datosApp->salvado = INVALIDO;
           DBG("Error en la configuracion. No pueden salvarse los datos. Error:%d \n", res);
        return FALSE;
    } else {
            printf("guardarConfiguracion--> Ejecucion correcta\n");
            
    }

        
    }
    
    printf("Configuracion guardada en memoria %d\n", offset);
    
    

    
    
    return true;
}

bool cargarConfiguracion(DATOS_APLICACION *datosApp, uint8 offset) {
    
    
    SpiFlashOpResult res;


    //Cargamos la estructura de aplicacion
    res = spi_flash_read((SEGMENTO_USUARIO + offset) * SPI_FLASH_SEC_SIZE,
           ( uint32*) datosApp,
            sizeof(struct DATOS_APLICACION));
    if (res != SPI_FLASH_RESULT_OK) {
        printf("Error al cargar la estructura de aplicacion\n");
        datosApp->salvado = INVALIDO;
        return false;
    }
    
    // Reservamos memoria para la programacion cargada
    
    datosApp->programacion = (TIME_PROGRAM*) calloc(datosApp->nProgramacion,sizeof(struct TIME_PROGRAM));
    //Cargamos la programacion
    res = spi_flash_read((SEGMENTO_USUARIO + offset +1) * SPI_FLASH_SEC_SIZE,
            (uint32*) datosApp->programacion, sizeof(struct TIME_PROGRAM) * datosApp->nProgramacion);
    
    if (res != SPI_FLASH_RESULT_OK) {
        printf("Error al cargar la estructura de aplicacion\n");
        return false;
        datosApp->salvado = INVALIDO;
    }
    
    if (datosApp->salvado != VALIDO) {
        datosApp->salvado = INVALIDO;
        return false;
    } else {
        printf ("Carga correcta desde memoria %d\n", offset);
        datosApp->ActiveConf = offset;
        return true;
    }
      
    
}

void Inicializacion(struct DATOS_APLICACION *datosApp, bool forzado) {
    
    bool res;
    
    
    if (forzado == true) {
        defaultConfig(datosApp);
        guardarConfiguracion(datosApp, MEMORIA_PRINCIPAL);
        printf("Inicializacion-->carga por defecto forzada\n");
        return;
        
    }
    
    res = cargarConfiguracion(datosApp, MEMORIA_PRINCIPAL);
    //ntpServerDefault(&datosApp->clock);
    datosApp->clock.estado = SIN_HORA;
    datosApp->clock.timeValid = false;
    //datosApp->estadoProgramacion = INVALID_PROG;
    datosApp->estadoApp = NORMAL_ARRANCANDO;
    if (res == true){
        DBG("Inicializacion:Carga de configuracion realizada correctamente.\n");
        return;
        
    } else {
        DBG("Inicializacion--> carga incorrecta. Se carga defecto");
        defaultConfig(datosApp);
        guardarConfiguracion(datosApp, MEMORIA_PRINCIPAL);
        
    }
    
}
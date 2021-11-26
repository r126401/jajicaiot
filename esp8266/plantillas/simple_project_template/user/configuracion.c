/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "configuracion.h"

#include "user_config.h"


bool SalvarConfiguracionMemoria(void *configuracion) {
    
   
    uint8 sectorActivo=0;
    SpiFlashOpResult Resultado;
    uint32 dato;
    
        
    
    
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
                sizeof(configuracion));
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

        DBG("Salvado en el segmento 1\n. El tamaNo de TERMOSTATO es %d\n", sizeof(configuracion));
        
        
    } else {


         // Borro el sector 0 antes de escribir 
        spi_flash_erase_sector(SEGMENTO_USUARIO + 0);
        //Escribo el texto en el  sector 0


        Resultado = spi_flash_write((SEGMENTO_USUARIO + 0) * SPI_FLASH_SEC_SIZE,
                (uint32*) configuracion,
                sizeof(configuracion));
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
        
      DBG("Salvado en el segmento 0\n. El tamaño de TERMOSTATO es %d", sizeof(configuracion));
        
    }
    
    
     
}



bool CargarDesdeMemoria(void *configuracion) {
    
    
    
    uint32 dato;

    printf("Cargando desde memoria...\n");
    
    // Leemos el sector 3 para ver cual es el sector del cual leer
    
    spi_flash_read((SEGMENTO_USUARIO +3) * SPI_FLASH_SEC_SIZE,
            (uint32*) &dato,
            sizeof(uint32));

    
    if (dato == 0) {
        spi_flash_read((SEGMENTO_USUARIO +0) * SPI_FLASH_SEC_SIZE,
               ( uint32*) configuracion,
                sizeof(configuracion));
        printf("texto leido desde sector 0\n");

        
    } else {
        
         spi_flash_read((SEGMENTO_USUARIO +1) * SPI_FLASH_SEC_SIZE,
               ( uint32*) configuracion,
                sizeof(configuracion));
         printf("texto leido desde sector 1\n");
         
        
    }
    



}

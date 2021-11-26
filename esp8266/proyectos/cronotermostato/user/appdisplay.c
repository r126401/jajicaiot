/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "appdisplay.h"
#include "wlcd_defs.h"




void displayBorrarLcd() {
    wlcd_rect_fill(0,0, WLCD_WIDTH, WLCD_HEIGHT, 0);
}

uint8 displayInicializarLcd() {
    
    
    return wlcd_init();
}


enum OP_DISPLAY displayPintarWifi(enum OP_DISPLAY op) {
    
    uint16_t x, y;
    
    x= WLCD_WIDTH - ICON_WIDTH;
    y = BAR_ICON_HEIGHT;

    wlcd_rect_fill(x,y, ICON_WIDTH, ICON_HEIGHT, 0);
    
    
    switch(op) {
        case PAINT:
            displayPintarImagen(x, y,WFOF_IDX_WIFION_PNG_BMP_16BPP, WFOF_SIZE_WIFION_PNG_BMP_16BPP );
            return PAINTED;
            break;
        case NO_PAINT:
            displayPintarImagen(x, y,WFOF_IDX_WIFIOFF_PNG_BMP_16BPP, WFOF_SIZE_WIFIOFF_PNG_BMP_16BPP );
            return NO_PAINTED;
            break;
        case NO_ACTION:
            return NO_ACTION;
            break;
        case TRANSIENT:
            return TRANSIENT;
            break;
            
    }
    
    
    
}

enum OP_DISPLAY displayPintarBroker(enum OP_DISPLAY op) {
    

    uint16_t x, y;
    
    x = WLCD_WIDTH - (ICON_WIDTH * 2 ) - SPACE_ICON;
    y = BAR_ICON_HEIGHT;
    
    
    
    switch(op) {
        case PAINT:
            wlcd_rect_fill(x,y, ICON_WIDTH, ICON_HEIGHT, 0);
            displayPintarImagen(x, y,WFOF_IDX_BROKER_PNG_BMP_16BPP, WFOF_SIZE_BROKER_PNG_BMP_16BPP );
            return PAINTED;
            break;
        case NO_PAINT:
            wlcd_rect_fill(WLCD_WIDTH - (ICON_WIDTH * 2 ) - SPACE_ICON,BAR_ICON_HEIGHT, ICON_WIDTH, ICON_HEIGHT, 0);
            displayPintarImagen(x, y,WFOF_IDX_BROKEROFF_PNG_BMP_16BPP,WFOF_SIZE_BROKEROFF_PNG_BMP_16BPP); 
            return NO_PAINTED;
            break;
        case NO_ACTION:
            return NO_ACTION;
            break;
        case TRANSIENT:
            return TRANSIENT;
            break;
            
    }
    
    
    
}

enum OP_DISPLAY displayPintarReloj(struct DATOS_APLICACION *datosApp, enum OP_DISPLAY op) {
    

    char hora[8];
    uint16_t width, height, x, y;
    uint32_t color;
    
    x = WLCD_WIDTH/4;
    y = BAR_ICON_HEIGHT;
    
   
    
    
    switch(op) {
        case PAINT:
            if (datosApp->clock.estado == EN_HORA) {
                sprintf(hora, "%02d:%02d", datosApp->clock.date.tm_hour, datosApp->clock.date.tm_min);
                color = VERDE;
            } else {
                if (datosApp->clock.estado != EN_HORA) {
                    if (datosApp->clock.timeValid == true) {
                        sprintf(hora, "%02d:%02d", datosApp->clock.date.tm_hour, datosApp->clock.date.tm_min);
                        color = AMARILLO;
                    } else {
                        strcpy(hora, "-----");
                        color = ROJO;
                    }
                }
            }
            displayEscribirTexto(x, y, hora, color, 3, &width, &height, 0 );
            return PAINTED;
            break;
        case NO_PAINT:
            wlcd_rect_fill(WLCD_WIDTH - (ICON_WIDTH * 2 ) - SPACE_ICON,BAR_ICON_HEIGHT, ICON_WIDTH, ICON_HEIGHT, 0);
            return NO_PAINTED;
            break;
        case NO_ACTION:
            return NO_ACTION;
            break;
        case TRANSIENT:
            return TRANSIENT;
            break;
            
    }
    
    
    
}


 void  lcd_task(void *pvParameters) {
     
     MSG_LCD msgLcd;
     struct DATOS_APLICACION *datosApp;
     
     datosApp = (struct DATOS_APLICACION*) pvParameters;
     //datosApp->estadoApp = NORMAL;
     
     
     while (1) {
         
         if (xQueueReceive(lcd_queue, &msgLcd, 0) == pdTRUE) {
             //printf("El estado de la app es %d\n", datosApp->estadoApp);
             //printf("lcd_task-->recibido :%d, %d\n", msgLcd.lcdStat, msgLcd.opLcd);

             if(datosApp->estadoApp == NORMAL) {
                     switch (msgLcd.lcdStat) {
                         
                         case WIFI_OFF:
                             displayPintarWifi(msgLcd.opLcd);
                             displayPintarBroker(msgLcd.opLcd);
                             break;
                         case WIFI_ON:
                             displayPintarWifi(msgLcd.opLcd);                     
                             break;
                         case BROKER_ON:
                         case BROKER_OFF:
                             displayPintarBroker(msgLcd.opLcd);
                             break;
                         case TRANSMITING:
                             displayPintarSend(datosApp, msgLcd.opLcd);
                             break;
                         case CLOCK:
                             displayPintarReloj(datosApp, msgLcd.opLcd);
                             break;
                         case TEMPERATURE:
                             displayTemperatura(datosApp, msgLcd.opLcd);

                             break;
                         case HEATING:
                             displayPintarRele(msgLcd.opLcd);
                             break;
                         case MODE_WORK:
                             displayPintarModo(datosApp);
                             break;
                         case UMBRAL_TEMP:
                             displayPintarUmbrales(datosApp, msgLcd.opLcd);
                             break;
                         case RECEIVING:
                             displayPintarReceived(datosApp, msgLcd.opLcd);
                             break;
                         case NORMAL_LCD:
                             displayPintarLcd(datosApp, msgLcd.opLcd);
                             break;                             
                     }
             } else {
                 switch(msgLcd.lcdStat) {
                     case CLOCK_SETTINGS:
                         printf("lcd_task-->Pintamos clockSettings\n");
                         displayBorrarLcd();
                         displayPintarPonerHora();
                         break;
                     case CLOCK_SETTINGS_DAY:
                         printf("lcd_task-->pintamos configurar dia\n");
                         displaySetDay(datosApp);
                         break;
                     case CLOCK_SETTINGS_MONTH:
                         printf("lcd_task-->pintamos configurar mes\n");
                         displaySetMonth(datosApp);
                         break;
                     case CLOCK_SETTINGS_YEAR:
                         printf("lcd_task-->pintamos configurar ano\n");
                         displaySetYear(datosApp);
                         break;
                     case CLOCK_SETTINGS_HOUR:
                         printf("lcd_task-->pintamos configurar hora\n");
                         displaySetHour(datosApp);
                         break;
                     case CLOCK_SETTINGS_MIN:
                         printf("lcd_task-->pintamos configurar minutos\n");
                         displaySetMin(datosApp);
                         break;
                     case CLOCK_SETTINGS_CONFIRM:
                         printf("lcd_task-->pintamos mensaje de confirmacion\n");
                         displayClockConfirm(datosApp);
                         break;
                     case SMARTCONFIG_SETTINGS:
                         printf("lcd_task-->pintamos smartconfig\n");
                         displayPintarSmartConfig();
                         break;
                     case SMARTCONFIG_FIND_CHANNEL:
                         printf("lcd_task-->pintamos findChannel\n");
                         displayPintarSmartConfigFindChannel(datosApp, msgLcd.opLcd);
                         break;
                     case RESET_SETTINGS:
                         printf("lcd_task-->pintamos reset Settings\n");
                         displayPintarReset();
                         break;
                     case FACTORY_RESET_SETTINGS:
                         printf("lcd_task-->pintamos factory reset\n");
                         displayPintarFactoryReset();
                         break;
                     case FACTORY_RESET_OK:
                         printf("lcd_task-->pintamos pulsar reset\n");

                         //sprintf(texto, "Despues pulsa Reset para reiniciar" );
                         displayEscribirTexto(OPCIONES_X, IMAGEN_SETTINGS_Y + 120, "Ok. Valores a fabrica, pulsa Reset.", BLANCO, 2,0, 0, 0);
                         break;

                         
                         
                         
                 
                 }
             }
             if (wlcd_status() == false) {
                 printf("lcd_task-->%02d:%02d:%02d. Error en display ", datosApp->clock.date.tm_hour,
                         datosApp->clock.date.tm_min, datosApp->clock.date.tm_sec);

             }
             //wlcd_sleep();
         }
         
     }
     
 }
             
                 
             

 
 void pintarLcd(enum ELEMENTOS_DISPLAY elem, enum OP_DISPLAY op) {
     
     MSG_LCD msg;
     
     msg.lcdStat = elem;
     msg.opLcd = op;
     xQueueSend(lcd_queue, &msg, 0);
 }
 
 enum OP_DISPLAY displayPintarSend(struct DATOS_APLICACION *datosApp, enum OP_DISPLAY op) {
     
     static os_timer_t send;
     uint16_t x,y;
     
     x = WLCD_WIDTH - (ICON_WIDTH * 3  )- SPACE_ICON;
     y = BAR_ICON_HEIGHT;
     

     
     
     switch(op) {         
         case PAINT:
             if (datosApp->estadoApp == NORMAL) {
                 displayPintarImagen(x, y, WFOF_IDX_TRANSFER_PNG_BMP_16BPP, WFOF_SIZE_TRANSFER_PNG_BMP_16BPP );
                 os_timer_disarm(&send);
                 os_timer_setfn(&send, (os_timer_func_t*) displaySendData, datosApp);
                 os_timer_arm(&send, 500, 0);             
                 
             }
            return PAINTED;
             
     }
     
 }
 
 void displaySendData(struct DATOS_APLICACION *datosApp) {


     uint16_t x,y;
     
     x = WLCD_WIDTH - (ICON_WIDTH * 3  )- SPACE_ICON;
     y = BAR_ICON_HEIGHT;
     if (datosApp->estadoApp == NORMAL)
     wlcd_rect_fill(x,y, ICON_WIDTH, ICON_HEIGHT, 0);


     
 }
 
 enum OP_DISPLAY displayTemperatura(struct DATOS_APLICACION *datosApp, enum OP_DISPLAY op) {
     
    char texto[7];
    uint16_t x, y;
    
    x = WLCD_WIDTH/5;
    y = 50;
    
    uint16_t width, height;
    
     switch(op) {
         case PAINT:
             
             //pintamos la temperatura
            sprintf(texto, "%.2f%cC", datosApp->tempActual, 176);
            displayEscribirTexto(x, y, texto, BLANCO, 2, &width, &height, 2 );
            // Pintamos la humedad
            sprintf(texto, "%.2f%%", datosApp->humedad);
            displayEscribirTexto(x, y + height , texto, AZUL, 3, &width, &height, 0 );
            //Pintamos el icono del termometro
            displayPintarImagen(x - 25, y + 12, WFOF_IDX_TERMOMETRO_PNG_BMP_16BPP, WFOF_SIZE_TERMOMETRO_PNG_BMP_16BPP );

             break;
             
     }
     
 }
 
 
 enum OP_DISPLAY displayPintarRele(enum OP_DISPLAY op) {


    uint16_t x, y;
    
    x = 240;
    y = 90;
     
     switch (op) {
         case PAINT:
             displayPintarImagen(x, y, WFOF_IDX_RELE_PNG_BMP_16BPP, WFOF_SIZE_RELE_PNG_BMP_16BPP );
             return PAINTED;
             break;
         case NO_PAINT:
             wlcd_rect_fill(x,y, 32, 32, 0);
             break;
             
         
     }
     
     
     
 }
 
void displayPintarModo(struct DATOS_APLICACION *datosApp) {


    uint16_t x, y;

    
    x = WLCD_WIDTH - (ICON_WIDTH * 3 );
    y = 170;
    
    //printf("displayPintarModo-->modo: %d\n", datosApp->modo);
     switch (datosApp->modo) {
         case AUTO:
             //pintamos el icono de auto
             wlcd_rect_fill(x,y, ICON_WIDTH, ICON_HEIGHT, 0);
             displayPintarImagen(x, y, WFOF_IDX_AUTO_PNG_BMP_16BPP, WFOF_SIZE_AUTO_PNG_BMP_16BPP );
             break;
         case MANUAL:
             wlcd_rect_fill(x,y, ICON_WIDTH, ICON_HEIGHT, 0);
             displayPintarImagen(x, y, WFOF_IDX_HANDX32_PNG_BMP_16BPP, WFOF_SIZE_HANDX32_PNG_BMP_16BPP);
             break;
         case ON:
             wlcd_rect_fill(x,y, ICON_WIDTH, ICON_HEIGHT, 0);
             displayPintarImagen(x, y, WFOF_IDX_ON_PNG_BMP_16BPP, WFOF_SIZE_ON_PNG_BMP_16BPP);
             break;
         case OFF:
             wlcd_rect_fill(x,y, ICON_WIDTH, ICON_HEIGHT, 0);
             displayPintarImagen(x, y, WFOF_IDX_OFF_PNG_BMP_16BPP, WFOF_SIZE_OFF_PNG_BMP_16BPP);
             break;
             
             
         
     }
     
     
     
 }
 

void displayPintarUmbrales(struct DATOS_APLICACION *datosApp, enum OP_DISPLAY op) {
    
    
    uint16_t x, y;
    char texto[10];
    char diaSem;
    uint16_t width, height;
    int programaFuturo, programaActual;
    bool res;
    x=20;
    y=160;
    
    
    
    
    switch (op) {
        case PAINT:
            sprintf(texto, "%.2f%cC", datosApp->tempUmbral, 176);
            displayEscribirTexto(x, y, texto, VERDE, 2,&width, &height, 1 );
            y = y + height;
            if ((datosApp->modo == AUTO) || (datosApp->modo == AUTOMAN)) {
                
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
                     programaFuturo = 0;
                     programaActual = datosApp->nProgramacion - 1;
                     
                     //programaFuturo = datosApp->nProgramacion - 1;
                     //programaActual = programaFuturo;
                     break;
                 case -2:
                     //Se han borrado todos los programas. Se asigna el de defecto
                     datosApp->tempUmbral = datosApp->tempUmbralDefecto;
                 default:
                     programaActual = programaFuturo - 1;
                     break;
                }*/

                switch (datosApp->programacion[programaFuturo].programacion.tm_wday) {
                    case 0:
                        diaSem = 'D';
                        break;
                    case 1:
                        diaSem = 'L';
                        break;
                    case 2:
                        diaSem = 'M';
                        break;
                    case 3:
                        diaSem = 'X';
                        break;
                    case 4:
                        diaSem = 'J';
                        break;
                    case 5:
                        diaSem ='V';
                        break;
                    case 6:
                        diaSem = 'S';
                        break;


                }

                sprintf(texto, "%c %02d:%02d", diaSem, datosApp->programacion[programaFuturo].programacion.tm_hour, datosApp->programacion[programaFuturo].programacion.tm_min);
                displayEscribirTexto(x, y, texto, ROJO, 2,&width, &height, 0 );
                sprintf(texto, "%.2f%cC", datosApp->programacion[programaFuturo].umbral, 176);
                displayEscribirTexto(x, y + height, texto, ROJO, 2,&width, &height, 0 );
                
            } else {
                wlcd_rect_fill(x,y, width, width*2, 0);
            }
            
            
            
            break;
            
            
        
    }
    
    
    
    
    

    
    
    
}

 enum OP_DISPLAY displayPintarReceived(struct DATOS_APLICACION *datosApp, enum OP_DISPLAY op) {
     
     uint16_t x,y;
     static os_timer_t rec;
     
     x =  WLCD_WIDTH - (ICON_WIDTH * 4 ) - SPACE_ICON;
     y = BAR_ICON_HEIGHT;

     
     
     switch(op) {         
         case PAINT:
             if (datosApp->estadoApp == NORMAL) {
                 displayPintarImagen(x, y,WFOF_IDX_RECEIVING_PNG_BMP_16BPP, WFOF_SIZE_RECEIVING_PNG_BMP_16BPP );
                 os_timer_disarm(&rec);
                 os_timer_setfn(&rec, (os_timer_func_t*) displayRecData, datosApp);
                 os_timer_arm(&rec, 500, 1);             
                 
             }
            return PAINTED;
             
     }
     
 }
 void displayRecData(struct DATOS_APLICACION *datosApp) {
     
     uint16_t x,y;

     
     x =  WLCD_WIDTH - (ICON_WIDTH * 4 ) - SPACE_ICON;
     y = BAR_ICON_HEIGHT;
     if (datosApp->estadoApp == NORMAL)
     wlcd_rect_fill(x,y, ICON_WIDTH, ICON_HEIGHT, 0);


     
 }

 void displayCuadrado(uint16_t x, uint16_t y, uint16_t longitud, uint16_t altura, uint32_t color, uint8 grosor ) {
     
    
     uint8 i;
     for (i=0;i<grosor; i++) {
    // Horizontales 
         //-
    wlcd_line_draw (x + i,y + i,  x - i + longitud, y + i, color); //ok
    // _
    wlcd_line_draw (x + i,y -i + altura,  x + longitud - i, y + altura - i, color); //ok
    //verticales
    //|
    wlcd_line_draw (x + i,y + i,  x + i, y + altura - i, color); //ok
    //  |
    wlcd_line_draw (x + longitud - i,y + i,  x + longitud - i, y + altura - i, color);
         
     }
     
     
     
     
 }
 
 
 void displayCirculo(uint8 x0, uint8 y0, uint8 r, uint32_t color) {
     
     
    int8_t x = r;
    int8_t y = 1;
    int16_t radius_err = 1 - x;
     
    if (r == 0)
        return;

    wlcd_pixel_draw(x0 - r, y0,     color);
    wlcd_pixel_draw(x0 + r, y0,     color);
    wlcd_pixel_draw(x0,     y0 - r, color);
    wlcd_pixel_draw(x0,     y0 + r, color);

    while (x >= y)
    {
        wlcd_pixel_draw(x0 + x, y0 + y, color);
        wlcd_pixel_draw(x0 - x, y0 + y, color);
        wlcd_pixel_draw(x0 + x, y0 - y, color);
        wlcd_pixel_draw(x0 - x, y0 - y, color);
        if (x != y)
        {
            /* Otherwise the 4 drawings below are the same as above, causing
             * problem when color is INVERT
             */
            wlcd_pixel_draw(x0 + y, y0 + x, color);
            wlcd_pixel_draw(x0 - y, y0 + x, color);
            wlcd_pixel_draw(x0 + y, y0 - x, color);
            wlcd_pixel_draw(x0 - y, y0 - x, color);
        }
        ++y;
        if (radius_err < 0)
        {
            radius_err += 2 * y + 1;
        }
        else
        {
            --x;
            radius_err += 2 * (y - x + 1);
        }

    }

     
 }
 
 void displayPintarLcd(struct DATOS_APLICACION *datosApp, enum OP_DISPLAY op) {
     
     
     
     switch(op) {
         case PAINT:
             displayBorrarLcd();
             //Pintamos el wifi
             if (wifi_station_get_connect_status() == STATION_GOT_IP) {
                 displayPintarWifi(PAINT);
             } else {
                 displayPintarWifi(NO_PAINT);
             }
             // Pintamos el estado de broker
             if(datosApp->parametrosMqtt.estadoBroker == ON_LINE) {
                 displayPintarBroker(PAINT);
             } else {
                 displayPintarBroker(NO_PAINT);
             }
             // Pintamos el reloj
             displayPintarReloj(datosApp, PAINT);
             //Pintamos la temperatura
             displayTemperatura(datosApp, PAINT);
             //Pintamos el rele
             if (datosApp->rele == ENCENDIDO) {
                 displayPintarRele(PAINT);
             }
             //Pintamos el modo
             displayPintarModo(datosApp);
             //Pintamos los umbrales
             displayPintarUmbrales(datosApp, PAINT);
             break;
         case NO_PAINT:
             wlcd_rect_fill(0,0, WLCD_WIDTH, WLCD_HEIGHT, 0);
             break;
             
     }
     
     
 }
 
 void displayPintarPonerHora() {



    uint16_t x, y;
    char texto[25];
    uint16_t width=0, height=0;
     
     
     x = TITULO_X;
     y = TITULO_Y;
     
    printf("displayPintarPonerHora-->comenzamos a pintar\n");
    sprintf(texto, "Poner en hora...");
    
    displayEscribirTexto(x, y, texto, BLANCO, 3, &width, &height, 0);

printf("displayPintarPonerHora-->comenzamos a dubujar el reloj\n");    
    
    // Pintamos el reloj     
    x = IMAGEN_SETTINGS_X;
    y = IMAGEN_SETTINGS_Y;
    displayPintarImagen(x, y,WFOF_IDX_CLOCKSETTINGS_PNG_BMP_16BPP, WFOF_SIZE_CLOCKSETTINGS_PNG_BMP_16BPP );
    
    x = OPCIONES_X;
    y = OPCIONES_Y;
    sprintf(texto, "M conf, + Sig, - Ant" );
    displayEscribirTexto(x, y, texto, VERDE, 2,&width, &height, 0);

 }

 void displayPintarSmartConfig() {
     

    uint16_t x, y;
    char texto[40];
    uint16_t width=0, height=0;
     
    displayBorrarLcd();
     x = TITULO_X;
     y = TITULO_Y;
     
    printf("displayPintarPonerHora-->comenzamos a pintar\n");
    sprintf(texto, "Wifi smartconfig");
    displayEscribirTexto(x, y, texto, BLANCO, 3, &width, &height, 0);
    
    
    // Pintamos el reloj     
    x = IMAGEN_SETTINGS_X;
    y = IMAGEN_SETTINGS_Y;
    displayPintarImagen(x, y,WFOF_IDX_WIFISETTINGS_PNG_BMP_16BPP, WFOF_SIZE_WIFISETTINGS_PNG_BMP_16BPP );
    
    x = OPCIONES_X;
    y = IMAGEN_SETTINGS_Y + 70;
    sprintf(texto, "Configura wifi en tu app y pulsa M" );
    displayEscribirTexto(x, y, texto, VERDE, 2,&width, &height, 0);
    
    
    x = OPCIONES_X;
    y = OPCIONES_Y;
    sprintf(texto, "M conf, + Sig, - Ant" );
    displayEscribirTexto(x, y, texto, VERDE, 2,&width, &height, 0);

    
    
 }
 

  void displaySetDay(struct DATOS_APLICACION *datosApp) {

    uint16_t x, y;
    char texto[10];
    uint16_t width, height;

    
    x = ICON_SETTINGS_X;
    y = ICON_SETTINGS_Y;

    wlcd_rect_fill(x, y + 100, 13*2, 16,0);
    sprintf(texto, "%02d/", datosApp->fechaTemporal.tm_mday);
    
    printf("displaySetDay-->pintamos : %02d\n", datosApp->fechaTemporal.tm_mday);
    displayEscribirTexto(x, y + 100, texto, BLANCO, 3, &width, &height, 0 );
    
}
void displaySetMonth(struct DATOS_APLICACION *datosApp) {

    
    uint16_t x, y;
    char texto[10];
    uint16_t width, height;

    
    x = ICON_SETTINGS_X + (13 * 3);
    y = ICON_SETTINGS_Y;

    wlcd_rect_fill(x, y + 100, 13*2, 16,0);
    sprintf(texto, "%02d/", datosApp->fechaTemporal.tm_mon);
    printf("displaySetMonth-->pintamos : %02d\n", datosApp->fechaTemporal.tm_mon);
    displayEscribirTexto(x, y + 100, texto, BLANCO, 3, &width, &height, 0 );
    
    
}

void displaySetYear(struct DATOS_APLICACION *datosApp) {

    uint16_t x, y;
    char texto[10];
    uint16_t width, height;

    
    x = ICON_SETTINGS_X + (13 * 6);
    y = ICON_SETTINGS_Y;

    wlcd_rect_fill(x, y + 100, 13*4, 16,0);
    sprintf(texto, "%d", datosApp->fechaTemporal.tm_year);
    displayEscribirTexto(x, y + 100, texto, BLANCO, 3, &width, &height, 0 );
}

void displaySetHour(struct DATOS_APLICACION *datosApp) {

    uint16_t x, y;
    char texto[10];
    uint16_t width, height;

    
    x = ICON_SETTINGS_X + (13 * 11);
    y = ICON_SETTINGS_Y;
    wlcd_rect_fill(x, y + 100, 13*2, 16,0);
    sprintf(texto, "%02d:", datosApp->fechaTemporal.tm_hour);
    displayEscribirTexto(x, y + 100, texto, BLANCO, 3, &width, &height, 0 );

}

void displaySetMin(struct DATOS_APLICACION *datosApp) {

    uint16_t x, y;
    char texto[10];
    uint16_t width, height;

    
    x = ICON_SETTINGS_X + (13 * 14);
    y = ICON_SETTINGS_Y;
    wlcd_rect_fill(x, y + 100, 13*2, 16,0);
    sprintf(texto, "%02d", datosApp->fechaTemporal.tm_min);
    displayEscribirTexto(x, y + 100, texto, BLANCO, 3, &width, &height, 0 );
}

void displayClockConfirm(struct DATOS_APLICACION *datosApp) {
 
    uint16_t x, y;
    char texto[25];
    uint16_t width, height;

    
    x = ICON_SETTINGS_X;
    y = ICON_SETTINGS_Y + 25;

    sprintf(texto, "M confirmar, + salir.", datosApp->fechaTemporal.tm_min);
    displayEscribirTexto(x, y+100, texto, ROJO, 2,&width, &height, 0 );
    
}

void displayPintarSmartConfigFindChannel(struct DATOS_APLICACION *datosApp, enum OP_DISPLAY op) {
    
    uint16_t x, y;
    char texto[25];
    static uint16_t width, height; 
    
    switch(op) {
        case PAINT:
            x = OPCIONES_X;
            y = IMAGEN_SETTINGS_Y + 90;
            sprintf(texto, "Escaneando 2 min..." );
            displayEscribirTexto(x, y, texto, VERDE, 2,&width, &height, 0);
            break;
        case PAINTED:
            displayEscribirTexto(x, y, "WiFi configurado!", VERDE, 2,&width, &height, 0 );
            break;
    }
    

    printf("displayPintarSmartConfigFindChannel-->Se pinta el modo %d\n", op);

    
}


void displayEscribirTexto(uint16_t x, uint16_t y, char *texto, uint16_t color, uint8 fuente, uint16 *width, uint16 *height, uint8 zoom  ) {
    
    wlcd_text_draw_settings_struct formatoTexto;
    //uint16_t width, height;

    formatoTexto.BoldAdd = 0;
    formatoTexto.FontIdx = fuente;
    formatoTexto.FontZoomAdd = zoom;
    formatoTexto.MaxH = 100;
    formatoTexto.HSpc = 0;
    formatoTexto.MaxW = WLCD_WIDTH;
    formatoTexto.VSpc = 0;
    formatoTexto.WrapStyle = WLCD_WRAP_NONE;
    formatoTexto.X= x ;
    formatoTexto.Y = y;
    formatoTexto.Color.u32t = color;
    wlcd_text_measure(texto, &formatoTexto, width, height);
    wlcd_rect_fill(formatoTexto.X, formatoTexto.Y, *width, *height,0);
    wlcd_text_draw(texto, &formatoTexto);


    
}


void displayPintarImagen(uint16_t x, uint16_t y, int index, int size) {
    
    uint8 *datos=NULL;
    
    datos = malloc(size);
    if (datos == NULL) {
        printf("displayPintarImagen-->No hay memoria para la imagen %d\n",index );
        free(datos);
        datos = NULL;
        system_print_meminfo();
        printf("Memoria libre:%d,\n", system_get_free_heap_size());
    } else {
        wfof_get_file_data_fast(index, (uint32_t*) datos, 0, size);
        wlcd_img_draw(datos,x , y);
        free(datos);
        
        
    }
    
    
}

void displayPintarFactoryReset() {



    uint16_t x, y;
    char texto[50];
    uint16_t width=0, height=0;
     
     displayBorrarLcd();
     x = TITULO_X;
     y = TITULO_Y;
     
    printf("displayPintarFactoryReset-->comenzamos a pintar\n");
    sprintf(texto, "Factory Reset...");
    
    displayEscribirTexto(x, y, texto, BLANCO, 3, &width, &height, 0);
    
    printf("displayPintarFactoryReset-->comenzamos a dubujar el reloj\n");    
    
    // Pintamos el reloj     
    x = IMAGEN_SETTINGS_X;
    y = IMAGEN_SETTINGS_Y;
    displayPintarImagen(x, y,WFOF_IDX_RESET_PNG_BMP_16BPP, WFOF_SIZE_RESET_PNG_BMP_16BPP );


    x = OPCIONES_X;
    y = IMAGEN_SETTINGS_Y + 90;
    sprintf(texto, "Pulsa M para valores de fabrica" );
    displayEscribirTexto(x, y, texto, VERDE, 2,&width, &height, 0);
    
    x = OPCIONES_X;
    y = IMAGEN_SETTINGS_Y + 110;
    sprintf(texto, "Despues pulsa Reset para reiniciar" );
    displayEscribirTexto(x, y, texto, VERDE, 2,&width, &height, 0);
 

    
    x = OPCIONES_X;
    y = OPCIONES_Y;
    sprintf(texto, "M conf, + Sig, - Ant" );
    displayEscribirTexto(x, y, texto, VERDE, 2,&width, &height, 0);

 }

void displayPintarReset() {



    uint16_t x, y;
    char texto[25];
    uint16_t width=0, height=0;
     
    displayBorrarLcd();
     x = TITULO_X;
     y = TITULO_Y;
     
    printf("displayPintarFactoryReset-->comenzamos a pintar\n");
    sprintf(texto, "Reset...");
    
    displayEscribirTexto(x, y, texto, BLANCO, 3, &width, &height, 0);
    
    printf("displayPintarFactoryReset-->comenzamos a dubujar el reloj\n");    
    
    // Pintamos el reloj     
    x = IMAGEN_SETTINGS_X;
    y = IMAGEN_SETTINGS_Y;
    displayPintarImagen(x, y,WFOF_IDX_RESET_PNG_BMP_16BPP, WFOF_SIZE_RESET_PNG_BMP_16BPP );


    x = OPCIONES_X;
    y = IMAGEN_SETTINGS_Y + 90;
    sprintf(texto, "Pulsa M para reiniciar." );
    displayEscribirTexto(x, y, texto, VERDE, 2,&width, &height, 0);
 

    
    x = OPCIONES_X;
    y = OPCIONES_Y;
    sprintf(texto, "M conf, + Sig, - Ant" );
    displayEscribirTexto(x, y, texto, VERDE, 2,&width, &height, 0);

 }


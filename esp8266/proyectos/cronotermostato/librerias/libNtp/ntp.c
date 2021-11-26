/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */


#include "ntp.h"
#include "user_config.h"
#include "appdisplay.h"



extern DATOS_APLICACION datosApp;





void  ntp_udp_timeout(void *arg) {
	
     
     

	os_timer_disarm(&ntp_timeout);
	printf("ntp_udp_timeout-->ntp timeout\r\n");
        datosApp.clock.estado = ERROR_NTP;
        //confGlobal.clock.estado = ERROR;
	// clean up connection
	if (pCon) {
		espconn_delete(pCon);
		free(pCon->proto.udp);
		free(pCon);
		pCon = 0;
	}
}

 void ICACHE_FLASH_ATTR ntp_udp_recv(void *arg, char *pdata, unsigned short len) {
	
	struct tm *dt;
	time_t timestamp;
	ntp_t *ntp;
        
        
	os_timer_disarm(&ntp_timeout);

	// extract ntp time
	ntp = (ntp_t*)pdata;
	timestamp = ntp->trans_time[0] << 24 | ntp->trans_time[1] << 16 |ntp->trans_time[2] << 8 | ntp->trans_time[3];
	// convert to unix time
	timestamp -= 2208988800UL;
        datosApp.clock.time =timestamp;
        //reloj.time = timestamp;

//        memcpy(&reloj.date, gmtime(&reloj.time), sizeof(struct tm));
        memcpy(&datosApp.clock.date, gmtime(&datosApp.clock.time), sizeof(struct tm));
        

        applyTZ(&datosApp.clock);
        //printf("ntp_udp_recv: reloj en hora\n");
        //printf("ntp_udp_recv: time: %02d:%02d:%02d\n", reloj.date.tm_hour, reloj.date.tm_min, reloj.date.tm_sec);

        datosApp.clock.estado = EN_HORA;
        datosApp.clock.timeValid = TRUE;
	// clean up connection
	if (pCon) {

                printf("ntp_udp_recv-->reloj sincronizado\n");
		espconn_delete(pCon);
		free(pCon->proto.udp);
		free(pCon);
		pCon = 0;
	}
}

 void  ntp_get_time(NTP_CLOCK *clock) {

	ntp_t ntp;
        uint8 i;
        

        clock->estado = SINCRONIZANDO;
	// set up the udp "connection"
	pCon = (struct espconn*)os_zalloc(sizeof(struct espconn));
	pCon->type = ESPCONN_UDP;
	pCon->state = ESPCONN_NONE;
	pCon->proto.udp = (esp_udp*)os_zalloc(sizeof(esp_udp));
	pCon->proto.udp->local_port = espconn_port();
	pCon->proto.udp->remote_port = 123;
        printf("ntp_get_time--> Pidiendo la hora...\n");
	memcpy(pCon->proto.udp->remote_ip, clock->ntpServerActive, 4);
        //printf("ntp_get_time--> %d.%d.%d.%d\n", pCon->proto.udp->remote_ip[0], 
        //        pCon->proto.udp->remote_ip[1],pCon->proto.udp->remote_ip[2],pCon->proto.udp->remote_ip[3]);


	// create a really simple ntp request packet
	memset(&ntp, 0, sizeof(ntp_t));
	ntp.options = 0b00100011; // leap = 0, version = 4, mode = 3 (client)

	// set timeout timer
	os_timer_disarm(&ntp_timeout);
	os_timer_setfn(&ntp_timeout, (os_timer_func_t*)ntp_udp_timeout, pCon);
	os_timer_arm(&ntp_timeout, clock->ntpTimeOut, 0);

	// send the ntp request
	espconn_create(pCon);
	espconn_regist_recvcb(pCon, ntp_udp_recv);
	espconn_send(pCon, (uint8*)&ntp, sizeof(ntp_t));
}

 void  applyTZ(struct NTP_CLOCK *clock) {
     
     struct tm *time;
     time = &clock->date;


	bool dst = false;

	// apply base timezone offset
	time->tm_hour += clock->ntpTimeZone; // e.g. central europe

	// call mktime to fix up (needed if applying offset has rolled the date back/forward a day)
	// also sets yday and fixes wday (if it was wrong from the rtc)
	mktime(time);

	// work out if we should apply dst, modify according to your local rules
    if (time->tm_mon < 2 || time->tm_mon > 9) {
		// these months are completely out of DST
	} else if (time->tm_mon > 2 && time->tm_mon < 9) {
		// these months are completely in DST
		dst = true;
	} else {
		// else we must be in one of the change months
		// work out when the last sunday was (could be today)
		int previousSunday = time->tm_mday - time->tm_wday;
		if (time->tm_mon == 2) { // march
			// was last sunday (which could be today) the last sunday in march
			if (previousSunday >= 25) {
				// are we actually on the last sunday today
				if (time->tm_wday == 0) {
					// if so are we at/past 2am gmt
					int s = (time->tm_hour * 3600) + (time->tm_min * 60) + time->tm_sec;
					if (s >= 7200) dst = true;
				} else {
					dst = true;
				}
			}
		} else if (time->tm_mon == 9) {
			// was last sunday (which could be today) the last sunday in october
			if (previousSunday >= 25) {
				// we have reached/passed it, so is it today?
				if (time->tm_wday == 0) {
					// change day, so are we before 1am gmt (2am localtime)
					int s = (time->tm_hour * 3600) + (time->tm_min * 60) + time->tm_sec;
					if (s < 3600) dst = true;
				}
			} else {
				// not reached the last sunday yet
				dst = true;
			}
		}
	}

	if (dst) {
		// add the dst hour
            //printf("Se aplica horario de Verano\n");
		time->tm_hour += 1;
		// mktime will fix up the time/date if adding an hour has taken us to the next day
		mktime(time);
		// don't rely on isdst returned by mktime, it doesn't know about timezones and tends to reset this to 0
		time->tm_isdst = 1;
                clock->ntpSummerTime = 1;
	} else {
		time->tm_isdst = 0;
                clock->ntpSummerTime = 0;
	}

}

 void relojTiempoReal(NTP_CLOCK *clock) {
     
     
     static uint8 clockExpired=0;
     static uint8 nretries=0;
     static uint16 timeLastRetry = TIME_RETRY;
     bool resRetry;
     static int programaFuturo;
     static bool pintado = false;

     

     if(clock->timeValid == TRUE)  {
         clock->time ++;
         memcpy(&clock->date, gmtime(&clock->time), sizeof(struct tm));
         applyTZ(clock);
         //Condicion de arranque desde 0
         if((datosApp.estadoProgramacion == INVALID_PROG) &&(datosApp.nProgramacion > 0)) {
             printf("relojTiempoReal-->Ordenando programaciones en fase inicial\n");
             programaFuturo = ejecutarAccionPrograma(&datosApp, ARRANQUE);
             datosApp.estadoProgramacion = VALID_PROG;
         }
         //printf("quedan: %d\n", datosApp.programacion[programaFuturo].programa - datosApp.clock.time);
         // Ha llegado el momento de aplicar la programacion.
         if ((datosApp.programacion[programaFuturo].programa - datosApp.clock.time) == 0) {
             printf("relojTiempoReal-->llegamos al temporizador y ejecutamos la accion que proceda\n");
             programaFuturo = ejecutarAccionPrograma(&datosApp, CAMBIO_PROGRAMA);
             
             
             
             
         }
         
         if((clock->date.tm_hour == 0) && (clock->date.tm_min == 0) && (clock->date.tm_sec == 0)) {
             //recalculamos los tiempos para actualizar las programaciones.
             programaFuturo = ejecutarAccionPrograma(&datosApp, MEDIA_NOCHE);
             //ordenarListaProgramas(datosApp.programacion, datosApp.nProgramacion, clock);
         }
         

     }


     
     switch(clock->estado) {
         case SIN_HORA:
             printf("relojTiempoReal-->SIN_HORA\n");
             nretries = 0;
             if ((wifi_station_get_connect_status() == STATION_GOT_IP) == TRUE) {
                 ntp_get_time(clock);
             }
            
            break;
         case EN_HORA:
             
             // Hora valida
             if (pintado == false) {
                 pintarLcd(CLOCK, PAINT);
                 pintado = true;
             }
             clock->timeValid = TRUE;
             nretries = 0;
             timeLastRetry = TIME_RETRY;
             clockExpired++;
             if (clockExpired == clock->ntpTimeExp) {
                 clock->estado = EXPIRADO;
                 clockExpired = 0;
             }
             break;

         case EXPIRADO:
             //ntpServerDefault(clock);
             //Consultamos al servidor ntp para volver de nuevo a estar en hora
             if (wifi_station_get_connect_status() == STATION_GOT_IP) {

                 if  (timeLastRetry == TIME_RETRY) {
                     ntp_get_time(clock);
                 }
             }
             //Si hay wifi y no se ha reintentado, pedimos el ntp.
             
             break;
         case SINCRONIZANDO:
             // Estado transitorio de la hora mientras se espera respuesta del servidor ntp
             printf("relojTiempoReal-->SINCRONIZANDO...\n");
             break;
             
         case ERROR_NTP:
             //Fallo la ocnsulta al servidor ntp!!
             printf("relojTiempoReal-->ERROR_NTP!!\n");
             if (wifi_station_get_connect_status() == STATION_GOT_IP) {
                 if(clock->timeValid == FALSE) {
                     printf("relojTiempoReal-->relojTiempoReal:Reintento %d:\n", nretries);
                     printf("relojTiempoReal-->Servidor activo: %d.%d.%d.%d\n", clock->ntpServerActive[0], clock->ntpServerActive[1],clock->ntpServerActive[2],clock->ntpServerActive[3]);
                     ntp_get_time(clock);
                     
                     if ((resRetry = retryQueryNtp(clock, &nretries)) == FALSE) nretries = 0;
                     else nretries++;
                     
                 } else {
                     timeLastRetry--;
                     printf("relojTiempoReal-->Reintento en %d segundos\n",timeLastRetry );
                     if(timeLastRetry == 0){
                         printf("relojTiempoReal-->relojTiempoReal:Reintento %d:\n", nretries);
                         timeLastRetry = TIME_RETRY;
                         ntp_get_time(clock);
                         if ((resRetry = retryQueryNtp(clock, &nretries)) == FALSE) nretries = 0;
                         else nretries++;
                     }
                 }
             }
             break;
             
             
             
             
     }
#ifdef LCD
     if (clock->date.tm_sec == 0) {
         pintarLcd(CLOCK, PAINT);
     }
    
#endif
     
     
 }
 
 bool retryQueryNtp(NTP_CLOCK *clock, uint8 *nretries) {
     
     uint8 i;
     //*nretries = *nretries + 1;
     if (*nretries <= MAX_RETRIES_QUERYS_NTP) {
         //printf("copiando ntp activo en el reintento %d\n", *nretries);
         for(i=0;i<4;i++) {
             
             clock->ntpServerActive[i] = clock->ntpServer[*nretries][i];
         }
         //memcpy(clock->ntpServerActive, clock->ntpServer[*nretries], 4 * sizeof(uint8));
         return true;
     }
     else {
         printf("retryQueryNtp: Agotado el numero de reintentos");
         *nretries = 0;
         return false;
     }
     
     
 }
 
 void initParNtp(NTP_CLOCK *clock) {
        
     clock->estado = SIN_HORA;
     clock->ntpTimeOut = 5000; // in ms
     clock->ntpTimeExp = 86400;//86400;
     clock->timeValid = false;
     clock->ntpTimeZone = 1;
     ntpServerDefault(clock);
     

 }
 
 void ntpServerDefault(NTP_CLOCK *clock) {

     uint8 i;
     //es.pool.ntp.org.
     clock->ntpServer[0][0] = 193;
     clock->ntpServer[0][1] = 145;
     clock->ntpServer[0][2] = 15;
     clock->ntpServer[0][3] = 15;
     
     clock->ntpServer[1][0] = 158;
     clock->ntpServer[1][1] = 227;
     clock->ntpServer[1][2] = 98;
     clock->ntpServer[1][3] = 15;

     clock->ntpServer[2][0] = 213;
     clock->ntpServer[2][1] = 251;
     clock->ntpServer[2][2] = 52;
     clock->ntpServer[2][3] = 234;
     
     clock->ntpServer[3][0] = 81;
     clock->ntpServer[3][1] = 19;
     clock->ntpServer[3][2] = 96;
     clock->ntpServer[3][3] = 148;

     //de.pool.ntp.org.
     clock->ntpServer[4][0] = 109;
     clock->ntpServer[4][1] = 239;
     clock->ntpServer[4][2] = 58;
     clock->ntpServer[4][3] = 247;
     
     for(i=0;i<4;i++) {
         clock->ntpServerActive[i] = clock->ntpServer[0][i];
     }
     //memcpy(clock->ntpServerActive, clock->ntpServer[0], 4 * sizeof(uint8));
     printf("Servidor activo: %d.%d.%d.%d\n", clock->ntpServerActive[0], clock->ntpServerActive[1],clock->ntpServerActive[2],clock->ntpServerActive[3]);
     
 }
 
 
 void realClock(NTP_CLOCK *clock) {
     os_timer_disarm(&realTimeClock);
     os_timer_setfn(&realTimeClock, (os_timer_func_t*) relojTiempoReal, clock);
     os_timer_arm(&realTimeClock, 1000, 1);

 }
 

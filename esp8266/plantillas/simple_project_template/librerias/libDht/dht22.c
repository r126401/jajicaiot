/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "dht22.h"
#include "gpio.h"

#define DHT_DEBUG printf

char* DHTFloat2String(char* buffer, float value)
{
  sprintf(buffer, "%d.%d", (int)(value),(int)((value - (int)value)*100));
  return buffer;
}



float calcularTemperaturaAm2302( uint8 tempInt, uint8 tempDec) {
    
    
    float resultado, tempMedida;
    
    if (tempInt >= 128) {
    
    tempInt -= 128;
    printf("tempInt: %d\n", tempInt);
    resultado = tempInt << 8;
    resultado += tempDec;
    tempMedida = (float) resultado;
    tempMedida = tempMedida/-10;

}

else {
       resultado = tempInt << 8;
       resultado += tempDec;
       tempMedida = (float) resultado;
       tempMedida = (float) tempMedida/10;
 
}
    
    return tempMedida;

}

int parteDecimal(float numero) {
    
    int entero;
    float parte;
    int decimal;
    
    entero = (int) numero;
    parte = numero - (float) entero;
    
    
    parte = parte*100;
    decimal = (int) parte;
    return parte;
}


float scale_humidity(DHTType sensor_type, uint8 *data)
{
	if(sensor_type == DHT11) {
		return (float) data[0];
	} else {
		float humidity = data[0] * 256 + data[1];
		return humidity /= 10;
	}
}

float scale_temperature(DHTType sensor_type, uint8 *data)
{
	if(sensor_type == DHT11) {
		return (float) data[2];
	} else {
		float temperature = data[2] & 0x7f;
		temperature *= 256;
		temperature += data[3];
		temperature /= 10;
		if (data[2] & 0x80)
			temperature *= -1;
		return temperature;
	}
}


bool ICACHE_FLASH_ATTR StartDHT() {
    
    
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTMS_U, FUNC_GPIO14);
    
    GPIO_OUTPUT_SET(GPIO_ID_PIN(PINDHT),1);

     //(2) Bajamos el pulso a 0 y lo mantenemos mas de 18 ms para que dht detecte el pulso y se prepare
     GPIO_OUTPUT_SET(PINDHT,0);
     os_delay_us(20000);
     
     // (3) Ekevamos el pulso a 1 y lo mantenemos 20 us para que comience la respuesta de dht
      GPIO_OUTPUT_SET(PINDHT,1);
      os_delay_us(20);
      
      // (3.a) Deshabilitamos GPIO2 como salida para leer de DHT
     GPIO_DIS_OUTPUT(PINDHT);
     // (3.b) Configuramos GPIO2 para leer la respuesta de DHT
     GPIO_AS_INPUT(PINDHT);
      //while((GPIO_INPUT_GET(2) == 1));
     
      // (3.c) Esperamos a que DHT ponga el pulso a 0 como comienzo de respuesta. Contamos el tiempo que esta a 1
      while (GPIO_INPUT_GET(PINDHT) == 0);//contador1++;
     
     while(GPIO_INPUT_GET(PINDHT) == 1);//contador2++;
     //printf ("contadores: %d, %d\n", contador1, contador2);
     return TRUE;
      
     
}





bool DHTRead(uint8 *data)
{
	int counter = 0;
	int laststate = 1;
	int i = 0;
	int j = 0;
	int checksum = 0;
	//int data[100];
	data[0] = data[1] = data[2] = data[3] = data[4] = 0;
	uint8_t pin = PINDHT;
                //PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTMS_U,FUNC_GPIO5);
                PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTMS_U, FUNC_GPIO14);    
	// Wake up device, 250ms of high
	GPIO_OUTPUT_SET(pin, 1);
        for (i=0;i<5;i++) {
            os_delay_us(50000);
        }
	// Hold low for 20ms
	GPIO_OUTPUT_SET(pin, 0);
	os_delay_us(20000);
	// High for 40ns
	GPIO_OUTPUT_SET(pin, 1);
	os_delay_us(40);
	// Set DHT_PIN pin as an input
	GPIO_DIS_OUTPUT(pin);

	// wait for pin to drop?
	while (GPIO_INPUT_GET(pin) == 1 && i < DHT_MAXCOUNT) {
		os_delay_us(1);
		i++;
	}

	if(i == DHT_MAXCOUNT)
	{
		DHT_DEBUG("DHT: Failed to get reading from GPIO%d, dying\r\n", pin);
	    return false;
	}

	// read data
	for (i = 0; i < DHT_MAXTIMINGS; i++)
	{
		// Count high time (in approx us)
		counter = 0;
		while (GPIO_INPUT_GET(pin) == laststate)
		{
			counter++;
			os_delay_us(1);
			if (counter == 1000)
				break;
		}
		laststate = GPIO_INPUT_GET(pin);
		if (counter == 1000)
			break;
		// store data after 3 reads
		if ((i>3) && (i%2 == 0)) {
			// shove each bit into the storage bytes
			data[j/8] <<= 1;
			if (counter > DHT_BREAKTIME)
				data[j/8] |= 1;
			j++;
		}
	}

	if (j >= 39) {
		checksum = (data[0] + data[1] + data[2] + data[3]) & 0xFF;
	    DHT_DEBUG("DHT22: %d %d %d %d [%d] CS: %d\r\n",
	              data[0], data[1], data[2], data[3], data[4], checksum);
		if (data[4] == checksum) {
                                        return true;
		} else {
			//DHT_DEBUG("Checksum was incorrect after %d bits. Expected %d but got %d\r\n", j, data[4], checksum);
			DHT_DEBUG("DHT: Checksum incorrecto despues de %d bits. Esperabamos %d pero vinieron %d \r\n",
		                j, data[4], checksum);
		    return false;
		}
	} else {
		//DHT_DEBUG("Got too few bits: %d should be at least 40\r\n", j);
	    DHT_DEBUG("DHT: Obtuvimos pocos bits: %d deberían ser al menos 40 en (GPIO%d)\r\n", j, pin);
	    return false;
	}
	return true;
}



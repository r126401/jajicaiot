/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

//#include "mqtt.h"
//#include "wifi.h"
//#include "user_config.h"
#include "dialogoJson.h"
#include "user_config.h"

#include "appdisplay.h"


 void  mqtt_task(void *pvParameters) {
    
    int ret;
    struct Network network;
    MQTTMessage message;
    MQTTClient client = DefaultClient;
    char mqtt_client_id[20];
    unsigned char mqtt_buf[LENBUFFER];
    unsigned char mqtt_readbuf[LENBUFFER];
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    MQTT_PARAM* mqttData;
    MQTT_PACKET packet;
    char topicSubscribe[75];
    struct DATOS_APLICACION *datosApp;
    datosApp = (struct DATOS_APLICACION*) pvParameters;
    mqttData = &datosApp->parametrosMqtt;
    
    //mqttData = (MQTT_PARAM*) pvParameters;
    
    sprintf(topicSubscribe, "%s%s", mqttData->prefix, mqttData->subscribe);
    NewNetwork(&network);
    
    while (1)
    {
        
        if (datosApp->estadoApp == NORMAL) {
            // Wait until wifi is up
            xSemaphoreTake(wifi_alive, portMAX_DELAY);

            // Unique client ID
            strcpy(mqtt_client_id, "ESP-");
            strcat(mqtt_client_id, get_my_id());


            DBG("(Re)connecting to MQTT server %s ... ", mqttData->broker);
            ret = ConnectNetwork(&network, mqttData->broker, mqttData->port);
            if (!ret)
            {
                DBG("ok.\r\n");
                mqttData->estadoBroker = ON_LINE;
                NewMQTTClient(&client, &network, 5000, mqtt_buf, LENBUFFER, mqtt_readbuf, LENBUFFER);
                data.willFlag = 0;
                data.MQTTVersion = 3;
                data.clientID.cstring = mqtt_client_id;
                data.username.cstring = mqttData->user;
                data.password.cstring = mqttData->password;
                data.keepAliveInterval = 10;
                data.cleansession = 0;
                DBG("Send MQTT connect ...");
                ret = MQTTConnect(&client, &data);
                if (!ret)
                {
                    DBG("ok.\r\n");
    #ifdef LCD
                pintarLcd(BROKER_ON,PAINT);
    #endif

                    // Subscriptions
                    MQTTSubscribe(&client, topicSubscribe, QOS1, topic_received);
                    // Empty the publish queue
                    xQueueReset(publish_queue);

                    //system_deep_sleep(3000000);

                    while (1)
                    {
                        // Publish all pending messages
                        while (xQueueReceive(publish_queue, &packet, 0) == pdTRUE)
                        {
                            packet.datos[MAXPAYLOAD - 1] = '\0';
                            //MQTTMessage message;
                            message.payload = packet.datos;
                            message.payloadlen = strlen(packet.datos);
                            message.dup = 0;
                            message.qos = QOS1;
                            message.retained = 0;
                            DBG("topic: %s, datos:%s\n", packet.topic, packet.datos);
                            ret = MQTTPublish(&client, packet.topic, &message);
                            if (ret != SUCCESS)
                                break;
                        }
                        // Receiving / Ping
                        ret = MQTTYield(&client, 1000);
                        if (ret == DISCONNECTED)
                        {
                            break;
                        }
                    }
                    DBG("Connection broken, request restart\r\n");
                    mqttData->estadoBroker = OFF_LINE;         
    #ifdef LCD
                pintarLcd(BROKER_ON,NO_PAINT);
    #endif

                }
                else
                {
                    DBG("failed.\r\n");
                    mqttData->estadoBroker = OFF_LINE;
    #ifdef LCD
                pintarLcd(BROKER_ON,NO_PAINT);
    #endif

                }
                DisconnectNetwork(&network);
            }
            else
            {
                DBG("failed.\r\n");
                mqttData->estadoBroker = OFF_LINE;
    #ifdef LCD
                pintarLcd(BROKER_ON,NO_PAINT);
    #endif


            }
            vTaskDelay(1000 / portTICK_RATE_MS);            
        }


    }
    DBG("MQTT task ended\r\n", ret);
    mqttData->estadoBroker = OFF_LINE;
#ifdef LCD
            pintarLcd(BROKER_ON,NO_PAINT);
#endif
    
    vTaskDelete(NULL);
}



LOCAL const char * ICACHE_FLASH_ATTR get_my_id(void)
{
    // Use MAC address for Station as unique ID
    static char my_id[13];
    static bool my_id_done = false;
    int8_t i;
    uint8_t x;
    if (my_id_done)
        return my_id;
    if (!wifi_get_macaddr(STATION_IF, my_id))
        return NULL;
    for (i = 5; i >= 0; --i)
    {
        x = my_id[i] & 0x0F;
        if (x > 9) x += 7;
        my_id[i * 2 + 1] = x + '0';
        x = my_id[i] >> 4;
        if (x > 9) x += 7;
        my_id[i * 2] = x + '0';
    }
    my_id[12] = '\0';
    my_id_done = true;
    return my_id;
}


LOCAL void ICACHE_FLASH_ATTR topic_received(MessageData* md)
{
    //char mensaje[100];
    char *peticion;
    char *respuesta;
    char *topic;
    int i;
    int len;
    cJSON *root;

    

    // 1.- Recibimos la peticion y la guardamos.

    peticion = zalloc(md->message->payloadlen+1);
    if(peticion == NULL) {
        printf("topic_received-->No puedo asignar %d bytes de memoria\n",md->message->payloadlen );
        return;
    }
    strncpy(peticion, md->message->payload, (int) md->message->payloadlen);
    DBG("topic_received-->peticion: %s\n%d, longitud cadena:%d\n", peticion, md->message->payloadlen, strlen(peticion));
    // 2.- Enviamos la peticion para ejecutar el comando pedido.
    root = analizarComando(peticion, &datosApp);
    free(peticion);
    peticion = NULL;
    // Solo si devolvemos NULL no enviamos nada mas
    if (root == NULL) {
        printf("topic_received-->No hay nada mas que enviar\n");
        cJSON_Delete(root);
        root = NULL;

        return;
    }
    respuesta = cJSON_Print(root);
    cJSON_Delete(root);
    root = NULL;
    
    // preparamos los datos para el envio
    topic = zalloc(75*sizeof(char));
    strcpy(topic, datosApp.parametrosMqtt.prefix);
    strcat(topic, datosApp.parametrosMqtt.publish);
    
    if (respuesta == NULL) {
        printf("topic_received-->No se va a poder enviar nada\n");
        xQueueReset(publish_queue);
        free(topic);
        free(respuesta);
        return;
    } else {

        printf("topic_received-->Se va a enviar %s\n", respuesta);

        //Enviamos por mqtt la respuesta
        printf("topic_received-->longitud de respuesta :%d\n", strlen(respuesta));
        sendMqttMessage(topic, respuesta);
    }
    printf("topic_received-->libero los recursos asociados\n");
    free(topic);
    free(respuesta);
    
#ifdef LCD
            pintarLcd(RECEIVING,PAINT);
#endif

    
  

     
}

bool sendMqttMessage(char* topic, char *message) {
    
    MQTT_PACKET packet;
    

    strcpy(packet.topic, topic);
    strcpy(packet.datos, message);
    
    DBG("sendMqttMessage-->topic: %s", packet.topic);

    //printf("vamos a enviar\n");
    if (wifi_station_get_connect_status() == STATION_GOT_IP)  {
        if (xQueueSend(publish_queue, &packet, 0) == pdFALSE) {
           
            DBG("sendMqttMessage--> Cola desbordada\n");
            return false;
            
        }else {
            DBG("sendMqttMessage--> enviado packet\n");
#ifdef LCD
            pintarLcd(TRANSMITING, PAINT);
#endif
            return true;
        }
    } else {
        DBG("sendMqttMessage-->no ip, no send nothing\r\n");
        return false;
    }

    
}
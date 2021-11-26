/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

//#include "mqtt.h"
//#include "wifi.h"
#include "user_config.h"
#include "dialogoJson.h"



 void  mqtt_task(void *pvParameters) {
    
    int ret;
    struct Network network;
    MQTTClient client = DefaultClient;
    char mqtt_client_id[20];
    unsigned char mqtt_buf[LENBUFFER];
    unsigned char mqtt_readbuf[LENBUFFER];
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    MQTT_PARAM* mqttData;
    MQTT_PACKET packet;
    char topicSubscribe[75];
    DATOS_APLICACION *datosApp;
    
    datosApp = (DATOS_APLICACION*) pvParameters;
    //mqttData = (MQTT_PARAM*) pvParameters;
    mqttData = &(datosApp->parametrosMqtt);
    
    sprintf(topicSubscribe, "%s%s", mqttData->prefix, mqttData->subscribe);
    NewNetwork(&network);
    ////notificarAlarma(datosApp, ALARMA_MQTT, ALARMA_INDETERMINADA, NO);
    while (1)
    {
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
                appUser_notificarBrokerConectado();
                //notificarAlarma(datosApp, ALARMA_MQTT, ALARMA_NO_ACTIVA, SI);
                
                // Subscriptions
                MQTTSubscribe(&client, topicSubscribe, QOS0, topic_received);
                // Empty the publish queue
                xQueueReset(publish_queue);
                appUser_notificarArranque(datosApp, 0);
                while (1)
                {
                    // Publish all pending messages
                    while (xQueueReceive(publish_queue, &packet, 0) == pdTRUE)
                    {
                        printf("mqtt_task--> procesando envio\n");
                        packet.datos[MAXPAYLOAD - 1] = '\0';
                        MQTTMessage message;
                        message.payload = packet.datos;
                        message.payloadlen = strlen(packet.datos);
                        message.dup = 0;
                        message.qos = QOS0;
                        message.retained = 0;
                        DBG("mqtt_task-->topic: %s, datos:\n%s\nlongitud:%d\n", packet.topic, packet.datos, message.payloadlen);
                        ret = MQTTPublish(&client, packet.topic, &message);
                        if (ret != SUCCESS)
                            break;
                    }
                    // Receiving / Ping
                    ret = MQTTYield(&client, 1000);
                    if (ret == DISCONNECTED)
                    {
                        //notificarAlarma(datosApp, ALARMA_MQTT, ALARMA_ACTIVA, NO);
                        break;
                    }
                }
                DBG("Connection broken, request restart\r\n");
                //notificarAlarma(datosApp, ALARMA_MQTT, ALARMA_ACTIVA, NO);
                appUser_notificarBrokerDesconectado();
            }
            else
            {
                DBG("failed.\r\n");
                //notificarAlarma(datosApp, ALARMA_MQTT, ALARMA_ACTIVA, NO);
                appUser_notificarBrokerDesconectado();
            }
            DisconnectNetwork(&network);
        }
        else
        {
            DBG("failed.\r\n");
            //notificarAlarma(datosApp, ALARMA_MQTT, ALARMA_ACTIVA, NO);
            appUser_notificarBrokerDesconectado();
        }
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
    DBG("MQTT task ended\r\n", ret);
    vTaskDelete(NULL);
}



 const char * get_my_id(void)
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
    char *respuesta = NULL;
    char *topic;
    int i;
    int len;
    cJSON *root;
    static bool bloqueo = FALSE;

    printf("topic_received-->Memoria antes: %d\n", system_get_free_heap_size());
    if (bloqueo == false) {
        
        bloqueo = TRUE;
        // 1.- Recibimos la peticion y la guardamos.

        peticion = zalloc(md->message->payloadlen);
        strncpy(peticion, md->message->payload, (int) md->message->payloadlen);
        DBG("peticion: %s y longitud: %d\n", peticion, md->message->payloadlen );
        // 2.- Enviamos la peticion para ejecutar el comando pedido.
        root = analizarComando(peticion, &datosApp);
        free(peticion);
        if (root != NULL) {

            // 3.- Una vez ejecutado enviamos la respuesta y liberamos recursos.

            // obtenemos el json de la respuesta para escribir
            respuesta = cJSON_Print(root);


            // preparamos los datos para el envio
            topic = zalloc(75*sizeof(char));
            strcpy(topic, datosApp.parametrosMqtt.prefix);
            strcat(topic, datosApp.parametrosMqtt.publish);

            //Enviamos por mqtt la respuesta
            if (respuesta != NULL) sendMqttMessage(topic, respuesta);
            //liberamos el texto de la peticion, la respuesta y el objeto cJSON
            free(topic);
            free(respuesta);
            cJSON_Delete(root);

        }

        printf("topic_received-->Memoria despues: %d\n", system_get_free_heap_size());
        bloqueo = FALSE;
    } else {
        printf("topic_received-->Peticion descartada\n");
    }
    
  

     
}

bool sendMqttMessage(char* topic, char *message) {
    
    MQTT_PACKET packet;
    

    //printf("sendMqttMessage-_> comenzamos\n");
    //printf("sendMqttMessage--> longitud del mensaje:%d\n", strlen(message));
    strcpy(packet.topic, topic);
    strcpy(packet.datos, message);
    //printf("sendMqttMessage--> longitud del mensaje: %d\n", strlen(message));
    
    //printf("sendMqttMessage-->\n topic: %s, longitud:%d\n %s\n", topic, strlen(message), message);

    if (wifi_station_get_connect_status() == STATION_GOT_IP) {
        if (xQueueSend(publish_queue, &packet, 10) == pdFALSE) {
           
            DBG("Cola desbordada\n");
            return false;
            
        }else {
            DBG("enviado packet\n");
            return true;
        }
    } else {
        DBG("no ip, no send nothing\r\n");
        return false;
    }

    
}

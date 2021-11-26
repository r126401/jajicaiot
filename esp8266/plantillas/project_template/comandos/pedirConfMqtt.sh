#!/bin/bash

contador=0

while true; do
((contador++))
echo contador es $contador

mosquitto_pub -h jajica.ddns.net -t /comandos -f mensajeMqtt.json
mosquitto_pub -h jajica.ddns.net -t /comandos -f mensajeClock.json
mosquitto_pub -h jajica.ddns.net -t /comandos -f mensajeWifi.json
mosquitto_pub -h jajica.ddns.net -t /comandos -f configApp.json
sleep 10
done




# iotOnOff

IotOnff es una aplicacion para controlar dispositivos interruptores.


python instalar.py jajicaiot.ddns.net 1883 /mnt/servidorWeb /mnt/servidorWeb iotCronotempLcd 2006161948 /mnt/servidorWeb jajicaiot.ddns.net/firmware/iotCronoTemp/

instalar.py es un script que genera el fichero lastVersion en el que se indican los datos de la ultima version compilada para el posterior upgrade ota desde el dispositivo. Por tanto, es necesario aportar al script la siguiente informacion.

Servidor mqtt: jajicaiot.ddns.net
Puerto mqtt: 1883
directorio de instalacion: /mnt/servidorWeb es el directorio base del servidor que recibira las peticiones ota
directorio imagen: servidorWeb es el directorio donde se alamacenara el binario recien compilado.
nombre_proyecto: Se utiliza para meter dentro del directorio dentro del dispositivo la imagen
version_ota: Es la version del dispositivo
directorio lastVersion
 
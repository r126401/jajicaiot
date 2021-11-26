# -*- coding: utf-8 -*-

# To change this license header, choose License Headers in Project Properties.
# To change this template file, choose Tools | Templates
# and open the template in the editor.

import sys
import commands

imagen1 = sys.argv[1]
app = sys.argv[2]
path = sys.argv[3]
nombre = sys.argv[4]
version = sys.argv[5]
dirLastVersion = sys.argv[6]
nombreLastVersion = dirLastVersion + "/lastVersion.txt"
fecha = commands.getoutput('date +%y%m%d%H%M')

comando1 = "cp " + imagen1 + ".bin" + " " + path + nombre + app + "-" + fecha + ".bin"
comando3 = "rm " + path + nombre + app + ".bin"
comando5 = "ln -s " + path + nombre + app + "-" + fecha + ".bin" + " " + path + nombre + app + ".bin"

print comando1
print comando3
print comando5

commands.getoutput(comando1)
commands.getoutput(comando3)
commands.getoutput(comando5)


ficheroVersion = "{\
  \"token\": \"5586625c-d148-482b-a71d-662399f97414\",\
  \"date\": \"13/01/2019 20:14:03\",\
  \"dlgComando\": 100,\
  \"idDevice\": \"OtaServer\",\
    \"comando\": 100,\
    \"dlgResultCode\": 200,\
    \"otaServer\": \"jajica.ddns.net\",\
    \"otaPort\": 80,\
    \"otaUrl\": \"jajicaiot.ddns.net/firmware/iotCronoTemp/\",\
    \"otaFile\": \"newVersion\",\
    \"otaVersion\":"
    

lastVersion = ficheroVersion + " " + version + "}\n"

fichero = open(nombreLastVersion, "w")
fichero.write(lastVersion);
fichero.close



#print lastVersion




import sys
import subprocess
import json
from _operator import sub



servidorMqtt = sys.argv[1]
#puertoMqtt = type(int(sys.argv[2]))
puertoMqtt = sys.argv[2]
puertoMqtt = int(puertoMqtt)
directorioInstalacion = sys.argv[3]
directorioImagen = sys.argv[4]
origenOta = sys.argv[5]
versionOta = sys.argv[6]
directorioLastVersion = sys.argv[7]
url = sys.argv[8]

destinoOta = origenOta + "_" + versionOta +".bin"
origenOta = origenOta + ".ota.bin"
ficheroLastVersion = "lastVersion.json"






def obtener_fecha():
    fecha = subprocess.getoutput('date +%y%m%d%H%M')
    return fecha
    
def crearFichero(servidor, puerto, directorio, destinoOta, otaVersion) :
    
    fecha = subprocess.getoutput('date +%d/%m/%y\ %H:%M')
    cadena = {}
    cadena['token'] = '5586625c-d148-482b-a71d-662399f97414'
    cadena['date'] = fecha
    cadena['dlgComando'] = 100
    cadena['idDevice'] = "otaServer"
    cadena['dlgResultCode'] = 200
    cadena['otaServer'] = servidor
    cadena['otaPort'] = puerto
    cadena['otaUrl'] = directorio
    cadena['otaFile'] = destinoOta
    cadena['otaVersion'] = otaVersion
    
    
    return json.dumps(cadena)


def copiarFichero(textoJson, directorioInstalacion, origenOta, destinoOta, directorioLastVersion):
    
    nombre_fichero = ficheroLastVersion
    fichero = open(nombre_fichero, "w")
    fichero.write(textoJson)
    fichero.write("\n")
    fichero.close
    
    comando1 = "cp build/" + origenOta + " " + directorioInstalacion + destinoOta
    comando2 = "cp " + ficheroLastVersion + " " + directorioLastVersion
    print(comando1)
       
    subprocess.getoutput(comando1)
    subprocess.getoutput(comando2)
    
def copiarLastVersion(ficheroLastVersion, directorioLastVersion):
    
    
    comando = "cp " + ficheroLastVersion + " " + directorioLastVersion
    print (comando)
    subprocess.getoutput(comando)
    comando2 = "rm ./" + ficheroLastVersion
    print(comando2)
    subprocess.getoutput(comando2)

    
def enlacelastVersion(ficheroLastVersion, directorioInstalacion, ficheroVersion):
    
    comando = "cd " + directorioInstalacion + ";rm " + ficheroVersion + ";ln -s " + ficheroLastVersion + " " + ficheroVersion
    print(comando)
    subprocess.getoutput(comando)


texto = crearFichero(servidorMqtt, puertoMqtt, url, destinoOta, versionOta)

print("el fichero es: \n" + texto)

copiarFichero(texto, directorioInstalacion, origenOta, destinoOta, directorioLastVersion)
copiarLastVersion(ficheroLastVersion, directorioLastVersion)
enlacelastVersion(destinoOta, directorioInstalacion, "newVersion")

    
    
    


 




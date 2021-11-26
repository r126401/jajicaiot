#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#
SW_VERSION :="`date +'%y%m%d%H%M'`"
CFLAGS := -DVERSION_SW=$(SW_VERSION)
PROJECT_NAME := plantilla


EXTRA_COMPONENT_DIRS = /opt/esp/componentes ~/temporal/esp8266/idf/proyectos/$(PROJECT_NAME)/components
COMPONENT_PRIV_INCLUDEDIRS = /opt/esp/componentes


include $(IDF_PATH)/make/project.mk


# PARAMETROS DE CONFIGURACION PARA OTA
DIR_LAST_VERSION=/trastero/dispositivosIot/lastVersion/$(PROJECT_NAME)
EJECUTABLE_COPIAR=instalar.py
SERVIDOR_MQTT = "jajicaiot.ddns.net"
PUERTO_MTT = 80
URL = "/firmware/$(PROJECT_NAME)/"
DIRECTORIO_IMAGEN = build/
FICHERO_OTA = $(PROJECT_NAME)_$(SW_VERSION)
DIRECTORIO_INSTALACION=/trastero/dispositivosIot/firmware/$(PROJECT_NAME)/
DIRECTORIO_LASTVERSION=/trastero/dispositivosIot/lastVersion/$(PROJECT_NAME)/


instalar:	ota	
	python $(EJECUTABLE_COPIAR) $(SERVIDOR_MQTT) $(PUERTO_MTT) $(DIRECTORIO_INSTALACION) $(DIRECTORIO_IMAGEN) $(PROJECT_NAME) $(SW_VERSION) $(DIRECTORIO_LASTVERSION) $(URL)
	

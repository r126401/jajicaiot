# esptool es la python tool para flashear el esp8266. 
#En mi caso /opt/Espressif/esp-open-sdk/xtensa-lx106-elf/bin/esptool.py

#SW_VERSION=`date +'%y.%m.%d %H:%M:%S'`
IMAGEN= "bin/upgrade/$(BIN_NAME)"
CAMINO="/trastero/dispositivosIot/firmware/iotOnOff/"
NOMBRE="newVersion"

DIR_LAST_VERSION="/trastero/dispositivosIot/lastVersion/iotOnOff"

ifeq ($(APP),1)

install:
	esptool.py --port /dev/ttyUSB0 --baud 115200 write_flash 0x01000 bin/upgrade/$(BIN_NAME).bin
	
else

install:
	esptool.py --port /dev/ttyUSB0 --baud 115200 write_flash 0x81000 bin/upgrade/$(BIN_NAME).bin
endif

	
IMAGEN1=$(BIN_NAME)



copiar:
	python copiar.py $(IMAGEN) $(APP) $(CAMINO) $(NOMBRE) $(SW_VERSION) $(DIR_LAST_VERSION)

sonoff:
	esptool.py --port /dev/ttyUSB0 --baud 115200 write_flash -fs 8m -fm dout 0x20000 bin/eagle.irom0text.bin
	
todo:
	rm -r bin/upgrade
	make clean
	make APP=1 SPI_SIZE_MAP=2
	make clean
	make APP=2 SPI_SIZE_MAP=2
	make APP=1 SPI_SIZE_MAP=2 copiar
	make APP=2 SPI_SIZE_MAP=2 copiar
	

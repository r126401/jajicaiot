# esptool es la python tool para flashear el esp8266. 
#En mi caso /opt/Espressif/esp-open-sdk/xtensa-lx106-elf/bin/esptool.py

SW_VERSION=`date +'%y%m%d%H%M'`
install:
	esptool.py --port /dev/ttyUSB0 --baud 115200 write_flash 0x81000 bin/upgrade/user2.4096.new.4.bin

copiar:
	cp bin/upgrade/user1.4096.new.4.bin /trastero/dispositivosIot/firmware/iotCronoTemp/newVersion1-$(SW_VERSION).bin
	cp bin/upgrade/user2.4096.new.4.bin /trastero/dispositivosIot/firmware/iotCronoTemp/newVersion2-$(SW_VERSION).bin


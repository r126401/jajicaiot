# esptool es la python tool para flashear el esp8266. 
#En mi caso /opt/Espressif/esp-open-sdk/xtensa-lx106-elf/bin/esptool.py


install:
	esptool.py --port /dev/ttyUSB0 --baud 115200 write_flash 0x01000 bin/upgrade/user1.4096.new.4.bin


copiar:
	cp bin/upgrade/user1.4096.new.4.bin /var/www/dispositivosIot/firmware/newVersion1.bin
	cp bin/upgrade/user2.4096.new.4.bin /var/www/dispositivosIot/firmware/newVersion2.bin

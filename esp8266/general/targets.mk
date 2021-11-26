flash:
	esptool.py --port /dev/ttyUSB0 --baud 115200 write_flash 0x01000 bin/upgrade/user1.4096.new.4.bin


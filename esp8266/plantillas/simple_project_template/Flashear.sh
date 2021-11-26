#!/bin/bash
# 
# File:   Flashear.bash
# Author: t126401
#
# Created on 19-nov-2016, 13:00:38
#



esptool.py --port /dev/ttyUSB0 --baud 115200 write_flash 0x01000 bin/upgrade/user1.4096.new.6.bin


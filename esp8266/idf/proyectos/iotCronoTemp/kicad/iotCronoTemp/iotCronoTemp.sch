EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L MCU_Espressif:ESP8266EX U?
U 1 1 5FC4373E
P 5600 3700
F 0 "U?" H 5600 2511 50  0000 C CNN
F 1 "ESP8266EX" H 5600 2420 50  0000 C CNN
F 2 "Package_DFN_QFN:QFN-32-1EP_5x5mm_P0.5mm_EP3.45x3.45mm" H 5600 2400 50  0001 C CNN
F 3 "http://espressif.com/sites/default/files/documentation/0a-esp8266ex_datasheet_en.pdf" H 5700 2400 50  0001 C CNN
	1    5600 3700
	1    0    0    -1  
$EndComp
$Comp
L Relay:SANYOU_SRD_Form_C K?
U 1 1 5FC44B4D
P 8750 3050
F 0 "K?" H 9180 3096 50  0000 L CNN
F 1 "SANYOU_SRD_Form_C" H 9180 3005 50  0000 L CNN
F 2 "Relay_THT:Relay_SPDT_SANYOU_SRD_Series_Form_C" H 9200 3000 50  0001 L CNN
F 3 "http://www.sanyourelay.ca/public/products/pdf/SRD.pdf" H 8750 3050 50  0001 C CNN
	1    8750 3050
	1    0    0    -1  
$EndComp
$Comp
L Sensor:DHT11 U?
U 1 1 5FC45DF5
P 3450 1900
F 0 "U?" H 3206 1854 50  0000 R CNN
F 1 "DHT11" H 3206 1945 50  0000 R CNN
F 2 "Sensor:Aosong_DHT11_5.5x12.0_P2.54mm" H 3450 1500 50  0001 C CNN
F 3 "http://akizukidenshi.com/download/ds/aosong/DHT11.pdf" H 3600 2150 50  0001 C CNN
	1    3450 1900
	-1   0    0    1   
$EndComp
$Comp
L Driver_Display:CR2013-MI2120 U?
U 1 1 5FC46240
P 8600 5050
F 0 "U?" H 8600 5831 50  0000 C CNN
F 1 "CR2013-MI2120" H 8600 5740 50  0000 C CNN
F 2 "Display:CR2013-MI2120" H 8600 4350 50  0001 C CNN
F 3 "http://pan.baidu.com/s/11Y990" H 7950 5550 50  0001 C CNN
	1    8600 5050
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW?
U 1 1 5FC48F98
P 2650 5350
F 0 "SW?" H 2650 5635 50  0000 C CNN
F 1 "SW_Push" H 2650 5544 50  0000 C CNN
F 2 "" H 2650 5550 50  0001 C CNN
F 3 "~" H 2650 5550 50  0001 C CNN
	1    2650 5350
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW?
U 1 1 5FC495BC
P 3400 5400
F 0 "SW?" H 3400 5685 50  0000 C CNN
F 1 "SW_Push" H 3400 5594 50  0000 C CNN
F 2 "" H 3400 5600 50  0001 C CNN
F 3 "~" H 3400 5600 50  0001 C CNN
	1    3400 5400
	1    0    0    -1  
$EndComp
$Comp
L Device:LED D?
U 1 1 5FC4F273
P 4250 5450
F 0 "D?" H 4243 5666 50  0000 C CNN
F 1 "LED" H 4243 5575 50  0000 C CNN
F 2 "" H 4250 5450 50  0001 C CNN
F 3 "~" H 4250 5450 50  0001 C CNN
	1    4250 5450
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW?
U 1 1 5FC4F8D8
P 1900 5350
F 0 "SW?" H 1900 5635 50  0000 C CNN
F 1 "SW_Push" H 1900 5544 50  0000 C CNN
F 2 "" H 1900 5550 50  0001 C CNN
F 3 "~" H 1900 5550 50  0001 C CNN
	1    1900 5350
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic_MountingPin:Conn_01x04_MountingPin J?
U 1 1 5FC5333F
P 2050 3600
F 0 "J?" H 1972 3917 50  0000 C CNN
F 1 "Conn_01x04_MountingPin" H 1972 3826 50  0000 C CNN
F 2 "" H 2050 3600 50  0001 C CNN
F 3 "~" H 2050 3600 50  0001 C CNN
	1    2050 3600
	-1   0    0    -1  
$EndComp
Wire Wire Line
	2250 3500 2250 2200
Wire Wire Line
	2250 2200 3450 2200
Wire Wire Line
	3450 2200 5100 2200
Wire Wire Line
	5100 2200 5100 2700
Connection ~ 3450 2200
$EndSCHEMATC

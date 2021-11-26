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
L power:GND #PWR02
U 1 1 600369C3
P 5750 5600
F 0 "#PWR02" H 5750 5350 50  0001 C CNN
F 1 "GND" H 5755 5427 50  0000 C CNN
F 2 "" H 5750 5600 50  0001 C CNN
F 3 "" H 5750 5600 50  0001 C CNN
	1    5750 5600
	1    0    0    -1  
$EndComp
Wire Wire Line
	5750 1985 5750 1255
$Comp
L suf:TJCTM24028-SPI U4
U 1 1 6004A8BF
P 9225 3315
F 0 "U4" H 9212 4252 60  0000 C CNN
F 1 "TJCTM24028-SPI" H 9212 4146 60  0000 C CNN
F 2 "iotCronotempLcd:tjctm24028-spi" H 9225 3315 60  0001 C CNN
F 3 "" H 9225 3315 60  0000 C CNN
	1    9225 3315
	1    0    0    -1  
$EndComp
$Comp
L conn:CONN_4 P2
U 1 1 6004DECD
P 7625 905
F 0 "P2" V 7538 667 50  0000 R CNN
F 1 "CONN_4" V 7447 667 50  0000 R CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x04_P2.54mm_Vertical" H 7625 905 50  0001 C CNN
F 3 "" H 7625 905 50  0001 C CNN
	1    7625 905 
	0    -1   -1   0   
$EndComp
Connection ~ 5750 1255
Wire Wire Line
	8075 2665 7955 2665
Wire Wire Line
	7775 1255 7775 2765
Wire Wire Line
	7775 2765 8020 2765
NoConn ~ 8075 3465
NoConn ~ 10350 3165
NoConn ~ 10350 3265
NoConn ~ 10350 3365
NoConn ~ 10350 3465
$Comp
L device:LED D2
U 1 1 60073FA9
P 7275 5197
F 0 "D2" V 7229 5275 50  0000 L CNN
F 1 "LED" V 7320 5275 50  0000 L CNN
F 2 "LED_THT:LED_D5.0mm" H 7275 5197 50  0001 C CNN
F 3 "" H 7275 5197 50  0001 C CNN
	1    7275 5197
	0    1    1    0   
$EndComp
Wire Wire Line
	5750 4785 5750 5540
Connection ~ 5750 5540
$Comp
L device:R R3
U 1 1 60080C74
P 3405 1665
F 0 "R3" H 3474 1711 50  0000 L CNN
F 1 "R" H 3474 1620 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" H 3405 1665 50  0001 C CNN
F 3 "" H 3405 1665 50  0001 C CNN
	1    3405 1665
	1    0    0    -1  
$EndComp
Wire Wire Line
	3405 1420 3405 1415
Connection ~ 3405 1415
Wire Wire Line
	3405 1415 3405 1255
$Comp
L device:SW_PUSH SW1
U 1 1 60082F41
P 3060 1915
F 0 "SW1" H 3060 2170 50  0000 C CNN
F 1 "SW_PUSH" H 3060 2079 50  0000 C CNN
F 2 "Button_Switch_THT:SW_PUSH_6mm_H4.3mm" H 3060 1915 50  0001 C CNN
F 3 "" H 3060 1915 50  0001 C CNN
	1    3060 1915
	1    0    0    -1  
$EndComp
Wire Wire Line
	3360 1915 3405 1915
Wire Wire Line
	2760 1915 1115 1915
Wire Wire Line
	1115 1915 1115 5540
Wire Wire Line
	5750 5540 5750 5600
$Comp
L device:R R4
U 1 1 6007368C
P 7275 4747
F 0 "R4" V 7069 4747 50  0000 C CNN
F 1 "1K" V 7160 4747 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" H 7275 4747 50  0001 C CNN
F 3 "" H 7275 4747 50  0001 C CNN
	1    7275 4747
	1    0    0    -1  
$EndComp
Wire Wire Line
	5750 5540 7275 5540
Wire Wire Line
	7275 5540 7275 5397
$Comp
L Transistor_FET:2N7002 Q1
U 1 1 600A5061
P 2505 3990
F 0 "Q1" H 2709 4036 50  0000 L CNN
F 1 "2N7002" H 2709 3945 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 2705 3915 50  0001 L CIN
F 3 "https://www.fairchildsemi.com/datasheets/2N/2N7002.pdf" H 2505 3990 50  0001 L CNN
	1    2505 3990
	1    0    0    -1  
$EndComp
$Comp
L device:R R1
U 1 1 600A8891
P 1810 3990
F 0 "R1" V 1604 3990 50  0000 C CNN
F 1 "R" V 1695 3990 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" H 1810 3990 50  0001 C CNN
F 3 "" H 1810 3990 50  0001 C CNN
	1    1810 3990
	0    1    1    0   
$EndComp
$Comp
L device:R R2
U 1 1 600A90B2
P 2185 4425
F 0 "R2" H 2254 4471 50  0000 L CNN
F 1 "R" H 2254 4380 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" H 2185 4425 50  0001 C CNN
F 3 "" H 2185 4425 50  0001 C CNN
	1    2185 4425
	1    0    0    -1  
$EndComp
Wire Wire Line
	2060 3990 2185 3990
Wire Wire Line
	2185 3990 2305 3990
Connection ~ 2185 3990
Wire Wire Line
	2185 4180 2185 4175
Wire Wire Line
	2185 4175 2185 3990
Connection ~ 2185 4175
Wire Wire Line
	1115 5540 2185 5540
Wire Wire Line
	2185 4675 2185 4725
Connection ~ 2185 5540
Wire Wire Line
	2605 4190 2605 4725
Wire Wire Line
	2605 4725 2185 4725
Connection ~ 2185 4725
Wire Wire Line
	2185 4725 2185 5540
Wire Wire Line
	2605 3140 3100 3140
$Comp
L conn:CONN_2 P1
U 1 1 600E905E
P 4380 3115
F 0 "P1" H 4508 3143 40  0000 L CNN
F 1 "CONN_2" H 4508 3067 40  0000 L CNN
F 2 "TerminalBlock_Phoenix:TerminalBlock_Phoenix_MKDS-1,5-2_1x02_P5.00mm_Horizontal" H 4380 3115 50  0001 C CNN
F 3 "" H 4380 3115 50  0001 C CNN
	1    4380 3115
	1    0    0    -1  
$EndComp
NoConn ~ 5150 3385
NoConn ~ 5150 3485
NoConn ~ 5150 3585
NoConn ~ 5150 3685
NoConn ~ 5150 3785
NoConn ~ 5150 3885
NoConn ~ 6350 3585
Connection ~ 3405 1915
Wire Wire Line
	8020 2765 8020 2145
Wire Wire Line
	8020 2145 10905 2145
Wire Wire Line
	10905 2145 10905 5540
Wire Wire Line
	10905 5540 8905 5540
Connection ~ 8020 2765
Wire Wire Line
	8020 2765 8075 2765
Connection ~ 7275 5540
$Comp
L Diode:1N4148 D1
U 1 1 5FE107A5
P 2605 3510
F 0 "D1" V 2559 3589 50  0000 L CNN
F 1 "1N4148" V 2650 3589 50  0000 L CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 2605 3335 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/1N4148_1N4448.pdf" H 2605 3510 50  0001 C CNN
	1    2605 3510
	0    1    1    0   
$EndComp
$Comp
L Sensor:DHT11 U3
U 1 1 5FE4FF39
P 8905 4825
F 0 "U3" H 8661 4871 50  0000 R CNN
F 1 "DHT11" H 8661 4780 50  0000 R CNN
F 2 "Sensor:Aosong_DHT11_5.5x12.0_P2.54mm" H 8905 4425 50  0001 C CNN
F 3 "http://akizukidenshi.com/download/ds/aosong/DHT11.pdf" H 9055 5075 50  0001 C CNN
	1    8905 4825
	-1   0    0    -1  
$EndComp
Wire Wire Line
	8905 4525 8905 4340
Wire Wire Line
	8905 4340 7955 4340
Wire Wire Line
	7955 4340 7955 2665
Wire Wire Line
	8905 5125 8905 5540
Connection ~ 8905 5540
Wire Wire Line
	8905 5540 7275 5540
Wire Wire Line
	6350 3185 6965 3185
Wire Wire Line
	6965 3185 6965 4825
Wire Wire Line
	6965 4825 8605 4825
Wire Wire Line
	6350 2185 6536 2185
Wire Wire Line
	6536 2185 6536 1915
NoConn ~ 5150 2385
NoConn ~ 5150 2485
NoConn ~ 5150 2185
Wire Wire Line
	2605 3740 2605 3790
Wire Wire Line
	2605 3660 2605 3740
Connection ~ 2605 3740
Wire Wire Line
	3100 3740 2605 3740
Wire Wire Line
	3600 3140 3600 3015
Wire Wire Line
	3600 3015 4030 3015
NoConn ~ 3400 3140
$Comp
L Relay:G6E K1
U 1 1 600548B9
P 3300 3440
F 0 "K1" H 3730 3486 50  0000 L CNN
F 1 "G6E" H 3730 3395 50  0000 L CNN
F 2 "Relay_THT:Relay_SPDT_Omron_G6E" H 4430 3410 50  0001 C CNN
F 3 "https://www.omron.com/ecb/products/pdf/en-g6e.pdf" H 3300 3440 50  0001 C CNN
	1    3300 3440
	1    0    0    -1  
$EndComp
Wire Wire Line
	3500 3740 3500 3843
Wire Wire Line
	3500 3843 3842 3843
Wire Wire Line
	3842 3843 3842 3215
Wire Wire Line
	3842 3215 4030 3215
Wire Wire Line
	7475 1255 5750 1255
Wire Wire Line
	5750 1255 5750 1256
Wire Wire Line
	7955 2665 7475 2665
Wire Wire Line
	7475 2665 7475 1255
Connection ~ 7955 2665
Connection ~ 7475 1255
$Comp
L Regulator_Linear:AMS1117-3.3 U1
U 1 1 60012C9B
P 3888 4637
F 0 "U1" H 3888 4879 50  0000 C CNN
F 1 "AMS1117-3.3" H 3888 4788 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-223-3_TabPin2" H 3888 4837 50  0001 C CNN
F 3 "http://www.advanced-monolithic.com/pdf/ds1117.pdf" H 3988 4387 50  0001 C CNN
	1    3888 4637
	1    0    0    -1  
$EndComp
Wire Wire Line
	3888 4937 3887 5540
Connection ~ 3887 5540
Wire Wire Line
	3887 5540 4519 5540
Wire Wire Line
	3405 1915 6536 1915
Wire Wire Line
	3405 1255 4921 1255
Wire Wire Line
	4188 4637 4519 4637
Wire Wire Line
	4921 4637 4921 1255
Connection ~ 4921 1255
Wire Wire Line
	4921 1255 5750 1255
$Comp
L device:C C2
U 1 1 600E380F
P 4519 5024
F 0 "C2" H 4649 5070 50  0000 L CNN
F 1 "C" H 4649 4979 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 4519 5024 50  0001 C CNN
F 3 "" H 4519 5024 50  0001 C CNN
	1    4519 5024
	1    0    0    -1  
$EndComp
Wire Wire Line
	3588 4637 3436 4637
Wire Wire Line
	3436 4637 3436 4839
$Comp
L device:C C1
U 1 1 600E1EAA
P 3436 5039
F 0 "C1" H 3566 5085 50  0000 L CNN
F 1 "C" H 3566 4994 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 3436 5039 50  0001 C CNN
F 3 "" H 3436 5039 50  0001 C CNN
	1    3436 5039
	1    0    0    -1  
$EndComp
Wire Wire Line
	3436 5238 3436 5239
Connection ~ 3436 5540
Wire Wire Line
	3436 5540 3887 5540
Connection ~ 3436 5239
Wire Wire Line
	3436 5239 3436 5540
Wire Wire Line
	4519 4824 4519 4637
Connection ~ 4519 4637
Wire Wire Line
	4519 4637 4921 4637
Wire Wire Line
	4519 5223 4519 5224
Connection ~ 4519 5540
Wire Wire Line
	4519 5540 5750 5540
Connection ~ 4519 5224
Wire Wire Line
	4519 5224 4519 5540
Connection ~ 3436 4637
$Comp
L power:GND #PWR01
U 1 1 601B60BC
P 2914 5609
F 0 "#PWR01" H 2914 5359 50  0001 C CNN
F 1 "GND" H 2919 5436 50  0000 C CNN
F 2 "" H 2914 5609 50  0001 C CNN
F 3 "" H 2914 5609 50  0001 C CNN
	1    2914 5609
	1    0    0    -1  
$EndComp
Wire Wire Line
	3405 1255 2605 1255
Wire Wire Line
	2605 1255 2605 3140
Connection ~ 3405 1255
Connection ~ 2605 3140
Wire Wire Line
	2605 3140 2605 3360
Wire Wire Line
	6350 2785 7738 2785
Wire Wire Line
	7738 2785 7738 2865
Wire Wire Line
	7738 2865 8075 2865
Wire Wire Line
	8075 2965 6508 2965
Wire Wire Line
	6508 2965 6508 2985
Wire Wire Line
	6508 2985 6350 2985
Wire Wire Line
	8075 3065 7606 3065
Wire Wire Line
	7606 3065 7606 4085
Wire Wire Line
	7606 4085 6350 4085
Wire Wire Line
	8075 3165 7493 3165
Wire Wire Line
	7493 3165 7493 3985
Wire Wire Line
	7493 3985 6350 3985
Wire Wire Line
	6350 3885 7651 3885
Wire Wire Line
	7651 3885 7651 3265
Wire Wire Line
	7651 3265 8075 3265
Wire Wire Line
	8075 3365 7747 3365
Wire Wire Line
	7747 3365 7747 4285
Wire Wire Line
	7747 4285 6350 4285
Wire Wire Line
	8075 3565 7815 3565
Wire Wire Line
	7815 3565 7815 4185
Wire Wire Line
	7815 4185 6350 4185
Wire Wire Line
	8075 3665 7406 3665
Wire Wire Line
	7406 3665 7406 3385
Wire Wire Line
	7406 3385 6350 3385
Wire Wire Line
	8075 3765 7325 3765
Wire Wire Line
	7325 3765 7325 3485
Wire Wire Line
	7325 3485 6350 3485
Wire Wire Line
	8075 3865 7262 3865
Wire Wire Line
	7262 3865 7262 3685
Wire Wire Line
	7262 3685 6350 3685
Wire Wire Line
	8075 3965 7184 3965
Wire Wire Line
	7184 3965 7184 3785
Wire Wire Line
	7184 3785 6350 3785
Wire Wire Line
	7275 4497 7275 2585
Wire Wire Line
	7275 2585 6350 2585
NoConn ~ 6350 2385
NoConn ~ 6350 4385
NoConn ~ 6350 4485
NoConn ~ 6350 2685
$Comp
L RF_Module:ESP32-WROOM-32 U2
U 1 1 60033917
P 5750 3385
F 0 "U2" H 5750 4966 50  0000 C CNN
F 1 "ESP32-WROOM-32" H 5750 4875 50  0000 C CNN
F 2 "RF_Module:ESP32-WROOM-32" H 5750 1885 50  0001 C CNN
F 3 "https://www.espressif.com/sites/default/files/documentation/esp32-wroom-32d_esp32-wroom-32u_datasheet_en.pdf" H 5450 3435 50  0001 C CNN
	1    5750 3385
	1    0    0    -1  
$EndComp
Wire Wire Line
	6349 3285 6350 3285
NoConn ~ 6350 2885
Wire Wire Line
	1405 3990 1560 3990
Wire Wire Line
	1405 6001 1405 3990
Wire Wire Line
	6648 6001 1405 6001
Wire Wire Line
	6648 3285 6648 6001
Connection ~ 6350 3285
Wire Wire Line
	6350 3285 6648 3285
NoConn ~ 6350 3085
Wire Wire Line
	2185 5540 2915 5540
Wire Wire Line
	3116 5483 3114 5540
Connection ~ 3114 5540
Wire Wire Line
	3114 5540 3436 5540
Wire Wire Line
	2915 5169 2915 5540
Connection ~ 2915 5540
Wire Wire Line
	2915 5540 3114 5540
Wire Wire Line
	2914 5540 2915 5540
Wire Wire Line
	2913 4637 2913 4969
$Comp
L Device:Battery_Cell BT1
U 1 1 6019FF55
P 3116 5383
F 0 "BT1" H 3234 5479 50  0000 L CNN
F 1 "Battery_Cell" H 2595 5369 50  0000 L CNN
F 2 "Connector_Wire:SolderWirePad_1x02_P5.08mm_Drill1.5mm" V 3116 5443 50  0001 C CNN
F 3 "~" V 3116 5443 50  0001 C CNN
	1    3116 5383
	1    0    0    -1  
$EndComp
$Comp
L Connector:Barrel_Jack_Switch J1
U 1 1 600C634C
P 2567 5069
F 0 "J1" H 2337 5019 50  0000 R CNN
F 1 "Barrel_Jack_Switch" H 2337 5110 50  0000 R CNN
F 2 "Connector_BarrelJack:BarrelJack_Wuerth_6941xx301002" H 2617 5029 50  0001 C CNN
F 3 "~" H 2617 5029 50  0001 C CNN
	1    2567 5069
	1    0    0    1   
$EndComp
Wire Wire Line
	2867 4969 2913 4969
Wire Wire Line
	2867 5169 2915 5169
Wire Wire Line
	2913 4637 3436 4637
Wire Wire Line
	2867 5069 3116 5069
Wire Wire Line
	3116 5069 3116 5183
Wire Wire Line
	2914 5540 2914 5609
Wire Wire Line
	7575 1255 7575 2285
Wire Wire Line
	7575 2285 6350 2285
Wire Wire Line
	6350 2485 7675 2485
Wire Wire Line
	7675 2485 7675 1255
$EndSCHEMATC

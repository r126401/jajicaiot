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
L tjctm24028-spi:TJCTM24028-SPI U3
U 1 1 6050E6D3
P 7377 1470
F 0 "U3" H 7364 2407 60  0000 C CNN
F 1 "TJCTM24028-SPI" H 7364 2301 60  0000 C CNN
F 2 "tjctm24028-spi:tjctm24028-spi" H 7377 1470 60  0001 C CNN
F 3 "" H 7377 1470 60  0000 C CNN
	1    7377 1470
	1    0    0    -1  
$EndComp
NoConn ~ 6227 1620
Wire Wire Line
	3194 1094 3194 1209
Connection ~ 3194 1094
Wire Wire Line
	3194 894  3194 1094
Wire Wire Line
	3047 894  3194 894 
Wire Wire Line
	3047 1094 3194 1094
$Comp
L Connector_Generic:Conn_02x05_Top_Bottom J1
U 1 1 60512C40
P 2747 994
F 0 "J1" H 2797 1411 50  0000 C CNN
F 1 "Conn_02x05_JTAG" H 2797 1320 50  0000 C CNN
F 2 "Connector_IDC:IDC-Header_2x05_P2.54mm_Vertical" H 2747 994 50  0001 C CNN
F 3 "~" H 2747 994 50  0001 C CNN
	1    2747 994 
	1    0    0    -1  
$EndComp
NoConn ~ 3047 1194
$Comp
L power:GND #PWR02
U 1 1 605284D4
P 3194 1209
F 0 "#PWR02" H 3194 959 50  0001 C CNN
F 1 "GND" H 3199 1036 50  0000 C CNN
F 2 "" H 3194 1209 50  0001 C CNN
F 3 "" H 3194 1209 50  0001 C CNN
	1    3194 1209
	1    0    0    -1  
$EndComp
Wire Wire Line
	2444 1194 2444 994 
Connection ~ 2444 1194
Wire Wire Line
	2547 1194 2444 1194
Wire Wire Line
	2444 994  2547 994 
Wire Wire Line
	2444 1482 2444 1194
$Comp
L power:GND #PWR01
U 1 1 6050D18D
P 2444 1482
F 0 "#PWR01" H 2444 1232 50  0001 C CNN
F 1 "GND" H 2449 1309 50  0000 C CNN
F 2 "" H 2444 1482 50  0001 C CNN
F 3 "" H 2444 1482 50  0001 C CNN
	1    2444 1482
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR04
U 1 1 604F9815
P 4559 907
F 0 "#PWR04" H 4559 657 50  0001 C CNN
F 1 "GND" H 4564 734 50  0000 C CNN
F 2 "" H 4559 907 50  0001 C CNN
F 3 "" H 4559 907 50  0001 C CNN
	1    4559 907 
	0    -1   -1   0   
$EndComp
$Comp
L Connector_Generic:Conn_02x03_Top_Bottom J2
U 1 1 604DC04C
P 4259 1007
F 0 "J2" H 4309 1324 50  0000 C CNN
F 1 "Conn_02x03_Top_Bottom" H 4309 1233 50  0000 C CNN
F 2 "Connector_IDC:IDC-Header_2x03_P2.54mm_Vertical" H 4259 1007 50  0001 C CNN
F 3 "~" H 4259 1007 50  0001 C CNN
	1    4259 1007
	1    0    0    -1  
$EndComp
Wire Wire Line
	2547 1094 2397 1094
Text Label 2397 1094 0    50   ~ 0
IO13
Text Label 3047 994  0    50   ~ 0
IO12
Wire Wire Line
	2547 894  2404 894 
Text Label 2404 894  0    50   ~ 0
IO14
Text Label 3047 794  0    50   ~ 0
IO15
Wire Wire Line
	2547 794  2405 794 
Text Label 2405 794  0    50   ~ 0
3.3V
Wire Wire Line
	6227 820  6107 820 
Text Label 6107 820  0    50   ~ 0
3.3V
$Comp
L power:GND #PWR0104
U 1 1 605FA0BF
P 6227 920
F 0 "#PWR0104" H 6227 670 50  0001 C CNN
F 1 "GND" V 6232 792 50  0000 R CNN
F 2 "" H 6227 920 50  0001 C CNN
F 3 "" H 6227 920 50  0001 C CNN
	1    6227 920 
	0    1    1    0   
$EndComp
Wire Wire Line
	6227 1020 6117 1020
Wire Wire Line
	6227 1120 6114 1120
Wire Wire Line
	6227 1220 6114 1220
Wire Wire Line
	6227 1320 6111 1320
Wire Wire Line
	6227 1420 6114 1420
Wire Wire Line
	6227 1520 6114 1520
Wire Wire Line
	6227 1720 6119 1720
Wire Wire Line
	6227 1820 6118 1820
Wire Wire Line
	6227 1920 6117 1920
Wire Wire Line
	6227 2020 6118 2020
Wire Wire Line
	6227 2120 6117 2120
Text Label 6117 1020 0    50   ~ 0
IO5
Text Label 6114 1120 0    50   ~ 0
IO21
Text Label 6114 1220 0    50   ~ 0
IO27
Text Label 6111 1320 0    50   ~ 0
IO26
Text Label 6114 1420 0    50   ~ 0
IO25
Text Label 6114 1520 0    50   ~ 0
IO33
Text Label 6119 1720 0    50   ~ 0
IO32
Text Label 6118 1820 0    50   ~ 0
IO18
Text Label 6117 2120 0    50   ~ 0
IO23
Text Label 6117 1920 0    50   ~ 0
IO19
Text Label 6118 2020 0    50   ~ 0
IO22
Wire Wire Line
	4059 907  3937 907 
Wire Wire Line
	4059 1007 3936 1007
Wire Wire Line
	4059 1107 3935 1107
Text Label 3937 907  0    50   ~ 0
EN
Text Label 3936 1007 0    50   ~ 0
3.3V
Text Label 3935 1107 0    50   ~ 0
IO3
Text Label 4559 1007 0    50   ~ 0
IO1
Text Label 4559 1107 0    50   ~ 0
IO0
$Comp
L Connector_Generic:Conn_02x04_Counter_Clockwise J6
U 1 1 605E5EA9
P 1145 1003
F 0 "J6" H 1195 1320 50  0000 C CNN
F 1 "Conn_02x04_Counter_Clockwise_lcd" H 1195 1229 50  0000 C CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_2x04_P2.54mm_Vertical" H 1145 1003 50  0001 C CNN
F 3 "~" H 1145 1003 50  0001 C CNN
	1    1145 1003
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR05
U 1 1 605E7B0D
P 945 903
F 0 "#PWR05" H 945 653 50  0001 C CNN
F 1 "GND" V 950 775 50  0000 R CNN
F 2 "" H 945 903 50  0001 C CNN
F 3 "" H 945 903 50  0001 C CNN
	1    945  903 
	0    1    1    0   
$EndComp
Wire Wire Line
	945  1003 742  1003
Wire Wire Line
	945  1103 742  1103
Wire Wire Line
	945  1203 742  1203
Text Label 742  1203 0    50   ~ 0
3.3V
Text Label 1445 1203 0    50   ~ 0
SCL
Text Label 1445 1003 0    50   ~ 0
SDA
Text Label 742  1103 0    50   ~ 0
3.3V
$Comp
L power:GND #PWR06
U 1 1 6060875B
P 742 1003
F 0 "#PWR06" H 742 753 50  0001 C CNN
F 1 "GND" V 747 875 50  0000 R CNN
F 2 "" H 742 1003 50  0001 C CNN
F 3 "" H 742 1003 50  0001 C CNN
	1    742  1003
	0    1    1    0   
$EndComp
Text Label 1445 1103 0    50   ~ 0
SCL
Text Label 1445 903  0    50   ~ 0
SDA
$Comp
L Device:R R1
U 1 1 6061D4CD
P 4583 7065
F 0 "R1" V 4376 7065 50  0000 C CNN
F 1 "R" V 4467 7065 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P15.24mm_Horizontal" V 4513 7065 50  0001 C CNN
F 3 "~" H 4583 7065 50  0001 C CNN
	1    4583 7065
	0    1    1    0   
$EndComp
$Comp
L Device:R R2
U 1 1 6061DB6D
P 4773 7272
F 0 "R2" H 4703 7226 50  0000 R CNN
F 1 "R" H 4703 7317 50  0000 R CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P15.24mm_Horizontal" V 4703 7272 50  0001 C CNN
F 3 "~" H 4773 7272 50  0001 C CNN
	1    4773 7272
	-1   0    0    1   
$EndComp
$Comp
L Transistor_BJT:BC547 Q1
U 1 1 60622F24
P 5017 7065
F 0 "Q1" H 5208 7111 50  0000 L CNN
F 1 "BC547" H 5208 7020 50  0000 L CNN
F 2 "Package_TO_SOT_THT:TO-92_Inline" H 5217 6990 50  0001 L CIN
F 3 "http://www.fairchildsemi.com/ds/BC/BC547.pdf" H 5017 7065 50  0001 L CNN
	1    5017 7065
	1    0    0    -1  
$EndComp
Wire Wire Line
	4733 7065 4773 7065
Wire Wire Line
	4773 7122 4773 7065
Connection ~ 4773 7065
Wire Wire Line
	4773 7065 4817 7065
$Comp
L power:GND #PWR07
U 1 1 6063E321
P 4773 7422
F 0 "#PWR07" H 4773 7172 50  0001 C CNN
F 1 "GND" H 4778 7249 50  0000 C CNN
F 2 "" H 4773 7422 50  0001 C CNN
F 3 "" H 4773 7422 50  0001 C CNN
	1    4773 7422
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR08
U 1 1 6063ECF7
P 5117 7265
F 0 "#PWR08" H 5117 7015 50  0001 C CNN
F 1 "GND" H 5122 7092 50  0000 C CNN
F 2 "" H 5117 7265 50  0001 C CNN
F 3 "" H 5117 7265 50  0001 C CNN
	1    5117 7265
	1    0    0    -1  
$EndComp
$Comp
L Diode:1N4148 D1
U 1 1 6063FD63
P 5117 6583
F 0 "D1" V 5071 6662 50  0000 L CNN
F 1 "1N4148" V 5162 6662 50  0000 L CNN
F 2 "Diode_THT:D_DO-34_SOD68_P7.62mm_Horizontal" H 5117 6408 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/1N4148_1N4448.pdf" H 5117 6583 50  0001 C CNN
	1    5117 6583
	0    1    1    0   
$EndComp
Wire Wire Line
	5117 6865 5117 6817
$Comp
L Relay:SANYOU_SRD_Form_A K1
U 1 1 6066A0E8
P 5647 6504
F 0 "K1" H 5977 6550 50  0000 L CNN
F 1 "SANYOU_SRD_Form_A" H 5977 6459 50  0000 L CNN
F 2 "Relay_THT:Relay_SPST_SANYOU_SRD_Series_Form_A" H 5997 6454 50  0001 L CNN
F 3 "http://www.sanyourelay.ca/public/products/pdf/SRD.pdf" H 5647 6504 50  0001 C CNN
	1    5647 6504
	1    0    0    -1  
$EndComp
Wire Wire Line
	5447 6804 5447 6817
Wire Wire Line
	5447 6817 5117 6817
Connection ~ 5117 6817
Wire Wire Line
	5117 6817 5117 6733
Wire Wire Line
	5117 6433 5117 6153
Wire Wire Line
	5117 6153 5447 6153
Wire Wire Line
	5447 6153 5447 6204
$Comp
L Device:R R3
U 1 1 6069E7AE
P 8868 5204
F 0 "R3" H 8798 5158 50  0000 R CNN
F 1 "R" H 8798 5249 50  0000 R CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P15.24mm_Horizontal" V 8798 5204 50  0001 C CNN
F 3 "~" H 8868 5204 50  0001 C CNN
	1    8868 5204
	-1   0    0    1   
$EndComp
$Comp
L Device:LED D2
U 1 1 606A0742
P 8868 5577
F 0 "D2" V 8907 5460 50  0000 R CNN
F 1 "LED" V 8816 5460 50  0000 R CNN
F 2 "LED_THT:LED_D5.0mm" H 8868 5577 50  0001 C CNN
F 3 "~" H 8868 5577 50  0001 C CNN
	1    8868 5577
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR011
U 1 1 606ADD76
P 8868 5727
F 0 "#PWR011" H 8868 5477 50  0001 C CNN
F 1 "GND" H 8873 5554 50  0000 C CNN
F 2 "" H 8868 5727 50  0001 C CNN
F 3 "" H 8868 5727 50  0001 C CNN
	1    8868 5727
	1    0    0    -1  
$EndComp
Wire Wire Line
	8868 5354 8868 5427
$Comp
L Sensor_Temperature:DS18B20 U4
U 1 1 606C15FE
P 7366 4555
F 0 "U4" H 7136 4601 50  0000 R CNN
F 1 "DS18B20" H 7136 4510 50  0000 R CNN
F 2 "Package_TO_SOT_THT:TO-92_Inline" H 6366 4305 50  0001 C CNN
F 3 "http://datasheets.maximintegrated.com/en/ds/DS18B20.pdf" H 7216 4805 50  0001 C CNN
	1    7366 4555
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR09
U 1 1 606C2E9A
P 7366 4855
F 0 "#PWR09" H 7366 4605 50  0001 C CNN
F 1 "GND" H 7371 4682 50  0000 C CNN
F 2 "" H 7366 4855 50  0001 C CNN
F 3 "" H 7366 4855 50  0001 C CNN
	1    7366 4855
	1    0    0    -1  
$EndComp
Text Label 7366 4255 0    50   ~ 0
3.3V
$Comp
L Sensor:DHT11 U5
U 1 1 606C7BE9
P 8437 3808
F 0 "U5" H 8193 3854 50  0000 R CNN
F 1 "DHT11" H 8193 3763 50  0000 R CNN
F 2 "Sensor:Aosong_DHT11_5.5x12.0_P2.54mm" H 8437 3408 50  0001 C CNN
F 3 "http://akizukidenshi.com/download/ds/aosong/DHT11.pdf" H 8587 4058 50  0001 C CNN
	1    8437 3808
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR010
U 1 1 606CACBB
P 8437 4108
F 0 "#PWR010" H 8437 3858 50  0001 C CNN
F 1 "GND" H 8442 3935 50  0000 C CNN
F 2 "" H 8437 4108 50  0001 C CNN
F 3 "" H 8437 4108 50  0001 C CNN
	1    8437 4108
	1    0    0    -1  
$EndComp
Wire Wire Line
	4433 7065 4310 7065
Text Label 5847 6204 0    50   ~ 0
E_RELE_A
Text Label 5847 6804 0    50   ~ 0
E_RELE_B
Text Label 8437 3508 0    50   ~ 0
3.3V
$Comp
L Switch:SW_Push SW1
U 1 1 6076CF72
P 10400 3576
F 0 "SW1" H 10400 3861 50  0000 C CNN
F 1 "SW_Push" H 10400 3770 50  0000 C CNN
F 2 "Button_Switch_THT:SW_PUSH_6mm_H4.3mm" H 10400 3776 50  0001 C CNN
F 3 "~" H 10400 3776 50  0001 C CNN
	1    10400 3576
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR012
U 1 1 607709B1
P 10600 3576
F 0 "#PWR012" H 10600 3326 50  0001 C CNN
F 1 "GND" H 10605 3403 50  0000 C CNN
F 2 "" H 10600 3576 50  0001 C CNN
F 3 "" H 10600 3576 50  0001 C CNN
	1    10600 3576
	1    0    0    -1  
$EndComp
$Comp
L Device:R R4
U 1 1 607789B5
P 9965 3415
F 0 "R4" H 10035 3461 50  0000 L CNN
F 1 "R" H 10035 3370 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P15.24mm_Horizontal" V 9895 3415 50  0001 C CNN
F 3 "~" H 9965 3415 50  0001 C CNN
	1    9965 3415
	1    0    0    -1  
$EndComp
Wire Wire Line
	10200 3576 9965 3576
Wire Wire Line
	9965 3565 9965 3576
Text Label 9965 3265 0    50   ~ 0
3.3V
Text Label 10103 3576 0    50   ~ 0
IO0
$Comp
L Connector_Generic:Conn_02x12_Counter_Clockwise J5
U 1 1 6084F2A9
P 10008 4986
F 0 "J5" H 10058 5703 50  0000 C CNN
F 1 "Conn_02x12_Counter_Clockwise" H 10058 5612 50  0000 C CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_2x12_P2.54mm_Vertical" H 10008 4986 50  0001 C CNN
F 3 "~" H 10008 4986 50  0001 C CNN
	1    10008 4986
	1    0    0    -1  
$EndComp
Text Label 9631 4486 0    50   ~ 0
3.3V
Wire Wire Line
	9808 4486 9631 4486
Wire Wire Line
	9808 4586 9633 4586
Wire Wire Line
	9808 4686 9633 4686
Wire Wire Line
	9808 4786 9630 4786
Wire Wire Line
	9808 4886 9627 4886
Wire Wire Line
	9808 4986 9630 4986
Wire Wire Line
	9808 5086 9631 5086
Wire Wire Line
	9808 5186 9630 5186
Wire Wire Line
	9808 5286 9630 5286
Wire Wire Line
	9808 5386 9630 5386
Wire Wire Line
	9808 5486 9631 5486
Wire Wire Line
	9808 5586 9629 5586
Text Label 9633 4686 0    50   ~ 0
3.3V
Text Label 9627 4886 0    50   ~ 0
3.3V
Text Label 9631 5086 0    50   ~ 0
3.3V
Text Label 9630 5286 0    50   ~ 0
3.3V
Text Label 9631 5486 0    50   ~ 0
3.3V
Text Label 10308 5586 0    50   ~ 0
VIN
Text Label 10308 5386 0    50   ~ 0
VIN
Text Label 10308 5186 0    50   ~ 0
VIN
Text Label 10308 4986 0    50   ~ 0
VIN
Text Label 10308 4786 0    50   ~ 0
VIN
Text Label 10308 4586 0    50   ~ 0
VIN
$Comp
L power:GND #PWR0106
U 1 1 60C71A49
P 9633 4586
F 0 "#PWR0106" H 9633 4336 50  0001 C CNN
F 1 "GND" H 9638 4413 50  0000 C CNN
F 2 "" H 9633 4586 50  0001 C CNN
F 3 "" H 9633 4586 50  0001 C CNN
	1    9633 4586
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR0107
U 1 1 60C7427B
P 9630 4786
F 0 "#PWR0107" H 9630 4536 50  0001 C CNN
F 1 "GND" H 9635 4613 50  0000 C CNN
F 2 "" H 9630 4786 50  0001 C CNN
F 3 "" H 9630 4786 50  0001 C CNN
	1    9630 4786
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR0108
U 1 1 60C74F0B
P 9630 4986
F 0 "#PWR0108" H 9630 4736 50  0001 C CNN
F 1 "GND" H 9635 4813 50  0000 C CNN
F 2 "" H 9630 4986 50  0001 C CNN
F 3 "" H 9630 4986 50  0001 C CNN
	1    9630 4986
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR0109
U 1 1 60C75F0B
P 9630 5186
F 0 "#PWR0109" H 9630 4936 50  0001 C CNN
F 1 "GND" H 9635 5013 50  0000 C CNN
F 2 "" H 9630 5186 50  0001 C CNN
F 3 "" H 9630 5186 50  0001 C CNN
	1    9630 5186
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR0110
U 1 1 60C77E5B
P 9630 5386
F 0 "#PWR0110" H 9630 5136 50  0001 C CNN
F 1 "GND" H 9635 5213 50  0000 C CNN
F 2 "" H 9630 5386 50  0001 C CNN
F 3 "" H 9630 5386 50  0001 C CNN
	1    9630 5386
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR0111
U 1 1 60C79200
P 9629 5586
F 0 "#PWR0111" H 9629 5336 50  0001 C CNN
F 1 "GND" H 9634 5413 50  0000 C CNN
F 2 "" H 9629 5586 50  0001 C CNN
F 3 "" H 9629 5586 50  0001 C CNN
	1    9629 5586
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR0112
U 1 1 60C79C95
P 10308 5486
F 0 "#PWR0112" H 10308 5236 50  0001 C CNN
F 1 "GND" H 10313 5313 50  0000 C CNN
F 2 "" H 10308 5486 50  0001 C CNN
F 3 "" H 10308 5486 50  0001 C CNN
	1    10308 5486
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR0113
U 1 1 60C7B70B
P 10308 5286
F 0 "#PWR0113" H 10308 5036 50  0001 C CNN
F 1 "GND" H 10313 5113 50  0000 C CNN
F 2 "" H 10308 5286 50  0001 C CNN
F 3 "" H 10308 5286 50  0001 C CNN
	1    10308 5286
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR0114
U 1 1 60C7C54E
P 10308 5086
F 0 "#PWR0114" H 10308 4836 50  0001 C CNN
F 1 "GND" H 10313 4913 50  0000 C CNN
F 2 "" H 10308 5086 50  0001 C CNN
F 3 "" H 10308 5086 50  0001 C CNN
	1    10308 5086
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR0115
U 1 1 60C7D520
P 10308 4886
F 0 "#PWR0115" H 10308 4636 50  0001 C CNN
F 1 "GND" H 10313 4713 50  0000 C CNN
F 2 "" H 10308 4886 50  0001 C CNN
F 3 "" H 10308 4886 50  0001 C CNN
	1    10308 4886
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR0116
U 1 1 60C7E8C1
P 10308 4686
F 0 "#PWR0116" H 10308 4436 50  0001 C CNN
F 1 "GND" H 10313 4513 50  0000 C CNN
F 2 "" H 10308 4686 50  0001 C CNN
F 3 "" H 10308 4686 50  0001 C CNN
	1    10308 4686
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR0117
U 1 1 60C7FF31
P 10308 4486
F 0 "#PWR0117" H 10308 4236 50  0001 C CNN
F 1 "GND" H 10313 4313 50  0000 C CNN
F 2 "" H 10308 4486 50  0001 C CNN
F 3 "" H 10308 4486 50  0001 C CNN
	1    10308 4486
	0    -1   -1   0   
$EndComp
Wire Wire Line
	8502 1320 8769 1320
$Comp
L Connector_Generic:Conn_02x04_Counter_Clockwise J4
U 1 1 606C1AF4
P 8969 1420
F 0 "J4" H 9019 1737 50  0000 C CNN
F 1 "Conn_02x04_Counter_Clockwise" H 9019 1646 50  0000 C CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_2x04_P2.54mm_Vertical" H 8969 1420 50  0001 C CNN
F 3 "~" H 8969 1420 50  0001 C CNN
	1    8969 1420
	1    0    0    -1  
$EndComp
Wire Wire Line
	8502 1420 8650 1420
Wire Wire Line
	8650 1420 8650 1520
Wire Wire Line
	8650 1520 8769 1520
Wire Wire Line
	8502 1520 8598 1520
Wire Wire Line
	8598 1520 8598 1743
Wire Wire Line
	8598 1743 9320 1743
Wire Wire Line
	9320 1743 9320 1620
Wire Wire Line
	9320 1620 9269 1620
Wire Wire Line
	8502 1620 8502 1805
Wire Wire Line
	8502 1805 9425 1805
Wire Wire Line
	9425 1805 9425 1320
Wire Wire Line
	9425 1320 9269 1320
Wire Wire Line
	8769 1420 8769 1320
Connection ~ 8769 1320
Wire Wire Line
	8769 1620 8769 1520
Connection ~ 8769 1520
Wire Wire Line
	9269 1520 9269 1620
Connection ~ 9269 1620
Wire Wire Line
	9269 1420 9269 1320
Connection ~ 9269 1320
$Comp
L power:GND #PWR0103
U 1 1 606840FB
P 4655 3006
F 0 "#PWR0103" H 4655 2756 50  0001 C CNN
F 1 "GND" V 4660 2878 50  0000 R CNN
F 2 "" H 4655 3006 50  0001 C CNN
F 3 "" H 4655 3006 50  0001 C CNN
	1    4655 3006
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR0102
U 1 1 60682B8F
P 4655 3606
F 0 "#PWR0102" H 4655 3356 50  0001 C CNN
F 1 "GND" V 4660 3478 50  0000 R CNN
F 2 "" H 4655 3606 50  0001 C CNN
F 3 "" H 4655 3606 50  0001 C CNN
	1    4655 3606
	0    -1   -1   0   
$EndComp
$Comp
L ESP32-DEVKITC-32D:ESP32-DEVKITC-32D U2
U 1 1 6059AB08
P 3855 3906
F 0 "U2" H 3855 5073 50  0000 C CNN
F 1 "ESP32-DEVKITC-32D" H 3855 4982 50  0000 C CNN
F 2 "devkitcv4:MODULE_ESP32-DEVKITC-32D" H 3855 3906 50  0001 L BNN
F 3 "" H 3855 3906 50  0001 L BNN
F 4 "Espressif Systems" H 3855 3906 50  0001 L BNN "MANUFACTURER"
F 5 "4" H 3855 3906 50  0001 L BNN "PARTREV"
	1    3855 3906
	1    0    0    -1  
$EndComp
Text Label 4655 4706 0    50   ~ 0
IO7
Text Label 4655 4806 0    50   ~ 0
IO6
Text Label 4655 4506 0    50   ~ 0
IO15
Text Label 4655 4406 0    50   ~ 0
IO2
Text Label 4655 4306 0    50   ~ 0
IO0
Text Label 4655 4206 0    50   ~ 0
IO4
Text Label 4655 4106 0    50   ~ 0
IO16
Text Label 4655 4006 0    50   ~ 0
IO17
Text Label 4655 3906 0    50   ~ 0
IO5
Text Label 4655 3806 0    50   ~ 0
IO18
Text Label 4655 3706 0    50   ~ 0
IO19
Text Label 4655 3506 0    50   ~ 0
IO21
Text Label 4655 3406 0    50   ~ 0
IO3
Text Label 4655 3306 0    50   ~ 0
IO1
Text Label 4655 3206 0    50   ~ 0
IO22
Text Label 4655 3106 0    50   ~ 0
IO23
Text Label 2872 4706 0    50   ~ 0
IO11
Wire Wire Line
	3055 4706 2872 4706
Wire Wire Line
	3055 4606 2872 4606
Wire Wire Line
	3055 4506 2871 4506
Text Label 2871 4506 0    50   ~ 0
IO9
Wire Wire Line
	3055 4406 2872 4406
Text Label 2872 4406 0    50   ~ 0
IO13
Wire Wire Line
	3055 4206 2870 4206
Text Label 2870 4206 0    50   ~ 0
IO12
Wire Wire Line
	3055 4106 2869 4106
Text Label 2869 4106 0    50   ~ 0
IO14
Wire Wire Line
	3055 4006 2872 4006
Text Label 2872 4006 0    50   ~ 0
IO27
Wire Wire Line
	3055 3906 2874 3906
Text Label 2874 3906 0    50   ~ 0
IO26
Wire Wire Line
	3055 3806 2874 3806
Text Label 2874 3806 0    50   ~ 0
IO25
Wire Wire Line
	3055 3706 2874 3706
Text Label 2874 3706 0    50   ~ 0
IO33
Wire Wire Line
	3055 3606 2873 3606
Text Label 2873 3606 0    50   ~ 0
IO32
Wire Wire Line
	3055 3506 2873 3506
Text Label 2873 3506 0    50   ~ 0
IO35
Wire Wire Line
	3055 3406 2873 3406
Wire Wire Line
	3055 3306 2874 3306
Wire Wire Line
	3055 3206 2873 3206
Text Label 2873 3406 0    50   ~ 0
IO34
Text Label 2874 3306 0    50   ~ 0
IO39
Text Label 2873 3206 0    50   ~ 0
IO36
Wire Wire Line
	3055 3106 2873 3106
Text Label 2873 3106 0    50   ~ 0
EN
$Comp
L power:GND #PWR0101
U 1 1 6138DB2C
P 3055 4306
F 0 "#PWR0101" H 3055 4056 50  0001 C CNN
F 1 "GND" V 3060 4178 50  0000 R CNN
F 2 "" H 3055 4306 50  0001 C CNN
F 3 "" H 3055 4306 50  0001 C CNN
	1    3055 4306
	0    1    1    0   
$EndComp
Wire Wire Line
	3055 4806 2873 4806
Text Label 2873 4806 0    50   ~ 0
VIN
Wire Wire Line
	3055 3006 2871 3006
Text Label 2871 3006 0    50   ~ 0
3.3V
Text Label 2872 4606 0    50   ~ 0
IO10
Text Label 4655 4606 0    50   ~ 0
IO8
$Comp
L Connector_Generic:Conn_02x19_Counter_Clockwise J7
U 1 1 607D1A38
P 6196 3895
F 0 "J7" H 6246 5012 50  0000 C CNN
F 1 "Conn_02x19_Counter_Clockwise" H 6246 4921 50  0000 C CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_2x19_P2.54mm_Vertical" H 6196 3895 50  0001 C CNN
F 3 "~" H 6196 3895 50  0001 C CNN
	1    6196 3895
	1    0    0    -1  
$EndComp
Wire Wire Line
	1410 3799 1111 3799
Wire Wire Line
	1410 3699 1111 3699
Wire Wire Line
	1410 3599 1112 3599
Wire Wire Line
	1410 3499 1113 3499
Wire Wire Line
	1410 3399 1112 3399
Wire Wire Line
	1410 3299 1112 3299
Wire Wire Line
	1410 3199 1113 3199
Wire Wire Line
	1410 3099 1113 3099
Wire Wire Line
	1410 2999 1113 2999
$Comp
L Connector_Generic:Conn_02x19_Counter_Clockwise J3
U 1 1 607C40D3
P 1610 3899
F 0 "J3" H 1660 5016 50  0000 C CNN
F 1 "Conn_02x19_Counter_Clockwise" H 1660 4925 50  0000 C CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_2x19_P2.54mm_Vertical" H 1610 3899 50  0001 C CNN
F 3 "~" H 1610 3899 50  0001 C CNN
	1    1610 3899
	1    0    0    -1  
$EndComp
Wire Wire Line
	1410 3899 1112 3899
Wire Wire Line
	1410 3999 1112 3999
Wire Wire Line
	1410 4099 1112 4099
Wire Wire Line
	1410 4199 1112 4199
Wire Wire Line
	1410 4299 1113 4299
Wire Wire Line
	1410 4399 1113 4399
Wire Wire Line
	1410 4499 1112 4499
Wire Wire Line
	1410 4599 1113 4599
Wire Wire Line
	1410 4699 1113 4699
Wire Wire Line
	1410 4799 1113 4799
Text Label 1113 2999 0    50   ~ 0
3.3V
Text Label 1113 3099 0    50   ~ 0
3.3V
Text Label 1113 3199 0    50   ~ 0
EN
Text Label 1112 3299 0    50   ~ 0
EN
Text Label 1112 3399 0    50   ~ 0
IO36
Text Label 1113 3499 0    50   ~ 0
IO36
Text Label 1112 3599 0    50   ~ 0
IO39
Text Label 1111 3699 0    50   ~ 0
IO39
Text Label 1111 3799 0    50   ~ 0
IO34
Text Label 1112 3899 0    50   ~ 0
IO34
Text Label 1112 3999 0    50   ~ 0
IO35
Text Label 1112 4099 0    50   ~ 0
IO35
Text Label 1112 4199 0    50   ~ 0
IO32
Text Label 1113 4299 0    50   ~ 0
IO32
Text Label 1113 4399 0    50   ~ 0
IO33
Text Label 1112 4499 0    50   ~ 0
IO33
Text Label 1113 4599 0    50   ~ 0
IO25
Text Label 1113 4699 0    50   ~ 0
IO25
Text Label 1113 4799 0    50   ~ 0
IO26
Text Label 1910 4799 0    50   ~ 0
IO26
Text Label 1910 4699 0    50   ~ 0
IO27
Text Label 1910 4599 0    50   ~ 0
IO27
Text Label 1910 4499 0    50   ~ 0
IO14
Text Label 1910 4399 0    50   ~ 0
IO14
$Comp
L power:GND #PWR03
U 1 1 6092FDBC
P 1910 3999
F 0 "#PWR03" H 1910 3749 50  0001 C CNN
F 1 "GND" V 1915 3871 50  0000 R CNN
F 2 "" H 1910 3999 50  0001 C CNN
F 3 "" H 1910 3999 50  0001 C CNN
	1    1910 3999
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR013
U 1 1 60930FF0
P 1910 4099
F 0 "#PWR013" H 1910 3849 50  0001 C CNN
F 1 "GND" V 1915 3971 50  0000 R CNN
F 2 "" H 1910 4099 50  0001 C CNN
F 3 "" H 1910 4099 50  0001 C CNN
	1    1910 4099
	0    -1   -1   0   
$EndComp
Text Label 1910 4299 0    50   ~ 0
IO12
Text Label 1910 4199 0    50   ~ 0
IO12
Text Label 1910 3899 0    50   ~ 0
IO13
Text Label 1910 3799 0    50   ~ 0
IO13
Text Label 1910 3699 0    50   ~ 0
IO9
Text Label 1910 3599 0    50   ~ 0
IO9
Text Label 1910 3499 0    50   ~ 0
IO10
Text Label 1910 3399 0    50   ~ 0
IO10
Text Label 1910 3299 0    50   ~ 0
IO11
Text Label 1910 3199 0    50   ~ 0
IO11
Text Label 1910 3099 0    50   ~ 0
VIN
Text Label 1910 2999 0    50   ~ 0
VIN
Wire Wire Line
	5996 3095 5702 3095
Wire Wire Line
	5996 3195 5700 3195
Wire Wire Line
	5996 2995 5700 2995
Wire Wire Line
	5996 3295 5700 3295
Wire Wire Line
	5996 3395 5700 3395
Wire Wire Line
	5996 3595 5700 3595
Wire Wire Line
	5996 3695 5701 3695
Wire Wire Line
	5996 3795 5700 3795
Wire Wire Line
	5996 3895 5700 3895
Wire Wire Line
	5996 3995 5700 3995
Wire Wire Line
	5996 4095 5701 4095
Wire Wire Line
	5996 4195 5701 4195
Wire Wire Line
	5996 4395 5700 4395
Wire Wire Line
	5996 4495 5700 4495
Wire Wire Line
	5996 4595 5700 4595
Wire Wire Line
	5996 4695 5700 4695
Wire Wire Line
	5996 4795 5700 4795
$Comp
L power:GND #PWR0105
U 1 1 60A1300D
P 5700 2995
F 0 "#PWR0105" H 5700 2745 50  0001 C CNN
F 1 "GND" V 5705 2867 50  0000 R CNN
F 2 "" H 5700 2995 50  0001 C CNN
F 3 "" H 5700 2995 50  0001 C CNN
	1    5700 2995
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR0118
U 1 1 60A1473C
P 5702 3095
F 0 "#PWR0118" H 5702 2845 50  0001 C CNN
F 1 "GND" V 5707 2967 50  0000 R CNN
F 2 "" H 5702 3095 50  0001 C CNN
F 3 "" H 5702 3095 50  0001 C CNN
	1    5702 3095
	0    1    1    0   
$EndComp
Text Label 5700 3195 0    50   ~ 0
IO23
Text Label 5700 3295 0    50   ~ 0
IO23
Wire Wire Line
	5996 3495 5700 3495
Text Label 5700 3395 0    50   ~ 0
IO22
Text Label 5700 3495 0    50   ~ 0
IO22
Text Label 5700 3595 0    50   ~ 0
IO1
Text Label 5701 3695 0    50   ~ 0
IO1
Text Label 5700 3795 0    50   ~ 0
IO3
Text Label 5700 3895 0    50   ~ 0
IO3
$Comp
L power:GND #PWR015
U 1 1 60A2367F
P 5701 4195
F 0 "#PWR015" H 5701 3945 50  0001 C CNN
F 1 "GND" V 5706 4067 50  0000 R CNN
F 2 "" H 5701 4195 50  0001 C CNN
F 3 "" H 5701 4195 50  0001 C CNN
	1    5701 4195
	0    1    1    0   
$EndComp
Text Label 5700 4395 0    50   ~ 0
IO19
Text Label 5700 4495 0    50   ~ 0
IO19
Text Label 5700 4595 0    50   ~ 0
IO18
Text Label 5700 4695 0    50   ~ 0
IO18
Text Label 5700 4795 0    50   ~ 0
IO5
Text Label 6496 4795 0    50   ~ 0
IO5
Text Label 6496 4695 0    50   ~ 0
IO17
Text Label 6496 4595 0    50   ~ 0
IO17
Text Label 6496 4495 0    50   ~ 0
IO16
Text Label 6496 4395 0    50   ~ 0
IO16
Text Label 6496 4295 0    50   ~ 0
IO4
Text Label 6496 4195 0    50   ~ 0
IO4
Text Label 6496 4095 0    50   ~ 0
IO0
Text Label 6496 3995 0    50   ~ 0
IO0
Text Label 6496 3795 0    50   ~ 0
IO2
Text Label 6496 3895 0    50   ~ 0
IO2
Text Label 6496 3695 0    50   ~ 0
IO15
Text Label 6496 3595 0    50   ~ 0
IO15
Text Label 6496 3495 0    50   ~ 0
IO8
Text Label 6496 3395 0    50   ~ 0
IO8
Wire Wire Line
	5996 4295 5700 4295
$Comp
L power:GND #PWR014
U 1 1 60A24767
P 5700 4295
F 0 "#PWR014" H 5700 4045 50  0001 C CNN
F 1 "GND" V 5705 4167 50  0000 R CNN
F 2 "" H 5700 4295 50  0001 C CNN
F 3 "" H 5700 4295 50  0001 C CNN
	1    5700 4295
	0    1    1    0   
$EndComp
Text Label 5700 3995 0    50   ~ 0
IO21
Text Label 5701 4095 0    50   ~ 0
IO21
Text Label 6496 3295 0    50   ~ 0
IO7
Text Label 6496 3195 0    50   ~ 0
IO7
Text Label 6496 3095 0    50   ~ 0
IO6
Text Label 6496 2995 0    50   ~ 0
IO6
Text Label 4310 7065 0    50   ~ 0
IO17
Text Label 8868 5054 0    50   ~ 0
IO2
Text Label 8737 3808 0    50   ~ 0
IO16
Text Label 7666 4555 0    50   ~ 0
IO4
$EndSCHEMATC
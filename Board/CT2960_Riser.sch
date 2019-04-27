EESchema Schematic File Version 4
LIBS:CT2960_Riser-cache
EELAYER 29 0
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
NoConn ~ 4650 1650
NoConn ~ 4650 1750
NoConn ~ 4650 1950
NoConn ~ 4650 2250
NoConn ~ 4650 2350
NoConn ~ 4650 3050
NoConn ~ 4650 3150
NoConn ~ 4650 3350
NoConn ~ 4650 3550
NoConn ~ 4650 3650
NoConn ~ 4650 3750
NoConn ~ 4650 3850
NoConn ~ 4650 3950
NoConn ~ 4650 4150
NoConn ~ 4650 4450
NoConn ~ 4650 4550
NoConn ~ 4650 4750
NoConn ~ 4650 4850
NoConn ~ 4650 4950
NoConn ~ 4650 5050
NoConn ~ 4650 5150
NoConn ~ 4650 5250
NoConn ~ 4650 5550
NoConn ~ 4650 5650
NoConn ~ 4650 6050
NoConn ~ 6050 1250
NoConn ~ 6050 2150
NoConn ~ 6050 2350
NoConn ~ 6050 2450
NoConn ~ 6050 2550
NoConn ~ 6050 2650
NoConn ~ 6050 4450
NoConn ~ 6050 4550
NoConn ~ 6050 4650
NoConn ~ 6050 4750
NoConn ~ 6050 4850
NoConn ~ 6050 4950
NoConn ~ 6050 5050
NoConn ~ 6050 5150
NoConn ~ 6050 5250
NoConn ~ 6050 5350
$Comp
L power:GND #PWR0101
U 1 1 5CA4C9E5
P 4650 6150
F 0 "#PWR0101" H 4650 5900 50  0001 C CNN
F 1 "GND" H 4655 5977 50  0000 C CNN
F 2 "" H 4650 6150 50  0001 C CNN
F 3 "" H 4650 6150 50  0001 C CNN
	1    4650 6150
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0102
U 1 1 5CA4D550
P 4650 4250
F 0 "#PWR0102" H 4650 4000 50  0001 C CNN
F 1 "GND" H 4550 4250 50  0000 C CNN
F 2 "" H 4650 4250 50  0001 C CNN
F 3 "" H 4650 4250 50  0001 C CNN
	1    4650 4250
	1    0    0    -1  
$EndComp
$Comp
L Connector:Bus_ISA_16bit J1
U 1 1 5CA35334
P 5350 3750
F 0 "J1" H 5350 6517 50  0000 C CNN
F 1 "Bus_ISA_16bit" H 5350 6426 50  0000 C CNN
F 2 "Connector_PCBEdge:ISA_Slot_98" H 5350 3800 50  0001 C CNN
F 3 "https://en.wikipedia.org/wiki/Industry_Standard_Architecture" H 5350 3800 50  0001 C CNN
	1    5350 3750
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0103
U 1 1 5CA8E7EA
P 4650 1250
F 0 "#PWR0103" H 4650 1000 50  0001 C CNN
F 1 "GND" V 4655 1122 50  0000 R CNN
F 2 "" H 4650 1250 50  0001 C CNN
F 3 "" H 4650 1250 50  0001 C CNN
	1    4650 1250
	0    1    1    0   
$EndComp
$Comp
L power:+5V #PWR0104
U 1 1 5CA8F721
P 4650 1450
F 0 "#PWR0104" H 4650 1300 50  0001 C CNN
F 1 "+5V" V 4665 1578 50  0000 L CNN
F 2 "" H 4650 1450 50  0001 C CNN
F 3 "" H 4650 1450 50  0001 C CNN
	1    4650 1450
	0    -1   -1   0   
$EndComp
Text Label 6200 4250 0    50   ~ 0
A00
Text Label 6200 4150 0    50   ~ 0
A01
Text Label 6200 4050 0    50   ~ 0
A02
Text Label 6200 3950 0    50   ~ 0
A03
Text Label 6200 3850 0    50   ~ 0
A04
Text Label 6200 3750 0    50   ~ 0
A05
Text Label 6200 3650 0    50   ~ 0
A06
Text Label 6200 3550 0    50   ~ 0
A07
Text Label 6200 3450 0    50   ~ 0
A08
Text Label 6200 3350 0    50   ~ 0
A09
Text Label 6200 3250 0    50   ~ 0
A10
Text Label 6200 3150 0    50   ~ 0
A11
Text Label 6200 3050 0    50   ~ 0
A12
Text Label 6200 2950 0    50   ~ 0
A13
Text Label 6200 2850 0    50   ~ 0
A14
Entry Wire Line
	6400 6150 6500 6250
Entry Wire Line
	6400 6050 6500 6150
Entry Wire Line
	6400 5850 6500 5950
Entry Wire Line
	6400 5750 6500 5850
Entry Wire Line
	6400 5650 6500 5750
Entry Wire Line
	6400 5550 6500 5650
Entry Wire Line
	6400 5950 6500 6050
Entry Wire Line
	6400 5450 6500 5550
Entry Wire Line
	6400 2750 6500 2850
Entry Wire Line
	6400 2850 6500 2950
Entry Wire Line
	6400 2950 6500 3050
Entry Wire Line
	6400 3050 6500 3150
Entry Wire Line
	6400 3150 6500 3250
Entry Wire Line
	6400 3250 6500 3350
Entry Wire Line
	6400 3350 6500 3450
Entry Wire Line
	6400 3450 6500 3550
Entry Wire Line
	6400 3550 6500 3650
Entry Wire Line
	6400 3650 6500 3750
Entry Wire Line
	6400 3750 6500 3850
Entry Wire Line
	6400 3850 6500 3950
Entry Wire Line
	6400 3950 6500 4050
Entry Wire Line
	6400 4050 6500 4150
Entry Wire Line
	6400 4150 6500 4250
Entry Wire Line
	6400 4250 6500 4350
Text Label 6200 2750 0    50   ~ 0
A15
Text Label 6150 5450 0    50   ~ 0
D8
Text Label 6150 5550 0    50   ~ 0
D9
Text Label 6150 5650 0    50   ~ 0
D10
Text Label 6150 5750 0    50   ~ 0
D11
Text Label 6150 5850 0    50   ~ 0
D12
Text Label 6150 5950 0    50   ~ 0
D13
Text Label 6150 6050 0    50   ~ 0
D14
Text Label 6150 6150 0    50   ~ 0
D15
Entry Wire Line
	6400 1350 6500 1450
Entry Wire Line
	6400 1450 6500 1550
Entry Wire Line
	6400 1550 6500 1650
Entry Wire Line
	6400 1650 6500 1750
Entry Wire Line
	6400 1750 6500 1850
Entry Wire Line
	6400 1850 6500 1950
Entry Wire Line
	6400 1950 6500 2050
Entry Wire Line
	6400 2050 6500 2150
Text Label 6150 2050 0    50   ~ 0
D0
Text Label 6150 1950 0    50   ~ 0
D1
Text Label 6150 1850 0    50   ~ 0
D2
Text Label 6150 1750 0    50   ~ 0
D3
Text Label 6150 1650 0    50   ~ 0
D4
Text Label 6150 1550 0    50   ~ 0
D5
Text Label 6150 1350 0    50   ~ 0
D7
Text Label 6150 1450 0    50   ~ 0
D6
Text Notes 7350 7500 0    50   ~ 10
CT2960 Riser Board
Text Notes 10600 7650 0    50   ~ 10
0.1
Text Notes 8100 7650 0    50   ~ 10
01-4-2019
Wire Bus Line
	6850 5050 7000 5050
Wire Wire Line
	6050 6150 6400 6150
Wire Wire Line
	6050 6050 6400 6050
Wire Wire Line
	6050 5950 6400 5950
Wire Wire Line
	6050 5850 6400 5850
Wire Wire Line
	6050 5750 6400 5750
Wire Wire Line
	6050 5650 6400 5650
Wire Wire Line
	6050 5550 6400 5550
Wire Wire Line
	6050 5450 6400 5450
Wire Wire Line
	6400 2750 6050 2750
Wire Wire Line
	6050 2850 6400 2850
Wire Wire Line
	6050 2950 6400 2950
Wire Wire Line
	6050 3050 6400 3050
Wire Wire Line
	6050 3150 6400 3150
Wire Wire Line
	6050 3250 6400 3250
Wire Wire Line
	6050 3350 6400 3350
Wire Wire Line
	6050 3450 6400 3450
Wire Wire Line
	6050 3550 6400 3550
Wire Wire Line
	6050 3650 6400 3650
Wire Wire Line
	6050 3750 6400 3750
Wire Wire Line
	6050 3850 6400 3850
Wire Wire Line
	6050 3950 6400 3950
Wire Wire Line
	6050 4050 6400 4050
Wire Wire Line
	6050 4150 6400 4150
Wire Wire Line
	6050 4250 6400 4250
Wire Wire Line
	6050 2050 6400 2050
Wire Wire Line
	6050 1950 6400 1950
Wire Wire Line
	6050 1850 6400 1850
Wire Wire Line
	6050 1750 6400 1750
Wire Wire Line
	6050 1650 6400 1650
Wire Wire Line
	6050 1550 6400 1550
Wire Wire Line
	6050 1450 6400 1450
Wire Wire Line
	6050 1350 6400 1350
$Comp
L Connector_Generic:Conn_02x20_Odd_Even J3
U 1 1 5CC9CF72
P 2200 3250
F 0 "J3" H 2250 4367 50  0000 C CNN
F 1 "Conn_02x20_Odd_Even" H 2250 4276 50  0000 C CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_2x20_P2.54mm_Vertical" H 2200 3250 50  0001 C CNN
F 3 "~" H 2200 3250 50  0001 C CNN
	1    2200 3250
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_02x12_Top_Bottom J4
U 1 1 5CB0A6C3
P 3300 1250
F 0 "J4" H 3350 1967 50  0000 C CNN
F 1 "Conn_02x12_Top_Bottom" H 3350 1876 50  0000 C CNN
F 2 "Connector_Molex:Molex_Mini-Fit_Jr_5566-20A_2x10_P4.20mm_Vertical" H 3300 1250 50  0001 C CNN
F 3 "~" H 3300 1250 50  0001 C CNN
	1    3300 1250
	1    0    0    -1  
$EndComp
Wire Wire Line
	3600 850  4400 850 
Wire Wire Line
	4400 850  4400 1850
Wire Wire Line
	4400 1850 4650 1850
Wire Wire Line
	4650 2050 3050 2050
Wire Wire Line
	3050 2050 3050 1650
Wire Wire Line
	3050 1650 3100 1650
$Comp
L Diode:1N4003 D1
U 1 1 5CB2FB98
P 4000 1200
F 0 "D1" V 4046 1121 50  0000 R CNN
F 1 "1N4003" V 3955 1121 50  0000 R CNN
F 2 "Diode_THT:D_DO-41_SOD81_P10.16mm_Horizontal" H 4000 1025 50  0001 C CNN
F 3 "http://www.vishay.com/docs/88503/1n4001.pdf" H 4000 1200 50  0001 C CNN
	1    4000 1200
	0    -1   -1   0   
$EndComp
Wire Wire Line
	4000 1350 3850 1350
Wire Wire Line
	3850 1350 3850 1150
Wire Wire Line
	3850 1150 3600 1150
NoConn ~ 9450 4250
$Comp
L Connector_Generic:Conn_02x20_Odd_Even J2
U 1 1 5CB4448D
P 9150 3750
F 0 "J2" H 9250 4900 50  0000 C CNN
F 1 "Conn_02x20_Odd_Even" H 9300 4800 50  0000 C CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_2x20_P2.54mm_Vertical" H 9150 3750 50  0001 C CNN
F 3 "~" H 9150 3750 50  0001 C CNN
	1    9150 3750
	1    0    0    -1  
$EndComp
Entry Wire Line
	7000 4700 7100 4800
Entry Wire Line
	7000 4300 7100 4400
Entry Wire Line
	7000 4100 7100 4200
Entry Wire Line
	7000 4000 7100 4100
Entry Wire Line
	7000 3800 7100 3900
Entry Wire Line
	7000 3700 7100 3800
Entry Wire Line
	7000 3600 7100 3700
Entry Wire Line
	7000 3200 7100 3300
Entry Wire Line
	7000 3000 7100 3100
Entry Wire Line
	7000 2900 7100 3000
Entry Wire Line
	7000 2700 7100 2800
Entry Wire Line
	7000 2600 7100 2700
Wire Wire Line
	7100 2700 7650 2700
Wire Wire Line
	7100 2800 7650 2800
Wire Wire Line
	7100 3000 7650 3000
Wire Wire Line
	7100 3100 7650 3100
Wire Wire Line
	7100 3300 7650 3300
Wire Wire Line
	7100 3600 7650 3600
Wire Wire Line
	7100 3700 7650 3700
Wire Wire Line
	7100 3800 7650 3800
Wire Wire Line
	7100 3900 7650 3900
Wire Wire Line
	7100 4100 7650 4100
Wire Wire Line
	7100 4200 7650 4200
Wire Wire Line
	7100 4400 7650 4400
Wire Wire Line
	7100 4700 7650 4700
Wire Wire Line
	7100 4800 7650 4800
Entry Wire Line
	7000 4600 7100 4700
Entry Wire Line
	7000 4400 7100 4500
Wire Wire Line
	7100 4500 7650 4500
Entry Wire Line
	7000 3500 7100 3600
Entry Wire Line
	7000 3300 7100 3400
Wire Wire Line
	7100 3400 7650 3400
$Comp
L Logic_LevelTranslator:IDT74ALVC164245 IC2
U 1 1 5CB12B46
P 8050 3300
F 0 "IC2" H 8050 4250 50  0000 C CNN
F 1 "IDT74ALVC164245" H 8050 4150 50  0000 C CNN
F 2 "Package_SO:SSOP-48_7.5x15.9mm_P0.635mm" H 8050 4200 50  0001 C CNN
F 3 "https://www.idt.com/document/dst/74alvc164245-datasheet" H 8050 4200 50  0001 C CNN
	1    8050 3300
	1    0    0    -1  
$EndComp
Entry Wire Line
	8600 2700 8700 2800
Entry Wire Line
	8600 2800 8700 2900
Wire Wire Line
	8600 2700 8450 2700
Wire Wire Line
	8600 2800 8450 2800
Entry Wire Line
	8600 3000 8700 3100
Entry Wire Line
	8600 3100 8700 3200
Entry Wire Line
	8600 3300 8700 3400
Entry Wire Line
	8600 3400 8700 3500
Entry Wire Line
	8600 3600 8700 3700
Entry Wire Line
	8600 3700 8700 3800
Entry Wire Line
	8600 3800 8700 3900
Entry Wire Line
	8600 3900 8700 4000
Entry Wire Line
	8600 4100 8700 4200
Entry Wire Line
	8600 4200 8700 4300
Entry Wire Line
	8600 4400 8700 4500
Entry Wire Line
	8600 4500 8700 4600
Entry Wire Line
	8600 4800 8700 4900
Entry Wire Line
	8600 4700 8700 4800
Wire Wire Line
	8600 3000 8450 3000
Wire Wire Line
	8450 3100 8600 3100
Wire Wire Line
	8600 3300 8450 3300
Wire Wire Line
	8600 3400 8450 3400
Wire Wire Line
	8600 3600 8450 3600
Wire Wire Line
	8600 3700 8450 3700
Wire Wire Line
	8600 3800 8450 3800
Wire Wire Line
	8600 3900 8450 3900
Wire Wire Line
	8600 4100 8450 4100
Wire Wire Line
	8600 4200 8450 4200
Wire Wire Line
	8600 4400 8450 4400
Wire Wire Line
	8600 4500 8450 4500
Wire Wire Line
	8600 4700 8450 4700
Wire Wire Line
	8600 4800 8450 4800
Text Label 7350 3100 0    50   ~ 0
A12
Text Label 7350 3300 0    50   ~ 0
A11
Text Label 7350 3400 0    50   ~ 0
A10
Text Label 7350 3600 0    50   ~ 0
A09
Text Label 7350 3700 0    50   ~ 0
A08
Text Label 7350 3800 0    50   ~ 0
A07
Text Label 7350 3900 0    50   ~ 0
A06
Text Label 7350 4100 0    50   ~ 0
A05
Text Label 7350 4200 0    50   ~ 0
A04
Text Label 7350 4400 0    50   ~ 0
A03
Text Label 7350 4500 0    50   ~ 0
A02
Text Label 7350 4700 0    50   ~ 0
A01
Text Label 7350 4800 0    50   ~ 0
A00
$Comp
L power:GND #PWR0105
U 1 1 5CE4F381
P 7650 2900
F 0 "#PWR0105" H 7650 2650 50  0001 C CNN
F 1 "GND" V 7600 2800 50  0000 R CNN
F 2 "" H 7650 2900 50  0001 C CNN
F 3 "" H 7650 2900 50  0001 C CNN
	1    7650 2900
	0    1    1    0   
$EndComp
$Comp
L power:+5V #PWR0113
U 1 1 5CE506F7
P 7650 3200
F 0 "#PWR0113" H 7650 3050 50  0001 C CNN
F 1 "+5V" V 7700 3300 50  0000 L CNN
F 2 "" H 7650 3200 50  0001 C CNN
F 3 "" H 7650 3200 50  0001 C CNN
	1    7650 3200
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR0114
U 1 1 5CE51E3E
P 7650 3500
F 0 "#PWR0114" H 7650 3250 50  0001 C CNN
F 1 "GND" V 7600 3400 50  0000 R CNN
F 2 "" H 7650 3500 50  0001 C CNN
F 3 "" H 7650 3500 50  0001 C CNN
	1    7650 3500
	0    1    1    0   
$EndComp
$Comp
L power:+5V #PWR0115
U 1 1 5CE53419
P 7650 4300
F 0 "#PWR0115" H 7650 4150 50  0001 C CNN
F 1 "+5V" V 7700 4400 50  0000 L CNN
F 2 "" H 7650 4300 50  0001 C CNN
F 3 "" H 7650 4300 50  0001 C CNN
	1    7650 4300
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR0116
U 1 1 5CE5405C
P 7650 4600
F 0 "#PWR0116" H 7650 4350 50  0001 C CNN
F 1 "GND" V 7600 4500 50  0000 R CNN
F 2 "" H 7650 4600 50  0001 C CNN
F 3 "" H 7650 4600 50  0001 C CNN
	1    7650 4600
	0    1    1    0   
$EndComp
Wire Wire Line
	4000 1050 3600 1050
Text Label 7350 3000 0    50   ~ 0
A13
Text Label 7350 2800 0    50   ~ 0
A14
Text Label 7350 2700 0    50   ~ 0
A15
$Comp
L power:GND #PWR0106
U 1 1 5CB56A9F
P 9450 3350
F 0 "#PWR0106" H 9450 3100 50  0001 C CNN
F 1 "GND" V 9455 3222 50  0000 R CNN
F 2 "" H 9450 3350 50  0001 C CNN
F 3 "" H 9450 3350 50  0001 C CNN
	1    9450 3350
	0    -1   -1   0   
$EndComp
Wire Bus Line
	7000 2300 7000 5050
Wire Bus Line
	8700 2300 8700 5000
Wire Bus Line
	6500 1200 6500 6450
$EndSCHEMATC

EESchema Schematic File Version 2
LIBS:power,device,transistors,conn,linear,regul,74xx,cmos4000,adc-dac,memory,xilinx,special,microcontrollers,dsp,microchip,analog_switches,motorola,texas,intel,audio,interface,digital-audio,philips,display,cypress,siliconi,opto,contrib,valves,./orxj-1.cache
EELAYER 23  0
EELAYER END
$Descr A4 11700 8267
Sheet 1 1
Title "ORXJ-1 Open Rocrail Xpressnet Junction"
Date "15 oct 2008"
Rev "0.2"
Comp "www.Rocrail.net"
Comment1 "Rocrail Hardware Team"
Comment2 "Creative Commons Attribution-ShareAlike 3.0 Licence"
Comment3 ""
Comment4 ""
$EndDescr
Connection ~ 2550 2750
Wire Wire Line
	2550 2600 2550 2750
Wire Wire Line
	3500 5950 1200 5950
Wire Wire Line
	1200 5950 1200 2600
Wire Wire Line
	9150 2600 9150 2850
Wire Wire Line
	9150 2850 3400 2850
Wire Wire Line
	8950 2950 8950 2600
Connection ~ 10200 2800
Connection ~ 8600 5100
Wire Wire Line
	8400 5100 10200 5100
Wire Wire Line
	10200 5100 10200 2600
Wire Wire Line
	9250 2800 9250 2600
Wire Wire Line
	7600 5100 7800 5100
Wire Wire Line
	5400 6100 5400 5950
Wire Wire Line
	4350 5100 5000 5100
Connection ~ 7050 5100
Connection ~ 4650 6100
Wire Wire Line
	4650 5500 4650 6100
Connection ~ 6100 6100
Wire Wire Line
	6100 6100 6100 5500
Connection ~ 6500 5100
Wire Wire Line
	2650 5100 2650 6100
Connection ~ 6100 5100
Connection ~ 4650 5100
Wire Wire Line
	3500 2600 3500 3350
Connection ~ 7650 2900
Connection ~ 7850 2800
Wire Wire Line
	6150 2950 6150 2600
Connection ~ 3600 2750
Wire Wire Line
	2250 2750 5100 2750
Wire Wire Line
	2250 2750 2250 2600
Connection ~ 4800 2900
Wire Wire Line
	4800 2600 4800 2900
Connection ~ 5000 2800
Wire Wire Line
	5000 2600 5000 2800
Connection ~ 6250 2900
Wire Wire Line
	6250 2900 6250 2600
Connection ~ 6450 2800
Wire Wire Line
	6450 2800 6450 2600
Wire Wire Line
	7550 2950 7550 2600
Wire Wire Line
	3200 2600 3200 2950
Wire Wire Line
	7750 2850 7750 2600
Wire Wire Line
	3400 2850 3400 2600
Wire Wire Line
	5100 2750 5100 2600
Wire Wire Line
	3600 2600 3600 2750
Wire Wire Line
	7850 2800 7850 2600
Wire Wire Line
	3300 2600 3300 2900
Wire Wire Line
	7650 2900 7650 2600
Wire Wire Line
	3100 2600 3100 3000
Wire Wire Line
	4600 2600 4600 3000
Wire Wire Line
	6350 2850 6350 2600
Connection ~ 6350 2850
Connection ~ 6150 2950
Wire Wire Line
	4900 2600 4900 2850
Connection ~ 4900 2850
Wire Wire Line
	4700 2600 4700 2950
Connection ~ 4700 2950
Wire Wire Line
	2050 2600 2050 3000
Wire Wire Line
	2050 3000 4600 3000
Connection ~ 3100 3000
Connection ~ 7750 2850
Connection ~ 7550 2950
Wire Wire Line
	4100 3350 4100 2800
Wire Wire Line
	3500 4300 3500 4250
Wire Wire Line
	7050 5600 7050 5500
Wire Wire Line
	6500 6100 6500 5500
Connection ~ 6500 6100
Connection ~ 5400 6100
Connection ~ 7050 6100
Wire Wire Line
	5400 5400 5400 5550
Connection ~ 8600 6100
Wire Wire Line
	5800 5100 7200 5100
Wire Wire Line
	8600 5600 8600 5500
Wire Wire Line
	4100 2800 10200 2800
Connection ~ 9250 2800
Wire Wire Line
	10400 2600 10400 6100
Wire Wire Line
	10400 6100 2650 6100
Wire Wire Line
	3200 2950 10400 2950
Connection ~ 10400 2950
Connection ~ 8950 2950
Wire Wire Line
	3300 2900 9050 2900
Wire Wire Line
	9050 2900 9050 2600
Wire Wire Line
	1400 2600 1400 4250
Wire Wire Line
	1400 4250 3500 4250
Wire Wire Line
	2350 2600 2350 3000
Connection ~ 2350 3000
$Comp
L CONN_2 P4
U 1 1 4900FAE4
P 2450 2250
F 0 "P4" V 2400 2250 40  0000 C C
F 1 "CONN_2" V 2500 2250 40  0000 C C
	1    2450 2250
	0    1    -1   0   
$EndComp
$Comp
L CONN_1 Drill6
U 1 1 48FF929F
P 5400 7550
F 0 "Drill6" H 5480 7550 40  0000 C C
F 1 "CONN_1" H 5350 7590 30  0001 C C
	1    5400 7550
	1    0    0    -1  
$EndComp
$Comp
L CONN_1 Drill5
U 1 1 48FF929A
P 5400 7450
F 0 "Drill5" H 5480 7450 40  0000 C C
F 1 "CONN_1" H 5350 7490 30  0001 C C
	1    5400 7450
	1    0    0    -1  
$EndComp
$Comp
L RJ12 J5
U 1 1 48FF6335
P 9050 2150
F 0 "J5" H 9250 2650 60  0000 C C
F 1 "RJ12" H 8900 2650 60  0000 C C
	1    9050 2150
	1    0    0    -1  
$EndComp
$Comp
L DIODE D4
U 1 1 48FF60C9
P 7400 5100
F 0 "D4" H 7400 5200 40  0000 C C
F 1 "1N4001" H 7400 5000 40  0000 C C
	1    7400 5100
	1    0    0    -1  
$EndComp
$Comp
L DIODE D2
U 1 1 48FF60C5
P 5400 5750
F 0 "D2" H 5400 5850 40  0000 C C
F 1 "1N4001" H 5400 5650 40  0000 C C
	1    5400 5750
	0    1    1    0   
$EndComp
$Comp
L BRIDGE D1
U 1 1 48FB552B
P 3500 5100
F 0 "D1" H 3500 5150 70  0000 C C
F 1 "B80C1500" H 3500 5050 70  0000 C C
	1    3500 5100
	1    0    0    -1  
$EndComp
$Comp
L LED D3
U 1 1 48FB5419
P 7050 5300
F 0 "D3" H 7050 5400 50  0000 C C
F 1 "LED 3mA" H 7050 5200 50  0000 C C
	1    7050 5300
	0    1    1    0   
$EndComp
$Comp
L R R2
U 1 1 48FB5410
P 7050 5850
F 0 "R2" V 7130 5850 50  0000 C C
F 1 "4K7" V 7050 5850 50  0000 C C
	1    7050 5850
	1    0    0    -1  
$EndComp
$Comp
L C C2
U 1 1 48FB53EE
P 6100 5300
F 0 "C2" H 6150 5400 50  0000 L C
F 1 "100nF" H 6150 5200 50  0000 L C
	1    6100 5300
	1    0    0    -1  
$EndComp
$Comp
L CONN_2 P1
U 1 1 48FB53E3
P 1300 2250
F 0 "P1" V 1250 2250 40  0000 C C
F 1 "CONN_2" V 1350 2250 40  0000 C C
	1    1300 2250
	0    -1   -1   0   
$EndComp
$Comp
L JUMPER JP1
U 1 1 48FB53DE
P 8100 5100
F 0 "JP1" H 8100 5250 60  0000 C C
F 1 "JUMPER" H 8100 5020 40  0000 C C
	1    8100 5100
	-1   0    0    1   
$EndComp
$Comp
L CAPAPOL C3
U 1 1 48FB53CD
P 6500 5300
F 0 "C3" H 6550 5400 50  0000 L C
F 1 "100uF" H 6550 5200 50  0000 L C
	1    6500 5300
	1    0    0    -1  
$EndComp
$Comp
L CAPAPOL C1
U 1 1 48FB53C8
P 4650 5300
F 0 "C1" H 4700 5400 50  0000 L C
F 1 "1000uF" H 4700 5200 50  0000 L C
	1    4650 5300
	1    0    0    -1  
$EndComp
$Comp
L LM7812 U1
U 1 1 48FB53B0
P 5400 5150
F 0 "U1" H 5550 4954 60  0000 C C
F 1 "LM7812" H 5400 5350 60  0000 C C
F 2 "~" H 5700 4850 60  0000 C C
	1    5400 5150
	1    0    0    -1  
$EndComp
$Comp
L JUMPER JP2
U 1 1 48FB52CB
P 3800 3350
F 0 "JP2" H 3800 3500 60  0000 C C
F 1 "JUMPER" H 3800 3270 40  0000 C C
	1    3800 3350
	1    0    0    -1  
$EndComp
$Comp
L CONN_2 P3
U 1 1 48FB5140
P 10300 2250
F 0 "P3" V 10250 2250 40  0000 C C
F 1 "CONN_2" V 10350 2250 40  0000 C C
	1    10300 2250
	0    -1   -1   0   
$EndComp
$Comp
L CONN_1 Drill4
U 1 1 48F618D4
P 5400 7350
F 0 "Drill4" H 5480 7350 40  0000 C C
F 1 "CONN_1" H 5350 7390 30  0001 C C
	1    5400 7350
	1    0    0    -1  
$EndComp
$Comp
L CONN_1 Drill3
U 1 1 48F6186F
P 5400 7250
F 0 "Drill3" H 5480 7250 40  0000 C C
F 1 "CONN_1" H 5350 7290 30  0001 C C
	1    5400 7250
	1    0    0    -1  
$EndComp
$Comp
L CONN_1 Drill2
U 1 1 48F6186C
P 5400 7150
F 0 "Drill2" H 5480 7150 40  0000 C C
F 1 "CONN_1" H 5350 7190 30  0001 C C
	1    5400 7150
	1    0    0    -1  
$EndComp
$Comp
L CONN_1 Drill1
U 1 1 48F61867
P 5400 7050
F 0 "Drill1" H 5480 7050 40  0000 C C
F 1 "CONN_1" H 5350 7090 30  0001 C C
	1    5400 7050
	1    0    0    -1  
$EndComp
$Comp
L CONN_2 P2
U 1 1 48F49B19
P 2150 2250
F 0 "P2" V 2100 2250 40  0000 C C
F 1 "CONN_2" V 2200 2250 40  0000 C C
	1    2150 2250
	0    1    -1   0   
$EndComp
$Comp
L LED D5
U 1 1 48F49AFC
P 8600 5300
F 0 "D5" H 8600 5400 50  0000 C C
F 1 "LED 3mA" H 8600 5200 50  0000 C C
	1    8600 5300
	0    1    1    0   
$EndComp
$Comp
L R R1
U 1 1 48F49AE6
P 8600 5850
F 0 "R1" V 8680 5850 50  0000 C C
F 1 "4K7" V 8600 5850 50  0000 C C
	1    8600 5850
	1    0    0    -1  
$EndComp
$Comp
L RJ12 J4
U 1 1 48F49AD6
P 7650 2150
F 0 "J4" H 7850 2650 60  0000 C C
F 1 "RJ12" H 7500 2650 60  0000 C C
	1    7650 2150
	1    0    0    -1  
$EndComp
$Comp
L RJ12 J3
U 1 1 48F49AD3
P 6250 2150
F 0 "J3" H 6450 2650 60  0000 C C
F 1 "RJ12" H 6100 2650 60  0000 C C
	1    6250 2150
	1    0    0    -1  
$EndComp
$Comp
L RJ12 J2
U 1 1 48F49AD1
P 4800 2150
F 0 "J2" H 5000 2650 60  0000 C C
F 1 "RJ12" H 4650 2650 60  0000 C C
	1    4800 2150
	1    0    0    -1  
$EndComp
$Comp
L RJ12 J1
U 1 1 48F49ACE
P 3300 2150
F 0 "J1" H 3500 2650 60  0000 C C
F 1 "RJ12" H 3150 2650 60  0000 C C
	1    3300 2150
	1    0    0    -1  
$EndComp
$EndSCHEMATC

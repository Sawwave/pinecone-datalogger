EESchema Schematic File Version 2
LIBS:Logger_KicadProject-rescue
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:Logger_KicadProject-cache
EELAYER 25 0
EELAYER END
$Descr A3 16535 11693
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
Text GLabel 13350 2050 2    39   Input ~ 0
Dend1
Text GLabel 13350 2150 2    39   Input ~ 0
Dend2
Text GLabel 13350 2850 2    39   Input ~ 0
SD_SERCOM_PAD0_MISO
Text GLabel 13350 2650 2    39   Output ~ 0
SD_SERCOM_PAD1_CS
Text GLabel 13350 2950 2    39   Output ~ 0
SD_SERCOM_PAD2_MOSI
Text GLabel 13350 2750 2    39   Output ~ 0
SD_SERCOM_PAD3_SCK
Text GLabel 13350 2550 2    39   Input ~ 0
SD_CHIP_DETECT
Text GLabel 13350 2350 2    39   BiDi ~ 0
DHT22_1
Text GLabel 13350 2450 2    39   BiDi ~ 0
DHT22_2
Text GLabel 13350 3750 2    39   BiDi ~ 0
DS3231_SCL
Text GLabel 13350 3650 2    39   BiDi ~ 0
DS3231_SDA
Text GLabel 8850 5950 2    39   Output ~ 0
MAX31856_SERCOM_PAD0_MISO
Text GLabel 8850 6150 2    39   Input ~ 0
MAX31856_SERCOM_PAD1_CS
Text GLabel 8850 5850 2    39   Input ~ 0
MAX31856_SERCOM_PAD2_MOSI
Text GLabel 8850 6050 2    39   Input ~ 0
MAX31856_SERCOM_PAD3_SCK
Text GLabel 13350 3150 2    39   Output ~ 0
MUX_SEL_A
Text GLabel 13350 3050 2    39   Output ~ 0
MUX_SEL_B
Text GLabel 13350 4250 2    39   Output ~ 0
SWDCLK
Text GLabel 13350 4350 2    39   Output ~ 0
SWDIO
Text GLabel 13350 2250 2    39   3State ~ 0
SDI_3v3_DATA
Text GLabel 13350 3950 2    39   Output ~ 0
5v_PWR_ENABLE
$Comp
L Battery-RESCUE-Logger_KicadProject BT1
U 1 1 585302CA
P 14900 8800
F 0 "BT1" H 14950 8650 50  0000 L CNN
F 1 "Battery" H 15000 8750 50  0001 L CNN
F 2 "Pinecone_footprints:S8201R_Batt_Holder" V 14900 8840 50  0001 C CNN
F 3 "" V 14900 8840 50  0000 C CNN
F 4 "S8201-46R" H 14900 8800 60  0001 C CNN "Part Id"
	1    14900 8800
	1    0    0    -1  
$EndComp
Text GLabel 7150 900  2    39   BiDi ~ 0
SWDIO
Text GLabel 7800 1000 2    39   BiDi ~ 0
SWDCLK
Text GLabel 7000 1300 2    39   BiDi ~ 0
RESET
Text GLabel 10650 1850 0    39   BiDi ~ 0
RESET
$Comp
L R_Small R1
U 1 1 585322E3
P 10700 1700
F 0 "R1" H 10730 1720 50  0000 L CNN
F 1 "RC0805JR-0710KL" H 10730 1660 50  0001 L CNN
F 2 "R_0805" H 10700 1700 50  0001 C CNN
F 3 "" H 10700 1700 50  0000 C CNN
F 4 "10KΩ" H 10550 1700 39  0000 C CNN "Resistance"
	1    10700 1700
	1    0    0    -1  
$EndComp
$Comp
L +3V3 #PWR01
U 1 1 58532780
P 10700 1550
F 0 "#PWR01" H 10700 1400 50  0001 C CNN
F 1 "+3V3" H 10700 1690 50  0000 C CNN
F 2 "" H 10700 1550 50  0000 C CNN
F 3 "" H 10700 1550 50  0000 C CNN
	1    10700 1550
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR02
U 1 1 58533060
P 7900 3100
F 0 "#PWR02" H 7900 2850 50  0001 C CNN
F 1 "GND" H 7900 2950 50  0000 C CNN
F 2 "" H 7900 3100 50  0000 C CNN
F 3 "" H 7900 3100 50  0000 C CNN
	1    7900 3100
	1    0    0    -1  
$EndComp
Text GLabel 8300 2650 2    39   Output ~ 0
Dend1
Text GLabel 8300 2750 2    39   Output ~ 0
Dend2
Text GLabel 5900 3650 2    39   BiDi ~ 0
DHT22_1
$Comp
L GND #PWR03
U 1 1 58538788
P 5750 3800
F 0 "#PWR03" H 5750 3550 50  0001 C CNN
F 1 "GND" H 5750 3650 50  0000 C CNN
F 2 "" H 5750 3800 50  0000 C CNN
F 3 "" H 5750 3800 50  0000 C CNN
	1    5750 3800
	1    0    0    -1  
$EndComp
$Comp
L R_Small R8
U 1 1 58538A9E
P 6000 3400
F 0 "R8" V 5900 3400 50  0000 L CNN
F 1 "RC0805JR-0710KL" H 5800 3550 50  0001 L CNN
F 2 "Resistors_SMD:R_0805" H 6000 3400 50  0001 C CNN
F 3 "" H 6000 3400 50  0000 C CNN
F 4 "10KΩ" V 5950 3600 39  0000 C CNN "Reistance"
	1    6000 3400
	0    1    1    0   
$EndComp
Text GLabel 5900 3800 2    39   BiDi ~ 0
DHT22_2
$Comp
L C_Small C15
U 1 1 585852A0
P 7500 5900
F 0 "C15" H 7550 6050 50  0000 L CNN
F 1 "C0805C104K5RACTU" H 7500 5750 50  0001 L CNN
F 2 "Capacitors_SMD:C_0805" H 7500 5900 50  0001 C CNN
F 3 "" H 7500 5900 50  0000 C CNN
F 4 ".1uF" H 7650 5900 39  0000 C CNN "Capacitance"
	1    7500 5900
	-1   0    0    1   
$EndComp
$Comp
L C_Small C12
U 1 1 58585A1D
P 8850 6450
F 0 "C12" H 8860 6520 50  0000 L CNN
F 1 "C0805C104K5RACTU" H 8860 6370 50  0001 L CNN
F 2 "Capacitors_SMD:C_0805" H 8850 6450 50  0001 C CNN
F 3 "" H 8850 6450 50  0000 C CNN
F 4 ".1uF" H 8950 6400 39  0000 C CNN "Capacitance"
	1    8850 6450
	1    0    0    -1  
$EndComp
$Comp
L C_Small C13
U 1 1 58585B43
P 7750 6200
F 0 "C13" H 7600 6300 50  0000 L CNN
F 1 "C0805C104K5RACTU" H 7760 6120 50  0001 L CNN
F 2 "Capacitors_SMD:C_0805" H 7750 6200 50  0001 C CNN
F 3 "" H 7750 6200 50  0000 C CNN
F 4 ".1uF" H 7650 6100 39  0000 C CNN "Capacitance"
	1    7750 6200
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR04
U 1 1 585868CD
P 8950 5700
F 0 "#PWR04" H 8950 5450 50  0001 C CNN
F 1 "GND" H 8950 5550 50  0001 C CNN
F 2 "" H 8950 5700 50  0000 C CNN
F 3 "" H 8950 5700 50  0000 C CNN
	1    8950 5700
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR05
U 1 1 58586920
P 7750 6350
F 0 "#PWR05" H 7750 6100 50  0001 C CNN
F 1 "GND" H 7750 6200 50  0001 C CNN
F 2 "" H 7750 6350 50  0000 C CNN
F 3 "" H 7750 6350 50  0000 C CNN
	1    7750 6350
	1    0    0    -1  
$EndComp
$Comp
L MAX4618 U6
U 1 1 585871C9
P 6250 6050
F 0 "U6" H 6300 5400 60  0000 C CNN
F 1 "MAX4618" H 6250 5550 60  0000 C CNN
F 2 "SMD_Packages:SO-16-N" H 6200 6450 60  0001 C CNN
F 3 "" H 6200 6450 60  0000 C CNN
	1    6250 6050
	1    0    0    -1  
$EndComp
$Comp
L C_Small C14
U 1 1 58587617
P 7000 5600
F 0 "C14" V 6900 5600 50  0000 L CNN
F 1 "C0805C104K5RACTU" V 6800 5600 50  0001 L CNN
F 2 "Capacitors_SMD:C_0805" H 7000 5600 50  0001 C CNN
F 3 "" H 7000 5600 50  0000 C CNN
F 4 ".1uF" V 7050 5500 39  0000 C CNN "Capacitance"
	1    7000 5600
	0    -1   -1   0   
$EndComp
$Comp
L GND #PWR06
U 1 1 585877DF
P 8850 6550
F 0 "#PWR06" H 8850 6300 50  0001 C CNN
F 1 "GND" H 8850 6400 50  0001 C CNN
F 2 "" H 8850 6550 50  0000 C CNN
F 3 "" H 8850 6550 50  0000 C CNN
	1    8850 6550
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR07
U 1 1 585885FE
P 6800 6500
F 0 "#PWR07" H 6800 6250 50  0001 C CNN
F 1 "GND" H 6800 6350 50  0001 C CNN
F 2 "" H 6800 6500 50  0000 C CNN
F 3 "" H 6800 6500 50  0000 C CNN
	1    6800 6500
	1    0    0    -1  
$EndComp
Text GLabel 6700 6250 2    39   Input ~ 0
MUX_SEL_B
Text GLabel 6700 6150 2    39   Input ~ 0
MUX_SEL_A
$Comp
L GND #PWR08
U 1 1 58589F32
P 7150 5700
F 0 "#PWR08" H 7150 5450 50  0001 C CNN
F 1 "GND" H 7150 5550 50  0001 C CNN
F 2 "" H 7150 5700 50  0000 C CNN
F 3 "" H 7150 5700 50  0000 C CNN
	1    7150 5700
	1    0    0    -1  
$EndComp
Text GLabel 13350 3550 2    39   Output ~ 0
MAX31856_SERCOM_PAD2_MOSI
Text GLabel 13350 3450 2    39   Input ~ 0
MAX31856_SERCOM_PAD0_MISO
Text GLabel 13350 3250 2    39   Output ~ 0
MAX31856_SERCOM_PAD1_CS
Text GLabel 13350 3350 2    39   Output ~ 0
MAX31856_SERCOM_PAD3_SCK
Text GLabel 3950 1050 2    39   Output ~ 0
SD_SERCOM_PAD0_MISO
Text GLabel 3950 1350 2    39   Input ~ 0
SD_SERCOM_PAD1_CS
Text GLabel 3950 1150 2    39   Input ~ 0
SD_SERCOM_PAD2_MOSI
Text GLabel 3950 1250 2    39   Input ~ 0
SD_SERCOM_PAD3_SCK
$Comp
L C_Small C11
U 1 1 58587370
P 4000 800
F 0 "C11" V 3900 850 50  0000 L CNN
F 1 "C0805C104K5RACTU" V 3900 750 50  0001 L CNN
F 2 "Capacitors_SMD:C_0805" H 4000 800 50  0001 C CNN
F 3 "" H 4000 800 50  0000 C CNN
F 4 ".1uF" V 4100 700 39  0000 C CNN "Capacitance"
	1    4000 800 
	0    1    1    0   
$EndComp
Text GLabel 2850 950  0    39   Output ~ 0
SD_CHIP_DETECT
$Comp
L GND #PWR09
U 1 1 58588A9D
P 3900 1500
F 0 "#PWR09" H 3900 1250 50  0001 C CNN
F 1 "GND" H 3900 1350 50  0000 C CNN
F 2 "" H 3900 1500 50  0000 C CNN
F 3 "" H 3900 1500 50  0000 C CNN
	1    3900 1500
	1    0    0    -1  
$EndComp
Text GLabel 3950 950  2    39   Output ~ 0
SWITCHED_3v3
Text GLabel 14850 8450 2    39   Input ~ 0
DS3231_SCL
Text GLabel 14850 8550 2    39   BiDi ~ 0
DS3231_SDA
Text GLabel 13350 3850 2    39   Output ~ 0
3v3_PWR_ENABLE
Text GLabel 8200 2250 2    39   Input ~ 0
SWITCHED_3v3
Text GLabel 6850 5450 2    39   Input ~ 0
SWITCHED_3v3
Text GLabel 7650 5300 0    39   Input ~ 0
SWITCHED_3v3
Text GLabel 3350 3150 2    39   Input ~ 0
SWITCHED_5v
$Comp
L R_Small R7
U 1 1 58594AFA
P 3550 4000
F 0 "R7" V 3600 4200 50  0000 L CNN
F 1 "RC0805JR-0710KL" V 3650 3900 50  0001 L CNN
F 2 "Resistors_SMD:R_0805" H 3550 4000 50  0001 C CNN
F 3 "" H 3550 4000 50  0000 C CNN
F 4 "10KΩ" V 3650 4000 39  0000 C CNN "Resistance"
	1    3550 4000
	0    -1   -1   0   
$EndComp
$Comp
L R_Small R6
U 1 1 58594BD6
P 3250 3300
F 0 "R6" H 3300 3350 50  0000 L CNN
F 1 "RC0805JR-0710KL" H 3350 3250 50  0001 L CNN
F 2 "Resistors_SMD:R_0805" H 3250 3300 50  0001 C CNN
F 3 "" H 3250 3300 50  0000 C CNN
F 4 "10KΩ" H 3400 3250 39  0000 C CNN "Resistance"
	1    3250 3300
	1    0    0    -1  
$EndComp
Text GLabel 3800 3950 2    39   3State ~ 0
SDI_3v3_DATA
$Comp
L +3V3 #PWR010
U 1 1 58597699
P 13550 8450
F 0 "#PWR010" H 13550 8300 50  0001 C CNN
F 1 "+3V3" H 13550 8600 50  0000 C CNN
F 2 "" H 13550 8450 50  0000 C CNN
F 3 "" H 13550 8450 50  0000 C CNN
	1    13550 8450
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR011
U 1 1 58597C9D
P 13600 9200
F 0 "#PWR011" H 13600 8950 50  0001 C CNN
F 1 "GND" H 13600 9050 50  0001 C CNN
F 2 "" H 13600 9200 50  0000 C CNN
F 3 "" H 13600 9200 50  0000 C CNN
	1    13600 9200
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR012
U 1 1 585981C8
P 14700 9200
F 0 "#PWR012" H 14700 8950 50  0001 C CNN
F 1 "GND" H 14700 9050 50  0001 C CNN
F 2 "" H 14700 9200 50  0000 C CNN
F 3 "" H 14700 9200 50  0000 C CNN
	1    14700 9200
	1    0    0    -1  
$EndComp
$Comp
L C_Small C9
U 1 1 5859AE09
P 10450 3050
F 0 "C9" V 10450 2800 50  0000 L CNN
F 1 "C0805C104K5RACTU" V 10500 2750 50  0001 L CNN
F 2 "Capacitors_SMD:C_0805" H 10450 3050 50  0001 C CNN
F 3 "" H 10450 3050 50  0000 C CNN
F 4 ".1uF" V 10350 3050 39  0000 C CNN "Capacitance"
	1    10450 3050
	0    -1   -1   0   
$EndComp
$Comp
L C_Small C2
U 1 1 5859B177
P 10400 2100
F 0 "C2" V 10450 1950 50  0000 L CNN
F 1 "C0805C104K5RACTU" V 10400 2250 50  0001 L CNN
F 2 "Capacitors_SMD:C_0805" H 10400 2100 50  0001 C CNN
F 3 "" H 10400 2100 50  0000 C CNN
F 4 ".1uF" V 10450 2300 39  0000 C CNN "Capacitance"
	1    10400 2100
	0    -1   -1   0   
$EndComp
$Comp
L C_Small C10
U 1 1 5859B472
P 10450 2900
F 0 "C10" V 10500 2650 50  0000 L CNN
F 1 "GRM219R61A106KE44D" V 10550 2650 50  0001 L CNN
F 2 "Capacitors_SMD:C_0805" H 10450 2900 50  0001 C CNN
F 3 "" H 10450 2900 50  0000 C CNN
F 4 "10uF" V 10550 2900 39  0000 C CNN "Capacitance"
	1    10450 2900
	0    -1   -1   0   
$EndComp
$Comp
L GND #PWR013
U 1 1 5859BBA7
P 10300 3150
F 0 "#PWR013" H 10300 2900 50  0001 C CNN
F 1 "GND" H 10300 3000 50  0001 C CNN
F 2 "" H 10300 3150 50  0000 C CNN
F 3 "" H 10300 3150 50  0000 C CNN
	1    10300 3150
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR014
U 1 1 5859E0BD
P 10800 4400
F 0 "#PWR014" H 10800 4150 50  0001 C CNN
F 1 "GND" H 10800 4250 50  0001 C CNN
F 2 "" H 10800 4400 50  0000 C CNN
F 3 "" H 10800 4400 50  0000 C CNN
	1    10800 4400
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR015
U 1 1 585A2972
P 4250 800
F 0 "#PWR015" H 4250 550 50  0001 C CNN
F 1 "GND" H 4250 650 50  0001 C CNN
F 2 "" H 4250 800 50  0000 C CNN
F 3 "" H 4250 800 50  0000 C CNN
	1    4250 800 
	1    0    0    -1  
$EndComp
Text GLabel 10600 6350 3    39   Output ~ 0
5v_PWR_ENABLE
$Comp
L +3V3 #PWR016
U 1 1 585A90C1
P 11900 6000
F 0 "#PWR016" H 11900 5850 50  0001 C CNN
F 1 "+3V3" H 11850 6150 50  0000 C CNN
F 2 "" H 11900 6000 50  0000 C CNN
F 3 "" H 11900 6000 50  0000 C CNN
	1    11900 6000
	1    0    0    -1  
$EndComp
Text GLabel 12200 6350 3    39   Input ~ 0
SWITCHED_5v
$Comp
L GND #PWR017
U 1 1 585AAE33
P 10450 6300
F 0 "#PWR017" H 10450 6050 50  0001 C CNN
F 1 "GND" H 10450 6150 50  0001 C CNN
F 2 "" H 10450 6300 50  0000 C CNN
F 3 "" H 10450 6300 50  0000 C CNN
	1    10450 6300
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR018
U 1 1 5859B00C
P 14550 6700
F 0 "#PWR018" H 14550 6450 50  0001 C CNN
F 1 "GND" H 14550 6550 50  0001 C CNN
F 2 "" H 14550 6700 50  0000 C CNN
F 3 "" H 14550 6700 50  0000 C CNN
	1    14550 6700
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR019
U 1 1 5859B092
P 13550 6500
F 0 "#PWR019" H 13550 6250 50  0001 C CNN
F 1 "GND" H 13550 6350 50  0001 C CNN
F 2 "" H 13550 6500 50  0000 C CNN
F 3 "" H 13550 6500 50  0000 C CNN
	1    13550 6500
	1    0    0    -1  
$EndComp
$Comp
L +3V3 #PWR020
U 1 1 5859B68A
P 13500 6100
F 0 "#PWR020" H 13500 5950 50  0001 C CNN
F 1 "+3V3" H 13500 6240 50  0001 C CNN
F 2 "" H 13500 6100 50  0000 C CNN
F 3 "" H 13500 6100 50  0000 C CNN
	1    13500 6100
	1    0    0    -1  
$EndComp
Text GLabel 14700 6200 2    39   Output ~ 0
SWITCHED_3v3
Text GLabel 14700 6300 2    39   Input ~ 0
3v3_PWR_ENABLE
$Comp
L C_Small C5
U 1 1 585A1F33
P 14650 6450
F 0 "C5" H 14660 6520 50  0000 L CNN
F 1 "C0805C104K5RACTU" H 14660 6370 50  0001 L CNN
F 2 "Capacitors_SMD:C_0805" H 14650 6450 50  0001 C CNN
F 3 "" H 14650 6450 50  0000 C CNN
F 4 ".1uF" H 14750 6350 39  0000 C CNN "Capacitance"
	1    14650 6450
	1    0    0    -1  
$EndComp
Text Notes 12750 7750 0    60   ~ 0
3v3 and 5v switching.\nUses two AP2281 load switches. \nPlace close to regulator, to shut off power to larger areas of board.\n
Text Notes 3400 4450 0    60   ~ 0
SDI-12 bus and level shifter.
$Comp
L GND #PWR021
U 1 1 585A9AB3
P 2900 4450
F 0 "#PWR021" H 2900 4200 50  0001 C CNN
F 1 "GND" H 2900 4300 50  0000 C CNN
F 2 "" H 2900 4450 50  0000 C CNN
F 3 "" H 2900 4450 50  0000 C CNN
	1    2900 4450
	1    0    0    -1  
$EndComp
Text GLabel 3400 4100 2    39   Input ~ 0
SWITCHED_3v3
$Comp
L GND #PWR022
U 1 1 585B0B6C
P 11350 9100
F 0 "#PWR022" H 11350 8850 50  0001 C CNN
F 1 "GND" H 11350 8950 50  0001 C CNN
F 2 "" H 11350 9100 50  0000 C CNN
F 3 "" H 11350 9100 50  0000 C CNN
	1    11350 9100
	1    0    0    -1  
$EndComp
Text GLabel 13350 4050 2    39   Output ~ 0
HEATER_ENABLE
Text GLabel 11000 8850 0    39   Input ~ 0
HEATER_ENABLE
Text Notes 10600 9400 0    60   ~ 0
Heater Jumper\nUses low-side NPN MOSFET\nPlace near 12v jack
Text Notes 5550 4550 0    60   ~ 0
DHT-22 ports with level shifters.\n
Text Notes 5300 1850 0    60   ~ 0
SWD connector for programming.\nplace near SAM D20 since connector requires unswitched 3v3\n
Text Notes 7200 3500 0    60   ~ 0
Ports for Dendrometers.\nData pins connected to analog inputs.
Text Notes 14050 9450 0    60   ~ 0
RTC, other only IC with unswitched power
$Comp
L C_Small C7
U 1 1 585D4B43
P 12050 6550
F 0 "C7" H 12060 6620 50  0000 L CNN
F 1 "C0805C104K5RACTU" H 12060 6470 50  0001 L CNN
F 2 "Capacitors_SMD:C_0805" H 12050 6550 50  0001 C CNN
F 3 "" H 12050 6550 50  0000 C CNN
F 4 ".1uF" H 12050 6700 39  0000 C CNN "Capacitance"
	1    12050 6550
	1    0    0    -1  
$EndComp
$Comp
L C_Small C8
U 1 1 585D4C3C
P 11850 6550
F 0 "C8" H 11860 6620 50  0000 L CNN
F 1 "C0805C104K5RACTU" H 11860 6470 50  0001 L CNN
F 2 "Capacitors_SMD:C_0805" H 11850 6550 50  0001 C CNN
F 3 "" H 11850 6550 50  0000 C CNN
F 4 ".1uF" H 11800 6750 39  0000 C CNN "Capacitance"
	1    11850 6550
	1    0    0    -1  
$EndComp
$Comp
L +12V #PWR023
U 1 1 585D9130
P 10600 6000
F 0 "#PWR023" H 10600 5850 50  0001 C CNN
F 1 "+12V" H 10600 6200 50  0000 C CNN
F 2 "" H 10600 6000 50  0000 C CNN
F 3 "" H 10600 6000 50  0000 C CNN
	1    10600 6000
	1    0    0    -1  
$EndComp
$Comp
L +12V #PWR024
U 1 1 585D9884
P 12100 6000
F 0 "#PWR024" H 12100 5850 50  0001 C CNN
F 1 "+12V" H 12100 6150 50  0000 C CNN
F 2 "" H 12100 6000 50  0000 C CNN
F 3 "" H 12100 6000 50  0000 C CNN
	1    12100 6000
	1    0    0    -1  
$EndComp
$Comp
L C_Small C3
U 1 1 585E04DC
P 10400 2400
F 0 "C3" V 10500 2550 50  0000 L CNN
F 1 "GRM21BR71C105KA01L" V 10400 2550 50  0001 L CNN
F 2 "Capacitors_SMD:C_0805" H 10400 2400 50  0001 C CNN
F 3 "" H 10400 2400 50  0000 C CNN
F 4 "1uF" V 10350 2150 39  0000 C CNN "Capacitance"
	1    10400 2400
	0    1    1    0   
$EndComp
$Comp
L C_Small C1
U 1 1 585E1387
P 10400 2250
F 0 "C1" V 10350 2550 50  0000 L CNN
F 1 "GRM219R61A106KE44D" V 10550 2150 50  0001 L CNN
F 2 "Capacitors_SMD:C_0805" H 10400 2250 50  0001 C CNN
F 3 "" H 10400 2250 50  0000 C CNN
F 4 "10uF" V 10400 1950 39  0000 C CNN "Capacitance"
	1    10400 2250
	0    1    1    0   
$EndComp
$Comp
L +3.3V #PWR025
U 1 1 585E4E26
P 5750 750
F 0 "#PWR025" H 5750 600 50  0001 C CNN
F 1 "+3.3V" H 5750 890 50  0000 C CNN
F 2 "" H 5750 750 50  0000 C CNN
F 3 "" H 5750 750 50  0000 C CNN
	1    5750 750 
	1    0    0    -1  
$EndComp
Text Notes 3050 2150 0    60   ~ 0
SD socket
$Comp
L C_Small C6
U 1 1 585C0E3D
P 13400 6300
F 0 "C6" H 13200 6400 50  0000 L CNN
F 1 "GRM21BR71C105KA01L" H 13250 6200 50  0001 L CNN
F 2 "Capacitors_SMD:C_0805" H 13400 6300 50  0001 C CNN
F 3 "" H 13400 6300 50  0000 C CNN
F 4 "1uF" H 13300 6200 39  0000 C CNN "Capacitance"
	1    13400 6300
	1    0    0    -1  
$EndComp
Text GLabel 11250 8400 0    39   Output ~ 0
HEATER_SWITCHED_GND
Text GLabel 5550 5400 2    39   Input ~ 0
HEATER_SWITCHED_GND
$Comp
L +12V #PWR026
U 1 1 585C9E6E
P 5750 5650
F 0 "#PWR026" H 5750 5500 50  0001 C CNN
F 1 "+12V" H 5750 5790 50  0000 C CNN
F 2 "" H 5750 5650 50  0000 C CNN
F 3 "" H 5750 5650 50  0000 C CNN
	1    5750 5650
	1    0    0    -1  
$EndComp
$Comp
L MAX31856 U5
U 1 1 5865F584
P 8050 5550
F 0 "U5" H 8550 4700 118 0000 C CNN
F 1 "MAX31856" H 8350 5650 118 0000 C CNN
F 2 "Pinecone_footprints:TSSOP-14" H 8050 5550 118 0001 C CNN
F 3 "" H 8050 5550 118 0000 C CNN
	1    8050 5550
	1    0    0    -1  
$EndComp
$Comp
L SAMD20E16A-A U1
U 1 1 586EBF33
P 12100 3100
F 0 "U1" H 11050 4500 50  0000 C CNN
F 1 "SAMD20E16A-A" H 12950 1700 50  0000 C CNN
F 2 "Housings_QFP:TQFP-32_7x7mm_Pitch0.8mm" H 12100 2100 50  0000 C CIN
F 3 "" H 12100 3100 50  0000 C CNN
	1    12100 3100
	1    0    0    -1  
$EndComp
$Comp
L L_Small FB1
U 1 1 585DD47F
P 10000 2600
F 0 "FB1" V 9900 2600 50  0000 L CNN
F 1 "Z0805C221APMST" H 10050 2750 50  0001 L CNN
F 2 "Resistors_SMD:R_0805" H 10000 2600 50  0001 C CNN
F 3 "" H 10000 2600 50  0000 C CNN
F 4 "50 mOhm" H 10000 2600 60  0001 C CNN "CD resistance"
	1    10000 2600
	-1   0    0    1   
$EndComp
$Comp
L GND #PWR027
U 1 1 586F78D2
P 10250 2500
F 0 "#PWR027" H 10250 2250 50  0001 C CNN
F 1 "GND" H 10250 2350 50  0001 C CNN
F 2 "" H 10250 2500 50  0000 C CNN
F 3 "" H 10250 2500 50  0000 C CNN
	1    10250 2500
	1    0    0    -1  
$EndComp
$Comp
L +3.3V #PWR028
U 1 1 586F84A6
P 10600 2050
F 0 "#PWR028" H 10600 1900 50  0001 C CNN
F 1 "+3.3V" H 10750 2100 50  0000 C CNN
F 2 "" H 10600 2050 50  0000 C CNN
F 3 "" H 10600 2050 50  0000 C CNN
	1    10600 2050
	1    0    0    -1  
$EndComp
$Comp
L +3.3V #PWR029
U 1 1 586F02F2
P 10000 2450
F 0 "#PWR029" H 10000 2300 50  0001 C CNN
F 1 "+3.3V" H 9800 2500 50  0000 C CNN
F 2 "" H 10000 2450 50  0000 C CNN
F 3 "" H 10000 2450 50  0000 C CNN
	1    10000 2450
	1    0    0    -1  
$EndComp
$Comp
L BS170-RESCUE-Logger_KicadProject Q2
U 1 1 586F1125
P 3600 3700
F 0 "Q2" H 3800 3775 50  0000 L CNN
F 1 "AO3420" H 3800 3700 50  0000 L CNN
F 2 "TO_SOT_Packages_SMD:SOT-23" H 3800 3625 50  0001 L CIN
F 3 "" H 3600 3700 50  0000 L CNN
	1    3600 3700
	1    0    0    -1  
$EndComp
$Comp
L BS170-RESCUE-Logger_KicadProject Q1
U 1 1 586F152B
P 11250 8800
F 0 "Q1" H 11450 8875 50  0000 L CNN
F 1 "AO3420" H 11450 8800 50  0000 L CNN
F 2 "TO_SOT_Packages_SMD:SOT-23" H 11450 8725 50  0000 L CIN
F 3 "" H 11250 8800 50  0000 L CNN
	1    11250 8800
	1    0    0    -1  
$EndComp
$Comp
L AP2281-RESCUE-Logger_KicadProject U3
U 1 1 586ED476
P 13800 6100
F 0 "U3" H 13900 5650 60  0000 C CNN
F 1 "AP2281" H 14100 6200 60  0000 C CNN
F 2 "TO_SOT_Packages_SMD:SOT-23-6" H 14250 6150 60  0001 C CNN
F 3 "" H 14250 6150 60  0001 C CNN
	1    13800 6100
	1    0    0    -1  
$EndComp
$Comp
L BARREL_JACK-RESCUE-Logger_KicadProject J1
U 1 1 586EDE2D
P 11150 7150
F 0 "J1" H 11300 6750 60  0000 C CNN
F 1 "PJ-102A" H 11200 6950 60  0000 C CNN
F 2 "Pinecone_footprints:PJ-102A" H 11450 7200 60  0001 C CNN
F 3 "" H 11450 7200 60  0001 C CNN
	1    11150 7150
	1    0    0    -1  
$EndComp
$Comp
L MIC5211_Dual_Regulator U4
U 1 1 586F1117
P 10900 6050
F 0 "U4" H 11150 5600 60  0000 C CNN
F 1 "MIC5211" H 11150 6100 60  0000 C CNN
F 2 "TO_SOT_Packages_SMD:SOT-23-6" H 11400 6100 60  0001 C CNN
F 3 "" H 11400 6100 60  0001 C CNN
	1    10900 6050
	1    0    0    -1  
$EndComp
$Comp
L Cortex_Debug_Connector_(10_pin) J2
U 1 1 586F9675
P 6000 800
F 0 "J2" H 6200 100 60  0000 C CNN
F 1 "M50-3600542" H 6050 850 60  0000 C CNN
F 2 "Pinecone_footprints:10_Pin_Cortex_SMD" H 6050 850 60  0001 C CNN
F 3 "" H 6050 850 60  0001 C CNN
	1    6000 800 
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR030
U 1 1 586FABCF
P 5650 1150
F 0 "#PWR030" H 5650 900 50  0001 C CNN
F 1 "GND" H 5650 1000 50  0000 C CNN
F 2 "" H 5650 1150 50  0000 C CNN
F 3 "" H 5650 1150 50  0000 C CNN
	1    5650 1150
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR031
U 1 1 58700A32
P 11950 6750
F 0 "#PWR031" H 11950 6500 50  0001 C CNN
F 1 "GND" H 11950 6600 50  0001 C CNN
F 2 "" H 11950 6750 50  0000 C CNN
F 3 "" H 11950 6750 50  0000 C CNN
	1    11950 6750
	1    0    0    -1  
$EndComp
$Comp
L +12V #PWR032
U 1 1 5870218B
P 11500 7000
F 0 "#PWR032" H 11500 6850 50  0001 C CNN
F 1 "+12V" H 11400 7200 50  0000 C CNN
F 2 "" H 11500 7000 50  0000 C CNN
F 3 "" H 11500 7000 50  0000 C CNN
	1    11500 7000
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR033
U 1 1 5870235C
P 11550 7350
F 0 "#PWR033" H 11550 7100 50  0001 C CNN
F 1 "GND" H 11550 7200 50  0001 C CNN
F 2 "" H 11550 7350 50  0000 C CNN
F 3 "" H 11550 7350 50  0000 C CNN
	1    11550 7350
	1    0    0    -1  
$EndComp
$Comp
L SD_Micro_Slot J7
U 1 1 58704938
P 3450 900
F 0 "J7" H 3700 -50 60  0000 C CNN
F 1 "DM3AT-SF-PEJM5" H 3450 1050 60  0000 C CNN
F 2 "Pinecone_footprints:Hirose_DM3AT" H 3450 900 60  0001 C CNN
F 3 "" H 3450 900 60  0001 C CNN
	1    3450 900 
	1    0    0    -1  
$EndComp
$Comp
L 6_pin_sdi_half J4
U 1 1 58708C19
P 2250 3000
F 0 "J4" H 2150 2250 60  0000 C CNN
F 1 "OSTVN06A150" H 2450 3100 60  0000 C CNN
F 2 "Pinecone_footprints:OSTVN06A150_6_Pin_Terminal" H 2250 3000 60  0001 C CNN
F 3 "" H 2250 3000 60  0001 C CNN
	1    2250 3000
	1    0    0    -1  
$EndComp
$Comp
L 6_pin_sdi_half J5
U 1 1 58708CDF
P 2250 3850
F 0 "J5" H 2150 3100 60  0000 C CNN
F 1 "OSTVN06A150" H 2550 3950 60  0000 C CNN
F 2 "Pinecone_footprints:OSTVN06A150_6_Pin_Terminal" H 2250 3850 60  0001 C CNN
F 3 "" H 2250 3850 60  0001 C CNN
	1    2250 3850
	1    0    0    -1  
$EndComp
$Comp
L 6_pin_dht_term J6
U 1 1 587093E8
P 5450 3050
F 0 "J6" H 5350 2300 60  0000 C CNN
F 1 "OSTVN06A150" H 5450 3050 60  0000 C CNN
F 2 "Pinecone_footprints:OSTVN06A150_6_Pin_Terminal" H 5450 3050 60  0001 C CNN
F 3 "" H 5450 3050 60  0001 C CNN
	1    5450 3050
	1    0    0    -1  
$EndComp
$Comp
L 6_pin_Dend_term J3
U 1 1 58709927
P 7400 2200
F 0 "J3" H 7300 1250 60  0000 C CNN
F 1 "OSTVN06A150" H 7400 2200 60  0000 C CNN
F 2 "Pinecone_footprints:OSTVN06A150_6_Pin_Terminal" H 7400 2200 60  0001 C CNN
F 3 "" H 7400 2200 60  0001 C CNN
	1    7400 2200
	1    0    0    -1  
$EndComp
$Comp
L 10_pin_tc_term J8
U 1 1 5870B171
P 4900 5500
F 0 "J8" H 5100 4400 60  0000 C CNN
F 1 "10_Pin_Header" H 5250 5600 60  0000 C CNN
F 2 "Pinecone_footprints:OSTVN10A150_10_pin_term" H 5250 5600 60  0001 C CNN
F 3 "" H 5250 5600 60  0001 C CNN
	1    4900 5500
	1    0    0    -1  
$EndComp
$Comp
L R_Small R2
U 1 1 58781C67
P 7650 850
F 0 "R2" H 7680 870 50  0000 L CNN
F 1 "RC0805JR-071KL" H 7680 810 50  0001 L CNN
F 2 "R_0805" H 7650 850 50  0001 C CNN
F 3 "" H 7650 850 50  0000 C CNN
F 4 "1KΩ" H 7500 850 39  0000 C CNN "Resistance"
	1    7650 850 
	1    0    0    -1  
$EndComp
$Comp
L +3.3V #PWR034
U 1 1 58782867
P 7650 700
F 0 "#PWR034" H 7650 550 50  0001 C CNN
F 1 "+3.3V" H 7650 840 50  0000 C CNN
F 2 "" H 7650 700 50  0000 C CNN
F 3 "" H 7650 700 50  0000 C CNN
	1    7650 700 
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR035
U 1 1 588A2A0F
P 2850 1150
F 0 "#PWR035" H 2850 900 50  0001 C CNN
F 1 "GND" H 2850 1000 50  0000 C CNN
F 2 "" H 2850 1150 50  0000 C CNN
F 3 "" H 2850 1150 50  0000 C CNN
	1    2850 1150
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR036
U 1 1 588A49EC
P 7150 6150
F 0 "#PWR036" H 7150 5900 50  0001 C CNN
F 1 "GND" H 7150 6000 50  0001 C CNN
F 2 "" H 7150 6150 50  0000 C CNN
F 3 "" H 7150 6150 50  0000 C CNN
	1    7150 6150
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR037
U 1 1 588A5FC8
P 7750 5600
F 0 "#PWR037" H 7750 5350 50  0001 C CNN
F 1 "GND" H 7750 5450 50  0001 C CNN
F 2 "" H 7750 5600 50  0000 C CNN
F 3 "" H 7750 5600 50  0000 C CNN
	1    7750 5600
	1    0    0    -1  
$EndComp
Text GLabel 8950 6300 2    39   Input ~ 0
SWITCHED_3v3
Text GLabel 7550 6150 3    39   Input ~ 0
SWITCHED_3v3
$Comp
L DS3231 U2
U 1 1 58B0BC60
P 14000 8350
F 0 "U2" H 13900 7350 60  0000 C CNN
F 1 "DS3231" H 14000 8450 60  0000 C CNN
F 2 "Housings_SOIC:SOIC-16W_7.5x10.3mm_Pitch1.27mm" H 14000 8350 60  0001 C CNN
F 3 "" H 14000 8350 60  0000 C CNN
	1    14000 8350
	1    0    0    -1  
$EndComp
$Comp
L C_Small C4
U 1 1 58B0D767
P 13450 8800
F 0 "C4" H 13300 8700 50  0000 L CNN
F 1 "C0805C104K5RACTU" V 13550 8700 50  0001 L CNN
F 2 "Capacitors_SMD:C_0805" H 13450 8800 50  0001 C CNN
F 3 "" H 13450 8800 50  0000 C CNN
F 4 ".1uF" H 13300 8800 39  0000 C CNN "Capacitance"
	1    13450 8800
	1    0    0    -1  
$EndComp
Text GLabel 13350 4150 2    39   Input ~ 0
DS3231_INTERRUPT
Text GLabel 13150 8650 0    39   Output ~ 0
DS3231_INTERRUPT
Text GLabel 5850 3050 1    39   Output ~ 0
SWITCHED_3v3
$Comp
L R_Small R9
U 1 1 585B66B5
P 6050 3500
F 0 "R9" V 6100 3600 50  0000 L CNN
F 1 "RC0805JR-0710KL" H 5950 3650 50  0001 L CNN
F 2 "Resistors_SMD:R_0805" H 6050 3500 50  0001 C CNN
F 3 "" H 6050 3500 50  0000 C CNN
F 4 "10KΩ" V 6000 3700 39  0000 C CNN "Resistance"
	1    6050 3500
	0    1    1    0   
$EndComp
$Comp
L R_Small R5
U 1 1 58B360DF
P 14650 8300
F 0 "R5" H 14500 8400 50  0000 L CNN
F 1 "RC0805JR-0710KL" H 14750 8250 50  0001 L CNN
F 2 "Resistors_SMD:R_0805" H 14650 8300 50  0001 C CNN
F 3 "" H 14650 8300 50  0000 C CNN
F 4 "10KΩ" H 14500 8300 39  0000 C CNN "Resistance"
	1    14650 8300
	1    0    0    -1  
$EndComp
$Comp
L R_Small R3
U 1 1 58B36667
P 13200 8450
F 0 "R3" H 13250 8500 50  0000 L CNN
F 1 "RC0805JR-0710KL" H 13300 8400 50  0001 L CNN
F 2 "Resistors_SMD:R_0805" H 13200 8450 50  0001 C CNN
F 3 "" H 13200 8450 50  0000 C CNN
F 4 "10KΩ" H 13350 8400 39  0000 C CNN "Resistance"
	1    13200 8450
	1    0    0    -1  
$EndComp
$Comp
L R_Small R4
U 1 1 58B36AD2
P 14800 8300
F 0 "R4" H 14850 8350 50  0000 L CNN
F 1 "RC0805JR-0710KL" H 14900 8250 50  0001 L CNN
F 2 "Resistors_SMD:R_0805" H 14800 8300 50  0001 C CNN
F 3 "" H 14800 8300 50  0000 C CNN
F 4 "10KΩ" H 14950 8250 39  0000 C CNN "Resistance"
	1    14800 8300
	1    0    0    -1  
$EndComp
$Comp
L +3V3 #PWR038
U 1 1 58B37BB0
P 14700 8150
F 0 "#PWR038" H 14700 8000 50  0001 C CNN
F 1 "+3V3" H 14700 8300 50  0000 C CNN
F 2 "" H 14700 8150 50  0000 C CNN
F 3 "" H 14700 8150 50  0000 C CNN
	1    14700 8150
	1    0    0    -1  
$EndComp
Wire Wire Line
	14900 8650 14600 8650
Wire Wire Line
	10000 2750 10850 2750
Connection ~ 6150 3400
Wire Wire Line
	6150 3400 6100 3400
Wire Wire Line
	6150 3200 6150 3500
Connection ~ 5850 3400
Wire Wire Line
	5850 3650 5850 3400
Wire Wire Line
	5900 3650 5850 3650
Wire Wire Line
	5700 3500 5950 3500
Connection ~ 5800 3500
Wire Wire Line
	5800 3800 5800 3500
Wire Wire Line
	5800 3800 5900 3800
Connection ~ 5850 3200
Wire Wire Line
	5700 3400 5900 3400
Wire Wire Line
	5850 3050 5850 3300
Wire Wire Line
	13450 8900 13450 8950
Wire Wire Line
	13550 8850 13550 9150
Connection ~ 13550 8550
Wire Wire Line
	13450 8550 13450 8700
Wire Wire Line
	13150 8650 13650 8650
Connection ~ 14700 9150
Connection ~ 14700 9050
Wire Wire Line
	14700 9150 14600 9150
Connection ~ 14700 8950
Wire Wire Line
	14700 9050 14600 9050
Connection ~ 14700 8850
Wire Wire Line
	14600 8950 14900 8950
Wire Wire Line
	14700 8850 14600 8850
Wire Wire Line
	14700 8750 14700 9200
Wire Wire Line
	14600 8750 14700 8750
Connection ~ 13600 9150
Wire Wire Line
	13600 9150 13600 9200
Connection ~ 13550 9050
Wire Wire Line
	13550 9150 13650 9150
Connection ~ 13550 8950
Wire Wire Line
	13550 9050 13650 9050
Wire Wire Line
	13450 8950 13650 8950
Wire Wire Line
	13550 8850 13650 8850
Wire Wire Line
	13550 8550 13550 8450
Wire Wire Line
	13450 8550 13650 8550
Wire Wire Line
	14600 8450 14850 8450
Wire Wire Line
	14600 8550 14850 8550
Wire Wire Line
	7550 6050 7550 6150
Connection ~ 8850 6300
Wire Wire Line
	8850 6300 8950 6300
Wire Wire Line
	14550 6200 14700 6200
Wire Wire Line
	14550 6300 14700 6300
Wire Wire Line
	6800 6450 6800 6500
Wire Wire Line
	6700 6450 6800 6450
Wire Wire Line
	10600 6300 10600 6350
Wire Wire Line
	10700 6300 10600 6300
Wire Wire Line
	8950 5650 8950 5700
Wire Wire Line
	8850 5650 8950 5650
Wire Wire Line
	7850 5600 7850 5650
Wire Wire Line
	7750 5600 7850 5600
Wire Wire Line
	7750 6050 7750 6100
Connection ~ 7700 5800
Wire Wire Line
	7850 5750 7700 5750
Connection ~ 7750 6050
Wire Wire Line
	7550 6050 7850 6050
Wire Wire Line
	7150 6050 7150 6150
Wire Wire Line
	6700 6050 7150 6050
Wire Wire Line
	2850 1050 2850 1150
Wire Wire Line
	3000 1050 2850 1050
Wire Wire Line
	2850 950  3000 950 
Wire Wire Line
	3900 1450 3900 1500
Wire Wire Line
	3850 1450 3900 1450
Wire Wire Line
	3950 1350 3850 1350
Wire Wire Line
	3850 1250 3950 1250
Wire Wire Line
	3950 1150 3850 1150
Wire Wire Line
	3850 1050 3950 1050
Wire Wire Line
	4100 800  4250 800 
Connection ~ 3900 950 
Wire Wire Line
	3900 950  3900 800 
Wire Wire Line
	3850 950  3950 950 
Connection ~ 5650 1100
Wire Wire Line
	5800 1000 5650 1000
Wire Wire Line
	5650 1000 5650 1150
Wire Wire Line
	5800 1100 5650 1100
Wire Wire Line
	7000 1300 6850 1300
Wire Wire Line
	5750 900  5800 900 
Wire Wire Line
	10450 6200 10450 6300
Wire Wire Line
	10700 6200 10450 6200
Connection ~ 12050 6300
Wire Wire Line
	12050 6300 12050 6450
Wire Wire Line
	12200 6300 12200 6350
Wire Wire Line
	11800 6300 12200 6300
Connection ~ 11850 6100
Wire Wire Line
	11850 6100 11850 6450
Wire Wire Line
	11800 6100 11900 6100
Wire Wire Line
	11900 6100 11900 6000
Wire Wire Line
	10600 6100 10700 6100
Wire Wire Line
	10600 6000 10600 6100
Wire Wire Line
	12100 6200 11800 6200
Wire Wire Line
	12100 6000 12100 6200
Wire Wire Line
	11550 7250 11550 7350
Wire Wire Line
	11450 7250 11550 7250
Wire Wire Line
	5750 900  5750 750 
Wire Wire Line
	7650 750  7650 700 
Connection ~ 7650 1000
Wire Wire Line
	7650 950  7650 1000
Wire Wire Line
	7750 6350 7750 6300
Connection ~ 7650 5650
Wire Wire Line
	7650 5300 7650 5650
Wire Wire Line
	5550 5550 5550 5400
Wire Wire Line
	5500 5550 5550 5550
Wire Wire Line
	5800 6450 5500 6450
Wire Wire Line
	5500 6350 5800 6350
Wire Wire Line
	5800 6250 5500 6250
Wire Wire Line
	5500 6150 5800 6150
Wire Wire Line
	5800 6050 5500 6050
Wire Wire Line
	5500 5950 5800 5950
Wire Wire Line
	5800 5850 5500 5850
Wire Wire Line
	5800 5750 5500 5750
Wire Wire Line
	5750 5650 5500 5650
Wire Wire Line
	11500 7050 11500 7000
Wire Wire Line
	11450 7050 11500 7050
Connection ~ 11950 6650
Wire Wire Line
	11950 6650 11950 6750
Wire Wire Line
	11850 6650 12050 6650
Wire Wire Line
	6850 900  7150 900 
Wire Wire Line
	6850 1000 7800 1000
Connection ~ 13550 6400
Wire Wire Line
	13400 6400 13600 6400
Wire Wire Line
	10000 2500 10000 2450
Wire Wire Line
	10000 2750 10000 2700
Wire Wire Line
	10600 2750 10600 3050
Wire Wire Line
	10600 2050 10600 2250
Wire Wire Line
	10500 2400 10850 2400
Wire Wire Line
	10250 2400 10300 2400
Wire Wire Line
	10500 2250 10850 2250
Wire Wire Line
	10500 2100 10600 2100
Connection ~ 10700 1850
Wire Wire Line
	10700 1850 10700 1800
Wire Wire Line
	10650 1850 10850 1850
Wire Wire Line
	10700 1550 10700 1600
Connection ~ 10250 2250
Wire Wire Line
	10300 2250 10250 2250
Connection ~ 10250 2400
Wire Wire Line
	10250 2100 10250 2500
Wire Wire Line
	10300 2100 10250 2100
Connection ~ 10600 2100
Connection ~ 10600 2250
Connection ~ 10600 2750
Wire Wire Line
	10350 2900 10300 2900
Wire Wire Line
	10300 2900 10300 3150
Connection ~ 10300 3050
Wire Wire Line
	10350 3050 10300 3050
Wire Wire Line
	10600 2900 10550 2900
Wire Wire Line
	10600 3050 10550 3050
Connection ~ 10600 2900
Connection ~ 10800 4350
Wire Wire Line
	10850 4250 10800 4250
Wire Wire Line
	10800 4250 10800 4400
Wire Wire Line
	10850 4350 10800 4350
Connection ~ 3250 3450
Connection ~ 2950 3450
Wire Wire Line
	3700 3450 3700 3500
Wire Wire Line
	2950 3450 3700 3450
Wire Wire Line
	3250 3400 3250 3450
Connection ~ 3250 3150
Wire Wire Line
	3250 3150 3250 3200
Wire Wire Line
	3000 3050 2800 3050
Wire Wire Line
	2800 3350 2950 3350
Connection ~ 3000 3150
Connection ~ 3000 3900
Wire Wire Line
	2800 3150 3350 3150
Wire Wire Line
	3000 4000 2800 4000
Wire Wire Line
	3000 3050 3000 4000
Wire Wire Line
	2800 3900 3000 3900
Connection ~ 2950 3350
Wire Wire Line
	2950 3250 2800 3250
Connection ~ 2950 4100
Wire Wire Line
	2950 4100 2800 4100
Wire Wire Line
	2950 3250 2950 4200
Wire Wire Line
	2950 4200 2800 4200
Connection ~ 2900 4400
Wire Wire Line
	2900 4400 2800 4400
Connection ~ 2900 4300
Wire Wire Line
	2900 4300 2800 4300
Connection ~ 2900 3550
Wire Wire Line
	2800 3450 2900 3450
Wire Wire Line
	2900 3450 2900 4450
Wire Wire Line
	2800 3550 2900 3550
Wire Wire Line
	6850 5450 6850 5750
Wire Notes Line
	4750 7500 10350 7500
Wire Notes Line
	4750 5000 4750 7500
Connection ~ 7900 2950
Wire Wire Line
	7800 2950 7900 2950
Wire Wire Line
	7900 2850 7900 3100
Wire Wire Line
	7800 2850 7900 2850
Wire Wire Line
	7800 2650 8300 2650
Wire Wire Line
	7800 2750 8300 2750
Connection ~ 7900 2450
Wire Wire Line
	7800 2450 7900 2450
Wire Wire Line
	7900 2250 7900 2550
Wire Wire Line
	7900 2550 7800 2550
Wire Wire Line
	5700 3200 6150 3200
Wire Wire Line
	5850 3300 5700 3300
Connection ~ 5750 3700
Wire Wire Line
	5700 3700 5750 3700
Wire Wire Line
	5750 3600 5750 3800
Wire Wire Line
	5700 3600 5750 3600
Wire Wire Line
	11350 8400 11350 8600
Wire Wire Line
	11250 8400 11350 8400
Wire Wire Line
	14650 6600 14550 6600
Wire Wire Line
	14650 6550 14650 6600
Wire Wire Line
	14650 6200 14650 6350
Connection ~ 13500 6200
Wire Wire Line
	13400 6200 13600 6200
Wire Wire Line
	13500 6100 13500 6300
Wire Wire Line
	13500 6300 13600 6300
Connection ~ 14650 6200
Connection ~ 14550 6600
Wire Wire Line
	14550 6400 14550 6700
Wire Wire Line
	8850 6250 8850 6350
Connection ~ 6850 5600
Wire Wire Line
	7150 5600 7150 5700
Wire Wire Line
	7100 5600 7150 5600
Wire Wire Line
	6850 5600 6900 5600
Wire Notes Line
	10350 9500 10350 5550
Wire Notes Line
	10350 7950 15950 7950
Wire Notes Line
	15950 9500 10350 9500
Wire Notes Line
	12500 5550 12500 9500
Wire Notes Line
	2100 2450 5000 2450
Wire Notes Line
	2100 500  2100 5000
Wire Notes Line
	7050 2000 7050 5000
Wire Notes Line
	5000 2000 9000 2000
Wire Notes Line
	2100 5000 9000 5000
Wire Notes Line
	5000 500  5000 5000
Wire Notes Line
	15950 500  15950 9500
Wire Notes Line
	9000 5550 15950 5550
Wire Notes Line
	9000 500  9000 5550
Wire Notes Line
	2100 500  15950 500 
Wire Wire Line
	11000 8850 11050 8850
Wire Wire Line
	11350 9000 11350 9100
Connection ~ 3400 4000
Wire Wire Line
	3400 3750 3400 4100
Wire Wire Line
	3450 4000 3400 4000
Wire Wire Line
	3700 4000 3650 4000
Wire Wire Line
	7700 5950 7850 5950
Wire Wire Line
	7700 6000 7700 5950
Wire Wire Line
	7350 6000 7700 6000
Wire Wire Line
	7700 5850 7850 5850
Wire Wire Line
	7700 5750 7700 5850
Wire Wire Line
	7350 5800 7700 5800
Wire Wire Line
	6700 5850 7350 5850
Wire Wire Line
	7350 5850 7350 5800
Connection ~ 7500 5800
Wire Wire Line
	6700 5950 7350 5950
Wire Wire Line
	7350 5950 7350 6000
Connection ~ 7500 6000
Wire Wire Line
	6850 5750 6700 5750
Wire Wire Line
	3700 3900 3700 4000
Wire Wire Line
	3700 3950 3800 3950
Connection ~ 3700 3950
Wire Wire Line
	13550 6400 13550 6500
Wire Wire Line
	14650 8400 14650 8450
Connection ~ 14650 8450
Wire Wire Line
	14800 8400 14800 8550
Connection ~ 14800 8550
Wire Wire Line
	14650 8200 14800 8200
Wire Wire Line
	14700 8200 14700 8150
Connection ~ 14700 8200
Wire Wire Line
	13200 8550 13200 8650
Connection ~ 13200 8650
$Comp
L +3V3 #PWR039
U 1 1 58B38D78
P 13200 8300
F 0 "#PWR039" H 13200 8150 50  0001 C CNN
F 1 "+3V3" H 13200 8450 50  0000 C CNN
F 2 "" H 13200 8300 50  0000 C CNN
F 3 "" H 13200 8300 50  0000 C CNN
	1    13200 8300
	1    0    0    -1  
$EndComp
Wire Wire Line
	13200 8300 13200 8350
$Comp
L C_Small C16
U 1 1 58B9D99C
P 8000 2950
F 0 "C16" H 7850 3100 50  0000 L CNN
F 1 "C0805C104K5RACTU" V 8000 3100 50  0001 L CNN
F 2 "Capacitors_SMD:C_0805" H 8000 2950 50  0001 C CNN
F 3 "" H 8000 2950 50  0000 C CNN
F 4 ".1uF" H 8050 3050 39  0000 C CNN "Capacitance"
	1    8000 2950
	1    0    0    -1  
$EndComp
$Comp
L C_Small C17
U 1 1 58B9DF9A
P 8150 2950
F 0 "C17" H 8250 2950 50  0000 L CNN
F 1 "C0805C104K5RACTU" V 8150 3100 50  0001 L CNN
F 2 "Capacitors_SMD:C_0805" H 8150 2950 50  0001 C CNN
F 3 "" H 8150 2950 50  0000 C CNN
F 4 ".1uF" H 8250 3050 39  0000 C CNN "Capacitance"
	1    8150 2950
	1    0    0    -1  
$EndComp
Connection ~ 8000 2750
Connection ~ 8150 2650
Wire Wire Line
	7900 2250 8200 2250
Connection ~ 8150 2250
Connection ~ 8000 2250
$Comp
L C_Small C18
U 1 1 58BA3AE6
P 7400 6500
F 0 "C18" H 7450 6300 50  0000 L CNN
F 1 "C0805C103K5RACTU" H 7410 6420 50  0001 L CNN
F 2 "Capacitors_SMD:C_0805" H 7400 6500 50  0001 C CNN
F 3 "" H 7400 6500 50  0000 C CNN
F 4 ".01uF" H 7500 6200 39  0000 C CNN "Capacitance"
	1    7400 6500
	1    0    0    -1  
$EndComp
$Comp
L C_Small C19
U 1 1 58BA3C69
P 7250 6500
F 0 "C19" H 7050 6300 50  0000 L CNN
F 1 "C0805C103K5RACTU" H 7260 6420 50  0001 L CNN
F 2 "Capacitors_SMD:C_0805" H 7250 6500 50  0001 C CNN
F 3 "" H 7250 6500 50  0000 C CNN
F 4 ".01uF" H 7100 6400 39  0000 C CNN "Capacitance"
	1    7250 6500
	1    0    0    -1  
$EndComp
Wire Wire Line
	7400 6000 7400 6400
Connection ~ 7400 6000
Wire Wire Line
	7250 6400 7250 5850
Connection ~ 7250 5850
$Comp
L GND #PWR040
U 1 1 58BA4624
P 7400 6650
F 0 "#PWR040" H 7400 6400 50  0001 C CNN
F 1 "GND" H 7400 6500 50  0001 C CNN
F 2 "" H 7400 6650 50  0000 C CNN
F 3 "" H 7400 6650 50  0000 C CNN
	1    7400 6650
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR041
U 1 1 58BA46B9
P 7250 6650
F 0 "#PWR041" H 7250 6400 50  0001 C CNN
F 1 "GND" H 7250 6500 50  0001 C CNN
F 2 "" H 7250 6650 50  0000 C CNN
F 3 "" H 7250 6650 50  0000 C CNN
	1    7250 6650
	1    0    0    -1  
$EndComp
Wire Wire Line
	7250 6600 7250 6650
Wire Wire Line
	7400 6600 7400 6650
Text GLabel 13350 1850 2    39   Output ~ 0
STATUS_LED
Text GLabel 10100 3700 2    39   Input ~ 0
STATUS_LED
$Comp
L LED D1
U 1 1 58BDB7E6
P 9450 3700
F 0 "D1" H 9450 3800 50  0000 C CNN
F 1 "LY R976-PS-36" H 9450 3900 50  0000 C CNN
F 2 "LEDs:LED_0805" H 9450 3700 50  0001 C CNN
F 3 "" H 9450 3700 50  0000 C CNN
	1    9450 3700
	1    0    0    -1  
$EndComp
Wire Wire Line
	9900 3700 10100 3700
$Comp
L R_Small R10
U 1 1 58BDBC1A
P 9800 3700
F 0 "R10" V 9900 3650 50  0000 L CNN
F 1 "RC0805JR-0768RL" H 9830 3660 50  0001 L CNN
F 2 "Resistors_SMD:R_0805" H 9800 3700 50  0001 C CNN
F 3 "" H 9800 3700 50  0000 C CNN
F 4 "68 Ohm" H 9800 3700 60  0001 C CNN "Resistance"
	1    9800 3700
	0    1    1    0   
$EndComp
$Comp
L GND #PWR042
U 1 1 58BDBEB8
P 9250 3800
F 0 "#PWR042" H 9250 3550 50  0001 C CNN
F 1 "GND" H 9250 3650 50  0001 C CNN
F 2 "" H 9250 3800 50  0000 C CNN
F 3 "" H 9250 3800 50  0000 C CNN
	1    9250 3800
	1    0    0    -1  
$EndComp
Wire Wire Line
	9250 3700 9250 3800
Wire Wire Line
	9700 3700 9650 3700
Wire Wire Line
	8000 2750 8000 2850
Wire Wire Line
	8150 2650 8150 2850
Wire Wire Line
	7900 3050 8150 3050
Connection ~ 8000 3050
Connection ~ 7900 3050
$EndSCHEMATC

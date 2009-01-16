'8 servo sturing
Define CONF_WORD = 0x3f3a

Dim motornumber As Byte
Dim expander_out As Byte
Dim expander_adres As Byte
Dim speed(9) As Byte  'indicates speed of servomotor
Dim angle(9) As Byte
Dim midden(9) As Byte
Dim direction(9) As Byte  'this is position of the relay
Dim servo(9) As Byte
Dim position(9) As Byte  'actual turnout position
Dim position_asked(9) As Byte
Dim angle_left(9) As Byte  'turning angle of servomotor
Dim angle_right(9) As Byte
Dim misc_value As Byte
Dim dummy As Byte
Dim dummy1 As Byte
Dim adjust As Bit
Dim frog_left As Byte
Dim frog_right As Byte
Dim temp1 As Byte
Dim temp As Byte

Symbol servopower = PORTA.5

ADCON1 = 4
TRISA = %01011  'ra0,ra1 ana in,ra2 sda, ra3 in,ra4 scl,ra5 out
TRISB = %00000000  '8 uitgangen
TRISC = %11111111  '8 ingangen
PORTB = %11111111
PORTA = %111000

servopower = 1

Symbol scl = PORTA.4  'I2c Initialisation
Symbol sda = PORTA.2
I2CPrepare sda, scl  'I2c Initialisation

Gosub read_eeprom  'read all settings

adjust = 0
expander_out = 0

For motornumber = 1 To 8
	If position(motornumber) = 0 Then
	angle(motornumber) = angle_left(motornumber)
	Else
	angle(motornumber) = angle_right(motornumber)
	Endif
Next motornumber

For dummy = 1 To 20
	ServoOut PORTB.0, angle(1)
	ServoOut PORTB.1, angle(2)
	ServoOut PORTB.2, angle(3)
	ServoOut PORTB.3, angle(4)
	ServoOut PORTB.4, angle(5)
	ServoOut PORTB.5, angle(6)
	ServoOut PORTB.6, angle(7)
	ServoOut PORTB.7, angle(8)
	If dummy = 5 Then servopower = 0  'switch power to servo's
	WaitMs 20
Next dummy

Adcin 3, temp  'PORTA.3 = 0 Then  'Adjustments activated
If temp < 128 Then Goto adjustments


main:

Gosub getinputs  'get input commands

For motornumber = 1 To 8
temp = motornumber
If temp > 4 Then temp = temp - 4
	frog_left = LookUp(0, %11111110, %11111011, %11101111, %10111111), temp
	frog_right = LookUp(0, %11111101, %11110111, %11011111, %01111111), temp
		If position_asked(motornumber) = 0 Then
			If position(motornumber) = 1 Then Gosub servo_control_left
			Else
			If position(motornumber) = 0 Then Gosub servo_control_right
		Endif
Next motornumber

WaitMs 250
Goto main
End                                               
measure_relay_postion:
Adcin 3, temp  'RA3

If temp < 128 Then
	direction(motornumber) = 0
Else
	direction(motornumber) = 1
Endif
dummy = 24 + motornumber  'adress in EEprom
Write dummy, direction(motornumber)
Return                                            


measure_midden:

Adcin 0, misc_value  'midden 64--245
If misc_value < 55 Then misc_value = 55
If misc_value > 195 Then misc_value = 195
midden(motornumber) = misc_value
dummy = 32 + motornumber  'adress in EEprom
Write dummy, midden(motornumber)
Return                                            


measure_angle:

Adcin 1, misc_value

misc_value = misc_value / 5  'hoek 0--51



angle_left(motornumber) = midden(motornumber) - misc_value
angle_right(motornumber) = midden(motornumber) + misc_value




dummy = motornumber + 8  'adress in EEprom
Write dummy, angle_left(motornumber)
dummy = dummy + 8  'adress in EEprom
Write dummy, angle_right(motornumber)
Return                                            
adjustments:

Gosub getinputs
For motornumber = 1 To 8
temp = motornumber
If temp > 4 Then temp = temp - 4
	If position_asked(motornumber) = 0 Then
		frog_left = LookUp(0, %11111110, %11111011, %11101111, %10111111), temp
		frog_right = LookUp(0, %11111101, %11110111, %11011111, %01111111), temp
		Gosub measure_relay_postion
'Gosub measure_speed
		Gosub measure_midden
		Gosub measure_angle
		Gosub servo_control_left
		WaitMs 1000
		Gosub servo_control_right
		WaitMs 1000
	Endif
Next motornumber
Goto adjustments
Return                                            
read_eeprom:

For dummy = 1 To 8
Read dummy, position(dummy)
If position(dummy) = 255 Then position(dummy) = 0
Next dummy

For dummy = 9 To 16
misc_value = dummy - 8
Read dummy, angle_left(misc_value)
If angle_left(misc_value) = 255 Then angle_left(misc_value) = 64
Next dummy

For dummy = 17 To 24
misc_value = dummy - 16
Read dummy, angle_right(misc_value)
If angle_right(misc_value) = 255 Then angle_right(misc_value) = 245
Next dummy

For dummy = 25 To 32
misc_value = dummy - 24
Read dummy, direction(misc_value)
If direction(misc_value) = 255 Then direction(misc_value) = 0
Next dummy

For dummy = 33 To 40
misc_value = dummy - 32
Read dummy, midden(misc_value)
If midden(misc_value) = 255 Then midden(misc_value) = 150
Next dummy

Return                                            
servo_control_left:

misc_value = angle_right(motornumber) - 5
While misc_value > angle_left(motornumber)
	If misc_value = midden(motornumber) Then  'change frog polarity in middle position
		If direction(motornumber) = 1 Then
			expander_out = frog_left
		Else
			expander_out = frog_right
		Endif
		Gosub writeoutputs
	Endif

	Select Case motornumber
	Case 1
		ServoOut PORTB.0, misc_value
	Case 2
		ServoOut PORTB.1, misc_value
	Case 3
		ServoOut PORTB.2, misc_value
	Case 4
		ServoOut PORTB.3, misc_value
	Case 5
		ServoOut PORTB.4, misc_value
	Case 6
		ServoOut PORTB.5, misc_value
	Case 7
		ServoOut PORTB.6, misc_value
	Case Else
		ServoOut PORTB.7, misc_value
	EndSelect
	WaitMs 30  'speed(motornumber)
	misc_value = misc_value - 1
Wend
expander_out = %11111111
position(motornumber) = 0
Gosub writeoutputs
Write motornumber, 0  'Position saved
Return                                            
servo_control_right:

misc_value = angle_left(motornumber) + 5
While misc_value < angle_right(motornumber)
	If misc_value = midden(motornumber) Then  'change frog polarity in middle position
		If direction(motornumber) = 1 Then
			expander_out = frog_right
		Else
			expander_out = frog_left
		Endif
		Gosub writeoutputs
	Endif

	Select Case motornumber
	Case 1
		ServoOut PORTB.0, misc_value
	Case 2
		ServoOut PORTB.1, misc_value
	Case 3
		ServoOut PORTB.2, misc_value
	Case 4
		ServoOut PORTB.3, misc_value
	Case 5
		ServoOut PORTB.4, misc_value
	Case 6
		ServoOut PORTB.5, misc_value
	Case 7
		ServoOut PORTB.6, misc_value
	Case Else
		ServoOut PORTB.7, misc_value
	EndSelect

	WaitMs 30  'speed(motornumber)
	misc_value = misc_value + 1
Wend
expander_out = %11111111
position(motornumber) = 1
Gosub writeoutputs
Write motornumber, 1  'Position saved
Return                                            
getinputs:

dummy = PORTC

dummy1 = dummy
dummy1 = dummy1 And 128
If dummy1 > 0 Then
	position_asked(1) = 1
Else
	position_asked(1) = 0
Endif

dummy1 = dummy
dummy1 = dummy1 And 64
If dummy1 > 0 Then
	position_asked(2) = 1
Else
	position_asked(2) = 0
Endif

dummy1 = dummy
dummy1 = dummy1 And 32
If dummy1 > 0 Then
	position_asked(3) = 1
Else
	position_asked(3) = 0
Endif

dummy1 = dummy
dummy1 = dummy1 And 16
If dummy1 > 0 Then
	position_asked(4) = 1
Else
	position_asked(4) = 0
Endif

dummy1 = dummy
dummy1 = dummy1 And 8
If dummy1 > 0 Then
	position_asked(5) = 1
Else
	position_asked(5) = 0
Endif

dummy1 = dummy
dummy1 = dummy1 And 4
If dummy1 > 0 Then
	position_asked(6) = 1
Else
	position_asked(6) = 0
Endif

dummy1 = dummy
dummy1 = dummy1 And 2
If dummy1 > 0 Then
	position_asked(7) = 1
Else
	position_asked(7) = 0
Endif

dummy1 = dummy
dummy1 = dummy1 And 1
If dummy1 > 0 Then
	position_asked(8) = 1
Else
	position_asked(8) = 0
Endif

Return                                            
writeoutputs:

If motornumber > 4 Then
	expander_adres = 120
Else
	expander_adres = 116
Endif

I2CStart  'Issue I2C start signal
I2CSend expander_adres  'send adress u4
I2CSend expander_out  'Send data to U4
I2CStop  'Issue stop

Return                                            
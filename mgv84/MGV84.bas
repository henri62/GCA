'program for control of 4 servomotors + 4 relais to switch turnout frogs.
'new version also provides feed back of servomotor position after movement.
'print MGV84
'with jumper JP1 set, the program gets into adjust mode.
'this enables adjust of:
'-  change polarity to turnout frog
'-  working angle of servo
'-  speed of servo.
Dim motornumber As Byte
Dim misc_value As Byte
Dim dummy As Byte
Dim dummy1 As Byte
Dim adjust As Bit
Dim frog_left As Byte
Dim frog_right As Byte
Dim servo(5) As Byte
Dim position(5) As Byte  'actual turnout position
Dim position_asked(5) As Byte
Dim angle_left(5) As Byte  'turning angle of servomotor
Dim angle_right(5) As Byte
Dim angle(5) As Byte
Dim direction(5) As Byte  'this is position of the relay
Dim speed(5) As Byte  'indicates speed of servomotor
Dim expander_out As Byte
Symbol servopower = PORTC.2
ADCON1 = 0
TRISA = %111111  'set 6 PORTA pins as inputs and +/-Vref
TRISB = %00000000  'set portb as output
PORTB = %11111111
TRISC = %00001011  'set portc.0,1,3 as input , rest is output
PORTC = %00001111
Symbol sda = PORTC.1  'I2c Initialisation
Symbol scl = PORTC.0  'I2c Initialisation
I2CPrepare sda, scl  'I2c Initialisation
Gosub read_eeprom  'read all settings
adjust = 0
expander_out = 0
For motornumber = 1 To 4
	Gosub writeoutputs
	If position(motornumber) = 0 Then
		angle(motornumber) = angle_left(motornumber)
	Else
		angle(motornumber) = angle_right(motornumber)
	Endif
Next motornumber
For dummy = 1 To 20
	ServoOut PORTC.4, angle(1)
	ServoOut PORTC.5, angle(2)
	ServoOut PORTC.6, angle(3)
	ServoOut PORTC.7, angle(4)
	If dummy = 5 Then
		servopower = 0  'switch power to servo's
	Endif
	WaitMs 20
Next dummy
If PORTC.3 = 0 Then  'Adjustments activated
	adjust = 1
	Goto adjustments
Endif
main:
	Gosub getinputs  'get input commands
	For motornumber = 1 To 4
		frog_left = LookUp(0, %11111110, %11111011, %11101111, %10111111), motornumber
		frog_right = LookUp(0, %11111101, %11110111, %11011111, %01111111), motornumber
		If position_asked(motornumber) = 0 Then
			If position(motornumber) = 1 Then  'Turnout to be changed
				Gosub servo_control_left
			Endif
		Else
			If position(motornumber) = 0 Then  'Turnout to be changed
				Gosub servo_control_right
			Endif
		Endif
	Next motornumber
	WaitMs 250
Goto main
End                                               

measure_relay_postion:
	If PORTC.3 = 0 Then
		direction(motornumber) = 0
	Else
		direction(motornumber) = 1
	Endif
	dummy = 12 + motornumber  'adress in EEprom
	Write dummy, direction(motornumber)
Return                                            

measure_speed:
	Adcin 0, misc_value
	If misc_value = 0 Then  'zero not allowed
		misc_value = 1
	Endif
	speed(motornumber) = misc_value
	dummy = 16 + motornumber  'adress in EEprom
	Write dummy, speed(motornumber)
Return                                            

measure_angle:
	Adcin 1, misc_value
	If misc_value < 8 Then
		misc_value = 8
	Endif
	misc_value = misc_value / 2
	angle_left(motornumber) = 128 - misc_value
	If angle_left(motornumber) < 55 Then
		angle_left(motornumber) = 55
	Endif
	angle_right(motornumber) = 127 + misc_value
	If angle_right(motornumber) > 245 Then
		angle_right(motornumber) = 245
	Endif
	dummy = motornumber + 4  'adress in EEprom
	Write dummy, angle_left(motornumber)
	dummy = dummy + 4  'adress in EEprom
	Write dummy, angle_right(motornumber)
Return                                            

adjustments:
	Gosub getinputs
	For motornumber = 1 To 4
		If position_asked(motornumber) = 0 Then
			frog_left = LookUp(0, %11111110, %11111011, %11101111, %10111111), motornumber
			frog_right = LookUp(0, %11111101, %11110111, %11011111, %01111111), motornumber
			Gosub measure_relay_postion
			Gosub measure_speed
			Gosub measure_angle
			Gosub servo_control_left
			WaitMs 1000
			Gosub servo_control_right
			WaitMs 1000
		Endif
	Next motornumber
Goto adjustments

read_eeprom:
For dummy = 1 To 4
	Read dummy, position(dummy)
	If position(dummy) = 255 Then
		position(dummy) = 0
	Endif
Next dummy
For dummy = 5 To 8
	misc_value = dummy - 4
	Read dummy, angle_left(misc_value)
	If angle_left(misc_value) = 255 Then
		angle_left(misc_value) = 60
	Endif
Next dummy
For dummy = 9 To 12
	misc_value = dummy - 8
	Read dummy, angle_right(misc_value)
	If angle_right(misc_value) = 255 Then
		angle_right(misc_value) = 200
	Endif
Next dummy
For dummy = 13 To 16
	misc_value = dummy - 12
	Read dummy, direction(misc_value)
	If direction(misc_value) = 255 Then
		direction(misc_value) = 0
	Endif
Next dummy
For dummy = 17 To 20
	misc_value = dummy - 16
	Read dummy, speed(misc_value)
	If speed(misc_value) = 255 Then
		speed(misc_value) = 20
	Endif
Next dummy
Return                                            

servo_control_left:
			misc_value = angle_right(motornumber) - 1
			While misc_value > angle_left(motornumber)
				If misc_value = 128 Then  'change frog polarity in middle position
					If direction(motornumber) = 1 Then
						PORTB = frog_left
					Else
						PORTB = frog_right
					Endif
				Endif
				Select Case motornumber
				Case 1
					ServoOut PORTC.4, misc_value
				Case 2
					ServoOut PORTC.5, misc_value
				Case 3
					ServoOut PORTC.6, misc_value
				Case Else
					ServoOut PORTC.7, misc_value
				EndSelect
				WaitMs speed(motornumber)
				misc_value = misc_value - 1
			Wend
			PORTB = %11111111
			position(motornumber) = 0
			Gosub writeoutputs
			Write motornumber, 0  'Position saved
Return                                            
servo_control_right:
			misc_value = angle_left(motornumber) + 1
			While misc_value < angle_right(motornumber)
				If misc_value = 128 Then  'change frog polarity in middle position
					If direction(motornumber) = 1 Then
						PORTB = frog_right
					Else
						PORTB = frog_left
					Endif
				Endif
				Select Case motornumber
				Case 1
					ServoOut PORTC.4, misc_value
				Case 2
					ServoOut PORTC.5, misc_value
				Case 3
					ServoOut PORTC.6, misc_value
				Case Else
					ServoOut PORTC.7, misc_value
				EndSelect
				WaitMs speed(motornumber)
				misc_value = misc_value + 1
			Wend
			PORTB = %11111111
			position(motornumber) = 1
			Gosub writeoutputs
			Write motornumber, 1  'Position saved
Return                                            
getinputs:
	I2CStart  'Issue I2C start signal
	I2CSend 65  'Send adress for U4
	I2CRecN dummy  'Read the input commands
	I2CStop  'Issue
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

Return                                            
writeoutputs:
	If position(motornumber) = 1 Then
		Select Case motornumber
		Case 1
			expander_out = expander_out Or 8
		Case 2
			expander_out = expander_out Or 4
		Case 3
			expander_out = expander_out Or 2
		Case Else
			expander_out = expander_out Or 1
		EndSelect
	Else
		Select Case motornumber
		Case 1
			expander_out = expander_out And 7
		Case 2
			expander_out = expander_out And 11
		Case 3
			expander_out = expander_out And 13
		Case Else
			expander_out = expander_out And 14
		EndSelect
	Endif
	expander_out = expander_out Or 240  'leave high nibble as input
	I2CStart  'Issue I2C start signal
	I2CSend 64  'send adress u4
	I2CSend expander_out  'Send data to U4
	I2CStop  'Issue stop
	
Return                                            
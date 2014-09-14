'program bridge up/down
'print MGV116asV1.1
'29-08-14 first edition
'RA0 used for increasing speeds
'RA1 used for decreasing speeds
'V1.2  feed-back also activated when same position is commanded.
Define CONF_WORD = 0x3f50
AllDigital
CMCON = 7
TRISA = %00111111
TRISB = %00000000
PORTA = 255
PORTB = %00011000
Symbol plus = RA0
Symbol minus = RA1
Symbol limit_up = RA2
Symbol limit_down = RA3
Symbol cmd_up = RA4
Symbol cmd_down = RA5
Symbol fbup = RA6
Symbol fbdn = RA7
Symbol redsign = RB0
Symbol yellowsign = RB1  '* * * * * * * * * Not implemented yet
Symbol greensign = RB2
'symbol PWM output = RB3 'do not define this one
Symbol redled = RB4
Symbol greenled = RB5
Symbol fwd = RB6
Symbol rev = RB7
Dim position As Bit
Dim speed As Word
Dim runtime_up As Word
Dim runtime_down As Word
Dim runtime As Word
Dim switchtime As Word  'time when motor should slow down
Dim breaktime As Word  'time that is needed from min_ tot max_speed
Dim min_speed As Word
Dim max_speed As Word
Dim value As Byte
Call init_value()
Call handle_eeprom(0)
Const ctrl_pause = 15
Const speed_step = 2
PWMon 1, 1
position = 0  'assume bridge is down
Call signs(1, 0)
If limit_up = 0 Then  'check otherwise
	If limit_down = 1 Then
		position = 1  'bridge is up
		Call signs(0, 1)
	Endif
Endif
Call feed_back(position)
breaktime = 400
WaitMs 2000
greenled = 1
redled = 1

main:
	Call check_keys()
	If cmd_down <> cmd_up Then
		If cmd_up = 1 Then  'bridge cmd up
			If position = 0 Then  'SURE bridge IS NOT down?
				greenled = 1
				redled = 0
				If limit_up = 1 Then  'is it already up?
					Call bridge()  'no
				Endif
			Else  'signal for rocrail, here feed back is first disabled and after delat enabled agaain
				fbup = 1
				fbdn = 1
				WaitMs 1000
				Call feed_back(position)
			Endif
		Else  'bridge cmd down
			If position = 1 Then  'ís bridge up?
				greenled = 0
				redled = 1
				If limit_down = 1 Then  'is it already down?
					Call bridge()  'no
				Endif
			Else  'signal for rocrail
				fbup = 1
				fbdn = 1
				Call feed_back(position)
			Endif
		Endif
	Endif
Goto main
End                                               

Proc check_keys()
		If plus = 0 Then
			If position = 0 Then  'position  is DOWN minimum_speed changing
				If min_speed < 970 Then
					min_speed = min_speed + 50
					Call handle_eeprom(1)
					Write 7, min_speed.LB
					Write 8, min_speed.HB
				Endif
			Else
				If max_speed < 970 Then
					max_speed = max_speed + 50
					Write 5, max_speed.LB
					Write 6, max_speed.HB
				Endif
			Endif
			redled = 0
			greenled = 0
			WaitMs 500
			greenled = position
			redled = Not position
		Endif
		If minus = 0 Then
			If position = 0 Then  'position  is down minimum_speed changing
				If min_speed > 100 Then
					min_speed = min_speed - 50
					Write 7, min_speed.LB
					Write 8, min_speed.HB
				Endif
			Else
				If max_speed > 300 Then
					max_speed = max_speed - 50
					Write 5, max_speed.LB
					Write 6, max_speed.HB
				Endif
			Endif
			redled = 0
			greenled = 0
			WaitMs 500
			greenled = position
			redled = Not position
		Endif
		WaitMs 20
End Proc                                          

Proc bridge()
	fbup = 1
	fbdn = 1
	runtime = 0
	Call signs(1, 0)
	Call handle_eeprom(0)
	If position = 0 Then  'bridge going up
		WaitMs 1000
		speed = min_speed
		PWMduty 1, speed  'speed of motor set to minimum
		switchtime = runtime_up - breaktime
		Call ctrl_motor(1, 1)  'motor forward
		While limit_up = 1  'wait for limit switch
			WaitMs ctrl_pause
			runtime = runtime + 1
			If runtime < breaktime Then
				If speed < max_speed Then
					speed = speed + speed_step  'increase motor speed
				Endif
			Else
				If runtime > switchtime Then
					If speed > min_speed Then
						speed = speed - speed_step
					Endif
				Endif
			Endif
			PWMduty 1, speed  'speed of motor
		Wend
		Call ctrl_motor(1, 0)  'motor stop
		WaitMs 1000
		Call signs(1, 1)
		WaitMs 1000
		Call signs(0, 1)
	Else  'bridge going down
		Call signs(1, 0)
		WaitMs 1000
		speed = min_speed
		PWMduty 1, speed  'speed of motor set to minimum
		switchtime = runtime_down - breaktime
		Call ctrl_motor(0, 1)  'motor reverse
		While limit_down = 1  'wait for limit switch
			WaitMs ctrl_pause
			runtime = runtime + 1
			If runtime < breaktime Then
				If speed < max_speed Then
					speed = speed + speed_step  'increase motor speed
				Endif
			Else
				If runtime > switchtime Then
					If speed > min_speed Then
						speed = speed - speed_step
					Endif
				Endif
			Endif
			PWMduty 1, speed  'speed of motor
		Wend
		Call ctrl_motor(0, 0)  'motor stop
		Call signs(1, 0)
	Endif
'now calculate the difference between old en new time and correct it
	If position = 0 Then  'bridge went up
		If runtime > runtime_up Then
			runtime = runtime - runtime_up
			runtime = runtime / 5
			runtime_up = runtime_up + runtime
		Else
			runtime = runtime_up - runtime
			runtime = runtime / 5
			runtime_up = runtime_up - runtime
		Endif
	Else  'bridge went down
		If runtime > runtime_down Then
			runtime = runtime - runtime_down
			runtime = runtime / 5
			runtime_down = runtime_down + runtime
		Else
			runtime = runtime_down - runtime
			runtime = runtime / 5
			runtime_down = runtime_down - runtime
		Endif
	Endif
'*************** end calculation
	Call handle_eeprom(1)
	position = Not position
	Call feed_back(position)
End Proc                                          

Proc handle_eeprom(task As Bit)
	If task = 0 Then  'this is reading eeprom
		Read 5, max_speed.LB
		Read 6, max_speed.HB
		Read 7, min_speed.LB
		Read 8, min_speed.HB
		Read 20, runtime_up.LB
		Read 21, runtime_up.HB
		Read 22, runtime_down.LB
		Read 23, runtime_down.HB
	Else  'write to eeprom
		Write 5, max_speed.LB
		Write 6, max_speed.HB
		Write 7, min_speed.LB
		Write 8, min_speed.HB
		Write 20, runtime_up.LB
		Write 21, runtime_up.HB
		Write 22, runtime_down.LB
		Write 23, runtime_down.HB
	Endif
End Proc                                          

Proc feed_back(pos As Bit)
	WaitMs 1000
	fbdn = position
	fbup = Not position
End Proc                                          

Proc init_value()
Read 0, value
If value = 0 Then
	Read 1, value
Endif
If value > 0 Then  'FIRST TIME THIS PROGRAM IS RUNNING.
	min_speed = 250
	max_speed = 750
	runtime_up = 1000
	runtime_down = 1000
	Call handle_eeprom(1)
	Write 0, 0
	Write 1, 0
Endif
End Proc                                          

Proc signs(red As Bit, green As Bit)
	redsign = red
	greensign = green
	yellowsign = 0
End Proc                                          

Proc ctrl_motor(dir As Bit, onoff As Bit)
	If onoff = 0 Then
		fwd = 0
		rev = 0
	Else
		fwd = dir
		rev = Not dir
	Endif
End Proc                                          
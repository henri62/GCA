'program bridge up/down
'print MGV116
Define CONF_WORD = 0x3f50
AllDigital
CMCON = 7
TRISA = %01011001
TRISB = %00000000
PORTA = 255
PORTB = 0

Symbol fwd = RB5
Symbol rev = RB4

Symbol red_led = RB6
Symbol green_led = RB7
Symbol fbup = RA1
Symbol fbdn = RA2
Symbol limit_up = RA3
Symbol limit_down = RA4
Symbol redsign = RA5
Symbol cmd_down = RA6
Symbol cmd_up = RA0
Symbol greensign = RA7
Dim position As Byte
Dim speed As Word
Const max_speed = 1000
Const min_speed = 300
PWMon 1, 1
fbup = 1
fbdn = 1
position = 0
If limit_up = 0 Then
	If limit_down = 1 Then
		position = 1
	Endif
Endif
WaitMs 3000
Call signals(position)
main:
	While cmd_down = cmd_up  'do nothing before 1st command
		WaitMs 20
	Wend
	If cmd_up = 1 Then  'bridge cmd up
		If position = 0 Then  'ís bridge down?
			If limit_up = 1 Then  'next position check
				Call bridge_up()
			Endif
		Endif
	Endif
	While cmd_down = cmd_up  'do nothing before 1st command
		WaitMs 20
	Wend
	If cmd_down = 1 Then  'bridge cmd down
		If position = 1 Then  'ís bridge up?
			If limit_down = 1 Then  'next position check
				Call bridge_down()
			Endif
		Endif
	Endif
Goto main
End                                               
Proc bridge_up()
	Call signals(position)
	speed = 300
	PWMduty 1, speed  'speed of motor set to 30%
	rev = 0
	fwd = 1  'run forward
	While limit_up = 1  'wait for limit switch
		If speed < max_speed Then
			speed = speed + 4  'increase motor speed
			PWMduty 1, speed  'speed of motor
		Endif
		WaitMs 10
	Wend
	fwd = 0  'stop motor
	fwd = 0
	rev = 0
	position = 1  'bridge = up now
	WaitMs 1000
	redsign = 0
	greensign = 1
	fbup = 0
	fbdn = 1
End Proc                                          
Proc bridge_down()
	Call signals(position)
	speed = min_speed
	PWMduty 1, speed  'speed of motor set to 30%
	fwd = 0
	rev = 1  'run bridge down
	While limit_down = 1  'wait for limit switch
		If speed < max_speed Then
			speed = speed + 4  'increase motor speed
			PWMduty 1, speed  'speed of motor
		Endif
		WaitMs 10
	Wend
	Call bouncing()
	rev = 0  'stop motor
	fwd = 0
	position = 0  'bridge = down now
	WaitMs 1000
	fbup = 1
	fbdn = 0
End Proc                                          
Proc signals(pos As Byte)
	If pos = 0 Then
		red_led = 1
		green_led = 0
		redsign = 1
		greensign = 0
	Else
		red_led = 1
		green_led = 0
		redsign = 1
		greensign = 0
	Endif
End Proc                                          
Proc bouncing()
	Exit
	PWMduty 1, min_speed  'speed of motor
	fwd = 1
	rev = 0  'run bridge up
	While limit_down = 0  'wait for limit switch
	WaitMs 10
	Wend
	fwd = 0
	rev = 1  'run brfidge down
	While limit_down = 1  'wait for limit switch
	WaitMs 10
	Wend
	rev = 0  'stop motor
	fwd = 0
End Proc                                          
'program bridge up/down
'print MGV116
Define CONF_WORD = 0x3f50
AllDigital
CMCON = 7
TRISA = %00011001
TRISB = %00000000
PORTA = 255
PORTB = 0

Symbol fwd = RB5
Symbol rev = RB4
Symbol start_motor = RB3
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
PWMon 1, 1
fbup = 1
fbdn = 1
Read 0, position
If position > 1 Then
	position = 0
Endif
Gosub signals
main:
	While cmd_down = cmd_up  'do nothing before 1st command
	Wend
	If cmd_up = 0 Then  'bridge cmd up
		If position = 0 Then  'ís bridge down?
			Gosub bridge_up
		Endif
	Endif
	If cmd_down = 0 Then  'bridge cmd down
		If position = 1 Then  'ís bridge up?
			Gosub bridge_down
		Endif
	Endif
Goto main
End                                               
bridge_up:
	Gosub signals
	speed = 200
	PWMduty 1, speed  'speed of motor set to 30%
	rev = 0
	fwd = 1  'run forward
	start_motor = 1  'inhibit output L293
	While limit_up = 1  'wait for limit switch
		If speed < 800 Then
			speed = speed + 4  'increase motor speed
			PWMduty 1, speed  'speed of motor
		Endif
		WaitMs 10
	Wend
	start_motor = 0
	position = 1  'bridge = up now
	Write 0, position
	WaitMs 1000
	redsign = 0
	greensign = 1
	fbup = 0
	fbdn = 1
Return                                            
bridge_down:
	Gosub signals
	speed = 200
	PWMduty 1, speed  'speed of motor set to 30%
	fwd = 0
	rev = 1  'run backward
	start_motor = 1  'enable output L293
	While limit_down = 1  'wait for limit switch
		If speed < 800 Then
			speed = speed + 4  'increase motor speed
			PWMduty 1, speed  'speed of motor
		Endif
		WaitMs 10
	Wend
	fwd = 1
	rev = 0  'run forward
	PWMduty 1, 150  'speed of motor
'now start bouncing
	While limit_down = 0  'wait for limit switch
	Wend
	fwd = 0
	rev = 1  'run backward
	While limit_down = 1  'wait for limit switch
	Wend
	start_motor = 0  'diable output L293
	position = 0  'bridge = down now
	Write 0, position
	WaitMs 1000
	fbup = 1
	fbdn = 0
Return                                            
signals:
	If position = 0 Then
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

Return                                            

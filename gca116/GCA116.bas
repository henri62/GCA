'program bridge up/down
'print MGV116
'24-8-14 version v1.08 bridge reversed, speed lower, speed-up/down slower.
'24-8-14 version v1.09 calculation runtime change decreased, bouncing disabled.
'24-8-14 version v1.11 calculation runtime corrected.
'24-8-14 version v1.14 procedure bridge changed.
'25-8-14 version v1.16 no eeprom used
'25-8-14 version v1.17 red led switched before going down en brigde delayed
'_                     Min_speed is lower on going down
'-                     extra delay between red sign on and start bridge down

Define CONF_WORD = 0x3f50
'Define SIMULATION_WAITMS_VALUE = 1
AllDigital
CMCON = 7
TRISA = %00111001
TRISB = %00000000
PORTA = 255
PORTB = 0
Symbol fwd = RB4
Symbol rev = RB5
Symbol red_led = RB6
Symbol green_led = RB7
Symbol fbup = RA1
Symbol fbdn = RA2
Symbol limit_up = RA4
Symbol limit_down = RA3
Symbol redsign = RA6
Symbol cmd_down = RA5
Symbol cmd_up = RA0
Symbol greensign = RA7
Dim position As Bit
Dim speed As Word
'Dim runtime_calculated As Word  'totsl time for runnung up to be calibrated
Dim runtime_up As Word
Dim runtime_down As Word
Dim runtime As Word
Dim switchtime As Word  'time when motor should slow down
Dim breaktime As Word  'time that is needed from min_ tot max_speed
Const ctrl_pause = 15
Const speed_step = 2
PWMon 1, 1
fbup = 1
fbdn = 1
position = 0
If limit_up = 0 Then
	If limit_down = 1 Then
		position = 1
	Endif
Endif
runtime_up = 900
runtime_down = 900
breaktime = 400
WaitMs 3000
redsign = 1
greensign = 0
green_led = position
red_led = Not green_led

main:
	While cmd_down = cmd_up  'do nothing before 1st command
		WaitMs 20
	Wend
	If cmd_up = 1 Then  'bridge cmd up
		If position = 0 Then  'ís bridge down?
			If limit_up = 1 Then  'is it already up?
				Call bridge()  'no
			Endif
		Endif
	Endif
	While cmd_down = cmd_up  'do nothing before 1st command
		WaitMs 20
	Wend
	If cmd_down = 1 Then  'bridge cmd down
		If position = 1 Then  'ís bridge up?
			If limit_down = 1 Then  'is it already down?
				Call bridge()  'no
			Endif
		Endif
	Endif
Goto main
End                                               

Proc bridge()
	PWMduty 1, speed  'speed of motor set to minimum
	rev = position
	fwd = Not position  'run forward
	runtime = 0
	red_led = Not position
	green_led = position
	If position = 0 Then  'bridge going up
		speed = 150
		switchtime = runtime_up - breaktime
		While limit_up = 1  'wait for limit switch
			runtime = runtime + 1
			If runtime < breaktime Then
				If speed < 550 Then
					speed = speed + speed_step  'increase motor speed
				Endif
			Else
				If runtime > switchtime Then
					If speed > 150 Then
						speed = speed - speed_step
					Endif
				Endif
			Endif
			PWMduty 1, speed  'speed of motor
			WaitMs ctrl_pause
		Wend
		WaitMs 1000
		redsign = 0
		greensign = 1
	Else  'bridge going down
		redsign = 1
		greensign = 0
		WaitMs 2000
		speed = 150
		switchtime = runtime_down - breaktime
		While limit_down = 1  'wait for limit switch
			runtime = runtime + 1
			If runtime < breaktime Then
				If speed < 550 Then
					speed = speed + speed_step  'increase motor speed
				Endif
			Else
				If runtime > switchtime Then
					If speed > 100 Then
						speed = speed - speed_step
					Endif
				Endif
			Endif
			PWMduty 1, speed  'speed of motor
			WaitMs ctrl_pause
		Wend
	Endif
	fwd = 0  'stop motor
	rev = 0
	If position = 0 Then  'bridge went up
		position = 1
		If runtime > runtime_up Then
			runtime = runtime / 30
			runtime_up = runtime_up + runtime
		Else
			runtime = runtime / 30
			runtime_up = runtime_up - runtime
		Endif
	Else  'bridge went down
		position = 0
		If runtime > runtime_down Then
			runtime = runtime / 30
			runtime_down = runtime_down + runtime
		Else
			runtime = runtime / 30
			runtime_down = runtime_down - runtime
		Endif
	Endif
	WaitMs 1000
	fbup = Not position
	fbdn = position
End Proc                                          





	
	
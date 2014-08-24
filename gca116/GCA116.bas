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
Dim position As Bit
Dim speed As Word
Dim runtime_up As Word  'totsl time for runnung up to be calibrated
Dim runtime_down As Word  'totsl time for runnung down tot be calibrated
Dim switchtime As Word  'time when motor should slow down
Dim breaktime As Word  'time that is needed from min_ tot max_speed
Const max_speed = 800
Const min_speed = 200
Const ctrl_pause = 10
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
Read 0, runtime_up.LB
Read 1, runtime_up.HB
Read 2, runtime_down.LB
Read 3, runtime_down.HB
breaktime = max_speed - min_speed
breaktime = breaktime / speed_step
WaitMs 3000
Call signals(position)
main:
	While cmd_down = cmd_up  'do nothing before 1st command
		WaitMs 20
	Wend
	If cmd_up = 1 Then  'bridge cmd up
		If position = 0 Then  'ís bridge down?
			If limit_up = 1 Then  'is it already up?
				Call bridge_up()  'no
			Endif
		Endif
	Endif
	While cmd_down = cmd_up  'do nothing before 1st command
		WaitMs 20
	Wend
	If cmd_down = 1 Then  'bridge cmd down
		If position = 1 Then  'ís bridge up?
			If limit_down = 1 Then  'is it already down?
				Call bridge_down()  'no
			Endif
		Endif
	Endif
Goto main
End                                               
Proc bridge_up()
	Call signals(position)
	If runtime_up > 2000 Then
		runtime_up = 2000
	Endif
	switchtime = runtime_up - breaktime
	speed = min_speed
	PWMduty 1, speed  'speed of motor set to minimum
	rev = 0
	fwd = 1  'run forward
	switchtime = runtime_up - 300
	runtime_up = 0
	While limit_up = 1  'wait for limit switch
		runtime_up = runtime_up + 1
		If runtime_up < breaktime Then
			If speed < max_speed Then
				speed = speed + speed_step  'increase motor speed
			Endif
		Else
			If runtime_up > switchtime Then
				If speed > min_speed Then
					speed = speed - speed_step
				Endif
			Endif
		Endif
		PWMduty 1, speed  'speed of motor
		WaitMs ctrl_pause
	Wend
	fwd = 0  'stop motor
	fwd = 0
	rev = 0
	position = 1  'bridge = up now
	Write 0, runtime_up.LB
	Write 1, runtime_up.HB
	WaitMs 1000
	redsign = 0
	greensign = 1
	fbup = 0
	fbdn = 1
End Proc                                          

Proc bridge_down()
	Call signals(position)
	If runtime_down > 2000 Then
		runtime_down = 2000
	Endif
	switchtime = runtime_down - breaktime
	speed = min_speed
	PWMduty 1, speed  'speed of motor set to minmum
	fwd = 0
	rev = 1  'run bridge down
	switchtime = runtime_down - 300  'slow down starts (max_speed - min_speed)
	runtime_down = 0
	While limit_down = 1  'wait for limit switch
		runtime_down = runtime_down + 1
		If runtime_down < breaktime Then
			If speed < max_speed Then
				speed = speed + speed_step  'increase motor speed
			Endif
		Else
			If runtime_down > switchtime Then
				If speed > min_speed Then
					speed = speed - speed_step
				Endif
			Endif
		Endif
		WaitMs ctrl_pause
		PWMduty 1, speed  'speed of motor
	Wend
	Call bouncing()
	rev = 0  'stop motor
	fwd = 0
	position = 0  'bridge = down now
	WaitMs 1000
	fbup = 1
	fbdn = 0
End Proc                                          

Proc signals(pos As Bit)
	red_led = Not pos
	green_led = pos
	redsign = Not pos
	greensign = pos
End Proc                                          

Proc bouncing()
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



	
	
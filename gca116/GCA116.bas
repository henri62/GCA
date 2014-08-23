'program bridge up/down
'print MGV116
Define CONF_WORD = 0x3f50
AllDigital
CMCON = 7
TRISA = %00011001
TRISB = %00000000
PORTA = 255
PORTB = 0

Symbol fwd = PORTB.4
Symbol rev = PORTB.5
Symbol red_led = PORTB.6
Symbol green_led = PORTB.7
Symbol fbup = PORTA.1
Symbol fbdn = PORTA.2
Symbol limit_up = PORTA.3
Symbol limit_dn = PORTA.4
Symbol redsign = PORTA.5
Symbol yellowsign = PORTA.6
Symbol greensign = PORTA.7
Dim direction As Byte
PWMon 1, 1
PWMduty 1, 100          'speed of motor ,  can be between 1 and 1023

main:
If PORTA.0 = 0 Then
	direction = 0
Else
	direction = 1
Endif
Select Case direction
Case 1          'bridge should be up
	red_led = 1
	green_led = 0
	If limit_up = 0 Then          'bridge is upper most position
		fwd = 0          'stop the motor
		rev = 0
		fbup = 0
		fbdn = 1
		redsign = 0
		yellowsign = 0
		greensign = 1
	Else
		fbup = 1
		fbdn = 1
		fwd = 1          'run forward
		rev = 0
		redsign = 1
		yellowsign = 1
		greensign = 0
	Endif
Case Else          'bridge must be in down position
	red_led = 0
	green_led = 1
	redsign = 1
	yellowsign = 1
	greensign = 0
	If limit_dn = 0 Then          'bridge is lower most position
		fbup = 1
		fbdn = 0
		fwd = 0          'stop the motor
		rev = 0
	Else
		fbup = 1
		fbdn = 1
		fwd = 0          'run backward
		rev = 1
	Endif
EndSelect
WaitMs 50          'there is no hurry

Goto main
End                                               
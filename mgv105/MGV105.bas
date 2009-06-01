'program for MGV105 DDX booster
Define CONF_WORD = 0x3194
TRISIO = %001001          'bits 0 , 3 as input
ANSEL = 1
WPU = %000000          'internal pull-up disabled
'assigned pins :
'bit 0 = analog input current detection
'bit 1 = enable L6203
'bit 2 = led 4
'bit 3 = input start from serial port
'bit4 = short circuit indication loconet
'bit5 = power on/off indication to serial port and loc0net
Dim current_value As Byte
Dim current_count As Byte
Dim short_circuit As Bit
Dim ddx_in_control As Bit
Dim led As Byte
Symbol demand_power_on = GP3
short_circuit = 0
GPIO = %1111101          'everything off
main:
WaitMs 100
Adcin 0, current_value
	If current_value > 75 Then          '~~ 0,47Ohm x 3Amp x 255 / 5
		If current_count < 11 Then
			current_count = current_count + 1
		Endif
		If current_count > 10 Then          'there is a problem
			short_circuit = 1          'indicate power should be off
		Endif
	Else
		If short_circuit = 0 Then
			If current_count > 0 Then
				current_count = current_count - 1
			Endif
		Endif
	Endif

	If short_circuit = 1 Then          '> 0,5sec short too high current > shut off
		If led = 0 Then          'create flashing led
			led = 1
			GPIO = %001100
		Else
			led = 0
			GPIO = %001000
		Endif
	Endif

	If demand_power_on = 1 Then          'ddx has send power off command
		ddx_in_control = 1          'ddx is from here able to switch on
		If short_circuit = 1 Then          '> 0,5sec short too high current > shut off
			If led = 0 Then          'create flashing led
				led = 1
				GPIO = %001100
			Else
				led = 0
				GPIO = %001000
			Endif
		Else
			GPIO = %001000
		Endif
	Endif
	If demand_power_on = 0 Then          'ddx has send power on command
		If ddx_in_control = 1 Then          'ddx is allowed to switch on
			short_circuit = 0          'assume no problem anymore
			GPIO = %101111          'output on indicators off
			current_count = 0          'short circuit counter reset
			ddx_in_control = 0          'no next power on possible
		Endif
	Endif

Goto main


End                                               

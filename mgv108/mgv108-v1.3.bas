'program for MGV108 DDX booster
'Version 1-3
'changes in MGV108 board : GP4 is changed to input for Power output select.
'.                                          Opto U3c is now commanded by GP2 (same as Enable for U5)
Define CONF_WORD = 0x3194
TRISIO = %011011          'bits 0 , 1, 3 as input
AllDigital
ANSEL = 1
WPU = %000000          'internal pull-up disabled
'assigned pins :
'GP0 = analog input current detection
'GP1 = Railsync puls input
'GP2 = enable L6203
'GP3 = input start from Loconet (Low = on)
'GP4 = power on/off indication to  loconet
'GP5 = short circuit indication loconet

Dim current_value As Word
Dim current_count As Word
Dim short_circuit As Bit
Dim no_pulse As Bit
Dim stop_booster As Bit
Dim pulse_count As Byte
Dim previous_pulse As Bit
Dim led As Byte
Dim loconet_in_control As Bit
Dim led_count As Byte
Dim max_current As Word
no_pulse = 0
short_circuit = 0
pulse_count = 0
current_count = 0
GPIO = %1111111          'system on
loconet_in_control = 1

main:
'WaitMs 1          'Total loop time of program = ~~ 265 uS.
					'so with this 1 mS wait it is approx 1.26 mSec.
'counter pulse_count need to count to 20 to shut output off in time
'current_count need to count to 800 for 1 sec delay for short circuit
	If GP3 = 1 Then
		GPIO = %101011          'normal power off
		short_circuit = 0          'reset short_circuit
		no_pulse = 0          'reset pulse check error
		loconet_in_control = 1          'enable loconet next power on
	Else
		Gosub check_current
		If short_circuit = 1 Then
			led_count = led_count + 1
			Select Case led_count
			Case 100
				GPIO = %101011          'short circuit out off
			Case 255
				GPIO = %001011          'short circuit out on
				led = 0
			Case Else
			EndSelect
			loconet_in_control = 0          'disable next possible
		Else
			Gosub check_pulse
			If no_pulse = 1 Then          'there was no pulse
				GPIO = %101011          'power off
				loconet_in_control = 0          'disable next possible
			Else          'everything seems to be fine
				GPIO = %111111          'normal power on, no indication
				loconet_in_control = 0          'disable loconet next power on
			Endif          'no pulse
		Endif          'end short_circuit
	Endif          'end gp3=1


Goto main


End                                               
check_current:
'.                  calculation of maximum value in ADCin word:
'.                             (R * I  * w)/ Uref
'.                              R= resistance R13
'.                              I = maximum current in R13
'.                              w = value of word from AD converter
'.             Uref = Volt reference for AD converter.
'.          For low current (1 Amp) :
'.             0,47 * 1 *1023 / 5 = 96
'.          For High current (3 Amp):
'.             0,47 * 3 *1023/5 = 288
	If GP4 = 0 Then          'select high or Low current output
		max_current = 96
	Else
		max_current = 288
	Endif
	Adcin 0, current_value          'measure current value in 10 bits
'.                  calculation of maximum value in ADCin word:
'.                             (R * I  * w)/ Uref
'.                              R= resistance R13
'.                              I = maximum current in R13
'.                              w = value of word from AD converter
'.             Uref = Volt reference for AD converter.
	If current_value > max_current Then          '~~ 0,47(=Ohm) x 3(=Amp) x 1023 (=10 bits) / 5 (=volt ref)
		If current_count < 800 Then          'no further counting is needed
			current_count = current_count + 1
		Else          '800 counts reached
			short_circuit = 1          'indicate power should be off
		Endif
	Else
		If short_circuit = 0 Then
			If current_count > 0 Then          'if no overcurrent is measured, count down
				current_count = current_count - 1
			Endif
		Endif
	Endif
Return                                            
check_pulse:
	If GP1 <> previous_pulse Then          'check railsync
		previous_pulse = GP1          'remember this bit
		pulse_count = 0
		no_pulse = 0
	Else
		If pulse_count < 20 Then          'within 30 mSec power off
			pulse_count = pulse_count + 1
		Else
			no_pulse = 1
		Endif
	Endif
Return                                            

'program for ORD-3 Spax booster / DDX Booster

Define CONF_WORD = 0x31c4

AllDigital
ANSEL.ANS0 = 1
ANSEL.ANS1 = 0
ANSEL.ANS2 = 0
ANSEL.ANS3 = 0

ADCON0.ADON = 1
ADCON0.VCFG = 0

TRISIO = %001111
WPU = %000010  'internal pull-up's
Dim short_count1 As Byte
Dim short_count2 As Byte
Dim short_time As Byte
Dim pulse_count1 As Byte
Dim pulse_count2 As Byte
Dim v_r5 As Word
Dim previous_pulse1 As Bit
Dim previous_pulse2 As Bit
Dim max_current As Word
Dim ok_state As Byte
Symbol pulse1 = GP3
Symbol pulse2 = GP2
Symbol l6203_enable = GP4
Symbol booster_on = GP5
l6203_enable = 0  'system off
short_count1 = 0
short_count2 = 0
short_time = 0
pulse_count1 = 0
pulse_count2 = 0
booster_on = 1
ok_state = 0
WaitMs 1000
Gosub check_jumper
main:
		WaitUs 1750  'Total loop time of program = ~~ 250us.
					'.         so this will make  2 mS  for each loop
		Gosub check_l6203
		If ok_state.2 = 1 Then
			If short_count2 > 0 Then
				If short_count1 > 0 Then
					short_count1 = short_count1 - 1
				Else
					short_count2 = short_count2 - 1
					short_count1 = 250
				Endif
			Else
				ok_state.2 = 0
			Endif
		Endif

		If pulse1 <> previous_pulse1 Then  'check railsync
			previous_pulse1 = pulse1  'remember this bit
			ok_state.0 = 0
			pulse_count1 = 0
		Else
			If pulse_count1 < 30 Then  'within 30 mSec power off
				pulse_count1 = pulse_count1 + 1
			Else
				ok_state.0 = 1
			Endif
		Endif
		If pulse2 <> previous_pulse2 Then  'check railsync
			previous_pulse2 = pulse2  'remember this bit
			ok_state.1 = 0
			pulse_count2 = 0
		Else
			If pulse_count2 < 30 Then  'within 30 mSec power off
				pulse_count2 = pulse_count2 + 1
			Else
				ok_state.1 = 1
			Endif
		Endif
		Select Case ok_state
		Case 0  'no problem found
			l6203_enable = 1  'power on booster
			booster_on = 0
		Case Else  'there is a problem
			Gosub check_jumper
			l6203_enable = 0  'power off booster
			booster_on = 1
		EndSelect
Goto main
End                                               

check_l6203:
	Adcin 0, v_r5  'measure currect
	If v_r5 > max_current Then
		If short_count1 < 250 Then  '250 times too high = after 0,5 second
			short_count1 = short_count1 + 1
		Else
			short_count2 = 10  'delay 10 x 0,5 before switch on again
			ok_state.2 = 1  'indicate power should be off
		Endif
	Endif
Return                                            

check_jumper:
	If GP1 = 1 Then
		max_current = 240  '(V_r5 * 1023)/Vref  = 0,33Ohm x 3 Amp x 1023 / 5
	Else
		max_current = 80  '(V_r5 * 1023)/Vref  = 0,33Ohm x 1 Amp x 1023 / 5
	Endif
Return                                            



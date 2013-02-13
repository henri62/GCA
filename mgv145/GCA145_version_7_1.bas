

Define CONF_WORD = 0x3f72

'program TT for PCBoard GCA145 / MGV145
'Universal version 7.1
'For sliding systems (FY) extra end switch is used.
'RB7 on ICSP1 (pin3) is endswitch (pass last rail)
'J4 is used as zero switch (pass first rail)
'EEP1 jumper is used to enable pos 61 and 62

'three extra position are possible:
'-  position 63 to go to zero switch and then to rail 1
'-  Position 62 to manually set all positions first time (Manual initialisation, only when eep1 is set)
'programs :
'0  correction actual position
'1  Max positions
'2  min speed
'3  max speed
'4  incr/decr delay
'5  extra correction steps
'6  bridge power normal/ reversed (Pos) at this position and all higher ones, bridge power is reversed
'7  Bridge power on/off (1/0) while moving
'8  pos_match delay
'9  setting of control type (0..6)
'-  control types:
'-    1 |control is set For turntable NO 'shortest way option' means that running from highest to lowest position (or v.v.) will NOT pass 0 position.|
'-    2 |control is set For turntable with  'shortest way option' which means that running from one to next position (or v.v.) could pass 0 position.|
'-    3 |version for two rail tt, no restriction On  'short way'. This is special for TT with contact rings  (no cables).|
'-    4 |fiddle yard.railpower will Not be reversed.End switch at the High position side is needed.\\ connector icsp1 is used For connecting this switch|

'-      for control type see manual

Dim steps As Word
Dim half_steps As Word
Dim ramp_down_point As Word
Dim program_set As Byte
Dim new_steps As Word
Dim act_steps As Word
Dim new_pos As Byte
Dim act_pos As Byte
Dim one_round As Word
Dim one_round_half As Word
Dim bus_pos As Byte
Dim old_bus_pos As Byte
Dim speed As Word
Dim speed_min As Word
Dim speed_max As Word
Dim new_encoder As Byte
Dim old_encoder As Byte
Dim manual_pos As Byte
Dim old_manual_pos As Byte
Dim result As Byte
Dim free_space_steps As Byte
Dim value As Byte
Dim eprom_pointer As Byte
Dim encoder_count As Byte
'Dim onoff As Bit
Dim max_pos As Byte
Dim disp1_val As Byte
Dim disp2_val As Byte
Dim old_pos_write As Bit
Dim ramp_up As Bit
Dim speed_step As Byte
Dim version As Bit
Dim steps_set As Bit
Dim bridge_power As Byte
Dim pos_match_delay As Byte
Dim bridge_rev_pos As Byte
Dim ctrl_type As Byte
Dim turn_counter As Byte
Const disp1 = 64
Const disp2 = 66
Const mem_act_pos = 127
Const mem_turn_counter = 101
Const mem_speed_min = 103
Const mem_speed_max = 104
Const mem_speed_step = 105
Const mem_one_round_lb = 106
Const mem_one_round_hb = 107
Const mem_ctrl_type = 108
Const mem_max_pos = 109
Const mem_free_space = 110
Const mem_rev_pos = 111
Const mem_bridge_power = 112
Const mem_pos_match_delay = 113
Const mem_bridge_rev_pos = 114
Symbol swfw = RB0
Symbol swrev = RB1
Symbol sck = RB2
Symbol sda = RB3
Symbol pos_match = RB4
Symbol pos_write = RB5
Symbol direction = RC0
Symbol clk = RC1
Symbol ena = RC2
Symbol zero = RB6
Symbol max_end = RB7
Symbol ec1 = RC4
Symbol ec2 = RC5
Symbol ecsw = RC6
Symbol pgm = RC7
Symbol adjust_en = RC3

ADCON1 = %0110
TRISA = %111111
TRISB = %11101100
TRISC = %11111000
PORTB = %11101100
PORTC = %11111000
I2CPrepare sda, sck
Gosub init_values
Call relays(1)
program_set = 0
max_end = 1

main:
	If pgm = 0 Then
		Gosub adjust_settings
		WaitMs 500
	Endif

	Gosub get_new_position
	If bus_pos <> old_bus_pos Then
		Call relays(0)
'onoff = 0
'Gosub relays
		pos_match = 1  'indicate not in position
		new_pos = bus_pos
		Gosub stepmotor
		Call relays(1)
'onoff = 1
'Gosub relays
		Gosub wait_pos_match
	Else
		If ecsw = 0 Then  'only when knob is pushed
			If manual_pos <> old_manual_pos Then
				Call relays(0)
				pos_match = 1  'indicate not in position
				old_manual_pos = manual_pos
				new_pos = manual_pos
				Gosub stepmotor
				Call relays(1)
				Gosub wait_pos_match
			Endif
		Endif
	Endif
	WaitMs 10
	Goto main
End                                               

get_new_position:
	value = bus_pos  'remember the position
	If pos_write = 1 Then
		If old_pos_write = 0 Then  'react once on rising edge
			old_pos_write = 1
			value = PORTA And 63  'now get value from J5
			Select Case value
			Case 63  'position reset is desired
				value = 63
			Case Else
				If value > max_pos Then  'this is not allowed
					value = bus_pos  'put it back to old situation
				Endif
			EndSelect
			Gosub i2c_display
			If value = bus_pos Then  'no other position is set, just a response on pos_write is required
				pos_match = 1
				WaitMs 1000
				pos_match = 0
			Endif
		Endif
	Else
		old_pos_write = 0
	Endif
	bus_pos = value  'now take over the desired position
	Gosub calc_rotation
	Select Case result
	Case 1
		If encoder_count < 6 Then
			encoder_count = encoder_count + 1
		Else
			encoder_count = 0
			Select Case manual_pos
			Case < max_pos
				manual_pos = manual_pos + 1
			Case max_pos
				If adjust_en = 1 Then
					manual_pos = 63
				Else
					manual_pos = 62
				Endif
			Case 63
				manual_pos = 1
			Case 62
				manual_pos = 63
			Case Else
			EndSelect
			value = manual_pos
			Gosub i2c_display
		Endif
	Case 2
		If encoder_count > 0 Then
			encoder_count = encoder_count - 1
		Else
			encoder_count = 6
			Select Case manual_pos
			Case 62
				manual_pos = max_pos
			Case 63
				If adjust_en = 1 Then
					manual_pos = max_pos
				Else
					manual_pos = 62
				Endif
			Case 1
				manual_pos = 63
			Case 0
				manual_pos = 1
			Case Else
				manual_pos = manual_pos - 1
			EndSelect
			value = manual_pos
			Gosub i2c_display
		Endif
	Case Else
	EndSelect
	WaitMs 2
	
Return                                            



calc_rotation:  'this routine checks the incrimental switch
'1 = increasing value
'2 = decreasing value
'3 = no change
		new_encoder = 0
		new_encoder.0 = ec1
		new_encoder.1 = ec2
		Select Case old_encoder
		Case 0
			result = LookUp(3, 2, 1, 3), new_encoder
		Case 1
			result = LookUp(1, 3, 3, 2), new_encoder
		Case 2
			result = LookUp(2, 3, 3, 1), new_encoder
		Case Else
			result = LookUp(3, 1, 2, 3), new_encoder
		EndSelect
		old_encoder = new_encoder
Return                                            


adjust_settings:
	Gosub i2c_display_pgm
	WaitMs 800  'give some time for display to be read
	Call relays(0)
	Gosub steps_reading
	new_steps = act_steps  'nothing is changed yet
	While pgm = 0  'the program switch is still activated
		If ecsw = 0 Then  'knob is pressed, inrease program_set
			program_set = program_set + 1
			If program_set > 9 Then  'no more than 9 items te change
				program_set = 0
			Endif
			Gosub i2c_display_pgm
			WaitMs 800  'give some time for display to be read
			While ecsw = 0  'be sure pushbutton is released
				WaitMs 250  'cancel contact dender
			Wend
		Endif
		Select Case program_set
		Case 0  'correction of current position
			value = act_pos
			Gosub i2c_display
			Gosub calc_rotation  'look for the knob to be turned
			Select Case result
			Case 1
				If new_steps < 60000 Then
					ena = 1  'motor on
					new_steps = new_steps + 1
					direction = 1
					clk = 1  'clock out
					WaitUs speed_min
					clk = 0
					WaitUs speed_min
				Endif
			Case 2
				If new_steps > 0 Then
					ena = 1  'motor on
					new_steps = new_steps - 1
					direction = 0
					clk = 1  'clock out
					WaitUs speed_min
					clk = 0
					WaitUs speed_min
				Endif
			Case Else
			EndSelect
		Case 1  'setting of maximum positions
			steps_set = 0
			ena = 0  'shut off motor
			value = max_pos
			Gosub i2c_display
			Gosub calc_rotation
			Select Case result
			Case 1
				If max_pos < 48 Then  'no more positions possible
					max_pos = max_pos + 1
					Write mem_max_pos, max_pos
				Endif
			Case 2
				If max_pos > 2 Then  'minimum 2 positions
					max_pos = max_pos - 1
					Write mem_max_pos, max_pos
				Endif
			Case Else
			EndSelect
		Case 2  'setting of speed interval Minimum
					'setting will be done in multiple of 256 uSecs
					'minimum is usually not less then 5, and maximum is 99 (is real slow)
			steps_set = 0
			ena = 0  'shut off motor
			value = speed_min.HB
			Gosub i2c_display
			Gosub calc_rotation
			Select Case result
			Case 1
				If speed_min.HB < 99 Then  'display is limited to 99
					speed_min.HB = speed_min.HB + 1
					Write mem_speed_min, speed_min.HB
				Endif
			Case 2
				If speed_min.HB > speed_max.HB Then
					speed_min.HB = speed_min.HB - 1
					Write mem_speed_min, speed_min.HB
				Endif
			Case Else
			EndSelect
		Case 3  'setting of speed interval Maximum
					'setting will be done in multiple of 256 uSecs
					'minimum is 1 and maximum is 99 (is real slow)
			steps_set = 0
			ena = 0  'shut off motor
			value = speed_max.HB
			Gosub i2c_display
			Gosub calc_rotation
			Select Case result
			Case 1
				If speed_max.HB < speed_min.HB Then  'not more then speed_min
					speed_max.HB = speed_max.HB + 1
					Write mem_speed_max, speed_max.HB
				Endif
			Case 2
				If speed_max.HB > 2 Then
					speed_max.HB = speed_max.HB - 1
					Write mem_speed_max, speed_max.HB
				Endif
			Case Else
			EndSelect
		Case 4  'setting of ramp up and down speed
					'speed will be the value that speed will be changed every step up and down
			steps_set = 0
			ena = 0  'shut off motor
			value = speed_step
			Gosub i2c_display
			Gosub calc_rotation
			Select Case result
			Case 1
				If speed_step < 99 Then  'maximum 99 is possible
					speed_step = speed_step + 1
					Write mem_speed_step, speed_step
				Endif
			Case 2
				If speed_step > 1 Then
					speed_step = speed_step - 1
					Write mem_speed_step, speed_step
				Endif
			Case Else
			EndSelect
	
		Case 5  'setting of extra free space steps
			steps_set = 0
			ena = 0  'shut off motor
			If free_space_steps > 250 Then
				free_space_steps = 250
				Write mem_free_space, free_space_steps
			Endif
			value = free_space_steps
			Gosub i2c_display
			Gosub calc_rotation
			Select Case result
			Case 1
				If free_space_steps < 99 Then  'display is limited to 99
					free_space_steps = free_space_steps + 1
					Write mem_free_space, free_space_steps
				Endif
			Case 2
				If free_space_steps > 0 Then
					free_space_steps = free_space_steps - 1
					Write mem_free_space, free_space_steps
				Endif
			Case Else
			EndSelect
		Case 6  'at which postion is in bridge reversed power
			steps_set = 0
			ena = 0  'shut off motor
			steps_set = 0
			ena = 0  'shut off motor
			value = bridge_rev_pos
			Gosub i2c_display
			Gosub calc_rotation
			Select Case result
			Case 1
				If bridge_rev_pos < max_pos Then  'no more positions possible
					bridge_rev_pos = bridge_rev_pos + 1
					Write mem_bridge_rev_pos, bridge_rev_pos
				Endif
			Case 2
				If bridge_rev_pos > 0 Then  '0 does not effect reversing
					bridge_rev_pos = bridge_rev_pos - 1
					Write mem_bridge_rev_pos, bridge_rev_pos
				Endif
			Case Else
			EndSelect

		Case 7  'setting if power should be on or off while bridge is moving (0 = off)
			Read mem_bridge_power, bridge_power
			If bridge_power > 1 Then
				bridge_power = 1
			Endif
			value = bridge_power
			Gosub i2c_display
			Gosub calc_rotation
			Select Case result
			Case 1
				bridge_power = 1
				Write mem_bridge_power, bridge_power
			Case 2
				bridge_power = 0
				Write mem_bridge_power, bridge_power
			Case Else
			EndSelect
		Case 8  'setting delay in millisecs before tt is ready is sent)
			Read mem_pos_match_delay, pos_match_delay
			If pos_match_delay > 50 Then
				pos_match_delay = 50
			Endif
			value = pos_match_delay
			Gosub i2c_display
			Gosub calc_rotation
			Select Case result
			Case 1
				If pos_match_delay < 50 Then
					pos_match_delay = pos_match_delay + 1
				Endif
				Write mem_pos_match_delay, pos_match_delay
			Case 2
				If pos_match_delay > 0 Then
					pos_match_delay = pos_match_delay - 1
				Endif
				Write mem_pos_match_delay, pos_match_delay
			Case Else
			EndSelect
		Case Else  'menu 9 , setting control type
			steps_set = 0
			ena = 0  'shut off motor
			If ctrl_type > 4 Then
				ctrl_type = 4
				Write mem_ctrl_type, ctrl_type
			Endif
			value = ctrl_type
			Gosub i2c_display
			Gosub calc_rotation
			Select Case result
			Case 1
				If ctrl_type < 4 Then
					ctrl_type = ctrl_type + 1
					Write mem_ctrl_type, ctrl_type
				Endif
			Case 2
				If ctrl_type > 1 Then
					ctrl_type = ctrl_type - 1
					Write mem_ctrl_type, ctrl_type
				Endif
			Case Else
			EndSelect


		EndSelect
	Wend
	ena = 0  'shut off motor
		'***************   now check if changes have to BE written in eeprom
	If new_steps <> act_steps Then  'now save new position
		Gosub save_steps
	Endif
	value = act_pos
	Gosub i2c_display
	Call relays(1)
Return                                            

i2c_display:
	Select Case value
	Case 99
		disp1_val = %10111111
		disp2_val = %10111111
	Case Else
		disp1_val = value
		disp1_val = disp1_val Mod 10
		disp1_val = LookUp(%11000000, %11111001, %10100100, %10110000, %10011001, %10010010, %10000010, %11111000, %10000000, %10010000), disp1_val
		disp1_val.7 = pgm
		disp2_val = value
		disp2_val = disp2_val / 10
		disp2_val = LookUp(%11111111, %11111001, %10100100, %10110000, %10011001, %10010010, %10000010, %11111000, %10000000, %10010000, %10111111), disp2_val
		disp2_val.7 = version
	EndSelect
	I2CStart
	I2CSend disp1
	I2CSend disp1_val
	I2CStop
	I2CStart
	I2CSend disp2
	I2CSend disp2_val
	I2CStop
Return                                            

i2c_display_pgm:
	disp1_val = LookUp(%11000000, %11111001, %10100100, %10110000, %10011001, %10010010, %10000010, %11111000, %10000000, %10010000), program_set
	I2CStart
	I2CSend disp1
	I2CSend disp1_val
	I2CStop
	I2CStart
	I2CSend disp2
	I2CSend %10111111
	I2CStop
Return                                            

stepmotor:
	speed = 0
	Gosub steps_reading
	Select Case new_pos
	Case 62  'this is the manual setting for each position
'this position is equal for all control types, except for max switch
'in case of control type < 6 the zero switch is used for total steps calculation
'and bridge will run one full round extra.
		direction = 0  'first backward
		ena = 1  'motor on
		pos_match = 1  'not indicate pos_match
		Call run_to_zero_switch(1, 0)
		ena = 0  'motor off
		WaitMs 1000
		one_round = 0  'start counting steps to max pos
		ena = 1  'motor on
		act_pos = 1
		If ctrl_type < 4 Then  'now extra round to be made to calculate total steps
			Call run_to_zero_switch(0, 1)  'RUN TO ZERO SWITCH AND COUNT STEPS
			Call run_to_zero_switch(1, 1)  'RUN TO ZERO SWITCH AND COUNT STEPS
		Endif
		Write mem_one_round_lb, one_round.LB
		Write mem_one_round_hb, one_round.HB
		one_round_half = one_round / 2
		WaitMs 1000
		direction = 1  'run forward
		value = 0  'WE ARE IN ZERO POSITION NOW
		Gosub i2c_display
		one_round = 0  'temporary use variable to count individual postitions
		While act_pos <= max_pos  'turn motor until max pos is reached
				clk = 1  'clock out
				WaitUs speed_min
				clk = 0
				WaitUs speed_min
				one_round = one_round + 1
				If ecsw = 0 Then  'knob is pressed, save position and increase act_pos
					new_steps = one_round
					Gosub save_steps
					value = act_pos
					Gosub i2c_display
					act_pos = act_pos + 1
					WaitMs 500  'give some time to breeze
					While ecsw = 0  'Check If knob is Not pressed
					Wend
				Endif
				If ctrl_type = 4 Then  'for FY , max limit switch is to be checked.
					If max_end = 0 Then  'end switch for fy is touched, end cycle
						act_pos = max_pos + 1  'cancel the procedure
						ena = 0
					Endif
				Endif
		Wend
		Write mem_turn_counter, 127  'reset turn_counter
		Read mem_one_round_lb, one_round.LB  'put stored value back in one_round
		Read mem_one_round_hb, one_round.HB
		direction = 0  'goto to zero switch
		ena = 1  'set anable on
		pos_match = 1  'not indicate pos_match
		Call run_to_zero_switch(1, 0)
		WaitMs 1000
		act_pos = 0
		new_pos = 1
		Gosub steps_reading
		direction = 1  'now forwards
		steps = new_steps + free_space_steps
		ramp_up = 1
		ramp_down_point = 0
		half_steps = steps / 2
		ena = 1  'set Enable on
		pos_match = 1  'not indicate pos_match
		Call run_to_zero(speed_min, 0)
		WaitMs 1000  'NOW WAIT 1 SECOND BEFORE GO BACK
		steps = free_space_steps
		direction = 0  'now go back the free-space correction
		Call run_to_zero(speed_min, 0)
		ena = 0  'enable off
	Case 63  'go to zero switch and then to 1st rail position (Pos 1)
		direction = 0  'goto to zero switch
		ena = 1  'set anable on
		pos_match = 1  'not indicate pos_match
		Call run_to_zero_switch(1, 0)
		WaitMs 1000
		new_pos = 1
		Gosub steps_reading
		direction = 1  'now forwards
		steps = new_steps + free_space_steps
		ramp_up = 1
		ramp_down_point = 0
		half_steps = steps / 2
		speed = speed_min
		ena = 1  'set Enable on
		pos_match = 1  'not indicate pos_match
		Call run_to_zero(speed_min, 1)
		WaitMs 1000  'NOW WAIT 1 SECOND BEFORE GO BACK
		steps = free_space_steps
		direction = 0  'now go back the free-space correction
		Call run_to_zero(speed_min, 1)
		ena = 0  'enable off
	Case Else  'this is the normal position sequence
		steps = 0
		one_round_half = one_round / 2
		If new_steps > act_steps Then
			steps = new_steps - act_steps
			direction = 1
			Gosub check_0_pass_allowed
		Else
			steps = act_steps - new_steps
			direction = 0
			Gosub check_0_pass_allowed
		Endif
		If steps > 0 Then  'motor must be activated
			If direction = 1 Then
				steps = steps + free_space_steps
			Endif
			ramp_up = 1
			ramp_down_point = 0
			half_steps = steps / 2
			speed = speed_min
			ena = 1  'set anable on
			pos_match = 1  'not indicate pos_match
			While steps > 0  'NOW RUN UNTIL STEPS = 0
				clk = 1  'clock out
				WaitUs speed
				clk = 0
				WaitUs speed
				Gosub speed_calc
				steps = steps - 1
				If ctrl_type = 4 Then  'only for fiddle yard
					If direction = 1 Then
						If max_end = 0 Then
							Gosub stop_the_motor
						Endif
					Else
						If zero = 0 Then
							Gosub stop_the_motor
						Endif
					Endif
				Endif
			Wend
			If direction = 1 Then
				If free_space_steps > 0 Then
					WaitMs 1000  'NOW WAIT 1 SECOND BEFORE GO BACK
					steps = free_space_steps
					direction = 0  'now go back the free-space correction
					Call run_to_zero(speed_min, 0)
				Endif
			Endif
			ena = 0  'enable off
		Endif
	EndSelect
	act_pos = new_pos
	Write mem_act_pos, act_pos
	bus_pos = act_pos
	old_bus_pos = act_pos
	manual_pos = act_pos
	old_manual_pos = act_pos

Return                                            

check_0_pass_allowed:
	If steps > one_round_half Then
		Select Case ctrl_type
		Case 2  'crossing zero possible but only if not too many turns in one direction
			If steps > one_round_half Then
				Read mem_turn_counter, turn_counter  'get the total turns
				If turn_counter > 124 And direction = 1 Then  'extra round in direction 0 allowed
					turn_counter = turn_counter - 1  'substract 1 pass
					Write mem_turn_counter, turn_counter
					direction = 0  'change direction
					steps = one_round - steps  'set the number of steps for the other direction
				Else
					If turn_counter < 130 And direction = 0 Then  'extra round in direction 1 allowed
						turn_counter = turn_counter + 1
						Write mem_turn_counter, turn_counter  'add 1 pass
						direction = 1  'change direction
						steps = one_round - steps  'set the number of steps for the other direction
					Endif
				Endif
			Endif
		Case 3  'crossing zero always possible
			direction = Not direction  'change direction
			steps = one_round - steps  'set the number of steps for the other direction
		Case Else  'no zero crossing allowed
		EndSelect
	Endif
Return                                            


speed_calc:
	If steps < half_steps Then  'at this point  ramp up MUST end half way
		ramp_up = 0
	Endif
	If ramp_up = 1 Then  'we were supposed to ramp up
		If speed > speed_max Then
			speed = speed - speed_step
			ramp_down_point = ramp_down_point + 1  'this counts down with the steps
		Else
			ramp_up = 0
		Endif
	Else
		If steps < ramp_down_point Then  'this is the point tot start ramp down
			If speed < speed_min Then
				speed = speed + speed_step
			Endif
		Endif
	Endif
Return                                            



init_values:
	value = 71  'version number
	version = 0  'decimal point in display on
	Gosub i2c_display
	WaitMs 3000
	version = 1  'decimal point in display off
	encoder_count = 0
	speed_min = 0
	
	Read mem_ctrl_type, ctrl_type
	If ctrl_type > 4 Or ctrl_type = 0 Then
		ctrl_type = 1
		Write mem_ctrl_type, ctrl_type
	Endif
	
	Read mem_speed_min, speed_min.HB
	If speed_min.HB > 99 Then
		speed_min.HB = 30
	Else
		If speed_min.HB < 2 Then
			speed_min.HB = 20
		Endif
	Endif

	Write mem_speed_min, speed_min.HB
	speed_max = 0
	Read mem_speed_max, speed_max.HB
	If speed_max.HB > speed_min.HB Then
		speed_max.HB = 6
	Else
		If speed_max.HB < 2 Then
			speed_max.HB = 6
		Endif
	Endif
	Write mem_speed_max, speed_max.HB

	Read mem_one_round_lb, one_round.LB
	Read mem_one_round_hb, one_round.HB
	If one_round = 0 Then
		one_round = 8000
	Endif
	If one_round > 60000 Then
		one_round = 8000
	Endif
	one_round_half = one_round / 2

	Read mem_max_pos, max_pos
	If max_pos > 48 Or max_pos < 2 Then
		max_pos = 20
		bridge_rev_pos = 10
		Write mem_max_pos, max_pos
		Write mem_bridge_rev_pos, bridge_rev_pos
	Endif
	
	Read mem_bridge_rev_pos, bridge_rev_pos
	If bridge_rev_pos > max_pos Then
		bridge_rev_pos = max_pos / 2
		Write mem_bridge_rev_pos, bridge_rev_pos
	Endif
	
	Read mem_act_pos, act_pos
	If act_pos > max_pos Then
		act_pos = 1
		Write mem_act_pos, act_pos
	Endif
	value = act_pos
	Gosub i2c_display
	Read mem_free_space, free_space_steps
	If free_space_steps > 99 Then
		free_space_steps = 50
	Endif
	Read mem_pos_match_delay, pos_match_delay
	If pos_match_delay > 50 Then
		pos_match_delay = 10
	Endif
	
	Read mem_speed_step, speed_step
	If speed_step > 99 Then
		speed_step = 20
	Endif
	steps_set = 0
	new_pos = act_pos
	bus_pos = act_pos
	old_bus_pos = act_pos
	manual_pos = act_pos
	old_manual_pos = act_pos
	old_pos_write = 1
Return                                            

steps_reading:
	Select Case act_pos
	Case 63, 62
		act_steps = 0
	Case Else
		eprom_pointer = act_pos
		eprom_pointer = eprom_pointer + 48
		Read act_pos, act_steps.LB
		Read eprom_pointer, act_steps.HB
	EndSelect
	Select Case new_pos
	Case 62, 63
		new_steps = 0
	Case Else
		eprom_pointer = new_pos
		eprom_pointer = eprom_pointer + 48
		Read new_pos, new_steps.LB
		Read eprom_pointer, new_steps.HB
	EndSelect
Return                                            


save_steps:
	eprom_pointer = act_pos
	eprom_pointer = eprom_pointer + 48
	Write act_pos, new_steps.LB
	Write eprom_pointer, new_steps.HB
Return                                            


stop_the_motor:
	ena = 0  'set anable oFF
	value = act_pos
	version = 0  'decimal point in display on
	While ecsw = 1
		WaitMs 250
		Gosub i2c_display
		version = Not version
	Wend
	version = 1  'decimal point off
	Gosub i2c_display
	ena = 1  'set anable on
Return                                            
wait_pos_match:
	value = 99
	Gosub i2c_display
	steps = pos_match_delay * 100
	WaitMs steps
	value = act_pos
	Gosub i2c_display
	pos_match = 0  'indicate pos_match
Return                                            
Proc run_to_zero(speed As Word, calc As Bit)
	While steps > 0  'NOW RUN UNTIL STEPS = 0
		If calc = 1 Then
			Gosub speed_calc
		Endif
		
		clk = 1  'clock out
		WaitUs speed
		clk = 0
		WaitUs speed
		Gosub speed_calc
		steps = steps - 1
	Wend

End Proc                                          
Proc run_to_zero_switch(bitposition As Bit, counting As Bit)
	If counting = 1 Then
		While zero = bitposition  'NOW RUN UNTIL switch is activated
			clk = 1  'clock out
			WaitUs speed_max
			clk = 0
			WaitUs speed_max
			steps = steps - 1
			one_round = one_round + 1
		Wend
	Else
		While zero = bitposition  'NOW RUN UNTIL switch is activated
			clk = 1  'clock out
			WaitUs speed_max
			clk = 0
			WaitUs speed_max
			steps = steps - 1
		Wend
	Endif
End Proc                                          

Proc relays(onoff As Bit)
		If act_pos < bridge_rev_pos Or bridge_rev_pos = 0 Then  'just normal pridge power (if allowed)
			swfw = 0
			WaitMs 100  'just wait for the relay to drop
			If onoff = 0 Then
				swrev = bridge_power.0
			Else
				swrev = 1
			Endif
		Else  'inverse bridge power (if allowed)
			swrev = 0
			WaitMs 100  'just wait for the relay to drop
			If onoff = 0 Then
				swfw = bridge_power.0
			Else
				swfw = 1
			Endif
		Endif
End Proc                                          

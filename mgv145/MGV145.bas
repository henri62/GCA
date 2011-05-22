'program TT for PCBoard MGV145
'version V3-1  Fiddle Yard version, for turntables and fiddle yard
'this version has all possibilities changeable by menue's.
'V3-2 counter for twisting cables tored at eeprom pos zero.
'the value of the counter can be anything between 125 and 131. 128 is zero point.
'V3-3 eeprom startup improved. Reprogramming PIC can now be done without reprogramming data
'V3.6 startup delayed, to avoid wrong positions
'V3.10 extra separate positions for FY and TT with contact sliders.
Define CONF_WORD = 0x3f72
Define CLOCK_FREQUENCY = 20
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
Dim eprom_pointer1 As Byte
Dim eprom_pointer2 As Byte
Dim encoder_count As Byte
Dim turn_counter As Byte
Dim onoff As Bit
Dim max_pos As Byte
Dim disp1_val As Byte
Dim disp2_val As Byte
Dim ctrl_type As Byte
Dim old_pos_write As Bit
Dim ramp_up As Bit
Dim speed_step As Byte
Dim version As Bit
Const disp1 = 64
Const disp2 = 66
Const mem_turn_counter = 101
Const mem_act_pos = 102
Const mem_speed_min = 103
Const mem_speed_max = 104
Const mem_speed_step = 105
Const mem_one_round_lb = 106
Const mem_one_round_hb = 107
Const mem_ctrl_type = 108
Const mem_max_pos = 109
Const mem_free_space = 110
Symbol swfw = RB0
Symbol swrev = RB1
Symbol sck = RB2
Symbol sda = RB3
Symbol pos_match = RB4
Symbol pos_write = RB5
Symbol eep = RB6
Symbol max_end = RB7
Symbol direction = RC0
Symbol clk = RC1
Symbol ena = RC2
Symbol zero = RC3
Symbol ec1 = RC4
Symbol ec2 = RC5
Symbol ecsw = RC6
Symbol pgm = RC7

ADCON1 = %0110
TRISA = %111111
TRISB = %11101100
TRISC = %11111000
PORTB = %11101100
PORTC = %11111000
I2CPrepare sda, sck
Gosub init_values
value = 37  'version number
version = 0
Gosub i2c_display
WaitMs 3000
value = act_pos
version = 1
Gosub i2c_display
onoff = 1
Gosub relays
program_set = 4
main:
	If pgm = 0 Then
		Gosub adjust_settings
		WaitMs 500
	Endif

	Gosub get_new_position
	If bus_pos <> old_bus_pos Then
		onoff = 0
		Gosub relays
		pos_match = 1  'indicate not in position
		new_pos = bus_pos
		Gosub stepmotor
		value = act_pos
		Gosub i2c_display
		pos_match = 0  'indicate pos_match
		onoff = 1
	Else
		If ecsw = 0 Then  'only when knob is pushed
			If manual_pos <> old_manual_pos Then
				onoff = 0
				Gosub relays
				pos_match = 1  'indicate not in position
				old_manual_pos = manual_pos
				new_pos = manual_pos
				Gosub stepmotor
				value = act_pos
				Gosub i2c_display
				pos_match = 0  'indicate pos_match
				onoff = 1
			Endif
		Endif
	Endif
	WaitMs 10
	Gosub relays
	Goto main
End                                               


get_new_position:
	value = bus_pos  'remember the position
	If pos_write = 1 Then
		If old_pos_write = 0 Then  'react only once on rising edge
			old_pos_write = 1
			value = PORTA And 63  'now get value from J5
			If value > max_pos Then  'this is not allowed
				value = bus_pos  'put it back to old situation
			Endif
			If value = 0 Then  'do not accept 0
				value = bus_pos  'put it back to old situation
			Endif
			Gosub i2c_display
			If value = bus_pos Then  'no other position is set, RR needs pos_match changes
				pos_match = 1
				WaitMs 1000
				pos_match = 0
			Endif
		Endif
	Else
		old_pos_write = 0
	Endif
	bus_pos = value  'now take over the dseired position
	Gosub calc_rotation
	Select Case result
	Case 1
		If encoder_count < 4 Then
			encoder_count = encoder_count + 1
		Else
			encoder_count = 0
			manual_pos = manual_pos + 1
			If manual_pos > max_pos Then
				manual_pos = max_pos
			Endif
			value = manual_pos
			Gosub i2c_display
		Endif
	Case 2
		If encoder_count > 0 Then
			encoder_count = encoder_count - 1
		Else
			encoder_count = 4
			If manual_pos > 0 Then
				manual_pos = manual_pos - 1
			Endif
			value = manual_pos
			Gosub i2c_display
		Endif
	Case Else
	EndSelect
	WaitMs 1
	
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
	onoff = 0
	Gosub relays
	Gosub steps_reading
	Gosub i2c_display_pgm
	new_steps = act_steps  'nothing is changed yet
	Gosub i2c_display_pgm
	WaitMs 1500  'give some time for display to be read
	While pgm = 0
		If ecsw = 0 Then  'knob is pressed, adjust max positions
			program_set = program_set + 1
			If program_set > 7 Then
				program_set = 1
			Endif
			Gosub i2c_display_pgm
			WaitMs 1500  'give some time for display to be read
			While ecsw = 0  'be sure pushbutton is released
				WaitMs 250  'cancel contact dender
			Wend
		Endif
		Select Case program_set
		Case 1  'correction position
			value = act_pos
			Gosub i2c_display
			Gosub calc_rotation
			Select Case result
			Case 1
				If new_steps < 60000 Then
					ena = 1  'motor on
					new_steps = new_steps + 1
					direction = 1
					clk = 1  'clock out
					WaitMs 5
					clk = 0
					WaitMs 5
				Endif
			Case 2
				If new_steps > 0 Then
					ena = 1  'motor on
					new_steps = new_steps - 1
					direction = 0
					clk = 1  'clock out
					WaitMs 5
					clk = 0
					WaitMs 5
				Endif
			Case Else
			EndSelect
		Case 2  'setting of maximum positions
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
				If max_pos > 4 Then  'minum 4 positions
					max_pos = max_pos - 1
					Write mem_max_pos, max_pos
				Endif
			Case Else
			EndSelect
		Case 3  'setting control type
			ena = 0  'shut off motor
			If ctrl_type > 6 Then
				ctrl_type = 0
				Write mem_ctrl_type, ctrl_type
			Endif
			value = ctrl_type
			Gosub i2c_display
			Gosub calc_rotation
			Select Case result
			Case 1
				If ctrl_type < 6 Then
					ctrl_type = ctrl_type + 1
					Write mem_ctrl_type, ctrl_type
				Endif
			Case 2
				If ctrl_type > 0 Then
					ctrl_type = ctrl_type - 1
					Write mem_ctrl_type, ctrl_type
				Endif
			Case Else
			EndSelect
		Case 4  'setting of speed interval Minimum
					'setting will be done in multiple of 256 uSecs
					'minimum is 2 and maximum is 99 (is real slow)
			ena = 0  'shut off motor
			value = speed_min.HB
			Gosub i2c_display
			Gosub calc_rotation
			Select Case result
			Case 1
				If speed_min.HB < 99 Then  'display is limited to 99
					speed_min = speed_min + 256
					Write mem_speed_min, speed_min.HB
				Endif
			Case 2
				If speed_min > speed_max Then
					speed_min = speed_min - 256
					Write mem_speed_min, speed_min.HB
				Endif
			Case Else
			EndSelect
		Case 5  'setting of speed interval Maximum
					'setting will be done in multiple of 256 uSecs
					'minimum is 1 and maximum is 99 (is real slow)
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
				If speed_max.HB > 1 Then
					speed_max.HB = speed_max.HB - 1
					Write mem_speed_max, speed_max.HB
				Endif
			Case Else
			EndSelect
		Case 6  'setting of ramp up and down speed
					'speed will be the value that speed will be changed every step up and down
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
	
		Case Else  'setting of extra free space steps
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
		EndSelect
	Wend
	ena = 0  'shut off motor
		'***************   now check if changes have to BE written in eeprom
	If new_steps <> act_steps Then  'now save new position
		eprom_pointer1 = act_pos
		eprom_pointer2 = eprom_pointer1 + 48
		Write eprom_pointer1, new_steps.LB
		Write eprom_pointer2, new_steps.HB
		Gosub check_next_steps
	Endif
	value = act_pos
	Gosub i2c_display
	onoff = 1
	Gosub relays
Return                                            

i2c_display:
	disp1_val = value
	disp1_val = disp1_val Mod 10
	disp1_val = LookUp(%11000000, %11111001, %10100100, %10110000, %10011001, %10010010, %10000010, %11111000, %10000000, %10010000), disp1_val
	disp1_val.7 = pgm
	disp2_val = value
	disp2_val = disp2_val / 10
	disp2_val = LookUp(%11111111, %11111001, %10100100, %10110000, %10011001, %10010010, %10000010, %11111000, %10000000, %10010000, %10111111), disp2_val
	disp2_val.7 = version
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
	disp1_val = LookUp(%11000000, %11111001, %10100100, %10110000, %10011001, %10010010, %10000010, %11111000), program_set
	I2CStart
	I2CSend disp1
	I2CSend disp1_val
	I2CStop
	I2CStart
	I2CSend disp2
	I2CSend %10111111
	I2CStop
Return                                            

steps_reading:
	If act_pos = 0 Then
		act_steps = 0
	Else
		eprom_pointer1 = act_pos
		eprom_pointer2 = eprom_pointer1 + 48
		Read eprom_pointer1, act_steps.LB
		Read eprom_pointer2, act_steps.HB
	Endif
	If new_pos = 0 Then
		new_steps = 0
	Else
		eprom_pointer1 = new_pos
		eprom_pointer2 = eprom_pointer1 + 48
		Read eprom_pointer1, new_steps.LB
		Read eprom_pointer2, new_steps.HB
	Endif
Return                                            

stepmotor:
	speed = 0
	Gosub steps_reading
	If new_steps = 0 Then
		direction = 0
		ena = 1  'set anable on
		pos_match = 1  'not indicate pos_match
		While zero = 1  'turn motor until zero switch is activated
			clk = 1  'clock out
			WaitUs speed_max
			clk = 0
			WaitUs 5
		Wend
		act_pos = 0
		Write mem_act_pos, act_pos
		Select Case ctrl_type
		Case 2, 3, 4, 5  'NOW GO ONE MORE COMPLETE ROUND FOR COUNTING STEPS
			one_round = 0
			ena = 1  'set anable on
			While zero = 0  'turn motor until zero switch is DISactivated
				clk = 1  'clock out
				WaitUs speed_max
				clk = 0
				one_round = one_round + 1
				WaitUs 5
			Wend
			For value = 1 To 100  'SOME MORE STEPS TO AVOID CONTACT DENDER.
				clk = 1  'clock out
				WaitUs speed_max
				clk = 0
				WaitUs 5
				one_round = one_round + 1
			Next value
			While zero = 1  'turn motor until zero switch is activated
				clk = 1  'clock out
				WaitUs speed_max
				clk = 0
				WaitUs 5
				one_round = one_round + 1
			Wend
			one_round_half = one_round / 2
			Write mem_one_round_lb, one_round.LB
			Write mem_one_round_hb, one_round.HB
			act_pos = 0
			Write mem_act_pos, act_pos
		Case 6  'Fiddle yard control, go the other way to end switch.
			one_round = 0
			ena = 1  'set anable on
			direction = 1  'go To max position
			While max_end = 1  'turn motor until end switch activated
				clk = 1  'clock out
				WaitUs speed_max
				clk = 0
				one_round = one_round + 1
				WaitUs 5
			Wend
			Write mem_one_round_lb, one_round.LB
			Write mem_one_round_hb, one_round.HB
			act_pos = max_pos + 1
			If eep = 1 Then
				For value = 1 To max_pos
					steps = one_round / 12
					steps = value * steps
					eprom_pointer2 = value + 48
					Write value, steps.LB
					Write eprom_pointer2, steps.HB
				Next value
				value = max_pos + 1
				eprom_pointer2 = value + 48
				Write value, one_round.LB
				Write eprom_pointer2, one_round.HB
			Endif
			act_pos = 13
			Write mem_act_pos, act_pos
		Case Else
			act_pos = 0
			Write mem_act_pos, act_pos
		EndSelect
		ena = 0  'set anable off
	Else  'this is the normal position sequence
		steps = 0
		If new_steps > act_steps Then
			steps = new_steps - act_steps
			direction = 1
			If steps > one_round_half Then  'is other direction shorter?
				Select Case ctrl_type
				Case 2, 3
					If ctrl_type > 1 Then  'check If short way is set
						Read mem_turn_counter, turn_counter
						If turn_counter > 126 Then  'check if shortest way is allowed
							turn_counter = turn_counter - 1  'yes it is, so substract one pass
							steps = one_round - steps  'go shortest way
							direction = 0  'change  direction
							Write mem_turn_counter, turn_counter
						Endif
					Endif
				Case 4, 5  'type with contact sliders, no twisting cables
					steps = one_round - steps
					direction = 0
				Case Else  'no short way possible
				EndSelect
			Endif
		Else
			steps = act_steps - new_steps
			direction = 0
			If steps > one_round_half Then  'other direction shorter?
				Select Case ctrl_type
				Case 2, 3  'cable connected bridges
					Read mem_turn_counter, turn_counter
					If turn_counter < 130 Then  'check if shortest way is allowed
						turn_counter = turn_counter + 1  'yes it is, so add one pass
						steps = one_round - steps  'go shortest way
						direction = 1  'change direction
						Write mem_turn_counter, turn_counter
					Endif
				Case 4, 5  'type with contact sliders, no twisting cables
					steps = one_round - steps
					direction = 1
				Case Else  'no short way possible
				EndSelect
			Endif
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
				Gosub speed_calc
				steps = steps - 1
			Wend
			WaitMs 1000  'NOW WAIT 1 SECOND BEFORE GO BACK
			If direction = 1 Then
				steps = free_space_steps
				direction = 0  'now go back the free-space correction
				While steps > 0  'NOW RUN UNTIL STEPS = 0
					clk = 1  'clock out
					WaitUs speed_min
					clk = 0
					steps = steps - 1
					WaitUs 5
				Wend
			Endif
			ena = 0  'enable off
		Endif
		pos_match = 0  'indicate pos_match
	Endif
	act_pos = new_pos
	Write mem_act_pos, act_pos
	bus_pos = act_pos
	old_bus_pos = act_pos
	manual_pos = act_pos
	old_manual_pos = act_pos
Return                                            


speed_calc:
	If steps < half_steps Then  'at this point  ramp up MUST end
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


relays:
	If onoff = 1 Then
		value = max_pos + 1
		value = value / 2  'calulation of half max positions
		Select Case ctrl_type
		Case 1, 3, 5, 6
			swfw = 0
			swrev = 1
		Case Else
			If act_pos > value Then  'select polarity rails on bridge
				swfw = 0
				swrev = 1
			Else
				swrev = 0
				swfw = 1
			Endif
		EndSelect
	Else
		swfw = 0
		swrev = 0
	Endif
Return                                            


check_next_steps:
	'this is ment to check all following steps after readjusting one position.
	'simply to avoid that tt will go backwards when set for the next (higher) position.
	value = act_pos
	If value > 1 Then
		Read 2, new_steps.LB
		Read 50, new_steps.HB
		Read 1, act_steps.LB
		Read 49, act_steps.HB
		steps = new_steps - act_steps  'calculate the steps between 1 And 2 ,
												'steps will be approx equal between all positionseps
	Endif
	While value < max_pos
		value = value + 1
		eprom_pointer2 = value + 48
		Read value, new_steps.LB
		Read eprom_pointer2, new_steps.HB
		new_steps = new_steps + steps
		Write value, new_steps.LB
		Write eprom_pointer2, new_steps.HB
	Wend
Return                                            


init_eeprom:
			'check if acceptable value is in eeprom
	act_pos = 1
	Write mem_act_pos, 1  'act_pos
	act_steps = 0
		'check memory for correct total steps

	For eprom_pointer1 = 1 To max_pos
		new_steps = one_round / max_pos  'devide the steps between min and max
		act_steps = act_steps + new_steps
		eprom_pointer2 = eprom_pointer1 + 48
		Write eprom_pointer1, act_steps.LB
		Write eprom_pointer2, act_steps.HB
	Next eprom_pointer1
	free_space_steps = 20
	Write mem_free_space, free_space_steps
			'check for correct control type
			'now check for correct amount of 0-point passes

Return                                            
init_values:

	encoder_count = 0
	speed_min = 0
	Read mem_speed_min, speed_min.HB
	If speed_min.HB > 99 Then
		speed_min.HB = 99
	Else
		If speed_min.HB < 1 Then
			speed_min.HB = 1
		Endif
	Endif
	speed_max = 0
	Read mem_speed_max, speed_max.HB
	If speed_max.HB > speed_min.HB Then
		speed_max.HB = speed_min.HB
	Else
		If speed_max.HB < 1 Then
			speed_max.HB = 1
		Endif
	Endif

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
	If max_pos > 48 Then
		max_pos = 12
	Endif
	Read mem_act_pos, act_pos
	If act_pos > max_pos Then
		act_pos = 1
	Endif
	Read mem_free_space, free_space_steps
	If free_space_steps > 99 Then
		free_space_steps = 20
	Endif
	Read mem_ctrl_type, ctrl_type
	If ctrl_type > 6 Then
		ctrl_type = 6
	Endif

	Read mem_turn_counter, turn_counter
	If turn_counter > 131 Then  'in program impossible value
		turn_counter = 128
	Else
		If turn_counter < 125 Then  'in program impossible value
			turn_counter = 128
		Endif
	Endif
	
	Read mem_speed_step, speed_step
	If speed_step > 99 Then
		speed_step = 50
	Endif
	
	If eep = 1 Then  'jumper set for initialisation
		Gosub init_eeprom
	Endif

	new_pos = act_pos
	bus_pos = act_pos
	old_bus_pos = act_pos
	manual_pos = act_pos
	old_manual_pos = act_pos
Return                                            
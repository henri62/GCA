'program TT for PCBoard MGV145

Define CONF_WORD = 0x3f72

TRISA = %111111
TRISB = %11111100
TRISC = %01110000
PORTC = 0
Dim steps As Word
Dim new_steps As Word
Dim act_steps As Word
Dim new_pos As Byte
Dim act_pos As Byte
Dim one_round As Word
Dim bus_position As Byte
Dim old_bus_position As Byte
Dim speed As Word
Dim encoder As Byte
Dim old_encoder As Byte
Dim manual_pos As Byte
Dim old_manual_pos As Byte
Dim manual_result As Byte
Symbol direction = RC0
Symbol ecsw = RC6
one_round = 8000

main:
	Gosub get_bus_position
	If new_pos <> act_pos Then
		Gosub stepmotor
	Else
		Gosub calc_rotation
		If manual_pos <> old_manual_pos Then
			old_manual_pos = manual_pos
			new_pos = manual_pos
			Gosub stepmotor
		Endif
	Endif
Goto main
End                                               
stepmotor:
	act_pos = act_pos + 64
	Read act_pos, act_steps.HB
	act_pos = act_pos - 64
	Read act_pos, act_steps.LB
	new_pos = new_pos + 64
	Read new_pos, new_steps.HB
	new_pos = new_pos - 64
	Read new_pos, new_steps.LB
	If new_steps > act_steps Then
		steps = new_steps - act_steps
		If steps > one_round Then  'calculate shortest way
			steps = steps - one_round
			direction = 1  'go reverse through 0 point
		Else
			direction = 0  'go forward
		Endif
	Else
		steps = act_steps - new_steps
		If steps > one_round Then  'calculate shortest way
			steps = steps - one_round
			direction = 0  'go forward through 0 point
		Else
			direction = 1  'go reverse
		Endif
	Endif
	RC2 = 1  'SET SPEED TO HALFSTEP
	RC3 = 1  'set anable on
	RB0 = 1  'not indicate pos_match
	While steps > 0
		If steps > 500 Then  'increase speed
			speed = speed - 1
		Else
			If speed < 5000 Then  'slow down
				speed = speed + 1
			Endif
		Endif
		RC1 = 1  'clock out
		WaitUs speed
		RC1 = 0
		WaitUs speed
		steps = steps - 1
	Wend
	RC3 = 0  'enable off
	act_pos = new_pos
	manual_pos = new_pos
	old_manual_pos = new_pos
	RB0 = 0  'indicate pos_match
		
Return                                            
get_bus_position:
	bus_position = PORTA And 63
	If bus_position <> old_bus_position Then
		old_bus_position = bus_position
		new_pos = bus_position
	Else  'now manual control
		
	Endif

Return                                            
calc_rotation:
	encoder = PORTC And 64
	encoder = encoder / 16
	Select Case encoder
	Case 0
		Select Case old_encoder
		Case 2
			manual_result = manual_result + 1
		Case 1
			manual_result = manual_result - 1
		Case Else
		EndSelect
	Case 1
		Select Case old_encoder
		Case 0
			manual_result = manual_result + 1
		Case 3
			manual_result = manual_result - 1
		Case Else
		EndSelect
	Case 2
		Select Case old_encoder
		Case 3
			manual_result = manual_result + 1
		Case 0
			manual_result = manual_result - 1
		Case Else
		EndSelect
	Case Else
		Select Case old_encoder
		Case 1
			manual_result = manual_result + 1
		Case 2
			manual_result = manual_result - 1
		Case Else
		EndSelect
	EndSelect
	If manual_result = 0 Then
		manual_result = 63
	Endif
	If manual_result = 64 Then
		manual_result = 1
	Endif
	If ecsw = 0 Then
		manual_pos = manual_result
	Endif
		
Return                                            

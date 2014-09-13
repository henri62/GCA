'program for controlling 7 aspects for dutch signal
'VERSION 1.1
'green_led_enable = green
'yellow_led_enable = yellow
'red_led_enable = red
'aspects :

'000 red led On
'001 green led on
'010  yellow led On
'011  red led flash
'100  green led flash
'101  yellow led flash
'110  yellow + red led flash
'111  all leds 0ff

Define CONFIG = 0x31c4
TRISIO = %111000
AllDigital

GPIO = %111111
Dim aspect As Byte

Dim mask As Byte
Dim red_led_duty As Byte  'for bottom led
Dim yellow_duty As Byte  'for mid led
Dim green_duty As Byte  'for top led
Dim flashtime As Word
Dim green_led_enable As Bit
Dim yellow_led_enable As Bit
Dim red_led_enable As Bit
Symbol green_led = GP1
Symbol yellow_led = GP2
Symbol red_led = GP0
red_led_duty = 0
yellow_duty = 0
green_duty = 0
flashtime = 0
aspect = 0
main:
	aspect.0 = GP3
	aspect.1 = GP4
	aspect.2 = GP5
	Select Case aspect
	Case 0  'red led on
		red_led_enable = 1
		yellow_led_enable = 0
		green_led_enable = 0
	Case 1  'yellow led on
		red_led_enable = 0
		yellow_led_enable = 1
		green_led_enable = 0
	Case 2  'green led on
		red_led_enable = 0
		yellow_led_enable = 0
		green_led_enable = 1
	Case 3  'bottom led flashes
		Call count_flash(1)
	Case 4  'top led flashes
		Call count_flash(2)
	Case 5  'mid led flashes
		Call count_flash(3)
	Case 6  'bottom + mid led flash
		Call count_flash(4)
	Case Else
		red_led_enable = 0
		yellow_led_enable = 0
		green_led_enable = 0
	EndSelect
	Call dutycycle()
Goto main
End                                               
Proc dutycycle()
	
	If red_led_enable = 0 Then  'dim down bottom led
		If red_led_duty > 0 Then
			red_led_duty = red_led_duty - 1
		Endif
	Else
		If red_led_duty < 50 Then
			red_led_duty = red_led_duty + 1
		Endif
	Endif
	If yellow_led_enable = 0 Then  'dim down MID led
		If yellow_duty > 0 Then
			yellow_duty = yellow_duty - 1
		Endif
	Else
		If yellow_duty < 50 Then
			yellow_duty = yellow_duty + 1
		Endif
	Endif
	If green_led_enable = 0 Then  'dim down top led
		If green_duty > 0 Then
			green_duty = green_duty - 1
		Endif
	Else
		If green_duty < 50 Then
			green_duty = green_duty + 1
		Endif
	Endif

	For mask = 1 To 50  '.    generate complete duty cycle for each output
		If mask < red_led_duty Then
			red_led = 0
		Else
			red_led = 1
		Endif
		If mask < yellow_duty Then
			yellow_led = 0
		Else
			yellow_led = 1
		Endif
		If mask < green_duty Then
			green_led = 0
		Else
			green_led = 1
		Endif
		WaitUs 30
	Next mask
End Proc                                          
Proc count_flash(flash As Byte)
		flashtime = flashtime + 1
		Select Case flashtime
		Case 500
			flashtime = 0
			Select Case flash
			Case 1
				green_led_enable = 0
				yellow_led_enable = 0
				red_led_enable = 1
			Case 2
				green_led_enable = 1
				yellow_led_enable = 0
				red_led_enable = 0
			Case 3
				green_led_enable = 0
				yellow_led_enable = 1
				red_led_enable = 0
			Case 4
				green_led_enable = 0
				yellow_led_enable = 0
				red_led_enable = 1  'toggle with top led
			Case Else
			EndSelect
		Case 250
			Select Case flash
			Case 1, 2, 3
				green_led_enable = 0
				yellow_led_enable = 0
				red_led_enable = 0
			Case Else
				green_led_enable = 1  'toggle with red_led_enable
				red_led_enable = 0
				yellow_led_enable = 0
			EndSelect
		Case Else
		EndSelect
End Proc                                          
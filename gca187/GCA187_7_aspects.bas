'program for controlling 7 aspects for dutch signal
'top_led = green
'mid_led = yellow
'bottom_led = red
Define CONFIG = 0x31c4

AllDigital
TRISIO = %111000
GPIO = %111111
Dim aspect As Byte
Dim flash As Byte
Dim mask As Byte
Dim mask0 As Byte
Dim mask1 As Byte
Dim mask2 As Byte
Dim top_led As Bit
Dim mid_led As Bit
Dim bottom_led As Bit
Dim flashtime As Word
Symbol green_led = GP1
Symbol yellow_led = GP0
Symbol red_led = GP2
mask0 = 0
mask1 = 0
mask2 = 0
main:
	aspect = GPIO
	aspect = aspect / 8  'shift 3 bits
	Select Case aspect
	Case 1
		top_led = 1  'top led on
		mid_led = 0  'mid led off
		bottom_led = 0  'bottom led off
		flash = 0
	Case 2
		top_led = 0
		mid_led = 1
		bottom_led = 0
		flash = 0
	Case 3
		top_led = 0
		mid_led = 0
		bottom_led = 1
		flash = 0
	Case 4
		flash = 2  'top led flashes
		mid_led = 0
		bottom_led = 0
	Case 5
		flash = 1  'mid led flashes
		top_led = 0
		bottom_led = 0
	Case 6
		flash = 4  'bottom led flashes
		top_led = 0
		mid_led = 0
	Case 7
		flash = 7  'all leds flash
	Case Else
		flash = 0
		top_led = 0
		mid_led = 0
		bottom_led = 0
	EndSelect
	Call count_flash()
	Call dutycycle()
Goto main
End                                               
Proc dutycycle()
	
	If top_led = 0 Then  'dim down top led
		If mask0 > 0 Then
			mask0 = mask0 - 1
		Endif
	Else
		If mask0 < 50 Then
			mask0 = mask0 + 1
		Endif
	Endif
	If mid_led = 0 Then  'dim down MID led
		If mask1 > 0 Then
			mask1 = mask1 - 1
		Endif
	Else
		If mask1 < 50 Then
			mask1 = mask1 + 1
		Endif
	Endif
	If bottom_led = 0 Then  'dim down BOTTOM led
		If mask2 > 0 Then
			mask2 = mask2 - 1
		Endif
	Else
		If mask2 < 50 Then
			mask2 = mask2 + 1
		Endif
	Endif

	For mask = 0 To 51  '.    generate complete duty cycle for each output
		If mask0 > mask Then
			green_led = 0
		Else
			green_led = 1
		Endif
		If mask1 > mask Then
			yellow_led = 0
		Else
			yellow_led = 1
		Endif
		If mask2 > mask Then
			red_led = 0
		Else
			red_led = 1
		Endif
		WaitUs 5
	Next mask
End Proc                                          
Proc count_flash()
	If flash = 0 Then
		Exit
	Endif
	flashtime = flashtime + 1
	If flashtime = 3000 Then
		flashtime = 0
		top_led = 0
		mid_led = 0
		bottom_led = 0
	Else
		If flashtime = 1500 Then
			top_led = flash.0
			mid_led = flash.1
			bottom_led = flash.2
		Endif
	Endif

End Proc                                          
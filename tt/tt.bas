Define CONF_WORD = 0x3f3a
AllDigital
ADCON1 = 7
TRISA = %00000000
TRISB = %11110111  'set portb as output
PORTB = %11111111
TRISC = %00000100
PORTC = %00000100

Symbol ir_receiver = RB0
Symbol nw_pos = RB1  'nieuw commando
Symbol bit5 = RB2
Symbol led_pos_ok = RB3  'positie bereikt led aan/uit
Symbol dat_in = PORTB  'bits 7 6 5 4 positie nibble
Symbol pos_ack = RC3
Symbol end_switch = RC2
Symbol relais_1 = RC0
Symbol relais_2 = RC1


Define STEP_A_REG = PORTA
Define STEP_A_BIT = 0
Define STEP_B_REG = PORTA
Define STEP_B_BIT = 2
Define STEP_C_REG = PORTA
Define STEP_C_BIT = 1
Define STEP_D_REG = PORTA
Define STEP_D_BIT = 3
Define STEP_MODE = 2

Define LCD_LINES = 2
Define LCD_CHARS = 8
Define LCD_BITS = 4
Define LCD_DREG = PORTC
Define LCD_DBIT = 4
Define LCD_RSREG = PORTA
Define LCD_RSBIT = 4
Define LCD_EREG = PORTA
Define LCD_EBIT = 5
Define LCD_DATAUS = 200

Dim commnd As Byte
Dim sst As Byte
Dim n As Byte
Dim t As Byte
Dim q As Byte
Dim value As Bit
Dim end_sw As Bit
Dim ok_button As Byte
Dim ir_ok As Bit
Dim toggbit As Bit
Dim tientallen As Bit
Dim eenheden As Bit
Dim keuze As Byte
Dim keuze2 As Byte
Dim remote_mode As Byte
Dim plaats_nieuw As Word
Dim plaats_oud As Word
Dim verschil As Word
Dim pulsduur1 As Word
Dim pulsduur2 As Word
Dim temp As Byte
Dim temppos As Word
Dim lastpos As Byte
Dim turn_round As Word
Dim turn_round_2 As Word
Dim tracks As Byte


Lcdinit 0
Lcdcmdout LcdCurOff
Lcdcmdout LcdClear
Lcdcmdout LcdLine1Home
Lcdout "Turntabl"
Lcdcmdout LcdLine2Home
Lcdout "e V1 RR"
WaitMs 500
Lcdcmdout LcdClear

INTCON.INTE = 1
OPTION_REG.INTEDG = 0

StepHold



tracks = 31
Read 0x70, lastpos
If lastpos < 0xff Then
keuze = lastpos
n = 2 * keuze
Read n, plaats_oud.HB
n = n + 1
Read n, plaats_oud.LB
Else
keuze = 0
plaats_oud = 30000
Endif


Read 0x71, remote_mode  '51 = hex 33
If remote_mode > 10 Then
remote_mode = 0
Write 0x71, remote_mode
Endif


Lcdcmdout LcdLine1Home
Lcdout "remote  "
Lcdcmdout LcdLine2Home
Select Case remote_mode
	Case 0
		Lcdout "TV"
		ok_button = 14
	Case 5
		Lcdout "VCR"
		ok_button = 55
	Case 8
		Lcdout "SAT"
	ok_button = 49
EndSelect
WaitMs 500
Lcdcmdout LcdClear

pulsduur1 = 11000
Write 0x72, pulsduur1.HB
Write 0x73, pulsduur1.LB
Read 0x72, pulsduur1.HB  'hex 35
Read 0x73, pulsduur1.LB
pulsduur2 = 6000
Write 0x76, pulsduur2.HB
Write 0x77, pulsduur2.LB
Read 0x76, pulsduur2.HB  'hex 39
Read 0x77, pulsduur2.LB  'hex 4A



Read 0x74, turn_round.HB
Read 0x75, turn_round.LB

If turn_round = 0 Or turn_round > 65530 Then Gosub ijken
turn_round_2 = turn_round / 2

Lcdcmdout LcdLine1Home
Lcdout "Press OK"
WaitMs 500

Enable
If sst = remote_mode And commnd = 13 Then  'mute ingedrukt dan naar plaats 0
	Gosub track_one
Else
	Lcdcmdout LcdLine1Home
	Lcdout "Press OK"
Endif
Disable

For q = 0x00 To 0x40  'leest posities uit eeprom als >65000 dan zet 30000 neer
	Read q, temppos.HB
	q = q + 1
	Read q, temppos.LB
	If temppos > 65000 Then
		q = q - 1
		Write q, 0x75
		q = q + 1
		Write q, 0x30
	Endif
Next q

end_sw = False
eenheden = False
tientallen = False



Lcdcmdout LcdClear
Lcdcmdout LcdLine1Home
Lcdout "on track"
Lcdcmdout LcdLine2Home
Lcdout " ", #keuze

'For n = 0 To 64
't = n + 2
'Read t, temp
'Write n, temp
'Next n

keuze2 = 99
Gosub start

End                                               
start:  '******************************s t a r t ***********************
commnd = 99
INTCON.INTF = 0
Enable


While nw_pos = 0
	If tientallen = False And keuze <> keuze2 Then
		Lcdcmdout LcdClear
		Lcdcmdout LcdLine1Home
		Lcdout #keuze, " next^"
		Lcdcmdout LcdLine2Home
		Lcdout " setup >"
	Endif

	If sst = remote_mode And commnd = 16 And ir_ok = True Then  '-> ga naar setup
		Disable
		Goto setup
		INTCON.INTF = 0
		Enable
	Endif

	If sst = remote_mode And commnd = 17 And ir_ok = True Then  '< - ga naar ijken
		Disable
		Gosub ijken
		INTCON.INTF = 0
		Enable
	Endif


	If commnd = 12 And ir_ok = True Then  'druk op tv/sat/vcr en daarna op uit voor mode switch
		Disable
		ir_ok = False
		Lcdcmdout LcdClear
		Lcdcmdout LcdLine1Home
		Select Case sst
			Case 0
				remote_mode = 0
				ok_button = 14
				Lcdout "TV"
			Case 5
				remote_mode = 5
				ok_button = 55
				Lcdout "VCR"
			Case 8
				remote_mode = 8
				ok_button = 49
				Lcdout "SAT"
		EndSelect
		WaitMs 1000
		Write 0x71, remote_mode
		INTCON.INTF = 0
		commnd = 99
		Enable
	Endif

	If sst = remote_mode And commnd = 13 And ir_ok = True Then  'mute ga naar spoor 0 en ijk
		Disable
		Gosub track_one
		Enable
	Endif

	If sst = remote_mode And commnd = 32 And ir_ok = True Then  '^
		keuze = keuze + 1
		If keuze > tracks Then keuze = 0
		Gosub draaien
		ir_ok = False
	Endif

	If sst = remote_mode And commnd = 33 And ir_ok = True Then  'v
		keuze = keuze - 1
		If keuze = 0xff Then keuze = tracks
		Gosub draaien
		ir_ok = False
	Endif


	If sst = remote_mode And commnd < 11 And ir_ok = True Then
				
				If eenheden = True Then
					keuze = keuze + commnd
					If keuze > tracks Then keuze = tracks
					Gosub draaien
					commnd = 99
					eenheden = False
				Endif

				If tientallen = True Then
					keuze = 10 * commnd
					tientallen = False
					eenheden = True
					Lcdcmdout LcdLine1Home
					Lcdout "to tr. ", #commnd, "."

				Endif

				If commnd = 10 Then
					Lcdcmdout LcdLine1Home
					Lcdout "to tr.", ".."
					eenheden = False
					tientallen = True
				Endif

				If tientallen = False And eenheden = False And commnd < 10 Then
						keuze = commnd
						Gosub draaien
				Endif

						ir_ok = False
	Endif
keuze2 = keuze

Wend

While nw_pos = 1  'het nieuwe adres mag gebruikt worden als dit bit naar 0 gaat
Wend

Disable
n = 0
pos_ack = 0
opnieuw:
keuze = ShiftRight(dat_in, 4)
keuze = keuze And 0xf
If bit5 = 1 Then keuze = keuze + 16
WaitMs 10
keuze2 = ShiftRight(dat_in, 4)
keuze2 = keuze2 And 0xf
If bit5 = 1 Then keuze2 = keuze2 + 16
n = n + 1
If n > 100 Or keuze = 0 Then Goto start
If keuze2 <> keuze Then Goto opnieuw  'RR stuurt positie 0 na een reset


keuze = Not keuze
keuze = keuze And 0x1f

Gosub draaien
Goto start
Return                                            

setup:  '****************** s e t u p  ************************************

keuze = 0
Gosub draaien

led_pos_ok = 1
Lcdcmdout LcdClear
Lcdcmdout LcdLine1Home
Lcdout #keuze, " next^"
Lcdcmdout LcdLine2Home
Lcdout " <move>"
ir_ok = False

loop:

		While sst = remote_mode And commnd = 16 And ir_ok = True  'linksom
			Disable
			Lcdcmdout LcdLine1Home
			Lcdout "Mem:", #keuze, "       "
			Lcdcmdout LcdLine2Home
			Lcdout "right >>"
			If plaats_nieuw > 1 Then
				plaats_nieuw = plaats_nieuw - 1
				StepCW 1, 9000
			Endif
			Enable
		Wend

		While sst = remote_mode And commnd = 17 And ir_ok = True  'rechtsom
			Disable
			Lcdcmdout LcdLine1Home
			Lcdout "Mem:", #keuze, "       "
			Lcdcmdout LcdLine2Home
			Lcdout "<<  left"
			If plaats_nieuw < 65500 Then
			plaats_nieuw = plaats_nieuw + 1
			StepCCW 1, 3000
			Endif
			Enable
		Wend
	



		If sst = remote_mode And commnd = ok_button And ir_ok = True Then  'als op ok gedrukt wordt, wordt plaats en keuze opgeslagen en keuze met 1 verhoogt
			Disable
			n = 2 * keuze  'keuze loopt van 0 tot en met 31
			Write n, plaats_nieuw.HB
			n = n + 1
			Write n, plaats_nieuw.LB
			Lcdcmdout LcdLine1Clear
			Lcdcmdout LcdLine1Home
			Lcdout #keuze, "saved"
			keuze = keuze + 1
			If keuze > tracks Then keuze = 0
			WaitMs 750
			Lcdcmdout LcdLine1Home
			Lcdout "Mem nx", #keuze, " "
			ir_ok = False
			INTCON.INTF = 0
			Enable
		Endif


	
		If sst = remote_mode And commnd = 32 And ir_ok = True Then  'verhoog keuze
			Disable
			keuze = keuze + 1
			If keuze > tracks Then keuze = 0
			Lcdcmdout LcdLine1Home
			Lcdout "Mem:", #keuze, "     "
			ir_ok = False
			INTCON.INTF = 0
			Enable
		Endif

		If sst = remote_mode And commnd = 33 And ir_ok = True Then  'verlaag keuze
			Disable
			keuze = keuze - 1
			If keuze > 32 Then keuze = tracks
			Lcdcmdout LcdLine1Home
			Lcdout "Mem:", #keuze, "     "
			ir_ok = False
			INTCON.INTF = 0
			Enable
		Endif

		
		If sst = remote_mode And commnd = 12 And ir_ok = True Then  'uit  ga setup verlaten
			Disable
			plaats_oud = plaats_nieuw
			keuze = 0
			ir_ok = False
			Gosub draaien
			Goto start
		Endif

Goto loop  '********************end loop**************************************


ijken:
led_pos_ok = 0
Lcdcmdout LcdClear
Lcdcmdout LcdLine1Home
Lcdout "ijken"
Lcdcmdout LcdLine2Home
Lcdout " wait"
pulsduur1 = pulsduur1 - 5000

For n = 1 To 100
If end_switch = 0 Then
StepCCW 1, pulsduur1
Endif
Next n

While end_switch = 1
	StepCW 1, pulsduur1
Wend

turn_round = 0
Lcdcmdout LcdLine2Home
Lcdout " start  "

For n = 1 To 100
	StepCW 1, pulsduur1
	turn_round = turn_round + 1
Next n

While end_switch = 1
	StepCW 1, pulsduur1
	turn_round = turn_round + 1
Wend

Lcdcmdout LcdLine1Home
Lcdout "on track"
Lcdcmdout LcdLine2Home
Lcdout "0  OK"
WaitMs 1000
plaats_nieuw = 30000
plaats_oud = plaats_nieuw
led_pos_ok = 1
pos_ack = 1
Read 0x72, pulsduur1.HB  'hex 35
Read 0x73, pulsduur1.LB
Write 0x74, turn_round.HB
Write 0x75, turn_round.LB


Write 0x70, 0  'ópslaan laatste plaats
Write 0x00, 0x75
Write 0x01, 0x30
keuze = 0
commnd = 99
ir_ok = False
Enable
Return                                            

draaien:  '******************* d r a a i e n **************************
Disable
n = 2 * keuze
Read n, plaats_nieuw.HB
n = n + 1
Read n, plaats_nieuw.LB


led_pos_ok = 0
Lcdcmdout LcdClear
Lcdcmdout LcdLine1Home
Lcdout "to track"
Lcdcmdout LcdLine2Home
Lcdout " ", #keuze


If plaats_oud > plaats_nieuw Then
	verschil = plaats_oud - plaats_nieuw
	If verschil > 10 Then pulsduur1 = pulsduur1 - 4000
	
	If verschil > turn_round_2 Then
			verschil = turn_round - plaats_oud + plaats_nieuw  '+1
		
			While verschil > 0
				StepCCW 1, pulsduur1
				verschil = verschil - 1
			Wend
	Else
			While verschil > 0
					StepCW 1, pulsduur1
					verschil = verschil - 1
			Wend
	Endif
Endif


If plaats_oud < plaats_nieuw Then
	verschil = plaats_nieuw - plaats_oud
	If verschil > 10 Then pulsduur1 = pulsduur1 - 4000
	If verschil > turn_round_2 Then
		verschil = plaats_nieuw - plaats_oud
		toggbit = False
		While verschil > 0
			If end_switch = 0 And toggbit = False Then
				verschil = 30000 + turn_round - plaats_nieuw
				Lcdcmdout LcdLine2Home
				Lcdout "  ", #verschil
				toggbit = True
			Endif
			StepCW 1, pulsduur1
			verschil = verschil - 1
		Wend
	Else
		While verschil > 0
			StepCCW 1, pulsduur1
			verschil = verschil - 1
		Wend
	Endif
Endif

plaats_oud = plaats_nieuw
led_pos_ok = 1
pos_ack = 1
Read 0x72, pulsduur1.HB  'hex 35
Read 0x73, pulsduur1.LB

Write 0x70, keuze  'ópslaan laatste plaats
Lcdcmdout LcdClear
Lcdcmdout LcdLine1Home
Lcdout "on track"
Lcdcmdout LcdLine2Home
Lcdout " ", #keuze

ir_ok = False
INTCON.INTF = 0
Enable
Return                                            
On Interrupt
led_pos_ok = 0
Save System
loopir:
If ir_receiver = 1 Then Goto loopir
WaitUs 100
If ir_receiver = 1 Then Goto loopir

Gosub leesir

If commnd.6 = 0 Then Goto skip
commnd = commnd And 63
sst = sst And 31
ir_ok = True

skip:
INTCON.INTF = 0
led_pos_ok = 1

Resume                                            
bitophalen:
value = ir_receiver
WaitUs 1800
Return                                            

leesir:
WaitUs 1000  '1100us
commnd = 0
sst = 0
Gosub bitophalen  'bit2
commnd.6 = value
Gosub bitophalen  'bit3
toggbit = value
Gosub bitophalen
sst.4 = value
Gosub bitophalen
sst.3 = value
Gosub bitophalen
sst.2 = value
Gosub bitophalen
sst.1 = value
Gosub bitophalen
sst.0 = value
Gosub bitophalen
commnd.5 = value
Gosub bitophalen  'bit10
commnd.4 = value
Gosub bitophalen
commnd.3 = value
Gosub bitophalen
commnd.2 = value
Gosub bitophalen
commnd.1 = value
Gosub bitophalen  'bit14
commnd.0 = value
Return                                            
track_one:
Lcdcmdout LcdClear
Lcdcmdout LcdLine1Home
Lcdout " moving"

While end_switch = 1
	StepCW 1, 6000
Wend
	keuze = 0
	plaats_oud = 30000
Lcdcmdout LcdClear
Lcdcmdout LcdLine1Home
Lcdout #keuze, " next^"
Lcdcmdout LcdLine2Home
Lcdout " setup >"
commnd = 99
Return                                            





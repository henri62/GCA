Define CONF_WORD = 0x3f3a

AllDigital
Symbol ena_ble = RA2
Symbol dat_in = PORTB  'bits 7 6 5 4 positie nibble
Symbol nw_pos = RC2  'nieuw commando
Symbol led_pos_ok = RC3  'positie bereikt led aan/uit
Symbol button_save = RC4
Symbol button_links = RC5
Symbol button_rechts = RC6
Symbol button_setup = RC7
Symbol button_next = RA3
Symbol cw_ccw = RA1
Symbol cl_puls = RA0

Dim positie(16) As Word
Dim lastpos As Byte
Dim temppos As Word
Dim plaats_nieuw As Word
Dim plaats_oud As Word
Dim speed As Word
Dim verschil As Long
Dim keuze As Byte
Dim temp As Byte
Dim keuze2 As Byte
Dim puls As Word
Dim n As Byte
Dim t As Byte
Dim q As Byte
Dim nul As Byte
Dim l As Word
Dim teller As Byte
Dim l297enable As Byte
Dim aantal_sporen As Byte
Dim pulsduur As Word
Dim puls_norm As Word
Dim puls_verschil As Word
Dim pulsduur1 As Word


ADCON1 = 6

TRISA = %00001000
TRISB = %11110000
TRISC = %11110111



Define LCD_LINES = 2
Define LCD_CHARS = 16
Define LCD_BITS = 4
Define LCD_DREG = PORTB
Define LCD_DBIT = 0
Define LCD_RSREG = PORTA
Define LCD_RSBIT = 4
Define LCD_EREG = PORTA
Define LCD_EBIT = 5

Lcdinit 0
Lcdcmdout LcdCurOff
Lcdcmdout LcdClear
Lcdcmdout LcdLine1Home
Lcdout " RR FiddleYard  "
Lcdcmdout LcdLine2Home
Lcdout "LCD Version 0.1 "

For n = 1 To 4
WaitMs 1000
Next n

For n = 60 To 75
	Read n, temp
		If temp > 1 Then
			t = 2 * (n - 60)
			Write t, 117
			t = t + 1
			Write t, 48
		Endif
Next n

aantal_sporen = 15
plaats_oud = 30000
speed = 250
teller = 0
puls_norm = 1250
puls_verschil = 12800

Read 50, l297enable
If l297enable = 5 Then
	ena_ble = 1  'dan stepper niet permanent  bekrachtigd
Else
	ena_ble = 0
Endif

Read 56, teller
If teller > 15 Then teller = 10
pulsduur1 = puls_norm + (teller * speed)

begin:
q = aantal_sporen * 2
t = 1
For n = 0 To q
	Read n, temppos.HB
	n = n + 1
	Read n, temppos.LB
	positie(t) = temppos.LB + (temppos.HB * 256)  'keuze 1 zit op geh.pl 0 en 1 en is positie 1
	t = t + 1
Next n

If button_setup = 0 Then Gosub setup
Lcdcmdout LcdClear
Lcdcmdout LcdLine1Home
Lcdout "Operating"
Lcdcmdout LcdLine2Home
Lcdout "Positie : "

Read 51, lastpos
If lastpos > 0 And lastpos < 16 Then
	keuze = lastpos
	plaats_oud = positie(keuze)
	Gosub lcdkeuze
Endif

start:
While nw_pos = 0  'als dit bit laag  is wacht op een nieuw adres
	If button_setup = 0 Then Gosub setup
	If button_setup = 1 And button_rechts = 0 Then
		keuze = keuze + 1
		If keuze > aantal_sporen Then keuze = 1
		While button_rechts = 0
		Wend
		Goto start2
	Endif

	If button_setup = 1 And button_links = 0 Then
		keuze = keuze - 1
		If keuze < 1 Then keuze = aantal_sporen
		While button_links = 0
		Wend
		Goto start2
	Endif

	If button_setup = 1 And button_save = 0 And button_next = 0 Then  'save snelheid
		Read 56, teller
		teller = teller + 1
		If teller > 15 Then teller = 0
		Write 56, teller
		Lcdcmdout LcdLine1Home
		Lcdout "  S A V E D    "
		Lcdcmdout LcdLine2Home
		Lcdout "Speed   : "
		Lcdcmdout LcdLine2Pos(11)
		Lcdout "    "
		Lcdcmdout LcdLine2Pos(11)
		Lcdout #teller
		pulsduur1 = puls_norm + (teller * speed)
		While button_save = 0
		Wend
		Lcdcmdout LcdLine1Home
		Lcdout "Operating     "
	Endif
	
	If button_setup = 1 And button_next = 0 Then  'laat snelheid zien
		Read 56, teller
		Lcdcmdout LcdLine2Home
		Lcdout "Speed   : "
		If temp <> teller Then
			Lcdcmdout LcdLine2Pos(11)
			Lcdout "    "
		Endif
		Lcdcmdout LcdLine2Pos(11)
		Lcdout #teller
		temp = teller
	Endif

	If button_setup = 1 And button_next = 1 Then  'laat keuze zien
		Read 51, keuze
		Lcdcmdout LcdLine2Home
		Lcdout "Positie : "
		If temp <> keuze Then
			Lcdcmdout LcdLine2Pos(11)
			Lcdout "    "
		Endif
		temp = keuze
		Lcdcmdout LcdLine2Pos(11)
		Gosub lcdkeuze
	Endif
Wend

While nw_pos = 1  'het nieuwe adres mag gebruikt worden als dit bit naar 1 gaat
	If button_setup = 0 Then Goto setup
Wend

n = 0
opnieuw:
keuze = ShiftRight(dat_in, 4)
WaitMs 10
keuze2 = ShiftRight(dat_in, 4)
n = n + 1
If n > 50 Or keuze = 0 Then Goto start
If keuze2 <> keuze Then Goto opnieuw  'RR stuurt positie 0 na een reset

start2:
pulsduur1 = puls_norm + (teller * speed)
keuze = keuze And 0xf

Lcdcmdout LcdClear
Lcdcmdout LcdLine1Home
Lcdout "Operating"
Lcdcmdout LcdLine2Home
Lcdout "Positie :     "
Gosub lcdkeuze

puls = 0
plaats_nieuw = positie(keuze)
led_pos_ok = 0
If l297enable = 5 Then ena_ble = 1  'dan stepper niet  bekrachtigd

l = Sqr(puls_verschil)

If plaats_oud > plaats_nieuw Then
	verschil = plaats_oud - plaats_nieuw
	pulsduur = pulsduur1
	While verschil > 0
		If verschil < l Then pulsduur = pulsduur1 + (l - verschil) * (l - verschil)  'trager
		If puls < l Then pulsduur = pulsduur1 + (l - puls) * (l - puls)  'sneller tot pulsduur1
		ena_ble = 0
		Gosub steplinks  '<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<-
		verschil = verschil - 1
		puls = puls + 1
	Wend
	Endif

If plaats_oud < plaats_nieuw Then
	verschil = plaats_nieuw - plaats_oud
	pulsduur = pulsduur1
	While verschil > 0
		If verschil < l Then pulsduur = pulsduur1 + (l - verschil) * (l - verschil)  'trager
		If puls < l Then pulsduur = pulsduur1 + (l - puls) * (l - puls)  'sneller tot pulsduur1
		ena_ble = 0
		Gosub steprechts  '->>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		verschil = verschil - 1
		puls = puls + 1
	Wend
Endif

plaats_oud = plaats_nieuw
led_pos_ok = 1
Write 51, keuze
Goto start
End                                               

setup:

Lcdcmdout LcdClear
Lcdcmdout LcdLine1Home
Lcdout "Setup  "
Lcdcmdout LcdLine2Home
Lcdout "Positie : "

keuze = 8
plaats_nieuw = 30000


While button_setup = 0
	pulsduur = puls_norm + (13 * speed) + puls_verschil

	Gosub lcdkeuze  'laat keuze zien

	While button_rechts = 0 And button_links = 0
		Read 50, l297enable
		If l297enable = 5 Then
			l297enable = 10  'dan is stepper niet  bekrachtigd
			ena_ble = 0
			Lcdcmdout LcdLine2Home
			Lcdout "Enabled is  ON  "
		Else
			l297enable = 5
			ena_ble = 1
			Lcdcmdout LcdLine2Home
			Lcdout "Enabled is  OFF "
		Endif
		Write 50, l297enable
		While button_rechts = 0 And button_links = 0
		Wend
		Lcdcmdout LcdClear
		Lcdcmdout LcdLine1Home
		Lcdout "Setup  "
		Lcdcmdout LcdLine2Home
		Lcdout "Positie : "
	Wend

	While button_rechts = 0 And button_links = 1  'naar rechts draaien en eind pos is niet bereikt
		plaats_nieuw = plaats_nieuw + 1
		If plaats_nieuw > 65530 Then plaats_nieuw = 65530
		Gosub steprechts
	Wend

	While button_links = 0 And button_rechts = 1  'naar links draaien en begin pos is niet bereikt
		plaats_nieuw = plaats_nieuw - 1
		If plaats_nieuw = 65535 Then plaats_nieuw = 0
		Gosub steplinks
	Wend

	If button_save = 0 Then  'als buton_save wordt ingedrukt wordt pos. vastgelegd
		n = (2 * keuze) - 2
		Write n, plaats_nieuw.HB
		n = n + 1
		Write n, plaats_nieuw.LB
		temp = 60 + keuze
		Write temp, 1
		Lcdcmdout LcdLine1Home
		Lcdout "   S A V E D    "
		While button_save = 0
		Wend
		Lcdcmdout LcdLine1Home
		Lcdout "Setup           "
		keuze = keuze + 1
		If keuze > aantal_sporen Then keuze = 1
	Endif

	If button_next = 0 And button_links = 1 And button_rechts = 1 Then
		keuze = keuze + 1
		If keuze > aantal_sporen Then keuze = 1
		Lcdcmdout LcdLine2Pos(11)
		Lcdout "    "
		While button_next = 0
		Wend
	Endif

Wend

plaats_oud = plaats_nieuw
q = aantal_sporen * 2
t = 1
For n = 0 To q
	Read n, temppos.HB
	n = n + 1
	Read n, temppos.LB
	positie(t) = temppos.LB + (temppos.HB * 256)  'positie 1 = keuze 1 is geh.pl 0 en 1
	t = t + 1
Next n
keuze = 8
Gosub start2
Return                                            

steprechts:
cw_ccw = 0
cl_puls = 0
WaitUs pulsduur
cl_puls = 1
WaitUs pulsduur
Return                                            
steplinks:
cw_ccw = 1
cl_puls = 0
WaitUs pulsduur
cl_puls = 1
WaitUs pulsduur
Return                                            

lcdkeuze:
Lcdcmdout LcdLine2Pos(11)
Lcdout #keuze
Return                                            



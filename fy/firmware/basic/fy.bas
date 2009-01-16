Define CONF_WORD = 0x3f3a

'fiddleyard controller
'na het starten van de software wordt eerst het display op 0 gezet en gaat het ledje 4 keer aan uit, daarna
'gaat de FY  in de init mode, totdat de micro begin 1 is geworden, daarna kan de setup mode ingegaan worden

AllDigital
Symbol ena_ble = RA2
Symbol dat_in = PORTB  'bits 7 6 5 4 positie nibble
Symbol micro_begin = RC0  'begin positie melder
Symbol micro_eind = RC1  'eind positie melder
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
Dim cijfer_l As Byte
Dim cijfer_r As Byte
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

Dim temppuls As Word
Dim permanent As Bit
ADCON1 = 6

TRISA = %00001000
TRISB = %11110000
TRISC = %11110111

keuze = 0
Gosub segment7  'display toont 00
Gosub test  'rode ledje 4x aan-uit

aantal_sporen = 15
plaats_oud = 0
speed = 250
teller = 0
puls_norm = 1250
puls_verschil = 12800

Read 50, l297enable
If l297enable = 5 Then ena_ble = 1  'dan stepper niet permanent  bekrachtigd

Read 56, teller
If teller > 15 Then teller = 10
pulsduur1 = puls_norm + (teller * speed)

Write 52, 0

Read 51, lastpos
If lastpos > 0 And lastpos < 16 Then
	plaats_oud = 0
	Gosub initialiseer
	keuze = lastpos
Endif

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

If keuze = 0 Then
	Gosub initialiseer  'zet de FY in de 0 positie plaats_oud=0 en keuze =0
Else
	If button_setup = 1 Then Goto start2  'FY gaat naar laats opgeslagen positie
Endif

If button_setup = 0 Then
	Gosub initialiseer  'ga naar de 0 positie en set plaats_oud=0
	Gosub setup  'maakt het vastleggen van posities mogelijk
Endif


start:
While nw_pos = 0  'als dit bit laag  is wacht op een nieuw adres
	If button_setup = 0 Then Goto begin

	If button_setup = 1 And button_rechts = 0 Then
		keuze = keuze + 1
		If keuze > aantal_sporen Then keuze = 1
		Goto start2
	Endif

	If button_setup = 1 And button_links = 0 Then
		keuze = keuze - 1
		If keuze < 1 Then keuze = aantal_sporen
		Goto start2
	Endif

	If button_setup = 1 And button_save = 0 And button_next = 0 Then
		Read 56, teller
		teller = teller + 1
		If teller > 15 Then teller = 0
		Write 56, teller
		temp = keuze
		keuze = teller
		Gosub segment7
		WaitMs 10
		pulsduur1 = puls_norm + (teller * speed)
		While button_save = 0
		Wend
		keuze = temp
	Endif
	
	If button_setup = 1 And button_next = 0 Then
		Read 56, teller
		temp = keuze
		keuze = teller
		Gosub segment7
		keuze = temp
	Endif

	If button_setup = 1 And button_next = 1 Then
		Read 51, keuze
		Gosub segment7
	Endif
Wend

While nw_pos = 1  'het nieuwe adres mag gebruikt worden als dit bit naar 1 gaat
	If button_setup = 0 Then Goto begin
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
Gosub segment7
puls = 0
plaats_nieuw = positie(keuze)
led_pos_ok = 0

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
	If l297enable = 5 Then ena_ble = 1  'dan stepper niet  bekrachtigd
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
	If l297enable = 5 Then ena_ble = 1  'dan stepper niet  bekrachtigd
Endif

plaats_oud = plaats_nieuw
led_pos_ok = 1
Write 51, keuze
Goto start
End                                               

initialiseer:
Read 52, nul
keuze = 0
Gosub segment7
puls = 0
ena_ble = 0  'stepper wordt  bekrachtigd
pulsduur1 = puls_norm + (teller * speed)

While micro_begin = 1
	If puls < 113 Then pulsduur = pulsduur1 + (113 - puls) * (113 - puls)  'sneller naar pulsduur1
	If puls > 113 Then pulsduur = pulsduur1
	If plaats_oud < 113 And nul = 1 And plaats_oud > 0 Then pulsduur = pulsduur1 + (113 - plaats_oud) * (113 - plaats_oud)
	plaats_oud = plaats_oud - 1
	Gosub steplinks
	puls = puls + 1
Wend

If l297enable = 5 Then ena_ble = 1  'dan stepper niet  bekrachtigd

WaitMs 10
If nul = 0 Then
	plaats_oud = 0
	Write 52, 1
Endif

Write 51, 0
Return  'fiddle yard staat nu in de geijkte begin positie

setup:
ena_ble = 0

keuze = 1
plaats_nieuw = 0

While button_setup = 0
	pulsduur = puls_norm + (13 * speed) + puls_verschil

	Gosub segment7  'laat keuze zien

	While button_rechts = 0 And button_links = 0
		Read 50, l297enable
		If l297enable = 5 Then
			l297enable = 10  'dan is stepper niet  bekrachtigd
		Else
			l297enable = 5
		Endif
	Gosub test
	Wend

	While button_rechts = 0 And button_links = 1 And micro_eind = 1  'naar rechts draaien en eind pos is niet bereikt
		plaats_nieuw = plaats_nieuw + 1
		If plaats_nieuw > 65530 Then plaats_nieuw = 65530
		Gosub steprechts
	Wend

	While button_links = 0 And button_rechts = 1 And micro_begin = 1  'naar links draaien en begin pos is niet bereikt
		plaats_nieuw = plaats_nieuw - 1
		If plaats_nieuw = 65535 Then plaats_nieuw = 0
		Gosub steplinks
	Wend

	If button_save = 0 Then  'als buton_save wordt ingedrukt wordt pos. vastgelegd
		n = (2 * keuze) - 2
		Write n, plaats_nieuw.HB
		n = n + 1
		Write n, plaats_nieuw.LB
		While button_save = 0
		Wend
		keuze = keuze + 1
		If keuze > aantal_sporen Then keuze = 1
		Gosub test
		Write 50, l297enable
	Endif

	If button_next = 0 And button_links = 1 And button_rechts = 1 Then
		keuze = keuze + 1
		If keuze > aantal_sporen Then keuze = 1
		While button_next = 0
		Wend
		WaitMs 10

	Endif

Wend
If l297enable = 5 Then ena_ble = 1  'dan stepper niet  bekrachtigd
plaats_oud = plaats_nieuw
Gosub initialiseer  'ga weer naar de 0 positie

q = aantal_sporen * 2
t = 1
For n = 0 To q
	Read n, temppos.HB
	n = n + 1
	Read n, temppos.LB
	positie(t) = temppos.LB + (temppos.HB * 256)  'positie 1 = keuze 1 is geh.pl 0 en 1
	t = t + 1
Next n


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

segment7:

cijfer_l = keuze / 10
PORTB = cijfer_l  'decimaal
RA4 = 1
WaitMs 10
RA4 = 0

cijfer_r = keuze Mod 10  'éenheid
PORTB = cijfer_r
RA5 = 1
WaitMs 10
RA5 = 0

Return                                            
test:
n = 3
While n > 1
	WaitMs 150
	led_pos_ok = 1
	WaitMs 150
	led_pos_ok = 0
	n = n - 1
Wend
Return                                            





Define CONFIG1L = 0x00
Define CONFIG1H = 0x02
Define CONFIG2L = 0x1e
Define CONFIG2H = 0x00
Define CONFIG3L = 0x00
Define CONFIG3H = 0x82
Define CONFIG4L = 0x80
Define CONFIG4H = 0x00
Define CONFIG5L = 0x0f
Define CONFIG5H = 0xc0
Define CONFIG6L = 0x0f
Define CONFIG6H = 0xe0
Define CONFIG7L = 0x0f
Define CONFIG7H = 0x40
TRISA = %000000
TRISB = %00110000
TRISC = %00000000

AllDigital

ADCON0.ADON = 0
ADCON0.CHS3 = 0
ADCON0.CHS2 = 0
ADCON0.CHS1 = 0
ADCON0.CHS0 = 0
ADCON1.PCFG0 = 0
ADCON1.PCFG1 = 1
ADCON1.PCFG2 = 1
ADCON1.PCFG3 = 1
ADCON1.VCFG0 = 0
ADCON1.VCFG1 = 0


'******************************************************************************
'microcontroller  P18F2480

'******************************************************************************


Dim hr As Byte
Dim dummy As Byte
Dim teller As Byte
Dim value As Byte
Dim minute As Byte
Dim hour As Byte
Dim dispnr As Byte
Dim puntje As Byte

Dim intensity As Word
Dim space As Word
Dim dis1 As Byte
Dim dis2 As Byte
Dim dis3 As Byte
Dim dis4 As Byte
Dim puntteller As Byte



main:

PORTA = 0
PORTB = 0
PORTC = 0

teller = 3
dummy = 0
dispnr = 0
minute = 47
hour = 9
puntteller = 0
WaitMs 1000
Read 0, intensity.LB
Read 1, intensity.HB
If intensity > 1000 Then
  intensity = 500
  Write 0, intensity.LB
  Write 1, intensity.HB
Endif
space = 1000 - intensity
puntje = 3
While dummy = 0
  teller = teller + 1
  dispnr = dispnr + 1
  Select Case teller
  Case 100
    If RB4 = 0 Then  'more power to display
      If intensity < 990 Then  '1000 = max
        intensity = intensity + 10
      Endif
      Write 0, intensity.LB
      Write 1, intensity.HB
    Endif
    If RB5 = 0 Then  'less power to display
      If intensity > 59 Then  'not lower than 50
        intensity = intensity - 10
      Endif
      Write 0, intensity.LB
      Write 1, intensity.HB
    Endif
    teller = 0
    minute = minute + 1
    If minute = 60 Then
      minute = 0
      hour = hour + 1
      If hour > 23 Then
        hour = 0
      Endif
    Endif
    puntteller = puntteller + 1
    space = 1000 - intensity
    value = minute Mod 10
    dis1 = LookUp(%00111111, %00000110, %01011011, %01001111, %01100110, %01101101, %01111101, %00000111, %01111111, %01101111), value
    value = minute / 10
    value = value Mod 10
    dis2 = LookUp(%00111111, %00000110, %01011011, %01001111, %01100110, %01101101, %01111101, %00000111, %01111111, %01101111), value
    value = hour Mod 10
    dis3 = LookUp(%00111111, %00000110, %01011011, %01001111, %01100110, %01101101, %01111101, %00000111, %01111111, %01101111), value
    value = hour / 10
    value = value Mod 10
    dis4 = LookUp(%00000000, %00000110, %01011011, %01001111, %01100110, %01101101, %01111101, %00000111, %01111111, %01101111), value
  Case Else
  EndSelect
  PORTC = hr
  Select Case dispnr
  Case 1
    PORTC = dis1
    RA5 = 1
  Case 2
    PORTC = dis2
    RA4 = 1
  Case 3
    PORTC = dis3
    RA3 = 1
  Case 4
    PORTC = dis4
    RA2 = 1
  Case Else
    Select Case puntteller
    Case 4
      PORTC = %00001001
    Case 8
      PORTC = 0
      puntteller = 0
    Case Else
    EndSelect
    RA1 = 1
    dispnr = 0
  EndSelect
  WaitUs intensity  'this is the time that diaplay is activated.
  PORTA = 0
  WaitUs space  'this is the time that display is blind.

Wend

End                                               

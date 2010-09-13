MCU=atmega8
CC=avr-gcc
OBJCOPY=avr-objcopy
OBJDUMP=avr-objdump
REMOVE = rm -f

OUTPUTDIR=Out
APPINC = Src
APPSRC = Src
LOCONETSRC = Src/LocoNet
LOCONETINC = Src/LocoNet


CFLAGS=-g -mmcu=$(MCU) -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums \
		  -ffunction-sections -fdata-sections -Wall -Wstrict-prototypes -Os -D F_CPU=8000000\
		  -I$(APPINC) -I$(LOCONETINC)
ASFLAGS = -mmcu=$(MCU) -I. -x assembler-with-cpp -Wall
 
APPLICATION = Mgv50AT

#-------------------
all: clean $(APPLICATION).hex 
#-------------------
$(APPLICATION).hex : $(APPLICATION).out 
	$(OBJCOPY) -R .eeprom -O ihex $(OUTPUTDIR)/$(APPLICATION).out $(APPLICATION).hex 
	avr-size $(OUTPUTDIR)/$(APPLICATION).out
	@echo " "
	@echo "Expl.: data=initialized data, bss=uninitialized data, text=code"
	@echo " "

$(APPLICATION).out : Main.o ln_buf.o ln_sw_uart.o ln_interface.o Mgv50At.o 
	$(CC) $(CFLAGS) -mno-interrupts -o $(OUTPUTDIR)/$(APPLICATION).out -Wl,-Map,$(OUTPUTDIR)/$(APPLICATION).map \
	$(OUTPUTDIR)/Main.o $(OUTPUTDIR)/Mgv50At.o $(OUTPUTDIR)/ln_buf.o $(OUTPUTDIR)/ln_sw_uart.o $(OUTPUTDIR)/ln_interface.o

#-------------------
Main.o : $(APPSRC)/Main.c
	$(CC) $(CFLAGS) -Os -c -o $(OUTPUTDIR)/Main.o $(APPSRC)/Main.c
#-------------------
Mgv50At.o : $(APPSRC)/Mgv50At.c 
	$(CC) $(CFLAGS) -c -o $(OUTPUTDIR)/Mgv50At.o $(APPSRC)/Mgv50At.c 
#-------------------
ln_sw_uart.o : $(LOCONETSRC)/ln_sw_uart.c 
	$(CC) $(CFLAGS) -c -o $(OUTPUTDIR)/ln_sw_uart.o $(LOCONETSRC)/ln_sw_uart.c 
#-------------------
utils.o : $(LOCONETSRC)/utils.c 
	$(CC) $(CFLAGS) -c -o $(OUTPUTDIR)/utils.o $(LOCONETSRC)/utils.c 
#-------------------
ln_buf.o : $(LOCONETSRC)/ln_buf.c 
	$(CC) $(CFLAGS) -c -o $(OUTPUTDIR)/ln_buf.o $(LOCONETSRC)/ln_buf.c 
#-------------------
ln_interface.o : $(LOCONETSRC)/ln_interface.c 
	$(CC) $(CFLAGS) -c -o $(OUTPUTDIR)/ln_interface.o $(LOCONETSRC)/ln_interface.c 
	
#-------------------
# MySmartUsb LIGHT usb programmer 
mysmart : clean $(APPLICATION).hex
	avrdude -p $(MCU) -P com2 -c stk500v2 -e -U flash:w:$(APPLICATION).hex

#-------------------
clean:
	$(REMOVE) $(OUTPUTDIR)/*.* 
	$(REMOVE) *.hex
#-------------------

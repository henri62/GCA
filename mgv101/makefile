MCU=atmega168
CC=avr-gcc
OBJCOPY=avr-objcopy
OBJDUMP=avr-objdump
REMOVE = rm -f

OUTPUTDIR=Out
APPINC = Src
APPSRC = Src
LIBSRC = Src/Lib
LIBINC = Src/Lib
NETWSRC = Src/ENJ2860
NETWINC = Src/ENJ2860
LOCONETSRC = Src/LocoNet
LOCONETINC = Src/LocoNet

CFLAGS=-g -mmcu=$(MCU) -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums \
		  -ffunction-sections -fdata-sections -Wall -Wstrict-prototypes -Os -DF_CPU=8000000\
		  -I$(APPINC) -I$(LIBINC) -I$(NETWINC) -I$(LOCONETINC)
ASFLAGS = -mmcu=$(MCU) -I. -x assembler-with-cpp -Wall
 
#For usbasp
ISP=usbasp
PORT=/dev/usb/ttyUSB0

APPLICATION = EtLocBuf

#-------------------
.PHONY: all
all: clean $(APPLICATION).hex 
#-------------------
$(APPLICATION).hex : $(APPLICATION).out 
	$(OBJCOPY) -R .eeprom -O ihex $(OUTPUTDIR)/$(APPLICATION).out $(APPLICATION).hex 
	avr-size $(OUTPUTDIR)/$(APPLICATION).out
	@echo " "
	@echo "Expl.: data=initialized data, bss=uninitialized data, text=code"
	@echo " "

$(APPLICATION).out : Main.o EthLocBuffer.o enc28j60.o ip_arp_udp_tcp.o UserIo.o ln_buf.o ln_sw_uart.o ln_interface.o Serial_mega.o
	$(CC) $(CFLAGS) -mno-interrupts -o $(OUTPUTDIR)/$(APPLICATION).out -Wl,-Map,$(OUTPUTDIR)/$(APPLICATION).map \
	$(OUTPUTDIR)/Main.o $(OUTPUTDIR)/EthLocBuffer.o $(OUTPUTDIR)/enc28j60.o $(OUTPUTDIR)/ip_arp_udp_tcp.o \
	$(OUTPUTDIR)/UserIo.o $(OUTPUTDIR)/ln_buf.o $(OUTPUTDIR)/ln_sw_uart.o $(OUTPUTDIR)/ln_interface.o $(OUTPUTDIR)/Serial_mega.o \
 
#-------------------
Main.o : $(APPSRC)/Main.c
	$(CC) $(CFLAGS) -Os -c -o $(OUTPUTDIR)/Main.o $(APPSRC)/Main.c
#-------------------
EthLocBuffer.o : $(APPSRC)/EthLocBuffer.c 
	$(CC) $(CFLAGS) -c -o $(OUTPUTDIR)/EthLocBuffer.o $(APPSRC)/EthLocBuffer.c 
#-------------------
enc28j60.o : $(NETWSRC)/enc28j60.c 
	$(CC) $(CFLAGS) -c -o $(OUTPUTDIR)/enc28j60.o $(NETWSRC)/enc28j60.c 
#-------------------
ip_arp_udp_tcp.o : $(NETWSRC)/ip_arp_udp_tcp.c 
	$(CC) $(CFLAGS) -c -o $(OUTPUTDIR)/ip_arp_udp_tcp.o $(NETWSRC)/ip_arp_udp_tcp.c 
#-------------------
UserIo.o : $(APPSRC)/UserIo.c 
	$(CC) $(CFLAGS) -c -o $(OUTPUTDIR)/UserIo.o $(APPSRC)/UserIo.c 
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
Serial_mega.o : $(APPSRC)/Serial_mega.c 
	$(CC) $(CFLAGS) -c -o $(OUTPUTDIR)/Serial_mega.o $(APPSRC)/Serial_mega.c 

#-------------------
# Use the ISP USB programmer. 
usbasp: all
	avrdude  -p $(MCU) -i 70 -c ${ISP} -P ${PORT} -U flash:w:$(APPLICATION).hex

#-------------------
# avrdude lpt ISP programmer. 
program: $(APPLICATION).hex
	avrdude -p $(MCU) -c stk200 -P lpt1 -e -U flash:w:$(APPLICATION).hex

#-------------------
clean:
	$(REMOVE) $(OUTPUTDIR)/*.* 
	$(REMOVE) *.hex
#-------------------

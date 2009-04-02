# makefile, written by guido socher
MCU=atmega88
CC=avr-gcc
OBJCOPY=avr-objcopy
# optimize for size:
CFLAGS=-g -mmcu=$(MCU) -Wall -Wstrict-prototypes -Os -mcall-prologues
#-------------------
all: lnudp.hex
#-------------------
help: 
	@echo "Usage: make all|load|rdfuses"
	@echo "or"
	@echo "Usage: make clean"
#-------------------
lnudp.hex : lnudp.out 
	$(OBJCOPY) -R .eeprom -O ihex lnudp.out lnudp.hex 
	avr-size lnudp.out
	@echo " "
	@echo "Expl.: data=initialized data, bss=uninitialized data, text=code"
	@echo " "

lnudp.out : lnudp.o ip-arp-udp.o enc28j60.o timeout.o
	$(CC) $(CFLAGS) -o lnudp.out -Wl,-Map,lnudp.map lnudp.o ip-arp-udp.o enc28j60.o timeout.o
enc28j60.o : enc28j60.c avr-compat.h timeout.h enc28j60.h
	$(CC) $(CFLAGS) -Os -c enc28j60.c
ip-arp-udp.o : ip-arp-udp.c net.h avr-compat.h enc28j60.h
	$(CC) $(CFLAGS) -Os -c ip-arp-udp.c
main.o : main.c ip-arp-udp.h avr-compat.h enc28j60.h timeout.h net.h
	$(CC) $(CFLAGS) -Os -c main.c
timeout.o : timeout.c timeout.h 
	$(CC) $(CFLAGS) -Os -c timeout.c
#------------------
load: lnudp.hex
	./prg-load-uc-88 lnudp.hex
#-------------------
# Check this with make rdfuses
rdfuses:
	./prg-fusebit-uc-88 -r
#-------------------
clean:
	rm -f *.o *.map *.out *.hex
#-------------------

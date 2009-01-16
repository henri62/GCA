# makefile for FY C Firmware


GENDIR=./picgen
OUTDIR=./picbin

CPP=sdcc
LNK=sdcc
PROG=k14

CC_FLAGS=-mpic14 -p16f873 -V --debug -c -I.
LNK_FLAGS=-mpic14 -p16f873 -V --debug -Wl-c -Wl-m -Wl-r -I.
PROG_FLAGS=p
VERIFY_FLAGS=v
CONF_FLAGS=c
INFO_FLAGS=i

OBJS=$(patsubst %.c,$(GENDIR)/%.o,$(wildcard *.c))

TARGET=$(OUTDIR)/fy.hex


all: $(TARGET)

$(TARGET): $(OBJS) fy.h utils.h
	$(LNK) $(LNK_FLAGS) -o $(TARGET) $(OBJS)


$(GENDIR)/%.o: %.c
	$(CPP) $(CC_FLAGS) $< -o $@



clean:
	-mkdir -p $(OUTDIR)
	-mkdir -p $(GENDIR)
	-rm -f $(GENDIR)/*.o
	
program:
	$(PROG) $(PROG_FLAGS) $(TARGET)

verify:
	$(PROG) $(VERIFY_FLAGS) $(TARGET)

config:
	$(PROG) $(CONF_FLAGS)

info:
	$(PROG) $(INFO_FLAGS)


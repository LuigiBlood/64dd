ROOTDIR = $(N64_INST)
GCCN64PREFIX = $(ROOTDIR)/bin/mips64-elf-
CHKSUM64PATH = $(ROOTDIR)/bin/chksum64
MKDFSPATH = $(ROOTDIR)/bin/mkdfs
HEADERPATH = $(ROOTDIR)/lib
N64TOOL = $(ROOTDIR)/bin/n64tool
HEADERNAME = header

PROG_NAME = 64ddview

LINK_FLAGS = -G0 -L$(ROOTDIR)/lib -L$(ROOTDIR)/mips64-elf/lib -ldragon -lc -lm -ldragonsys -Tn64ld.x 
CFLAGS = -std=gnu99 -march=vr4300 -mtune=vr4300 -O2 -G0 -Wall -Werror -I$(ROOTDIR)/include -I$(ROOTDIR)/mips64-elf/include
ASFLAGS = -mtune=vr4300 -march=vr4300
CC = $(GCCN64PREFIX)gcc
AS = $(GCCN64PREFIX)as
LD = $(GCCN64PREFIX)ld
OBJCOPY = $(GCCN64PREFIX)objcopy
OBJS = $(PROG_NAME).o dd.o


$(PROG_NAME).v64: $(PROG_NAME).elf
	$(OBJCOPY) $(PROG_NAME).elf $(PROG_NAME).bin -O binary
	rm -f $(PROG_NAME).v64
	$(N64TOOL) -b -l 2M -t "64DD Test" -h $(HEADERPATH)/$(HEADERNAME) -o $(PROG_NAME).v64 $(PROG_NAME).bin
	$(CHKSUM64PATH) $(PROG_NAME).v64

$(PROG_NAME).elf : $(OBJS)
	$(LD) -o $(PROG_NAME).elf $(OBJS) $(LINK_FLAGS)

copy: $(PROG_NAME).v64
	cp $(PROG_NAME).v64 ~/public_html/

all: $(PROG_NAME).v64

clean:
	rm -f *.elf *.o *.bin

clean-all:
	rm -f *.v64 *.elf *.o *.bin

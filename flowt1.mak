PROJ	=FLOWT1
DEBUG	=1
CC	=qcl
CFLAGS_G	= /AM /W4 /Ze 
CFLAGS_D	= /Zi /Gi$(PROJ).mdt /Od 
CFLAGS_R	= /Od /DNDEBUG 
CFLAGS	=$(CFLAGS_G) $(CFLAGS_D)
LFLAGS_G	= /CP:0xfff /NOI /SE:0x80 /ST:0x61a8 
LFLAGS_D	= /CO /INCR 
LFLAGS_R	= 
LFLAGS	=$(LFLAGS_G) $(LFLAGS_D)
RUNFLAGS	=
OBJS_EXT = 	
LIBS_EXT = 	

.asm.obj: ; $(AS) $(AFLAGS) -c $*.asm

all:	$(PROJ).EXE

flowt1.obj:	flowt1.c $(H)

flow1.obj:	flow1.c $(H)

$(PROJ).EXE:	flowt1.obj flow1.obj $(OBJS_EXT)
	echo >NUL @<<$(PROJ).crf
flowt1.obj +
flow1.obj +
$(OBJS_EXT)
$(PROJ).EXE

$(LIBS_EXT);
<<
	ilink -a -e "qlink $(LFLAGS) @$(PROJ).crf" $(PROJ)

run: $(PROJ).EXE
	$(PROJ) $(RUNFLAGS)


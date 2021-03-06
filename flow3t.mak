PROJ	=FLOW3T
DEBUG	=1
CC	=qcl
CFLAGS_G	= /AL /W1 /Ze 
CFLAGS_D	= /Zi /Zr /Gi$(PROJ).mdt /Od 
CFLAGS_R	= /O /Ot /DNDEBUG 
CFLAGS	=$(CFLAGS_G) $(CFLAGS_D)
LFLAGS_G	= /CP:0xfff /NOI /NOE /SE:0x80 /ST:0x2710 
LFLAGS_D	= /CO /INCR 
LFLAGS_R	= 
LFLAGS	=$(LFLAGS_G) $(LFLAGS_D)
RUNFLAGS	=
OBJS_EXT = 	
LIBS_EXT = 	

.asm.obj: ; $(AS) $(AFLAGS) -c $*.asm

all:	$(PROJ).EXE

flow3t.obj:	flow3t.c $(H)

meshpt.obj:	meshpt.c $(H)

$(PROJ).EXE:	flow3t.obj meshpt.obj $(OBJS_EXT)
	echo >NUL @<<$(PROJ).crf
flow3t.obj +
meshpt.obj +
$(OBJS_EXT)
$(PROJ).EXE

$(LIBS_EXT);
<<
	ilink -a -e "qlink $(LFLAGS) @$(PROJ).crf" $(PROJ)

run: $(PROJ).EXE
	$(PROJ) $(RUNFLAGS)


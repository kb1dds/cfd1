PROJ	=FLOW2T
DEBUG	=1
CC	=qcl
CFLAGS_G	= /AL /W4 /Za 
CFLAGS_D	= /Zi /Zr /Gi$(PROJ).mdt /Od 
CFLAGS_R	= /O /Ot /DNDEBUG 
CFLAGS	=$(CFLAGS_G) $(CFLAGS_D)
LFLAGS_G	= /CP:0xfff /NOI /NOE /SE:0x80 /ST:0x5f90 
LFLAGS_D	= /CO /INCR 
LFLAGS_R	= 
LFLAGS	=$(LFLAGS_G) $(LFLAGS_D)
RUNFLAGS	=
OBJS_EXT = 	
LIBS_EXT = 	

.asm.obj: ; $(AS) $(AFLAGS) -c $*.asm

all:	$(PROJ).EXE

flow2t.obj:	flow2t.c $(H)

cfd1.obj:	cfd1.c $(H)

$(PROJ).EXE:	flow2t.obj cfd1.obj $(OBJS_EXT)
	echo >NUL @<<$(PROJ).crf
flow2t.obj +
cfd1.obj +
$(OBJS_EXT)
$(PROJ).EXE

$(LIBS_EXT);
<<
	ilink -a -e "qlink $(LFLAGS) @$(PROJ).crf" $(PROJ)

run: $(PROJ).EXE
	$(PROJ) $(RUNFLAGS)


# MAKEFILE (libb)

T= libb

ALL= $(T).o $(T).a


INCDIR= $(REPOROOT)/include
LIBDIR= $(REPOROOT)/lib
MANDIR= $(REPOROOT)/man

INFODIR= $(REPOROOT)/info
HELPDIR= $(REPOROOT)/share/help
LDRPATH= $(REPOROOT)/lib

CRTDIR= $(CGS_CRTDIR)
VALDIR= $(CGS_VALDIR)


CPP=	cpp
CC=	gcc
CXX=	gpp
LD=	gld
RANLIB=	granlib
AR=	gar
NM=	gnm
COV=	gcov

LORDER=	lorder
TSORT=	tsort
LINT=	lint
RM=	rm -f
TOUCH=	touch
LINT=	lint


DEFS +=


INCS += bfile.h


LIBS +=


INCDIRS=

LIBDIRS= -L$(LIBDIR)

# flag setting
CPPFLAGS= $(DEFS) $(INCDIRS) $(MAKECPPFLAGS)
CFLAGS= $(MAKECFLAGS)
CCFLAGS= $(MAKECCFLAGS)
ARFLAGS= $(MAKEARFLAGS)
LDFLAGS= $(MAKELDFLAGS)



OBJ00= bopen.o bopentmp.o bopencmd.o bopenrcmd.o bopenlock.o 
OBJ01= bopenmod.o bdup.o bminmod.o
OBJ02= bopensched.o bopenshell.o bopenrcmde.o
OBJ03= bwrite.o bwriteblock.o bwriteblanks.o 
OBJ04= bprintf.o bprintline.o bprintlines.o bputc.o 
OBJ05= bflush.o breserve.o
OBJ06= bcontrol.o btruncate.o brewind.o bisterm.o
OBJ07= bseek.o btell.o

OBJ08= bread.o breadln.o breadlns.o bgetc.o 
OBJ09= bwritefile.o
OBJ10= 
OBJ11=
OBJ12=
OBJ13=
OBJ14=
OBJ15=

OBJ16= bfliner.o
OBJ17=
OBJ18=
OBJ19=
OBJ20=
OBJ21=
OBJ22=
OBJ23=

OBJ24=
OBJ25=
OBJ26=
OBJ27=
OBJ28=
OBJ29=
OBJ30=
OBJ31=

OBJ32=
OBJ33=
OBJ34=
OBJ35=
OBJ36=
OBJ37=
OBJ38=
OBJ39=

OBJA= $(OBJ00) $(OBJ01) $(OBJ02) $(OBJ03) $(OBJ04) $(OBJ05) $(OBJ06) $(OBJ07)
OBJB= $(OBJ08) $(OBJ09) $(OBJ10) $(OBJ11) $(OBJ12) $(OBJ13) $(OBJ14) $(OBJ15)
OBJC= $(OBJ16) $(OBJ17) $(OBJ18) $(OBJ19) $(OBJ20) $(OBJ21) $(OBJ22) $(OBJ23)
OBJD= $(OBJ24) $(OBJ25) $(OBJ26) $(OBJ27) $(OBJ28) $(OBJ29) $(OBJ30) $(OBJ31)
OBJE= $(OBJ32) $(OBJ33) $(OBJ34) $(OBJ35) $(OBJ36) $(OBJ37) $(OBJ38) $(OBJ39)

OBJ= $(OBJA) $(OBJB) $(OBJC) $(OBJD) $(OBJE)

OBJS= obja.o objb.o


obja.o:			$(OBJA)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJA)

objb.o:			$(OBJB)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJB)

objc.o:			$(OBJC)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJC)

objd.o:			$(OBJD)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJD)

obje.o:			$(OBJE)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJE)



default:		$(T).a

all:			$(ALL)

.c.ln:
	$(LINT) -c $(LINTFLAGS) $(CPPFLAGS) $<

.c.ls:
	$(LINT) $(LINTFLAGS) $(CPPFLAGS) $<

.c.i:
	$(CPP) $(CPPFLAGS) $< > $(*).i

.c.o:
	$(CC) $(CPPFLAGS) -c $(CFLAGS) $<

.cc.o:
	$(CXX) $(CPPFLAGS) -c $(CCFLAGS) $<


$(T).o:			$(OBJ_UCENT)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ_UCENT)

$(T).a:			$(OBJ_UCENT)
	$(AR) $(ARFLAGS) -rc $@ $?

$(T).nm:		$(T).so
	$(NM) $(NMFLAGS) $(T).so > $(T).nm

$(T).order:		$(OBJ) $(T).a
	$(LORDER) $(T).a | $(TSORT) > $(T).order
	$(RM) $(T).a
	while read O ; do $(AR) $(ARFLAGS) -cr $(T).a $${O} ; done < $(T).order

again:
	rm -f $(TALL)

clean:
	makeclean $(ALL)

control:
	(uname -n ; date) > Control


bcontrol.o:		bcontrol.cc		$(INCS)
bisterm.o:		bisterm.cc		$(INCS)
bcopyblock.o:		bcopyblock.cc		$(INCS)
bflush.o:		bflush.cc		$(INCS)
breserve.o:		breserve.cc		$(INCS)
bgetc.o:		bgetc.cc		$(INCS)
bopen.o:		bopen.cc		$(INCS)
bopencmd.o:		bopencmd.cc		$(INCS)
bopenlock.o:		bopenlock.cc		$(INCS)
bopenrcmd.o:		bopenrcmd.cc		$(INCS)
bopenremote.o:		bopenremote.cc		$(INCS)
bopensched.o:		bopensched.cc		$(INCS)
bopentmp.o:		bopentmp.cc		$(INCS)
bprintf.o:		bprintf.cc		$(INCS)
bprintline.o:		bprintline.cc		$(INCS)
bputc.o:		bputc.cc		$(INCS)
bread.o:		bread.cc		$(INCS)
breadln.o:		breadln.cc		$(INCS)
breadlns.o:		breadlns.cc		$(INCS)
bseek.o:		bseek.cc		$(INCS)
btell.o:		btell.cc		$(INCS)
btruncate.o:		btruncate.cc		$(INCS)
bwasteline.o:		bwasteline.cc		$(INCS)
bwrite.o:		bwrite.cc		$(INCS)
bwriteblock.o:		bwriteblock.cc		$(INCS)
bwritefile.o:		bwritefile.cc		$(INCS)
bwriteblanks.o:		bwriteblanks.cc		$(INCS)
bprintcleanline.o:	bprintcleanline.cc	$(INCS)
bprintcleanlines.o:	bprintcleanlines.cc	$(INCS)
brewind.o:		brewind.cc		$(INCS)
bdup.o:			bdup.cc			$(INCS)
bsupport.o:		bsupport.cc		$(INCS)

bfliner.o:		bfliner.cc bfliner.h



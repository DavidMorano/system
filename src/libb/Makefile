# MAKEFILE (libb)

T= libb

ALL= $(T).o $(T).a


BINDIR= $(REPOROOT)/bin
INCDIR= $(REPOROOT)/include
LIBDIR= $(REPOROOT)/lib
MANDIR= $(REPOROOT)/man
INFODIR= $(REPOROOT)/info
HELPDIR= $(REPOROOT)/share/help


CRTDIR= $(CGS_CRTDIR)
VALDIR= $(CGS_VALDIR)
LIBDIR= $(CGS_LIBDIR)

CPP= cpp
CC= gcc
CXX= gpp
LD= gld
RANLIB= $(CGS_GRANLIB)
AR= $(CGS_GAR)
NM= $(CGS_GNM)
COV= $(CGS_GCOV)

LORDER= lorder
TSORT= tsort
LINT= lint
RM= rm -f
TOUCH= touch
LINT= lint


DEFS=

LDRPATH= $(EXTRA)/lib

LIBDIRS= -L$(LIBDIR)

LIBS=


# flag setting
CPPFLAGS= $(DEFS) $(INCDIRS) $(MAKECPPFLAGS)
CFLAGS= $(MAKECFLAGS)
CCFLAGS= $(MAKECCFLAGS)
ARFLAGS= $(MAKEARFLAGS)
LDFLAGS= $(MAKELDFLAGS)


INCS= usystem.h localmisc.h bfile.h


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

OBJ16=
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



default:		$(T).o

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



bcontrol.o:		bcontrol.c $(INCS)
bisterm.o:		bisterm.c $(INCS)
bcopyblock.o:		bcopyblock.c $(INCS)
bflush.o:		bflush.c $(INCS)
breserve.o:		breserve.c $(INCS)
bgetc.o:		bgetc.c $(INCS)
bopen.o:		bopen.c $(INCS)
bopencmd.o:		bopencmd.c $(INCS)
bopenlock.o:		bopenlock.c $(INCS)
bopenrcmd.o:		bopenrcmd.c $(INCS)
bopenremote.o:		bopenremote.c $(INCS)
bopensched.o:		bopensched.c $(INCS)
bopentmp.o:		bopentmp.c $(INCS)
bprintf.o:		bprintf.c $(INCS)
bprintline.o:		bprintline.c $(INCS)
bputc.o:		bputc.c $(INCS)
bread.o:		bread.c $(INCS)
breadln.o:		breadln.cc $(INCS)
breadlns.o:		breadlns.c $(INCS)
bseek.o:		bseek.c $(INCS)
btell.o:		btell.c $(INCS)
btruncate.o:		btruncate.c $(INCS)
bwasteline.o:		bwasteline.c $(INCS)
bwrite.o:		bwrite.c $(INCS)
bwriteblock.o:		bwriteblock.c $(INCS)
bwritefile.o:		bwritefile.c $(INCS)
bwriteblanks.o:		bwriteblanks.c $(INCS)
bprintcleanline.o:	bprintcleanline.c $(INCS)
bprintcleanlines.o:	bprintcleanlines.c $(INCS)
brewind.o:		brewind.c $(INCS)
bdup.o:			bdup.c $(INCS)
bsupport.o:		bsupport.c $(INCS)



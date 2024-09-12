# MAKEFILE (libb)

T= libb

ALL= $(T).o $(T).so


BINDIR		?= $(REPOROOT)/bin
INCDIR		?= $(REPOROOT)/include
LIBDIR		?= $(REPOROOT)/lib
MANDIR		?= $(REPOROOT)/man
INFODIR		?= $(REPOROOT)/info
HELPDIR		?= $(REPOROOT)/share/help
CRTDIR		?= $(CGS_CRTDIR)
VALDIR		?= $(CGS_VALDIR)
RUNDIR		?= $(CGS_RUNDIR)


CPP		?= cpp
CC		?= gcc
CXX		?= gxx
LD		?= gld
RANLIB		?= granlib
AR		?= gar
NM		?= gnm
COV		?= gcov
LORDER		?= lorder
TSORT		?= tsort
LINT		?= lint
RM		?= rm -f
TOUCH		?= touch
LINT		?= lint


DEFS +=

INCS += libb.h bfile.h

LIBS += -luc


INCDIRS=

LIBDIRS= -L$(LIBDIR)


RUNINFO= -rpath $(RUNDIR)

LIBINFO= $(LIBDIRS) $(LIBS)

# flag setting
CPPFLAGS	?= $(DEFS) $(INCDIRS) $(MAKECPPFLAGS)
CFLAGS		?= $(MAKECFLAGS)
CXXFLAGS	?= $(MAKECXXFLAGS)
ARFLAGS		?= $(MAKEARFLAGS)
LDFLAGS		?= $(MAKELDFLAGS)


OBJ00= bsupport.o bopen.o bcontrol.o
OBJ01= bminmod.o bflush.o breserve.o
OBJ02= bseek.o brewind.o btell.o btruncate.o
OBJ03= bisterm.o

OBJ04= bwrite.o 
OBJ05= bwriteblock.o bwriteblanks.o bwritefile.o
OBJ06=
OBJ07=

OBJ08= bprintf.o bputc.o
OBJ09= bprintcleanln.o bprintcleanlns.o 
OBJ10= bprintln.o bprintlns.o 
OBJ11=

OBJ12= bread.o breadln.o breadlns.o bgetc.o 
OBJ13= bwasteln.o
OBJ14= 
OBJ15=

OBJA= $(OBJ00) $(OBJ01) $(OBJ02) $(OBJ03) 
OBJB= $(OBJ04) $(OBJ05) $(OBJ06) $(OBJ07)
OBJC= $(OBJ08) $(OBJ09) $(OBJ10) $(OBJ11) 
OBJD= $(OBJ12) $(OBJ13) $(OBJ14) $(OBJ15)

OBJ= obja.o objb.o objc.o objd.o


.SUFFIXES:		.hh .ii


default:		$(T).o

all:			$(ALL)

.c.i:
	$(CPP) $(CPPFLAGS) $< > $(*).i

.cc.ii:
	$(CPP) $(CPPFLAGS) $< > $(*).ii

.c.s:
	$(CC) -S $(CPPFLAGS) $(CFLAGS) $<

.cc.s:
	$(CXX) -S $(CPPFLAGS) $(CXXFLAGS) $<

.c.o:
	$(COMPILE.c) $<

.cc.o:
	$(COMPILE.cc) $<


$(T).so:		$(OBJ) Makefile
	$(LD) -shared -o $@ $(LDFLAGS) $(OBJ) $(LIBINFO)

$(T).o:			$(OBJ)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJ)

$(T).a:			$(OBJ)
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


obja.o:			$(OBJA)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJA)

objb.o:			$(OBJB)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJB)

objc.o:			$(OBJC)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJC)

objd.o:			$(OBJD)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJD)


bcontrol.o:		bcontrol.cc		$(INCS)
bisterm.o:		bisterm.cc		$(INCS)
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
bminmod.o:		bminmod.cc		$(INCS)

bsupport.o:		bsupport.cc		$(INCS)



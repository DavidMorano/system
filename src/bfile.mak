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

MODS +=

LIBS += -luc


OBJ00= bsupport.o bopen.o bcontrol.o
OBJ01= bminmod.o bflush.o breserve.o
OBJ02= bseek.o brewind.o btell.o 
OBJ03= bisterm.o bobj.o

OBJ04= bwrite.o 
OBJ05= bwriteblock.o bwriteblanks.o bwritefile.o
OBJ06= btruncate.o
OBJ07=

OBJ08= bprintf.o bputc.o
OBJ09= bprintln.o bprintlns.o 
OBJ10= bprintcleanln.o bprintcleanlns.o 
OBJ11=

OBJ12= bread.o breadln.o breadlns.o bgetc.o 
OBJ13= bwasteln.o
OBJ14= 
OBJ15=

OBJA= obj00.o obj01.o obj02.o obj03.o
OBJB= obj04.o obj05.o obj06.o
OBJC= obj08.o obj09.o obj10.o
OBJD= obj12.o obj13.o

OBJ= obja.o objb.o objc.o objd.o


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


.SUFFIXES:		.hh .ii .iim .ccm


default:		$(T).o

all:			$(ALL)


.c.i:
	$(CPP) $(CPPFLAGS) $< > $(*).i

.cc.ii:
	$(CPP) $(CPPFLAGS) $< > $(*).ii

.ccm.iim:
	$(CPP) $(CPPFLAGS) $< > $(*).iim

.c.s:
	$(CC) -S $(CPPFLAGS) $(CFLAGS) $<

.cc.s:
	$(CXX) -S $(CPPFLAGS) $(CXXFLAGS) $<

.c.o:
	$(COMPILE.c) $<

.cc.o:
	$(COMPILE.cc) $<

.ccm.o:
	makemodule $(*)


$(T).so:		$(OBJ) Makefile
	$(LD) -shared -o $@ $(LDFLAGS) $(OBJ) $(LIBINFO)

$(T).o:			$(OBJ)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJ)

$(T).nm:		$(T).o
	$(NM) $(NMFLAGS) $(T).o > $(T).nm

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


obj00.o:		$(OBJ00)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ00)

obj01.o:		$(OBJ01)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ01)

obj02.o:		$(OBJ02)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ02)

obj03.o:		$(OBJ03)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ03)

obj04.o:		$(OBJ04)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ04)

obj05.o:		$(OBJ05)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ05)

obj06.o:		$(OBJ06)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ06)

obj07.o:		$(OBJ07)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ07)

obj08.o:		$(OBJ08)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ08)

obj09.o:		$(OBJ09)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ09)

obj10.o:		$(OBJ10)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ10)

obj11.o:		$(OBJ11)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ11)

obj12.o:		$(OBJ12)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ12)

obj13.o:		$(OBJ13)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ13)

obj14.o:		$(OBJ14)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ14)

obj15.o:		$(OBJ15)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ15)


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
bprintf.o:		bprintf.cc		$(INCS)
bprintln.o:		bprintln.cc		$(INCS)
bprintlns.o:		bprintlns.cc		$(INCS)
bprintcleanline.o:	bprintcleanline.cc	$(INCS)
bprintcleanlines.o:	bprintcleanlines.cc	$(INCS)
brewind.o:		brewind.cc		$(INCS)
bdup.o:			bdup.cc			$(INCS)
bminmod.o:		bminmod.cc		$(INCS)

bsupport.o:		bsupport.cc		$(INCS)
bobj.o:			bobj.cc			$(INCS)



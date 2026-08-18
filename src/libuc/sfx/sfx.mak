# MAKEFILE (sfx)

T= sfx

ALL= $(T).o


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
CXX		?= gpp
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

INCS += sfx.h

MODS +=

LIBS +=


OBJ00= sfbaselib.o sfbasename.o sfbracketval.o
OBJ01= sfcenter.o sfcookkey.o sfdequote.o sfdirname.o
OBJ02= sflast.o sfnamecomp.o sfprogroot.o 
OBJ03= sfrootname.o sfshrink.o sfskipwhite.o 
OBJ04= sfcasesub.o sfsubstance.o sfthing.o 
OBJ05= sfword.o sfprogname.o sfsign.o 
OBJ06= sfnext.o sfnextqtok.o xfcontent.o
OBJ07= sfxchr.o sfxbrk.o sfxsub.o sfext.o

OBJ08= sfweirdo.o cfcontent.o
OBJ09= sfkey.o sfkeyval.o
OBJ10=
OBJ11=

OBJA= obj00.o obj01.o
OBJB= obj02.o obj03.o
OBJC= obj04.o obj05.o
OBJD= obj06.o obj07.o obj08.o obj09.o

OBJG0= obja.o objb.o
OBJG1= objc.o objd.o

OBJ= $(OBJG0) $(OBJG1_SFX)


INCDIRS=
LIBDIRS= -L lib

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
	gxx -c -x c++ -o $@ $(CPPFLAGS) $(CXXFLAGS) $<


$(T).o:			$(OBJ)
	$(LD) -r $(LDFLAGS) -o $@ $^

$(T).a:			$(OBJ)
	$(AR) $(ARFLAGS) -rc $@ $?

$(T).nm:		$(T).o
	$(NM) $(NMFLAGS) $(T).o > $(T).nm

again:
	rm -f $(ALL)

clean:
	makeclean $(ALL)

control:
	(uname -n ; date) > Control


obj00.o:		$(OBJ00)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj01.o:		$(OBJ01)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj02.o:		$(OBJ02)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj03.o:		$(OBJ03)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj04.o:		$(OBJ04)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj05.o:		$(OBJ05)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj06.o:		$(OBJ06)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj07.o:		$(OBJ07)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj08.o:		$(OBJ08)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj09.o:		$(OBJ09)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj10.o:		$(OBJ10)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj11.o:		$(OBJ11)
	$(LD) -r $(LDFLAGS) -o $@ $^


obja.o:			$(OBJA)
	$(LD) -r $(LDFLAGS) -o $@ $^

objb.o:			$(OBJB)
	$(LD) -r $(LDFLAGS) -o $@ $^

objc.o:			$(OBJC)
	$(LD) -r $(LDFLAGS) -o $@ $^

objd.o:			$(OBJD)
	$(LD) -r $(LDFLAGS) -o $@ $^


objg0.o:		$(OBJG0)
	$(LD) -r $(LDFLAGS) -o $@ $^

objg1.o:		$(OBJG1)
	$(LD) -r $(LDFLAGS) -o $@ $^


sfxchr.o:		sfxchr.cc	sfxchr.h		$(INCS)
sfxbrk.o:		sfxbrk.cc	sfxbrk.h		$(INCS)
sfxsub.o:		sfxsub.cc	sfxsub.h		$(INCS)
sfext.o:		sfext.cc	sfext.h			$(INCS)
sfnext.o:		sfnext.cc	sfnext.h		$(INCS)
sfnextqtok.o:		sfnextqtok.cc	sfnext.h		$(INCS)
sfweirdo:		sfweirdo.cc	sfweirdo.h		$(INCS)
sfcasesub.o:		sfcasesub.cc	sfcasesub.h		$(INCS)

sfbasename.o:		sfbasename.cc	sfxname.h		$(INCS)
sfdirname.o:		sfdirname.cc	sfxname.h		$(INCS)
sfprogname.o:		sfprogname.cc	sfxname.h		$(INCS)
sfrootname.o:		sfrootname.cc	sfxname.h		$(INCS)

sfcontent.o:		sfcontent.cc	sfcontent.h		$(INCS)

sfkey.o:		sfkey.cc	sfkey.h			$(INCS)
sfkeyval.o:		sfkeyval.cc	sfkeyval.h		$(INCS)



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


OBJ0_SFX= sfbaselib.o sfbasename.o sfbracketval.o
OBJ1_SFX= sfcenter.o sfcookkey.o sfdequote.o sfdirname.o
OBJ2_SFX= sfkey.o sflast.o sfnamecomp.o sfprogroot.o 
OBJ3_SFX= sfrootname.o sfshrink.o sfskipwhite.o 
OBJ4_SFX= sfcasesub.o sfsubstance.o sfthing.o 
OBJ5_SFX= sfword.o sfprogname.o sfsign.o sfkeyval.o 
OBJ6_SFX= sfnext.o sfnextqtok.o 
OBJ7_SFX= sfxchr.o sfxbrk.o sfxsub.o sfext.o

OBJ8_SFX= sfweirdo.o

OBJA_SFX= obj0_sfx.o obj1_sfx.o
OBJB_SFX= obj2_sfx.o obj3_sfx.o
OBJC_SFX= obj4_sfx.o obj5_sfx.o
OBJD_SFX= obj6_sfx.o obj7_sfx.o obj8_sfx.o

OBJG0_SFX= obja_sfx.o objb_sfx.o
OBJG1_SFX= objc_sfx.o objd_sfx.o

OBJ_SFX= $(OBJG0_SFX) $(OBJG1_SFX)


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
	gxx -c -x c++ -o $@ -O $<


$(T).o:			$(OBJ_SFX)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ_SFX)

$(T).a:			$(OBJ_SFX)
	$(AR) $(ARFLAGS) -rc $@ $?

$(T).nm:		$(T).o
	$(NM) $(NMFLAGS) $(T).o > $(T).nm

again:
	rm -f $(ALL)

clean:
	makeclean $(ALL)

control:
	(uname -n ; date) > Control


obj0_sfx.o:		$(OBJ0_SFX)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj1_sfx.o:		$(OBJ1_SFX)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj2_sfx.o:		$(OBJ2_SFX)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj3_sfx.o:		$(OBJ3_SFX)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj4_sfx.o:		$(OBJ4_SFX)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj5_sfx.o:		$(OBJ5_SFX)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj6_sfx.o:		$(OBJ6_SFX)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj7_sfx.o:		$(OBJ7_SFX)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj8_sfx.o:		$(OBJ8_SFX)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj9_sfx.o:		$(OBJ9_SFX)
	$(LD) -r $(LDFLAGS) -o $@ $^


obja_sfx.o:		$(OBJA_SFX)
	$(LD) -r $(LDFLAGS) -o $@ $^

objb_sfx.o:		$(OBJB_SFX)
	$(LD) -r $(LDFLAGS) -o $@ $^

objc_sfx.o:		$(OBJC_SFX)
	$(LD) -r $(LDFLAGS) -o $@ $^

objd_sfx.o:		$(OBJD_SFX)
	$(LD) -r $(LDFLAGS) -o $@ $^


objg0_sfx.o:		$(OBJG0_SFX)
	$(LD) -r $(LDFLAGS) -o $@ $^

objg1_sfx.o:		$(OBJG1_SFX)
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



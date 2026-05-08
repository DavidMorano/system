# MAKEFILE (usupport)

T= usupport

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

INCS += usupport.h usupport_sncpyx.hh

MODS +=

LIBS +=


OBJ00= usupport_prime.o usupport_sncpyx.o
OBJ01= usupport_itimer.o usupport_hasx.o
OBJ02= usupport_cfdec.o usupport_ctdec.o
OBJ03= usupport_snwcpy.o usupport_strwcpy.o
OBJ04= usupport_toxc.o usupport_nleadstr.o
OBJ05= usupport_matstr.o usupport_rmx.o
OBJ06= usupport_ustrftime.o usupport_sixchr.o
OBJ07= usupport_sichar.o usupport_cfdect.o

OBJ08= usupport_getsign.o
OBJ09= usupport_strnxchr.o usupport_strnxbrk.o
OBJ10= usupport_strnchar.o usupport_strnwht.o
OBJ11=

OBJA= obj00.o obj01.o obj02.o obj03.o
OBJB= obj04.o obj05.o obj06.o obj07.o
OBJC= obj08.o obj09.o obj10.o

OBJ= obja.o objb.o objc.o


INCDIRS +=

LIBDIRS += -L$(LIBDIR)

RUNINFO= -rpath $(RUNDIR)
LIBINFO= $(LIBDIRS) $(LIBS)

# flag setting
CPPFLAGS	?= $(DEFS) $(INCDIRS) $(MAKECPPFLAGS)
CFLAGS		?= $(MAKECFLAGS)
CXXFLAGS	?= $(MAKECXXFLAGS)
ARFLAGS		?= $(MAKEARFLAGS)
LDFLAGS		?= $(MAKELDFLAGS)


.SUFFIXES:		.hhh .ii .iim .ccm


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


$(T).o:			$(OBJ)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ)

$(T).nm:		$(T).o
	$(NM) $(NMFLAGS) $(T).o > $(T).nm

again:
	$(RM) $(ALL)

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


obj.o:			$(OBJ)
	$(LD) -r $(LDFLAGS) -o $@ $^


uhupport_prime.o:	usupport_prime.cc	usupport_prime.h	$(INCS)
usupport_itimer.o:	usupport_itimer.cc	usupport_itimer.hh	$(INCS)
usupport_sncpyx.o:	usupport_sncpyx.cc	usupport_sncpyx.hh	$(INCS)
usupport_cfdec.o:	usupport_cfdec.cc	usupport_cfdec.hh	$(INCS)
usupport_snwcpy.o:	usupport_snwcpy.cc	usupport_snwcpy.hh	$(INCS)
usupport_hasx.o:	usupport_hasx.cc	usupport_hasx.hh	$(INCS)
usupport_ctdec.o:	usupport_ctdec.cc	usupport_ctdec.hh	$(INCS)
usupport_snwcpy.o:	usupport_snwcpy.cc	usupport_snwcpy.hh	$(INCS)
usupport_matstr.o:	usupport_matstr.cc	usupport_matstr.hh	$(INCS)
usupport_nleadstr.o:	usupport_nleadstr.cc	usupport_nleadstr.hh	$(INCS)
usupport_toxc.o:	usupport_toxc.cc	usupport_toxc.hh	$(INCS)
usupport_rmx.o:		usupport_rmx.cc		usupport_rmx.hh		$(INCS)
usupport_strwcpy.o:	usupport_strwcpy.cc	usupport_strwcpy.hh	$(INCS)
usupport_ustrftime.o:	usupport_ustrftime.cc	usupport_ustrftime.hh	$(INCS)
usupport_sixchr.o:	usupport_sixchr.cc	usupport_sixchr.hh	$(INCS)
usupport_sichar.o:	usupport_sichar.cc	usupport_sichar.hh	$(INCS)
usupport_cfdect.o:	usupport_cfdect.cc	usupport_cfdect.hh	$(INCS)
usupport_strnxchr.o:	usupport_strnxchr.cc	usupport_strnxchr.hh	$(INCS)
usupport_strnxbrk.o:	usupport_strnxbrk.cc	usupport_strnxbrk.hh	$(INCS)
usupport_strnchar.o:	usupport_strnchar.cc	usupport_strnchar.hh	$(INCS)
usupport_strnwht.o:	usupport_strnwht.cc	usupport_strnwht.hh	$(INCS)



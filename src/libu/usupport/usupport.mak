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


OBJ0= usupport_prime.o usupport_sncpyx.o
OBJ1= usupport_itimer.o usupport_hasx.o
OBJ2= usupport_cfdec.o usupport_ctdec.o
OBJ3= usupport_snwcpy.o usupport_strwcpy.o
OBJ4= usupport_toxc.o usupport_nleadstr.o
OBJ5= usupport_matstr.o usupport_rmx.o
OBJ6= usupport_ustrftime.o usupport_sixchr.o
OBJ7= usupport_sichar.o usupport_cfdect.o

OBJ8= usupport_getsign.o
OBJ9=

OBJA= obj0.o obj1.o obj2.o obj3.o
OBJB= obj4.o obj5.o obj6.o obj7.o
OBJC= obj8.o

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


obj0.o:			$(OBJ0)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj1.o:			$(OBJ1)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj2.o:			$(OBJ2)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj3.o:			$(OBJ3)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj4.o:			$(OBJ4)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj5.o:			$(OBJ5)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj6.o:			$(OBJ6)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj7.o:			$(OBJ7)
	$(LD) -r $(LDFLAGS) -o $@ $^


obj8.o:			$(OBJ8)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj9.o:			$(OBJ9)
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



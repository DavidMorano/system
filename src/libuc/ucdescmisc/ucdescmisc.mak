# MAKEFILE (ucdescmisc)

T= ucdescmisc

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

INCS += ucdescmisc.h

MODS +=

LIBS +=


OBJ0= ucdescmiscers.o uccloseonexec.o
OBJ1= ucftruncate.o ucfminmod.o
OBJ2= ucfstat.o ucdescmanip.o
OBJ3= ucnonblock.o ucndelay.o ucfsync.o

OBJ4= ucttyname.o ucsetappend.o
OBJ5= ucclose.o
OBJ6= ucpeek.o ucfpassfd.o
OBJ7= ucfattach.o

OBJA= obj0.o obj1.o obj2.o obj3.o
OBJB= obj4.o obj5.o obj6.o obj7.o

OBJ= obja.o objb.o


INCDIRS +=
LIBDIRS += -L lib

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


obja.o:			$(OBJA)
	$(LD) -r $(LDFLAGS) -o $@ $^

objb.o:			$(OBJB)
	$(LD) -r $(LDFLAGS) -o $@ $^


ucdescmanip.o:		ucdescmanip.cc		ucdescmanip.h		$(INCS)
ucdescmiscers.o:	ucdescmiscers.cc	ucdescmiscers.h		$(INCS)
ucclose.o:		ucclose.cc		ucclose.h		$(INCS)
uccloseonexec.o:	uccloseonexec.cc	uccloseonexec.h		$(INCS)
ucfattach.o:		ucfattach.cc		ucfattach.h		$(INCS)
ucftruncate.o:		ucftruncate.cc		ucftruncate.h		$(INCS)
ucfminmod.o:		ucfminmod.cc		ucfminmod.h		$(INCS)
ucfstat.o:		ucfstat.cc		ucfstat.h		$(INCS)
ucfsync.o:		ucfsync.cc		ucfsync.h		$(INCS)
ucndelay.o:		ucndelay.cc		ucndelay.h		$(INCS)
ucnonblock.o:		ucnonblock.cc		ucnonblock.h		$(INCS)
ucpeek.o:		ucpeek.cc		ucpeek.h		$(INCS)
ucttyname.o:		ucttyname.cc		ucttyname.h		$(INCS)
ucsetappend.o:		ucsetappend.cc		ucsetappend.h		$(INCS)
ucfpassfd.o:		ucfpassfd.cc		ucfpassfd.h		$(INCS)



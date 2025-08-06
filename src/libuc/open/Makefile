# MAKEFILES (open)

T= open

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

INCS += open.h

MODS +=

LIBS +=


OBJ0_OPEN= openaddrinfo.o opensockaddr.o openshm.o
OBJ1_OPEN= openport.o openportmsg.o
OBJ2_OPEN= opendefstds.o opensysfs.o opentmp.o
OBJ3_OPEN= openusd.o 
OBJ4_OPEN= openqotd.o opensys_banner.o
OBJ5_OPEN= splitfname.o opendial.o
OBJ6_OPEN= opensysdbs.o

OBJA= obj0_open.o obj1_open.o obj2_open.o 
OBJB= obj3_open.o obj4_open.o obj5_open.o obj6_open.o

OBJ_OPEN= obja.o objb.o


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


.SUFFIXES:		.hh .ii .ccm


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


$(T).o:			$(OBJ_OPEN)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ_OPEN)

$(T).nm:		$(T).o
	$(NM) $(NMFLAGS) $(T).o > $(T).nm

again:
	rm -f $(ALL)

clean:
	makeclean $(ALL)

control:
	(uname -n ; date) > Control


obj0_open.o:		$(OBJ0_OPEN)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ0_OPEN)

obj1_open.o:		$(OBJ1_OPEN)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ1_OPEN)

obj2_open.o:		$(OBJ2_OPEN)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ2_OPEN)

obj3_open.o:		$(OBJ3_OPEN)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ3_OPEN)

obj4_open.o:		$(OBJ4_OPEN)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ4_OPEN)

obj5_open.o:		$(OBJ5_OPEN)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ5_OPEN)

obj6_open.o:		$(OBJ6_OPEN)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ6_OPEN)


obja.o:			$(OBJA)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJA)

objb.o:			$(OBJB)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJB)


# objects
openport.o:		openport.cc openport.h		$(INCS)
openportmsg.o:		openportmsg.cc openportmsg.h	$(INCS)

# subroutines
openaddrinfo.o:		openaddrinfo.cc openaddrinfo.h	$(INCS)
opensockaddr.o:		opensockaddr.cc opensockaddr.h	$(INCS)
opendefstds.o:		opendefstds.cc			$(INCS)
openqotd.o:		openqotd.cc openqotd.h		$(INCS)
openshm.o:		openshm.cc openshm.h		$(INCS)
opensysfs.o:		opensysfs.cc opensysfs.h	$(INCS)
opentmp.o:		opentmp.cc			$(INCS)
openusd.o:		openusd.cc			$(INCS)
opensys_banner.o:	opensys_banner.cc		$(INCS)

opendial.o:		opendial.cc opendial.h		$(INCS)

# associated
splitfname.o:		splitfname.cc splitfname.h	$(INCS)
opensysdbs.o:		opensysdbs.c opensysdbs.h	$(INCS)



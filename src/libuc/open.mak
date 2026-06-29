# MAKEFILE (open)

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
OBJ2_OPEN= opendefstds.o opensysfs.o
OBJ3_OPEN= openusd.o 
OBJ4_OPEN= opensys_banner.o
OBJ5_OPEN= opendial.o opendials.o
OBJ6_OPEN= opensysdbs.o
OBJ7_OPEN=

OBJA= obj0_open.o obj1_open.o obj2_open.o 
OBJB= obj3_open.o obj4_open.o obj5_open.o 
OBJC= obj6_open.o

OBJ_OPEN= obja.o objb.o objc.o


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


$(T).o:			$(OBJ_OPEN)
	$(LD) -r $(LDFLAGS) -o $@ $^

$(T).nm:		$(T).o
	$(NM) $(NMFLAGS) $(T).o > $(T).nm

again:
	rm -f $(ALL)

clean:
	makeclean $(ALL)

control:
	(uname -n ; date) > Control


obj0_open.o:		$(OBJ0_OPEN)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj1_open.o:		$(OBJ1_OPEN)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj2_open.o:		$(OBJ2_OPEN)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj3_open.o:		$(OBJ3_OPEN)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj4_open.o:		$(OBJ4_OPEN)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj5_open.o:		$(OBJ5_OPEN)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj6_open.o:		$(OBJ6_OPEN)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj7_open.o:		$(OBJ7_OPEN)
	$(LD) -r $(LDFLAGS) -o $@ $^


obja.o:			$(OBJA)
	$(LD) -r $(LDFLAGS) -o $@ $^

objb.o:			$(OBJB)
	$(LD) -r $(LDFLAGS) -o $@ $^

objc.o:			$(OBJC)
	$(LD) -r $(LDFLAGS) -o $@ $^


# objects
openport.o:		openport.cc		openport.h	$(INCS)
openportmsg.o:		openportmsg.cc		openportmsg.h	$(INCS)

# subroutines
openaddrinfo.o:		openaddrinfo.cc		openaddrinfo.h	$(INCS)
opensockaddr.o:		opensockaddr.cc		opensockaddr.h	$(INCS)
opendefstds.o:		opendefstds.cc				$(INCS)
openshm.o:		openshm.cc		openshm.h	$(INCS)
opensysfs.o:		opensysfs.cc		opensysfs.hh	$(INCS)
openusd.o:		openusd.cc				$(INCS)
opensys_banner.o:	opensys_banner.cc			$(INCS)

opendial.o:		opendial.cc		opendial.h	$(INCS)
opendials.o:		opendials.cc		opendials.h	$(INCS)

# associated
opensysdbs.o:		opensysdbs.c		opensysdbs.h	$(INCS)



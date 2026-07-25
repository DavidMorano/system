# MAKEFILE (libxti)

T= libxti

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

INCS += libxti.h

MODS +=

LIBS +=


OBJ0= snxtierr.o
OBJ1= snxtilook.o
OBJ2= 
OBJ3= 

OBJA= obj0.o obj1.o
OBJB=

OBJ= $(OBJA)


INCDIRS += -I inc -I /usr/local/include
LIBDIRS += -L$(LIBDIR)

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

$(T).order:		$(OBJ) $(T).a
	$(LORDER) $(T).a | $(TSORT) > $(T).order
	$(RM) $(T).a
	while read O ; do $(AR) $(ARFLAGS) -cr $(T).a $${O} ; done < $(T).order

again:
	$(RM) $(ALL)

clean:
	makeclean $(ALL)

control:
	(uname -n ; date) > Control


obj0.o:			$(OBJ0)
	$(CXX) -r -o $@ $(LDFLAGS) $^

obj1.o:			$(OBJ1)
	$(CXX) -r -o $@ $(LDFLAGS) $^

obj2.o:			$(OBJ2)
	$(CXX) -r -o $@ $(LDFLAGS) $^

obj3.o:			$(OBJ3)
	$(CXX) -r -o $@ $(LDFLAGS) $^

obj4.o:			$(OBJ4)
	$(CXX) -r -o $@ $(LDFLAGS) $^

obj5.o:			$(OBJ5)
	$(CXX) -r -o $@ $(LDFLAGS) $^


obja.o:			$(OBJA)
	$(CXX) -r -o $@ $(LDFLAGS) $^

objb.o:			$(OBJB)
	$(CXX) -r -o $@ $(LDFLAGS) $^


snxtierr.o:		snxtierr.cc	snxti.h			$(INCS)
snxtilook.o:		snxtilook.cc	snxti.h			$(INCS)



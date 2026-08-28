# MAKEFILE (usysconf)

T= usysconf

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

INCS += usysconf.h sysconfcmds.h

MODS += usysconf.o usysconfcheck.o

LIBS +=


DEPS= usysconfcheck.o

OBJPART=

OBJPRIME= usysconf0.o

OBJ0= usysconf1.o usysconf2.o
OBJ1= usysconfcheck.o
OBJ2= sysconfcmds.o
OBJ3= unprocessors.o

OBJA= obj0.o obj1.o obj2.o obj3.o
OBJB=

OBJIMPL= obja.o


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


$(T).o:			objprime.o objimpl.o
	$(LD) -r -o $@ $(LDFLAGS) $^

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


obja.o:			$(OBJA)
	$(LD) -r $(LDFLAGS) -o $@ $^

objb.o:			$(OBJB)
	$(LD) -r $(LDFLAGS) -o $@ $^


objpart.o:		$(OBJPART)
	$(LD) -r $(LDFLAGS) -o $@ $^

objprime.o:		$(OBJPRIME)
	$(LD) -r $(LDFLAGS) -o $@ $^

objimpl.o:		$(OBJIMPL)
	$(LD) -r $(LDFLAGS) -o $@ $^


# module primary
usysconf0.o:		usysconf.ccm				$(INCS)
	gxx -c -x c++ -o $@ $(CPPFLAGS) $(CXXFLAGS) $<

# module implementation
usysconf1.o:		usysconf1.cc usysconf0.o $(DEPS)	$(INCS)
	$(COMPILE.cc) $<

# module implementation
usysconf2.o:		usysconf2.cc usysconf0.o $(DEPS)	$(INCS)
	$(COMPILE.cc) $<

usysconfcheck.o:	usysconfcheck0.o usysconfcheck1.o
	$(LD) -r $(LDFLAGS) -o $@ $^

# module primary
usysconfcheck0.o:	usysconfcheck.ccm usysconfcheck.hh	$(INCS)
	gxx -c -x c++ -o $@ $(CPPFLAGS) $(CXXFLAGS) $<

# module implementation
usysconfcheck1.o:	usysconfcheck1.cc usysconfcheck0.o	$(INCS)
	$(COMPILE.cc) $<

sysconfcmds.o:		sysconfcmds.cc	sysconfcmds.h		$(INCS)
unprocessors.o:		unprocessors.cc				$(INCS)



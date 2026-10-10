# MAKEFILE (isfd)

T= isfd

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


DEFS=

INCS= isfd.h

MODS=

LIBS=


OBJ0_ISFD= isfdsocket.o 
OBJ1_ISFD= isfdfsremote.o
OBJ2_ISFD= isfdterminal.o
OBJ3_ISFD=

OBJA_ISFD= obj0_isfd.o obj1_isfd.o
OBJB_ISFD= obj2_isfd.o

OBJ_ISFD= $(OBJA_ISFD) $(OBJB_ISFD)


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


$(T).o:			$(OBJ_ISFD)
	$(LD) -r $(LDFLAGS) -o $@ $^

$(T).nm:		$(T).o
	$(NM) $(NMFLAGS) $(T).o > $(T).nm

again:
	rm -f $(ALL)

clean:
	makeclean $(ALL)

control:
	(uname -n ; date) > Control


obj0_isfd.o:		$(OBJ0_ISFD)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj1_isfd.o:		$(OBJ1_ISFD)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj2_isfd.o:		$(OBJ2_ISFD)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj3_isfd.o:		$(OBJ3_ISFD)
	$(LD) -r $(LDFLAGS) -o $@ $^


isfdsocket.o:		isfdsocket.cc		$(INCS)
isfdterminal.o:		isfdterminal.cc		$(INCS)
isfdfsremote.o:		isfdfsremote.cc		$(INCS)



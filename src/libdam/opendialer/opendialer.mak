# MAKEFILES (opendialer)

T= opendialer

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

INCS += opendialer.h

MODS +=

LIBS +=

OBJ0= opendialer_finger.o opendialer_hello.o 
OBJ1= opendialer_prog.o opendialer_tcp.o
OBJ2= opendialer_tcpmux.o opendialer_tcpnls.o
OBJ3= opendialer_ticotsord.o opendialer_uss.o

OBJA= obj0.o obj1.o obj2.o obj3.o
OBJB=

OBJ= obja.o


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


.SUFFIXES:		.hh .ii .ccm


default:		$(T).o

all:			$(ALL)


.c.i:
	$(CPP) $(CPPFLAGS) $< > $(*).i

.cc.ii:
	$(CPP) $(CPPFLAGS) $< > $(*).ii

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

$(T).a:			$(OBJ)
	$(AR) $(ARFLAGS) -rc $@ $?

$(T).nm:		$(T).o
	$(NM) $(NMFLAGS) $(T).o > $(T).nm

again:
	$(RM) $(ALL)

clean:
	makeclean $(ALL)

control:
	(uname -n ; date) > Control


obj0.o:			$(OBJ0)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ0)

obj1.o:			$(OBJ1)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ1)

obj2.o:			$(OBJ2)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ2)

obj3.o:			$(OBJ3)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ3)

obj4.o:			$(OBJ4)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ4)

obj5.o:			$(OBJ5)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ5)


obja.o:			$(OBJA)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJA)

objb.o:			$(OBJB)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJB)


opendialer_main.o:		opendialer_main.cc		$(INCS)

opendialer_finger.o:		opendialer_finger.cc		$(INCS)
opendialer_hello.o:		opendialer_hello.cc		$(INCS)
opendialer_prog.o:		opendialer_prog.cc		$(INCS)
opendialer_tcp.o:		opendialer_tcp.cc		$(INCS)
opendialer_tcpmux.o:		opendialer_tcpmux.cc		$(INCS)
opendialer_tcpnls.o:		opendialer_tcpnls.cc		$(INCS)
opendialer_ticotsord.o:		opendialer_ticotsord.cc		$(INCS)
opendialer_uss.o:		opendialer_uss.cc		$(INCS)



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

LIBS +=


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


OBJ0= opendialer_finger.o opendialer_hello.o 
OBJ1= opendialer_prog.o opendialer_tcp.o
OBJ2= opendialer_tcpmux.o opendialer_tcpnls.o
OBJ3= opendialer_ticotsord.o opendialer_uss.o

OBJA= obj0.o obj1.o obj2.o obj3.o
OBJB=

OBJ= obja.o


.SUFFIXES:		.hh .ii


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


$(T).o:			$(OBJ)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ)

$(T).a:			$(OBJ)
	$(AR) $(ARFLAGS) -rc $@ $?

$(T).nm:		$(T).so
	$(NM) $(NMFLAGS) $(T).so > $(T).nm

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
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0)

obj1.o:			$(OBJ1)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1)

obj2.o:			$(OBJ2)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2)

obj3.o:			$(OBJ3)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ3)

obj4.o:			$(OBJ4)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ4)

obj5.o:			$(OBJ5)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ5)


obja.o:			$(OBJA)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJA)


opendialer_main.o:		opendialer_main.cc		$(INCS)

opendialer_finger.o:		opendialer_finger.cc		$(INCS)
opendialer_hello.o:		opendialer_hello.cc		$(INCS)
opendialer_prog.o:		opendialer_prog.cc		$(INCS)
opendialer_tcp.o:		opendialer_tcp.cc		$(INCS)
opendialer_tcpmux.o:		opendialer_tcpmux.cc		$(INCS)
opendialer_tcpnls.o:		opendialer_tcpnls.cc		$(INCS)
opendialer_ticotsord.o:		opendialer_ticotsord.cc		$(INCS)
opendialer_uss.o:		opendialer_uss.cc		$(INCS)



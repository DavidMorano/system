# MAKEFILE (ufiledesc)

T= ufiledesc

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

INCS += ufiledesc.h ufiledescbase.hh

MODS +=

LIBS +=


OBJ0= ufiledescbase.o 
OBJ1= uconnect.o uregular.o 
OBJ2= usocket.o usockaddr.o
OBJ3= ugetdents.o 
OBJ4= uatx.o ufcntl.o
OBJ5= ulockfile.o uselect.o uterminal.o
OBJ6= ureade.o ureadext.o
OBJ7=

OBJA_UFILEDESC= obj0.o obj1.o obj2.o
OBJB_UFILEDESC= obj3.o obj4.o obj5.o obj6.o

OBJ_UFILEDESC= obja.o objb.o


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
	makemodule $(*)


$(T).o:			$(OBJ_UFILEDESC)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ_UFILEDESC)

$(T).nm:		$(T).o
	$(NM) $(NMFLAGS) $(T).o > $(T).nm

again:
	rm -f $(ALL)

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


obja.o:			$(OBJA_UFILEDESC)
	$(LD) -r $(LDFLAGS) -o $@ $^

objb.o:			$(OBJB_UFILEDESC)
	$(LD) -r $(LDFLAGS) -o $@ $^


ufiledescbase.o:	ufiledescbase.cc	ufiledescbase.hh	$(INCS)

usocket.o:		usocket.cc		usocket.h		$(INCS)
usockaddr.o:		usockaddr.cc		usockaddr.h		$(INCS)
uconnect.o:		uconnect.cc					$(INCS)
uregular.o:		uregular.cc		uregular.h		$(INCS)
ugetdents.o:		ugetdents.cc		ugetdents.h		$(INCS)
ureade.o:		ureade.cc		ureade.h		$(INCS)
ureadext.o:		ureadext.cc		ureadext.h		$(INCS)
uatx.o:			uatx.cc			uatx.h			$(INCS)
ufcntl.o:		ufcntl.cc		ufcntl.h		$(INCS)
ulockfile.o:		ulockfile.cc		ulockfile.h		$(INCS)
uselect.o:		uselect.cc		uselect.h		$(INCS)
uterminal.o:		uterminal.cc		uterminal.h		$(INCS)



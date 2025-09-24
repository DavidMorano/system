# MAKEFILES (mailmsghdrx)

T= mailmsghdrx

ALL= $(T).o


INDIR		?= $(REPOROOT)/bin
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

INCS= mailmsghdrx.h

MPDS +=

LIBS=


#OBJ0_MAILMSGHDRX= mailmsghdrct.o mailmsghdrfold.o
OBJ0_MAILMSGHDRX= mailmsghdrfold.o
OBJ1_MAILMSGHDRX= mailmsghdrs.o mailmsghdrval.o
OBJ2_MAILMSGHDRX=
OBJ3_MAILMSGHDRX=

OBJA_MAILMSGHDRX= obj0_mailmsghdrx.o obj1_mailmsghdrx.o

OBJ_MAILMSGHDRX= $(OBJA_MAILMSGHDRX)


LDRPATH= $(EXTRA)/lib

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


$(T).o:			$(OBJ_MAILMSGHDRX)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ_MAILMSGHDRX)

$(T).nm:		$(T).o
	$(NM) $(NMFLAGS) $(T).o > $(T).nm

again:
	rm -f $(ALL)

clean:
	makeclean $(ALL)

control:
	(uname -n ; date) > Control


obj0_mailmsghdrx.o:	$(OBJ0_MAILMSGHDRX)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj1_mailmsghdrx.o:	$(OBJ1_MAILMSGHDRX)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj2_mailmsghdrx.o:	$(OBJ2_MAILMSGHDRX)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj3_mailmsghdrx.o:	$(OBJ3_MAILMSGHDRX)
	$(LD) -r $(LDFLAGS) -o $@ $^


mailmsghdrct.o:		mailmsghdrct.cc		mailmsghdrct.h		$(INCS)
mailmsghdrfold.o:	mailmsghdrfold.cc	mailmsghdrfold.h	$(INCS) 
mailmsghdrs.o:		mailmsghdrs.cc		mailmsghdrs.h		$(INCS)
mailmsghdrval.o:	mailmsghdrval.cc	mailmsghdrval.h		$(INCS)



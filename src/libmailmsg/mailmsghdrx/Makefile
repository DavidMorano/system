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

LIBS=


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


#OBJ0_MAILMSGHDRX= mailmsghdrct.o mailmsghdrfold.o
OBJ0_MAILMSGHDRX= mailmsghdrfold.o
OBJ1_MAILMSGHDRX= mailmsghdrs.o mailmsghdrval.o

OBJA_MAILMSGHDRX= obj0_mailmsghdrx.o obj1_mailmsghdrx.o

OBJ_MAILMSGHDRX= $(OBJA_MAILMSGHDRX)


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


$(T).o:			$(OBJ_MAILMSGHDRX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ_MAILMSGHDRX)

$(T).nm:		$(T).so
	$(NM) $(NMFLAGS) $(T).so > $(T).nm

$(T).order:		$(OBJ) $(T).a
	$(LORDER) $(T).a | $(TSORT) > $(T).order
	$(RM) $(T).a
	while read O ; do $(AR) $(ARFLAGS) -cr $(T).a $${O} ; done < $(T).order

again:
	rm -f $(ALL)

clean:
	makeclean $(ALL)

control:
	(uname -n ; date) > Control


obj0_mailmsghdrx.o:	$(OBJ0_MAILMSGHDRX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0_MAILMSGHDRX)

obj1_mailmsghdrx.o:	$(OBJ1_MAILMSGHDRX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1_MAILMSGHDRX)

obj2_mailmsghdrx.o:	$(OBJ2_MAILMSGHDRX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2_MAILMSGHDRX)


mailmsghdrct.o:		mailmsghdrct.cc		mailmsghdrct.h		$(INCS)
mailmsghdrfold.o:	mailmsghdrfold.cc	mailmsghdrfold.h	$(INCS) 
mailmsghdrs.o:		mailmsghdrs.cc		mailmsghdrs.h		$(INCS)
mailmsghdrval.o:	mailmsghdrval.cc	mailmsghdrval.h		$(INCS)



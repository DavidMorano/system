# MAKEFILE (ucsys)

T= ucsys

ALL= $(T).o $(T).a


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

INCS= ucsys.h

LIBS=


INCDIRS=

LIBDIRS= -L$(LIBDIR)


LIBINFO= $(LIBDIRS) $(LIBS)

# flag setting
CPPFLAGS	?= $(DEFS) $(INCDIRS) $(MAKECPPFLAGS)
CFLAGS		?= $(MAKECFLAGS)
CXXFLAGS	?= $(MAKECXXFLAGS)
ARFLAGS		?= $(MAKEARFLAGS)
LDFLAGS		?= $(MAKELDFLAGS)


OBJ0_UCSYS= ucsys_sup.o
OBJ1_UCSYS= userattr.o spwd.o
OBJ2_UCSYS= ucsyspw.o ucsysgr.o ucsyspj.o
OBJ3_UCSYS= ucsyspr.o ucsysnw.o ucsysho.o ucsyssv.o

OBJ_UCSYS= obj0_ucsys.o obj1_ucsys.o obj2_ucsys.o obj3_ucsys.o


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


$(T).o:			$(OBJ_UCSYS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ_UCSYS)

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


obj0_ucsys.o:	$(OBJ0_UCSYS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0_UCSYS)

obj1_ucsys.o:	$(OBJ1_UCSYS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1_UCSYS)

obj2_ucsys.o:	$(OBJ2_UCSYS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2_UCSYS)

obj3_ucsys.o:	$(OBJ3_UCSYS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ3_UCSYS)


ucsys_sup.o:	ucsys_sup.cc ucsys.h $(INCS)

spwd.o:		spwd.cc spwd.h $(INCS)
userattr.o:	userattr.cc userattr.h $(INCS)

ucsyspw.o:	ucsyspw.cc ucsyspw.h $(INCS)
ucsysgr.o:	ucsysgr.cc ucsysgr.h $(INCS)
ucsyspj.o:	ucsyspj.cc ucsyspj.h $(INCS)

ucsyspr.o:	ucsyspr.cc ucsyspr.h $(INCS)
ucsysnw.o:	ucsysnw.cc ucsysnw.h $(INCS)
ucsysho.o:	ucsysho.cc ucsysho.h $(INCS)
ucsyssv.o:	ucsyssv.cc ucsyssv.h $(INCS)



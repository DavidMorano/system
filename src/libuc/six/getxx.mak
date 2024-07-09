# MAKEFILES (getxx)

T= getxx

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

INCS= getxx.h

LIBS=


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


OBJ0_GETXX= getho.o getax.o
OBJ1_GETXX= getnw.o
OBJ2_GETXX= getpr.o
OBJ3_GETXX= getsv.o getus.o


OBJA_GETXX= obj0_getxx.o obj1_getxx.o
OBJB_GETXX= obj2_getxx.o obj3_getxx.o

OBJ_GETXX= $(OBJA_GETXX) $(OBJB_GETXX)


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


$(T).o:			$(OBJ_GETXX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ_GETXX)

$(T).a:			$(OBJ_GETXX)
	$(AR) $(ARFLAGS) -rc $@ $?

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

obj0_getxx.o:	$(OBJ0_GETXX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0_GETXX)

obj1_getxx.o:	$(OBJ1_GETXX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1_GETXX)

obj2_getxx.o:	$(OBJ2_GETXX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2_GETXX)

obj3_getxx.o:	$(OBJ3_GETXX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ3_GETXX)


getax.o:		getax.cc getax.h $(INCS)

getus.o:		getus.cc getus.h $(INCS)

getho.o:		getho.cc getho.h $(INCS)
getnw.o:		getnw.cc getnw.h $(INCS)
getpr.o:		getpr.cc getpr.h $(INCS)
getse.o:		getse.cc getsv.h $(INCS)



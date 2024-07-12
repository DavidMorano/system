# MAKEFILES (ema)

T= ema

ALL= $(T).o $(T).a


BINDIR= $(REPOROOT)/bin
INCDIR= $(REPOROOT)/include
LIBDIR= $(REPOROOT)/lib
MANDIR= $(REPOROOT)/man

INFODIR= $(REPOROOT)/info
HELPDIR= $(REPOROOT)/share/help

CRTDIR= $(CGS_CRTDIR)
VALDIR= $(CGS_VALDIR)
RUNDIR= $(USRLOCAL)/lib


CPP=	cpp
CC=	gcc
CXX=	gpp
LD=	gld
RANLIB=	granlib
AR=	gar
NM=	gnm
COV=	gcov

LORDER=	lorder
TSORT=	tsort
LINT=	lint
RM=	rm -f
TOUCH=	touch
LINT=	lint


DEFS=

INCS= ema.h

LIBS=


INCDIRS=

LIBDIRS= -L$(LIBDIR)


LIBINFO= $(LIBDIRS) $(LIBS)

# flag setting
CPPFLAGS= $(DEFS) $(INCDIRS) $(MAKECPPFLAGS)
CFLAGS= $(MAKECFLAGS)
CXXFLAGS= $(MAKECXXFLAGS)
ARFLAGS= $(MAKEARFLAGS)
LDFLAGS= $(MAKELDFLAGS)


OBJ_EMA= emaobj.o asstr.o parts.o entry.o


default:		$(T).o

all:			$(ALL)

.c.ln:
	$(LINT) -c $(LINTFLAGS) $(CPPFLAGS) $<

.c.ls:
	$(LINT) $(LINTFLAGS) $(CPPFLAGS) $<

.c.i:
	$(CPP) $(CPPFLAGS) $< > $(*).i

.c.o:
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $<

.cc.o:
	$(CXX)  $(CPPFLAGS) $(CXXFLAGS) -c $<


$(T).o:			$(OBJ_EMA)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ_EMA)

$(T).a:			$(OBJ_EMA)
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

obj0_ema.o:	$(OBJ0EMA)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0EMA)

obj1_ema.o:	$(OBJ1EMA)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1EMA)

obj2_ema.o:	$(OBJ2EMA)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2EMA)

obj3_ema.o:	$(OBJ3EMA)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ3EMA)


emaobj.o:		emaobj.cc ema.h parts.hh asstr.hh entry.hh
parts.o:		parts.cc parts.hh asstr.hh
asstr.o:		asstr.cc asstr.hh
entry.o:		entry.cc entry.hh ema.h



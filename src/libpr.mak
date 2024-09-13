# MAKEFILE (libpr)

T= libpr

ALL= $(T).o $(T).a


BINDIR= $(REPOROOT)/bin
INCDIR= $(REPOROOT)/include
LIBDIR= $(REPOROOT)/lib
MANDIR= $(REPOROOT)/man

INFODIR= $(REPOROOT)/info
HELPDIR= $(REPOROOT)/share/help
LDRPATH= $(REPOROOT)/lib

CRTDIR= $(CGS_CRTDIR)
VALDIR= $(CGS_VALDIR)


CPP= cpp
CC= gcc
CXX= gpp
LD= gld
RANLIB= granlib
AR= gar
NM= gnm
COV= gcov

LORDER= lorder
TSORT= tsort
LINT= lint
RM= rm -f
TOUCH= touch
LINT= lint


DEFS +=

INCS += libpr.h

LIBS=


INCDIRS=

LIBDIRS= -L$(LIBDIR)


LDRPATH=

# flag setting
CPPFLAGS= $(DEFS) $(INCDIRS) $(MAKECPPFLAGS)
CFLAGS= $(MAKECFLAGS)
CXXFLAGS= $(MAKECXXFLAGS)
ARFLAGS= $(MAKEARFLAGS)
LDFLAGS= $(MAKELDFLAGS)


OBJ0_LIBPR= prgetprogpath.o prgetclustername.o
OBJ1_LIBPR= prmktmpdir.o prmkfname.o


OBJA_LIBPR= obj0_libpr.o obj1_libpr.o

OBJ_LIBPR= $(OBJA_LIBPR)


default:		$(T).a

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


$(T).o:			$(OBJ_LIBPR)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ_LIBPR)

$(T).a:			$(OBJ_LIBPR)
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

obj0_libpr.o:	$(OBJ0_LIBPR)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0_LIBPR)

obj1_libpr.o:	$(OBJ1_LIBPR)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1_LIBPR)

obj2_libpr.o:	$(OBJ2_LIBPR)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2_LIBPR)

obj3_libpr.o:	$(OBJ3_LIBPR)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ3_LIBPR)


prgetprogpath.o:	prgetprogpath.cc prgetprogpath.h	$(INCS)
prgetclustername.o:	prgetclustername.cc prgetclustername.h	#(INCS)
prmktmpdir.o:		prmktmpdir.cc prmktmpdir.h		$(INCS)
prmkfname.o:		prmkfname.cc prmkfname.h		$(INCS)



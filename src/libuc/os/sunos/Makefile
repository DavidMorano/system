# MAKEFILE (sunos)

T= sunos

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

INCS +=

LIBS +=


INCDIRS=

LIBDIRS= -L$(LIBDIR)


LDRPATH=

LIBINFO= $(LIBDIRS) $(LIBS)

# flag setting
CPPFLAGS= $(DEFS) $(INCDIRS) $(MAKECPPFLAGS)
CFLAGS= $(MAKECFLAGS)
CXXFLAGS= $(MAKECXXFLAGS)
ARFLAGS= $(MAKEARFLAGS)
LDFLAGS= $(MAKELDFLAGS)


OBJ0_SUNOS= sunos_fsdir.o
OBJ1_SUNOS=
OBJ2_SUNOS=

#OBJ_SUNOS= obj0.o obj1.o obj2.o
OBJ_SUNOS= obj0.o


default:		$(T).o

all:			$(ALL)

.c.ln:
	$(LINT) -c $(LINTFLAGS) $(CPPFLAGS) $<

.c.ls:
	$(LINT) $(LINTFLAGS) $(CPPFLAGS) $<

.c.i:
	$(CPP) $(CPPFLAGS) $< > $(*).i

.c.o:
	$(CC) $(CPPFLAGS) -c $(CFLAGS) $<

.cc.o:
	$(CXX) $(CPPFLAGS) -c $(CXXFLAGS) $<


$(T).o:			$(OBJ_SUNOS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ_SUNOS)

$(T).a:			$(OBJ_SUNOS)
	$(AR) $(ARFLAGS) -rc $@ $?

$(T).nm:		$(T).so
	$(NM) $(NMFLAGS) $(T).so > $(T).nm

$(T).order:		$(OBJ) $(T).a
	$(LORDER) $(T).a | $(TSORT) > $(T).order
	$(RM) $(T).a
	while read O ; do $(AR) $(ARFLAGS) -cr $(T).a $${O} ; done < $(T).order

again:
	rm -f $(TALL)

clean:
	makeclean $(ALL)

control:
	(uname -n ; date) > Control


obj0.o:			$(OBJ0_SUNOS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0_SUNOS)

obj1.o:			$(OBJ1_SUNOS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1_SUNOS)

obj2.o:			$(OBJ2_SUNOS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2_SUNOS)


sunos_fsdir.o:		sunos_fsdir.cc sunos_fsdir.h



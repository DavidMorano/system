# MAKEFILE (libsio)

T= libsio

ALL= $(T).o $(T).a


BINDIR= $(REPOROOT)/bin
INCDIR= $(REPOROOT)/include
LIBDIR= $(REPOROOT)/lib
MANDIR= $(REPOROOT)/man
INFODIR= $(REPOROOT)/info
HELPDIR= $(REPOROOT)/share/help


CRTDIR= $(CGS_CRTDIR)
VALDIR= $(CGS_VALDIR)
LIBDIR= $(CGS_LIBDIR)

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


DEFS=

LDRPATH= $(EXTRA)/lib

INCDIRS= -I$(INCDIR)
LIBDIRS= -L$(LIBDIR)

LIBS=


# flag setting
CPPFLAGS= $(DEFS) $(INCDIRS) $(MAKECPPFLAGS)
CFLAGS= $(MAKECFLAGS)
CXXFLAGS= $(MAKECXXFLAGS)
ARFLAGS= $(MAKEARFLAGS)
LDFLAGS= $(MAKELDFLAGS)


INCS= stdsio.h


OBJ_LIBSIO= freadln.o


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


$(T).o:			$(OBJ_LIBSIO)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ_LIBSIO)

$(T).a:			$(OBJ_LIBSIO)
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

obj0_six.o:	$(OBJ0_LIBSIO)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0_LIBSIO)

obj1_six.o:	$(OBJ1_LIBSIO)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1_LIBSIO)

obj2_six.o:	$(OBJ2_LIBSIO)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2_LIBSIO)

obj3_six.o:	$(OBJ3_LIBSIO)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ3_LIBSIO)


freadln.o:		freadln.cc stdsio.h



# MAKEFILE (darwin)

T= darwin

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
CCFLAGS= $(MAKECCFLAGS)
ARFLAGS= $(MAKEARFLAGS)
LDFLAGS= $(MAKELDFLAGS)


OBJ0_DARWIN= darwin_fsdir.o
OBJ1_DARWIN= darwin_getexecname.o
OBJ2_DARWIN=

#OBJ_DARWIN= obj0.o obj1.o obj2.o
OBJ_DARWIN= obj0.o obj1.o


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
	$(CXX) $(CPPFLAGS) -c $(CCFLAGS) $<


$(T).o:			$(OBJ_DARWIN)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ_DARWIN)

$(T).a:			$(OBJ_DARWIN)
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


obj0.o:			$(OBJ0_DARWIN)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0_DARWIN)

obj1.o:			$(OBJ1_DARWIN)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1_DARWIN)

obj2.o:			$(OBJ2_DARWIN)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2_DARWIN)


darwin_fsdir.o:		darwin_fsdir.cc darwin_fsdir.h
darwin_getexecname.o:	darwin_getexecname.cc



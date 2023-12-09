# MAKEFILE (usys)

T= usys

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
RANLIB= $(CGS_GRANLIB)
AR= $(CGS_GAR)
NM= $(CGS_GNM)
COV= $(CGS_GCOV)

LORDER= lorder
TSORT= tsort
LINT= lint
RM= rm -f
TOUCH= touch
LINT= lint


DEFS=

LDRPATH= $(EXTRA)/lib

LIBDIRS= -L$(LIBDIR)

LIBS=


# flag setting
CPPFLAGS= $(DEFS) $(INCDIRS) $(MAKECPPFLAGS)
CFLAGS= $(MAKECFLAGS)
CCFLAGS= $(MAKECCFLAGS)
ARFLAGS= $(MAKEARFLAGS)
LDFLAGS= $(MAKELDFLAGS)


INCS= usystem.h localmisc.h


OBJ0_USYS= usys_xxx.o
OBJ1_USYS= usys_sunos.o usys_darwin.o usys_linux.o 
OBJ2_USYS= usys_mqueue.o

OBJ_USYS= obj0_ucent.o obj1_ucent.o obj2_ucent.o


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


$(T).o:			$(OBJ_USYS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ_USYS)

$(T).a:			$(OBJ_USYS)
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

obj0_ucent.o:	$(OBJ0_USYS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0_USYS)

obj1_ucent.o:	$(OBJ1_USYS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1_USYS)

obj2_ucent.o:	$(OBJ2_USYS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2_USYS)


usys_xxx.o:		usys_xxx.cc usys_xxx.h
usys_sunos.o:		usys_sunos.cc usys_sunos.h
usys_darwin.o:		usys_darwin.cc usys_darwin.h
usys_linux.o:		usys_linux.cc usys_linux.h

usys_mqueue.o:		usys_mqueue.cc usys_mqueue.h



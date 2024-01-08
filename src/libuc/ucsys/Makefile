# MAKEFILE (ucsys)

T= ucsys

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


OBJ0_UCSYS= ucsys_sup.o userattr.o spwd.o
OBJ1_UCSYS= ucsyspw.o ucsysgr.o ucsyspj.o
OBJ2_UCSYS= ucsyspr.o ucsysnw.o ucsysho.o ucsyssv.o

OBJ_UCSYS= obj0_ucget.o obj1_ucget.o obj2_ucget.o



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


$(T).o:			$(OBJ_UCSYS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ_UCSYS)

$(T).a:			$(OBJ_UCSYS)
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

obj0_ucget.o:	$(OBJ0_UCSYS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0_UCSYS)

obj1_ucget.o:	$(OBJ1_UCSYS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1_UCSYS)

obj2_ucget.o:	$(OBJ2_UCSYS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2_UCSYS)


ucsys_sup.o:	ucsys_sup.cc ucsys.h

spwd.o:		spwd.cc spwd.h
userattr.o:	userattr.cc userattr.h

ucsyspw.o:	ucsyspw.cc ucsyspw.h
ucsysgr.o:	ucsysgr.cc ucsysgr.h
ucsyspj.o:	ucsyspj.cc ucsyspj.h

ucsyspr.o:	ucsyspr.cc ucsyspr.h
ucsysnw.o:	ucsysnw.cc ucsysnw.h



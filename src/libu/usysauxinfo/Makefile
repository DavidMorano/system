# MAKEFILE

T= usysauxinfo

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
RUNDIR= $(USRLOCAL)/lib


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

INCS= usysauxinfo.h usysauxinforeqs.h

LIBS=


INCDIRS=

LIBDIRS= -L$(LIBDIR)


LIBINFO= $(LIBDIRS) $(LIBS)

# flag setting
CPPFLAGS= $(DEFS) $(INCDIRS) $(MAKECPPFLAGS)
CFLAGS= $(MAKECFLAGS)
CCFLAGS= $(MAKECCFLAGS)
ARFLAGS= $(MAKEARFLAGS)
LDFLAGS= $(MAKELDFLAGS)


OBJ0_UCSYSAUXINFO=
OBJ1_UCSYSAUXINFO= usysauxinfo_sunos.o
OBJ2_UCSYSAUXINFO= usysauxinfo_darwin.o
OBJ3_UCSYSAUXINFO= usysauxinfo_linux.o


OBJA_UCSYSAUXINFO= obj1_usysauxinfo.o
OBJB_UCSYSAUXINFO= obj2_usysauxinfo.o obj3_usysauxinfo.o

OBJ_UCSYSAUXINFO= $(OBJA_UCSYSAUXINFO) $(OBJB_UCSYSAUXINFO)


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
	$(CXX)  $(CPPFLAGS) $(CCFLAGS) -c $<


$(T).o:			$(OBJ_UCSYSAUXINFO)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ_UCSYSAUXINFO)

$(T).a:			$(OBJ_UCSYSAUXINFO)
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


obj0_usysauxinfo.o:	$(OBJ0_UCSYSAUXINFO)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0_UCSYSAUXINFO)

obj1_usysauxinfo.o:	$(OBJ1_UCSYSAUXINFO)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1_UCSYSAUXINFO)

obj2_usysauxinfo.o:	$(OBJ2_UCSYSAUXINFO)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2_UCSYSAUXINFO)

obj3_usysauxinfo.o:	$(OBJ3_UCSYSAUXINFO)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ3_UCSYSAUXINFO)


usysauxinfo_sunos.o:	usysauxinfo_sunos.cc usysauxinfo_sunos.h $(INCS)
usysauxinfo_darwin.o:	usysauxinfo_darwin.cc usysauxinfo_darwin.h $(INCS)
usysauxinfo_linux.o:	usysauxinfo_linux.cc usysauxinfo_linux.h $(INCS)



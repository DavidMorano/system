# MAKEFILE

T= ucsysauxinfo

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


INCS= usystem.h localmisc.h sysauxinfonames.h


OBJ0_UCSYSAUXINFO= ucsub.o
OBJ1_UCSYSAUXINFO= usysauxinfo_sunos.o
OBJ2_UCSYSAUXINFO= usysauxinfo_darwin.o
OBJ3_UCSYSAUXINFO= usysauxinfo_linux.o


OBJA_UCSYSAUXINFO= obj0_ucsysauxinfo.o obj1_ucsysauxinfo.o
OBJB_UCSYSAUXINFO= obj2_ucsysauxinfo.o obj3_ucsysauxinfo.o

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

obj0_ucsysauxinfo.o:	$(OBJ0_UCSYSAUXINFO)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0_UCSYSAUXINFO)

obj1_ucsysauxinfo.o:	$(OBJ1_UCSYSAUXINFO)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1_UCSYSAUXINFO)

obj2_ucsysauxinfo.o:	$(OBJ2_UCSYSAUXINFO)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2_UCSYSAUXINFO)

obj3_ucsysauxinfo.o:	$(OBJ3_UCSYSAUXINFO)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ3_UCSYSAUXINFO)


ucsub.o:		ucsub.cc sysauxinfo.h $(INCS)

usysauxinfo_sunos.o:	usysauxinfo_sunos.cc usysauxinfo_sunos.h $(INCS)
usysauxinfo_darwin.o:	usysauxinfo_darwin.cc usysauxinfo_darwin.h $(INCS)
usysauxinfo_linux.o:	usysauxinfo_linux.cc usysauxinfo_linux.h $(INCS)



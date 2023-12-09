# MAKEFILE (ucent)

T= ucent

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


OBJ0_UCENT= ucentpw.o ucentsp.o ucentua.o 
OBJ1_UCENT= ucentgr.o ucentpj.o
OBJ2_UCENT= ucentpr.o ucentnw.o ucentho.o ucentsv.o

OBJ_UCENT= obj0_ucent.o obj1_ucent.o obj2_ucent.o


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


$(T).o:			$(OBJ_UCENT)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ_UCENT)

$(T).a:			$(OBJ_UCENT)
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

obj0_ucent.o:	$(OBJ0_UCENT)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0_UCENT)

obj1_ucent.o:	$(OBJ1_UCENT)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1_UCENT)

obj2_ucent.o:	$(OBJ2_UCENT)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2_UCENT)


ucentpw.o:		ucentpw.cc ucentpw.h $(INCS)
ucentsp.o:		ucentsp.cc ucentsp.h $(INCS)
ucentua.o:		ucentua.cc ucentua.h $(INCS)
ucentgr.o:		ucentgr.cc ucentgr.h $(INCS)
ucentpj.o:		ucentpj.cc ucentpj.h $(INCS)

ucentus.o:		ucentus.cc ucentus.h $(INCS)

ucentpr.o:		ucentpr.cc ucentpr.h $(INCS)
ucentnw.o:		ucentnw.cc ucentnw.h $(INCS)
ucentho.o:		ucentho.cc ucentho.h $(INCS)
ucentsv.o:		ucentsv.cc ucentsv.h $(INCS)



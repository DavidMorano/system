# MAKEFILE (ucget)

T= ucget

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


OBJ0_UCGET= ucgetpw.o ucgetsp.o ucgetgr.o ucgetpj.o
OBJ1_UCGET= ucgetus.o
OBJ2_UCGET= ucuserattr.o ucgetua.o
OBJ3_UCGET= ucgetpr.o ucgetnw.o ucgetho.o ucgetsv.o

OBJ_UCGET= obj0_ucget.o obj1_ucget.o obj2_ucget.o obj3_ucget.o


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


$(T).o:			$(OBJ_UCGET)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ_UCGET)

$(T).a:			$(OBJ_UCGET)
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

obj0_ucget.o:	$(OBJ0_UCGET)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0_UCGET)

obj1_ucget.o:	$(OBJ1_UCGET)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1_UCGET)

obj2_ucget.o:	$(OBJ2_UCGET)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2_UCGET)

obj3_ucget.o:	$(OBJ3_UCGET)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ3_UCGET)


ucgetpw.o:		ucgetpw.cc ucgetpw.h $(INCS)
ucgetsp.o:		ucgetsp.cc ucgetsp.h $(INCS)
ucgetgr.o:		ucgetgr.cc ucgetgr.h $(INCS)
ucgetpj.o:		ucgetpj.cc ucgetpj.h $(INCS)

ucuserattr.o:		ucuserattr.cc ucuserattr.h $(INCS)
ucgetus.o:		ucgetus.cc ucgetus.h $(INCS)

ucgetpr.o:		ucgetpr.cc ucgetpr.h $(INCS)
ucgetnw.o:		ucgetnw.cc ucgetnw.h $(INCS)
ucgetho.o:		ucgetho.cc ucgetho.h $(INCS)
ucgetsv.o:		ucgetsv.cc ucgetsv.h $(INCS)



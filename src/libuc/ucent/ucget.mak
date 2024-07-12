# MAKEFILE (ucget)

T= ucget

ALL= $(T).o $(T).a


BINDIR		?= $(REPOROOT)/bin
INCDIR		?= $(REPOROOT)/include
LIBDIR		?= $(REPOROOT)/lib
MANDIR		?= $(REPOROOT)/man
INFODIR		?= $(REPOROOT)/info
HELPDIR		?= $(REPOROOT)/share/help
CRTDIR		?= $(CGS_CRTDIR)
VALDIR		?= $(CGS_VALDIR)
RUNDIR		?= $(CGS_RUNDIR)


CPP		?= cpp
CC		?= gcc
CXX		?= gxx
LD		?= gld
RANLIB		?= granlib
AR		?= gar
NM		?= gnm
COV		?= gcov
LORDER		?= lorder
TSORT		?= tsort
LINT		?= lint
RM		?= rm -f
TOUCH		?= touch
LINT		?= lint


DEFS=

INCS= ucget.h

LIBS=


INCDIRS=

LIBDIRS= -L$(LIBDIR)


RUNINFO= -rpath $(RUNINFO)

LIBINFO= $(LIBDIRS) $(LIBS)

# flag setting
CPPFLAGS	?= $(DEFS) $(INCDIRS) $(MAKECPPFLAGS)
CFLAGS		?= $(MAKECFLAGS)
CXXFLAGS	?= $(MAKECXXFLAGS)
ARFLAGS		?= $(MAKEARFLAGS)
LDFLAGS		?= $(MAKELDFLAGS)


OBJ0_UCGET= ucgetpw.o ucgetsp.o ucgetgr.o ucgetpj.o
OBJ1_UCGET= ucgetus.o
OBJ2_UCGET= ucuserattr.o ucgetua.o
OBJ3_UCGET= ucgetpr.o ucgetnw.o ucgetho.o ucgetsv.o

OBJ_UCGET= obj0_ucget.o obj1_ucget.o obj2_ucget.o obj3_ucget.o


.SUFFIXES:		.hh .ii


default:		$(T).o

all:			$(ALL)

.c.i:
	$(CPP) $(CPPFLAGS) $< > $(*).i

.cc.ii:
	$(CPP) $(CPPFLAGS) $< > $(*).ii

.c.s:
	$(CC) -S $(CPPFLAGS) $(CFLAGS) $<

.cc.s:
	$(CXX) -S $(CPPFLAGS) $(CXXFLAGS) $<

.c.o:
	$(COMPILE.c) $<

.cc.o:
	$(COMPILE.cc) $<


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



# MAKEFILES (timestr)

T= timestr

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


CPP=	cpp
CC=	gcc
CXX=	gpp
LD=	gld
RANLIB=	granlib
AR=	gar
NM=	gnm
COV=	gcov

LORDER=	lorder
TSORT=	tsort
LINT=	lint
RM=	rm -f
TOUCH=	touch
LINT=	lint


DEFS=

INCS= timestr.h

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


OBJ0_TIMESTR= timestr_date.o timestr_elapsed.o
OBJ1_TIMESTR= timestr_nist.o timestr_scandate.o

OBJA_TIMESTR= obj0_timestr.o obj1_timestr.o

OBJ_TIMESTR= $(OBJA_TIMESTR)


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


$(T).o:			$(OBJ_TIMESTR)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ_TIMESTR)

$(T).a:			$(OBJ_TIMESTR)
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

obj0_timestr.o:	$(OBJ0_TIMESTR)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0_TIMESTR)

obj1_timestr.o:	$(OBJ1_TIMESTR)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1_TIMESTR)

obj2_timestr.o:	$(OBJ2_TIMESTR)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2_TIMESTR)

obj3_timestr.o:	$(OBJ3_TIMESTR)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ3_TIMESTR)


timestr_date.o:		timestr_date.cc		$(INCS)
timestr_elapsed.o:	timestr_elapsed.cc	$(INCS)
timestr_nist.o:		timestr_nist.cc		$(INCS)
timestr_scandate.o:	timestr_scandate.cc	$(INCS)



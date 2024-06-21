# MAKEFILES (dater)

T= dater

ALL= $(T).o $(T).a


BINDIR= $(REPOROOT)/bin
INCDIR= $(REPOROOT)/include
LIBDIR= $(REPOROOT)/lib
MANDIR= $(REPOROOT)/man

INFODIR= $(REPOROOT)/info
HELPDIR= $(REPOROOT)/share/help

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


DEFS +=

INCS += dater.h

LIBS +=


INCDIRS=

LIBDIRS= -L$(LIBDIR)


LIBINFO= $(LIBDIRS) $(LIBS)

# flag setting
CPPFLAGS= $(DEFS) $(INCDIRS) $(MAKECPPFLAGS)
CFLAGS= $(MAKECFLAGS)
CCFLAGS= $(MAKECCFLAGS)
ARFLAGS= $(MAKEARFLAGS)
LDFLAGS= $(MAKELDFLAGS)


OBJ0_DATER= dater_main.o
OBJ1_DATER= dater_getdate.o
OBJ2_DATER= dater_getbbtime.o
OBJ3_DATER= dater_setkey.o


OBJA_DATER= obj0_dater.o obj1_dater.o
OBJB_DATER= obj2_dater.o obj3_dater.o

OBJ_DATER= $(OBJA_DATER)


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


$(T).o:			$(OBJ_DATER)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ_DATER)

$(T).a:			$(OBJ_DATER)
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

obj0_dater.o:	$(OBJ0_DATER)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0_DATER)

obj1_dater.o:	$(OBJ1_DATER)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1_DATER)

obj2_dater.o:	$(OBJ2_DATER)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2_DATER)

obj3_dater.o:	$(OBJ3_DATER)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ3_DATER)


dater_main.o:		dater_main.cc		$(INCS)
dater_getdate.o:	dater_getdate.cc	$(INCS)
dater_getbbtime.o:	dater_getbbtime.cc	$(INCS)
dater_setkey.o:		dater_setkey.cc		$(INCS)



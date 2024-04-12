# MAKEFILES (getxx)

T= getxx

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


INCS= getxx.h


LIBS=


LIBDIRS= -L$(LIBDIR)

# flag setting
CPPFLAGS= $(DEFS) $(INCDIRS) $(MAKECPPFLAGS)
CFLAGS= $(MAKECFLAGS)
CCFLAGS= $(MAKECCFLAGS)
ARFLAGS= $(MAKEARFLAGS)
LDFLAGS= $(MAKELDFLAGS)


OBJ0_GETXX= getho.o
OBJ1_GETXX= getnw.o
OBJ2_GETXX= getpr.o
OBJ3_GETXX= getsv.o getus.o


OBJA_GETXX= obj0_getxx.o obj1_getxx.o
OBJB_GETXX= obj2_getxx.o obj3_getxx.o

OBJ_GETXX= $(OBJA_GETXX) $(OBJB_GETXX)


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


$(T).o:			$(OBJ_GETXX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ_GETXX)

$(T).a:			$(OBJ_GETXX)
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

obj0_getxx.o:	$(OBJ0_GETXX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0_GETXX)

obj1_getxx.o:	$(OBJ1_GETXX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1_GETXX)

obj2_getxx.o:	$(OBJ2_GETXX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2_GETXX)

obj3_getxx.o:	$(OBJ3_GETXX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ3_GETXX)


getho.o:		getho.cc getho.h $(INCS)
getnw.o:		getnw.cc getnw.h $(INCS)
getpr.o:		getpr.cc getpr.h $(INCS)
getse.o:		getse.cc getsv.h $(INCS)
getus.o:		getus.cc getus.h $(INCS)



# MAKEFILES (tmpx)

T= tmpx

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

LDRPATH= $(EXTRA)/lib

LIBDIRS= -L$(LIBDIR)

LIBS=


# flag setting
CPPFLAGS= $(DEFS) $(INCDIRS) $(MAKECPPFLAGS)
CFLAGS= $(MAKECFLAGS)
CCFLAGS= $(MAKECCFLAGS)
ARFLAGS= $(MAKEARFLAGS)
LDFLAGS= $(MAKELDFLAGS)


INCS= tmpx.h


OBJ0_TMPX= tmpx_main.o
OBJ1_TMPX= tmpx_getboottime.o
OBJ2_TMPX= tmpx_getrunlevel.o
OBJ3_TMPX= tmpx_getuserlines.o
OBJ4_TMPX= tmpx_getuserterms.o


OBJA_TMPX= obj0_tmpx.o obj1_tmpx.o
OBJB_TMPX= obj2_tmpx.o obj3_tmpx.o obj4_tmpx.o

OBJ_TMPX= $(OBJA_TMPX) $(OBJB_TMPX)


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


$(T).o:			$(OBJ_TMPX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ_TMPX)

$(T).a:			$(OBJ_TMPX)
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

obj0_tmpx.o:	$(OBJ0_TMPX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0_TMPX)

obj1_tmpx.o:	$(OBJ1_TMPX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1_TMPX)

obj2_tmpx.o:	$(OBJ2_TMPX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2_TMPX)

obj3_tmpx.o:	$(OBJ3_TMPX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ3_TMPX)

obj4_tmpx.o:	$(OBJ4_TMPX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ4_TMPX)


tmpx_main.o:		tmpx_main.cc		$(INCS)
tmpx_getboottime.o:	tmpx_getboottime.cc	$(INCS)
tmpx_getrunlevel.o:	tmpx_getrunlevel.cc	$(INCS)
tmpx_getuserlines.o:	tmpx_getuserlines.cc	$(INCS)
tmpx_getuserterms.o:	tmpx_getuserterms.cc	$(INCS)



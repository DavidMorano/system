# MAKEFILES (hdrx)

T= hdrx

ALL= $(T).o


INDIR		?= $(REPOROOT)/bin
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

INCS= hdrx.h

LIBS=


LDRPATH= $(EXTRA)/lib

LIBDIRS= -L$(LIBDIR)


RUNINFO= -rpath $(RUNDIR)

LIBINFO= $(LIBDIRS) $(LIBS)

# flag setting
CPPFLAGS	?= $(DEFS) $(INCDIRS) $(MAKECPPFLAGS)
CFLAGS		?= $(MAKECFLAGS)
CXXFLAGS	?= $(MAKECXXFLAGS)
ARFLAGS		?= $(MAKEARFLAGS)
LDFLAGS		?= $(MAKELDFLAGS)


OBJ0_HDRX= hdrctype.o hdrdecode.o
OBJ1_HDRX= hdrextid.o hdrextnum.o

OBJA_HDRX= obj0_hdrx.o obj1_hdrx.o

OBJ_HDRX= $(OBJA_HDRX)


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


$(T).o:			$(OBJ_HDRX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ_HDRX)

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


obj0_hdrx.o:	$(OBJ0_HDRX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0_HDRX)

obj1_hdrx.o:	$(OBJ1_HDRX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1_HDRX)

obj2_hdrx.o:	$(OBJ2_HDRX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2_HDRX)


hdrctype.o:		hdrctype.cc	hdrctype.h	$(INCS)
hdrdecode.o:		hdrdecode.cc	hdrdecode.h	$(INCS)
hdrextid.o:		hdrextid.cc	hdrextid.h	$(INCS)
hdrextnum.o:		hdrextnum.cc	hdrextnum.h	$(INCS)



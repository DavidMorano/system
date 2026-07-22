# MAKEFILE (sbuf)

T= sbuf

ALL= $(T).o


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


DEFS +=

INCS += sbuf.h

MODS +=

LIBS +=


OBJ0_SBUF= sbuf_prime.o sbuf_addquoted.o
OBJ1_SBUF= sbuf_termconseq.o sbuf_hexp.o
OBJ2_SBUF= rebuf.o
OBJ3_SBUF=

OBJA_SBUF= obj0_sbuf.o obj1_sbuf.o obj2_sbuf.o

OBJ_SBUF= $(OBJA_SBUF)


INCDIRS=
LIBDIRS= -L lib

RUNINFO= -rpath $(RUNDIR)
LIBINFO= $(LIBDIRS) $(LIBS)

# flag setting
CPPFLAGS	?= $(DEFS) $(INCDIRS) $(MAKECPPFLAGS)
CFLAGS		?= $(MAKECFLAGS)
CXXFLAGS	?= $(MAKECXXFLAGS)
ARFLAGS		?= $(MAKEARFLAGS)
LDFLAGS		?= $(MAKELDFLAGS)


.SUFFIXES:		.hh .ii .iim .ccm


default:		$(T).o

all:			$(ALL)


.c.i:
	$(CPP) $(CPPFLAGS) $< > $(*).i

.cc.ii:
	$(CPP) $(CPPFLAGS) $< > $(*).ii

.ccm.iim:
	$(CPP) $(CPPFLAGS) $< > $(*).iim

.c.s:
	$(CC) -S $(CPPFLAGS) $(CFLAGS) $<

.cc.s:
	$(CXX) -S $(CPPFLAGS) $(CXXFLAGS) $<

.c.o:
	$(COMPILE.c) $<

.cc.o:
	$(COMPILE.cc) $<

.ccm.o:
	gxx -c -x c++ -o $@ $(CPPFLAGS) $(CXXFLAGS) $<


$(T).o:			$(OBJ_SBUF)
	$(LD) -r $(LDFLAGS) -o $@ $^

$(T).nm:		$(T).o
	$(NM) $(NMFLAGS) $(T).o > $(T).nm

again:
	rm -f $(ALL)

clean:
	makeclean $(ALL)

control:
	(uname -n ; date) > Control


obj0_sbuf.o:	$(OBJ0_SBUF)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj1_sbuf.o:	$(OBJ1_SBUF)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj2_sbuf.o:	$(OBJ2_SBUF)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj3_sbuf.o:	$(OBJ3_SBUF)
	$(LD) -r $(LDFLAGS) -o $@ $^


sbuf_prime.o:		sbuf_prime.cc		$(INCS)
sbuf_addquoted.o:	sbuf_addquoted.cc	$(INCS)
sbuf_termconseq.o:	sbuf_termconseq.cc	$(INCS)
sbuf_hexp.o:		sbuf_hexp.cc		$(INCS)

rebuf.o:		rebuf.cc	rebuf.hh		$(INCS)



# MAKEFILE (varsub)

T= varsub

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

INCS += varsub.h

MODS +=

LIBS +=


DEPS= varsub_util.o

OBJ0_VARSUB= varsub_prime.o
OBJ1_VARSUB= varsub_loadfile.o
OBJ2_VARSUB= varsub_addvec.o
OBJ3_VARSUB= varsub_util.o

OBJA_VARSUB= obj0_varsub.o obj1_varsub.o
OBJB_VARSUB= obj2_varsub.o obj3_varsub.o

OBJ_VARSUB= obja_varsub.o objb_varsub.o


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
	gxx -c -x c++ -o $@ -O $<


$(T).o:			$(OBJ_VARSUB)
	$(LD) -r $(LDFLAGS) -o $@ $^

$(T).nm:		$(T).o
	$(NM) $(NMFLAGS) $(T).o > $(T).nm

again:
	rm -f $(ALL)

clean:
	makeclean $(ALL)

control:
	(uname -n ; date) > Control


obj0_varsub.o:		$(OBJ0_VARSUB)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj1_varsub.o:		$(OBJ1_VARSUB)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj2_varsub.o:		$(OBJ2_VARSUB)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj3_varsub.o:		$(OBJ3_VARSUB)
	$(LD) -r $(LDFLAGS) -o $@ $^


obja_varsub.o:		$(OBJA_VARSUB)
	$(LD) -r $(LDFLAGS) -o $@ $^

objb_varsub.o:		$(OBJB_VARSUB)
	$(LD) -r $(LDFLAGS) -o $@ $^


varsub_prime.o:		varsub_prime.cc				$(DEPS) $(INCS)
varsub_addvec.o:	varsub_addvec.cc			$(DEPS) $(INCS)
varsub_loadfile.o:	varsub_loadfile.cc			$(DEPS) $(INCS)
varsub_extras.o:	varsub_extras.cc			$(DEPS) $(INCS)

varsub_util.o:		varsub_util.ccm		varsub_util.hh		$(INCS)



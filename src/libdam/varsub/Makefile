# MAKEFILES (varsub)

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


OBJ0_VARSUB= varsub_main.o
OBJ1_VARSUB= varsub_loadfile.o
OBJ2_VARSUB= varsub_addvec.o
OBJ3_VARSUB=

OBJA_VARSUB= obj0_varsub.o obj1_varsub.o
OBJB_VARSUB= obj2_varsub.o

OBJ_VARSUB= obja_varsub.o objb_varsub.o


INCDIRS=

LIBDIRS= -L$(LIBDIR)


RUNINFO= -rpath $(RUNDIR)
LIBINFO= $(LIBDIRS) $(LIBS)

# flag setting
CPPFLAGS	?= $(DEFS) $(INCDIRS) $(MAKECPPFLAGS)
CFLAGS		?= $(MAKECFLAGS)
CXXFLAGS	?= $(MAKECXXFLAGS)
ARFLAGS		?= $(MAKEARFLAGS)
LDFLAGS		?= $(MAKELDFLAGS)


.SUFFIXES:		.hh .ii .ccm


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


$(T).o:			$(OBJ_VARSUB)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ_VARSUB)

$(T).nm:		$(T).o
	$(NM) $(NMFLAGS) $(T).o > $(T).nm

again:
	rm -f $(ALL)

clean:
	makeclean $(ALL)

control:
	(uname -n ; date) > Control


obj0_varsub.o:		$(OBJ0_VARSUB)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ0_VARSUB)

obj1_varsub.o:		$(OBJ1_VARSUB)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ1_VARSUB)

obj2_varsub.o:		$(OBJ2_VARSUB)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ2_VARSUB)

obj3_varsub.o:		$(OBJ3_VARSUB)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ3_VARSUB)


obja_varsub.o:		$(OBJA_VARSUB)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJA_VARSUB)

objb_varsub.o:		$(OBJB_VARSUB)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJB_VARSUB)


varsub_main.o:		varsub_main.cc		$(INCS)
varsub_addvec.o:	varsub_addvec.cc	$(INCS)
varsub_loadfile.o:	varsub_loadfile.cc	$(INCS)
varsub_extras.o:	varsub_extras.cc	$(INCS)



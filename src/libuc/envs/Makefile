# MAKEFILES (envs)

T= envs

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

INCS += envs.h

MODS +=

LIBS +=


OBJ0_ENVS= envs_main.o
OBJ1_ENVS= envs_procxe.o
OBJ2_ENVS= envs_subs.o
OBJ3_ENVS= 

OBJA_ENVS= obj0_envs.o obj1_envs.o
OBJB_ENVS= obj2_envs.o

OBJ_ENVS= $(OBJA_ENVS) $(OBJB_ENVS)


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
	makemodule $(*)


$(T).o:			$(OBJ_ENVS)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ_ENVS)

$(T).nm:		$(T).o
	$(NM) $(NMFLAGS) $(T).o > $(T).nm

again:
	rm -f $(ALL)

clean:
	makeclean $(ALL)

control:
	(uname -n ; date) > Control


obj0_envs.o:	$(OBJ0_ENVS)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj1_envs.o:	$(OBJ1_ENVS)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj2_envs.o:	$(OBJ2_ENVS)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj3_envs.o:	$(OBJ3_ENVS)
	$(LD) -r $(LDFLAGS) -o $@ $^


envs_main.o:		envs_main.cc	$(INCS)
envs_procxe.o:		envs_procxe.cc	#(INCS)
envs_subs.o:		envs_subs.cc	#(INCS)



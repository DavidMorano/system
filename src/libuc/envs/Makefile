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


OBJ0_ENVS= envs_main.o
OBJ1_ENVS= envs_procxe.o
OBJ2_ENVS= envs_subs.o
OBJ3_ENVS= 


OBJA_ENVS= obj0_envs.o obj1_envs.o
OBJB_ENVS= obj2_envs.o

OBJ_ENVS= $(OBJA_ENVS) $(OBJB_ENVS)


default:		$(T).o

all:			$(ALL)


.SUFFIXES:		.hh .ii .ccm


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

.ccm.o:
	makemodule $(*)


$(T).o:			$(OBJ_ENVS)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ_ENVS)

$(T).a:			$(OBJ_ENVS)
	$(AR) $(ARFLAGS) -rc $@ $?

$(T).nm:		$(T).o
	$(NM) $(NMFLAGS) $(T).o > $(T).nm

again:
	rm -f $(ALL)

clean:
	makeclean $(ALL)

control:
	(uname -n ; date) > Control


obj0_envs.o:	$(OBJ0_ENVS)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ0_ENVS)

obj1_envs.o:	$(OBJ1_ENVS)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ1_ENVS)

obj2_envs.o:	$(OBJ2_ENVS)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ2_ENVS)


envs_main.o:		envs_main.cc	$(INCS)
envs_procxe.o:		envs_procxe.cc	#(INCS)
envs_subs.o:		envs_subs.cc	#(INCS)



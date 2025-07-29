# MAKEFILES (snwcpy)

T= snwcpy

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
CXX		?= gpp
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

INCS += snwcpy.h snwcpyx.h snwcpyxc.h

MODS +=

LIBS +=


OBJ0_SNWCPYX= snwcpyxc.o snwcpyopaque.o
OBJ1_SNWCPYX= snwcpyclean.o snwcpylatin.o
OBJ2_SNWCPYX= snwcpyshrink.o snwcpycompact.o
OBJ3_SNWCPYX= snwcpyer.o snwcpyrev.o
OBJ4_SNWCPYX= snwcpyhyphen.o snwcpywidehdr.o
OBJ5_SNWCPYX= snwcpyexpesc.o

OBJA_SNWCPYX= obj0.o obj1.o
OBJB_SNWCPYX= obj2.o obj3.o
OBJC_SNWCPYX= obj4.o obj5.o

OBJ_SNWCPYX= obja.o objb.o objc.o


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


$(T).o:			$(OBJ_SNWCPYX)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ_SNWCPYX)

$(T).nm:		$(T).so
	$(NM) $(NMFLAGS) $(T).so > $(T).nm

again:
	rm -f $(ALL)

clean:
	makeclean $(ALL)

control:
	(uname -n ; date) > Control


obj0.o:			$(OBJ0_SNWCPYX)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ0_SNWCPYX)

obj1.o:			$(OBJ1_SNWCPYX)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ1_SNWCPYX)

obj2.o:			$(OBJ2_SNWCPYX)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ2_SNWCPYX)

obj3.o:			$(OBJ3_SNWCPYX)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ3_SNWCPYX)

obj4.o:			$(OBJ4_SNWCPYX)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ4_SNWCPYX)

obj5.o:			$(OBJ5_SNWCPYX)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ5_SNWCPYX)


obja.o:			$(OBJA_SNWCPYX)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJA_SNWCPYX)

objb.o:			$(OBJB_SNWCPYX)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJB_SNWCPYX)

objc.o:			$(OBJC_SNWCPYX)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJC_SNWCPYX)


snwcpyclean.o:		snwcpyclean.cc		$(INCS)
snwcpycompact.o:	snwcpycompact.cc	$(INCS)
snwcpyer.o:		snwcpyer.cc		$(INCS)
snwcpyhyphen.o:		snwcpyhyphen.cc		$(INCS)
snwcpylatin.o:		snwcpylatin.cc		$(INCS)
snwcpyopaque.o:		snwcpyopaque.cc		$(INCS)
snwcpyrev.o:		snwcpyrev.cc		$(INCS)
snwcpyshrink.o:		snwcpyshrink.cc		$(INCS)
snwcpywidehdr.o:	snwcpywidehdr.cc	$(INCS)
snwcpyxc.o:		snwcpyxc.cc		$(INCS)
snwcpyexpesc.o:		snwcpyexpesc.cc		$(INCS)



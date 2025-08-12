# MAKEFILES (ema)

T= ema

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

INCS += ema.h

MODS += ema_asstr.ccm ema_entry.ccm ema_parts.ccm

LIBS +=


DEPS_MAIN += ema_asstr.o ema_entry.o ema_parts.o

OBJ0= ema_main.o ema_obj.o
OBJ1= ema_parse.o ema_load.o
OBJ2= ema_haveaddr.o ema_first.o

OBJ3= $(DEPS_MAIN)

OBJA= obj0.o obj1.o
OBJB= obj2.o obj3.o

OBJ_EMA= obja.o objb.o


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


$(T).o:			$(OBJ_EMA)
	$(LD) -r $(LDFLAGS) -o $@ $^

$(T).nm:		$(T).o
	$(NM) $(NMFLAGS) $(T).o > $(T).nm

again:
	rm -f $(ALL)

clean:
	makeclean $(ALL)

control:
	(uname -n ; date) > Control


obj0.o:			$(OBJ0)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ0)

obj1.o:			$(OBJ1)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ1)

obj2.o:			$(OBJ2)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ2)

obj3.o:			$(OBJ3)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ3)


obja.o:			$(OBJA)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJA)

objb.o:			$(OBJB)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJB)


ema_main.o:		ema_main.cc $(DEPS_MAIN)		$(INCS)
ema_parse.o:		ema_parse.cc $(DEPS_MAIN)		$(INCS)
ema_load.o:		ema_load.cc $(DEPS_MAIN)		$(INCS)
ema_obj.o:		ema_obj.cc				$(INCS)
ema_haveaddr.o:		ema_haveaddr.cc				$(INCS)
ema_first.o:		ema_first.cc				$(INCS)

# EMA_PARTS
ema_parts.o:		ema_parts.ccm				$(INCS)

# EMA_ENTRY
ema_entry.o:		ema_entry.ccm				$(INCS)

# EMA_ASSTR
ema_asstr.o:		ema_asstr0.o ema_asstr1.o
	$(LD) -r $(LDFLAGS) -o $@ $^

ema_asstr0.o:		ema_asstr.ccm				$(INCS)
	makemodule ema_asstr

ema_asstr1.o:		ema_asstr1.cc				$(INCS)
	makemodule ema_asstr
	$(COMPILE.cc) $<



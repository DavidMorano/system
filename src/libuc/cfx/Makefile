# MAKEFILES (cfx)

T= cfx

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


DEFS=

INCS= cfx.h cfutil.hh

MODS += cfalphax.ccm cfcharsx.ccm cfdigx.ccm
MODS += cfpowx.ccm cfsysx.ccm

LIBS=


OBJ0_CFX= mods.o
OBJ1_CFX= cfbin.o 
OBJ2_CFX= cfoct.o
OBJ3_CFX= cfdec.o cfdecf.o cfdecmf.o cfdect.o
OBJ4_CFX= cfhex.o
OBJ5_CFX= cfxxx.o
OBJ6_CFX= cfa26.o
OBJ7_CFX= cfnum.o
OBJ8_CFX= cfutil.o
OBJ9_CFX= cfchars.o cfhexstr.o cfroman.o

OBJA_CFX= obj0cfx.o obj1cfx.o obj2cfx.o obj3cfx.o
OBJB_CFX= obj4cfx.o obj5cfx.o obj6cfx.o obj7cfx.o
OBJC_CFX= obj8cfx.o

OBJ_CFX= obja_cfx.o objb_cfx.o objc_cfx.o


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


$(T).o:			$(OBJ_CFX)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ_CFX)

$(T).nm:		$(T).o
	$(NM) $(NMFLAGS) $(T).o > $(T).nm

again:
	rm -f $(ALL)

clean:
	makeclean $(ALL)

control:
	(uname -n ; date) > Control


obj0cfx.o:		$(OBJ0_CFX)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj1cfx.o:		$(OBJ1_CFX)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj2cfx.o:		$(OBJ2_CFX)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj3cfx.o:		$(OBJ3_CFX)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj4cfx.o:		$(OBJ4_CFX)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj5cfx.o:		$(OBJ5_CFX)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj6cfx.o:		$(OBJ6_CFX)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj7cfx.o:		$(OBJ7_CFX)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj8cfx.o:		$(OBJ8_CFX)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj9cfx.o:		$(OBJ9_CFX)
	$(LD) -r $(LDFLAGS) -o $@ $^


obja_cfx.o:		$(OBJA_CFX)
	$(LD) -r $(LDFLAGS) -o $@ $^

objb_cfx.o:		$(OBJB_CFX)
	$(LD) -r $(LDFLAGS) -o $@ $^

objc_cfx.o:		$(OBJC_CFX)
	$(LD) -r $(LDFLAGS) -o $@ $^

objd_cfx.o:		$(OBJD_CFX)
	$(LD) -r $(LDFLAGS) -o $@ $^


cfutil.o:		cfutil.cc cfutil.hh			$(INCS)

cfchars.o:		mods.o cfchars.cc cfchars.h		$(INCS)
cfbin.o:		mods.o cfbin.cc cfbin.h			$(INCS)
cfoct.o:		mods.o cfoct.cc cfoct.h			$(INCS)
cfdec.o:		mods.o cfdec.cc cfdec.h			$(INCS)
cfdecf.o:		mods.o cfdecf.cc cfdecf.h		$(INCS)
cfdecmf.o:		mods.o cfdecmf.cc cfdecmf.h		$(INCS)
cfdect.o:		mods.o cfdect.cc cfdect.h		$(INCS)
cfhex.o:		mods.o cfhex.cc cfhex.h			$(INCS)
cfxxx.o:		mods.o cfxxx.cc cfxxx.h			$(INCS)
cfnum.o:		mods.o cfnum.cc cfnum.h			$(INCS)
cfa26.o:		mods.o cfa26.cc cfa26.h			$(INCS)
cfroman.o:		cfroman.cc cfroman.h			$(INCS)

# algorithms
cfhexstr.o:		cfhexstr.cc cfhexstr.h			$(INCS)

MOBJ= cfalphax.o cfcharsx.o cfdigx.o cfpowx.o cfsysx.o

mods.o:			$(MOBJ)
	$(LD) -r $(LDFLAGS) -o $@ $(MOBJ)

cfalphax.o:		cfalphax.ccm	cfutil.hh
cfcharsx.o:		cfcharsx.ccm	cfutil.hh
cfdigx.o:		cfdigx.ccm	cfutil.hh
cfpowx.o:		cfpowx.ccm	cfutil.hh
cfsysx.o:		cfsysx.ccm	cfutil.hh



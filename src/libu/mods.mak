# MAKEFILE (umods)

T= umods

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

INCS += umods.h

MODS += valuelims.o digbufsizes.o
MODS += bitop.o
MODS += digtab.o
MODS +=
MODS += sysdbfnames.o constdiv.o builtin.o
MODS += usysbasic.o

LIBS +=


OBJ00_UMODS = valuelims.o 
OBJ01_UMODS = builtin.o
OBJ02_UMODS = digtab.o
OBJ03_UMODS = bitop.o
OBJ04_UMODS = sysdbfnames.o 
OBJ05_UMODS = constdiv.o 
OBJ06_UMODS = usysbasic.o
OBJ07_UMODS = digbufsizes.o numbasedigs.o

OBJA_UMODS= obj00_umods.o obj01_umods.o 
OBJB_UMODS= obj02_umods.o obj03_umods.o
OBJC_UMODS= obj04_umods.o obj05_umods.o 
OBJD_UMODS= obj06_umods.o obj07_umods.o

OBJ_UMODS= obja_umods.o objb_umods.o objc_umods.o objd_umods.o


INCDIRS=
LIBDIRS=

RUNINFO= -rpath $(RUNDIR)
LIBINFO= $(LIBDIRS) $(LIBS)

# flag setting
CPPFLAGS	?= $(DEFS) $(INCDIRS) $(MAKECPPFLAGS)
CFLAGS		?= $(MAKECFLAGS)
CXXFLAGS	?= $(MAKECXXFLAGS)
ARFLAGS		?= $(MAKEARFLAGS)
LDFLAGS		?= $(MAKELDFLAGS)

#SOFL= -shared -Xlinker -flat_namespace -Xlinker -undefined -Xlinker suppress
#SOFL= -shared -Xlinker -undefined -Xlinker dynamic_lookup
SOFL= -shared


.SUFFIXES:		.hh .ii .iim .ccm


default:		$(T).o

all:			$(ALL)

so:			$(T).so


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


$(T).o:			$(OBJ_UMODS) Makefile
	$(LD) -r -o $@ $(LDFLAGS) $(OBJ_UMODS)

$(T).nm:		$(T).o
	$(NM) $(NMFLAGS) $(T).o > $(T).nm

safe:
	makesafe -v=3 -I $(INCDIR) $(OBJ)

again:
	rm -f $(ALL)

clean:
	makeclean $(ALL)
	rmsubpat digtab		gcm.cache
	rmsubpat constdiv	gcm.cache
	rmsubpat builtin	gcm.cache
	rmsubpat bitop		gcm.cache
	rmsubpat valuelims	gcm.cache
	rmsubpat digbufsizes	gcm.cache
	rmsubpat usysbasic	gcm.cache
	rmsubpat numbasedigs	gcm.cache
	rmobj

control:
	(uname -n ; date) > Control


obj00_umods.o:		$(OBJ00_UMODS)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJ00_UMODS)

obj01_umods.o:		$(OBJ01_UMODS)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJ01_UMODS)

obj02_umods.o:		$(OBJ02_UMODS)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJ02_UMODS)

obj03_umods.o:		$(OBJ03_UMODS)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJ03_UMODS)


obj04_umods.o:		$(OBJ04_UMODS)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJ04_UMODS)

obj05_umods.o:		$(OBJ05_UMODS)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJ05_UMODS)

obj06_umods.o:		$(OBJ06_UMODS)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJ06_UMODS)

obj07_umods.o:		$(OBJ07_UMODS)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJ07_UMODS)


obj08_umods.o:		$(OBJ08_UMODS)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJ08_UMODS)

obj09_umods.o:		$(OBJ09_UMODS)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJ09_UMODS)

obj10_umods.o:		$(OBJ10_UMODS)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJ10_UMODS)

obj11_umods.o:		$(OBJ11_UMODS)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJ11_UMODS)


obj12_umods.o:		$(OBJ12_UMODS)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJ12_UMODS)

obj13_umods.o:		$(OBJ13_UMODS)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJ13_UMODS)

obj14_umods.o:		$(OBJ14_UMODS)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJ14_UMODS)

obj15_umods.o:		$(OBJ15_UMODS)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJ15_UMODS)


obja_umods.o:		$(OBJA_UMODS)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJA_UMODS)

objb_umods.o:		$(OBJB_UMODS)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJB_UMODS)

objc_umods.o:		$(OBJC_UMODS)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJC_UMODS)

objd_umods.o:		$(OBJD_UMODS)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJD_UMODS)

obje_umods.o:		$(OBJE_UMODS)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJE_UMODS)

objf_umods.o:		$(OBJF_UMODS)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJF_UMODS)


# VARIOUS (module)
digtab.o:		digtab.ccm
	gxx -c -x c++ -o $@ $(CPPFLAGS) $(CXXFLAGS) $<

constdiv.o:		constdiv.ccm
	gxx -c -x c++ -o $@ $(CPPFLAGS) $(CXXFLAGS) $<

builtin.o:		builtin.ccm
	gxx -c -x c++ -o $@ $(CPPFLAGS) $(CXXFLAGS) $<

bitop.o:		bitop.ccm
	gxx -c -x c++ -o $@ $(CPPFLAGS) $(CXXFLAGS) $<

# VALUELIMS (module)
valuelims.o:		valuelims.ccm			$(INCS)
	gxx -c -x c++ -o $@ $(CPPFLAGS) $(CXXFLAGS) $<

# DIGBUFSIZES (module)
digbufsizes.o:		digbufsizes.ccm
	gxx -c -x c++ -o $@ $(CPPFLAGS) $(CXXFLAGS) $<

# SYSDBFNAMES
sysdbfnames.o:		sysdbfnames0.o sysdbfnames1.o
	$(LD) -r -o $@ $(LDFLAGS) $^

sysdbfnames0.o:		sysdbfnames.ccm sysdbfnames.hh
	gxx -c -x c++ -o $@ $(CPPFLAGS) $(CXXFLAGS) $<

sysdbfnames1.o:		sysdbfnames1.cc sysdbfnames0.o sysdbfnames.hh
	$(COMPILE.cc) $<

# USYSBASIC
usysbasic.o:		usysbasic0.o usysbasic1.o
	$(LD) -r -o $@ $(LDFLAGS) $^

usysbasic0.o:		usysbasic.ccm
	gxx -c -x c++ -o $@ $(CPPFLAGS) $(CXXFLAGS) $<

usysbasic1.o:		usysbasic1.cc usysbasic0.o
	$(COMPILE.cc) $<

# NUMBASEDIGS (module)
numbasedigs.o:		numbasedigs.ccm
	gxx -c -x c++ -o $@ $(CPPFLAGS) $(CXXFLAGS) $<



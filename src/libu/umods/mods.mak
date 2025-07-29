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

INCS += umods.h

MODS += valuelims.ccm digbufsizes.ccm uconstants.ccm 
MODS += bitop.ccm
MODS += xxtostr.ccm digtab.ccm
MODS += ulibvals.ccm
MODS += usigset.ccm usigblock.ccm
MODS += unixfnames.ccm constdiv.ccm builtin.ccm
MODS += usysbasic.ccm

LIBS += -liconv -lproc


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


OBJ00_UMODS = valuelims.o digbufsizes.o 
OBJ01_UMODS = uconstants.o
OBJ02_UMODS = xxtostr.o digtab.o bitop.o
OBJ03_UMODS = ulibvals.o 
OBJ04_UMODS = usigset.o usigblock.o 
OBJ05_UMODS = unixfnames.o constdiv.o 
OBJ06_UMODS = usysbasic.o
OBJ07_UMODS = builtin.o

OBJA_UMODS= obj00_umods.o obj01_umods.o 
OBJB_UMODS= obj02_umods.o obj03_umods.o
OBJC_UMODS= obj04_umods.o obj05_umods.o 
OBJD_UMODS= obj06_umods.o obj07_umods.o

OBJ_UMODS= obja_umods.o objb_umods.o objc_umods.o objd_umods.o


.SUFFIXES:		.hh .ii .ccm


default:		$(T).o

all:			$(ALL)

so:			$(T).so


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


$(T).o:			$(OBJ_UMODS) Makefile
	$(LD) -r -o $@ $(LDFLAGS) $(OBJ_UMODS)

$(T).nm:		$(T).o
	$(NM) $(NMFLAGS) $(T).o > $(T).nm

install-pre:
	filefind . -s h | makenewer -af - -d $(INCDIR)

install:		$(ALL) Makefile
	ranlib $(T).a
	install -S -p -m 0775 $(T).so $(LIBDIR)

install-incs:		$(INSTALLINCS)
	makenewer $(INSTALLINCS) $(INCDIR)

safe:
	makesafe -v=3 -I $(INCDIR) $(OBJ)

again:
	rm -f $(ALL)

clean:
	makeclean $(ALL)
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
usigset.o:		usigset.ccm
constdiv.o:		constdiv.ccm
builtin.o:		builtin.ccm
bitop.o:		bitop.ccm

# VALUELIMS (module)
valuelims.o:		valuelims.ccm			$(INCS)

# DIGBUFSIZES (module)
digbufsizes.o:		digbufsizes.ccm

# UCONSTANTS
uconstants.o:		uconstants0.o uconstants1.o
	$(LD) -r -o $@ $(LDFLAGS) $^

uconstants0.o:		uconstants.ccm valuelims.o digbufsizes.o 
	makemodule uconstants

uconstants1.o:		uconstants1.cc uconstants.ccm 
	makemodule uconstants
	$(COMPILE.cc) $<

# ULIBVALS
ulibvals.o:		ulibvals0.o ulibvals1.o
	$(LD) -r -o $@ $(LDFLAGS) $^

ulibvals0.o:		ulibvals.ccm
	makemodule ulibvals

ulibvals1.o:		ulibvals1.cc ulibvals.ccm 
	makemodule ulibvals
	$(COMPILE.cc) $<

# UNIXFNAMES
unixfnames.o:		unixfnames0.o unixfnames1.o
	$(LD) -r -o $@ $(LDFLAGS) $^

unixfnames0.o:		unixfnames.ccm
	makemodule unixfnames

unixfnames1.o:		unixfnames1.cc unixfnames.ccm 
	makemodule unixfnames
	$(COMPILE.cc) $<

# USIGBLOCK
usigblock.o:		usigblock.ccm usigset.o 
	makemodule usigset
	makemodule usigblock

# USYSBASIC
usysbasic.o:		usysbasic0.o usysbasic1.o
	$(LD) -r -o $@ $(LDFLAGS) $^

usysbasic0.o:		usysbasic.ccm usigset.o usigblock.o 
	makemodule usigset
	makemodule usigblock
	makemodule usysbasic

usysbasic1.o:		usysbasic1.cc usysbasic.ccm
	makemodule usysbasic
	$(COMPILE.cc) $<



# MAKEFILE (proginfo)

T= proginfo

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

INCS += proginfo.hh

MODS += proginfo_base.o proginfo_rand.o proginfo_log.o

LIBS += -luo -lu


# overall
PR_PROG= proginfo0.o

# base
# module primary
PR_BASE= proginfo_base0.o

# module parts
PA_BASE=
	
# module implementations
PI_BASE0= proginfo_base1.o
PI_BASE1= proginfo_base2.o
PI_BASE2= proginfo_base3.o proginfo_base4.o
PI_BASE3= proginfo_base5.o

PI_BASE= pi_base0.o pi_base1.o pi_base2.o pi_base3.o

MOD_BASE= proginfo_base.o

# rand
PR_RAND= proginfo_rand0.o

PA_RAND=

PI_RAND=

MOD_RAND= proginfo_rand.o

# log
PR_LOG= proginfo_log0.o

PI_LOG0= proginfo_log1.o proginfo_log2.o
PI_LOG1=
PI_LOG2=
PI_LOG3=

PI_LOG= pi_log0.o

MOD_LOG= proginfo_log.o

OBJ0= proginfo_base.o
OBJ1= proginfo_rand.o
OBJ2= proginfo_log.o
OBJ3=

OBJ= obj0.o obj1.o obj2.o


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


default:		all

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

control:
	(uname -n ; date) > Control

pi_base0.o:		$(PI_BASE0)
	$(LD) -r -o $@ $(LDFLAGS) $^

pi_base1.o:		$(PI_BASE1)
	$(LD) -r -o $@ $(LDFLAGS) $^

pi_base2.o:		$(PI_BASE2)
	$(LD) -r -o $@ $(LDFLAGS) $^

pi_base3.o:		$(PI_BASE3)
	$(LD) -r -o $@ $(LDFLAGS) $^

pi_base.o:		$(PI_BASE)
	$(LD) -r $(LDFLAGS) -o $@ $^


pi_rand0.o:		$(PI_RAND0)
	$(LD) -r -o $@ $(LDFLAGS) $^

pi_rand1.o:		$(PI_RAND1)
	$(LD) -r -o $@ $(LDFLAGS) $^

pi_rand2.o:		$(PI_RAND2)
	$(LD) -r -o $@ $(LDFLAGS) $^

pi_rand3.o:		$(PI_RAND3)
	$(LD) -r -o $@ $(LDFLAGS) $^

pi_rand.o:		$(PI_RAND)
	$(LD) -r -o $@ $(LDFLAGS) $^


pi_log0.o:		$(PI_LOG0)
	$(LD) -r -o $@ $(LDFLAGS) $^

pi_log1.o:		$(PI_LOG1)
	$(LD) -r -o $@ $(LDFLAGS) $^

pi_log2.o:		$(PI_LOG2)
	$(LD) -r -o $@ $(LDFLAGS) $^

pi_log3.o:		$(PI_LOG3)
	$(LD) -r -o $@ $(LDFLAGS) $^

pi_log.o:		$(PI_LOG)
	$(LD) -r -o $@ $(LDFLAGS) $^


# module
proginfo.o:		$(PR_PROG) $(MOD_BASE) $(MOD_RAND) $(MOD_LOG)
	$(LD) -r -o $@ $(LDFLAGS) $^

proginfo0.o:		proginfo.ccm $(PR_BASE) $(PR_RAND) $(PR_LOG)	$(INCS)
	gxx -c -x c++ -o $@ $(CPPFLAGS) $(CXXFLAGS) $<

# sub-module
proginfo_base.o:	$(PA_BASE) $(PR_BASE) $(PI_BASE)
	$(LD) -r -o $@ $(LDFLAGS) $^

# sub-module-primary
proginfo_base0.o:	proginfo_base.ccm				$(INCS)
	gxx -c -x c++ -o $@ $(CPPFLAGS) $(CXXFLAGS) $<

# sub-module-implementations
proginfo_base1.o:	proginfo_base1.cc		$(PR_BASE)	$(INCS)
proginfo_base2.o:	proginfo_base2.cc		$(PR_BASE)	$(INCS)
proginfo_base3.o:	proginfo_base3.cc		$(PR_BASE)	$(INCS)
proginfo_base4.o:	proginfo_base4.cc		$(PR_BASE)	$(INCS)
proginfo_base5.o:	proginfo_base5.cc		$(PR_BASE)	$(INCS)

# sub-module-primary
proginfo_rand.o:	$(PA_RAND) $(PR_RAND) $(PI_RAND)
	$(LD) -r -o $@ $(LDFLAGS) $^

proginfo_rand0.o:	proginfo_rand.ccm $(PR_BASE)			$(INCS)
	gxx -c -x c++ -o $@ $(CPPFLAGS) $(CXXFLAGS) $<

# sub-module-primary
proginfo_log.o:		$(PA_LOG) $(PR_LOG) $(PI_LOG)
	$(LD) -r -o $@ $(LDFLAGS) $^

proginfo_log0.o:	proginfo_log.ccm $(PR_BASE)			$(INCS)
	gxx -c -x c++ -o $@ $(CPPFLAGS) $(CXXFLAGS) $<

proginfo_log1.o:	proginfo_log1.cc $(PR_BASE) $(PR_LOG)		$(INCS)

proginfo_log2.o:	proginfo_log2.cc $(PR_BASE) $(PR_LOG)		$(INCS)

proginfo_log3.o:	proginfo_log3.cc $(PR_BASE) $(PR_LOG)		$(INCS)

proginfo_log4.o:	proginfo_log4.cc $(PR_BASE) $(PR_LOG)		$(INCS)

proginfo_log5.o:	proginfo_log5.cc $(PR_BASE) $(PR_LOG)		$(INCS)


progmsgid.o:		progmsgid.cc	progmsgid.h			$(INCS)
proguserlist.o:		proguserlist.cc	proguserlist.h			$(INCS)
progexpand.o:		progexpand.cc	progexpand.h			$(INCS)

logzone.o:		logzone.cc	logzone.h



# MAKEFILE (modmisc)

T= modmisc

ALL= $(T).o $(T).so


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

INCS += modmisc.hh

MODS +=

LIBS +=


LIBFMISC_OP= modmisc0.o

LIBFMISC_OA0= modmisc-mknpathx.o
LIBFMISC_OA1= modmisc-mknpathxw.o
LIBFMISC_OA2= modmisc-pathnadd.o
LIBFMISC_OA3= modmisc-snadd.o

LIBFMISC_OA+= modmisc_oa0.o modmisc_oa1.o
LIBFMISC_OA+= modmisc_oa2.o modmisc_oa3.o

LIBFMISC_OI0= modmisc1.o modmisc2.o
LIBFMISC_OI1= modmisc3.o modmisc4.o
LIBFMISC_OI2= modmisc5.o modmisc6.o

LIBFMISC_OI= modmisc_oi0.o modmisc_oi1.o modmisc_oi2.o


INCDIRS +=
LIBDIRS += -L lib

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


$(T).so:		$(T).o
	$(CXX) -shared $(LDFLAGS) -o $@ $^ $(RUNINFO) $(LIBINFO)

$(T).o:			modmisc_oa.o modmisc0.o modmisc_oi.o
	$(LD) -r $(LDFLAGS) -o $@ $^

$(T).nm:		$(T).o
	$(NM) $(NMFLAGS) $(T).o > $(T).nm

again:
	$(RM) $(ALL)

clean:
	makeclean $(ALL)

control:
	(uname -n ; date) > Control


# primary
modmisc0.o:		modmisc.ccm modmisc_oa.o	modmisc.hh	$(INCS)
	gxx -c -x c++ -o $@ $(CPPFLAGS) $(CXXFLAGS) $<

modmisc_op.o:		modmisc0.o
	$(LD) -r $(LDFLAGS) -o $@ $^

# partitions
modmisc-mknpathx.o:	modmisc-mknpathx.ccm	modmisc.hh	$(INCS)
modmisc-mknpathxw.o:	modmisc-mknpathxw.ccm	modmisc.hh	$(INCS)
modmisc-pathnadd.o:	modmisc-pathnadd.ccm	modmisc.hh	$(INCS)
modmisc-snadd.o:	modmisc-snadd.ccm	modmisc.hh	$(INCS)

modmisc_oa0.o:		$(LIBFMISC_OA0)
	$(LD) -r $(LDFLAGS) -o $@ $^

modmisc_oa1.o:		$(LIBFMISC_OA1)
	$(LD) -r $(LDFLAGS) -o $@ $^

modmisc_oa2.o:		$(LIBFMISC_OA2)
	$(LD) -r $(LDFLAGS) -o $@ $^

modmisc_oa3.o:		$(LIBFMISC_OA3)
	$(LD) -r $(LDFLAGS) -o $@ $^

modmisc_oa.o:		$(LIBFMISC_OA)
	$(LD) -r $(LDFLAGS) -o $@ $^

modmisc_oi0.o:		$(LIBFMISC_OI0)
	$(LD) -r $(LDFLAGS) -o $@ $^

modmisc_oi1.o:		$(LIBFMISC_OI1)
	$(LD) -r $(LDFLAGS) -o $@ $^

modmisc_oi2.o:		$(LIBFMISC_OI2)
	$(LD) -r $(LDFLAGS) -o $@ $^

modmisc_oi.o:		$(LIBFMISC_OI)
	$(LD) -r $(LDFLAGS) -o $@ $^

# implementation
modmisc1.o:		modmisc1.cc modmisc0.o 	$(DEPS_MISC) 	$(INCS)
modmisc2.o:		modmisc2.cc modmisc0.o	$(DEPS_MISC)	$(INCS)
modmisc3.o:		modmisc3.cc modmisc0.o	$(DEPS_MISC)	$(INCS)
modmisc4.o:		modmisc4.cc modmisc0.o	$(DEPS_MISC)	$(INCS)
modmisc5.o:		modmisc5.cc modmisc0.o	$(DEPS_MISC)	$(INCS)
modmisc6.o:		modmisc6.cc modmisc0.o	$(DEPS_MISC)	$(INCS)



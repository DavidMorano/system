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


DEPS_IMPL= modmisc0.o

# module primary
MODMISC_OP= modmisc0.o

# module partitions
MODMISC_OA0= modmisc-mknpathx.o
MODMISC_OA1= modmisc-mknpathxw.o
MODMISC_OA2= modmisc-pathnadd.o
MODMISC_OA3= modmisc-snadd.o

MODMISC_OA4= modmisc-strw.o
MODMISC_OA5= modmisc-strbcpy.o
MODMISC_OA6=
MODMISC_OA7= 

MODMISC_OA+= modmisc_oa0.o modmisc_oa1.o
MODMISC_OA+= modmisc_oa2.o modmisc_oa3.o
MODMISC_OA+= modmisc_oa4.o modmisc_oa5.o

# module implementations
MODMISC_OI0= modmisc1.o modmisc2.o
MODMISC_OI1= modmisc3.o modmisc4.o
MODMISC_OI2= modmisc5.o modmisc6.o
MODMISC_OI3= modmisc7.o
MODMISC_OI4= modmisc%strw.o modmisc%strbcpy.o
MODMISC_OI5=
MODMISC_OI6=
MODMISC_OI7=

MODMISC_OI+= modmisc_oi0.o modmisc_oi1.o 
MODMISC_OI+= modmisc_oi2.o modmisc_oi3.o
MODMISC_OI+= modmisc_oi4.o


DEPS_PRIM= $(MODMISC_OA)


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
	rmsubpat

control:
	(uname -n ; date) > Control


# primary
modmisc0.o:		modmisc.ccm modmisc_oa.o	modmisc.hh	$(INCS)
	gxx -c -x c++ -o $@ $(CPPFLAGS) $(CXXFLAGS) $<

# partitions
modmisc-mknpathx.o:	modmisc-mknpathx.ccm	modmisc.hh	$(INCS)
modmisc-mknpathxw.o:	modmisc-mknpathxw.ccm	modmisc.hh	$(INCS)
modmisc-pathnadd.o:	modmisc-pathnadd.ccm	modmisc.hh	$(INCS)
modmisc-snadd.o:	modmisc-snadd.ccm	modmisc.hh	$(INCS)

modmisc_oa0.o:		$(MODMISC_OA0)
	$(LD) -r $(LDFLAGS) -o $@ $^

modmisc_oa1.o:		$(MODMISC_OA1)
	$(LD) -r $(LDFLAGS) -o $@ $^

modmisc_oa2.o:		$(MODMISC_OA2)
	$(LD) -r $(LDFLAGS) -o $@ $^

modmisc_oa3.o:		$(MODMISC_OA3)
	$(LD) -r $(LDFLAGS) -o $@ $^

modmisc_oa4.o:		$(MODMISC_OA4)
	$(LD) -r $(LDFLAGS) -o $@ $^

modmisc_oa5.o:		$(MODMISC_OA5)
	$(LD) -r $(LDFLAGS) -o $@ $^

modmisc_oa6.o:		$(MODMISC_OA6)
	$(LD) -r $(LDFLAGS) -o $@ $^

modmisc_oa7.o:		$(MODMISC_OA7)
	$(LD) -r $(LDFLAGS) -o $@ $^

# consolidation partitions
modmisc_oa.o:		$(MODMISC_OA)
	$(LD) -r $(LDFLAGS) -o $@ $^


modmisc_oi0.o:		$(MODMISC_OI0)
	$(LD) -r $(LDFLAGS) -o $@ $^

modmisc_oi1.o:		$(MODMISC_OI1)
	$(LD) -r $(LDFLAGS) -o $@ $^

modmisc_oi2.o:		$(MODMISC_OI2)
	$(LD) -r $(LDFLAGS) -o $@ $^

modmisc_oi3.o:		$(MODMISC_OI3)
	$(LD) -r $(LDFLAGS) -o $@ $^

modmisc_oi4.o:		$(MODMISC_OI4)
	$(LD) -r $(LDFLAGS) -o $@ $^

modmisc_oi5.o:		$(MODMISC_OI5)
	$(LD) -r $(LDFLAGS) -o $@ $^

modmisc_oi6.o:		$(MODMISC_OI6)
	$(LD) -r $(LDFLAGS) -o $@ $^

modmisc_oi7.o:		$(MODMISC_OI7)
	$(LD) -r $(LDFLAGS) -o $@ $^

# consolidation implementations
modmisc_oi.o:		$(MODMISC_OI)
	$(LD) -r $(LDFLAGS) -o $@ $^

# implementation
modmisc1.o:		modmisc1.cc	 		$(DEPS_IMPL) 	$(INCS)
modmisc2.o:		modmisc2.cc			$(DEPS_IMPL)	$(INCS)
modmisc3.o:		modmisc3.cc			$(DEPS_IMPL)	$(INCS)
modmisc4.o:		modmisc4.cc			$(DEPS_IMPL)	$(INCS)
modmisc5.o:		modmisc5.cc			$(DEPS_IMPL)	$(INCS)
modmisc6.o:		modmisc6.cc			$(DEPS_IMPL)	$(INCS)
modmisc7.o:		modmisc7.cc			$(DEPS_IMPL)	$(INCS)
modmisc%strw.o:		modmisc%strw.cc			$(DEPS_IMPL)	$(INCS)
modmisc%strbcpy.o:	modmisc%strbcp.cc		$(DEPS_IMPL)	$(INCS)



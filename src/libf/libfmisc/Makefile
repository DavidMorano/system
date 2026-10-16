# MAKEFILE (libfmisc)

T= libfmisc

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

INCS += $(T).hh

MODS += 

LIBS +=


DEPS= 		$(MODS)
DEPS_MISC=	

# primary
LIBFMISC_OP= libfmisc0.o

# parts
LIBFMISC_OA0= libfmisc-mknpathx.o
LIBFMISC_OA1= libfmisc-mknpathxw.o
LIBFMISC_OA2= libfmisc-pathnadd.o
LIBFMISC_OA3= libfmisc-snadd.o

LIBFMISC_OA+= libfmisc_oa0.o libfmisc_oa1.o
LIBFMISC_OA+= libfmisc_oa2.o libfmisc_oa3.o

# implementation
LIBFMISC_OI0= libfmisc1.o libfmisc2.o
LIBFMISC_OI1= libfmisc3.o libfmisc4.o
LIBFMISC_OI2= libfmisc5.o libfmisc6.o

LIBFMISC_OI= libfmisc_oi0.o libfmisc_oi1.o libfmisc_oi2.o

# all-together
OBJ= $(LIBFMISC_OA) $(LIBFMISC_OP) $(LIBFMISC_OI)


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

$(T).o:			libfmisc_oa.o libfmisc0.o libfmisc_oi.o
	$(LD) -r $(LDFLAGS) -o $@ $^

$(T).nm:		$(T).o
	$(NM) $(NMFLAGS) $(T).o > $(T).nm

again:
	$(RM) $(ALL)

clean:
	makeclean $(ALL)

control:
	(uname -n ; date) > Control


objpart00.o:		$(OBJPART00)
	$(LD) -r $(LDFLAGS) -o $@ $^

objpart01.o:		$(OBJPART01)
	$(LD) -r $(LDFLAGS) -o $@ $^

objpart02.o:		$(OBJPART02)
	$(LD) -r $(LDFLAGS) -o $@ $^

objpart03.o:		$(OBJPART03)
	$(LD) -r $(LDFLAGS) -o $@ $^

objpart04.o:		$(OBJPART04)
	$(LD) -r $(LDFLAGS) -o $@ $^

objpart05.o:		$(OBJPART05)
	$(LD) -r $(LDFLAGS) -o $@ $^

objpart06.o:		$(OBJPART06)
	$(LD) -r $(LDFLAGS) -o $@ $^

objpart07.o:		$(OBJPART07)
	$(LD) -r $(LDFLAGS) -o $@ $^

objpart08.o:		$(OBJPART08)
	$(LD) -r $(LDFLAGS) -o $@ $^

objpart09.o:		$(OBJPART09)
	$(LD) -r $(LDFLAGS) -o $@ $^

objpart10.o:		$(OBJPART10)
	$(LD) -r $(LDFLAGS) -o $@ $^

objpart11.o:		$(OBJPART11)
	$(LD) -r $(LDFLAGS) -o $@ $^


objparta.o:		$(OBJPARTA)
	$(LD) -r $(LDFLAGS) -o $@ $^

objpartb.o:		$(OBJPARTB)
	$(LD) -r $(LDFLAGS) -o $@ $^

objpartc.o:		$(OBJPARTC)
	$(LD) -r $(LDFLAGS) -o $@ $^

objpartd.o:		$(OBJPARTD)
	$(LD) -r $(LDFLAGS) -o $@ $^

objparte.o:		$(OBJPARTE)
	$(LD) -r $(LDFLAGS) -o $@ $^

objpartf.o:		$(OBJPARTF)
	$(LD) -r $(LDFLAGS) -o $@ $^

objpartg.o:		$(OBJPARTG)
	$(LD) -r $(LDFLAGS) -o $@ $^

objparth.o:		$(OBJPARTH)
	$(LD) -r $(LDFLAGS) -o $@ $^


obj0.o:			$(OBJ0)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj1.o:			$(OBJ1)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj2.o:			$(OBJ2)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj3.o:			$(OBJ3)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj4.o:			$(OBJ4)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj5.o:			$(OBJ5)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj6.o:			$(OBJ6)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj7.o:			$(OBJ7)
	$(LD) -r $(LDFLAGS) -o $@ $^


obja.o:			$(OBJA)
	$(LD) -r $(LDFLAGS) -o $@ $^

objb.o:			$(OBJB)
	$(LD) -r $(LDFLAGS) -o $@ $^


# parts
objpart.o:		$(OBJPART)
	$(LD) -r $(LDFLAGS) -o $@ $^

objprime.o:		$(OBJPRIME)
	$(LD) -r $(LDFLAGS) -o $@ $^

# implementation
objimpl.o:		$(OBJIMPL)
	$(LD) -r $(LDFLAGS) -o $@ $^

# primary
libfmisc0.o:		libfmisc.ccm libfmisc_oa.o	libfmisc.hh	$(INCS)
	gxx -c -x c++ -o $@ $(CPPFLAGS) $(CXXFLAGS) $<

libfmisc_op.o:		libfmisc0.o
	$(LD) -r $(LDFLAGS) -o $@ $^

# partitions
libfmisc-mknpathx.o:	libfmisc-mknpathx.ccm	libfmisc.hh	$(INCS)
libfmisc-mknpathxw.o:	libfmisc-mknpathxw.ccm	libfmisc.hh	$(INCS)
libfmisc-pathnadd.o:	libfmisc-pathnadd.ccm	libfmisc.hh	$(INCS)
libfmisc-snadd.o:	libfmisc-snadd.ccm	libfmisc.hh	$(INCS)

libfmisc_oa0.o:		$(LIBFMISC_OA0)
	$(LD) -r $(LDFLAGS) -o $@ $^

libfmisc_oa1.o:		$(LIBFMISC_OA1)
	$(LD) -r $(LDFLAGS) -o $@ $^

libfmisc_oa2.o:		$(LIBFMISC_OA2)
	$(LD) -r $(LDFLAGS) -o $@ $^

libfmisc_oa3.o:		$(LIBFMISC_OA3)
	$(LD) -r $(LDFLAGS) -o $@ $^

libfmisc_oa.o:		$(LIBFMISC_OA)
	$(LD) -r $(LDFLAGS) -o $@ $^

libfmisc_oi0.o:		$(LIBFMISC_OI0)
	$(LD) -r $(LDFLAGS) -o $@ $^

libfmisc_oi1.o:		$(LIBFMISC_OI1)
	$(LD) -r $(LDFLAGS) -o $@ $^

libfmisc_oi2.o:		$(LIBFMISC_OI2)
	$(LD) -r $(LDFLAGS) -o $@ $^

libfmisc_oi.o:		$(LIBFMISC_OI)
	$(LD) -r $(LDFLAGS) -o $@ $^

# implementation
libfmisc1.o:		libfmisc1.cc libfmisc0.o 	$(DEPS_MISC) 	$(INCS)
libfmisc2.o:		libfmisc2.cc libfmisc0.o	$(DEPS_MISC)	$(INCS)
libfmisc3.o:		libfmisc3.cc libfmisc0.o	$(DEPS_MISC)	$(INCS)
libfmisc4.o:		libfmisc4.cc libfmisc0.o	$(DEPS_MISC)	$(INCS)
libfmisc5.o:		libfmisc5.cc libfmisc0.o	$(DEPS_MISC)	$(INCS)
libfmisc6.o:		libfmisc6.cc libfmisc0.o	$(DEPS_MISC)	$(INCS)



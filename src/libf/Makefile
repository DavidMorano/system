# MAKEFILE (libf)

T= libf

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

INCS += libf.h

MODS += modstrw.o modsncpy.o modsnwcpy.o

LIBS += -lu


DEPS= 		libfsup.o libfmisc.o
DEPS_MISC=	modstrw.o modsncpy.o modsnwcpy.o

OBJPART00= libfsup-lenstr.o libfsup-memclear.o
OBJPART01= libfsup-memcopy.o libfsup-memload.o
OBJPART02= libfsup-memnset.o libfsup-resumelife.o
OBJPART03=

OBJPART04= 
OBJPART05= 
OBJPART06=
OBJPART07=

OBJPARTA= objpart00.o objpart01.o 
OBJPARTB= objpart02.o
OBJPARTC= objpart04.o objpart05.o 
OBJPARTD= objpart06.o objpart07.o
OBJPARTE= objpart08.o objpart09.o
OBJPARTF= objpart10.o

OBJPART+= objparta.o objpartb.o
#OBJPART+= objparta.o objpartb.o objpartc.o objpartd.o
#OBJPART+= objparte.o objpartf.o

OBJPRIME= libfsup0.o

LIBFMISC_OP= libfmisc0.o

LIBFMISC_OA0= libfmisc-mknpathx.o
LIBFMISC_OA1= libfmisc-mknpathxw.o
LIBFMISC_OA2= libfmisc-pathnadd.o
LIBFMISC_OA3= libfmisc-snadd.o

LIBFMISC_OA+= libfmisc_oa0.o libfmisc_oa1.o
LIBFMISC_OA+= libfmisc_oa2.o libfmisc_oa3.o

LIBFMISC_OI0= libfmisc1.o libfmisc2.o
LIBFMISC_OI1= libfmisc3.o libfmisc4.o
LIBFMISC_OI2= libfmisc5.o libfmisc6.o

LIBFMISC_OI= libfmisc_oi0.o libfmisc_oi1.o libfmisc_oi2.o

OBJ0= freadln.o
OBJ1= fwrite.o fprint.o fputch.o
OBJ2= fopenroot.o
OBJ3= $(DEPS)

OBJ4= $(MODS)
OBJ5=
OBJ6=
OBJ7=

OBJA= obj0.o obj1.o obj2.o obj3.o
OBJB= obj4.o

OBJ= obja.o objb.o

OBJIMPL= libfsup1.o


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

$(T).o:			$(OBJ)
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


freadln.o:		freadln.cc	freadln.h	$(DEPS) $(INCS)
fwrite.o:		fwrite.cc			$(DEPS) $(INCS)
fprint.o:		fprint.cc			$(DEPS) $(INCS)
fputch.o:		fputch.cc			$(DEPS) $(INCS)
fopenroot.o:		fopenroot.cc			$(DEPS) $(INCS)

objpart.o:		$(OBJPART)
	$(LD) -r $(LDFLAGS) -o $@ $^

objprime.o:		$(OBJPRIME)
	$(LD) -r $(LDFLAGS) -o $@ $^

objimpl.o:		$(OBJIMPL)
	$(LD) -r $(LDFLAGS) -o $@ $^

# MODULE (libfsup)
libfsup.o:		objpart.o objprime.o objimpl.o
	$(LD) -r $(LDFLAGS) -o $@ $^

# primary
libfsup0.o:		libfsup.ccm $(OBJPARTS)	libfsup.hh	$(INCS)
	gxx -c -x c++ -o $@ $(CPPFLAGS) $(CXXFLAGS) $<

# primary partitions
libfsup-lenstr.o:	libfsup-lenstr.ccm		libfsup.hh $(INCS)
libfsup-memclear.o:	libfsup-memclear.ccm		libfsup.hh $(INCS)
libfsup-memcopy.o:	libfsup-memcopy.ccm		libfsup.hh $(INCS)
libfsup-memload.o:	libfsup-memload.ccm		libfsup.hh $(INCS)
libfsup-memnset.o:	libfsup-memnset.ccm		libfsup.hh $(INCS)
libfsup-resumelife.o:	libfsup-resumelife.ccm		libfsup.hh $(INCS)

# implementaiton
libfsup1.o:		libfsup1.cc libfsup0.o			#(INCS)
	$(COMPILE.cc) $<

libfmisc.o:		libfmisc_oa.o libfmisc0.o libfmisc_oi.o
	$(LD) -r $(LDFLAGS) -o $@ $^

# primary
libfmisc0.o:		libfmisc.ccm libfmisc_oa.o	libfmisc.hh	$(INCS)
	gxx -c -x c++ -o $@ $(CPPFLAGS) $(CXXFLAGS) $<

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

# MODSTRW
modstrw.o:		modstrw.dir
modstrw.dir:
	makesubdir $@

# MODSNCPY
modsncpy.o:		modsncpy.dir
modsncpy.dir:
	makesubdir $@

# MODSNWCPY
modsnwcpy.o:		modsnwcpy.dir modstrw.o modsncpy.o
modsnwcpy.dir:
	makesubdir $@



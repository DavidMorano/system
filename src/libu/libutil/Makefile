# MAKEIFLE (libutil)

T= libutil

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

INCS += libutil.hh

MODS += modcstring.o

LIBS +=


DEPS += modcstring.o

OBJPART00= libutil-strlen.o libutil-strxspn.o
OBJPART01= libutil-cvtconst.o libutil-getlen.o 
OBJPART02= libutil-lenstr.o libutil-lenstrarr.o
OBJPART03= libutil-loadstrs.o libutil-memclear.o
OBJPART04= libutil-memcopy.o libutil-memcpy.o
OBJPART05= libutil-resumelife.o libutil-stpcpy.o
OBJPART06= libutil-strcopy.o libutil-strcpy.o
OBJPART07= libutil-memload.o libutil-strcmp.o

OBJPART08= libutil-getbyte.o
OBJPART09= libutil-memncpy.o libutil-memnset.o
OBJPART10= libutil-operators.o
OBJPART11=

OBJPARTA= objpart00.o objpart01.o 
OBJPARTB= objpart02.o objpart03.o 
OBJPARTC= objpart04.o objpart05.o 
OBJPARTD= objpart06.o objpart07.o
OBJPARTE= objpart08.o objpart09.o
OBJPARTF= objpart10.o

OBJPART+= objparta.o objpartb.o objpartc.o objpartd.o
OBJPART+= objparte.o objpartf.o

OBJPRIME= libutil0.o

OBJ00= libutil1.o
OBJ01= $(DEPS)
OBJ02=
OBJ03=

OBJIMPL= obj00.o obj01.o


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
	gxx -c -x c++ -o $@ $(CPPFLAGS) $(CXXFLAGS) $<


$(T).o:			objpart.o objprime.o objimpl.o
	$(LD) -r -o $@ $(LDFLAGS) $^

$(T).nm:		$(T).o
	$(NM) $(NMFLAGS) $(T).o > $(T).nm

safe:
	makesafe -v=3 -I $(INCDIR) $(OBJIMPL)

again:
	rm -f $(ALL)

clean:
	makeclean $(ALL)
	rmsubpat libutil	gcm.cache
	rmobj

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


objpart.o:		$(OBJPART)
	$(LD) -r $(LDFLAGS) -o $@ $^


obj00.o:		$(OBJ00)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj01.o:		$(OBJ01)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj02.o:		$(OBJ02)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj03.o:		$(OBJ03)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj0.o:			$(OBJ04)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj05.o:		$(OBJ05)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj06.o:		$(OBJ06)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj07.o:		$(OBJ07)
	$(LD) -r $(LDFLAGS) -o $@ $^


obja.o:			$(OBJA)
	$(LD) -r $(LDFLAGS) -o $@ $^

objb.o:			$(OBJB)
	$(LD) -r $(LDFLAGS) -o $@ $^

objc.o:			$(OBJC)
	$(LD) -r $(LDFLAGS) -o $@ $^

objd.o:			$(OBJD)
	$(LD) -r $(LDFLAGS) -o $@ $^


objprime.o:		$(OBJPRIME)
	$(LD) -r $(LDFLAGS) -o $@ $^

objimpl.o:		$(OBJIMPL)
	$(LD) -r $(LDFLAGS) -o $@ $^


libutil0.o:		libutil.ccm objpart.o		$(DEPS) $(INCS)
	gxx -c -x c++ -o $@ $(CPPFLAGS) $(CXXFLAGS) $<

libutil1.o:		libutil1.cc libutil0.o		$(DEPS) $(INCS)
libutil2.o:		libutil2.cc libutil0.o		$(DEPS) $(INCS)
libutil3.o:		libutil3.cc libutil0.o		$(DEPS) $(INCS)
libutil4.o:		libutil4.cc libutil0.o		$(DEPS) $(INCS)
libutil5.o:		libutil5.cc libutil0.o		$(DEPS) $(INCS)
libutil6.o:		libutil6.cc libutil.o		$(DEPS) $(INCS)
libutil7.o:		libutil7.cc libutil.o		$(DEPS) $(INCS)
libutil8.o:		libutil8.cc libutil.o		$(DEPS) $(INCS)
libutil9.o:		libutil9.cc libutil.o		$(DEPS) $(INCS)
libutil10.o:		libutil10.cc libutil.o		$(DEPS) $(INCS)
libutil11.o:		libutil11.cc libutil.o		$(DEPS) $(INCS)

libutil-cvtconst.o:	libutil-cvtconst.ccm		$(DEPS)	$(INCS)
libutil-getlen.o:	libutil-getlen.ccm		$(DEPS)	$(INCS)
libutil-lenstr.o:	libutil-lenstr.ccm		$(DEPS)	$(INCS)
libutil-lenstrarr.o:	libutil-lenstrarr.ccm		$(DEPS)	$(INCS)
libutil-loadstrs.o:	libutil-loadstrs.ccm		$(DEPS) $(INCS)
libutil-memclear.o:	libutil-memclear.ccm		$(DEPS)	$(INCS)
libutil-memload.o:	libutil-memload.ccm		$(DEPS)	$(INCS)
libutil-memcopy.o:	libutil-memcopy.ccm		$(DEPS)	$(INCS)
libutil-memcpy.o:	libutil-memcpy.ccm		$(DEPS)	$(INCS)
libutil-resumelife.o:	libutil-resumelife.ccm		$(DEPS)	$(INCS)
libutil-strlen.o:	libutil-strlen.ccm		$(DEPS)	$(INCS)
libutil-strxspn.o:	libutil-strxspn.ccm		$(DEPS)	$(INCS)
libutil-stpcpy.o:	libutil-stpcpy.ccm		$(DEPS)	$(INCS)
libutil-strcpy.o:	libutil-strcpy.ccm		$(DEPS)	$(INCS)
libutil-strcopy.o:	libutil-strcopy.ccm		$(DEPS)	$(INCS)
libutil-strcmp.o:	libutil-strcmp.ccm		$(DEPS)	$(INCS)
libutil-getbyte.o:	libutil-getbyte.ccm		$(DEPS)	$(INCS)
libutil-memncpy.o:	libutil-memncpy.ccm		$(DEPS)	$(INCS)
libutil-memnset.o:	libutil-memnset.ccm		$(DEPS)	$(INCS)
libutil-operators.o:	libutil-operators.ccm		$(DEPS)	$(INCS)

modcstring.o:		modcstring.dir
modcstring.dir:
	makesubdir $@



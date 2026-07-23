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

MODS += libutil.o

LIBS +=


DEPS += 

OBJPART00= libutil-strlen.o libutil-strxspn.o
OBJPART01= libutil-cvtconst.o libutil-getlen.o 
OBJPART02= libutil-lenstr.o libutil-lenstrarr.o
OBJPART03= libutil-loadstrs.o libutil-memclear.o
OBJPART04= libutil-memcopy.o libutil-memcpy.o
OBJPART05= libutil-resumelife.o libutil-stpcpy.o
OBJPART06= libutil-strcopy.o libutil-strcpy.o
OBJPART07= libutil-memload.o libutil-strcmp.o

OBJPART08= libutil-getbyte.o
OBJPART09= libutil-memnset.o
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
OBJ01=
OBJ02=
OBJ03=

OBJIMPL= obj00.o


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


libutil0.o:		libutil.ccm objpart.o
	gxx -c -x c++ -o $@ $(CPPFLAGS) $(CXXFLAGS) $<

libutil1.o:		libutil1.cc libutil0.o
	$(COMPILE.cc) $<

libutil2.o:		libutil2.cc libutil0.o
	$(COMPILE.cc) $<

libutil3.o:		libutil3.cc libutil0.o
	$(COMPILE.cc) $<

libutil4.o:		libutil4.cc libutil0.o
	$(COMPILE.cc) $<

libutil5.o:		libutil5.cc libutil0.o
	$(COMPILE.cc) $<

libutil6.o:		libutil6.cc libutil.ccm
	$(COMPILE.cc) $<

libutil7.o:		libutil7.cc libutil.ccm
	$(COMPILE.cc) $<

libutil8.o:		libutil8.cc libutil.ccm
	$(COMPILE.cc) $<

libutil9.o:		libutil9.cc libutil.ccm
	$(COMPILE.cc) $<

libutil10.o:		libutil10.cc libutil.ccm
	$(COMPILE.cc) $<

libutil11.o:		libutil11.cc libutil.ccm
	$(COMPILE.cc) $<

libutil-cvtconst.o:	libutil-cvtconst.ccm			$(INCS)
	gxx -c -x c++ -o $@ $(CPPFLAGS) $(CXXFLAGS) $<

libutil-getlen.o:	libutil-getlen.ccm			$(INCS)
	gxx -c -x c++ -o $@ $(CPPFLAGS) $(CXXFLAGS) $<

libutil-lenstr.o:	libutil-lenstr.ccm			$(INCS)
	gxx -c -x c++ -o $@ $(CPPFLAGS) $(CXXFLAGS) $<

libutil-lenstrarr.o:	libutil-lenstrarr.ccm			$(INCS)
	gxx -c -x c++ -o $@ $(CPPFLAGS) $(CXXFLAGS) $<

libutil-loadstrs.o:	libutil-loadstrs.ccm			$(INCS)
	gxx -c -x c++ -o $@ $(CPPFLAGS) $(CXXFLAGS) $<

libutil-memclear.o:	libutil-memclear.ccm			$(INCS)
	gxx -c -x c++ -o $@ $(CPPFLAGS) $(CXXFLAGS) $<

libutil-memload.o:	libutil-memload.ccm			$(INCS)
	gxx -c -x c++ -o $@ $(CPPFLAGS) $(CXXFLAGS) $<

libutil-memcopy.o:	libutil-memcopy.ccm			$(INCS)
	gxx -c -x c++ -o $@ $(CPPFLAGS) $(CXXFLAGS) $<

libutil-memcpy.o:	libutil-memcpy.ccm			$(INCS)
	gxx -c -x c++ -o $@ $(CPPFLAGS) $(CXXFLAGS) $<

libutil-resumelife.o:	libutil-resumelife.ccm			$(INCS)
	gxx -c -x c++ -o $@ $(CPPFLAGS) $(CXXFLAGS) $<

libutil-strlen.o:	libutil-strlen.ccm			$(INCS)
	gxx -c -x c++ -o $@ $(CPPFLAGS) $(CXXFLAGS) $<

libutil-strxspn.o:	libutil-strxspn.ccm			$(INCS)
	gxx -c -x c++ -o $@ $(CPPFLAGS) $(CXXFLAGS) $<

libutil-stpcpy.o:	libutil-stpcpy.ccm			$(INCS)
	gxx -c -x c++ -o $@ $(CPPFLAGS) $(CXXFLAGS) $<

libutil-strcpy.o:	libutil-strcpy.ccm			$(INCS)
	gxx -c -x c++ -o $@ $(CPPFLAGS) $(CXXFLAGS) $<

libutil-strcopy.o:	libutil-strcopy.ccm			$(INCS)
	gxx -c -x c++ -o $@ $(CPPFLAGS) $(CXXFLAGS) $<

libutil-strcmp.o:	libutil-strcmp.ccm			$(INCS)
	gxx -c -x c++ -o $@ $(CPPFLAGS) $(CXXFLAGS) $<

libutil-getbyte.o:	libutil-getbyte.ccm			$(INCS)
	gxx -c -x c++ -o $@ $(CPPFLAGS) $(CXXFLAGS) $<

libutil-memnset.o:	libutil-memnset.ccm			$(INCS)
	gxx -c -x c++ -o $@ $(CPPFLAGS) $(CXXFLAGS) $<

libutil-operators.o:	libutil-operators.ccm			$(INCS)
	gxx -c -x c++ -o $@ $(CPPFLAGS) $(CXXFLAGS) $<



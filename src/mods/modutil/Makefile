# MAKEFILE (modutil)

T= modutil

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

INCS += modutil.hh

MODS +=

LIBS +=


OBJPART00= modutil-lenstr.o modutil-memclear.o
OBJPART01= modutil-memcopy.o modutil-memload.o
OBJPART02= modutil-memnset.o modutil-resumelife.o
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

OBJPRIME= modutil0.o

OBJIMPL= modutil1.o


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

$(T).o:			objpart.o objprime.o objimpl.o
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


objpart.o:		$(OBJPART)
	$(LD) -r $(LDFLAGS) -o $@ $^

objprime.o:		$(OBJPRIME)
	$(LD) -r $(LDFLAGS) -o $@ $^

objimpl.o:		$(OBJIMPL)
	$(LD) -r $(LDFLAGS) -o $@ $^

# primary
modutil0.o:		modutil.ccm $(OBJPARTS)	modutil.hh	$(INCS)
	gxx -c -x c++ -o $@ $(CPPFLAGS) $(CXXFLAGS) $<

# primary partitions
modutil-lenstr.o:	modutil-lenstr.ccm		modutil.hh $(INCS)
modutil-memclear.o:	modutil-memclear.ccm		modutil.hh $(INCS)
modutil-memcopy.o:	modutil-memcopy.ccm		modutil.hh $(INCS)
modutil-memload.o:	modutil-memload.ccm		modutil.hh $(INCS)
modutil-memnset.o:	modutil-memnset.ccm		modutil.hh $(INCS)
modutil-resumelife.o:	modutil-resumelife.ccm		modutil.hh $(INCS)

# implementaiton
modutil1.o:		modutil1.cc modutil0.o			#(INCS)
	$(COMPILE.cc) $<



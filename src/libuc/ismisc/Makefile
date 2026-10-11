# MAKEFILE (ismisc)

T= ismisc

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


DEFS=

INCS= ismisc.h

MODS=

LIBS=


OBJ0_ISMISC= isdotdir.o isaccmode.o
OBJ1_ISMISC= isleapyear.o
OBJ2_ISMISC= isprime.o isinteractive.o
OBJ3_ISMISC= isobjspecial.o 

OBJ4_ISMISC= isindomain.o isinetaddr.o
OBJ5_ISMISC= issamehostname.o
OBJ6_ISMISC=
OBJ7_ISMISC=

OBJA_ISMISC= obj0_ismisc.o obj1_ismisc.o
OBJB_ISMISC= obj2_ismisc.o obj3_ismisc.o
OBJC_ISMISC= obj4_ismisc.o obj5_ismisc.o
OBJD_ISMISC=

OBJ_ISMISC= $(OBJA_ISMISC) $(OBJB_ISMISC) $(OBJC_ISMISC)


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


$(T).o:			$(OBJ_ISMISC)
	$(LD) -r $(LDFLAGS) -o $@ $^

$(T).nm:		$(T).o
	$(NM) $(NMFLAGS) $(T).o > $(T).nm

again:
	rm -f $(ALL)

clean:
	makeclean $(ALL)

control:
	(uname -n ; date) > Control


obj0_ismisc.o:		$(OBJ0_ISMISC)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj1_ismisc.o:		$(OBJ1_ISMISC)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj2_ismisc.o:		$(OBJ2_ISMISC)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj3_ismisc.o:		$(OBJ3_ISMISC)
	$(LD) -r $(LDFLAGS) -o $@ $^


obj4_ismisc.o:		$(OBJ4_ISMISC)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj5_ismisc.o:		$(OBJ5_ISMISC)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj6_ismisc.o:		$(OBJ6_ISMISC)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj7_ismisc.o:		$(OBJ7_ISMISC)
	$(LD) -r $(LDFLAGS) -o $@ $^


obja_ismisc.o:		$(OBJA_ISMISC)
	$(LD) -r $(LDFLAGS) -o $@ $^

objb_ismisc.o:		$(OBJB_ISMISC)
	$(LD) -r $(LDFLAGS) -o $@ $^

objc_ismisc.o:		$(OBJC_ISMISC)
	$(LD) -r $(LDFLAGS) -o $@ $^

objd_ismisc.o:		$(OBJD_ISMISC)
	$(LD) -r $(LDFLAGS) -o $@ $^


isdotdir.o:		isdotdir.cc					$(INCS)
isleapyear.o:		isleapyear.cc					$(INCS)
isobjspecial.o:		isobjspecial.cc					$(INCS)
isprime.o:		isprime.cc					$(INCS)
isaccmode.o:		isaccmode.cc		isaccmode.h		$(INCS)
isinteractive.o:	isinteractive.cc	isinteractive.hh	$(INCS)

isindomain.o:		isindomain.cc		isindomain.h		$(INCS)
isinetaddr.o:		isinetaddr.cc		isinetaddr.h		$(INCS)
issamehostname.o:	issamehostname.cc				$(INCS)



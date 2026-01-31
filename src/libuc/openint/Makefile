# MAKEFILES (openint)

T= openint

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

INCS += openint.h

MODS +=

LIBS +=


OBJ0=
OBJ1=
OBJ2=
OBJ3=
OBJ4=
OBJ5=
OBJ6=
OBJ7= 

OBJA= obj0.o obj1.o obj2.o obj3.o 
OBJB= obj4.o obj5.o obj6.o obj7.o

OBJ= obja.o objb.o


INCDIRS +=

LIBDIRS += -L$(LIBDIR)


RUNINFO= -rpath $(RUNDIR)
LIBINFO= $(LIBDIRS) $(LIBS)

# flag setting
CPPFLAGS	?= $(DEFS) $(INCDIRS) $(MAKECPPFLAGS)
CFLAGS		?= $(MAKECFLAGS)
CXXFLAGS	?= $(MAKECXXFLAGS)
ARFLAGS		?= $(MAKEARFLAGS)
LDFLAGS		?= $(MAKELDFLAGS)


.SUFFIXES:		.hh .ii .iim .cccm


default:		$(T).o

all:			$(ALL)


.cc.i:
	$(CPP) $(CPPFLAGS) $< > $(*).i

.ccc.ii:
	$(CPP) $(CPPFLAGS) $< > $(*).ii

.cccm.iim:
	$(CPP) $(CPPFLAGS) $< > $(*).iim

.cc.s:
	$(CC) -S $(CPPFLAGS) $(CFLAGS) $<

.ccc.s:
	$(CXX) -S $(CPPFLAGS) $(CXXFLAGS) $<

.cc.o:
	$(COMPILE.cc) $<

.ccc.o:
	$(COMPILE.ccc) $<

.cccm.o:
	makemodule $(*)


$(T).o:			$(OBJ)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ)

$(T).nm:		$(T).o
	$(NM) $(NMFLAGS) $(T).o > $(T).nm

again:
	$(RM) $(ALL)

clean:
	makeclean $(ALL)

control:
	(uname -n ; date) > Control


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


openint_hello.o:	openint_hello.cc	openint_hello.h		$(INCS)
openint_copyout.o:	openint_copyout.cc	openint_copyout.h	$(INCS)
openint_daytime.o:	openint_daytime.cc	openint_daytime.h	$(INCS)
openint_loginblueb.o:	openint_loginblurb.cc	openint_loginblurb.h	$(INCS)
openint_fshome.o:	openint_fshome.cc	openint_fshome.h	$(INCS)
openint_org.o:		openint_org.cc		openint_org.h		$(INCS)
openint_issue.o:	openint_issue.cc	openint_issue.h		$(INCS)
openint_motd.o:		openint_motd.cc		openint_motd.h		$(INCS)
openint_imail.o:	openint_imail.cc	openint_imail.h		$(INCS)
openint_termnote.o:	openint_termnote.cc	openint_termnote.h	$(INCS)
openint_statmsg.o:	openint_statmsg.cc	openint_statmsg.h	$(INCS)



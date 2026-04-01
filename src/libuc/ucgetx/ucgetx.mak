# MAKEFILE (ucgetx)

T= ucgetx

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

INCS += ucgetx.h

MODS=

LIBS +=


OBJ0= ucgetarchitecture.o ucgetcwd.o ucgethost.o
OBJ1= ucgethostname.o ucgetipnodeby.o ucgetloadavg.o
OBJ2= ucgetlogin.o ucgetnameinfo.o ucgetnisdomain.o
OBJ3= ucgetpid.o ucgetpuid.o ucgetrandom.o
OBJ4= ucgetrusage.o ucgetsocktype.o ucgetnetname.o
OBJ5= ucgetauid.o ucgetdefproj.o ucgetpriority.o
OBJ6=
OBJ7=

OBJA= obj0.o obj1.o obj2.o 
OBJB= obj3.o obj4.o obj5.o

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


.SUFFIXES:		.hh .ii .iim .ccm


default:		$(T).o

all:			$(ALL)


.c.i:
	$(CPP) $(CPPFLAGS) $< > $(*).i

.cc.ii:
	$(CPP) $(CPPFLAGS) $< > $(*).ii

.c.s:
	$(CC) -S $(CPPFLAGS) $(CFLAGS) $<

.cc.s:
	$(CXX) -S $(CPPFLAGS) $(CXXFLAGS) $<

.c.o:
	$(COMPILE.c) $<

.cc.o:
	$(COMPILE.cc) $<

.ccm.o:
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


ucgetarchitecture.o:	ucgetarchitecture.cc			$(INCS)
ucgetauid.o:		ucgetauid.cc				$(INCS)
ucgetcwd.o:		ucgetcwd.cc				$(INCS)
ucgetdefproj.o:		ucgetdefproj.cc				$(INCS)
ucgethost.o:		ucgethost.cc		ucgethost.h	$(INCS)
ucgethostname.o:	ucgethostname.cc			$(INCS)
ucgetipnodeby.o:	ucgetipnodeby.cc			$(INCS)
ucgetloadavg.o:		ucgetloadavg.cc		ucgetloadavg.h	$(INCS)
ucgetlogin.o:		ucgetlogin.cc				$(INCS)
ucgetnameinfo.o:	ucgetnameinfo.cc			$(INCS)
ucgetnetname.o:		ucgetnetname.cc				$(INCS)
ucgetnisdomain.o:	ucgetnisdomain.cc			$(INCS)
ucgetpid.o:		ucgetpid.cc		ucgetpid.h	$(INCS)
ucgetpriority.o:	ucgetpriority.cc			$(INCS)
ucgetpuid.o:		ucgetpuid.cc				$(INCS)
ucgetrandom.o:		ucgetrandom.cc		ucgetrandom.h	$(INCS)
ucgetrusage.o:		ucgetrusage.cc				$(INCS)
ucgetsocktype.o:	ucgetsocktype.cc			$(INCS)



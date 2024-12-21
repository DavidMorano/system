# MAKEFILES (ucgetx)

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

LIBS +=


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


OBJ0= ucgetarchitecture.o ucgetcwd.o ucgethost.o
OBJ1= ucgethostname.o ucgetipnodeby.o ucgetloadavg.o
OBJ2= ucgetlogin.o ucgetnameinfo.o ucgetnisdomain.o
OBJ3= ucgetpid.o ucgetpuid.o ucgetrandom.o
OBJ4= ucgetrusage.o ucgetsocktype.o
OBJ5= 

OBJA= obj0.o obj1.o obj2.o 
OBJB= obj3.o obj4.o

OBJ= obja.o objb.o


.SUFFIXES:		.hh .ii


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


$(T).o:			$(OBJ)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ)

$(T).nm:		$(T).so
	$(NM) $(NMFLAGS) $(T).so > $(T).nm

$(T).order:		$(OBJ) $(T).a
	$(LORDER) $(T).a | $(TSORT) > $(T).order
	$(RM) $(T).a
	while read O ; do $(AR) $(ARFLAGS) -cr $(T).a $${O} ; done < $(T).order

again:
	$(RM) $(ALL)

clean:
	makeclean $(ALL)

control:
	(uname -n ; date) > Control


obj0.o:			$(OBJ0)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0)

obj1.o:			$(OBJ1)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1)

obj2.o:			$(OBJ2)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2)

obj3.o:			$(OBJ3)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ3)

obj4.o:			$(OBJ4)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ4)

obj5.o:			$(OBJ5)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ5)


obja.o:			$(OBJA)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJA)

objb.o:			$(OBJB)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJB)


ucgetarchitecture.o:	ucgetarchitecture.cc			$(INCS)
ucgetcwd.o:		ucgetcwd.cc				$(INCS)
ucgethost.o:		ucgethost.cc ucgethost.h		$(INCS)
ucgethostname.o:	ucgethostname.cc			$(INCS)
ucgetipnodeby.o:	ucgetipnodeby.cc			$(INCS)
ucgetloadavg.o:		ucgetloadavg.cc ucgetloadavg.h		$(INCS)
ucgetlogin.o:		ucgetlogin.cc				$(INCS)
ucgetnameinfo.o:	ucgetnameinfo.cc			$(INCS)
ucgetnisdomain.o:	ucgetnisdomain.cc			$(INCS)
ucgetpid.o:		ucgetpid.cc ucgetpid.h			$(INCS)
ucgetpuid.o:		ucgetpuid.cc				$(INCS)
ucgetrandom.o:		ucgetrandom.cc ucgetrandom.h		$(INCS)
ucgetrusage.o:		ucgetrusage.cc				$(INCS)
ucgetsocktype.o:	ucgetsocktype.cc			$(INCS)



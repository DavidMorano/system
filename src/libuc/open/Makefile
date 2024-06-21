# MAKEFILES (open)

T= open

ALL= $(T).o $(T).a


BINDIR= $(REPOROOT)/bin
INCDIR= $(REPOROOT)/include
LIBDIR= $(REPOROOT)/lib
MANDIR= $(REPOROOT)/man

INFODIR= $(REPOROOT)/info
HELPDIR= $(REPOROOT)/share/help

CRTDIR= $(CGS_CRTDIR)
VALDIR= $(CGS_VALDIR)
RUNDIR= $(USRLOCAL)/lib


CPP= cpp
CC= gcc
CXX= gpp
LD= gld
RANLIB= granlib
AR= gar
NM= gnm
COV= gcov

LORDER= lorder
TSORT= tsort
LINT= lint
RM= rm -f
TOUCH= touch
LINT= lint


DEFS +=

INCS += open.h

LIBS +=


INCDIRS=

LIBDIRS= -L$(LIBDIR)


LIBINFO= $(LIBDIRS) $(LIBS)

# flag setting
CPPFLAGS= $(DEFS) $(INCDIRS) $(MAKECPPFLAGS)
CFLAGS= $(MAKECFLAGS)
CCFLAGS= $(MAKECCFLAGS)
ARFLAGS= $(MAKEARFLAGS)
LDFLAGS= $(MAKELDFLAGS)


OBJ0_OPEN= openaddrinfo.o opensockaddr.o openshm.o
OBJ1_OPEN= opendial.o openport.o openportmsg.o
OBJ2_OPEN= opendefstds.o opensysfs.o opentmp.o
OBJ3_OPEN= openusd.o
OBJ4_OPEN= openqotd.o opensys_banner.o


OBJA_OPEN= obj0_open.o obj1_open.o
OBJB_OPEN= obj2_open.o obj3_open.o obj4_open.o

OBJ_OPEN= $(OBJA_OPEN) $(OBJB_OPEN)


default:		$(T).o

all:			$(ALL)

.c.ln:
	$(LINT) -c $(LINTFLAGS) $(CPPFLAGS) $<

.c.ls:
	$(LINT) $(LINTFLAGS) $(CPPFLAGS) $<

.c.i:
	$(CPP) $(CPPFLAGS) $< > $(*).i

.c.o:
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $<

.cc.o:
	$(CXX)  $(CPPFLAGS) $(CCFLAGS) -c $<


$(T).o:			$(OBJ_OPEN)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ_OPEN)

$(T).a:			$(OBJ_OPEN)
	$(AR) $(ARFLAGS) -rc $@ $?

$(T).nm:		$(T).so
	$(NM) $(NMFLAGS) $(T).so > $(T).nm

$(T).order:		$(OBJ) $(T).a
	$(LORDER) $(T).a | $(TSORT) > $(T).order
	$(RM) $(T).a
	while read O ; do $(AR) $(ARFLAGS) -cr $(T).a $${O} ; done < $(T).order

again:
	rm -f $(ALL)

clean:
	makeclean $(ALL)

control:
	(uname -n ; date) > Control

obj0_open.o:	$(OBJ0_OPEN)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0_OPEN)

obj1_open.o:	$(OBJ1_OPEN)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1_OPEN)

obj2_open.o:	$(OBJ2_OPEN)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2_OPEN)

obj3_open.o:	$(OBJ3_OPEN)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ3_OPEN)

obj4_open.o:	$(OBJ4_OPEN)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ4_OPEN)


# objects
openport.o:		openport.cc openport.h		$(INCS)
openportmsg.o:		openportmsg.cc openportmsg.h	$(INCS)

# subroutines
openaddrinfo.o:		openaddrinfo.cc openaddrinfo.h	$(INCS)
opensockaddr.o:		opensockaddr.cc opensockaddr.h	$(INCS)
opendefstds.o:		opendefstds.cc			$(INCS)
opendial.o:		opendial.cc opendial.h		$(INCS)
openqotd.o:		openqotd.cc openqotd.h		$(INCS)
openshm.o:		openshm.cc openshm.h		$(INCS)
opensysfs.o:		opensysfs.cc opensysfs.h	$(INCS)
opentmp.o:		opentmp.cc			$(INCS)
openusd.o:		openusd.cc			$(INCS)
opensys_banner.o:	opensys_banner.cc		$(INCS)



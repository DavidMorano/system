# MAKEFILES (libacl)

T= libacl

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

INCS += libacl.h

LIBS +=


INCDIRS=

LIBDIRS= -L$(LIBDIR)


RUNINFO= -rpath $(RUNDIR)

LIBINFO= $(LIBDIRS) $(LIBS)

# flag setting
CPPFLAGS	?= $(DEFS) $(INCDIRS) $(MAKECPPFLAGS)
CFLAGS		?= $(MAKECFLAGS)
CXXFLAGS	?= $(MAKECXXFLAGS)
ARFLAGS		?= $(MAKEARFLAGS)
LDFLAGS		?= $(MAKELDFLAGS)


OBJ0_LIBACL= aclinfo.o acltypes.o
OBJ1_LIBACL=
OBJ2_LIBACL=
OBJ3_LIBACL=


OBJA_LIBACL= obj0_libacl.o

OBJ_LIBACL= $(OBJA_LIBACL)


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


$(T).o:			$(OBJ_LIBACL)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ_LIBACL)

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


obj0_libacl.o:	$(OBJ0_LIBACL)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0_LIBACL)

obj1_libacl.o:	$(OBJ1_LIBACL)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1_LIBACL)

obj2_libacl.o:	$(OBJ2_LIBACL)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2_LIBACL)

obj3_libacl.o:	$(OBJ3_LIBACL)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ3_LIBACL)

obj4_libacl.o:	$(OBJ4_LIBACL)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ4_LIBACL)


aclinfo.o:		aclinfo.cc	$(INCS)
acltypes.o:		acltypes.cc	$(INCS)



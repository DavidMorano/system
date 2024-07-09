# MAKEFILES (filer)

T= filer

ALL= $(T).o $(T).a


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

INCS= filer.h

LIBS=


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


OBJ0_FILER= filer_main.o
OBJ1_FILER= filer_writers.o
OBJ2_FILER= filer_mailsup.o
OBJ3_FILER=

OBJA_FILER= obj0_filer.o obj1_filer.o
OBJB_FILER= obj2_filer.o

OBJ_FILER= $(OBJA_FILER) $(OBJB_FILER)


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


$(T).o:			$(OBJ_FILER)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ_FILER)

$(T).a:			$(OBJ_FILER)
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


obj0_filer.o:	$(OBJ0_FILER)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0_FILER)

obj1_filer.o:	$(OBJ1_FILER)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1_FILER)

obj2_filer.o:	$(OBJ2_FILER)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2_FILER)

obj3_filer.o:	$(OBJ3_FILER)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ3_FILER)


filer_main.o:		filer_main.cc		$(INCS)
filer_writers.o:	filer_writers.cc	$(INCS)
filer_mailsup.o:	filer_mailsup.cc	$(INCS)



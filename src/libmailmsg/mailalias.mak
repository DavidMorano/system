# MAKEFILES (mailalias)

T= mailalias

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

INCS += mailalias.h mailaliashdr.h

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


OBJ0_MAILALIAS= mailalias_obj.o
OBJ1_MAILALIAS= dbmake.o
OBJ2_MAILALIAS= 
OBJ3_MAILALIAS= 
OBJ4_MAILALIAS= 

OBJA_MAILALIAS= obj0.o obj1.o
OBJB_MAILALIAS= obj2.o obj3.o obj4.o

OBJ_MAILALIAS= obja.o


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


$(T).o:			$(OBJ_MAILALIAS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ_MAILALIAS)

$(T).a:			$(OBJ_MAILALIAS)
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


obj0.o:			$(OBJ0_MAILALIAS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0_MAILALIAS)

obj1.o:			$(OBJ1_MAILALIAS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1_MAILALIAS)

obj2.o:			$(OBJ2_MAILALIAS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2_MAILALIAS)

obj3.o:			$(OBJ3_MAILALIAS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ3_MAILALIAS)

obj4.o:			$(OBJ4_MAILALIAS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ4_MAILALIAS)


obja.o:			$(OBJA_MAILALIAS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJA_MAILALIAS)

objb.o:			$(OBJB_MAILALIAS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJB_MAILALIAS)


mailalias_obj.o:	mailalias_obj.cc	$(INCS) dbmake.hh

dbmake.o:		dbmake.cc dbmake.hh	$(INCS)



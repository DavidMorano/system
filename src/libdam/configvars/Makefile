# MAKEFILES (configvars)

T= configvars

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

INCS += configvars.h

LIBS +=


INCDIRS +=

LIBDIRS += -L$(LIBDIR)


RUNINFO= -tpath $(RUNDIR)

LIBINFO= $(LIBDIRS) $(LIBS)

# flag setting
CPPFLAGS	?= $(DEFS) $(INCDIRS) $(MAKECPPFLAGS)
CFLAGS		?= $(MAKECFLAGS)
CXXFLAGS	?= $(MAKECXXFLAGS)
ARFLAGS		?= $(MAKEARFLAGS)
LDFLAGS		?= $(MAKELDFLAGS)


OBJ0= configvars_main.o configvars_parse.o configvars_sub.o
OBJ1= configvars_file.o configvars_var.o
OBJ2= configvars_aux.o
OBJ3=
OBJ4=
OBJ5=
OBJ6=
OBJ7=

OBJA= obj0.o obj1.o obj2.o
#OBJB= obj4.o obj5.o obj6.o obj7.o

OBJ= $(OBJA) $(OBJB)


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

$(T).a:			$(OBJ)
	$(AR) $(ARFLAGS) -rc $@ $?

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

obj6.o:			$(OBJ6)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ6)

obj7.o:			$(OBJ7)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ7)


configvars_main.o:	configvars_main.cc	$(INCS)
configvars_parse.o:	configvars_parse.cc	$(INCS)
configvars_sub.o:	configvars_sub.cc	$(INCS)
configvars_file.o:	configvars_file.cc	$(INCS)
configvars_var.o:	configvars_var.cc	$(INCS)
configvars_aux.o:	configvars_aux.cc	$(INCS)



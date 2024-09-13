# MAKEFILES (field)

T= field

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

INCS= field.h fieldterms.h terminit.hh

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


OBJ0_FIELD= field_main.o field_obj.o
OBJ1_FIELD= field_srvarg.o field_svcargs.o
OBJ2_FIELD= field_word.o field_wordphrase.o
OBJ3_FIELD= fieldterms.o

OBJA_FIELD= obj0_field.o obj1_field.o
OBJB_FIELD= obj2_field.o obj3_field.o

OBJ_FIELD= $(OBJA_FIELD) $(OBJB_FIELD)


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


$(T).o:			$(OBJ_FIELD)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ_FIELD)

$(T).a:			$(OBJ_FIELD)
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


obj0_field.o:	$(OBJ0_FIELD)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0_FIELD)

obj1_field.o:	$(OBJ1_FIELD)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1_FIELD)

obj2_field.o:	$(OBJ2_FIELD)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2_FIELD)

obj3_field.o:	$(OBJ3_FIELD)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ3_FIELD)


field_main.o:		field_main.cc		$(INCS)
field_obj.o:		field_obj.cc		$(INCS)
field_srvarg.o:		field_srvarg.cc		$(INCS)
field_svcargs.o:	field_svcargs.cc	$(INCS)
field_word.o:		field_word.cc		$(INCS)
field_wordphrase.o:	field_wordphrase.cc	$(INCS)

fieldterms.o:		fieldterms.cc fieldterms.h



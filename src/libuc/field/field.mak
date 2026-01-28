# MAKEFILES (field)

T= field

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

INCS += field.h fieldterms.h fieldterminit.hh

MODS += 

LIBS +=


OBJ0_FIELD= field_main.o field_obj.o
OBJ1_FIELD= field_srvarg.o field_svcargs.o
OBJ2_FIELD= field_word.o field_wordphrase.o
OBJ3_FIELD= fieldterms.o

OBJA_FIELD= obj0_field.o obj1_field.o
OBJB_FIELD= obj2_field.o obj3_field.o

OBJ_FIELD= $(OBJA_FIELD) $(OBJB_FIELD)


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
	makemodule $(*)


$(T).o:			$(OBJ_FIELD)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ_FIELD)

$(T).nm:		$(T).o
	$(NM) $(NMFLAGS) $(T).o > $(T).nm

again:
	rm -f $(ALL)

clean:
	makeclean $(ALL)

control:
	(uname -n ; date) > Control


obj0_field.o:	$(OBJ0_FIELD)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj1_field.o:	$(OBJ1_FIELD)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj2_field.o:	$(OBJ2_FIELD)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj3_field.o:	$(OBJ3_FIELD)
	$(LD) -r $(LDFLAGS) -o $@ $^


obja.o:		$(OBJA)
	$(LD) -r $(LDFLAGS) -o $@ $^

objb.o:		$(OBJB)
	$(LD) -r $(LDFLAGS) -o $@ $^


field_main.o:		field_main.cc		$(INCS)
field_obj.o:		field_obj.cc		$(INCS)
field_srvarg.o:		field_srvarg.cc		$(INCS)
field_svcargs.o:	field_svcargs.cc	$(INCS)
field_word.o:		field_word.cc		$(INCS)
field_wordphrase.o:	field_wordphrase.cc	$(INCS)

fieldterms.o:		fieldterms.cc fieldterms.h



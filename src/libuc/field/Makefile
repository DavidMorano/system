# MAKEFILES (field)

T= field

ALL= $(T).o $(T).a


BINDIR= $(REPOROOT)/bin
INCDIR= $(REPOROOT)/include
LIBDIR= $(REPOROOT)/lib
MANDIR= $(REPOROOT)/man
INFODIR= $(REPOROOT)/info
HELPDIR= $(REPOROOT)/share/help


CRTDIR= $(CGS_CRTDIR)
VALDIR= $(CGS_VALDIR)
LIBDIR= $(CGS_LIBDIR)

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


DEFS=

LDRPATH= $(EXTRA)/lib

LIBDIRS= -L$(LIBDIR)

LIBS=


# flag setting
CPPFLAGS= $(DEFS) $(INCDIRS) $(MAKECPPFLAGS)
CFLAGS= $(MAKECFLAGS)
CCFLAGS= $(MAKECCFLAGS)
ARFLAGS= $(MAKEARFLAGS)
LDFLAGS= $(MAKELDFLAGS)


INCS= field.h


OBJ0_FIELD= field_main.o
OBJ1_FIELD= field_srvarg.o field_svcargs.o
OBJ2_FIELD= field_word.o 
OBJ3_FIELD= field_wordphrase.o


OBJA_FIELD= obj0_six.o obj1_six.o
OBJB_FIELD= obj2_six.o obj3_six.o

OBJ_FIELD= $(OBJA_FIELD) $(OBJB_FIELD)


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

obj0_six.o:	$(OBJ0_FIELD)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0_FIELD)

obj1_six.o:	$(OBJ1_FIELD)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1_FIELD)

obj2_six.o:	$(OBJ2_FIELD)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2_FIELD)

obj3_six.o:	$(OBJ3_FIELD)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ3_FIELD)


field_main.o:		field_main.cc		$(INCS)
field_srvarg.o:		field_srvarg.cc		$(INCS)
field_svcargs.o:	field_svcargs.cc	$(INCS)
field_word.o:		field_word.cc		$(INCS)
field_wordphrase.o:	field_wordphrase.cc	$(INCS)



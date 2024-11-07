# MAKEFILES (matxstr)

T= matxstr

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

INCS= matxstr.h

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


OBJ0_MATXSTR= matstr.o matostr.o matpstr.o
OBJ1_MATXSTR= matkeystr.o matsubstr.o 
OBJ2_MATXSTR= matnstr.o 
OBJ3_MATXSTR= matlocalfs.o

OBJA_MATXSTR= obj0.o obj1.o
OBJB_MATXSTR= obj2.o obj3.o

OBJ_MATXSTR= obja.o objb.o


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


$(T).o:			$(OBJ_MATXSTR)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ_MATXSTR)

$(T).a:			$(OBJ_MATXSTR)
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


obj0.o:			$(OBJ0_MATXSTR)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0_MATXSTR)

obj1.o:			$(OBJ1_MATXSTR)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1_MATXSTR)

obj2.o:			$(OBJ2_MATXSTR)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2_MATXSTR)

obj3.o:			$(OBJ3_MATXSTR)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ3_MATXSTR)


obja.o:			$(OBJA_MATXSTR)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJA_MATXSTR)

objb.o:			$(OBJB_MATXSTR)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJB_MATXSTR)


matstr.o:		matstr.cc matstr.h		$(INCS)
matostr.o:		matostr.cc matostr.h		$(INCS)
matpstr.o:		matpstr.cc matpstr.h		$(INCS)
matkeystr.o:		matkeystr.cc matkeystr.h	$(INCS)
matsubstr.o:		matsubstr.cc			$(INCS)
matnstr.o:		matnstr.cc			$(INCS)
matlocalfs.o:		matlocalfs.cc			$(INCS)



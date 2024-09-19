# MAKEFILES (strlistx)

T= strlistx

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


DEFS=

INCS= strlist.h strlisthdr.h

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


OBJ0_STRLIST= strlist.o
OBJ1_STRLIST= strlisthdr.o
OBJ2_STRLIST= strlistmk.o
OBJ3_STRLIST= strlistmks.o rectab.o

OBJA_STRLIST= obj0.o obj1.o
OBJB_STRLIST= obj2.o obj3.o

OBJ_STRLIST= obja.o objb.o


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


$(T).o:			$(OBJ_STRLIST)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ_STRLIST)

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


obj0.o:			$(OBJ0_STRLIST)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0_STRLIST)

obj1.o:			$(OBJ1_STRLIST)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1_STRLIST)

obj2.o:			$(OBJ2_STRLIST)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2_STRLIST)

obj3.o:			$(OBJ3_STRLIST)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ3_STRLIST)


obja.o:			$(OBJA_STRLIST)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJA_STRLIST)

objb.o:			$(OBJB_STRLIST)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJB_STRLIST)


strlist.o:		strlist.cc				$(INCS)
strlisthdr.o:		strlisthdr.cc				$(INCS)
strlistmk.o:		strlistmk.cc strlistmk.h 		$(INCS)
strlistmks.o:		strlistmks.cc strlistmks.h rectab.h	$(INCS)

rectab.o:		rectab.cc rectab.h			$(INCS)



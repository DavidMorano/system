# MAKEFILES (cluster)

T= cluster

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

INCS += cluster.h dialcprog.h

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


OBJ0_DIAL= dialcprog.o dialcprogmsg.o
OBJ1_DIAL=
OBJ2_DIAL=
OBJ3_DIAL=
OBJ4_DIAL=
OBJ5_DIAL=
OBJ6_DIAL=
OBJ7_DIAL=

OBJA_DIAL= obj0_dial.o obj1_dial.o
OBJB_DIAL= obj2_dial.o obj3_dial.o
OBJC_DIAL= obj4_dial.o obj5_dial.o
OBJD_DIAL= obj6_dial.o obj7_dial.o

OBJ_DIAL= obja_dial.o objb_dial.o objc_dial.o objd_dial.o


.SUFFIXES:		.hh .ii .ccm


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

.ccm.o:
	makemodule $(*)


$(T).o:			$(OBJ_DIAL)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ_DIAL)

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


obj0_dial.o:	$(OBJ0_DIAL)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0_DIAL)

obj1_dial.o:	$(OBJ1_DIAL)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1_DIAL)

obj2_dial.o:	$(OBJ2_DIAL)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2_DIAL)

obj3_dial.o:	$(OBJ3_DIAL)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ3_DIAL)

obj4_dial.o:	$(OBJ4_DIAL)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ4_DIAL)

obj5_dial.o:	$(OBJ5_DIAL)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ5_DIAL)

obj6_dial.o:	$(OBJ6_DIAL)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ6_DIAL)

obj7_dial.o:	$(OBJ7_DIAL)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ7_DIAL)

obj8_dial.o:	$(OBJ8_DIAL)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ8_DIAL)


obja_dial.o:	$(OBJA_DIAL)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJA_DIAL)

objb_dial.o:	$(OBJB_DIAL)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJB_DIAL)

objc_dial.o:	$(OBJC_DIAL)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJC_DIAL)

objd_dial.o:	$(OBJD_DIAL)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJD_DIAL)

obje_dial.o:	$(OBJE_DIAL)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJE_DIAL)


dialcprog.o:		dialcprog.cc dialcprog.h	$(INCS)
dialcprog.o:		msflag.h			$(INCS)

dialcprogmsg.o:		dialcprogmsg.cc dialcprogmsg.h	$(INCS)



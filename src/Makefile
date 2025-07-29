# MAKEFILE (src)

T= src

ALL= $(T).a


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

INCS +=

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


OBJ0_SRC= libu.o libuc.o
OBJ1_SRC= libdam.o libpcs.o
OBJ2_SRC= libmailmsg.o
OBJ3_SRC= libpr.o

OBJA_SRC= obj0_src.o obj1_src.o
OBJB_SRC= obj2_src.o obj3_src.o

OBJ_SRC= $(OBJA_SRC) $(OBJB_SRC)


.SUFFIXES:		.hh .ii .iim .ccm


default:		$(T).a

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


$(T).a:			$(OBJ_SRC)
	$(AR) $(ARFLAGS) -rc $@ $?

$(T).nm:		$(T).o
	$(NM) $(NMFLAGS) $(T).o > $(T).nm

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


obj0_src.o:		$(OBJ0_SRC)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0_SRC)

obj1_src.o:		$(OBJ1_SRC)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1_SRC)

obj2_src.o:		$(OBJ2_SRC)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2_SRC)

obj3_src.o:		$(OBJ3_SRC)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ3_SRC)


# LIBU
libu.o:			libu.dir
libu.dir:
	makesubdir $@

# LIBUC
libuc.o:		libuc.dir
libuc.dir:
	makesubdir $@

# LIBDAM
libdam.o:		libdam.dir
libdam.dir:
	makesubdir $@

# LIBMAILMSG
libmailmsg.o:		libmailmsg.dir
libmailmsg.dir:
	makesubdir $@

# LIBPCS
libpcs.o:		libpcs.dir
libpcs.dir:
	makesubdir $@

# LIBPR
libpr.o:		libpr.dir
libpr.dir:
	makesubdir $@

# LIBPROG
libprog.o:		libprog.dir
libprog.dir:
	makesubdir $@



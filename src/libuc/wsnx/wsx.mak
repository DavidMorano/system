# MAKEFILES (wsx)

T= wsx

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

INCS= wsx.h

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


OBJ0_WSNX=  wsfnext.o wswcpynarrow.o
OBJ1_WSNX= 
OBJ2_WSNX= 
OBJ3_WSNX= 
OBJ4_WSNX= 
OBJ5_WSNX= 

OBJA_WSNX= obj0_sfx.o
OBJB_WSNX= obj2_sfx.o obj3_sfx.o
OBJC_WSNX= obj4_sfx.o obj5_sfx.o

OBJ_WSNX= obja.o


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


$(T).o:			$(OBJ_WSNX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ_WSNX)

$(T).a:			$(OBJ_WSNX)
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


obj0_sfx.o:		$(OBJ0_WSNX) $(INCS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0_WSNX)

obj1_sfx.o:		$(OBJ1_WSNX) $(INCS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1_WSNX)

obj2_sfx.o:		$(OBJ2_WSNX) $(INCS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2_WSNX)

obj3_sfx.o:		$(OBJ3_WSNX) $(INCS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ3_WSNX)

obj4_sfx.o:		$(OBJ4_WSNX) $(INCS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ4_WSNX)

obj5_sfx.o:		$(OBJ5_WSNX) $(INCS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ5_WSNX)


obja.o:			$(OBJA_WSNX) $(INCS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJA_WSNX)

objb.o:			$(OBJB_WSNX) $(INCS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJB_WSNX)

objc.o:			$(OBJC_WSNX) $(INCS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJC_WSNX)


wsfnext.o:		wsfnext.cc		$(INCS)
wswcpynarrow.o:		wswcpynarrow.cc		$(INCS)



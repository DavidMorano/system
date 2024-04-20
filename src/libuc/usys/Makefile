# MAKEFILE (usys)

T= usys

ALL= $(T).o


BINDIR= $(REPOROOT)/bin
INCDIR= $(REPOROOT)/include
LIBDIR= $(REPOROOT)/lib
MANDIR= $(REPOROOT)/man

INFODIR= $(REPOROOT)/info
HELPDIR= $(REPOROOT)/share/help
LDRPATH= $(REPOROOT)/lib

CRTDIR= $(CGS_CRTDIR)
VALDIR= $(CGS_VALDIR)


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


DEFS +=


INCS= usys.h


LIBS=


LIBDIRS= -L$(LIBDIR)

# flag setting
CPPFLAGS= $(DEFS) $(INCDIRS) $(MAKECPPFLAGS)
CFLAGS= $(MAKECFLAGS)
CCFLAGS= $(MAKECCFLAGS)
ARFLAGS= $(MAKEARFLAGS)
LDFLAGS= $(MAKELDFLAGS)


OBJ0_USYS= usys_xxx.o
OBJ1_USYS= usys_sunos.o usys_darwin.o usys_linux.o 
OBJ2_USYS= usys_mqueue.o usys_gethrtime.o usys_getrandom.o
OBJ3_USYS= usys_ttynamerp.o usys_ugetnisdom.o

OBJ_USYS= obj0_usys.o obj1_usys.o obj2_usys.o obj3_usys.o


default:		$(T).o

all:			$(ALL)

.c.ln:
	$(LINT) -c $(LINTFLAGS) $(CPPFLAGS) $<

.c.ls:
	$(LINT) $(LINTFLAGS) $(CPPFLAGS) $<

.c.i:
	$(CPP) $(CPPFLAGS) $< > $(*).i

.c.o:
	$(CC) $(CPPFLAGS) -c $(CFLAGS) $<

.cc.o:
	$(CXX) $(CPPFLAGS) -c $(CCFLAGS) $<


$(T).o:			$(OBJ_USYS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ_USYS)

$(T).a:			$(OBJ_USYS)
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

obj0_usys.o:	$(OBJ0_USYS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0_USYS)

obj1_usys.o:	$(OBJ1_USYS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1_USYS)

obj2_usys.o:	$(OBJ2_USYS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2_USYS)

obj3_usys.o:	$(OBJ3_USYS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ3_USYS)


usys_xxx.o:		usys_xxx.cc usys_xxx.h			$(INCS)
usys_sunos.o:		usys_sunos.cc usys_sunos.h		$(INCS)
usys_darwin.o:		usys_darwin.cc usys_darwin.h		$(INCS)
usys_linux.o:		usys_linux.cc usys_linux.h		$(INCS)

usys_mqueue.o:		usys_mqueue.cc usys_mqueue.h		$(INCS)
usys_gethrtime.o:	usys_gethrtime.cc usys_gethrtime.h	$(INCS)
usys_getrandom.o:	usys_getrandom.cc usys_getrandom.h	$(INCS)
usys_ttynamerp.o:	usys_ttynamerp.cc usys_ttynamerp.h	$(INCS)
usys_ugetnisdom.o:	usys_ugetnisdom.cc usys_ugetnisdom.h	$(INCS)



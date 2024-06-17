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

LDRPATH= $(USRLOCAL)/lib

# flag setting
CPPFLAGS= $(DEFS) $(INCDIRS) $(MAKECPPFLAGS)
CFLAGS= $(MAKECFLAGS)
CCFLAGS= $(MAKECCFLAGS)
ARFLAGS= $(MAKEARFLAGS)
LDFLAGS= $(MAKELDFLAGS)


OBJ0_USYS= usys_xxx.o
OBJ1_USYS= usys_sunos.o usys_darwin.o usys_linux.o 
OBJ2_USYS= usys_ufcntl.o usys_ugetdents.o usys_ugetnisdom.o
OBJ3_USYS= usys_libstr.o

OBJ4_USYS= usys_mqueue.o usys_gethrtime.o usys_getrandom.o
OBJ5_USYS= usys_ttynamerp.o 
OBJ6_USYS= usys_stime.o usys_resolvepath.o usys_waitid.o
OBJ7_USYS= usys_sigx.o usys_streams.o

OBJ8_USYS= usys_pipes.o

OBJA= obj0_usys.o obj1_usys.o obj2_usys.o 
OBJB= obj3_usys.o obj4_usys.o obj5_usys.o
OBJC= obj6_usys.o obj7_usys.o obj8_usys.o

OBJ_USYS= obja.o objb.o objc.o


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

obj4_usys.o:	$(OBJ4_USYS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ4_USYS)

obj5_usys.o:	$(OBJ5_USYS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ5_USYS)

obj6_usys.o:	$(OBJ6_USYS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ6_USYS)

obj7_usys.o:	$(OBJ7_USYS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ7_USYS)

obj8_usys.o:	$(OBJ8_USYS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ8_USYS)


obja.o:		$(OBJA)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJA)

objb.o:		$(OBJB)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJB)

objc.o:		$(OBJC)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJC)


usys_xxx.o:		usys_xxx.cc usys_xxx.h			$(INCS)
usys_sunos.o:		usys_sunos.cc usys_sunos.h		$(INCS)
usys_darwin.o:		usys_darwin.cc usys_darwin.h		$(INCS)
usys_linux.o:		usys_linux.cc usys_linux.h		$(INCS)

# utilities
usys_ufcntl.o:		usys_ufcntl.cc usys_ufcntl.h 		$(INCS)
usys_ugetdents.o:	usys_ugetdents.cc usys_ugetdents.h	$(INCS)
usys_ugetnisdom.o:	usys_ugetnisdom.cc usys_ugetnisdom.h	$(INCS)

# missing operating system calls
usys_mqueue.o:		usys_mqueue.cc usys_mqueue.h		$(INCS)
usys_gethrtime.o:	usys_gethrtime.cc usys_gethrtime.h	$(INCS)
usys_getrandom.o:	usys_getrandom.cc usys_getrandom.h	$(INCS)
usys_ttynamerp.o:	usys_ttynamerp.cc usys_ttynamerp.h	$(INCS)
usys_stime.o:		usys_stime.cc usys_stime.h		$(INCS)
usys_resolvepath.o:	usys_resolvepath.cc usys_resolvepath.h	$(INCS)
usys_waitid.o:		usys_waitid.cc usys_waitid.h		$(INCS)
usys_sigx.o:		usys_sigx.cc usys_sigx.h		$(INCS)
usys_streams.o:		usys_streams.cc usys_streams.h		$(INCS)
usys_pipes.o:		usys_pipes.cc usys_pipes.h		$(INCS)
usys_stat.o:		usys_stat.cc usys_stat.h		$(INCS)
usys_libstr.o:		usys_libstr.cc usys_libstr.h		$(INCS)



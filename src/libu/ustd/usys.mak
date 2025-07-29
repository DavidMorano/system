# MAKEFILE (usys)

T= usys

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
CXX		?= gpp
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

INCS += usys.h

MODS +=

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


OBJ00_USYS= usys_xxx.o
OBJ01_USYS= usys_sunos.o 
OBJ02_USYS= usys_darwin.o usys_darwinexec.o usys_darwinargz.o
OBJ03_USYS= usys_linux.o 

OBJ04_USYS= usys_pathpid.o usysargz.o
OBJ05_USYS= usys_ttynamerp.o 
OBJ06_USYS= usys_stime.o usys_resolvepath.o usys_waitid.o
OBJ07_USYS= usys_sigx.o usys_streams.o usys_getexecname.o

OBJ08_USYS= usys_pipes.o usys_umaxmsglen.o
OBJ09_USYS= usys_ufcntl.o usys_ugetdents.o usys_ugetnisdom.o
OBJ10_USYS= usys_usendfile.o usys_ufstype.o usys_libstr.o
OBJ11_USYS= usys_mqueue.o usys_gethrtime.o usys_getrandom.o

OBJA= obj00_usys.o obj01_usys.o obj02_usys.o 
OBJB= obj03_usys.o obj04_usys.o obj05_usys.o
OBJC= obj06_usys.o obj07_usys.o obj08_usys.o
OBJD= obj09_usys.o obj10_usys.o obj11_usys.o

OBJ_USYS= obja.o objb.o objc.o objd.o


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


$(T).o:			$(OBJ_USYS)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ_USYS)

$(T).nm:		$(T).o
	$(NM) $(NMFLAGS) $(T).o > $(T).nm

again:
	rm -f $(ALL)

clean:
	makeclean $(ALL)

control:
	(uname -n ; date) > Control


obj00_usys.o:	$(OBJ00_USYS)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ00_USYS)

obj01_usys.o:	$(OBJ01_USYS)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ01_USYS)

obj02_usys.o:	$(OBJ02_USYS)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ02_USYS)

obj03_usys.o:	$(OBJ03_USYS)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ03_USYS)

obj04_usys.o:	$(OBJ04_USYS)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ04_USYS)

obj05_usys.o:	$(OBJ05_USYS)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ05_USYS)

obj06_usys.o:	$(OBJ06_USYS)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ06_USYS)

obj07_usys.o:	$(OBJ07_USYS)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ07_USYS)

obj08_usys.o:	$(OBJ08_USYS)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ08_USYS)

obj09_usys.o:	$(OBJ09_USYS)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ09_USYS)

obj10_usys.o:	$(OBJ10_USYS)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ10_USYS)

obj11_usys.o:	$(OBJ11_USYS)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ11_USYS)


obja.o:		$(OBJA)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJA)

objb.o:		$(OBJB)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJB)

objc.o:		$(OBJC)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJC)

objd.o:		$(OBJD)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJD)


# operating-system specific
usys_xxx.o:		usys_xxx.cc usys_xxx.h				$(INCS)
usys_sunos.o:		usys_sunos.cc usys_sunos.h			$(INCS)
usys_darwin.o:		usys_darwin.cc usys_darwin.h			$(INCS)
usys_darwinttyname.o:	usys_darwinttyname.cc usys_darwin.h		$(INCS)
usys_darwinexec.o:	usys_darwinexec.cc usys_darwin.h		$(INCS)
usys_darwinargz.o:	usys_darwinargz.cc usys_darwin.h		$(INCS)
usys_linux.o:		usys_linux.cc usys_linux.h			$(INCS)

# utilities
usys_ufcntl.o:		usys_ufcntl.cc usys_ufcntl.h 			$(INCS)
usys_ugetdents.o:	usys_ugetdents.cc usys_ugetdents.h		$(INCS)
usys_ugetnisdom.o:	usys_ugetnisdom.cc usys_ugetnisdom.h		$(INCS)
usys_usendfile.o:	usys_usendfile.cc usys_usendfile.h		$(INCS)
usys_ufstype.o:		usys_ufstype.cc usys_ufstype.h			$(INCS)
usys_pathpid.o:		usys_pathpid.cc usys_pathpid.h			$(INCS)
usys_umaxmsglen.o:	usys_umaxmsglen.cc	usys_umaxmsglen.h	$(INCS)
usysargz.o:		usysargz.cc usysargz.hh				$(INCS)

# missing operating system calls
usys_mqueue.o:		usys_mqueue.cc usys_mqueue.h			$(INCS)
usys_gethrtime.o:	usys_gethrtime.cc usys_gethrtime.h		$(INCS)
usys_getrandom.o:	usys_getrandom.cc usys_getrandom.h		$(INCS)
usys_ttynamerp.o:	usys_ttynamerp.cc usys_ttynamerp.h		$(INCS)
usys_stime.o:		usys_stime.cc usys_stime.h			$(INCS)
usys_resolvepath.o:	usys_resolvepath.cc usys_resolvepath.h		$(INCS)
usys_waitid.o:		usys_waitid.cc usys_waitid.h			$(INCS)
usys_sigx.o:		usys_sigx.cc usys_sigx.h			$(INCS)
usys_streams.o:		usys_streams.cc usys_streams.h			$(INCS)
usys_pipes.o:		usys_pipes.cc usys_pipes.h			$(INCS)
usys_stat.o:		usys_stat.cc usys_stat.h			$(INCS)
usys_libstr.o:		usys_libstr.cc usys_libstr.h			$(INCS)
usys_getexecname.o:	usys_getexecname.cc getexecname.h		$(INCS)



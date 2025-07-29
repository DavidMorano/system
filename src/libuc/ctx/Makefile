# MAKEFILES (ctx)

T= ctx

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

INCS= ctx.h

MODS += uconstants.ccm digtab.ccm cvtdig.ccm cvtfloat.ccm
MODS += fmtflag.ccm

LIBS=


INCDIRS=

LIBDIRS= -L${LIBDIR}


RUNINFO= -rpath $(RUNDIR)
LIBINFO= $(LIBDIRS) $(LIBS)

# flag setting
CPPFLAGS	?= $(DEFS) $(INCDIRS) $(MAKECPPFLAGS)
CFLAGS		?= $(MAKECFLAGS)
CXXFLAGS	?= $(MAKECXXFLAGS)
ARFLAGS		?= $(MAKEARFLAGS)
LDFLAGS		?= $(MAKELDFLAGS)


OBJ0_CTX= cvtdig.o cvtfloat.o
OBJ1_CTX= ctbin.o 
OBJ2_CTX= ctoct.o
OBJ3_CTX= ctdec.o ctdecp.o ctdecf.o

OBJ4_CTX= cthex.o
OBJ5_CTX= cta26.o
OBJ6_CTX= ctchars.o ctxxx.o 
OBJ7_CTX= ctroman.o ctwords.o

OBJA_CTX= obj0ctx.o obj1ctx.o obj2ctx.o obj3ctx.o
OBJB_CTX= obj4ctx.o obj5ctx.o obj6ctx.o obj7ctx.o

OBJ_CTX= obja_ctx.o objb_ctx.o


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


$(T).o:			$(OBJ_CTX)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ_CTX)

$(T).nm:		$(T).so
	$(NM) $(NMFLAGS) $(T).so > $(T).nm


again:
	rm -f $(ALL)

clean:
	makeclean $(ALL)

control:
	(uname -n ; date) > Control


obj0ctx.o:		$(OBJ0_CTX)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ0_CTX)

obj1ctx.o:		$(OBJ1_CTX)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ1_CTX)

obj2ctx.o:		$(OBJ2_CTX)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ2_CTX)

obj3ctx.o:		$(OBJ3_CTX)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ3_CTX)

obj4ctx.o:		$(OBJ4_CTX)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ4_CTX)

obj5ctx.o:		$(OBJ5_CTX)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ5_CTX)

obj6ctx.o:		$(OBJ6_CTX)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ6_CTX)

obj7ctx.o:		$(OBJ7_CTX)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ7_CTX)


obja_ctx.o:		$(OBJA_CTX)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJA_CTX)

objb_ctx.o:		$(OBJB_CTX)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJB_CTX)


ctchars.o:		mods.o ctchars.cc ctchars.h	$(INCS)
ctbin.o:		mods.o ctbin.cc ctbin.h		$(INCS)
ctoct.o:		mods.o ctoct.cc ctoct.h		$(INCS)
ctdec.o:		mods.o ctdec.cc ctdec.h		$(INCS)
ctdecp.o:		mods.o ctdecp.cc ctdecp.h	$(INCS)
ctdecf.o:		mods.o ctdecf.cc ctdecf.h	$(INCS)
cthex.o:		mods.o cthex.cc cthex.h		$(INCS)
cthexstr.o:		mods.o cthexstr.cc cthexstr.h	$(INCS)
cta26.o:		mods.o cta26.cc cta26.h		$(INCS)
ctxxx.o:		mods.o ctxxx.cc ctxxx.h		$(INCS)
ctroman.o:		mods.o ctroman.cc ctroman.h	$(INCS)
ctwords.o:		mods.o ctwords.cc ctwords.hh	$(INCS)

MOBJ += uconstants.o digtab.o cvtdig.o cvtfloat.o
MOBJ += fmtflag.o

mods.o:			$(MOBJ)
	$(LD) -r $(LDFLAGS) -o $@ $(MOBJ)

variables.o:		uconstants.ccm
digtab.o:		digtab.ccm

cvtdig.o:		cvtdig.ccm 			$(INCS)
cvtfloat.o:		cvtfloat.ccm 			$(INCS)

fmtflag.o:		fmtflag.ccm			$(INCS)



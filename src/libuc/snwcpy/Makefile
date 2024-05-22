# MAKEFILES (snwcpy)

T= snwcpy

ALL= $(T).o $(T).a


BINDIR= $(REPOROOT)/bin
INCDIR= $(REPOROOT)/include
LIBDIR= $(REPOROOT)/lib
MANDIR= $(REPOROOT)/man

INFODIR= $(REPOROOT)/info
HELPDIR= $(REPOROOT)/share/help
LDRPATH= $(REPOROOT)/lib

CRTDIR= $(CGS_CRTDIR)
VALDIR= $(CGS_VALDIR)


CPP=	cpp
CC=	gcc
CXX=	gpp
LD=	gld
RANLIB=	granlib
AR=	gar
NM=	gnm
COV=	gcov

LORDER=	lorder
TSORT=	tsort
LINT=	lint
RM=	rm -f
TOUCH=	touch
LINT=	lint


DEFS=

INCS= snwcpy.h snwcpyx.h snwcpyxc.h

LIBS=


INCDIRS=

LIBDIRS= -L$(LIBDIR)


LDRPATH=

# flag setting
CPPFLAGS= $(DEFS) $(INCDIRS) $(MAKECPPFLAGS)
CFLAGS= $(MAKECFLAGS)
CCFLAGS= $(MAKECCFLAGS)
ARFLAGS= $(MAKEARFLAGS)
LDFLAGS= $(MAKELDFLAGS)


OBJ0_SNWCPYX= snwcpyxc.o snwcpyopaque.o
OBJ1_SNWCPYX= snwcpyclean.o snwcpylatin.o
OBJ2_SNWCPYX= snwcpyshrink.o snwcpycompact.o
OBJ3_SNWCPYX= snwcpyer.o snwcpyrev.o
OBJ4_SNWCPYX= snwcpyhyphen.o snwcpywidehdr.o
OBJ5_SNWCPYX=

OBJA_SNWCPYX= obj0.o obj1.o
OBJB_SNWCPYX= obj2.o obj3.o
OBJC_SNWCPYX= obj4.o

OBJ_SNWCPYX= obja.o objb.o objc.o


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


$(T).o:			$(OBJ_SNWCPYX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ_SNWCPYX)

$(T).a:			$(OBJ_SNWCPYX)
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

obj0.o:			$(OBJ0_SNWCPYX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0_SNWCPYX)

obj1.o:			$(OBJ1_SNWCPYX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1_SNWCPYX)

obj2.o:			$(OBJ2_SNWCPYX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2_SNWCPYX)

obj3.o:			$(OBJ3_SNWCPYX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ3_SNWCPYX)

obj4.o:			$(OBJ4_SNWCPYX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ4_SNWCPYX)

obj5.o:			$(OBJ5_SNWCPYX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ5_SNWCPYX)


obja.o:			$(OBJA_SNWCPYX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJA_SNWCPYX)

objb.o:			$(OBJB_SNWCPYX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJB_SNWCPYX)

objc.o:			$(OBJC_SNWCPYX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJC_SNWCPYX)


snwcpyclean.o:		snwcpyclean.cc		$(INCS)
snwcpycompact.o:	snwcpycompact.cc	$(INCS)
snwcpyer.o:		snwcpyer.cc		$(INCS)
snwcpyhyphen.o:		snwcpyhyphen.cc		$(INCS)
snwcpylatin.o:		snwcpylatin.cc		$(INCS)
snwcpyopaque.o:		snwcpyopaque.cc		$(INCS)
snwcpyrev.o:		snwcpyrev.cc		$(INCS)
snwcpyshrink.o:		snwcpyshrink.cc		$(INCS)
snwcpywidehdr.o:	snwcpywidehdr.cc	$(INCS)
snwcpyxc.o:		snwcpyxc.cc		$(INCS)



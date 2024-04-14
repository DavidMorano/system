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


LIBDIRS= -L$(LIBDIR)

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

OBJA_SNWCPYX= obj0_snwcpyx.o obj1_snwcpyx.o
OBJB_SNWCPYX= obj2_snwcpyx.o obj3_snwcpyx.o
OBJC_SNWCPYX= obj4_snwcpyx.o

OBJ_SNWCPYX= $(OBJA_SNWCPYX) $(OBJB_SNWCPYX) $(OBJC_SNWCPYX)


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

obj0_snwcpyx.o:	$(OBJ0_SNWCPYX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0_SNWCPYX)

obj1_snwcpyx.o:	$(OBJ1_SNWCPYX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1_SNWCPYX)

obj2_snwcpyx.o:	$(OBJ2_SNWCPYX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2_SNWCPYX)

obj3_snwcpyx.o:	$(OBJ3_SNWCPYX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ3_SNWCPYX)

obj4_snwcpyx.o:	$(OBJ4_SNWCPYX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ4_SNWCPYX)



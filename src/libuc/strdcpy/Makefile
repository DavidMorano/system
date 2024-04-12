# MAKEFILES (strdcpy)

T= strdcpy

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


DEFS=


INCS= strdcpy.h


LIBS=


LIBDIRS= -L$(LIBDIR)

# flag setting
CPPFLAGS= $(DEFS) $(INCDIRS) $(MAKECPPFLAGS)
CFLAGS= $(MAKECFLAGS)
CCFLAGS= $(MAKECCFLAGS)
ARFLAGS= $(MAKEARFLAGS)
LDFLAGS= $(MAKELDFLAGS)


OBJ0_DTRDCPY= strdcpyx.o strdcpyxw.o
OBJ1_STRDCPY= strdcpyclean.o
OBJ2_STRDCPY= strdcpycompact.o
OBJ3_STRDCPY= strdcpyopaque.o


OBJA_STRDCPY= obj0_strd.o obj1_strd.o
OBJB_STRDCPY= obj2_strd.o obj3_strd.o

OBJ_STRDCPY= $(OBJA_STRDCPY) $(OBJB_STRDCPY)


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


$(T).o:			$(OBJ_STRDCPY)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ_STRDCPY)

$(T).a:			$(OBJ_STRDCPY)
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


obj0_strd.o:	$(OBJ0_DTRDCPY)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0_DTRDCPY)

obj1_strd.o:	$(OBJ1_STRDCPY)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1_STRDCPY)

obj2_strd.o:	$(OBJ2_STRDCPY)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2_STRDCPY)

obj3_strd.o:	$(OBJ3_STRDCPY)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ3_STRDCPY)


strdcpyx.o:		strdcpyx.cc strdcpyx.h $(INCS)
strdcpyxw.o:		strdcpyxw.cc strdcpyxw.h $(INCS)

strdcpyclean.o:		strdcpyclean.cc $(INCS)
strdcpycompact.o:	strdcpycompact.cc $(INCS)
strdcpyopaque.o:	strdcpyopaque.cc $(INCS)



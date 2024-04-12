# MAKEFILES (matxstr)

T= matxstr

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


INCS= matxstr.h


LIBS=


LIBDIRS= -L$(LIBDIR)

# flag setting
CPPFLAGS= $(DEFS) $(INCDIRS) $(MAKECPPFLAGS)
CFLAGS= $(MAKECFLAGS)
CCFLAGS= $(MAKECCFLAGS)
ARFLAGS= $(MAKEARFLAGS)
LDFLAGS= $(MAKELDFLAGS)


OBJ0_MATXSTR= matstr.o matostr.o matpstr.o
OBJ1_MATXSTR= matkeystr.o matlocalfs.o
OBJ2_MATXSTR= matsubstr.o matcasesubstr.o
OBJ3_MATXSTR= matnstr.o


OBJA_MATXSTR= obj0_matxstr.o obj1_matxstr.o
OBJB_MATXSTR= obj2_matxstr.o obj3_matxstr.o

OBJ_MATXSTR= $(OBJA_MATXSTR) $(OBJB_MATXSTR)


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


$(T).o:			$(OBJ_MATXSTR)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ_MATXSTR)

$(T).a:			$(OBJ_MATXSTR)
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

obj0_matxstr.o:	$(OBJ0_MATXSTR)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0_MATXSTR)

obj1_matxstr.o:	$(OBJ1_MATXSTR)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1_MATXSTR)

obj2_matxstr.o:	$(OBJ2_MATXSTR)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2_MATXSTR)

obj3_matxstr.o:	$(OBJ3_MATXSTR)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ3_MATXSTR)


matstr.o:		matstr.cc matstr.h		$(INCS)
matostr.o:		matostr.cc matostr.h		$(INCS)
matpstr.o:		matpstr.cc matpstr.h		$(INCS)
matkeystr.o:		matkeystr.cc matkeystr.h	$(INCS)
matsubstr.o:		matsubstr.cc			$(INCS)
matcasesubstr.o:	matcasesubstr.cc		$(INCS)
matnstr.o:		matnstr.cc			$(INCS)
matlocalfs.o:		matlocalfs.cc			$(INCS)



# MAKEFILES (strlist)

T= strlist

ALL= $(T).o $(T).a


BINDIR= $(REPOROOT)/bin
INCDIR= $(REPOROOT)/include
LIBDIR= $(REPOROOT)/lib
MANDIR= $(REPOROOT)/man
INFODIR= $(REPOROOT)/info
HELPDIR= $(REPOROOT)/share/help


CRTDIR= $(CGS_CRTDIR)
VALDIR= $(CGS_VALDIR)
LIBDIR= $(CGS_LIBDIR)

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

LDRPATH= $(EXTRA)/lib

LIBDIRS= -L$(LIBDIR)

LIBS=


# flag setting
CPPFLAGS= $(DEFS) $(INCDIRS) $(MAKECPPFLAGS)
CFLAGS= $(MAKECFLAGS)
CCFLAGS= $(MAKECCFLAGS)
ARFLAGS= $(MAKEARFLAGS)
LDFLAGS= $(MAKELDFLAGS)


INCS= strlist.h


OBJ0_STRLIST= 
OBJ1_STRLIST= 
OBJ2_STRLIST= 
OBJ3_STRLIST= 
OBJ4_STRLIST= 


OBJA_STRLIST= obj0_strlist.o obj1_strlist.o
OBJB_STRLIST= obj2_strlist.o obj3_strlist.o obj4_strlist.o

OBJ_STRLIST= $(OBJA_STRLIST) $(OBJB_STRLIST)


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


$(T).o:			$(OBJ_STRLIST)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ_STRLIST)

$(T).a:			$(OBJ_STRLIST)
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

obj0_strlist.o:	$(OBJ0_STRLIST)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0_STRLIST)

obj1_strlist.o:	$(OBJ1_STRLIST)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1_STRLIST)

obj2_strlist.o:	$(OBJ2_STRLIST)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2_STRLIST)

obj3_strlist.o:	$(OBJ3_STRLIST)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ3_STRLIST)

obj4_strlist.o:	$(OBJ4_STRLIST)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ4_STRLIST)




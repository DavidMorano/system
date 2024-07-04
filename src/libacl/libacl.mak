# MAKEFILES (libacl)

T= libacl

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


DEFS +=

INCS += libacl.h

LIBS +=


INCDIRS +=

LIBDIRS += -L$(LIBDIR)

LDRPATH= $(USRLOCAL)/lib

# flag setting
CPPFLAGS= $(DEFS) $(INCDIRS) $(MAKECPPFLAGS)
CFLAGS= $(MAKECFLAGS)
CXXFLAGS= $(MAKECXXFLAGS)
ARFLAGS= $(MAKEARFLAGS)
LDFLAGS= $(MAKELDFLAGS)


OBJ0_LIBACL= sialnum.o sialpha.o sibasename.o sibreak.o
OBJ1_LIBACL= libaclchr.o sicasechr.o sicite.o sidigit.o sidquote.o
OBJ2_LIBACL= sihyphen.o silbrace.o sileader.o sinext.o
OBJ3_LIBACL= siskipwhite.o sispan.o sisub.o sicasesub.o siterm.o
OBJ4_LIBACL= sifext.o


OBJA_LIBACL= obj0_libacl.o obj1_libacl.o
OBJB_LIBACL= obj2_libacl.o obj3_libacl.o obj4_libacl.o

OBJ_LIBACL= $(OBJA_LIBACL) $(OBJB_LIBACL)


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
	$(CXX)  $(CPPFLAGS) $(CXXFLAGS) -c $<


$(T).o:			$(OBJ_LIBACL)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ_LIBACL)

$(T).a:			$(OBJ_LIBACL)
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


obj0_libacl.o:	$(OBJ0_LIBACL)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0_LIBACL)

obj1_libacl.o:	$(OBJ1_LIBACL)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1_LIBACL)

obj2_libacl.o:	$(OBJ2_LIBACL)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2_LIBACL)

obj3_libacl.o:	$(OBJ3_LIBACL)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ3_LIBACL)

obj4_libacl.o:	$(OBJ4_LIBACL)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ4_LIBACL)



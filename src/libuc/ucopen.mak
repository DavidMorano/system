# MAKEFILES (ucopen)

T= ucopen

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

INCS += ucopen.h

LIBS +=


INCDIRS +=

LIBDIRS += -L$(LIBDIR)

# flag setting
CPPFLAGS= $(DEFS) $(INCDIRS) $(MAKECPPFLAGS)
CFLAGS= $(MAKECFLAGS)
CCFLAGS= $(MAKECCFLAGS)
ARFLAGS= $(MAKEARFLAGS)
LDFLAGS= $(MAKELDFLAGS)


OBJ00= ucopenmain.o ucaccepte.o
OBJ01= ucopensysdb.o ucopenuser.o
OBJ02= ucopeninfo.o ucopenpt.o ucopenshm.o
OBJ03= ucopensocket.o ucopensys.o

OBJ04= uc_opendev.o uc_opendialer.o
OBJ05= uc_openfint.o uc_openfsvc.o
OBJ06= uc_openfs.o uc_openpass.o
OBJ07= uc_openprog.o uc_openproto.o

OBJ08= uc_openpsem.o uc_openusvc.o
OBJ09= uc_create.o
OBJ10=
OBJ11=

OBJA= obj00.o obj01.o obj02.o obj03.o
OBJB= obj04.o obj05.o obj06.o obj07.o
OBJC= obj08.o obj09.o

OBJ_UCOPEN= obja.o objb.o objc.o


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


$(T).o:			$(OBJ_UCOPEN)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ_UCOPEN)

$(T).a:			$(OBJ_UCOPEN)
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


obj00.o:		$(OBJ00)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ00)

obj01.o:		$(OBJ01)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ01)

obj02.o:		$(OBJ02)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ02)

obj03.o:		$(OBJ03)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ03)

obj04.o:		$(OBJ04)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ04)

obj05.o:		$(OBJ05)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ05)

obj06.o:		$(OBJ06)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ06)

obj07.o:		$(OBJ07)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ07)


obja.o:			$(OBJA)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJA)

objb.o:			$(OBJB)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJB)

objc.o:			$(OBJC)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJC)

objd.o:			$(OBJD)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJD)


ucopenmain.o:		ucopenmain.cc		$(INCS)
ucaccepte.o:		ucaccepte.cc		$(INCS)
ucopeninfo.o:		ucopeninfo.cc		$(INCS)
ucopenpt.o:		ucopenpt.cc		$(INCS)
ucopenshm.o:		ucopenshm.cc		$(INCS)
ucopensocket.o:		ucopensocket.cc		$(INCS)
ucopensys.o:		ucopensys.cc		$(INCS)
ucopensysdb.o:		ucopensysdb.cc		$(INCS)
ucopenuser.o:		ucopenuser.cc		$(INCS)

uc_create.o:		uc_create.cc		$(INCS)
uc_opendev.o:		uc_opendev.cc		$(INCS)
uc_opendialer.o:	uc_opendialer.cc	$(INCS)
uc_openfint.o:		uc_openfint.cc		$(INCS)
uc_openfs.o:		uc_openfs.cc		$(INCS)
uc_openfsvc.o:		uc_openfsvc.cc		$(INCS)
uc_openpass.o:		uc_openpass.cc		$(INCS)
uc_openprog.o:		uc_openprog.cc		$(INCS)
uc_openproto.o:		uc_openproto.cc		$(INCS)
uc_openpsem.o:		uc_openpsem.cc		$(INCS)
uc_openusvc.o:		uc_openusvc.cc		$(INCS)



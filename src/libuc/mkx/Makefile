# MAKEFILES (mkx)

T= mkx

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

INCS= mkx.h

MODS +=

LIBS=


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


OBJ00_MKX= mkaddrname.o mkaddrdisp.o mkaddrbest.o mkaddrfrom.o
OBJ01_MKX= mkaltext.o
OBJ02_MKX= mkcdpath.o mkcexsync.o 
OBJ03_MKX= mkbasename.o
OBJ04_MKX= mkdisplayable.o
OBJ05_MKX= mkexpandpath.o mkfdfname.o mkfingerquery.o 
OBJ06_MKX= mkfmtphone.o mkfname.o mkfnamesuf.o
OBJ07_MKX= mkgecosname.o
OBJ08_MKX= mkintfname.o mkmagic.o
OBJ09_MKX= mkmaildirtest.o mkmailname.o mkmid.o
OBJ10_MKX= mknoise.o mknpathx.o mknpathxw.o
OBJ11_MKX= mkonefrom.o mkpathrooted.o mkpathx.o mkpathxw.o
OBJ12_MKX= mkquoted.o
OBJ13_MKX= mkrealname.o
OBJ14_MKX= mkshlibname.o mkshmname.o mksofname.o mkunique.o
OBJ15_MKX= mktagfname.o
OBJ16_MKX= mkufname.o
OBJ17_MKX= mkuserpath.o mkutmpid.o
OBJ18_MKX= mkuuid.o mkvarpath.o
OBJ19_MKX= mkpr.o mkxdisp.o

OBJA_MKX= obj00_mkx.o obj01_mkx.o obj02_mkx.o obj03_mkx.o
OBJB_MKX= obj04_mkx.o obj05_mkx.o obj06_mkx.o obj07_mkx.o
OBJC_MKX= obj08_mkx.o obj09_mkx.o obj10_mkx.o obj11_mkx.o
OBJD_MKX= obj12_mkx.o obj13_mkx.o obj14_mkx.o obj15_mkx.o
OBJE_MKX= obj16_mkx.o obj17_mkx.o obj18_mkx.o obj19_mkx.o

OBJ_MKX= obja_mkx.o objb_mkx.o objc_mkx.o objd_mkx.o obje_mkx.o


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


$(T).o:			$(OBJ_MKX)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ_MKX)

$(T).a:			$(OBJ_MKX)
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


obj00_mkx.o:	$(OBJ00_MKX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ00_MKX)

obj01_mkx.o:	$(OBJ01_MKX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ01_MKX)

obj02_mkx.o:	$(OBJ02_MKX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ02_MKX)

obj03_mkx.o:	$(OBJ03_MKX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ03_MKX)

obj04_mkx.o:	$(OBJ04_MKX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ04_MKX)

obj05_mkx.o:	$(OBJ05_MKX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ05_MKX)

obj06_mkx.o:	$(OBJ06_MKX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ06_MKX)

obj07_mkx.o:	$(OBJ07_MKX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ07_MKX)

obj08_mkx.o:	$(OBJ08_MKX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ08_MKX)

obj09_mkx.o:	$(OBJ09_MKX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ09_MKX)

obj10_mkx.o:	$(OBJ10_MKX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ10_MKX)

obj11_mkx.o:	$(OBJ11_MKX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ11_MKX)

obj12_mkx.o:	$(OBJ12_MKX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ12_MKX)

obj13_mkx.o:	$(OBJ13_MKX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ13_MKX)

obj14_mkx.o:	$(OBJ14_MKX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ14_MKX)

obj15_mkx.o:	$(OBJ15_MKX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ15_MKX)

obj16_mkx.o:	$(OBJ16_MKX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ16_MKX)

obj17_mkx.o:	$(OBJ17_MKX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ17_MKX)

obj18_mkx.o:	$(OBJ18_MKX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ18_MKX)

obj19_mkx.o:	$(OBJ19_MKX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ19_MKX)


obja_mkx.o:	$(OBJA_MKX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJA_MKX)

objb_mkx.o:	$(OBJB_MKX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJB_MKX)

objc_mkx.o:	$(OBJC_MKX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJC_MKX)

objd_mkx.o:	$(OBJD_MKX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJD_MKX)

obje_mkx.o:	$(OBJE_MKX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJE_MKX)


mkpathrooted.o:		mkpathrooted.cc	mkpath.h	$(INCS)
mkpathx.o:		mkpathx.cc	mkpath.h	$(INCS)
mkpathxw.o:		mkpathxw.cc	mkpath.h	$(INCS)

mknpathx.o:		mknpathx.cc	mknpath.h	$(INCS)
mknpathxw.o:		mknpathxw.cc	mknpath.h	$(INCS)

mkfdfname.o:		mkfdfname.cc		$(INCS) mkfdfname.h
mkgecosname.o:		mkgecosname.cc		$(INCS) mkgecosname.h
mkmid.o:		mkmid.cc		$(INCS)

mkaddrname.o:		mkaddrname.cc		$(INCS)
mkaddrdisp.o:		mkaddrdisp.cc		$(INCS)
mkaddrbest.o:		mkaddrbest.cc		$(INCS)
mkaddrfrom.o:		mkaddrfrom.cc		$(INCS)

mkbasename.o:		mkbasename.cc		$(INCS)
mkshlibname.o:		mkshlibname.cc		$(INCS)
mkshmname.o:		mkshmname.cc		$(INCS)
mksofname.o:		mksofname.cc		$(INCS)
mkintfname.o:		mkintfname.cc		$(INCS)
mkufname.o:		mkufname.cc		$(INCS)

mkonefrom.o:		mkonefrom.cc		$(INCS)
mkcdpath.o:		mkcdpath.cc		$(INCS)
mkuserpath.o:		mkuserpath.cc		$(INCS)
mkvarpath.o:		mkvarpath.cc		$(INCS)

mkaltext.o:		mkaltext.cc				$(INCS)
mkmaildirtest.o:	mkmaildirtest.cc			$(INCS)
mkunique.o:		mkunique.cc				$(INCS)
mkfmtphone.o:		mkfmtphone.cc				$(INCS)
mkfingerquery.o:	mkfingerquery.cc			$(INCS)
mklinelcean.o:		mklineclean.cc	mklineclean.h		$(INCS)

mkpr.o:			mkpr.cc mkpr.h				$(INCS)
mkxdisp.o:		mkxdisp.cc	mkxdisp.h		$(INCS)



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


OBJ00_MKX= mkaddrname.o mkaddrdisp.o mkaddrbest.o mkaddrfrom.o
OBJ01_MKX= mkaltext.o
OBJ02_MKX= mkcexsync.o 
OBJ03_MKX= mkbasename.o
OBJ04_MKX= mkdisplayable.o
OBJ05_MKX= mkfdfname.o mkfingerquery.o 
OBJ06_MKX= mkfmtphone.o mkfname.o mkfnamesuf.o
OBJ07_MKX= mkgecosname.o
OBJ08_MKX= mkintfname.o mkmagic.o
OBJ09_MKX= mkmaildirtest.o mkmailname.o mkmid.o
OBJ10_MKX= mknoise.o mknpathx.o mknpathxw.o
OBJ11_MKX= mkonefrom.o mkpathx.o mkpathxw.o
OBJ12_MKX= mkquoted.o
OBJ13_MKX= mkrealname.o
OBJ14_MKX= mkshlibname.o mkshmname.o mksofname.o mkunique.o
OBJ15_MKX= mktagfname.o
OBJ16_MKX= mkufname.o
OBJ17_MKX= mkutmpid.o
OBJ18_MKX= mkuuid.o 
OBJ19_MKX= mkpr.o mkxdisp.o

OBJ20_MKX= mkpathrooted.o 
OBJ21_MKX= mkpathuser.o mkpathcd.o
OBJ22_MKX= mkpathexp.o
OBJ23_MKX= mkpathmulti.o

OBJA_MKX= obj00.o obj01.o obj02.o obj03.o
OBJB_MKX= obj04.o obj05.o obj06.o obj07.o
OBJC_MKX= obj08.o obj09.o obj10.o obj11.o
OBJD_MKX= obj12.o obj13.o obj14.o obj15.o
OBJE_MKX= obj16.o obj17.o obj18.o obj19.o
OBJF_MKX= obj20.o obj21.o obj22.o obj23.o

OBJ_MKX= obja.o objb.o objc.o objd.o obje.o objf.o


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


.SUFFIXES:		.hh .ii .iim .ccm


default:		$(T).o

all:			$(ALL)


.c.i:
	$(CPP) $(CPPFLAGS) $< > $(*).i

.cc.ii:
	$(CPP) $(CPPFLAGS) $< > $(*).ii

.ccm.iim:
	$(CPP) $(CPPFLAGS) $< > $(*).iim

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


obj00.o:	$(OBJ00_MKX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ00_MKX)

obj01.o:	$(OBJ01_MKX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ01_MKX)

obj02.o:	$(OBJ02_MKX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ02_MKX)

obj03.o:	$(OBJ03_MKX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ03_MKX)

obj04.o:	$(OBJ04_MKX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ04_MKX)

obj05.o:	$(OBJ05_MKX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ05_MKX)

obj06.o:	$(OBJ06_MKX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ06_MKX)

obj07.o:	$(OBJ07_MKX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ07_MKX)

obj08.o:	$(OBJ08_MKX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ08_MKX)

obj09.o:	$(OBJ09_MKX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ09_MKX)

obj10.o:	$(OBJ10_MKX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ10_MKX)

obj11.o:	$(OBJ11_MKX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ11_MKX)

obj12.o:	$(OBJ12_MKX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ12_MKX)

obj13.o:	$(OBJ13_MKX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ13_MKX)

obj14.o:	$(OBJ14_MKX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ14_MKX)

obj15.o:	$(OBJ15_MKX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ15_MKX)

obj16.o:	$(OBJ16_MKX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ16_MKX)

obj17.o:	$(OBJ17_MKX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ17_MKX)

obj18.o:	$(OBJ18_MKX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ18_MKX)

obj19.o:	$(OBJ19_MKX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ19_MKX)


obj20.o:		$(OBJ20_MKX)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj21.o:		$(OBJ21_MKX)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj22.o:		$(OBJ22_MKX)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj23.o:		$(OBJ23_MKX)
	$(LD) -r $(LDFLAGS) -o $@ $^


obja.o:			$(OBJA_MKX)
	$(LD) -r $(LDFLAGS) -o $@ $^

objb.o:			$(OBJB_MKX)
	$(LD) -r $(LDFLAGS) -o $@ $^

objc.o:			$(OBJC_MKX)
	$(LD) -r $(LDFLAGS) -o $@ $^

objd.o:			$(OBJD_MKX)
	$(LD) -r $(LDFLAGS) -o $@ $^

obje.o:			$(OBJE_MKX)
	$(LD) -r $(LDFLAGS) -o $@ $^

objf.o:			$(OBJF_MKX)
	$(LD) -r $(LDFLAGS) -o $@ $^


mkpathx.o:		mkpathx.cc	mkpath.h	$(INCS)
mkpathxw.o:		mkpathxw.cc	mkpath.h	$(INCS)

mkpathrooted.o:		mkpathrooted.cc	mkpath.h	$(INCS)
mkpathuser.o:		mkpathuser.cc			$(INCS)
mkpathcd.o:		mkpathcd.cc			$(INCS)
mkpathexp.o:		mkpathexp.cc			$(INCS)
mkpathmulti.o:		mkpathmulti.cc			$(INCS)

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

mkaltext.o:		mkaltext.cc				$(INCS)
mkmaildirtest.o:	mkmaildirtest.cc			$(INCS)
mkunique.o:		mkunique.cc				$(INCS)
mkfmtphone.o:		mkfmtphone.cc				$(INCS)
mkfingerquery.o:	mkfingerquery.cc			$(INCS)
mklinelcean.o:		mklineclean.cc	mklineclean.h		$(INCS)

mkpr.o:			mkpr.cc		mkpr.h			$(INCS)
mkxdisp.o:		mkxdisp.cc	mkxdisp.h		$(INCS)

# depracated
mkcdpath.o:		mkcdpath.cc		$(INCS)
mkuserpath.o:		mkuserpath.cc		$(INCS)
mkexpandpath.o:		mkexpandpath.cc		$(INCS)


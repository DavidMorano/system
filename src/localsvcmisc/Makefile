# MAKEFILE

T= misc

TT= test

ALL= $(T).so 

SRCROOT= $(LOCAL)


BINDIR= $(SRCROOT)/bin
INCDIR= $(SRCROOT)/include
LIBDIR= $(SRCROOT)/lib
MANDIR= $(SRCROOT)/man/man7
SVCDIR= $(SRCROOT)/lib/opensvcs
EXTRALIBDIR= $(EXTRA)/lib

LDCRTDIR= $(CGS_CRTDIR)
VALDIR= $(CGS_VALDIR)


CC= $(CGS_CC)
GPP= $(CGS_GPP)

GPPFLAGS= $(CGS_CXXFLAGS)
CCOPTS_GCCALL= -pthreads -fexceptions
CCOPTS_CCALL= -MT

CCOPTS_GCCOPTSGPP= $(CGS_CXXFLAGS)
CCOPTS_GCCOPTSPARC= -mcpu=ultrasparc
CCOPTS_GCCLIBSPARC= -mno-app-regs

CCOPTS= $(CCOPTS_GCCALL) -O3 $(CCOPTS_GCCOPTSPARC)
#CCOPTS= $(CCOPTS_GCCALL) -g -O
#CCOPTS= $(CCOPTS_GCCALL) -g -pg
#CCOPTS= $(CCOPTS_GCCALL) -g -Wstrict-aliasing
#CCOPTS= $(CCOPTS_GCCALL) -g -Wall

CCOPTS_GCCLIB= -fpic $(CCOPTS_GCCLIBSPARC)

#CCOPTS= $(CCOPTS_CCALL) -g -xs

# HyperSPARC
#CCOPTS= -xO5 -xtarget=ss20/hs22 -dalign -xdepend

# UltraSPARC
#CCOPTS= -xO5 -xtarget=ultra -xsafe=mem -dalign -xdepend


DEF0=
DEF1=
DEF2=
DEF3=
DEF4=
DEF5=
DEF6=
DEF7= $(LF_DEFS)

DEFS= $(DEF0) $(DEF1) $(DEF2) $(DEF3) $(DEF4) $(DEF5) $(DEF6) $(DEF7)


INCDIRS= -I$(INCDIR)

CPPFLAGS= $(DEFS) $(INCDIRS)

CFLAGS= $(CCOPTS)

#LD= $(CC)
#LD= cc
LD= ld


LDRPATH=

LDFLAGS= -m $(LF_LDFLAGS) -R$(LIBDIR):$(CGS_RPATH):$(EXTRALIBDIR)

SLDFLAGS= $(LF_LDFLAGS) $(LDFLAGS) -M $(T).map


LIBDIRS= -L$(LIBDIR) -L$(CGS_LIBDIR)

LIB0=
LIB1= -ldam -lb
LIB2=
LIB3= -luc -lu
LIB4= $(CGS_LIBS)
LIB5= 
LIB6= -lsecdb -lproject -lpthread -lrt -lxnet -lsocket -lnsl
LIB7= -ldl -lc

LIBS= $(LIB0) $(LIB1) $(LIB2) $(LIB3) $(LIB4) $(LIB5) $(LIB6) $(LIB7)

LIBINFO= $(LIBDIRS) $(LIBS)


SLIBDIRS= -L$(LIBDIR) -L$(CGS_LIBDIR)

SLIB0=
SLIB0=
SLIB1= -ldam -lb
SLIB2=
SLIB3= -luc -lu
SLIB4= $(CGS_LIBS)
SLIB5=
SLIB6= -lsecdb -lproject -lpthread -lrt -lxnet -lsocket -lnsl
SLIB7=

SLIBA= $(SLIB0) $(SLIB1) $(SLIB2) $(SLIB3) 
SLIBB= $(SLIB4) $(SLIB5) $(SLIB6) $(SLIB7)

SLIBS= $(SLIBA) $(SLIBB)

SLIBINFO= $(SLIBDIRS) $(SLIBS)


XLIBDIRS= -L$(LIBDIR) -L$(CGS_LIBDIR)

XLIB0=
XLIB1= -lcsdb -ldam -lb
XLIB2= -ldebug
XLIB3= -lut -luc -lu
XLIB4=
XLIB5=  $(LF_LIBS)
XLIB6= -lsecdb -lproject -lpthread -lrt -lxnet -lsocket -lnsl
XLIB7= -ldl

XLIBS= $(XLIB0) $(XLIB1) $(XLIB2) $(XLIB3) $(XLIB4) $(XLIB5) $(XLIB6) $(XLIB7)

XLIBINFO= $(SLIBDIRS) $(SLIBS)


VALUES= $(VALDIR)/$(CGS_VALUES)

CRT1= $(LDCRTDIR)/crt1.o
CRTI= $(LDCRTDIR)/crti.o
CRTBEGIN= $(LDCRTDIR)/crtbegin.o
MCRT1= $(LDCRTDIR)/mcrt1.o
GCRT1= $(LDCRTDIR)/gcrt1.o
CRTEND= $(LDCRTDIR)/crtend.o
CRTN= $(LDCRTDIR)/crtn.o

CRTFRONT= $(CRT1) $(CRTI) $(VALUES) $(CRTBEGIN)
CRTBACK= $(CRTEND) $(CRTN)

CRT0= $(CRT1) $(CRTI) $(VALUES)
CRTC= makedate.o

LINT= lint
LINTFLAGS= -uxn -Dlint

NM= nm
NMFLAGS= -xs -v

CPP= cpp


INCS= defs.h


OBJ00= opensvc_hello.o 
OBJ01= opensvc_daytime.o opensvc_fshome.o opensvc_org.o
OBJ02= opensvc_loginblurb.o opensvc_motdblurb.o opensvc_statmsg.o
OBJ03= opensvc_imail.o opensvc_termnote.o 
OBJ04= opensvc_quota.o opensvc_conslog.o
OBJ05= opensvc_issue.o opensvc_notices.o
OBJ06= 
OBJ07= 
OBJ08= 
OBJ09= localgetorg.o getuserorg.o localgetorgcode.o
OBJ10= localnoticecheck.o
OBJ11= isNotPresent.o isNotLib.o isSpecialObject.o
OBJ12= getourenv.o nusers.o
OBJ13= readfileline.o attachso.o
OBJ14= prgetprogpath.o
OBJ15=

OBJ16= issue.o statmsg.o termnote.o finduid.o pwcache.o
OBJ17= tmpx.o tmpx_getuserlines.o
OBJ18=
OBJ19= mbcache.o mailbox.o mailmsg.o mailmsg_enver.o 
OBJ20= mailmsghdrs.o
OBJ21= mailmsgmatenv.o mailmsgmathdr.o 
OBJ22= 
OBJ23=

OBJ24= mkuserpath.o mkfdfname.o mkbestfrom.o
OBJ25= sfsubstance.o
OBJ26= opentermnote.o writeto.o
OBJ27= ndig.o
OBJ28= timestr_nist.o
OBJ29=
OBJ30=
#OBJ31= debug.o debugprintf.o nprintf.o strlinelen.o

OBJA= $(OBJ00) $(OBJ01) $(OBJ02) $(OBJ03) $(OBJ04) $(OBJ05) $(OBJ06) $(OBJ07)
OBJB= $(OBJ08) $(OBJ09) $(OBJ10) $(OBJ11) $(OBJ12) $(OBJ13) $(OBJ14) $(OBJ15)
OBJC= $(OBJ16) $(OBJ17) $(OBJ18) $(OBJ19) $(OBJ20) $(OBJ21) $(OBJ22) $(OBJ23)
OBJD= $(OBJ24) $(OBJ25) $(OBJ26) $(OBJ27) $(OBJ28) $(OBJ29) $(OBJ30) $(OBJ31)

OBJ= $(OBJA) $(OBJB) $(OBJC) $(OBJD) makedate.o

OBJS= $(VALUES) $(OBJ)

AOBJ0= main.o proginfo.o proginfo_setpiv.o
AOBJ1= headkeymat.o 
AOBJ2= printhelp.o 
AOBJ3= $(OBJ)

AOBJ= $(AOBJ0) $(AOBJ1) $(AOBJ2) $(AOBJ3) makedate.o

AOBJS= $(CRTFRONT) $(AOBJ) $(CRTC) $(CRTBACK)


.SUFFIXES:		.ls .i .cx .cs


default:		$(T).so

all:			$(ALL)

.cc.o:
	$(GPP) -c $(GPPFLAGS) $(CFLAGS) $(CPPFLAGS) $<

.c.o:
	$(CC) -c $(CFLAGS) $(CCOPTS_GCCLIB) $(CPPFLAGS) $<

.c.ln:
	$(LINT) -c -u $(CPPFLAGS) $<

.c.ls:
	$(LINT) $(LINTFLAGS) $(CPPFLAGS) $<

.c.i:
	$(CPP) $(CPPFLAGS) $< > $(*).i

.c.cx:
	$(CXREF) -C $(CXREFFLAGS) $(CPPFLAGS) $<

.c.cs:
	$(CXREF) $(CXREFFLAGS) $(CPPFLAGS) -o $(*).cs $<


test:			$(TT).x

$(TT).x:		main.c $(AOBJS) Makefile
	$(LD) -o $@ $(LDFLAGS) $(AOBJS) $(LIBINFO) > $(TT).lm

$(T):			$(T).ee
	cp -p $(T).ee $(T)

$(T).so:		$(OBJ) Makefile $(T).map
	makedate > makedate.c
	$(CC) -c makedate.c
	$(LD) -G -o $@ $(SLDFLAGS) $(OBJS) $(SLIBINFO) > $(T).lm

$(T).$(OFF) $(OFF):	$(T).x
	cp -p $(T).x $(T).$(OFF)

so:			$(T).so

install:		install-so

install-so:		$(T).so
	makenewer -r $(T).so $(SVCDIR)

install-raw:		$(T).x
	rm -f $(BINDIR)/$(T).$(OFF)
	makenewer -r -o rmsuf $(T) $(BINDIR)

$(T).nm nm:		$(T).x
	$(NM) $(NMFLAGS) $(T).x > $(T).nm

safe:
	makesafe -v=3 -I $(INCDIR) $(OBJ)

strip:			$(T).so
	strip $(T).so
	rm -f $(T).$(OFF)

again:
	rm -f $(ALL) $(T).x $(T).$(OFF)

clean:			again
	makeclean

control:
	(uname -n ; date) > Control


main.o:			main.c $(INCS)

whatinfo.o:		whatinfo.c config.h

proginfo.o:		proginfo.c $(INCS)


opensvc_hello.o:	opensvc_hello.c opensvc_hello.h
	$(CC) -c $(CFLAGS) $(CCOPTS_GCCLIB) $(CPPFLAGS) $<

opensvc_daytime.o:	opensvc_daytime.c opensvc_daytime.h
	$(CC) -c $(CFLAGS) $(CCOPTS_GCCLIB) $(CPPFLAGS) $<

opensvc_loginblueb.o:	opensvc_loginblurb.c opensvc_loginblurb.h
	$(CC) -c $(CFLAGS) $(CCOPTS_GCCLIB) $(CPPFLAGS) $<

opensvc_fshome.o:	opensvc_fshome.c opensvc_fshome.h
	$(CC) -c $(CFLAGS) $(CCOPTS_GCCLIB) $(CPPFLAGS) $<

opensvc_org.o:		opensvc_org.c opensvc_org.h
	$(CC) -c $(CFLAGS) $(CCOPTS_GCCLIB) $(CPPFLAGS) $<

opensvc_projinfo.o:	opensvc_projinfo.c opensvc_projinfo.h
	$(CC) -c $(CFLAGS) $(CCOPTS_GCCLIB) $(CPPFLAGS) $<

opensvc_issue.o:	opensvc_issue.c opensvc_issue.h
	$(CC) -c $(CFLAGS) $(CCOPTS_GCCLIB) $(CPPFLAGS) $<

opensvc_motd.o:		opensvc_motd.c opensvc_motd.h
	$(CC) -c $(CFLAGS) $(CCOPTS_GCCLIB) $(CPPFLAGS) $<

opensvc_imail.o:	opensvc_imail.c opensvc_imail.h
	$(CC) -c $(CFLAGS) $(CCOPTS_GCCLIB) $(CPPFLAGS) $<

opensvc_termnote.o:	opensvc_termnote.c opensvc_termnote.h
	$(CC) -c $(CFLAGS) $(CCOPTS_GCCLIB) $(CPPFLAGS) $<

opensvc_statmsg.o:	opensvc_statmsg.c opensvc_statmsg.h
	$(CC) -c $(CFLAGS) $(CCOPTS_GCCLIB) $(CPPFLAGS) $<

opensvc_quota.o:	opensvc_quota.c opensvc_quota.h
	$(CC) -c $(CFLAGS) $(CCOPTS_GCCLIB) $(CPPFLAGS) $<

opensvc_notices.o:	opensvc_notices.c opensvc_notices.h
	$(CC) -c $(CFLAGS) $(CCOPTS_GCCLIB) $(CPPFLAGS) $<


# testing

X01= testopensvc

X01OBJ00= $(X01).o 
X01OBJ01=
X01OBJ02=
X01OBJ03=
X01OBJ04=
X01OBJ05=
X01OBJ06= uc_openfsvc.o
X01OBJ07= uc_openshmto.o uc_openshm.o

X01OBJa= $(X01OBJ00) $(X01OBJ01) $(X01OBJ02) $(X01OBJ03) 
X01OBJb= $(X01OBJ04) $(X01OBJ05) $(X01OBJ06) $(X01OBJ07)

X01OBJ= $(X01OBJa) $(X01OBJb)

$(X01).o:		$(X01).c

$(X01).x:		$(X01OBJ)
	$(CC) -o $@ $(X01OBJ) $(XLIBDIRS) $(XLIBS)



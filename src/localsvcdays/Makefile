# MAKEFILE (days)

T= days

ALL= $(T).so


BINDIR= $(SRCROOT)/bin
INCDIR= $(SRCROOT)/include
LIBDIR= $(SRCROOT)/lib
MANDIR= $(SRCROOT)/man/man7
SVCDIR= $(SRCROOT)/lib/opensvcs
EXTRALIBDIR= $(EXTRA)/lib
ESVCDIR= $(EXTRA)/lib/opensvcs

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
CXXFLAGS= $(GPPFLAGS) $(CCOPTS)

#LD= $(CC)
#LD= cc
LD= ld


LDRPATH= $(EXTRALIBDIR)

LDFLAGS= $(LF_LDFLAGS) -R$(LIBDIR):$(CGS_RPATH):$(EXTRALIBDIR) 


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

SLIB0= -llkcmd -lcsdb
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

XLIBINFO= $(XLIBDIRS) $(XLIBS)


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


INCS= config.h defs.h


SOBJ00= opensvc_cotd.o opensvc_votd.o opensvc_hotd.o
SOBJ01= opensvc_qotd.o
SOBJ02= opensvc_hols.o
SOBJ03= opensvc_motd.o
SOBJ04=
SOBJ05=
SOBJ06=
SOBJ07=

SOBJ08= votdc.o votdchdr.o
SOBJ09= shmalloc.o
SOBJ10= motd.o finduid.o
SOBJ11= spawner.o
SOBJ12=
SOBJ13=
SOBJ14=
SOBJ15=

SOBJ16= mkfdfname.o
SOBJ17= opentmpfile.o openqotd.o
SOBJ18= maintqotd.o maintqotd_prog.o
SOBJ19= attachso.o
SOBJ20= prmktmpdir.o getmjd.o
SOBJ21= 
SOBJ22=
SOBJ23=

SOBJAa= $(SOBJ00) $(SOBJ01) $(SOBJ02) $(SOBJ03)
SOBJAb= $(SOBJ04) $(SOBJ05) $(SOBJ06) $(SOBJ07)
SOBJA= $(SOBJAa) $(SOBJAb)

SOBJBa= $(SOBJ08) $(SOBJ09) $(SOBJ10) $(SOBJ11)
SOBJBb= $(SOBJ12) $(SOBJ13) $(SOBJ14) $(SOBJ15)
SOBJB= $(SOBJBa) $(SOBJBb)

SOBJCa= $(SOBJ16) $(SOBJ17) $(SOBJ18) $(SOBJ19)
SOBJCb= $(SOBJ20) $(SOBJ21) $(SOBJ22) $(SOBJ23)
SOBJC= $(SOBJCa) $(SOBJCb)

SOBJ= $(SOBJA) $(SOBJB) $(SOBJC)

SOBJS= $(VALUES) $(SOBJ)

ASOBJ0= main.o proginfo.o proginfo_setpiv.o
ASOBJ1= headkeymat.o 
ASOBJ2= printhelp.o 
ASOBJ3= $(SOBJ)

ASOBJ= $(ASOBJ0) $(ASOBJ1) $(ASOBJ2) $(ASOBJ3)

ASOBJS= $(CRTFRONT) $(ASOBJ) $(CRTC) $(CRTBACK)


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


$(T):			$(T).ee
	cp -p $(T).ee $(T)

$(T).so:		$(SOBJ) Makefile $(T).map
	$(LD) -G -o $@ $(LDFLAGS) -M $(T).map $(SOBJS) $(SLIBINFO) > $(T).lm

so:			$(T).so

install:		install-so

install-so:		$(T).so
	makenewer -r $(T).so $(SVCDIR)
	makenewer -r $(T).so $(ESVCDIR)

safe:
	makesafe -v=3 -I $(INCDIR) $(SOBJ)

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

opensvc_qotd.o:		opensvc_qotd.c opensvc_qotd.h
	$(CC) -c $(CFLAGS) $(CCOPTS_GCCLIB) $(CPPFLAGS) $<

opensvc_cotd.o:		opensvc_cotd.c opensvc_cotd.h
	$(CC) -c $(CFLAGS) $(CCOPTS_GCCLIB) $(CPPFLAGS) $<

opensvc_votd.o:		opensvc_votd.c opensvc_votd.h votdc.h
	$(CC) -c $(CFLAGS) $(CCOPTS_GCCLIB) $(CPPFLAGS) $<

opensvc_hotd.o:		opensvc_hotd.c opensvc_hotd.h
	$(CC) -c $(CFLAGS) $(CCOPTS_GCCLIB) $(CPPFLAGS) $<

opensvc_hols.o:		opensvc_hols.c opensvc_hols.h
	$(CC) -c $(CFLAGS) $(CCOPTS_GCCLIB) $(CPPFLAGS) $<


prmktmpdir.o:		prmktmpdir.c
	$(CC) -c $(CFLAGS) $(CCOPTS_GCCLIB) $(CPPFLAGS) $<


oc.o:			oc.c oc.h
	$(CC) -c $(CFLAGS) $(CCOPTS_GCCLIB) $(CPPFLAGS) $<


X01= testopensvc

X01OBJ00= $(X01).o 
X01OBJ01=
X01OBJ02=
X01OBJ03=
X01OBJ04=
X01OBJ05=
X01OBJ06=
X01OBJ07=

X01OBJa= $(X01OBJ00) $(X01OBJ01) $(X01OBJ02) $(X01OBJ03) 
X01OBJb= $(X01OBJ04) $(X01OBJ05) $(X01OBJ06) $(X01OBJ07)

X01OBJ= $(X01OBJa) $(X01OBJb)

$(X01).o:		$(X01).c

$(X01).x:		$(X01OBJ)
	$(CC) -o $@ $(X01OBJ) $(XLIBDIRS) $(XLIBS)


X02= testcotd

X02OBJ00= $(X02).o 
X02OBJ01=
X02OBJ02=
X02OBJ03=
X02OBJ04=
X02OBJ05=
X02OBJ06=
X02OBJ07=

X02OBJa= $(X02OBJ00) $(X02OBJ01) $(X02OBJ02) $(X02OBJ03) 
X02OBJb= $(X02OBJ04) $(X02OBJ05) $(X02OBJ06) $(X02OBJ07)

X02OBJ= $(X02OBJa) $(X02OBJb)

$(X02).o:		$(X02).c $(X02).h

$(X02).x:		$(X02OBJ)
	$(GPP) -o $@ $(X02OBJ) $(XLIBDIRS) $(XLIBS)

$(X02).o:		$(X02).cc
	$(GPP) -c $(GPPFLAGS) $(CFLAGS) $(CCOPTS_GCCLIB) $(CPPFLAGS) $(X02).cc


X03= testshmalloc

X03OBJ00= $(X03).o 
X03OBJ01= shmalloc.o
X03OBJ02= requests.o
X03OBJ03= vecobj.o
X03OBJ04=
X03OBJ05=
X03OBJ06=
X03OBJ07=

X03OBJa= $(X03OBJ00) $(X03OBJ01) $(X03OBJ02) $(X03OBJ03)
X03OBJb= $(X03OBJ04) $(X03OBJ05) $(X03OBJ06) $(X03OBJ07) 

X03OBJ= $(X03OBJa) $(X03OBJb)

$(X03).o:		$(X03).c $(X03).h

$(X03).x:		$(X03OBJ)
	$(GPP) -o $@ $(X03OBJ) $(XLIBDIRS) $(XLIBS)

$(X03).o:		$(X03).cc
	$(GPP) -c $(GPPFLAGS) $(CFLAGS) $(CCOPTS_GCCLIB) $(CPPFLAGS) $(X03).cc


X04= testvotdc

X04OBJ00= $(X04).o 
X04OBJ01= votdc.o votdchdr.o
X04OBJ02= shmalloc.o
X04OBJ03=
X04OBJ04=
X04OBJ05=
X04OBJ06=
X04OBJ07=

X04OBJa= $(X04OBJ00) $(X04OBJ01) $(X04OBJ02) $(X04OBJ03)
X04OBJb= $(X04OBJ04) $(X04OBJ05) $(X04OBJ06) $(X04OBJ07) 

X04OBJ= $(X04OBJa) $(X04OBJb)

$(X04).o:		$(X04).c $(X04).h

$(X04).x:		$(X04OBJ)
	$(GPP) -o $@ $(X04OBJ) $(XLIBDIRS) $(XLIBS)

$(X04).o:		$(X04).cc
	$(GPP) -c $(GPPFLAGS) $(CFLAGS) $(CCOPTS_GCCLIB) $(CPPFLAGS) $(X04).cc



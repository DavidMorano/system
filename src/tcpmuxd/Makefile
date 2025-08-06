# MAKEFILE

T= tcpmuxd

ALL= $(T) $(TR)

SRCROOT= $(LOCAL)


BINDIR= $(SRCROOT)/bin
INCDIR= $(SRCROOT)/include
LIBDIR= $(SRCROOT)/lib
MANDIR= $(SRCROOT)/man/man1
HELPDIR= $(SRCROOT)/share/help

EXTRABIN= /usr/extra/sbin


#LDCRTDIR= /opt/SUNWspro/WS6/lib
#LDCRTDIR= /opt/SUNWspro/SC5.0/lib
#LDCRTDIR= /opt/SUNWspro/SC4.0/lib
#LDCRTDIR= /opt/SUNWspro/lib
LDCRTDIR= $(SRCROOT)/lib


CC= gcc
GPP= g++

CCOPTS_GCCALL= -pthreads -fexceptions
CCOPTS_CCALL= -MT

CCOPTS_GCCOPTSPARC= -mcpu=ultrasparc
CCOPTS_GCCLIBSPARC= -mno-app-regs

CCOPTS= $(CCOPTS_GCCALL) -O2 $(CCOPTS_GCCOPTSPARC)
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

LDFLAGS= -m $(LF_LDFLAGS) -R$(LIBDIR)


LIBDIRS= -L$(LIBDIR)

LIB0=
LIB1= -ldam -lb
LIB2=
LIB3= -luc -lu
LIB4= -L$(GNU)/lib -lstdc++ -lgcc_eh -lgcc
LIB5= 
LIB6= -lsecdb -lproject -lpthread -lrt -lxnet -lsocket -lnsl
LIB7= -ldl -lc

LIBS= $(LIB0) $(LIB1) $(LIB2) $(LIB3) $(LIB4) $(LIB5) $(LIB6) $(LIB7)

LIBINFO= $(LIBDIRS) $(LIBS)


CRT1= $(LDCRTDIR)/crt1.o
CRTI= $(LDCRTDIR)/crti.o
VALUES= $(LDCRTDIR)/values-xa.o
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
NMFLAGS= -xs

CPP= cpp


INCS= config.h defs.h jobdb.h


OBJ00= main.o whatinfo.o proginfo.o proginfo_setpiv.o proginfo_rootname.o
OBJ01= proginfo_nameid.o proguserlist.o
OBJ02= progjobdir.o proglogout.o progcheckstamp.o
OBJ03= progprocess.o progwatch.o progcheckdir.o proghandle.o progreqfile.o 
OBJ04= progserve.o progsrvargs.o progpeername.o
OBJ05= progcmd.o progpass.o proguseracct.o progexec.o progshlib.o
OBJ06= progconfig.o progpid.o progsvc.o progacc.o proglog.o progipc.o
OBJ07= printhelp.o 
OBJ08=
OBJ09= field_svcargs.o field_srvarg.o
OBJ10= varsub_extras.o
OBJ11= standing.o builtin.o sysmisc.o muximsg.o procse.o
OBJ12= clientinfo.o
OBJ13= 
OBJ14=
OBJ15=
OBJ16= jobdb.o
OBJ17=
OBJ18=
OBJ19=
OBJ20=
OBJ21=
OBJ22=
OBJ23=
OBJ24=
OBJ25=
OBJ26=
OBJ27=
OBJ28=
OBJ29=
OBJ30=
OBJ31=

OBJ32= 
OBJ33=
OBJ34=
OBJ35=
OBJ36=
OBJ37=
OBJ38=
OBJ39=

OBJ40=
OBJ41=
OBJ42=
OBJ43=
OBJ44=
OBJ45=
OBJ46= 
OBJ47=

OBJ48= getproviderid.o
OBJ49=
OBJ50= isBadSend.o isBadRecv.o isBadMsg.o
OBJ51= 
OBJ52=
OBJ53=
OBJ54=
#OBJ55= debug.o debugfork.o debugprintf.o nprintf.o

OBJA= $(OBJ00) $(OBJ01) $(OBJ02) $(OBJ03) $(OBJ04) $(OBJ05) $(OBJ06) $(OBJ07)
OBJB= $(OBJ08) $(OBJ09) $(OBJ10) $(OBJ11) $(OBJ12) $(OBJ13) $(OBJ14) $(OBJ15)
OBJC= $(OBJ16) $(OBJ17) $(OBJ18) $(OBJ19) $(OBJ20) $(OBJ21) $(OBJ22) $(OBJ23)
OBJD= $(OBJ24) $(OBJ25) $(OBJ26) $(OBJ27) $(OBJ28) $(OBJ29) $(OBJ30) $(OBJ31)
OBJE= $(OBJ32) $(OBJ33) $(OBJ34) $(OBJ35) $(OBJ36) $(OBJ37) $(OBJ38) $(OBJ39)
OBJF= $(OBJ40) $(OBJ41) $(OBJ42) $(OBJ43) $(OBJ44) $(OBJ45) $(OBJ46) $(OBJ47)
OBJG= $(OBJ48) $(OBJ49) $(OBJ50) $(OBJ51) $(OBJ52) $(OBJ53) $(OBJ54) $(OBJ55)

OBJ= $(OBJA) $(OBJB) $(OBJC) $(OBJD) $(OBJE) $(OBJF) $(OBJG)

OBJS= $(CRTFRONT) $(OBJ) $(CRTC) $(CRTBACK)


.SUFFIXES:		.ls .i .cx .cs


default:		$(T).x

all:			$(ALL)

.c.o:
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $<

.cc.o:
	$(GPP) -c $(CFLAGS) $(CPPFLAGS) $<

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

$(T).x:			$(OBJ) Makefile
	rm -f $(T).x
	makedate -m $(T) > makedate.c
	$(CC) -c $(CFLAGS) makedate.c
	$(LD) -o $@ $(LDFLAGS) $(OBJS) $(LIBDIRS) $(LIBS) > $(T).lm

$(T).$(OFF) $(OFF):	$(T).x
	cp -p $(T).x $(T).$(OFF)

$(T).nm nm:		$(T).x
	$(NM) $(NMFLAGS) $(T).x > $(T).nm

safe:
	makesafe -I $(INCDIR) $(OBJ)

strip:			$(T).x
	strip $(T).x
	rm -f $(T).$(OFF) $(T)

install:		install-raw install-help

install-ee:		$(ALL)
	makenewer -r $(ALL) $(BINDIR)

install-raw:		$(T).x
	rm -f $(BINDIR)/$(T).$(OFF)
	makenewer -r -o rmsuf $(T).x $(BINDIR)

install-help:		$(T).help
	-mkdir -p $(HELPDIR) 2> /dev/null
	makenewer -o rmsuf $(T).help $(HELPDIR)

install.telserv:		$(T).x
	rm -f telserv
	ln -d $(T).x telserv
	makenewer -r telserv $(EXTRABIN)

again:
	rm -f $(ALL) $(T).x $(T).lm

clean:			again
	rm -f *.ls *.ln *.i *.o *.so *.x *.lm

control:
	uname -n > Control
	date >> Control


main.o:			main.c $(INCS)

whatinfo.o:		whatinfo.c config.h

proginfo.o:		proginfo.c defs.h

proginfo_setpiv_o:	proginfo_setpiv.c defs.h

proginfo_rootname.o:	proginfo_rootname.c defs.h

proguserlist.o:		proguserlist.c $(INCS)

progcmd.o:		progcmd.c $(INCS)

progpass.o:		progpass.c $(INCS)

progprocess.o:		progprocess.c $(INCS)

progconfig.o:		progconfig.c $(INCS)

progpid.o:		progpid.c $(INCS)

progsvc.o:		progsvc.c $(INCS)

progacc.o:		progacc.c $(INCS)

proglog.o:		proglog.c $(INCS)

progwatch.o:		progwatch.c $(INCS) poller.h

progipc.o:		progipc.c $(INCS)

proghandle.o:		proghandle.c $(INCS)

proglogout.o:		proglogout.c $(INCS)

progpcsconfig.o:	progpcsconfig.c $(INCS)

progcheckstamp.o:	progcheckstamp.c $(INCS)

progserve.o:		progserve.c $(INCS)

progsvcargs.o:		progsvcargs.c $(INCS)

progreqfile.o:		progreqfile.c $(INCS)

progexec.o:		progexec.c $(INCS)

progpeername.o:		progpeername.c $(INCS)

proguseracct.o:		proguseracct.c $(INCS)

standing.o:		standing.c standing.h $(INCS)

builtin.o:		builtin.c builtin.h $(INCS)

clientinfo.o:		clientinfo.c $(INCS)



# MAKEFILE

T= rsend

ALL= $(T)

SRCROOT= $(LOCAL)


BINDIR= $(SRCROOT)/bin
INCDIR= $(SRCROOT)/include
LIBDIR= $(SRCROOT)/lib
HELPDIR= $(SRCROOT)/share/help

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

CCOPTS= $(CCOPTS_GCCALL) -O $(CCOPTS_GCCOPTSPARC)
#CCOPTS= $(CCOPTS_GCCALL) -g -O
#CCOPTS= $(CCOPTS_GCCALL) -g -pg
#CCOPTS= $(CCOPTS_GCCALL) -g -Wstrict-aliasing -Wall

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

LDFLAGS= -m -R$(LIBDIR)/$(OFD):$(LIBDIR)


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
NMFLAGS= -xs -v

CXREF= cxref
CXREFFLAGS= -R -s

CPP= cpp

LORDER= lorder
TSORT= tsort

RM= rm -f


INCS= config.h defs.h


OBJ00= main.o whatinfo.o proginfo.o proginfo_setpiv.o
OBJ01= 
OBJ02= debug.o
OBJ03= starmat.o
OBJ04= hostent.o inetaddr.o sockaddress.o
OBJ05= timestr_date.o
OBJ06= systems.o ns.o dialer.o cm.o
OBJ07= printhelp.o
OBJ08=
OBJ09= dialuss.o dialussmux.o dialussnls.o
OBJ10= dialtcp.o dialtcpmux.o dialtcpnls.o
OBJ11= dialticotsord.o dialticotsordnls.o 
OBJ12=
OBJ13= ut_open.o ut_close.o ut_bind.o ut_connect.o ut_alloc.o ut_free.o
OBJ14= ut_look.o ut_sync.o
OBJ15= u_connect.o

OBJA= $(OBJ00) $(OBJ01) $(OBJ02) $(OBJ03) $(OBJ04) $(OBJ05) $(OBJ06) $(OBJ07)
OBJB= $(OBJ08) $(OBJ09) $(OBJ10) $(OBJ11) $(OBJ12) $(OBJ13) $(OBJ14) $(OBJ15)

OBJ= $(OBJA) $(OBJB)

OBJS= $(CRT0) $(OBJ) $(CRTC)


SRC= $(OBJ:.c=.o)


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
	makedate > makedate.c
	$(CC) -c $(CFLAGS) makedate.c
	$(LD) -o $@ $(LDFLAGS) $(OBJS) $(LIBDIRS) $(LIBS) > $(T).lm

$(T).$(OFF) $(OFF):	$(T).x
	cp -p $(T).x $(T).$(OFF)

strip:			$(T).x
	strip $(T).x
	rm -f $(T).$(OFF) $(T)

install:		$(ALL)
	makenewer -r $(ALL) $(BINDIR)

install-raw:		$(T).x
	rm -f $(BINDIR)/$(T).$(OFF)
	makenewer -r -o rmsuf $(T).x $(BINDIR)

install-help:		$(T).help
	-mkdir -p $(SRCROOT)/share/help
	makenewer -o rmsuf $(T).help $(HELPDIR)

again:
	rm -f $(ALL) $(T).x

clean:			again
	rm -f *.o

control:
	uname -n > Control
	date >> Control


main.o:			main.c $(INCS) dialer.h systems.h cm.h

whatinfo.o:		whatinfo.c config.h

proginfo.o:		proginfo.c $(INCS)


systems.o:		systems.c systems.h

ns.o:			ns.c ns.h

dialer.o:		dialer.c dialer.h ns.h

cm.o:			cm.c cm.h dialer.h systems.h ns.h


vecstr_loadfile.o:	vecstr_loadfile.c

vecstr_env.o:		vecstr_env.c

streamsync.o:		streamsync.c streamsync.h

envlist.o:		envlist.c envlist.h

hdb.o:			hdb.c hdb.h

lookaside.o:		lookaside.c lookaside.h

pq.o:			pq.c pq.h

hdbstr.o:		hdbstr.c hdbstr.h

hdbstr_loadfile.o:	hdbstr_loadfile.c

msfile.o:		msfile.c msfile.h msfilee.h msflag.h

msfilee.o:		msfilee.c msfilee.h

mapstrint.o:		mapstrint.c mapstrint.h

mkcexsync.o:		mkcexsync.c mkcexsync.h

nodedb.o:		nodedb.c nodedb.h

clusterdb.o:		clusterdb.c clusterdb.h kvsfile.h

svctab.o:		svctab.c svctab.h

strstore.o:		strstore.c strstore.h

serialbuf.o:		serialbuf.c serialbuf.h

kvsfile.o:		kvsfile.c kvsfile.h

buffer.o:		buffer.c buffer.h



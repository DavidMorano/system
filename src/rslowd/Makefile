# MAKEFILE

T= rslowd

ALL= $(T) $(T).$(OFF)

#PCS= /usr/add-on/pcs


BINDIR= $(PCS)/bin
INCDIR= $(PCS)/include
LIBDIR= $(PCS)/lib

#LDCRTDIR= /opt/SUNWspro/WS6/lib
#LDCRTDIR= /opt/SUNWspro/SC5.0/lib
#LDCRTDIR= /opt/SUNWspro/SC4.0/lib
#LDCRTDIR= /opt/SUNWspro/lib
LDCRTDIR= $(SRCROOT)/lib


CC= gcc
CCOPTS= -O -mcpu=ultrasparc # -fpic
#CCOPTS= -g -pg
#CCOPTS= -g

# HyperSPARC
#CCOPTS= -xO5 -xtarget=ss20/hs22 -dalign -xdepend

# UltraSPARC
#CCOPTS= -xO5 -xtarget=ultra -xsafe=mem -dalign -xdepend


DEFS0= -D$(OSTYPE)=1 -DSOLARIS=1 
DEFS1= -DPOSIX=1 -D_POSIX_PTHREAD_SEMANTICS=1 -D_REENTRANT=1
DEFS2= -D_POSIX_C_SOURCE=199506L -D_POSIX_PER_PROCESS_TIMER_SOURCE=1
DEFS3= -D__EXTENSIONS__=1
DEFS4=

DEFS= $(DEFS0) $(DEFS1) $(DEFS2) $(DEFS3) $(DEFS4)


INCDIRS= -I$(INCDIR)
CFLAGS= $(CCOPTS) $(DEFS) $(INCDIRS)

#LD= $(CC)
#LD= cc
LD= ld

#LDFLAGS= -pg -L/usr/lib/libp -lc # -m -R$(LIBDIR)/$(OFD):$(LIBDIR)
LDFLAGS= -m -R$(LIBDIR)/$(OFD):$(LIBDIR)


LIBDIRS= -L$(LIBDIR)/$(OFD) -L$(LIBDIR)

LIBS0=
LIBS1= -Bstatic -lpcs -ldam -Bdynamic -lnews
LIBS2=
LIBS3= -Bstatic -lb -luc -Bdynamic
LIBS4= -lu
LIBS5= -L$(GNU)/lib -lgcc
LIBS6= -lgen -lpthread -lrt -lsocket -lnsl
#LIBS6= -lgen -ldl -lpthread -lrt -lsocket -lnsl 
LIBS7= -lc

LIBS= $(LIBS0) $(LIBS1) $(LIBS2) $(LIBS3) $(LIBS4) $(LIBS5) $(LIBS6) $(LIBS7)


CRTI= $(LDCRTDIR)/crti.o
CRT1= $(LDCRTDIR)/crt1.o
CRTN= $(LDCRTDIR)/crtn.o

CRT0= $(CRTI) $(CRT1)
CRTC= makedate.o

LINT= lint
LINTFLAGS= -uxn -Dlint $(DEFS) $(INCDIRS)

CPP= cpp



OBJ0=
OBJ1= main.o watch.o handle.o handlejob.o
OBJ2= checklockfile.o configfile.o srvfile.o
OBJ3= debug.o
OBJ4= 
OBJ5= path.o jobdb.o hostequiv.o
OBJ6= msg.o msgheaders.o sortlist.o
OBJ7=

OBJA= $(OBJ0) $(OBJ1) $(OBJ2) $(OBJ3) $(OBJ4) $(OBJ5) $(OBJ6) $(OBJ7) 

OBJ= $(OBJA) $(OBJB)

OBJS= $(CRT0) $(OBJ) $(CRTC)




#.SUFFIXES:	.t



default:		$(T).x

all:			$(ALL)

.c.o:
	$(CC) -c $(CFLAGS) $<

.c.ln:
	$(LINT) -u $(DEFS) $(INCDIRS) $<


$(T):			$(T).ee
	cp -p $(T).ee $(T)

$(T).prof:			$(OBJ) Makefile mcrt1.o
	makedate -m $(T) > makedate.c
	$(CC) -c makedate.c
	$(LD) -o $@ $(LDFLAGS) $(OBJS) $(LIBDIRS) $(LIBS)

$(T).x:			$(OBJ) Makefile
	makedate -m $(T) > makedate.c
	$(CC) -c makedate.c
	$(LD) -o $@ $(LDFLAGS) $(OBJS) $(LIBDIRS) $(LIBS) $(CRTN) > $(T).lm

$(T).$(OFF) $(OFF):	$(T).x
	cp -p $(T).x $(T).$(OFF)

strip:			$(T).x
	strip $(T).x
	rm -f $(T).$(OFF) $(T)

install:		$(ALL)
	bsdinstall $(ALL) $(BINDIR)

install-raw:		$(T).x
	cp -p $(T).x $(T)
	rm -f $(BINDIR)/$(T).$(OFF)
	bsdinstall $(T) $(BINDIR)

bbpost:			bbfile.o
	cp -p bbpost.sh bbpost

bbfile:			bbfile.o
	$(LD) -o $@ $(OBJ) $(LIBDIRS) $(LIBS)

again:
	rm -fr $(ALL) $(T).x

clean:			again
	rm -f *.o

control:
	uname -n > Control
	date >> Control

bb.cflow:
	cflow $(INCDIRS) *.c > $@



mcrt1.o:		mcrt1.save
	cp -p mcrt1.save mcrt1.o

bbfile.o:
	$(CC) -c $(CFLAGS) bbfile.c


main.o:			main.c config.h defs.h

whatinfo.o:		whatinfo.c config.h

emit_item.o:		emit_item.c defs.h headerkeys.h

bbnewsrc.o:		bbnewsrc.c bbnewsrc.h

get_bds.o:		get_bds.c bbnewsrc.h

update.o:		update.c bbnewsrc.h

pcspoll.o:		pcspoll.c


dirlist.t:		dirlist.c





# MAKEFILE (libpretime)

T= libpretime

ALL= $(T).so

SRCROOT= $(EXTRA)


BINDIR= $(SRCROOT)/bin
INCDIR= $(SRCROOT)/include
LIBDIR= $(SRCROOT)/lib

LDCRTDIR= $(CGS_CRTDIR)
VALDIR= $(CGS_VALDIR)


CC= $(CGS_CC)
GPP= $(CGS_GPP)

GPPFLAGS= $(CGS_CXXFLAGS)

CCOPTS= $(CCOPTS_GCCOPT) $(CCOPTS_GCCALL) $(CCOPTS_GCCLIB)
#CCOPTS= $(CCOPTS_GCCALL) $(CCOPTS_GCCLIB)
#CCOPTS= -g -Wstrict-aliasing $(CCOPTS_GCCALL) $(CCOPTS_GCCLIB) 


LDA=
LDA += -z initarray=pretime_init -z finiarray=pretime_fini


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

CFLAGS= $(CCOPTS) $(LF_CFLAGS)

#LD= $(CC)


LDRPATH= $(EXTRA}/lib


RLDFLAGS=


SLDFLAGS= $(LF_LDFLAGS) $(LDA)

SLIBDIRS= -L$(LIBDIR) -L$(CGS_LIBDIR)

SLIB0=
SLIB1= -ldam
SLIB2=
SLIB3= -luc -lu
SLIB4= $(CGS_LIBS)
SLIB5= $(LF_LIBS) 
SLIB6= -lrt
SLIB7= -ldl

SLIBS= $(SLIB0) $(SLIB1) $(SLIB2) $(SLIB3) $(SLIB4) $(SLIB5) $(SLIB6) $(SLIB7)

SLIBINFO= $(SLIBDIRS) $(SLIBS)


XLIBDIRS= -L$(LIBDIR) -L$(CGS_LIBDIR)

XLIB0=
XLIB1= -ldam -lb
XLIB2= -ldebug
XLIB3= -lut -luc -lu
XLIB4= $(CGS_LIBS)
XLIB5=  $(LF_LIBS)
XLIB6= -lsecdb -lproject -lpthread -lrt -lxnet -lsocket -lnsl
XLIB7= -ldl

XLIBS= $(XLIB0) $(XLIB1) $(XLIB2) $(XLIB3) $(XLIB4) $(XLIB5) $(XLIB6) $(XLIB7)

XLIBINFO= $(XLIBDIRS) $(XLIBS)


LINT= lint
LINTFLAGS= $(LF_LINTFLAGS) -uxn -Dlint

NM= nm
NMFLAGS= -sx -v

CPP= cpp

LORDER= lorder
TSORT= ${CCS}/bin/tsort

RM= rm -f
TOUCH= /bin/touch


INCS= vsystem.h localmisc.h


OBJ00= pretime.o
OBJ01= time.o
OBJ02= gettimeofday.o
OBJ03= ftime.o
OBJ04= clock.o
OBJ05=
OBJ06=
OBJ07=

OBJ08= tmz.o
OBJ09=
OBJ10=
OBJ11=
OBJ12=
OBJ13=
OBJ14=
OBJ15=

OBJ16=
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

OBJ48=
OBJ49=
OBJ50=
OBJ51=
OBJ52= 
OBJ53=
OBJ54=
OBJ55=

OBJ56=
OBJ57= 
OBJ58=
OBJ59=
OBJ60=
OBJ61=
OBJ62=
OBJ63=

OBJ64=
OBJ65=
OBJ66=
OBJ67=
OBJ68=
OBJ69=
OBJ70=
OBJ71=

OBJ72=
OBJ73=
OBJ74=
OBJ75=
OBJ76=
OBJ77=
OBJ78=
OBJ79=

OBJ80=
OBJ81=
OBJ82=
OBJ83=
OBJ84=
OBJ85=
OBJ86=
OBJ87=

OBJ88=
OBJ89=
OBJ90=
OBJ91=
OBJ92=
OBJ93=
OBJ94=
OBJ95=

OBJA= $(OBJ00) $(OBJ01) $(OBJ02) $(OBJ03) $(OBJ04) $(OBJ05) $(OBJ06) $(OBJ07) 
OBJB= $(OBJ08) $(OBJ09) $(OBJ10) $(OBJ11) $(OBJ12) $(OBJ13) $(OBJ14) $(OBJ15)
OBJC= $(OBJ16) $(OBJ17) $(OBJ18) $(OBJ19) $(OBJ20) $(OBJ21) $(OBJ22) $(OBJ23)
OBJD= $(OBJ24) $(OBJ25) $(OBJ26) $(OBJ27) $(OBJ28) $(OBJ29) $(OBJ30) $(OBJ31)
OBJE= $(OBJ32) $(OBJ33) $(OBJ34) $(OBJ35) $(OBJ36) $(OBJ37) $(OBJ38) $(OBJ39)
OBJF= $(OBJ40) $(OBJ41) $(OBJ42) $(OBJ43) $(OBJ44) $(OBJ45) $(OBJ46) $(OBJ47)
OBJG= $(OBJ48) $(OBJ49) $(OBJ50) $(OBJ51) $(OBJ52) $(OBJ53) $(OBJ54) $(OBJ55)
OBJH= $(OBJ56) $(OBJ57) $(OBJ58) $(OBJ59) $(OBJ60) $(OBJ61) $(OBJ62) $(OBJ63)
OBJI= $(OBJ64) $(OBJ65) $(OBJ66) $(OBJ67) $(OBJ68) $(OBJ69) $(OBJ70) $(OBJ71)
OBJJ= $(OBJ72) $(OBJ73) $(OBJ74) $(OBJ75) $(OBJ76) $(OBJ77) $(OBJ78) $(OBJ79)
OBJK= $(OBJ80) $(OBJ81) $(OBJ82) $(OBJ83) $(OBJ84) $(OBJ85) $(OBJ86) $(OBJ87)
OBJL= $(OBJ88) $(OBJ89) $(OBJ90) $(OBJ91) $(OBJ92) $(OBJ93) $(OBJ94) $(OBJ95)

OBJg0= $(OBJA) $(OBJB) $(OBJC) $(OBJD) $(OBJE) $(OBJF) $(OBJG) $(OBJH) 
OBJg1= $(OBJI) $(OBJJ) $(OBJK) $(OBJL)

OBJ= $(OBJg0) $(OBJg1)

#OBJS0= obja.o objb.o objc.o
OBJS0= obja.o objb.o
#OBJS0= obja.o objb.o objc.o objd.o obje.o objf.o objg.o objh.o obji.o objj.o
#OBJS1= objk.o objl.o

OBJS= $(OBJS0) $(OBJS1)

TOBJ00=

#TOBJ01a=
TOBJ01b=

TOBJ01= $(TOBJ01a) $(TOBJ01b) $(TOBJ01c) $(TOBJ01d)

#TOBJA= $(OBJ00) $(OBJ01) $(OBJ02) $(OBJ03) $(OBJ04) $(OBJ05) $(OBJ06) $(OBJ07) 
#TOBJA= $(OBJ00) $(OBJ01) $(OBJ02) $(OBJ03) 
# working
#TOBJA= $(OBJ02) $(OBJ03) 
# not-working
#TOBJA= $(OBJ00) $(OBJ01) 
# not-working
TOBJA= $(TOBJ01) 
# working
#TOBJA= $(TOBJ00) 

#TOBJG0= $(OBJA) $(OBJB) $(OBJC) $(OBJD) $(OBJE) $(OBJF) $(OBJG) $(OBJH) 
#TOBJG0= $(OBJE) $(OBJF) $(OBJG) $(OBJH) 
#TOBJG0= $(OBJA) $(OBJB) $(OBJC) $(OBJD) 
#TOBJG0= $(OBJA) $(OBJB) 
#TOBJG0= $(OBJB) 
TOBJG0= $(TOBJA) 
#TOBJG0= $(OBJC) $(OBJD) 
#TOBJG1= $(OBJI) $(OBJJ) $(OBJK) $(OBJL) $(OBJM) $(OBJN) $(OBJO) $(OBJP)
#TOBJG1= $(OBJI) $(OBJJ) $(OBJK) $(OBJL) 
#TOBJG1= $(OBJM) $(OBJN) $(OBJO) $(OBJP)

TOBJ= $(TOBJG0) $(TOBJG1)


.SUFFIXES:		.ls .i .cx .cs


default:		all

all:			$(ALL)

.cc.o:
	$(GPP) -c $(GPPFLAGS) $(CFLAGS) $(CPPFLAGS) $<

.c.o:
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $<

.c.ln:
	$(LINT) -c $(LINTFLAGS) $(CPPFLAGS) $<

.c.ls:
	$(LINT) $(LINTFLAGS) $(CPPFLAGS) $<

.c.i:
	$(CPP) $(CPPFLAGS) $< > $(*).i


$(TT).a:		$(TOBJ)
	$(AR) -rc $@ $?

$(T).so:		$(OBJS) Makefile
	$(GXX) -shared -o $@ $(SLDFLAGS) $(OBJS) $(SLIBDIRS) $(SLIBS)

$(T).a:			$(OBJ)
	$(AR) -rc $@ $?

$(T).nm:		$(T).so
	$(NM) $(NMFLAGS) $(T).so > $(T).nm

$(T).order order:	$(OBJ) $(T).a
	$(LORDER) $(T).a | $(TSORT) > $(T).order
	$(RM) $(T).a
	while read O ; do $(AR) -cr $(T).a $${O} ; done < $(T).order

intall-pre:
	filefind . -s h | makenewer -af - -d $(INCDIR)

install:		$(ALL) Makefile install-incs
	ranlib $(T).a
	makenewer -r $(ALL) $(LIBDIR)
	makeinstall $(T)

install-incs:
	makenewer -im -z -m o=h $(OBJA) -d $(INCDIR)
	makenewer -im -z -m o=h $(OBJB) -d $(INCDIR)
	makenewer -im -z -m o=h $(OBJC) -d $(INCDIR)
	makenewer -im -z -m o=h $(OBJD) -d $(INCDIR)
	makenewer -im -z -m o=h $(OBJE) -d $(INCDIR)
	makenewer -im -z -m o=h $(OBJF) -d $(INCDIR)
	makenewer -im -z -m o=h $(OBJG) -d $(INCDIR)
	makenewer -im -z -m o=h $(OBJH) -d $(INCDIR)
	makenewer -im -z -m o=h $(OBJI) -d $(INCDIR)
	makenewer -im -z -m o=h $(OBJJ) -d $(INCDIR)
	makenewer -im -z -m o=h $(OBJK) -d $(INCDIR)
	makenewer -im -z -m o=h $(OBJL) -d $(INCDIR)

safe:
	makesafe -z -v=3 -I $(INCDIR) $(OBJA)
	makesafe -z -v=3 -I $(INCDIR) $(OBJB)
	makesafe -z -v=3 -I $(INCDIR) $(OBJC)
	makesafe -z -v=3 -I $(INCDIR) $(OBJD)
	makesafe -z -v=3 -I $(INCDIR) $(OBJE)
	makesafe -z -v=3 -I $(INCDIR) $(OBJF)
	makesafe -z -v=3 -I $(INCDIR) $(OBJG)
	makesafe -z -v=3 -I $(INCDIR) $(OBJH)
	makesafe -z -v=3 -I $(INCDIR) $(OBJI)
	makesafe -z -v=3 -I $(INCDIR) $(OBJJ)
	makesafe -z -v=3 -I $(INCDIR) $(OBJK)
	makesafe -z -v=3 -I $(INCDIR) $(OBJL)

again:
	rm -f $(ALL)

clean:
	rm -f $(ALL) *.o *.i *.ls *.ln *.lm

control:
	(uname -n ; date) > Control


obja.o:			$(OBJA)
	$(LD) -r -o $@ $(RLDFLAGS) $(OBJA)

objb.o:			$(OBJB)
	$(LD) -r -o $@ $(RLDFLAGS) $(OBJB)

objc.o:			$(OBJC)
	$(LD) -r -o $@ $(RLDFLAGS) $(OBJC)

objd.o:			$(OBJD)
	$(LD) -r -o $@ $(RLDFLAGS) $(OBJD)

obje.o:			$(OBJE)
	$(LD) -r -o $@ $(RLDFLAGS) $(OBJE)

objf.o:			$(OBJF)
	$(LD) -r -o $@ $(RLDFLAGS) $(OBJF)

objg.o:			$(OBJG)
	$(LD) -r -o $@ $(RLDFLAGS) $(OBJG)

objh.o:			$(OBJH)
	$(LD) -r -o $@ $(RLDFLAGS) $(OBJH)

obji.o:			$(OBJI)
	$(LD) -r -o $@ $(RLDFLAGS) $(OBJI)

objj.o:			$(OBJJ)
	$(LD) -r -o $@ $(RLDFLAGS) $(OBJJ)

objk.o:			$(OBJK)
	$(LD) -r -o $@ $(RLDFLAGS) $(OBJK)

objl.o:			$(OBJL)
	$(LD) -r -o $@ $(RLDFLAGS) $(OBJL)


# library private subroutine modules

pretime.o:		pretime.c pretime.h

time.o:			time.c pretime.h

gettimeofday.o:		gettimeofday.c pretime.h

ftime.o:		ftime.c pretime.h

clock.o:		clock.c pretime.h


# testing

X01= testugetpw

X01OBJ= $(X01).o ugetpw.o upwcache.o recarr.o

$(X01).o:		$(X01).c ugetpw.h upwcache.h recarr.h

$(X01).x:		$(X01OBJ)
	$(CC) -o $@ $(X01OBJ) $(XLIBDIRS) $(XLIBS)


X02= testutmpacc

X02OBJ= $(X02).o utmpacc.o utmpaccent.o recarr.o

$(X02).o:		$(X02).c utmpacc.h utmpaccent.h recarr.h

$(X02).x:		$(X02OBJ)
	$(CC) -o $@ $(X02OBJ) $(XLIBDIRS) $(XLIBS)


XDIAL0= dialticotsordnls.o dialticotsordmux.o dialticotsord.o 
XDIAL1= opensysfs.o
XDIAL2= nlsdialassist.o strnwcpy.o
XDIAL3= snopenflags.o
XDIAL4= uc_reade.o uc_openuser.o uc_opensys.o uc_opendev.o

XDIALERS= $(XDIAL0) $(XDIAL1) $(XDIAL2) $(XDIAL3) $(XDIAL4)


X03= testucopen

X03OBJ= $(X03).o uc_openinfo.o uc_openproto.o uc_openprog.o $(XDIALERS)

$(X03).o:		$(X03).c vsystem.h nlsdialassist.h

$(X03).x:		$(X03OBJ)
	$(CC) -o $@ $(X03OBJ) $(XLIBDIRS) $(XLIBS)



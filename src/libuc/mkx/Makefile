# MAKEFILES (mkx)

T= mkx

ALL= $(T).o $(T).a


BINDIR= $(REPOROOT)/bin
INCDIR= $(REPOROOT)/include
LIBDIR= $(REPOROOT)/lib
MANDIR= $(REPOROOT)/man
INFODIR= $(REPOROOT)/info
HELPDIR= $(REPOROOT)/share/help


CRTDIR= $(CGS_CRTDIR)
VALDIR= $(CGS_VALDIR)
LIBDIR= $(CGS_LIBDIR)

CPP= cpp
CC= gcc
CXX= gpp
LD= gld
RANLIB= granlib
AR= gar
NM= gnm
COV= gcov

LORDER= lorder
TSORT= tsort
LINT= lint
RM= rm -f
TOUCH= touch
LINT= lint


DEFS=

LDRPATH= $(EXTRA)/lib

LIBDIRS= -L$(LIBDIR)

LIBS=


# flag setting
CPPFLAGS= $(DEFS) $(INCDIRS) $(MAKECPPFLAGS)
CFLAGS= $(MAKECFLAGS)
CCFLAGS= $(MAKECCFLAGS)
ARFLAGS= $(MAKEARFLAGS)
LDFLAGS= $(MAKELDFLAGS)


INCS= mkx.h


OBJ00_MKX= mkaddrname.o mkaddrpart.o mkaltext.o
OBJ01_MKX= mkaudit.o mkbangname.o mkbasename.o mkbestaddr.o
OBJ02_MKX= mkbestfrom.o mkcaselower.o mkcdpath.o mkcexsync.o
OBJ03_MKX= mkchar.o mkcleanline.o
OBJ04_MKX= mkdisphdr.o mkdisplayable.o
OBJ05_MKX= mkexpandpath.o mkfdfname.o mkfingerquery.o 
OBJ06_MKX= mkfmtphone.o mkfname.o mkfnamesuf.o
OBJ07_MKX= mkgecosname.o
OBJ08_MKX= mkintfname.o mklogid.o mkmagic.o
OBJ09_MKX= mkmailbox.o mkmaildirtest.o mkmailname.o mkmid.o
OBJ10_MKX= mkmsgbound.o mknoise.o mknpathx.o mknpathxw.o
OBJ11_MKX= mkonefrom.o mkpathrooted.o mkpathx.o mkpathxw.o
OBJ12_MKX= mkplogid.o mkprogenv.o mkquery.o mkquoted.o
OBJ13_MKX= mkrealname.o mkreplyaddr.o mkrevstr.o mksenderaddr.o
OBJ14_MKX= mkshlibname.o mksofname.o mkstrunique.o mksublogid.o
OBJ15_MKX= mktagfname.o mktagprint.o
OBJ16_MKX= mktmplock.o mktmpuserdir.o mkufname.o
OBJ17_MKX= mkuibang.o mkuiname.o mkuserpath.o mkutmpid.o
OBJ18_MKX= mkuuid.o mkvarpath.o mkvarpath.o

OBJA_MKX= obj00_mkx.o obj11_mkx.o obj02_mkx.o obj03_mkx.o
OBJB_MKX= obj04_mkx.o obj05_mkx.o obj06_mkx.o obj07_mkx.o
OBJC_MKX= obj08_mkx.o obj09_mkx.o obj10_mkx.o obj11_mkx.o
OBJD_MKX= obj12_mkx.o obj13_mkx.o obj14_mkx.o obj15_mkx.o
OBJE_MKX= obj16_mkx.o obj17_mkx.o obj18_mkx.o 

OBJ_MKX= obja_mkx.o objb_mkx.o objc_mkx.o 0bjd_mlx.o obje_mkx.o


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


$(T).o:			$(OBJ_MKX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ_MKX)

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


mkfdfname.o:		mkfdfname.cc mkfdfname.h
mkgecosname.o:		mkgecosname.cc mkgecosname.h




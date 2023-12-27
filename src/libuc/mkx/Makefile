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
RANLIB= $(CGS_GRANLIB)
AR= $(CGS_GAR)
NM= $(CGS_GNM)
COV= $(CGS_GCOV)

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


INCS= usystem.h localmisc.h snx.h


OBJ00_MKX= mkaddrname.o mkaddrpart.o mkaltext.o mkartfile.o
OBJ01_MKX= mkaudit.o mkbangname.o mkbasename.o mkbestaddr.o
OBJ02_MKX= mkbestfrom.o mkcaselower.o mkcdpath.o mkcexsync.o
OBJ03_MKX= mkchar.o mkcleanline.o mkcmds.o mkdatefile.o
OBJ04_MKX= mkdirlist.o mkdisphdr.o mkdisplayable.o mkenvfrom.o
OBJ05_MKX= mkexpandpath.o mkface.o mkfdfname.o mkfingerargs.o
OBJ06_MKX= mkfingerquery.o mkfmtphone.o mkfname.o mkfnamesuf.o
OBJ07_MKX= mkfromaddr.o mkfromname.o mkgecosname.o mkhashfile.o
OBJ08_MKX= mkintfname.o mkjobfile.o mklogid.o mkmagic.o
OBJ09_MKX= mkmailbox.o mkmaildirtest.o mkmailname.o mkmid.o
OBJ10_MKX= mkmsgbound.o mknoise.o mknpathx.o mknpathxw.o
OBJ11_MKX= mkonefrom.o mkpathrooted.o mkpathx.o mkpathxw.o
OBJ12_MKX= mkplogid.o mkprogenv.o mkquery.o mkquoted.o
OBJ13_MKX= mkrealname.o mkreplyaddr.o mkrevstr.o mksenderaddr.o
OBJ14_MKX= mkshlibname.o mksofname.o mkstrunique.o mksublogid.o
OBJ15_MKX= mktagfname.o mktagprint.o mktmpdir.o mktmpfile.o
OBJ16_MKX= mktmplock.o mktmpuserdir.o mktxtindexcmds.o mkufname.o
OBJ17_MKX= mkuibang.o mkuiname.o mkuserpath.o mkutmpid.o
OBJ18_MKX= mkuuid.o mkvarpath.o mkvarpath.o

OBJA_MKX= obj00_sfx.o obj11_sfx.o obj02_sfx.o obj03_sfx.o
OBJB_MKX= obj04_sfx.o obj05_sfx.o obj06_sfx.o obj07_sfx.o
OBJC_MKX= obj08_sfx.o obj09_sfx.o obj10_sfx.o obj11_sfx.o
OBJD_MKX= obj12_sfx.o obj13_sfx.o obj14_sfx.o obj15_sfx.o
OBJE_MKX= obj16_sfx.o obj17_sfx.o obj18_sfx.o 

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
	rm -f $(TALL)

clean:
	makeclean $(ALL)

control:
	(uname -n ; date) > Control

obj00_sfx.o:	$(OBJ00_MKX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ00_MKX)

obj01_sfx.o:	$(OBJ01_MKX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ01_MKX)

obj02_sfx.o:	$(OBJ02_MKX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ02_MKX)

obj03_sfx.o:	$(OBJ03_MKX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ03_MKX)

obj04_sfx.o:	$(OBJ04_MKX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ04_MKX)

obj05_sfx.o:	$(OBJ05_MKX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ05_MKX)

obj06_sfx.o:	$(OBJ06_MKX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ06_MKX)

obj07_sfx.o:	$(OBJ07_MKX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ07_MKX)

obj08_sfx.o:	$(OBJ08_MKX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ08_MKX)

obj09_sfx.o:	$(OBJ09_MKX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ09_MKX)

obj10_sfx.o:	$(OBJ10_MKX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ10_MKX)

obj11_sfx.o:	$(OBJ11_MKX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ11_MKX)

obj12_sfx.o:	$(OBJ12_MKX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ12_MKX)

obj13_sfx.o:	$(OBJ13_MKX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ13_MKX)

obj14_sfx.o:	$(OBJ14_MKX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ14_MKX)

obj15_sfx.o:	$(OBJ15_MKX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ15_MKX)

obj16_sfx.o:	$(OBJ16_MKX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ16_MKX)

obj17_sfx.o:	$(OBJ17_MKX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ17_MKX)

obj18_sfx.o:	$(OBJ18_MKX)
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


mkdatefile.o:		mkdatefile.cc mkdatefile.h



# MAKEFILE (libuc)

T= libuc

ALL= $(T).o $(T).so $(T).a


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


DEFS +=

INCS += usystem.h localmisc.h
INCS += uclibsubs.h

MODS += bstree.ccm sview.ccm strfilter.ccm
MODS += mapblock.ccm memtrack.ccm addrset.ccm
MODS += sif.ccm

LIBS= -lu


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


OBJ00_MOD += valuelims.o
OBJ01_MOD += uconstants.o ulibvals.o
OBJ02_MOD +=
OBJ03_MOD += bstree.o sview.o
OBJ04_MOD += mapblock.o memtrack.o addrset.o
OBJ05_MOD += strfilter.o
OBJ06_MOD +=

OBJA_MOD= obj00_mod.o obj01_mod.o obj02_mod.o obj03_mod.o
OBJB_MOD= obj04_mod.o

OBJ_MOD= obja_mod.o objb_mod.o

OBJ00_INIT=
OBJ01_INIT=
OBJ02_INIT=
OBJ03_INIT=

OBJ04_INIT=
OBJ05_INIT=
OBJ06_INIT=
OBJ07_INIT=

OBJ00= matxstr.o toxc.o char.o 
OBJ01= strn.o strnxcmp.o
OBJ02= snwcpy.o strdcpy.o
OBJ03= strw.o strx.o mnw.o

OBJ04= isx.o
OBJ05= nleadx.o
OBJ06= mapex.o
OBJ07=

OBJ08= strkeyx.o
OBJ09=
OBJ10= cfx.o memtrack.o mapblock.o
OBJ11= field.o

OBJ12=
OBJ13=
OBJ14=
OBJ15=

OBJ16 = six.o snx.o sfx.o sif.o rmx.o
OBJ17 = wsix.o wsnx.o wsfx.o
OBJ18 = 
OBJ19 = 

OBJ20= hdb.o hdbstr.o ccfile.o
OBJ21=
OBJ22=
OBJ23=

OBJ24=
OBJ25=
OBJ26=
OBJ27=

OBJ28= ucttyname.o uctc.o ucsysconf.o
OBJ29=
OBJ30= ucsys.o
OBJ31= tcx.o

OBJ=
OBJ += $(OBJ00) $(OBJ01) $(OBJ02) $(OBJ03) 
OBJ += $(OBJ04) $(OBJ05) $(OBJ06) $(OBJ07)
OBJ += $(OBJ08) $(OBJ09) $(OBJ10) $(OBJ11)
OBJ += $(OBJ12) $(OBJ13) $(OBJ14) $(OBJ15)
OBJ += $(OBJ16) $(OBJ17) $(OBJ18) $(OBJ19)


.SUFFIXES:		.hh .ii .ccm .ccm


default:		$(T).o

all:			$(ALL)

so:			$(T).so

a:			$(T).a


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


$(T).so:		$(OBJ) Makefile
	$(LD) -shared -o $@ $(LDFLAGS) $(OBJ) $(LIBINFO)

$(T).o:			$(OBJ)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJ) $(LIBINFO)

$(T).a:			$(OBJ)
	$(AR) -rc $@ $?

$(T).nm:		$(T).o
	$(NM) $(NMFLAGS) $(T).o > $(T).nm

$(T).order:		$(OBJ) $(T).a
	$(LORDER) $(T).a | $(TSORT) > $(T).order
	$(RM) $(T).a
	while read O ; do $(AR) -cr $(T).a $${O} ; done < $(T).order

intall-pre:
	filefind . -s h | makenewer -af - -d $(INCDIR)

install:		$(ALL) Makefile install-incs
	ranlib $(T).a
	makenewer -r $(ALL) $(LIBDIR)

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
	makenewer -im -z -m o=h $(OBJM) -d $(INCDIR)
	makenewer -im -z -m o=h $(OBJN) -d $(INCDIR)
	makenewer -im -z -m o=h $(OBJO) -d $(INCDIR)
	makenewer -im -z -m o=h $(OBJP) -d $(INCDIR)
	makenewer -im -z -m o=h $(OBJQ) -d $(INCDIR)
	makenewer -im -z -m o=h $(OBJR) -d $(INCDIR)
	makenewer -im -z -m o=h $(OBJS) -d $(INCDIR)
	makenewer -im -z -m o=h $(OBJT) -d $(INCDIR)

safe:
	makesafe -v=3 -I $(INCDIR) $(OBJA)
	makesafe -v=3 -I $(INCDIR) $(OBJB)
	makesafe -v=3 -I $(INCDIR) $(OBJC)
	makesafe -v=3 -I $(INCDIR) $(OBJD)
	makesafe -v=3 -I $(INCDIR) $(OBJE)
	makesafe -v=3 -I $(INCDIR) $(OBJF)
	makesafe -v=3 -I $(INCDIR) $(OBJG)
	makesafe -v=3 -I $(INCDIR) $(OBJH)
	makesafe -v=3 -I $(INCDIR) $(OBJI)
	makesafe -v=3 -I $(INCDIR) $(OBJJ)
	makesafe -v=3 -I $(INCDIR) $(OBJK)
	makesafe -v=3 -I $(INCDIR) $(OBJL)
	makesafe -v=3 -I $(INCDIR) $(OBJM)
	makesafe -v=3 -I $(INCDIR) $(OBJN)
	makesafe -v=3 -I $(INCDIR) $(OBJO)
	makesafe -v=3 -I $(INCDIR) $(OBJP)
	makesafe -v=3 -I $(INCDIR) $(OBJQ)
	makesafe -v=3 -I $(INCDIR) $(OBJR)
	makesafe -v=3 -I $(INCDIR) $(OBJS)
	makesafe -v=3 -I $(INCDIR) $(OBJT)

again:
	rm -f $(ALL)

clean:
	makeclean $(ALL)

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

objm.o:			$(OBJM)
	$(LD) -r -o $@ $(RLDFLAGS) $(OBJM)

objn.o:			$(OBJN)
	$(LD) -r -o $@ $(RLDFLAGS) $(OBJN)

objo.o:			$(OBJO)
	$(LD) -r -o $@ $(RLDFLAGS) $(OBJO)

objp.o:			$(OBJP)
	$(LD) -r -o $@ $(RLDFLAGS) $(OBJP)

objq.o:			$(OBJQ)
	$(LD) -r -o $@ $(RLDFLAGS) $(OBJQ)

objr.o:			$(OBJR)
	$(LD) -r -o $@ $(RLDFLAGS) $(OBJR)

objs.o:			$(OBJS)
	$(LD) -r -o $@ $(RLDFLAGS) $(OBJS)

objt.o:			$(OBJT)
	$(LD) -r -o $@ $(RLDFLAGS) $(OBJT)

obju.o:			$(OBJU)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJU)

objv.o:			$(OBJV)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJV)

objw.o:			$(OBJW)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJW)

objx.o:			$(OBJX)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJX)

objy.o:			$(OBJY)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJY)

objz.o:			$(OBJZ)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJZ)


# base
OBJ0_BASE= uctimeout.o
OBJ1_BASE= ucsysconf.o 
OBJ2_BASE=
OBJ3_BASE=

OBJ_BASE= obj0_base.o obj1_base.o

obj0_base.o:	$(OBJ0_BASE)
	$(LD) -r -o $@ $(OBJ0_BASE)

obj1_base.o:	$(OBJ1_BASE)
	$(LD) -r -o $@ $(OBJ1_BASE)

obj2_base.o:	$(OBJ2_BASE)
	$(LD) -r -o $@ $(OBJ2_BASE)

obj3_base.o:	$(OBJ3_BASE)
	$(LD) -r -o $@ $(OBJ3_BASE)

base.o:		$(OBJ_BASE)
	$(LD) -r -o $@ $(OBJ_BASE)


# CHARACTER
OBJA_CHAR= toxc.o char.o
OBJB_CHAR= hasx.o

OBJ_CHAR= obja_char.o objb_char.o

obja_char.o:	$(OBJA_CHAR)
	$(LD) -r -o $@ $(OBJA_CHAR)

objb_char.o:	$(OBJB_CHAR)
	$(LD) -r -o $@ $(OBJB_CHAR)

character.o:	$(OBJ_CHAR)
	$(LD) -r -o $@ $(OBJ_CHAR)


# LIBUC
ucsysmisc.o:		ucsysmisc.cc
ucnprocs.o:		ucnprocs.cc
ucpathconf.o:		ucpathconf.cc
ucmain.o:		ucmain.cc
ucatfork.o:		ucatfork.cc
ucatexit.o:		ucatexit.cc
ucfork.o:		ucfork.cc
ucgetipnodeby.o:	ucgetipnodeby.cc
ucygetpw.o:		ucygetpw.cc ucygetpw.h ucpwcache.h recarr.h
ucproguser.o:		ucproguser.cc ucproguser.h
ucprogdata.o:		ucprogdata.cc ucprogdata.h
ucclustername.o:	ucclustername.cc ucclustername.h
uclibmemalloc.o:	uclibmemalloc.cc uclibmemalloc.h
ucpwcache.o:		ucpwcache.cc ucpwcache.h recarr.h
ucobjmode.o:		ucobjmode.cc
ucunlink.o:		ucunlink.cc

# UNIX C-language system library string-to-integer interface
# string-to-x
ucstrtox.o:		ucstrtox.cc ucstrtox.h
ucstrtod.o:		ucstrtod.cc ucstrtod.h

# uctimeout (time-out call-backs)
uctimeout.o:		uctimeout.cc

# SYSTEM
ucsysauxinfo.o:		ucsysauxinfo.cc ucsysauxinfo.h

uc_safesleep.o:		uc_safesleep.cc

uc_openinfo.o:		uc_openinfo.c opensysfs.h
uc_openuser.o:		uc_openuser.c opensysfs.h

hostinfo.o:		hostinfo.cc hostinfo.h
hostaddr.o:		hostaddr.cc hostaddr.h
hostent.o:		hostent.cc hostent.h
inetaddr.o:		inetaddr.cc inetaddr.h
sockaddress.o:		sockaddress.cc sockaddress.h

storeitem.o:		storeitem.cc storeitem.h
storebuf.o:		storebuf.cc storebuf.h

varray.o:		varray.cc varray.h

lookaside.o:		lookaside.cc lookaside.h

spawner.o:		spawner.cc spawner.h

nulstr.o:		nulstr.cc nulstr.h
csem.o:			csem.cc csem.h
dirlist.o:		dirlist.cc dirlist.h
randomvar.o:		randomvar.cc randomvar.h

serialbuf.o:		serialbuf.cc serialbuf.h stdorder.h
stdorder.o:		stdorder.cc stdorder.h

mapex.o:		mapex.cc mapex.h
sigevent.o:		sigevent.cc sigevent.h
timeout.o:		timeout.cc timeout.h
upt.o:			upt.cc upt.h
spawnproc.o:		spawnproc.cc spawnproc.h

memfile.o:		memfile.cc memfile.h
filemap.o:		filemap.cc filemap.h

numsign.o:		numsign.cc numsign.h

passwdent.o:		passwdent.cc passwdent.h
spwdent.o:		spwdent.cc spwdent.h
groupent.o:		groupent.cc groupent.h
projectent.o:		projectent.cc projectent.h
userattrent.o:		userattrent.cc userattrent.h

mkprogenv.o:		mkprogenv.cc mkprogenv.h

strcpyxc.o:		strcpyxc.cc strcpyxc.h

opensysfs.o:		opensysfs.cc opensysfs.h

quoteshellarg.o:	quoteshellarg.cc
mkquoted.o:		mkquoted.cc mkquoted.h
termconseq.o:		termconseq.cc termconseq.h
termescseq.o:		termescseq.cc termescseq.h

tmtime.o:		tmtime.cc tmtime.h

strshrink.o:		strshrink.c
 
# digit-character management
ndigit.o:		ndigit.cc ndigit.h
checkbase.o:		mods.o checkbase.cc checkbase.h

# malloc
mallocxx.o:		mallocxx.cc mallocxx.h
mallocstuff.o:		mallocstuff.cc mallocstuff.h

# UNIX C-language system library data-base interface (support)
spwd.o:			spwd.cc spwd.h
userattr.o:		userattr.cc userattr.h
ucpasswd.o:		ucpasswd.cc ucpasswd.h
ucgroup.o:		ucgroup.cc ucgroup.h
ucproject.o:		ucproject.cc ucproject.h

# UNIX C-language system library support
libmallocxx.o:		libmallocxx.cc libmallocxx.h

# UNIX C-language system library memory management
mapblock.o:		mapblock.ccm
	makemodule mapblock

# MEMTRACK
memtrack.o:		memtrack0.o memtrack1.o
	$(LD) -r -o $@ $(LDFLAGS) memtrack0.o memtrack1.o
memtrack0.o:		memtrack.ccm mapblock.ccm
	makemodule mapblock
	makemodule memtrack
memtrack1.o:		memtrack.ccm memtrack1.cc
	makemodule mapblock memtrack
	$(COMPILE.cc) memtrack1.cc

ucmemalloc.o:		ucmemalloc.cc ucmemalloc.h ucmallreg.h
ucmemalloc.o:		addrset.o
ucmallocx.o:		ucmallocx.cc ucmallocx.h

# UNIX C-language system library timer management
uctim.o:		uctim.cc uctim.h itcontrol.h
uctimer.o:		uctimer.cc uctimer.h

# misc-character
toxc.o:			toxc.cc toxc.h
char.o:			char.cc char.h

# UCSUPPORT
ucsupport.o:		ucsupport.dir
ucsupport.dir:
	makesubdir $@

# UCSYSCONF
ucsysconf.o:		ucsysconf.dir
ucsysconf.dir:
	makesubdir $@

# ADDRSET
addrset.o:		addrset.dir
addrset.dir:
	makesubdir $@

# STRWCPY
strwcpy.o:		strwcpy.dir
strwcpy.dir:
	makesubdir $@

# STRW
strw.o:			strw.dir
strw.dir:
	makesubdir $@

# STRDCPY
strdcpy.o:		strdcpy.dir
strdcpy.dir:
	makesubdir $@

open.o:			open.dir
open.dir:
	makesubdir $@

dial.o:			dial.dir
dial.dir:
	makesubdir $@

six.o:			six.dir
six.dir:
	makesubdir $@

sfx.o:			sfx.dir
sfx.dir:
	makesubdir $@

snx.o:			snx.dir
snx.dir:
	makesubdir $@

snwcpy.o:		snwcpy.dir
snwcpy.dir:
	makesubdir $@

# WSNX (Wide-String-NX)
wsnx.o:			wsnx.dir
wsnx.dir:
	makesubdir $@

# WSIX (Wide-String-Index-X)
wsix.o:			wsix.dir
wsix.dir:
	makesubdir $@

# WSFX (Wide-String-Find-X)
wsfx.o:			wsfx.dir
wsfx.dir:
	makesubdir $@

# WSWCPYX (Wide-String-Write-Copy-X)
wswcpyx.o:		wswcpyx.dir
wswcpyx.dir:
	makesubdir $@

# MATXSTR (Match-X-String)
matxstr.o:		matxstr.dir
matxstr.dir:
	makesubdir $@

# HASH
hash.o:			hash.dir
hash.dir:
	makesubdir $@

# UCSYS
ucsys.o:		ucsys.dir
ucsys.dir:
	makesubdir $@

# UCENT
ucent.o:		ucent.dir
ucent.dir:
	makesubdir $@

# UCGET
ucget.o:		ucget.dir
ucget.dir:
	makesubdir $@

# UCENUM
ucenum.o:		ucenum.dir
ucenum.dir:
	makesubdir $@

# UCGETX
ucgetx.o:		ucgetx.dir
ucgetx.dir:
	makesubdir $@

# UCOPEN
ucopen.o:		ucopen.dir
ucopen.dir:
	makesubdir $@

# STRN
strn.o:			strn.dir
strn.dir:
	makesubdir $@

# STRNXCMP
strnxcmp.o:		strnxcmp.dir
strnxcmp.dir:
	makesubdir $@

# STRXCMP
strxcmp.o:		strxcmp.dir
strxcmp.dir:
	makesubdir $@

# STRX
strx.o:			strx.dir
strx.dir:
	makesubdir $@

# STRKEYX
strkeyx.o:		strkeyx.dir
strkeyx.dir:
	makesubdir $@

# VEC
vec.o:			vec.dir
vec.dir:
	makesubdir $@

# MKX
mkx.o:			mkx.dir
mkx.dir:
	makesubdir $@

# MKLOGID
mklogid.o:		mklogid.dir
mklogid.dir:
	makesubdir $@

# MKTMP
mktmp.o:		mktmp.dir
mktmp.dir:
	makesubdir $@

# MKFILE
mkfile.o:		mkfile.dir
mkfile.dir:
	makesubdir $@

# QUEUE
queue.o:		queue.dir
queue.dir:
	makesubdir $@

# CFX
cfx.o:			cfx.dir
cfx.dir:
	makesubdir $@

# CTX
ctx.o:			ctx.dir
ctx.dir:
	makesubdir $@

# CHRONO
chrono.o:		chrono.dir
chrono.dir:
	makesubdir $@

# DATER
dater.o:		dater.dir
dater.dir:
	makesubdir $@

# SBUF
sbuf.o:			sbuf.dir
sbuf.dir:
	makesubdir $@

# BUFFER
buffer.o:		buffer.dir
buffer.dir:
	makesubdir $@

# ISMISC
ismisc.o:		ismisc.dir
ismisc.dir:
	makesubdir $@

# ISFILEDESC
isfiledesc.o:		isfiledesc.dir
isfiledesc.dir:
	makesubdir $@

# GETX
getx.o:			getx.dir
getx.dir:
	makesubdir $@

# GETXX
getxx.o:		getxx.dir
getxx.dir:
	makesubdir $@

# FIELD
field.o:		field.dir
field.dir:
	makesubdir $@

# ENVS
envs.o:			envs.dir
envs.dir:
	makesubdir $@

# ENVX
envx.o:			envx.dir
envx.dir:
	makesubdir $@

# FILER
filer.o:		filer.dir
filer.dir:
	makesubdir $@

# LINER
liner.o:		liner.dir
liner.dir:
	makesubdir $@

# STRPACK
strpack.o:		strpack.dir
strpack.dir:
	makesubdir $@

# HDB
hdb.o:			hdb.dir
hdb.dir:
	makesubdir $@

# HDBSTR
hdbstr.o:		hdbstr.dir
hdbstr.dir:
	makesubdir $@

# TMPX
tmpx.o:			tmpx.dir
tmpx.dir:
	makesubdir $@

# TIMESTR
timestr.o:		timestr.dir
timestr.dir:
	makesubdir $@

# POW
pow.o:			pow.dir
pow.dir:
	makesubdir $@

# UTMPXS
utmpxx.o:		utmpxx.dir
utmpxx.dir:
	makesubdir $@

# GETUTMP
getutmp.o:		getutmp.dir
getutmp.dir:
	makesubdir $@

# GETXNAME
getxname.o:		getxname.dir
getxname.dir:
	makesubdir $@

# ISZ
isx.o:			isx.dir
isx.dir:
	makesubdir $@

# SIGX
sigx.o:			sigx.dir
sigx.dir:
	makesubdir $@

# UCLOCK
uclock.o:		uclock.dir
uclock.dir:
	makesubdir $@

# UCSIG
ucsig.o:		ucsig.dir
ucsig.dir:
	makesubdir $@

# SIGOBJ
sigobj.o:		sigobj.dir
sigobj.dir:
	makesubdir $@

# UTERM
uterm.o:		uterm.dir
uterm.dir:
	makesubdir $@

# SETX
setx.o:			setx.dir
setx.dir:
	makesubdir $@

# SEMX
semx.o:			semx.dir
semx.dir:
	makesubdir $@

# DIRSEEN
dirseen.o:		dirseen.dir
dirseen.dir:
	makesubdir $@

# PATH
path.o:			path.dir
path.dir:
	makesubdir $@

# REALNAME
realname.o:		realname.dir
realname.dir:
	makesubdir $@

# CACHES
caches.o:		caches.dir
caches.dir:
	makesubdir $@

# FSDIR
fsdir.o:		fsdir.dir
fsdir.dir:
	makesubdir $@

# FMTSTR
fmtstr.o:		fmtstr.dir
fmtstr.dir:
	makesubdir $@

# INETADDRX
inetaddrx.o:		inetaddrx.dir
inetaddrx.dir:
	makesubdir $@

# EMA
ema.o:			ema.dir
ema.dir:
	makesubdir $@

# NLEADX
nleadx.o:		nleadx.dir
nleadx.dir:
	makesubdir $@

# SIF
sif.o:			sif.dir
sif.dir:
	makesubdir $@

# STRFILTER
strfilter.o:		strfilter.dir
strfilter.dir:
	makesubdir $@

# OBUF
obuf.o:			obuf.dir
obuf.dir:
	makesubdir $@

# PERMX
permx.o:		permx.dir
permx.dir:
	makesubdir $@

# UCINET
ucinetconv.o:		ucinetconv.cc ucinetconv.h

# tab and character column handling
tabcols.o:		tabcols.cc tabcols.h
ncol.o:			ncol.cc ncol.h tabcols.h
tabexpand.o:		tabexpand.cc tabexpand.h tabcols.h

# INET
inetconv.o:		inetconv.cc inetconv.h

# LIBUC
ucmemla.o:		ucmemla.cc ucmemla.h
ucpts.o:		ucpts.cc ucpts.h

# other
expcook.o:		expcook.cc	expcook.h
keyopt.o:		keyopt.cc	keyopt.h
utf8decoder.o:		utf8decoder.cc	utf8decoder.h
termstr.o:		termstr.cc	termstr.h
td.o:			td.cc		td.h termstr.h
recip.o:		recip.cc	recip.h
querystr.o:		querystr.cc	querystr.h
absfn.o:		absfn.cc	absfn.h
posname.o:		posname.cc	posname.h
ids.o:			ids.cc		ids.h
groupids.o:		groupids.cc	groupids.hh
bits.o:			bits.cc		bits.h
userattrdb.o:		userattrdb.cc	userattrdb.h
pmq.o:			pmq.cc		pmq.h
filegrp.o:		filegrp.cc	filegrp.h
unameo.o:		unameo.cc	unameo.h
hostaddr.o:		hostaddr.cc	hostaddr.h
hostinfo.o:		hostinfo.cc	hostinfo.h
lookaside.o:		lookaside.cc	lookaside.h
expcook.o:		expcook.cc	expcook.h
paramfile.o:		paramfile.cc	paramfile.h
memfile.o:		memfile.cc	memfile.h
dirlist.o:		dirlist.cc	dirlist.h
varray.o:		varray.cc	varray.h
csem.o:			csem.cc		csem.h
pwcache.o:		pwcache.cc	pwcache.h
lockrw.o:		lockrw.cc	lockrw.h
nodedb.o:		nodedb.cc	nodedb.h
ccfile.o:		ccfile.cc	ccfile.hh
thrcomm.o:		thrcomm.cc	thrcomm.h
thrbase.o:		thrbase.cc	thrbase.h thrcomm.h
pwentry.o:		pwentry.cc	pwentry.h
pwfile.o:		pwfile.cc	pwfile.h
bufstr.o:		bufstr.cc	bufstr.h
syspasswd.o:		syspasswd.cc	syspasswd.h
absfn.o:		absfn.cc	absfn.h
outbuf.o:		outbuf.cc	outbuf.h

# sring-comparisons
vstr.o:			vstr.dir
vstr.dir:
	makesubdir $@

# TARDIR
tardir.o:		tardir.dir
tardir.dir:
	makesubdir $@

# RMX
rmx.o:			rmx.dir
rmx.dir:
	makesubdir $@

# BUFSIZE
bufsize.o:		bufsize.dir
bufsize.dir:
	makesubdir $@

# STROBJ
strobj.o:		strobj.dir
strobj.dir:
	makesubdir $@

# HASX
hasx.o:			hasx.dir
hasx.dir:
	makesubdir $@

# MNW
mnw.o:			mnw.dir
mnw.dir:
	makesubdir $@

# DEBUG
debug.o:		debug.dir
debug.dir:
	makesubdir $@

# string-constants
syhsdbfiles.o:		sysdbfiles.c sysdbfiles.h
sysdbfn.o:		sysdbfn.cc sysdbfn.h
opensysdbs.o:		opensysdbs.c opensysdbs.h

# UTILITY
splitfname.o:		splitfname.cc splitfname.h
findbit.o:		findbit.cc findbit.h
termtypemat.o:		termtypemat.cc termtypemat.h
termcmd.o:		termcmd.cc termcmd.h
matparam.o:		matparam.cc matparam.h
typenonpath.o:		typenonpath.cc typenonpath.h
digval.o:		digval.cc digval.h
mkdirs.o:		mkdirs.cc mkdirs.h
strcpyxc.o:		strcpyxc.cc strcpyxc.h
stpcpyxc.o:		stpcpyxc.cc stpcpyxc.h

inetaddrparse.o:	inetaddrparse.cc inetaddrparse.h
readln.o:		readln.cc readln.hh
strwcmp.o:		strwcmp.cc strwcmp.h
isort.o:		isort.cc isort.h
sysnoise.o:		sysnoise.cc sysnoise.h
findfilepath.o:		findfilepath.cc findfilepath.h
findxfile.o:		findxfile.cc findxfile.h
calstrs.o:		calstrs.cc calstrs.h
ipow.o:			ipow.cc ipow.h
base64.o:		base64.cc base64.h
ffbs.o:			ffbs.cc ffbs.h
shellunder.o:		shellunder.cc shellunder.h
itcontrol.o:		itcontrol.cc itcontrol.h
dictdiff.o:		dictdiff.cc dictdiff.h
rsfree.o:		rsfree.cc rsfree.h
xfile.o:		xfile.cc xfile.h
sysmemutil.o:		sysmemutil.cc sysmemutil.h
bitrotate.o:		bitrotate.cc bitrotate.h
nonpath.o:		nonpath.cc	nonpath.h		$(INCS)

# integer-conversion-to-string-digits
strval.o:		uconstants.o strval.cc strval.h

# emulated system kernel calls
uinfo.o:		uinfo.cc uinfo.h
umask.o:		umask.cc umask.h
unameo.o:		unameo.cc unameo.h
ucrand.o:		ucrand.cc ucrand.h
ucprochave.o:		ucprochave.cc ucprochave.h

# environment related string values
strlibval.o:		strlibval.cc strlibval.hh
strenv.o:		strenv.cc strenv.hh

# BSTREE
bstree.o:		bstree.ccm			$(INCS)

# SVIEW
sview.o:		sview.ccm			$(INCS)

# BUFSIZEDATA
bufsizedata.o:		bufsizedata.ccm			$(INCS)

# UCONSTABTS
uconstants.o:		valuelims.o uconstants.ccm


valuelims.o:		valuelims.ccm
ulibvals.o:		ulibvals.ccm


obj00_mod.o:		$(OBJ00_MOD)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJ00_MOD)

obj01_mod.o:		$(OBJ01_MOD)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJ01_MOD)

obj02_mod.o:		$(OBJ02_MOD)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJ02_MOD)

obj03_mod.o:		$(OBJ03_MOD)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJ03_MOD)

obj04_mod.o:		$(OBJ04_MOD)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJ04_MOD)

obj05_mod.o:		$(OBJ05_MOD)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJ05_MOD)

obj06_mod.o:		$(OBJ06_MOD)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJ06_MOD)


obja_mod.o:		$(OBJA_MOD)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJA_MOD)

objb_mod.o:		$(OBJB_MOD)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJB_MOD)


obj_mod.o:		$(OBJ_MOD)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJ_MOD)

mods.o:			$(OBJ_MOD)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJ_MOD)



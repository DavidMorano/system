# MAKEFILE (libuc)

T= libuc

ALL= $(T).so $(T).a


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

LIBS=


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


OBJ00= matxstr.o toxc.o char.o 
OBJ01= strn.o strnxcmp.o sif.o
OBJ02= snwcpy.o strcpyxc.o strwcpy.o strwcmp.o strdcpy.o
OBJ03= stdfnames.o

OBJ04= isnot.o isoneof.o hasx.o ischarx.o 
OBJ05= nleadstr.o nleadkeystr.o
OBJ06= mapex.o getourenv.o strnul.o
OBJ07= sncpyx.o sncpyxw.o snaddw.o

OBJ08= strkeycmp.o ccfile.o strnul.o
OBJ09= readln.o ccfile.o
OBJ10= cfx.o memtrack.o addrset.o mapblock.o
OBJ11= strmgr.o strop.o field.o

OBJ12= ucttyname.o uctc.o ucsysconf.o
OBJ13=
OBJ14= ucsys.o
OBJ15= tcx.o

OBJ16 += wsix.o wsnx.o wsx.o
OBJ17 += six.o snx.o sfx.o rmx.o
OBJ18 +=
OBJ19 +=

OBJ=
OBJ += $(OBJ00) $(OBJ01) $(OBJ02) $(OBJ03) 
OBJ += $(OBJ04) $(OBJ05) $(OBJ06) $(OBJ07)
OBJ += $(OBJ08) $(OBJ09) $(OBJ10) $(OBJ11)
OBJ += $(OBJ12) $(OBJ13) $(OBJ14) $(OBJ15)
OBJ += $(OBJ16) $(OBJ17) $(OBJ18) $(OBJ19)


.SUFFIXES:		.hh .ii


default:		all

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


libmacuser.a:		$(OBJ)
	$(AR) -rc $@ $?

$(T).so:		$(OBJ) Makefile
	$(LD) -G -o $@ $(LDFLAGS) $(OBJ) $(LIBINFO)

$(T).o:			$(OBJ)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJ) $(LIBINFO)

$(T).a:			$(OBJ)
	$(AR) -rc $@ $?

$(T).nm:		$(T).so
	$(NM) $(NMFLAGS) $(T).so > $(T).nm

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


# base
OBJ0_BASE= uctimeout.o
OBJ1_BASE= ucsysconf.o 
OBJ2_BASE= bufsizedata.o getbufsizenames.o
OBJ3_BASE= getbufsize.o bufsizevar.o 

OBJ_BASE= obj0_base.o obj1_base.o obj2_base.o obj3_base.o

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
OBJA_CHAR= toxc.o char.o rmeol.o
OBJB_CHAR= ischarx.o hasx.o

OBJ_CHAR= obja_char.o objb_char.o

obja_char.o:	$(OBJA_CHAR)
	$(LD) -r -o $@ $(OBJA_CHAR)

objb_char.o:	$(OBJB_CHAR)
	$(LD) -r -o $@ $(OBJB_CHAR)

character.o:	$(OBJ_CHAR)
	$(LD) -r -o $@ $(OBJ_CHAR)


# LIBU
ugetloadavg.o:		ugetloadavg.cc ugetloadavg.h

# LIBUC
ucsysmisc.o:		ucsysmisc.cc
ucnprocs.o:		ucnprocs.cc
ucpathconf.o:		ucpathconf.cc
ucmain.o:		ucmain.cc
ucatfork.o:		ucatfork.cc
ucatexit.o:		ucatexit.cc
ucfork.o:		ucfork.cc
ucgetloadavg.o:		ucgetloadavg.cc ucgetloadavg.h
ucygetpw.o:		ucygetpw.cc ucygetpw.h ucpwcache.h recarr.h
ucgetpid.o:		ucgetpid.cc ucgetpid.h
ucproguser.o:		ucproguser.cc ucproguser.h
ucprogdata.o:		ucprogdata.cc ucprogdata.h
ucclustername.o:	ucclustername.cc ucclustername.h
uclibmemalloc.o:	uclibmemalloc.cc uclibmemalloc.h
ucpwcache.o:		ucpwcache.cc ucpwcache.h recarr.h
ucsysconf.o:		ucsysconf.cc ucsysconf.h

# UNIX C-language system library string-to-integer interface
# string-to-x
ucstrtox.o:		ucstrtox.cc ucstrtox.h
ucstrtod.o:		ucstrtod.cc ucstrtod.h

# uctimeout (time-out call-backs)
uctimeout.o:		uctimeout.cc


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


# SYSTEM
ucsysauxinfo.o:		ucsysauxinfo.cc ucsysauxinfo.h
ucgetpuid.o:		ucgetpuid.cc

uc_safesleep.o:		uc_safesleep.c


uc_openinfo.o:		uc_openinfo.c opensysfs.h
uc_openuser.o:		uc_openuser.c opensysfs.h

# FSDIR
posixdirent.o:		posixdirent.cc posixdirent.hh
fsdir.o:		fsdir.cc fsdir.h
sunos_fsdir.o:		sunos_fsdir.cc sunos_fsdir.h posixdirent.hh
darwin_fsdir.o:		darwin_fsdir.cc darwin_fsdir.h posixdirent.hh

# POSIX Thread Attribute
pta.o:			pta.cc pta.h

# POSIX Thread Mutex Attribute
ptma.o:			ptma.cc ptma.h

# POSIX Thread Condition Attribute
ptca.o:			ptca.cc ptca.h

# POSIX Thread Mutex
ptm.o:			ptm.cc ptm.h

# POSIX Thread Condition Variable
ptc.o:			ptc.cc ptc.h

# POSIX Semaphore
psem.o:			psem.cc psem.h

# POSIX Read-Write Lock (broken in dynamically loadable modeules)
ptrwlock.o:		ptrwlock.cc ptrwlock.h

# POSIX Thread Read-Write Lock Attribute
ptrwa.o:		ptrwa.cc ptrwa.h

# POSIX Message Queue
pmq.o:			pmq.cc pmq.h

sighand.o:		sighand.cc sighand.h
sigblock.o:		sigblock.cc sigblock.h
sigign.o:		sigign.cc sigign.h

hostinfo.o:		hostinfo.cc hostinfo.h
hostaddr.o:		hostaddr.cc hostaddr.h
hostent.o:		hostent.cc hostent.h
inetaddr.o:		inetaddr.cc inetaddr.h
sockaddress.o:		sockaddress.cc sockaddress.h

storeitem.o:		storeitem.cc storeitem.h
storebuf.o:		storebuf.cc storebuf.h

obuf.o:			obuf.cc obuf.hh

baops.o:		baops.c baops.h
strop.o:		strop.cc strop.h
dstr.o:			dstr.cc dstr.h

varray.o:		varray.cc varray.h

hdb.o:			hdb.cc hdb.h
lookaside.o:		lookaside.cc lookaside.h
linefold.o:		linefold.cc linefold.h

envlist.o:		envlist.cc envlist.h
envhelp.o:		envhelp.cc envhelp.h

mkpr.o:			mkpr.cc
mkprogenv.o:		mkprogenv.cc mkprogenv.h

spawner.o:		spawner.cc spawner.h

nulstr.o:		nulstr.cc nulstr.h

lockrw.o:		lockrw.cc lockrw.h
csem.o:			csem.cc csem.h
ucsem.o:		ucsem.cc ucsem.h

fdliner.o:		fdliner.cc fdliner.h
fbliner.o:		fbliner.cc fbliner.h

dirlist.o:		dirlist.cc dirlist.h

dirseen.o:		dirseen.cc dirseen.h
dirseen_not.o:		dirseen_not.cc dirseen.h

randomvar.o:		randomvar.cc randomvar.h

strpack.o:		strpack.cc strpack.h
strtab.o:		strtab.cc strtab.h
strstore.o:		strstore.cc strstore.h
strmgr.o:		strmgr.cc strmgr.h

serialbuf.o:		serialbuf.cc serialbuf.h stdorder.h
stdorder.o:		stdorder.cc stdorder.h

getbufsize.o:		getbufsize.cc getbufsize.h getbufsizenames.h usysdefs.h
getbufsize.o:		getbufsizenames.h usysdefs.h bufsizedata.hh

getbufsizenames.o:	getbufsizenames.c getbufsizenames.h
bufsizedata.o:		bufsizedata.cc bufsizedata.hh
bufsizevar.o:		bufsizevar.cc bufsizevar.hh

mapex.o:		mapex.cc mapex.h

sigevent.o:		sigevent.cc sigevent.h

timeout.o:		timeout.cc timeout.h

upt.o:			upt.cc upt.h

spawnproc.o:		spawnproc.cc spawnproc.h

getusername.o:		getusername.cc getusername.h
getuserhome.o:		getuserhome.cc getuserhome.h
getgroupname.o:		getgroupname.cc getgroupname.h
getprojname.o:		getprojname.cc getprojname.h

memfile.o:		memfile.cc memfile.h
filemap.o:		filemap.cc filemap.h

numsign.o:		numsign.cc numsign.h

passwdent.o:		passwdent.cc passwdent.h
spwdent.o:		spwdent.cc spwdent.h
groupent.o:		groupent.cc groupent.h
projectent.o:		projectent.cc projectent.h
userattrent.o:		userattrent.cc userattrent.h

matstr.o:		matstr.cc matstr.h
matostr.o:		matostr.cc matostr.h

sncpyarray.o:		sncpyarray.c sncpy.h
sncpyxc.o:		sncpyxc.cc sncpyxc.h
sncpyx.o:		sncpyx.cc sncpyx.h
sncpyxw.o:		sncpyxw.cc sncpyxw.h

mkpathx.o:		mkpathx.c mkpathx.h
mkpathxw.o:		mkpathxw.cc mkpathxw.h

mknpathx.o:		mknpathx.cc mknpathx.h
mknpathxw.o:		mknpathxw.cc mknpathxw.h

mkvarpath.o:		mkvarpath.cc
mkuserpath.o:		mkuserpath.cc
mkcdpath.o:		mkcdpath.cc

strcpyxc.o:		strcpyxc.cc strcpyxc.h

opensysfs.o:		opensysfs.cc opensysfs.h

quoteshellarg.o:	quoteshellarg.cc
mkquoted.o:		mkquoted.cc mkquoted.h
termconseq.o:		termconseq.cc termconseq.h

tmtime.o:		tmtime.cc tmtime.h

format.o:		format.cc format.h

strshrink.o:		strshrink.c
 
# digit-character management
ndigits.o:		ndigits.c
getdig.o:		getdig.cc getdig.h
checkbase.o:		checkbase.cc

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
mapblock.o:		mapblock.cc mapblock.hh
memtrack.o:		memtrack.cc memtrack.hh mapblock.hh
addrset.o:		addrset.cc addrset.hh
ucmemalloc.o:		ucmemalloc.cc ucmemalloc.h ucmallreg.h addrset.hh
ucmallocx.o:		ucmallocx.cc ucmallocx.h

# UNIX C-language system library timer management
uctimer.o:		uctimer.cc uctimer.h

# misc-character
toxc.o:			toxc.c toxc.h
char.o:			char.cc char.h
ischarx.o:		ischarx.cc ischarx.h
hasx.o:			hasx.cc hasx.h char.h ischarx.h

# RMX
rmx.o:			rmx.cc rmx.h

# UCSUPPORT
ucsupport.o:		ucsupport.dir
ucsupport.dir:
	makesubdir $@

# STRWCPY
strwcpy.o:		strwcpy.dir
strwcpy.dir:
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

wsnx.o:			wsnx.dir
wsnx.dir:
	makesubdir $@

wsix.o:			wsix.dir
wsix.dir:
	makesubdir $@

wsx.o:			wsx.dir
wsx.dir:
	makesubdir $@

# MATXSTR
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

# VEC
vec.o:			vec.dir
vec.dir:
	makesubdir $@

# MKX
mkx.o:			mkx.dir
mkx.dir:
	makesubdir $@

# MKTMP
mktmp.o:		mktmp.dir
mktmp.dir:
	makesubdir $@

# MKFILE
mkfile.o:		mkfile.dir
mkfile.dir:
	makesubdir $@

# EMA
ema.o:		ema.dir
ema.dir:
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

# LOCALSET
localset.o:		localset.dir
localset.dir:
	makesubdir $@

# LOCALGET
localget.o:		localget.dir
localget.dir:
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

# GETXX
getxx.o:		getxx.dir
getxx.dir:
	makesubdir $@

# FIELD
field.o:		field.dir
field.dir:
	makesubdir $@

# VARSUB
varsub.o:		varsub.dir
varsub.dir:
	makesubdir $@

# ENVS
envs.o:			envs.dir
envs.dir:
	makesubdir $@

# LOGFILE
logfile.o:		logfile.dir
logfile.dir:
	makesubdir $@

# FILER
filer.o:		filer.dir
filer.dir:
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


# UCINET
ucinetconv.o:		ucinetconv.cc ucinetconv.h

# ENCODE-DECODE
b64decover.o:		b64decoder.cc b64decoder.h

# tab handling
tabcols.o:		tabcols.cc tabcols.h
ncol.o:			ncol.cc ncol.h tabcols.h
tabexpand.o:		tabexpand.cc tabexpand.h tabcols.h

# INET
inetconv.o:		inetconv.cc inetconv.h

# LIBUC
ucmemla.o:		ucmemla.cc ucmemla.h
ucpts.o:		ucpts.cc ucpts.h

# other
nodedb.o:		nodedb.cc	nodedb.h
expcook.o:		expcook.cc	expcook.h
keyopt.o:		keyopt.cc	keyopt.h
utf8decoder.o:		utf8decoder.cc	utf8decoder.h
termstr.o:		termstr.cc	termstr.h
td.o:			td.cc		td.h termstr.h
recip.o:		recip.cc	recip.h
querystr.o:		querystr.cc	querystr.h
absfn.o:		absfn.cc	absfn.h
ids.o:			ids.cc		ids.h
groupids.o:		groupids.cc	groupids.hh
realname.o:		realname.cc	realname.h
bits.o:			bits.cc		bits.h
userattrdb.o:		userattrdb.cc	userattrdb.h
hdb.o:			hdb.cc		hdb.h
pmq.o:			pmq.cc		pmq.h
filegrp.o:		filegrp.cc	filegrp.h
unameo.o:		unameo.cc	unameo.h
hostaddr.o:		hostaddr.cc	hostaddr.h
b64decoder.o:		b64decoder.cc	b64decoder.h
lookaside.o:		lookaside.cc	lookaside.h
strpack.o:		strpack.cc	strpack.h
strtab.o:		strtab.cc	strtab.h
strstore.o:		strstore.cc	strstore.h
expcook.o:		expcook.cc	expcook.h
hostinfo.o:		hostinfo.cc	hostinfo.h
paramfile.o:		paramfile.cc	paramfile.h
memfile.o:		memfile.cc	memfile.h
dirlist.o:		dirlist.cc	dirlist.h
varray.o:		varray.cc	varray.h
csem.o:			csem.cc		csem.h
pwcache.o:		pwcache.cc	pwcache.h
lockrw.o:		lockrw.cc	lockrw.h
nodedb.o:		nodedb.cc	nodedb.h
mailmsg.o:		mailmsg.cc	mailmsg.h
ccfile.o:		ccfile.cc	ccfile.hh
thrcomm.o:		thrcomm.cc	thrcomm.h
thrbase.o:		thrbase.cc	thrbase.h thrcomm.h
pwentry.o:		pwentry.cc	pwentry.h
pwfile.o:		pwfile.cc	pwfile.h
sif.o:			sif.cc		sif.hh
bufstr.o:		bufstr.cc bufstr.h
setint.o:		setint.cc setint.h
osetint.o:		osetint.cc osetint.h
osetstr.o:		osetstr.cc osetstr.h


# sring-comparisons
vstrcmpx.o:		vstrcmpx.cc vstrcmpx.h
vstrkeycmpx.o:		vstrkeycmpx.cc vstrkeycmpx.h
vstrkeydictcmp.o:	vstrkeydictcmp.cc vstrkeycmpx.h

# string-constants
stdfnames.o:		stdfnames.c stdfnames.h
syhsdbfnames.o:		sysdbfnames.c sysdbfnames.h
sysdbfname.o:		sysdbfname.cc sysdbfname.h

# PATH
pathadd.o:		pathadd.cc pathadd.h
pathclean.o:		pathclean.cc pathclean.h

# UTILITY
splitfname.o:		splitfname.cc splitfname.h
strtabfind.o:		strtabfind.cc strtabfind.h
findbit.o:		findbit.cc findbit.h
termtypemat.o:		termtypemat.cc termtypemat.h
termcmd.o:		termcmd.cc termcmd.h
matparam.o:		matparam.cc matparam.h
typenonpath.o:		typenonpath.cc typenonpath.h
getpwetry.o:		getpwentry.cc getpwentry.h pwentry.h
intsat.o:		intsat.cc intsat.h
intfloor.o:		intfloor.cc intfloor.h
intceil.o:		intceil.cc intceil.h
digval.o:		digval.cc digval.h
willaddover.o:		willaddover.cc willaddover.h
getxid.o:		getxid.cc getxid.h
getngroups.o:		getngroups.cc getngroups.h
getsysname.o:		getsysname.cc getsysname.h
getnodename.o:		getnodename.cc getnodename.h
getnodedomain.o:	getnodedomain.cc getnodename.h
getuserorg.o:		getuserorg.cc getuserorg.h
getrealname.o:		getrealname.cc getrealname.h
getourenv.o:		getourenv.cc getourenv.h
getaddr.o:		getaddr.cc getaddr.h
getmjd.o:		getmjd.cc getmjd.h
gethz.o:		gethz.cc gethz.h
getpassword.o:		getpassword.cc getpassword.h
getrand.o:		getrand.cc getrand.h
inetaddrparse.o:	inetaddrparse.cc inetaddrparse.h
readln.o:		readln.cc readln.hh
strlibval.o:		strlibval.cc strlibval.hh
strenv.o:		strenv.cc strenv.hh
strwcmp.o:		strwcmp.cc strwcmp.h
isort.o:		isort.cc isort.h
xperm.o:		xperm.cc xperm.h ids.h
sysnoise.o:		sysnoise.cc sysnoise.h
findfilepath.o:		findfilepath.cc findfilepath.h
calstrs.o:		calstrs.cc calstrs.h
ipow.o:			ipow.cc ipow.h
base64.o:		base64.cc base64.h
ffbs.o:			ffbs.cc ffbs.h
utmpent.o:		utmpent.cc utmpent.h
shellunder.o:		shellunder.cc shellunder.h
callback.o:		callback.cc callback.h
#
isproc.o:		isproc.cc isproc.h
#
isinetaddr.o:		isinetaddr.cc isinetaddr.h
isindomain.o:		isindomain.cc isindomain.h
isoneof.o:		isoneof.cc isoneof.h
isnot.o:		isnot.cc isnot.h isoneof.h
iserror.o:		iserror.cc iserror.h isoneof.h

# emulated system kernel calls
uinfo.o:		uinfo.cc uinfo.h
umask.o:		umask.cc umask.h
unameo.o:		unameo.cc unameo.h
utmpacc.o:		utmpacc.cc utmpacc.h
utmpaccent.o:		utmpaccent.cc utmpaccent.h
ucrand.o:		ucrand.cc ucrand.h



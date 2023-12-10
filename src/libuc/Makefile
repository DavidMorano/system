# MAKEFILE (libuc)

T= libuc
TT= libuctest

ALL= $(T).a $(T).so


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


DEF0=
DEF1=
DEF2=
DEF3=
DEF4=
DEF5=
DEF6=
DEF7=

DEFS= $(DEF0) $(DEF1) $(DEF2) $(DEF3) $(DEF4) $(DEF5) $(DEF6) $(DEF7)

#INCDIRS= -Iinclude
INCDIRS=

LDRPATH= $(EXTRA)/lib

LIBDIRS= -L$(LIBDIR)

LIBS=


# flag setting
CPPFLAGS= $(DEFS) $(INCDIRS) $(MAKECPPFLAGS)
CFLAGS= $(MAKECFLAGS)
CCFLAGS= $(MAKECCFLAGS)
LDFLAGS= $(MAKELDFLAGS)


SLDFLAGS= -R$(LIBDIR):$(CGS_RPATH):$(LDRPATH)

SLIBDIRS= -L$(LIBDIR) -L$(CGS_LIBDIR)

SLIB0=
SLIB1=
SLIB2=
SLIB3= -lu
SLIB4= $(CGS_LIBS)
SLIB5=
SLIB6= -lsecdb -lproject -lpthread -lrt -lxnet -lsocket -lnsl
SLIB7= -ldl

SLIBS= $(SLIB0) $(SLIB1) $(SLIB2) $(SLIB3) $(SLIB4) $(SLIB5) $(SLIB6) $(SLIB7)

SLIBINFO= $(SLIBDIRS) $(SLIBS)


XLDFLAGS= -R$(LIBDIR):$(CGS_RPATH):$(LDRPATH)

XLIBDIRS= -L$(LIBDIR)

XLIB0=
XLIB1= -ldam -lb
XLIB2= -ldebug
XLIB3= -luc -lu
XLIB4= $(CGS_LIBS)
XLIB5=
XLIB6= -lsecdb -lproject -lpthread -lrt -lxnet -lsocket -lnsl
XLIB7= -ldl

XLIBS= $(XLIB0) $(XLIB1) $(XLIB2) $(XLIB3) $(XLIB4) $(XLIB5) $(XLIB6) $(XLIB7)

XLIBINFO= $(XLIBDIRS) $(XLIBS)


INCS= usystem.h localmisc.h


OBJ001= uclibmemalloc.o ucmemalloc.o ucmallocx.o
OBJ002= uc_open.o uc_openinfo.o 
OBJ003= uc_opensocket.o uc_openproto.o uc_openprog.o uc_openpass.o uc_openuser.o
OBJ004= uc_openfsvc.o uc_openfint.o uc_openusvc.o uc_opendialer.o
OBJ005= uc_opensys.o uc_opendev.o uc_create.o uc_createfile.o uc_joinus.o
OBJ006= uc_readn.o uc_reade.o uc_readline.o 
OBJ007= uc_write.o uc_writen.o uc_writefile.o uc_writedesc.o
OBJ008= uc_recve.o uc_recvfrome.o uc_recvmsge.o
OBJ009= uc_remove.o uc_connecte.o uc_stat.o uc_lstat.o uc_statvfs.o uc_fstat.o
OBJ010= uc_fsync.o uc_fdatasync.o uc_fchmodsuid.o uc_chmodsuid.o uc_fsize.o
OBJ011= uc_moveup.o uc_closeonexec.o uc_setappend.o uc_keepalive.o
OBJ012= uc_nonblock.o uc_ndelay.o uc_msgdiscard.o uc_reuseaddr.o uc_linger.o
OBJ013= uc_aioread.o uc_aiowrite.o uc_aioreturn.o uc_aiolist.o uc_aiosuspend.o
OBJ014= uc_fattach.o uc_fdetach.o uc_mkfifo.o uc_msync.o uc_aiofsync.o 
OBJ015= uc_accepte.o uc_lockf.o uc_lockfile.o uc_lockend.o uc_fpassfd.o
OBJ016= uc_close.o
OBJ017= uc_realpath.o uc_readlink.o uc_rename.o
OBJ018= uc_getcwd.o uc_initgroups.o uc_getnetname.o uc_procpid.o
OBJ019= uc_chmod.o uc_chown.o
OBJ020= uc_getproto.o uc_getnet.o uc_gethost.o uc_getserv.o 
OBJ021= uc_getipnodeby.o uc_getnameinfo.o uc_getaddrinfo.o 
OBJ022=
OBJ023= uc_ftime.o uc_mktime.o uc_clock.o
OBJ024= uc_getlogin.o uc_getrusage.o uc_getloadavg.o
OBJ025= uc_system.o uc_localtime.o uc_gmtime.o ucztime.o uc_sigset.o 
OBJ026=
OBJ027= uc_raise.o uc_killpg.o uc_sigqueue.o uc_syncer.o
OBJ028= uc_sigwaitinfo.o uc_sigpause.o
OBJ029= uc_sigdefault.o uc_sigignore.o uc_sighold.o uc_sigrelease.o
OBJ030= uc_settimeofday.o uc_gettimeofday.o

OBJ031= ucgetpw.o ucgetsp.o ucgetgr.o 
OBJ032= ucgetpj.o ucgetdefaultproj.o ucinproj.o 
OBJ033= ucgetus.o ucgetuserattr.o uckvamatch.o

OBJ034= ucnprocs.o ucinetconv.o
OBJ035= uc_cfsetispeed.o uc_cfsetospeed.o
OBJ036= uc_openpt.o uc_grantpt.o uc_unlockpt.o uc_ptsname.o uc_ttyname.o
OBJ037= uc_truncate.o uc_ftruncate.o
OBJ038=
OBJ039= uc_safesleep.o uc_exit.o 
OBJ040= uc_setpriority.o uc_getpriority.o 
OBJ041= uc_openshm.o uc_openshmto.o uc_unlinkshm.o 
OBJ042= uc_openpsem.o uc_unlinkpsem.o
OBJ043= uc_isatty.o uc_fminmod.o uc_minmod.o uc_link.o uc_symlink.o uc_unlink.o 
OBJ044= uc_mkdir.o uc_rmdir.o uc_access.o uc_utime.o uc_utimes.o
OBJ045= 
OBJ046= uc_madvise.o uc_mlock.o uc_munlock.o uc_mlockall.o uc_munlockall.o
OBJ047= uc_plock.o 
OBJ048= uc_fork.o uc_execve.o uc_isaexecve.o 
OBJ049=
OBJ050=
OBJ051= uc_waitwritable.o
OBJ052= 
OBJ053= uc_piper.o
OBJ054= uc_tcsetattr.o uc_tcgetattr.o uc_tcdrain.o uc_tcflush.o uc_tcflow.o
OBJ055= uc_tcsetpgrp.o uc_tcgetpgrp.o uc_tcgetsid.o
OBJ056= uc_libmemalloc.o
OBJ057= uc_getrandom.o uc_getsocktype.o
OBJ058=
OBJ059= pt_sigmask.o pt_atfork.o
OBJ060=
OBJ061= getrandom.o
OBJ062= bufsizevar.o
OBJ063=

OBJ064= getprotofamily.o geteaddrinfo.o getaf.o getaflen.o getpwd.o
OBJ065= getnodedomain.o getutmpent.o getax.o getuserhome.o getxusername.o
OBJ066= getbufsize.o getbufsizenames.o gethz.o getngroups.o getproto.o getserv.o
OBJ067= digbfsizes.o
OBJ068= getportnum.o getenver.o getourenv.o
OBJ069= nextpowtwo.o ndigits.o ipow.o llpow.o findbit.o
OBJ070= isFailOpen.o isFailConn.o
OBJ071= isnot.o iserror.o isoneof.o

OBJ072= filebuf.o filebuf_writers.o filebuf_mailsupport.o
OBJ073= inetpton.o xperm.o getprogpath.o findxfile.o xfile.o
OBJ074= vecstr.o vecstr_adduniqs.o vecstr_addpath.o
OBJ075= vecstr_env.o vecstr_envfile.o vecstr_loadfile.o
OBJ076= vecpstr.o vecpstr_loadfile.o vecpstr_addpath.o
OBJ077= vecpstr_envadd.o vecpstr_envfile.o
OBJ078= pta.o ptm.o ptma.o ptc.o ptca.o psem.o lockrw.o envhelp.o
OBJ079= storeitem.o storebuf.o buffer.o field.o char.o mkprogenv.o

OBJ080= fsdir.o pmq.o sigblock.o userattr.o filemap.o uname.o 
OBJ081= vechand.o vecsorthand.o vecobj.o vecelem.o recarr.o
OBJ082= envlist.o hdb.o lookaside.o ids.o linefold.o nulstr.o 
OBJ083= dirseen.o dirseen_not.o
OBJ084= strpack.o strmgr.o strtab.o
OBJ085= varray.o dirlist.o randomvar.o sigign.o ucsem.o
OBJ086= sbuf.o sbuf_addquoted.o sbuf_termconseq.o
OBJ087= hostinfo.o hostaddr.o hostent.o inetaddr.o sockaddress.o ucpwcache.o

OBJ088= strsub.o
OBJ089= strnsub.o strncasesub.o 
OBJ090= strncpyxc.o
OBJ091= strnkeycmp.o strnleadcmp.o strnncmp.o strnlen.o strnnlen.o
OBJ092= strnxpbrk.o
OBJ093= strnsetblanks.o strnset.o strnwcpy.o
OBJ094=
OBJ095= strdcpyx.o strdcpyxw.o

OBJ096=
OBJ097= sncpyx.o sncpyxw.o sncpyxc.o 
OBJ098= snflags.o snfsflags.o snpollflags.o snopenflags.o snfilemode.o
OBJ099= snxxx.o sntmtime.o snshellunder.o 
OBJ100= pathadd.o pathclean.o
OBJ101=
OBJ102= mkvarpath.o mkuserpath.o mkcdpath.o mkexpandpath.o
OBJ103= mkquoted.o mksofname.o 

OBJ104= sichr.o rmext.o
OBJ105= sfskipwhite.o
OBJ106= sfshrink.o sfdirname.o sfbasename.o sfwhitedot.o sfprogroot.o
OBJ107=
OBJ108=
OBJ109= matstr.o matostr.o matkeystr.o 
OBJ110= strkeycmp.o strwcmp.o
OBJ111= vstrcmpx.o vstrkeycmpx.o vstrkeydictcmp.o

OBJ112= strcpyxc.o
OBJ113= strwcpy.o strwcpyx.o strwcpyxc.o strwcpyxx.o
OBJ114=
OBJ115= upt.o uptspawn.o tmtime.o spawnproc.o dupup.o nlsdialassist.o
OBJ116= mkcleanline.o tabcols.o checkbase.o ncol.o
OBJ117= mapex.o calstrs.o
OBJ118=
OBJ119= toxc.o base64.o 

OBJ120= uinfo.o umask.o uxygetpw.o utmpacc.o ucproguser.o ucprogdata.o
OBJ121= nisdomainname.o
OBJ122= opentmpfile.o opensysfs.o opensys_banner.o
OBJ123= opensockaddr.o openaddrinfo.o
OBJ124=
OBJ125= getuid_name.o getgid_name.o getpjid_name.o
OBJ126= passwdent.o shadowent.o groupent.o projectent.o userattrent.o
OBJ127= utmpaccent.o

OBJ128= sigaction.o sigevent.o 
OBJ129= timeval.o itimerspec.o timespec.o timeout.o
OBJ130= sigignores.o sigdefaults.o
OBJ131= isasocket.o isinetaddr.o isindomain.o
OBJ132= hasNotDots.o
OBJ133= nleadstr.o nleadkeystr.o
OBJ134= format.o
OBJ135= getrootdname.o getdig.o randlc.o

OBJ136= ctxxx.o ctbin.o ctoct.o ctdec.o cthex.o ctdecp.o ctdecf.o cvtdig.o
OBJ137= cfxxx.o cfbin.o cfoct.o cfhex.o cfdec.o cfdecmf.o cfdect.o cfxxxx.o 
OBJ138= cfnum.o cfhexs.o hexval.o
OBJ139= isalphalatin.o isalnumlatin.o isdigitlatin.o ishexlatin.o
OBJ140= isdict.o isprintlatin.o isprintbad.o
OBJ141= hasxc.o
OBJ142=
OBJ143=

OBJ144= intceil.o intfloor.o intsat.o
OBJ145= ucontext_rtn.o
OBJ146= dialopts.o
OBJ147=
OBJ148= dialuss.o dialussmux.o dialussnls.o
OBJ149= dialtcp.o dialtcpmux.o dialtcpnls.o 
OBJ150= dialudp.o dialusd.o dialpass.o
OBJ151= dialprog.o dialhttp.o dialfinger.o

OBJ152= openshmtmp.o
OBJ153= mallocxx.o
OBJ154= strtoxlonglong.o xxtostr.o
OBJ155=
OBJ156=
OBJ157=
OBJ158=
OBJ159=

OBJAa= $(OBJ000) $(OBJ001) $(OBJ002) $(OBJ003)
OBJAb= $(OBJ004) $(OBJ005) $(OBJ006) $(OBJ007) 
OBJA= $(OBJAa) $(OBJAb)

OBJBa= $(OBJ008) $(OBJ009) $(OBJ010) $(OBJ011)
OBJBb= $(OBJ012) $(OBJ013) $(OBJ014) $(OBJ015)
OBJB= $(OBJBa) $(OBJBb)

OBJCa= $(OBJ016) $(OBJ017) $(OBJ018) $(OBJ019)
OBJCb= $(OBJ020) $(OBJ021) $(OBJ022) $(OBJ023)
OBJC= $(OBJCa) $(OBJCb)

OBJDa= $(OBJ024) $(OBJ025) $(OBJ026) $(OBJ027)
OBJDb= $(OBJ028) $(OBJ029) $(OBJ030) $(OBJ031)
OBJD= $(OBJDa) $(OBJDb)

OBJEa= $(OBJ032) $(OBJ033) $(OBJ034) $(OBJ035)
OBJEb= $(OBJ036) $(OBJ037) $(OBJ038) $(OBJ039)
OBJE= $(OBJEa) $(OBJEb)

OBJFa= $(OBJ040) $(OBJ041) $(OBJ042) $(OBJ043)
OBJFb= $(OBJ044) $(OBJ045) $(OBJ046) $(OBJ047)
OBJF= $(OBJFa) $(OBJFb)

OBJGa= $(OBJ048) $(OBJ049) $(OBJ050) $(OBJ051)
OBJGb= $(OBJ052) $(OBJ053) $(OBJ054) $(OBJ055)
OBJG= $(OBJGa) $(OBJGb)

OBJHa= $(OBJ056) $(OBJ057) $(OBJ058) $(OBJ059)
OBJHb= $(OBJ060) $(OBJ061) $(OBJ062) $(OBJ063)
OBJH= $(OBJHa) $(OBJHb)

OBJIa= $(OBJ064) $(OBJ065) $(OBJ066) $(OBJ067)
OBJIb= $(OBJ068) $(OBJ069) $(OBJ070) $(OBJ071)
OBJI= $(OBJIa) $(OBJIb)

OBJJa= $(OBJ072) $(OBJ073) $(OBJ074) $(OBJ075)
OBJJb= $(OBJ076) $(OBJ077) $(OBJ078) $(OBJ079)
OBJJ= $(OBJJa) $(OBJJb)

OBJKa= $(OBJ080) $(OBJ081) $(OBJ082) $(OBJ083)
OBJKb= $(OBJ084) $(OBJ085) $(OBJ086) $(OBJ087)
OBJK= $(OBJKa) $(OBJKb)

OBJLa= $(OBJ088) $(OBJ089) $(OBJ090) $(OBJ091)
OBJLb= $(OBJ092) $(OBJ093) $(OBJ094) $(OBJ095)
OBJL= $(OBJLa) $(OBJLb)

OBJMa= $(OBJ096) $(OBJ097) $(OBJ098) $(OBJ099)
OBJMb= $(OBJ100) $(OBJ101) $(OBJ102) $(OBJ103)
OBJM= $(OBJMa) $(OBJMb)

OBJNa= $(OBJ104) $(OBJ105) $(OBJ106) $(OBJ107)
OBJNb= $(OBJ108) $(OBJ109) $(OBJ110) $(OBJ111)
OBJN= $(OBJNa) $(OBJNb)

OBJOa= $(OBJ112) $(OBJ113) $(OBJ114) $(OBJ115)
OBJOb= $(OBJ116) $(OBJ117) $(OBJ118) $(OBJ119)
OBJO= $(OBJOa) $(OBJOb)

OBJPa= $(OBJ120) $(OBJ121) $(OBJ122) $(OBJ123)
OBJPb= $(OBJ124) $(OBJ125) $(OBJ126) $(OBJ127)
OBJP= $(OBJPa) $(OBJPb)

OBJQa= $(OBJ128) $(OBJ129) $(OBJ130) $(OBJ131)
OBJQb= $(OBJ132) $(OBJ133) $(OBJ134) $(OBJ135)
OBJQ= $(OBJQa) $(OBJQb)

OBJRa= $(OBJ136) $(OBJ137) $(OBJ138) $(OBJ139)
OBJRb= $(OBJ140) $(OBJ141) $(OBJ142) $(OBJ143)
OBJR= $(OBJRa) $(OBJRb)

OBJSa= $(OBJ144) $(OBJ145) $(OBJ146) $(OBJ147)
OBJSb= $(OBJ148) $(OBJ149) $(OBJ150) $(OBJ151)
OBJS= $(OBJSa) $(OBJSb)

OBJTa= $(OBJ152) $(OBJ153) $(OBJ154) $(OBJ155)
OBJTb= $(OBJ156) $(OBJ157) $(OBJ158) $(OBJ159)
OBJT= $(OBJTa) $(OBJTb)


OBJg0= $(OBJA) $(OBJB) $(OBJC) $(OBJD) $(OBJE) $(OBJF) $(OBJG) $(OBJH) 
OBJg1= $(OBJI) $(OBJJ) $(OBJK) $(OBJL) $(OBJM) $(OBJN) $(OBJO) $(OBJP)
OBJg2= $(OBJQ) $(OBJR) $(OBJS) $(OBJT)

OBJ= $(OBJg0) $(OBJg1) $(OBJg2)


SOBJ0= obja.o objb.o objc.o objd.o obje.o objf.o objg.o objh.o obji.o objj.o
SOBJ1= objk.o objl.o objm.o objn.o objo.o objp.o objq.o objr.o objs.o objt.o

SOBJ= $(SOBJ0) $(SOBJ1)


TOBJ00= uc_gethostid.o uc_confstr.o uc_gethostname.o

#TOBJ01a= uc_memalloc.o 
TOBJ01b= uc_mallocstrw.o uc_mallocbuf.o

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

so:			$(T).so

a:			$(T).a

.c.ln:
	$(LINT) -c $(LINTFLAGS) $(CPPFLAGS) $<

.c.ls:
	$(LINT) $(LINTFLAGS) $(CPPFLAGS) $<

.c.i:
	$(CPP) $(CPPFLAGS) $< > $(*).i

.cc.ii:
	$(CPP) $(CPPFLAGS) $< > $(*).ii

.c.s:
	$(CC) -S $(CPPFLAGS) $(CFLAGS) $<

.cc.s:
	$(CXX) -S $(CPPFLAGS) $(CCFLAGS) $<

.c.o:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $<

.cc.o:
	$(CXX) -c $(CPPFLAGS) $(CCFLAGS) $<


$(TT).a:		$(TOBJ)
	$(AR) -rc $@ $?

$(T).so:		$(SOBJ) Makefile $(T).a
	$(LD) -G -o $@ $(SLDFLAGS) $(SOBJ) $(SLIBINFO) > $(T).lm

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


# pre-base
OBJ_PREBASE0= usupport.o utimeout.o utimeouts.o ulogerror.o aflag.o
OBJ_PREBASE1= usys_xxx.o usys_sunos.o usys_darwin.o usys_linux.o

OBJ_PREBASE= $(OBJ_PREBASE0) $(OBJ_PREBASE1)

prebase.o:	$(OBJ_PREBASE)
	$(LD) -r -o $@ $(OBJ_PREBASE)


# base
OBJ0_BASE= varnames.o valuelimits.o digbufsizes.o ucvariables.o
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


# convert-to (CT)
OBJ0_CT= ctxxx.o ctbin.o ctoct.o cthex.o ctdec.o ctchars.o 
OBJ1_CT= ctdecp.o cvtdig.o digbufsizes.o xxtostr.o

OBJ_CT= obj0_ct.o obj1_ct.o

obj0_ct.o:	$(OBJ0_CT)
	$(LD) -r -o $@ $(OBJ0_CT)

obj1_ct.o:	$(OBJ1_CT)
	$(LD) -r -o $@ $(OBJ1_CT)

ct.o:		$(OBJ_CT)
	$(LD) -r -o $@ $(OBJ_CT)


# convert-from (CF)
OBJ_CF= cfxxx.o cfbin.o cfoct.o cfhex.o cfdec.o cfchars.o cfxxxx.o

cf.o:		$(OBJ_CF)
	$(LD) -r -o $@ $(OBJ_CF)


# string-to-x
strtox.o:	strtoxlonglong.o

# string-to-x
ucstr.o:	ucstrtox.o ucstrtod.o
ucsysconf.o:	ucsysconf.cc


# vec

# vecstr
OBJA_VECSTR= vecstr.o vecstr_env.o vecstr_envfile.o 
OBJB_VECSTR= vecstr_adduniqs.o vecstr_addpath.o
OBJC_VECSTR= vecstr_avmkstr.o vecstr_loadfile.o
OBJD_VECSTR= vecstr_adds.o vecstr_foilcmp.o
OBJE_VECSTR= vecstr_srvargs.o vecstr_svcargs.o

obja_vecstr.o:	$(OBJA_VECSTR)
	$(LD) -r -o $@ $(OBJA_VECSTR)
objb_vecstr.o:	$(OBJB_VECSTR)
	$(LD) -r -o $@ $(OBJB_VECSTR)
objc_vecstr.o:	$(OBJC_VECSTR)
	$(LD) -r -o $@ $(OBJC_VECSTR)
objd_vecstr.o:	$(OBJD_VECSTR)
	$(LD) -r -o $@ $(OBJD_VECSTR)
obje_vecstr.o:	$(OBJE_VECSTR)
	$(LD) -r -o $@ $(OBJE_VECSTR)

OBJ_VECSTR+= obja_vecstr.o objb_vecstr.o objc_vecstr.o
OBJ_VECSTR+= objd_vecstr.o obje_vecstr.o

obj_vecstr.o:	$(OBJ_VECSTR)
	$(LD) -r -o $@ $(OBJ_VECSTR)

# vecpstr
OBJA_VECPSTR= vecpstr.o vecpstr_env.o vecpstr_envfile.o 
OBJB_VECPSTR= vecpstr_adduniqs.o vecpstr_addpath.o
OBJC_VECPSTR= vecpstr_avmkstr.o vecpstr_loadfile.o
OBJD_VECPSTR= vecpstr_adds.o vecpstr_foilcmp.o
OBJE_VECPSTR= vecpstr_srvargs.o vecpstr_svcargs.o
OBJF_VECPSTR= vecpstr_loadgrusers.o vecpstr_addsubdirs.o


obja_vecpstr.o:	$(OBJA_VECPSTR)
	$(LD) -r -o $@ $(OBJA_VECPSTR)
objb_vecpstr.o:	$(OBJB_VECPSTR)
	$(LD) -r -o $@ $(OBJB_VECPSTR)
objc_vecpstr.o:	$(OBJC_VECPSTR)
	$(LD) -r -o $@ $(OBJC_VECPSTR)
objd_vecpstr.o:	$(OBJD_VECPSTR)
	$(LD) -r -o $@ $(OBJD_VECPSTR)
obje_vecpstr.o:	$(OBJE_VECPSTR)
	$(LD) -r -o $@ $(OBJE_VECPSTR)
objf_vecpstr.o:	$(OBJF_VECPSTR)
	$(LD) -r -o $@ $(OBJF_VECPSTR)

OBJ_VECPSTR+= obja_vecpstr.o objb_vecpstr.o objc_vecpstr.o 
OBJ_VECPSTR+= objd_vecpstr.o obje_vecpstr.o objf_vecpstr.o 

obj_vecpstr.o:	$(OBJ_VECPSTR)
	$(LD) -r -o $@ $(OBJ_VECPSTR)


OBJA_VEC= obj_vecstr.o obj_vecpstr.o
OBJB_VEC= vechand.o recarr.o raqhand.o
OBJ_VEC= obja_vec.o objb_vec.o

obja_vec.o:	$(OBJA_VEC)
	$(LD) -r -o $@ $(OBJA_VEC)
objb_vec.o:	$(OBJB_VEC)
	$(LD) -r -o $@ $(OBJB_VEC)

vec.o:		$(OBJ_VEC)
	$(LD) -r -o $@ $(OBJ_VEC)


# string-find
OBJ0_SF= sfbaselib.o sfbasename.o sfbracketval.o sfbreak.o 
OBJ1_SF= sfcasesub.o sfcenter.o sfcookkey.o sfdequote.o 
OBJ2_SF= sfdirname.o sfkey.o sflast.o
OBJ3_SF= sfnamecomp.o sfprogroot.o
OBJ4_SF= sfrootname.o sfshrink.o sfskipwhite.o sfsub.o
OBJ5_SF= sfsubstance.o sfthing.o sfwhitedot.o sfword.o
OBJ6_SF=

obj0_sf.o:	$(OBJ0_SF)
	$(LD) -r -o $@ $(OBJ0_SF)

obj1_sf.o:	$(OBJ1_SF)
	$(LD) -r -o $@ $(OBJ1_SF)

obj2_sf.o:	$(OBJ2_SF)
	$(LD) -r -o $@ $(OBJ2_SF)

obj3_sf.o:	$(OBJ3_SF)
	$(LD) -r -o $@ $(OBJ3_SF)

obj4_sf.o:	$(OBJ4_SF)
	$(LD) -r -o $@ $(OBJ4_SF)

obj5_sf.o:	$(OBJ5_SF)
	$(LD) -r -o $@ $(OBJ5_SF)

obj6_sf.o:	$(OBJ6_SF)
	$(LD) -r -o $@ $(OBJ6_SF)

OBJA_SF= obj0_sf.o obj1_sf.o obj2_sf.o
OBJB_SF= obj3_sf.o obj4_sf.o obj5_sf.o

sf.o:		$(OBJA_SF) $(OBJB_SF)
	$(LD) -r -o $@ $(OBJA_SF) $(OBJB_SF)

# uctimeout (time-out call-backs)
uctimeout.o:	uctimeout.cc


# CHARACTER
OBJA_CHAR= mkchar.o toxc.o char.o rmeol.o
OBJB_CHAR= ischarx.o hasx.o

OBJ_CHAR= obja_char.o objb_char.o

obja_char.o:	$(OBJA_CHAR)
	$(LD) -r -o $@ $(OBJA_CHAR)

objb_char.o:	$(OBJB_CHAR)
	$(LD) -r -o $@ $(OBJB_CHAR)

character.o:	$(OBJ_CHAR)
	$(LD) -r -o $@ $(OBJ_CHAR)


# QUEUE
OBJA_QUEUE= plainq.o q.o aiq.o
OBJB_QUEUE= pq.o ciq.o piq.o cq.o

OBJ_QUEUE= obja_queue.o objb_queue.o

obja_queue.o:	$(OBJA_QUEUE)
	$(LD) -r -o $@ $(OBJA_QUEUE)

objb_queue.o:	$(OBJB_QUEUE)
	$(LD) -r -o $@ $(OBJB_QUEUE)

queue.o:	$(OBJ_QUEUE)
	$(LD) -r -o $@ $(OBJ_QUEUE)


# SYSMISC
ucsysmisc.o:		ucsysmisc.cc
ucnprocs.o:		ucnprocs.cc
ucpathconf.o:		ucpathconf.cc
ucmain.o:		ucmain.cc
ucatfork.o:		ucatfork.cc
ucatexit.o:		ucatexit.cc
ucfork.o:		ucfork.cc



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


uc_safesleep.o:		uc_safesleep.c

ucgetpuid.o:		ucgetpuid.cc

uc_openinfo.o:		uc_openinfo.c opensysfs.h

uc_openuser.o:		uc_openuser.c opensysfs.h

uc_timeout.o:		uc_timeout.cc timeout.h
	$(GPP) -c $(GPPFLAGS) $(CFLAGS) $(CPPFLAGS) uc_timeout.cc


# FSDIR
posixdirent.o:		posixdirent.cc posixdirent.hh
fsdir.o:		fsdir.cc fsdir.h
sunos_fsdir.o:		sunos_fsdir.cc sunos_fsdir.h posixdirent.hh
darwin_fsdir.o:		darwin_fsdir.cc darwin_fsdir.h posixdirent.hh

# POSIX Thread Attribute
pta.o:			pta.c pta.h

# POSIX Thread Mutex Attribute
ptma.o:			ptma.cc ptma.h

# POSIX Thread Condition Attribute
ptca.o:			ptca.c ptca.h

# POSIX Thread Mutex
ptm.o:			ptm.cc ptm.h

# POSIX Thread Condition Variable
ptc.o:			ptc.cc ptc.h

# POSIX Semaphore
psem.o:			psem.cc psem.h

# POSIX Read-Write Lock (broken in dynamically loadable modeules)
ptrwlock.o:		ptrwlock.c ptrwlock.h

# POSIX Thread Read-Write Lock Attribute
ptrwa.o:		ptrwa.c ptrwa.h

# POSIX Message Queue
pmq.o:			pmq.cc pmq.h

sighand.o:		sighand.c sighand.h
sigblock.o:		sigblock.cc sigblock.h
sigign.o:		sigign.c sigign.h

hostinfo.o:		hostinfo.cc hostinfo.h
hostaddr.o:		hostaddr.cc hostaddr.h
hostent.o:		hostent.c hostent.h
inetaddr.o:		inetaddr.cc inetaddr.h
sockaddress.o:		sockaddress.c sockaddress.h

storeitem.o:		storeitem.cc storeitem.h
storebuf.o:		storebuf.cc storebuf.h

sbuf.o:			sbuf.cc sbuf.h
sbuf_addquoted.o:	sbuf_addquoted.c sbuf.h
sbuf_termconseq.o:	sbuf_termconseq.c sbuf.h

buffer.o:		buffer.cc buffer.h
obuf.o:			obuf.cc obuf.hh

baops.o:		baops.c baops.h
strop.o:		strop.cc strop.h
dstr.o:			dstr.c dstr.h

field.o:		field.cc field.h
field_srvarg.o:		field_srvarg.cc field.h
field_svcargs.o:	field_svcargs.c field.h
field_word.o:		field_word.c field.h
field_wordphrase.o:	field_wordphrase.c field.h

filebuf.o:		filebuf.cc filebuf.h
filebuf_writers.o:	filebuf_writers.cc filebuf.h
filebuf_mailsupport.o:	filebuf_mailsupport.c filebuf.h

varray.o:		varray.cc varray.h

recarr.o:		recarr.cc recarr.h
raqhand.o:		raqhand.cc raqhand.h
vechand.o:		vechand.cc vechand.h
vecsorthand.o:		vecsorthand.cc vecsorthand.h

vecstr.o:		vecstr.cc vecstr.h
vecstr_env.o:		vecstr_env.c vecstr.h
vecstr_envfile.o:	vecstr_envfile.cc vecstr.h
vecstr_adduniqs.o:	vecstr_adduniqs.c vecstr.h
vecstr_addpath.o:	vecstr_addpath.cc vecstr.h
vecstr_avmkstr.o:	vecstr_avmkstr.c vecstr.h
vecstr_loadfile.o:	vecstr_loadfile.cc vecstr.h

vecpstr.o:		vecpstr.cc vecpstr.h
vecpstr_env.o:		vecpstr_env.c vecpstr.h
vecpstr_envfile.o:	vecpstr_envfile.cc vecpstr.h
vecpstr_adduniqs.o:	vecpstr_adduniqs.c vecpstr.h
vepcstr_addpath.o:	vecpstr_addpath.cc vecpstr.h
vecpstr_avmkstr.o:	vecpstr_avmkstr.c vecpstr.h
vecpstr_loadfile.o:	vecpstr_loadfile.cc vecpstr.h
vecpstr_loadgrpusers.o:	vecpstr_loadgrusers.cc vecpstr.h
vecpstr_loadpjusers.o:	vecpstr_loadpjusers.cc vecpstr.h
vecpstr_addsubdirs.o:	vecpstr_addsubdirs.cc vecpstr.h

vecobj.o:		vecobj.cc vecobj.h

vecelem.o:		vecelem.cc vecelem.h

hdb.o:			hdb.cc hdb.h

lookaside.o:		lookaside.cc lookaside.h

pwcache.o:		pwcache.cc pwcache.h

linefold.o:		linefold.c linefold.h

envlist.o:		envlist.c envlist.h
envhelp.o:		envhelp.c envhelp.h

mkpr.o:			mkpr.cc
mkprogenv.o:		mkprogenv.c mkprogenv.h

spawner.o:		spawner.c spawner.h

nulstr.o:		nulstr.cc nulstr.h

lockrw.o:		lockrw.cc lockrw.h
csem.o:			csem.cc csem.h
ucsem.o:		ucsem.cc ucsem.h


dirlist.o:		dirlist.c dirlist.h

dirseen.o:		dirseen.c dirseen.h

dirseen_not.o:		dirseen_not.c dirseen.h

randomvar.o:		randomvar.cc randomvar.h

strpack.o:		strpack.cc strpack.h
strtab.o:		strtab.cc strtab.h
strstore.o:		strstore.cc strstore.h
strmgr.o:		strmgr.c strmgr.h

serialbuf.o:		serialbuf.c serialbuf.h

stdorder.o:		stdorder.cc stdorder.h


getbufsize.o:		getbufsize.cc getbufsize.h getbufsizenames.h usysdefs.h
getbufsize.o:		getbufsizenames.h usysdefs.h bufsizedata.hh

getbufsizenames.o:	getbufsizenames.c getbufsizenames.h
varnames.o:		varnames.cc varnames.hh
valuelimits.o:		valuelimits.cc valuelimits.hh
digbufsizes.o:		digbufsizes.cc digbufsizes.hh
bufsizedata.o:		bufsizedata.cc bufsizedata.hh
bufsizevar.o:		bufsizevar.cc bufsizevar.hh

mapex.o:		mapex.c mapex.h

calstrs.o:		calstrs.c calstrs.h

sigevent.o:		sigevent.c sigevent.h

timeval.o:		timeval.c timeval.h

timespec.o:		timespec.c timespec.h

itimerspec.o:		itimerspec.c itimerspec.h

timeout.o:		timeout.c timeout.h

upt.o:			upt.c upt.h

base64.o:		base64.c base64.h

spawnproc.o:		spawnproc.cc spawnproc.h

getxusername.o:		getxusername.c getxusername.h

getutmpent.o:		getutmpent.cc getutmpent.h
mkutmpid.o:		mkutmpid.cc mkutmpid.h


unameo.o:		unameo.cc unameo.h
memfile.o:		memfile.cc memfile.h
filemap.o:		filemap.cc filemap.h

numsign.o:		numsign.cc numsign.h


# GETXX
getax.o:		getax.c getax.h
getpe.o:		getpe.c getpe.h
getse.o:		getse.c getse.h
getne.o:		getne.c getne.h
gethe.o:		gethe.c gethe.h

passwdent.o:		passwdent.cc passwdent.h
spwdent.o:		spwdent.cc spwdent.h
groupent.o:		groupent.cc groupent.h
projectent.o:		projectent.cc projectent.h
userattrent.o:		userattrent.cc userattrent.h

findbit.o:		findbit.c findbit.h

matstr.o:		matstr.cc matstr.h
matostr.o:		matostr.cc matostr.h

sncpyarray.o:		sncpyarray.c sncpy.h
sncpyxc.o:		sncpyxc.cc sncpyxc.h
sncpyx.o:		sncpyx.cc sncpyx.h
sncpyxw.o:		sncpyxw.cc sncpyxw.h

snxxx.o:		snxxx.cc sn.h
snfsflags.o:		snfsflags.c sn.h snflags.h
snfilemode.o:		snfilemode.c sn.h snflags.h
snopenflags.o:		snopenflags.c sn.h snflags.h
snfsflags.o:		snfsflags.c sn.h snflags.h
snpollflags.o:		snpollflags.c sn.h snflags.h
sntmtime.o:		sntmtime.c sn.h tmtime.h
snflags.o:		snflags.c snflags.h
snabbr.o:		snabbr.c sn.h
snshellunder.o:		snshellunder.c sn.h
snsigabbr.o:		snsigabbr.c sn.h
snxtilook.o:		snxtilook.c sn.h
snxtierr.o:		snxtierr.c sn.h

mkpathx.o:		mkpathx.c mkpathx.h
mkpathxw.o:		mkpathxw.cc mkpathxw.h

mknpathx.o:		mknpathx.cc mknpathx.h
mknpathxw.o:		mknpathxw.cc mknpathxw.h

mkvarpath.o:		mkvarpath.cc
mkuserpath.o:		mkuserpath.cc
mkcdpath.o:		mkcdpath.cc

strcpyxc.o:		strcpyxc.cc strcpyxc.h

strwcpy.o:		strwcpy.cc strwcpy.h
strwcpyx.o:		strwcpyx.cc strwcpyx.h
strwcpyxc.o:		strwcpyxc.cc strwcpyxc.h
strwcpyxx.o:		strwcpyxx.cc strwcpyxx.h

strdcpyx.o:		strdcpyx.c strdcpyx.h
strdcpyxw.o:		strdcpyxw.c strdcpyxw.h

cfxxxx.o:		cfxxxx.cc cfxxxx.hh
cfxxx.o:		cfxxx.cc cfxxx.h cfxxxx.hh
cfbin.o:		cfbin.cc cfbin.h cfxxxx.hh
cfoct.o:		cfoct.cc cfoct.h cfxxxx.hh
cfhex.o:		cfhex.cc cfhex.h cfxxxx.hh
cfdec.o:		cfdec.cc cfdec.h cfxxxx.hh
cfdecmf.o:		cfdecmf.cc cfdecmf.h
cfdect.o:		cfdect.cc cfdect.h
cfdecf.o:		cfdecf.cc cfdecf.h
cfchars.o:		cfchars.cc cfchars.h

cvtdig.o:		cvtdig.cc cvtdig.hh
ctxxx.o:		ctxxx.cc ctxxx.h
ctbin.o:		ctbin.cc ctbin.h cvtdig.hh
ctoct.o:		ctoct.cc ctoct.h cvtdig.hh
cthex.o:		cthex.cc cthex.h cvtdig.hh
ctdec.o:		ctdec.cc ctdec.h cvtdig.hh
ctdecp.o:		ctdecp.cc ctdecp.h ctdec.h
ctdecf.o:		ctdecf.c ctdec.h
ctchars.o:		ctchars.cc ctchars.h
xxtostr.o:		xxtostr.cc xxtostr.h

dialopts.o:		dialopts.c dialopts.h
dialprog.o:		dialprog.c dialopts.h
dialudp.o:		dialudp.c dialopts.h
dialtcp.o:		dialtcp.c dialopts.h
dialtcpmux.o:		dialtcpmux.c dialopts.h
dialtcpnls.o:		dialtcpnls.c nlsdialassist.h dialopts.h
dialfinger.o:		dialfinger.c dialopts.h
nlsdialassist.o:	nlsdialassist.c nlsdialassist.h dialopts.h

# QUEUE
plainq.o:		plainq.cc plainq.h
q.o:			q.cc q.h plainq.h
aiq.o:			aiq.cc aiq.h q.h
pq.o:			pq.c pq.h
ciq.o:			ciq.cc ciq.h pq.h
piq.o:			piq.cc piq.h pq.h
cq.o:			cq.cc cq.h


opensysfs.o:		opensysfs.cc opensysfs.h

quoteshellarg.o:	quoteshellarg.c
mkquoted.o:		mkquoted.cc mkquoted.h
termconseq.o:		termconseq.cc termconseq.h

intsat.o:		intsat.c intsat.h

tmtime.o:		tmtime.c tmtime.h

getnodedomain.o:	getnodedomain.c

format.o:		format.cc format.h

strshrink.o:		strshrink.c
 
# strnwx
strnwcpy.o:		strnwcpy.cc strn.h
strncpyxc.o:		strncpyxc.cc strncpyxc.h

# chracter-find
strnxchr.o:		strnxchr.cc strnxchr.h strn.h
strnxpbrk.o:		strnxpbrk.cc strn.h

# digit-character management
ndigits.o:		ndigits.c
getdig.o:		getdig.cc getdig.h
checkbase.o:		checkbase.cc

# malloc
mallocxx.o:		mallocxx.cc mallocxx.h
mallocstuff.o:		mallocstuff.c mallocstuff.h

# special built-ins (user-space enumlated kernel calls)
uinfo.o:		uinfo.cc uinfo.h
umask.o:		umask.c umask.h
ucygetpw.o:		ucygetpw.cc ucygetpw.h ucpwcache.h recarr.h
ucgetpid.o:		ucgetpid.cc ucgetpid.h
ucproguser.o:		ucproguser.cc ucproguser.h
ucprogdata.o:		ucprogdata.cc ucprogdata.h
ucclustername.o:	ucclustername.c ucclustername.h
ucpwcache.o:		ucpwcache.c ucpwcache.h recarr.h
utmpacc.o:		utmpacc.cc utmpacc.h
utmpaccent.o:		utmpaccent.c utmpaccent.h
tmpx.o:			tmpx.cc tmpx.h

# part of string-to-integer interface
strtoxlonglong.o:	strtoxlonglong.cc strtoxlonglong.h

# UNIX C-language system library string-to-integer interface
ucstrtox.o:		ucstrtox.cc ucstrtox.h strtoxlonglong.h
ucstrtod.o:		ucstrtod.c ucstrtod.h

# UNIX C-language system library data-base interface
ucgetpw.o:		ucgetpw.cc ucgetpw.h
ucgetsp.o:		ucgetsp.cc ucgetsp.h
ucgetgr.o:		ucgetgr.cc ucgetgr.h
ucgetpj.o:		ucgetpj.cc ucgetpj.h
ucgetus.o:		ucgetus.cc ucgetus.h
ucgetpr.o:		ucgetpr.cc ucgetpr.h
ucgetnw.o:		ucgetnw.cc ucgetnw.h
ucgetho.o:		ucgetho.cc ucgetho.h
ucgetsv.o:		ucgetsv.cc ucgetsv.h

# UNIX C-language system library data-base interface (support)
spwd.o:			spwd.cc spwd.h
userattr.o:		userattr.cc userattr.h
ucpasswd.o:		ucpasswd.cc ucpasswd.h
ucgroup.o:		ucgroup.cc ucgroup.h
ucproject.o:		ucproject.cc ucproject.h

# UNIX C-language system library memory management
ucmemalloc.o:		ucmemalloc.cc ucmemalloc.h
ucmallocx.o:		ucmallocx.cc ucmallocx.h

# UNIX C-language system library timer management
uctimer.o:		uctimer.cc uctimer.h

# UNIX operating system support
usupport.o:		usupport.cc usupport.h
utimeout.o:		utimeout.c utimeoutdefs.h
utimeouts.o:		utimeouts.cc utimeouts.h utimeoutdefs.h
ulogerror.o:		ulogerror.cc ulogerror.h
usys_xxx.o:		usys_xxx.cc usys_xxx.h
usys_sunos.o:		usys_sunos.cc usys_sunos.h
usys_darwin.o:		usys_darwin.cc usys_darwin.h
usys_linux.o:		usys_linux.cc usys_linux.h
aflag.o:		aflag.cc aflag.hh
timewatch.o:		timwatch.cc timewatch.hh
timecount.o:		timecount.cc timecount.hh

# UNIX C-language system library support
uclibmemalloc.o:	uclibmemalloc.cc uclibmemalloc.h

mapblock.o:		mapblock.cc mapblock.hh
memtrack.o:		memtrack.cc memtrack.hh
libmallocxx.o:		libmalloccc.xx libmallocxx.h

# misc-character
toxc.o:			toxc.c toxc.h
char.o:			char.cc char.h
mkchar.o:		mkchar.c mkchar.h
ischarx.o:		ischarx.cc ischarx.h
hasx.o:			hasx.cc hasx.h char.h ischarx.h

rmeol.o:		rmeol.c rmx.h
rmext.o:		rmext.c rmx.h
rmchr.o:		rmchr.c rmx.h

# system-auxillary-information
ucsysauxinfo.o:		ucsysauxinfo.dir
ucsysauxinfo.dir:
	makebelow $@

six.o:			six.dir
six.dir:
	makebelow $@

sfx.o:			sfx.dir
sfx.dir:
	makebelow $@

snwcpyx.o:		snwcpyx.dir
snwcpyx.dir:
	makebelow $@

# UCENT
ucentpw.o:		ucentpw.cc ucentpw.h
ucentsp.o:		ucentsp.cc ucentsp.h
ucentua.o:		ucentua.cc ucentua.h
ucentgr.o:		ucentgr.cc ucentgr.h
ucentpj.o:		ucentpj.cc ucentpj.h
ucentpr.o:		ucentpr.cc ucentpr.h
ucentnw.o:		ucentnw.cc ucentnw.h
ucentho.o:		ucentho.cc ucentho.h
ucentsv.o:		ucentsv.cc ucentsv.h

# UCSYS
ucsys.o:		ucsys.dir
ucsys.dir:
	makebelow $@

# UCENT
ucent.o:		ucent.dir
ucent.dir:
	makebelow $@

# UCGET
ucget.o:		ucget.dir
ucget.dir:
	makebelow $@

# UCENUM
ucenum.o:		ucenum.dir
ucenum.dir:
	makebelow $@

# UCINET
ucinetconv.o:		ucinetconv.cc ucinetconv.h

# ENCODE-DECODE
b64decover.o:		b64decoder.cc b64decoder.h

# tab handling
ncol.o:			ncol.c ncol.h

# other IS
isinetaddr.o:		isinetaddr.cc isinetaddr.h

# INET
inetconv.o:		inetconv.cc inetconv.h

# LIBU
um.o:			um.cc um.h
ustr.o:			ustr.cc ustr.h
umsg.o:			umsg.cc umsg.h

# LIBUC
ucmemla.o:		ucmemla.cc ucmemla.h


nodedb.o:		nodedb.cc nodedb.h
bits.o:			bits.cc bits.h
envs.o:			envs.cc envs.h
envs_procxe.o:		envs_procxe.cc envs.h
expcook.o:		expcook.cc expcook.h


# sring-comparisons
vstrcmpx.o:		vstrcmpx.c vstrcmpx.h
vstrkeycmpx.o:		vstrkeycmpx.c vstrkeycmpx.h
vstrkeydictcmp.o:	vstrkeydictcmp.c vstrkeycmpx.h

# string-constants
stdfnames.o:		stdfnames.c stdfnames.h
syhsdbfnames.o:		sysdbfnames.c sysdbfnames.h

# UTILITY
inetaddrparse.o:	inetaddrparse.cc inetaddrparse.h
ids.o:			ids.cc ids.h
xperm.o:		xperm.cc xperm.h ids.h
isoneof.o:		isoneof.c isoneof.h
isnot.o:		isnot.cc isnot.h isoneof.h
iserror.o:		iserror.cc iserror.h isoneof.h



# testing

X01= testugetpw

X01OBJ= $(X01).o ugetpw.o ucpwcache.o recarr.o

$(X01).o:		$(X01).c ugetpw.h ucpwcache.h recarr.h

$(X01).x:		$(X01OBJ)
	$(CC) -o $@ $(XLDFLAGS) $(X01OBJ) $(XLIBINFO)


X02= testutmpacc

X02OBJ= $(X02).o utmpacc.o utmpaccent.o filemap.o recarr.o

$(X02).o:		$(X02).c utmpacc.h utmpaccent.h filemap.h recarr.h

$(X02).x:		$(X02OBJ)
	$(CC) -o $@ $(XLDFLAGS) $(X02OBJ) $(XLIBINFO)


#XDIAL0= dialticotsordnls.o dialticotsordmux.o dialticotsord.o 
XDIAL1= filebuf.o opensysfs.o
XDIAL2= nlsdialassist.o strnwcpy.o
XDIAL3= snopenflags.o
XDIAL4= uc_reade.o uc_openuser.o uc_opensys.o uc_opendev.o

XDIALERS= $(XDIAL0) $(XDIAL1) $(XDIAL2) $(XDIAL3) $(XDIAL4)


X03= testucopen

X03OBJ00= $(X03).o mkprogenv.o spawnproc.o
X03OBJ01= uc_openinfo.o uc_openproto.o uc_openprog.o 
X03OBJ02= uc_opendialer.o uc_openfsvc.o uc_openusvc.o uc_execve.o
X03OBJ03= $(XDIALERS)

X03OBJ04= attachso.o opensvc_hotd.o
X03OBJ05= mkexpandpath.o mkuserpath.o mkvarpath.o mkcdpath.o
X03OBJ06=
X03OBJ07= debug.o

X03OBJa= $(X03OBJ00) $(X03OBJ01) $(X03OBJ02) $(X03OBJ03) 
X03OBJb= $(X03OBJ04) $(X03OBJ05) $(X03OBJ06) $(X03OBJ07)
X03OBJ= $(X03OBJa) $(X03OBJb)

$(X03).o:		$(X03).c usystem.h nlsdialassist.h

$(X03).x:		$(X03OBJ)
	$(CC) -o $@ $(XLDFLAGS) $(X03OBJ) $(XLIBINFO)


X04= testsafesleep

X04OBJ00= $(X04).o 
X04OBJ01=
X04OBJ02=
X04OBJ03=
X04OBJ04=
X04OBJ05= debug.o

X04OBJ= $(X04OBJ00) $(X04OBJ01) $(X04OBJ02) $(X04OBJ03) $(X04OBJ04) $(X04OBJ05)

$(X04).o:		$(X04).c usystem.h

$(X04).x:		$(X04OBJ)
	$(CC) -o $@ $(XLDFLAGS) $(X04OBJ) $(XLIBINFO)


X05= testspawnproc

X05OBJ00= $(X05).o 
X05OBJ01= spawnproc.o
X05OBJ02=
X05OBJ03=
X05OBJ04=
X05OBJ05= debug.o

X05OBJ= $(X05OBJ00) $(X05OBJ01) $(X05OBJ02) $(X05OBJ03) $(X05OBJ04) $(X05OBJ05)

$(X05).o:		$(X05).c usystem.h

$(X05).x:		$(X05OBJ)
	$(CC) -o $@ $(XLDFLAGS) $(X05OBJ) $(XLIBINFO)


X06= testugetpid

X06OBJ00= $(X06).o 
X06OBJ01= ucgetpid.o
X06OBJ02= ucatfork.o
X06OBJ03=
X06OBJ04=
X06OBJ05= debug.o
X06OBJ06=
X06OBJ07=

X06OBJa= $(X06OBJ00) $(X06OBJ01) $(X06OBJ02) $(X06OBJ03) 
X06OBJb= $(X06OBJ04) $(X06OBJ05) $(X06OBJ06) $(X06OBJ07)

X06OBJ= $(X06OBJa) $(X06OBJb)

$(X06).o:		$(X06).c usystem.h

$(X06).x:		$(X06OBJ)
	$(CC) -o $@ $(XLDFLAGS) $(X06OBJ) $(XLIBINFO)


X07= testnaked

X07OBJ00= $(X07).o 
X07OBJ01=
X07OBJ02=
X07OBJ03=
X07OBJ04=
X07OBJ05=
X07OBJ06=
X07OBJ07=

X07OBJa= $(X07OBJ00) $(X07OBJ01) $(X07OBJ02) $(X07OBJ03) 
X07OBJb= $(X07OBJ04) $(X07OBJ05) $(X07OBJ06) $(X07OBJ07)

X07OBJ= $(X07OBJa) $(X07OBJb)

$(X07).o:		$(X07).c usystem.h

$(X07).x:		$(X07OBJ)
	$(CC) -o $@ $(XLDFLAGS) $(X07OBJ) $(XLIBINFO)


# testcharconv
X08= testcharconv

$(X08):			$(X08).x

X08OBJ = $(X08).o
X08OBJ += ctdec.o
X08OBJ += cfdec.o 
X08OBJ += digbufsizes.o
X08OBJ += sncpyx.o
X08OBJ += getbufsize.o

$(X08).o:		$(X08).cc usystem.h

$(X08).x:		$(X08OBJ)
	print -- $(LD) -o $@ $(X08OBJ)

UCMAOBJ =
UCMAOBJ += ucmalloc.o
UCMAOBJ += ucmallocsyc.o
UCMAOBJ += ucmallocsyc.o

# testmemtrack
MTOBJ = testmemtrack.o mapblock.o memtrack.o

testmemtrack.x:		$(MTOBJ)
	$(LD) -o $@ $(MTOBJ)

testmemtrack.o:		testmemtrack.cc

# TESYVALUES
OBJ_TV= testvalues.o digbufsizes.o valuelimits.o ucvariables.o
testvalues.x:	$(OBJ_TV)
	$(LD) -o $@ $(OBJ_TV)



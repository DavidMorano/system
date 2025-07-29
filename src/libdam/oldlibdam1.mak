# MAKEFILE (libdam)

T= libdam

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

I00= vsystem.h exitcodes.h localmisc.h
I01= vechand.h vecstr.h vecitem.h vecobj.h vecint.h veclong.h vecelem.h
I02= hdb.h lookaside.h pq.h plainq.h
I03= field.h 
I04= egs.h randmwc.o random.h
I05= dater.h zdb.h zos.h tmz.h zoffparts.h
I06= termstr.h
I07= kinfo.h loadave.h fsdir.h fsdirtree.h
I08= logfile.h lfm.h lastlogfile.h tmpx.h
I09= storeitem.h 
I10= mallocstuff.h userinfo.h openport.h
I11= char.h ansigr.h baops.h bwops.h
I12= envs.h
I13= raqhand.h
I14=
I15=

INCA= $(I00) $(I01) $(I02) $(I03)
INCB= $(I04) $(I05) $(I06) $(I07)
INCC= $(I08) $(I09) $(I10) $(I11)
INCD= $(I12) $(I13) $(I14) $(I15)
INCE= $(I16) $(I17) $(I18) $(I19) 
INCF= $(I20) $(I21) $(I22) $(I32)

INCS= $(INCA) $(INCB) $(INCC) $(INCD) $(INCE) $(INCF) 

LIBS +=


INCDIRS=

LIBDIRS= -L$(LIBDIR) -L$(CGS_LIBDIR)


RUNINFO= -rpath $(RUNDIR)

LIBINFO= $(LIBDIRS) $(LIBS)

# flag setting
CPPFLAGS	?= $(DEFS) $(INCDIRS) $(MAKECPPFLAGS)
CFLAGS		?= $(MAKECFLAGS)
CXXFLAGS	?= $(MAKECXXFLAGS)
ARFLAGS		?= $(MAKEARFLAGS)
LDFLAGS		?= $(MAKELDFLAGS)


INSTALLINC0= install-inca install-incb install-incc install-incd
INSTALLINC1= install-ince install-incf install-incg install-inch
INSTALLINC2= install-inci install-incj install-inck install-incl
INSTALLINC3= install-incm install-incn install-inco install-incp
INSTALLINC4= install-incq install-incr install-incs install-inct
INSTALLINC5= install-incu install-incv

INSTALLINCa= $(INSTALLINC0) $(INSTALLINC1) $(INSTALLINC2) $(INSTALLINC3)
INSTALLINCb= $(INSTALLINC4) $(INSTALLINC5)

INSTALLINCS= $(INSTALLINCa) $(INSTALLINCb)


OBJ00= substring.o strwhite.o strdomain.o 
OBJ01= strbasename.o strdirname.o strshrink.o 
OBJ02= strdictcmp.o strnndictcmp.o
OBJ03= strkeycmp.o strkeydictcmp.o
OBJ04= stremacmp.o
OBJ05= strsub.o strleadcmp.o strnleadcmp.o strtoken.o strrpbrk.o
OBJ06=
OBJ07= vstrkeycmp.o vstrkeydictcmp.o vstrcmp.o vstrcmpr.o

OBJ08= strcasestr.o strwcmp.o strwcasecmp.o strpcmp.o strnncmp.o
OBJ09= strsub.o strcasesub.o strnsub.o strncasesub.o
OBJ10= 
OBJ11= strnlen.o strnnlen.o strlinelen.o
OBJ12= starmat.o 
OBJ13= matstr.o matostr.o matpstr.o matpcasestr.o
OBJ14= matkeystr.o matnstr.o matsubstr.o matcasesubstr.o
OBJ15=

OBJ16= mallocstuff.o fileobject.o 
OBJ17= 
OBJ18= 
OBJ19= 
OBJ20= strcpyxc.o
OBJ21= strwcpy.o strwcpyxc.o strwcpyrev.o
OBJ22= strwcpycompact.o strwcpyopaque.o strwcpyblanks.o strwcpywide.o
OBJ23= strncpyfc.o strncpyuc.o strncpyfc.o strncpyblanks.o

OBJ24= rexecl.o rcmdu.o qualdisplay.o 
OBJ25= rex.o rfile.o lockfile.o lockend.o
OBJ26= timestr_date.o timestr_nist.o timestr_elapsed.o timevalstr_ulog.o 
OBJ27= timestr_scandate.o 
OBJ28=
OBJ29= 
OBJ30= chmods.o chowns.o readignore.o inetping.o termdevice.o 
OBJ31= pathclean.o makedirs.o mkdirs.o chownsame.o tmpmailboxes.o 

OBJ32=
OBJ33=
OBJ34=
OBJ35= sfsub.o sfcasesub.o sfkey.o sfbreak.o sfskipwhite.o sfshrink.o sfword.o
OBJ36= sfdirname.o sfbasename.o sfwhitedot.o sfprogroot.o sfdequote.o
OBJ37= sfcookkey.o sfsubstance.o sfthing.o sfcenter.o sfrootname.o sfnamecomp.o
OBJ38= sichr.o sicasechr.o sialpha.o sidigit.o sialnum.o sidquote.o sinext.o
OBJ39= sisub.o sicasesub.o sibreak.o siskipwhite.o sibasename.o sispan.o 

OBJ40= getenv3.o
OBJ41= permsched.o
OBJ42=
OBJ43=
OBJ44= dialticotsord.o dialticotsordnls.o
OBJ45=
OBJ46= opentermnote.o openport.o opendefstds.o
OBJ47= openusd.o

OBJ48= wdt.o base64.o netorder.o stdorder.o getserial.o log2.o logbase.o
OBJ49=
OBJ50= readn.o writen.o writeto.o acceptpass.o passfd.o
OBJ51=
OBJ52= mailaddrquote.o compactstr.o
OBJ53= randmwc.o
OBJ54= wchar_iswhite.o toxc.o baops.o 
OBJ55= 

# these next two are in LINBUC
#OBJ56= isalphalatin.o isalnumlatin.o isdigitlatin.o ishexlatin.o
#OBJ57= isdict.o isprintlatin.o isprintbad.o
OBJ58= islowerlatin.o isupperlatin.o isprintterm.o iswhite.o iseol.o rmeol.o
OBJ59= isindomain.o issamehostname.o isleapyear.o isfnamespecial.o
# this next one is in LIBUC
#OBJ60= hasuc.o haslc.o haswhite.o 
OBJ60= hasalpha.o hasnum.o
OBJ61= hasalllc.o hasalluc.o hasallalnum.o hasallalpha.o hasnonwhite.o
OBJ62= hasalldig.o hasallbase.o hasprintbad.o hasallminus.o hasallplusminus.o
OBJ63= hasdoublewhite.o hasallwhite.o hasfext.o hasallof.o

OBJ64= optval.o
OBJ65= freadln.o hasMeAlone.o hasEOH.o
OBJ66= fbreadline.o fbread.o fbwrite.o fbprint.o fbprintf.o
OBJ67= nextpowtwo.o hashelf.o hashagain.o urotate.o
OBJ68= fmeanvaral.o fmeanvarai.o cpuspeed.o
OBJ69= sncpyarray.o snrealname.o snxtilook.o snxtierr.o
OBJ70= snddd.o snscs.o snses.o snsdd.o snabbr.o snkeyval.o
OBJ71= sncpy.o sncpylc.o sncpyuc.o sncpyfc.o sncpyw.o

OBJ72= snwcpy.o snwcpylc.o snwcpyuc.o snwcpyfc.o snwcpyshrink.o snwcpyrev.o
OBJ73= snwcpyhyphen.o snwcpylatin.o snwcpyclean.o snwcpycompact.o snwcpyopaque.o
OBJ74= snwcpywidehdr.o strwset.o 
OBJ75= snfsflags.o snopenflags.o snpollflags.o snfilemode.o snshellunder.o 
OBJ76= snsigabbr.o snerrabbr.o snloadavg.o sninetaddr.o sntmtime.o snmkuuid.o
OBJ77= sntid.o sncat1.o sncat2.o
OBJ78= strdcpy.o strdcpyw.o strdcpycompact.o strdcpyopaque.o strdcpyclean.o
OBJ79= strnwcpy.o strnncpy.o strnset.o strdfill.o

OBJ80=  nathai.o
OBJ81= 
OBJ82= density.o densitystati.o densitystatll.o denpercents.o
OBJ83= tmstrs.o shellunder.o udomain.o
OBJ84= mktmpuserdir.o mkcaselower.o mkfmtphone.o
OBJ85= mklogid.o mkplogid.o mksublogid.o mkquoted.o mkutmpid.o mkmagic.o 
OBJ86= mktmpfile.o mktmplock.o mkjobfile.o mkdatefile.o mkuuid.o mkchar.o
OBJ87= nchr.o mnwcpy.o

OBJ88= tcgetlines.o tcsetlines.o tcpeek.o tcsetmesg.o tcsetbiff.o tcsetown.o
OBJ89= tcgetws.o tcsetws.o
OBJ90=
OBJ91= getus.o getua.o getpe.o getse.o getne.o gethe.o getus.o getua.o
OBJ92= unlinkd.o mapshmtmp.o
OBJ93= mkfnamesuf.o mkfname.o mksofname.o mkfdfname.o
OBJ94= mkgecosname.o mkmailname.o mkrealname.o mkuiname.o mkuibang.o 
OBJ95= mkaddrname.o mkbestaddr.o mkbestfrom.o mkdisphdr.o mkaltext.o

OBJ96= memfile.o linehist.o langstate.o sigman.o sighand.o
OBJ97= comparse.o mhcom.o zos.o tmz.o zdb.o zoffparts.o snflags.o
OBJ98= openportmsg.o varsub.o mailalias.o userattr.o 
OBJ99= tmpx.o tmpx_getrunlevel.o tmpx_getuserlines.o tmpx_getsessions.o
OBJ100= mknoise.o rijndael.o expcook.o schedvar.o
OBJ101= termnote.o egs.o random.o bits.o 
OBJ102= loadave.o modload.o userinfo.o pwentry.o 
OBJ103= dw.o fmq.o useraccdb.o csem.o

OBJ104= sesmsg.o msgdata.o
OBJ105= dater.o dater_getdate.o date.o cachetime.o
OBJ106= field.o field_word.o field_wordphrase.o field_srvarg.o field_svcargs.o
OBJ107= lfm.o lastlogfile.o pwi.o raqhand.o spawner.o thrbase.o thrcomm.o
OBJ108= logfile.o logfile_userinfo.o logfile_printfold.o
OBJ109= vecstr_adds.o vecstr_foilcmp.o vecstr_svcargs.o vecstr_srvargs.o
OBJ110= vecstr_avmkstr.o
OBJ111= vecpstr_loadgrusers.o vecpstr_loadpjusers.o vecpstr_addsubdirs.o

OBJ112= strmgr.o dayspec.o bcspec.o termtrans.o uiconv.o chartrans.o obuf.o
OBJ113= hdrdecode.o qpdecoder.o b64decoder.o hexdecoder.o utf8decoder.o
OBJ114= vecitem.o vecint.o veclong.o vecelem.o vecobj.o 
OBJ115=
OBJ116= setint.o osetint.o setstr.o osetstr.o mapstrs.o
OBJ117= msfile.o msfilee.o ebuf.o nodedb.o clusterdb.o kinfo.o
OBJ118= cksum.o sha1.o gecos.o pwfile.o ipasswd.o ema.o fsi.o
OBJ119= q.o plainq.o cq.o fifostr.o fifoitem.o charq.o intiq.o

OBJ120= kvsfile.o paramfile.o strtab.o strstore.o svcfile.o querystr.o
OBJ121= mailmsgmatenv.o mailmsgmathdr.o mailmsgfrom.o mailmsgstage.o
OBJ122= mailbox.o mailbox_getfrom.o
OBJ123= mailmsg.o mailmsg_loadmb.o mailmsg_loadfile.o mailmsg_loadfd.o
OBJ124= mailmsg_enver.o mailmsg_envtimes.o
OBJ125= mailmsgenv.o mailmsghdrval.o mailmsghdrfold.o mailmsghdrs.o 
OBJ126= fsdirtree.o netfile.o realname.o termstr.o filecounts.o
OBJ127= pwcache.o grcache.o gncache.o namecache.o grmems.o

OBJ128= buffer.o buffer_extras.o 
OBJ129= buffer_stropaque.o buffer_strcompact.o buffer_strquote.o
OBJ130= sbuf_termconseq.o sbuf_blanks.o
OBJ131= paramopt.o paramopt_loadone.o
OBJ132= serialbuf.o srvreg.o srvrege.o wordfill.o linefold.o storeitem.o dstr.o 
OBJ133= bufstr.o stackaddr.o outstore.o
OBJ134= hdbstr.o mapstrint.o keyopt.o keyvals.o
OBJ135= envs.o envs_procxe.o envs_subs.o

OBJ136= isSpecialObject.o isDotDir.o isValidMagic.o isStrEmpty.o
OBJ137= isNotValid.o isNotAccess.o isNotLib.o
OBJ138= isBadSend.o isBadRecv.o isBadMsg.o
OBJ139= isasocket.o isinteractive.o isfsremote.o isproc.o
OBJ140= fsdirtreestat.o statvfsdir.o
OBJ141= rmsesfiles.o dircount.o isdirempty.o
OBJ142= prgetprogpath.o prsetfname.o prmktmpdir.o
OBJ143= intceil.o intfloor.o intsat.o

OBJ144= cfdouble.o cfdecf.o cfb26.o
OBJ145= cfdecmf.o cfdect.o cfroman.o
OBJ146= ctb26.o cthexstr.o ctroman.o ctwords.o
OBJ147= quickselecti.o igcd.o minmax.o isort.o
OBJ148= fam.o fhm.o fsum.o binexp.o
OBJ149= permutations.o combinations.o factorial.o fibonacci.o ipowell.o
OBJ150= wsnwcpynarrow.o
OBJ151= wsfnext.o wsinul.o wsichr.o wsirchr.o

OBJ152= getuserorg.o getdefzinfo.o getrealname.o getprojname.o
OBJ153= getev.o getaflen.o getprotofamily.o getlogfac.o getlogpri.o getmjd.o 
OBJ154= msghdr.o cmsghdr.o
OBJ155= readfileline.o readfilestrs.o mailboxappend.o
OBJ156= termescseq.o termconseq.o termconseqi.o hdrextid.o hdrextnum.o 
OBJ157= localgetorg.o localgetorgcode.o localgetorgloc.o 
OBJ158= localgetnetload.o localsetnetload.o localgetsystat.o localsetsystat.o
OBJ159= inetpton.o inetntop.o inet_ntoa_r.o inet4int.o

OBJ160= tabcols.o nextfield.o nextfieldterm.o nextqtoken.o 
OBJ161= nusers.o mesg.o sysmemutil.o initnow.o 
OBJ162= getfiledirs.o findfilepath.o findfile.o findxfile.o
OBJ163= cvttemperature.o dictdiff.o bufprintf.o xfile.o
OBJ164= mapex.o strsigabbr.o strerrabbr.o strlocktype.o straltwchar.o
OBJ165= emainfo.o
OBJ166= listentcp.o listenudp.o listenuss.o listenusd.o 
OBJ167= listenpass.o listenconn.o

OBJ168= getprogexec.o getnodedomain.o getmaxpid.o getrand.o getgid_def.o
OBJ169= getehostname.o getchostname.o getcanonical.o getourhe.o
OBJ170= gethename.o getheaddr.o getcname.o getfstype.o gethz.o getsocktype.o
OBJ171= getsystypenum.o getstacksize.o gettid.o getprovider.o getproviderid.o
OBJ172= getnodename.o getclustername.o getnodeinfo.o getdomainname.o getseed.o
OBJ173= getnprocessors.o getarchitecture.o getnfile.o getpwd.o getrunlevel.o
OBJ174= getlogname.o getloghost.o getutmpterm.o getsocktype.o
OBJ175= getpwentry.o getpwlogname.o getgroupname.o getarchitecture.o

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

OBJM0= $(OBJ96) $(OBJ97) $(OBJ98) $(OBJ99) 
OBJM1= $(OBJ100) $(OBJ101) $(OBJ102) $(OBJ103)
OBJM= $(OBJM0) $(OBJM1)

OBJN0= $(OBJ104) $(OBJ105) $(OBJ106) $(OBJ107) 
OBJN1= $(OBJ108) $(OBJ109) $(OBJ110) $(OBJ111)
OBJN= $(OBJN0) $(OBJN1)

OBJO0= $(OBJ112) $(OBJ113) $(OBJ114) $(OBJ115) 
OBJO1= $(OBJ116) $(OBJ117) $(OBJ118) $(OBJ119)
OBJO= $(OBJO0) $(OBJO1)

OBJP0= $(OBJ120) $(OBJ121) $(OBJ122) $(OBJ123) 
OBJP1= $(OBJ124) $(OBJ125) $(OBJ126) $(OBJ127)
OBJP= $(OBJP0) $(OBJP1)

OBJQ0= $(OBJ128) $(OBJ129) $(OBJ130) $(OBJ131) 
OBJQ1= $(OBJ132) $(OBJ133) $(OBJ134) $(OBJ135)
OBJQ= $(OBJQ0) $(OBJQ1)

OBJR0= $(OBJ136) $(OBJ137) $(OBJ138) $(OBJ139) 
OBJR1= $(OBJ140) $(OBJ141) $(OBJ142) $(OBJ143)
OBJR= $(OBJR0) $(OBJR1)

OBJS0= $(OBJ144) $(OBJ145) $(OBJ146) $(OBJ147) 
OBJS1= $(OBJ148) $(OBJ149) $(OBJ150) $(OBJ151)
OBJS= $(OBJS0) $(OBJS1)

OBJT0= $(OBJ152) $(OBJ153) $(OBJ154) $(OBJ155) 
OBJT1= $(OBJ156) $(OBJ157) $(OBJ158) $(OBJ159)
OBJT= $(OBJT0) $(OBJT1)

OBJU0= $(OBJ160) $(OBJ161) $(OBJ162) $(OBJ163) 
OBJU1= $(OBJ164) $(OBJ165) $(OBJ166) $(OBJ167)
OBJU= $(OBJU0) $(OBJU1)

OBJV0= $(OBJ168) $(OBJ169) $(OBJ170) $(OBJ171) 
OBJV1= $(OBJ172) $(OBJ173) $(OBJ174) $(OBJ175)
OBJV= $(OBJV0) $(OBJV1)

OBJ= $(OBJA) $(OBJB) $(OBJC) $(OBJD) \
	$(OBJE) $(OBJF) $(OBJG) $(OBJH) \
	$(OBJI) $(OBJJ) $(OBJK) $(OBJL) \
	$(OBJM) $(OBJN) $(OBJO) $(OBJP) \
	$(OBJQ) $(OBJR) $(OBJS) $(OBJT) $(OBJU) $(OBJV)

OBJFILE0= obja.o objb.o objc.o objd.o obje.o objf.o objg.o objh.o 
OBJFILE1= obji.o objj.o objk.o objl.o objm.o objn.o objo.o objp.o 
OBJFILE2= objq.o objr.o objs.o objt.o obju.o objv.o

OBJFILE= $(OBJFILE0) $(OBJFILE1) $(OBJFILE2)


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


$(T).so:		$(OBJ) Makefile $(T).a
	$(LD) -G -o $@ $(SLDFLAGS) $(OBJ) $(SLIBINFO)

$(T).a:			$(OBJ)
	$(AR) -rc $@ $?

$(T).nm:		$(T).so
	$(NM) $(NMFLAGS) $(T).so > $(T).nm

$(T).order:		$(OBJ) $(T).a
	$(LORDER) $(T).a | $(TSORT) > $(T).order
	$(RM) $(T).a
	while read O ; do $(AR) -cr $(T).a $${O} ; done < $(T).order


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
	makesafe -v=3 -I $(INCDIR) $(OBJU)
	makesafe -v=3 -I $(INCDIR) $(OBJV)

install-pre:
	filefind . -s h | makenewer -af - -d $(INCDIR)

install:		install-inc Makefile $(ALL)
	makenewer -r $(ALL) -d $(SRCROOT)/lib

install-inc:		install-ih install-io
	touch install-inc

install-ih:		$(INSTALLINCS)
	touch install-ih

install-inca:		$(INCA)
	makenewer -z $? -d $(INCDIR) -t $@

install-incb:		$(INCB)
	makenewer -z $? -d $(INCDIR) -t $@

install-incc:		$(INCC)
	makenewer -z $? -d $(INCDIR) -t $@

install-incd:		$(INCD)
	makenewer -z $? -d $(INCDIR) -t $@

install-ince:		$(INCE)
	makenewer -z $? -d $(INCDIR) -t $@

install-incf:		$(INCF)
	makenewer -z $? -d $(INCDIR) -t $@

install-incg:		$(INCG)
	makenewer -z $? -d $(INCDIR) -t $@

install-inch:		$(INCH)
	makenewer -z $? -d $(INCDIR) -t $@

install-inci:		$(INCI)
	makenewer -z $? -d $(INCDIR) -t $@
install-incj:		$(INCJ)
	makenewer -z $? -d $(INCDIR) -t $@
install-inck:		$(INCK)
	makenewer -z $? -d $(INCDIR) -t $@
install-incl:		$(INCL)
	makenewer -z $? -d $(INCDIR) -t $@
install-incm:		$(INCM)
	makenewer -z $? -d $(INCDIR) -t $@
install-incn:		$(INCN)
	makenewer -z $? -d $(INCDIR) -t $@
install-inco:		$(INCO)
	makenewer -z $? -d $(INCDIR) -t $@
install-incp:		$(INCP)
	makenewer -z $? -d $(INCDIR) -t $@
install-incq:		$(INCQ)
	makenewer -z $? -d $(INCDIR) -t $@
install-incr:		$(INCR)
	makenewer -z $? -d $(INCDIR) -t $@
install-incs:		$(INCS)
	makenewer -z $? -d $(INCDIR) -t $@
install-inct:		$(INCT)
	makenewer -z $? -d $(INCDIR) -t $@
install-incu:		$(INCU)
	makenewer -z $? -d $(INCDIR) -t $@
install-incv:		$(INCV)
	makenewer -z $? -d $(INCDIR) -t $@

install-io:		$(OBJ)
	makenewer -im -z -m o=h $? -d $(INCDIR) -t $@

other:			here
	makenewer -im -z -m o=h $(OBJA) -d $(INCDIR)
	makenewer -im -z -m o=h $(OBJB) -d $(INCDIR)
	makenewer -im -z -m o=h $(OBJC) -d $(INCDIR)
	makenewer -im -z -m o=h $(OBJD) -d $(INCDIR)
	makenewer -im -z -m o=h $(OBJE) -d $(INCDIR)
	makenewer -im -z -m o=h $(OBJF) -d $(INCDIR)
	makenewer -im -z -m o=h $(OBJG) -d $(INCDIR)
	makenewer -im -z -m o=h $(OBJH) -d $(INCDIR)
	makenewer -im -z -m o=h $(OBJI) -d $(INCDIR)
	makenewer -im -z -m o=h $(OBJJ) -d $(INCDIR) -t $@
	makenewer -im -z -m o=h $(OBJK) -d $(INCDIR)
	makenewer -im -z -m o=h $(OBJL) -d $(INCDIR)
	makenewer -im -z -m o=h $(OBJM) -d $(INCDIR)
	makenewer -im -z -m o=h $(OBJN) -d $(INCDIR)
	makenewer -im -z -m o=h $(OBJO) -d $(INCDIR)
	makenewer -im -z -m o=h $(OBJP) -d $(INCDIR)
	makenewer -im -z -m o=h $(OBJQ) -d $(INCDIR)

again:
	$(RM) $(ALL)

clean:			again
	$(RM) *.o $(ALL)

control:
	(uname -n ; date) > Control


obja.o:			$(OBJA)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJA)

objb.o:			$(OBJB)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJB)

objc.o:			$(OBJC)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJC)

objd.o:			$(OBJD)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJD)

obje.o:			$(OBJE)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJE)

objf.o:			$(OBJF)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJF)

objg.o:			$(OBJG)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJG)

objh.o:			$(OBJH)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJH)

obji.o:			$(OBJI)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJI)

objj.o:			$(OBJJ)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJJ)

objk.o:			$(OBJK)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJK)

objl.o:			$(OBJL)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJL)

objm.o:			$(OBJM)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJM)

objn.o:			$(OBJN)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJN)

objo.o:			$(OBJO)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJO)

objp.o:			$(OBJP)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJP)

objq.o:			$(OBJQ)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJQ)

objr.o:			$(OBJR)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJR)

objs.o:			$(OBJS)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJS)

objt.o:			$(OBJT)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJT)

obju.o:			$(OBJU)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJU)

objv.o:			$(OBJV)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJV)


field.o:		field.c field.h

field_word.o:		field_word.c field.h

field_srvarg.o:		field_srvarg.c field.h

dstr.o:			dstr.c dstr.h

fifostr.o:		fifostr.c fifostr.h

fifoitem.o:		fifoitem.c fifoitem.h

fifoelem.o:		fifoelem.c fifoelem.h

vechand.o:		vechand.c vechand.h

vecstr.o:		vecstr.c vecstr.h

vecstr_adduniq.o:	vecstr_adduniq.c vecstr.h

vecstr_loadfile.o:	vecstr_loadfile.c vecstr.h

vecstr_foilcmp.o:	vecstr_foilcmp.c vecstr.h

vecstr_env.o:		vecstr_env.c vecstr.h

vecstr_envfile.o:	vecstr_envfile.c vecstr.h

vecstr_svcargs.o:	vecstr_svcargs.c vecstr.h

vecstr_srvargs.o:	vecstr_srvargs.c vecstr.h

vecpstr.o:		vecpstr.c vecpstr.h

vecpstr_loadfile.o:	vecpstr_loadfile.c vecpstr.h

vecpstr_loadgrusers.o:	vecpstr_loadgrusers.c vecpstr.h

vecpstr_loadpjusers.o:	vecpstr_loadpjusers.c vecpstr.h

vecpstr_envadd.o:	vecpstr_envadd.c vecpstr.h

vecpstr_envfile.o:	vecpstr_envfile.c vecpstr.h

vecitem.o:		vecitem.c vecitem.h

vecobj.o:		vecobj.c vecobj.h

vecint.o:		vecint.c vecint.h

veclong.o:		veclong.c veclong.h

vecelem.o:		vecelem.c vecelem.h

hdb.o:			hdb.c hdb.h

hdbstr.o:		hdbstr.c hdbstr.h

keyvals.o:		keyvals.c keyvals.h

paramopt.o:		paramopt.c paramopt.h

logfile.o:		logfile.c logfile.h
logfile_userinfo.o:	logfile_userinfo.c logfile.h userinfo.h
logfile_printfold.o:	logfile_printfold.c logfile.h

lastlogfile.o:		lastlogfile.c lastlogfile.h

loadave.o:		loadave.c loadave.h

egs.o:			egs.c egs.h

sha1.o:			sha1.c sha1.h

outstore.o:		outstore.c outstore.h
outbuf.o:		outbuf.c outbuf.h

strmgr.o:		strmgr.c strmgr.h

lookaside.o:		lookaside.c lookaside.h

fsi.o:			fsi.cc fsi.h
intiq.o:		intiq.cc intiq.h

serialbuf.o:		serialbuf.c serialbuf.h

netorder.o:		netorder.c netorder.h

storeitem.o:		storeitem.c storeitem.h

matenv.o:		matenv.c matenv.h

mailmsgmatenv.o:	mailmsgmatenv.c mailmsgmatenv.h
mailmsgmathdr.o:	mailmsgmathdr.c mailmsgmathdr.h
mailmsgfrom.o:		mailmsgfrom.c mailmsgfrom.h
mailbox.o:		mailbox.c mailbox.h
mailbox_getfrom.o:	mailbox_getfrom.c mailbox.h
mailmsg.o:		mailmsg.c mailmsg.h mailmsgmatenv.h
mailmsg_loadfile.o:	mailmsg_loadfile.c mailmsg.h
mailmsg_loadfd.o:	mailmsg_loadfile.c mailmsg.h
mailmsg_loadmb.o:	mailmsg_loadmb.c mailmsg.h mailmsgmatenv.h
mailmsg_enver.o:	mailmsg_enver.c mailmsg.h mailmsgmatenv.h
mailmsg_envtimes.o:	mailmsg_envtimes.c mailmsg.h
mailmsg_envdates.o:	mailmsg_envdates.c mailmsg.h
mailmsghdrs.o:		mailmsghdrs.c mailmsghdrs.h mailmsg.h
mailmsgstage.o:		mailmsgstage.c mailmsgstage.h 
mailmsghdrval.o:	mailmsghdrval.c mailmsghdrval.h
mailmsghdrfold.o:	mailmsghdrfold.c mailmsghdrfold.h
mailaddrquote.o:	mailaddrquote.cc mailaddrquote.h

staackaddr.o:		stackaddr.c stsackaddr.h


char.o:			char.c char.h
mkchar.o:		mkchar.c mkchar.h

strcpyxc.o:		strcpyxc.cc strcpyxc.h
strwcpyxc.o:		strwcpyxc.cc strwcpyxc.h
matstr.o:		matstr.cc matstr.h
matostr.o:		matostr.cc matostr.h

comparse.o:		comparse.c comparse.h

mhcom.o:		mhcom.c mhcom.h

dater.o:		dater.c dater.h

dater_getdate.o:	dater_getdate.c dater.h date.h

date.o:			date.c date.h

zdb.o:			zdb.c zdb.h

zos.o:			zos.c zos.h

tmz.o:			tmz.c tmz.h

zoffparts.o:		zoffparts.c zoffparts.h

dayspec.o:		dayspec.c dayspec.h

cvtdater.o:		cvtdater.c cvtdater.h

realname.o:		realname.c realname.h

gecos.o:		gecos.c gecos.h

kinfo.o:		kinfo.c kinfo.h

msfile.o:		msfile.c msfile.h msfilee.h

msfilee.o:		msfilee.c msfilee.h

ebuf.o:			ebuf.c ebuf.h

mapstrint.o:		mapstrint.c mapstrint.h

srvreg.o:		srvreg.c srvreg.h srvrege.h

srvrege.o:		srvrege.c srvrege.h

spawnproc.o:		spawnproc.c spawnproc.h

openport.o:		openport.c openport.h

openportmsg.o:		openportmsg.c openportmsg.h

pmq.o:			pmq.cc pmq.h

fmq.o:			fmq.cc fmq.h

sigblock.o:		sigblock.c sigblock.h
sigman.o:		sigman.c sigman.h
sighand.o:		sighand.c sighand.h

paramfile.o:		paramfile.cc paramfile.h

kvsfile.o:		kvsfile.cc kvsfile.h

svcfile.o:		svcfile.cc svcfile.h
srvtab.o:		srvtab.cc srvtab.h
acctab.o:		acctab.cc acctab.h

schedvar.o:		schedvar.cc schedvar.h

varsub.o:		varsub.cc varsub.h

memfile.o:		memfile.cc memfile.h

baops.o:		baops.cc baops.h

bits.o:			bits.cc bits.h baops.h

msgenv.o:		msgenv.c msgenv.h

rijndael.o:		rijndael.c rijndael.h

ema.o:			ema.c ema.h
mimetypes.o:		mimetypes.c mimetypes.h
randomvar.o:		randomvar.c randomvar.h
fsdir.o:		fsdir.c fsdir.h
fsdirtree.o:		fsdirtree.c fsdirtree.h

density.o:		density.c density.h
dialtab.o:		dialtab.c dialtab.h
optval.o:		optval.cc optval.h

clusterdb.o:		clusterdb.c clusterdb.h
hostinfo.o:		hostinfo.cc hostinfo.h
hostaddr.o:		hostaddr.cc hostaddr.h
hostent.o:		hostent.cc hostent.h
inetaddr.o:		inetaddr.c inetaddr.h

lfm.o:			lfm.cc lfm.h

keyopt.o:		keyopt.cc keyopt.h

strpack.o:		strpack.cc strpack.h
strtab.o:		strtab.cc strtab.h
strstore.o:		strstore.cc strstore.h

userattr.o:		userattr.c userattr.h
pwfile.o:		pwfile.c pwfile.h pwentry.h
ts.o:			ts.cc ts.h

dir:
	makesubdir $@

# DATABASE
database.o:		database.dir
database.dir:
	makesubdir $@

# IPAASWD
ipasswd.o:		ipasswd.dir
ipasswd.dir:
	makesubdir $@


# testing

X01= testgrcache

X01OBJ= $(X01).o grcache.o groupent.o cq.o

$(X01).o:		$(X01).c grcache.h groupent.h cq.h

$(X01).x:		$(X01OBJ)
	$(CC) -o $@ $(X01OBJ) $(XLIBINFO)


X02= testgetuserhome

X02OBJ= $(X02).o getuserhome.o

$(X02).o:		$(X02).c getxusername.h getax.h ugetpw.h

$(X02).x:		$(X02OBJ)
	$(CC) -o $@ $(X02OBJ) $(XLIBINFO)


X03= testgetrunlevel

X03OBJ= $(X03).o getrunlevel.o

$(X03).o:		$(X03).c tmpx.h

$(X03).x:		$(X03OBJ)
	$(CC) -o $@ $(X03OBJ) $(XLIBINFO)


X04= testcomparse

X04OBJ= $(X04).o comparse.o

$(X04).o:		$(X04).c comparse.h

$(X04).x:		$(X04OBJ)
	$(CC) -o $@ $(X04OBJ) $(XLIBINFO)


X05= testmkuserpath

X05OBJ= $(X05).o mkuserpath.o

$(X05).o:		$(X05).c getxusername.h getax.h ugetpw.h

$(X05).x:		$(X05OBJ)
	$(CC) -o $@ $(X05OBJ) $(XLIBINFO)


X06= testmkuuid

X06OBJ= $(X06).o mkuuid.o snmkuuid.o debug.o

$(X06).o:		$(X06).c mkuuid.h snmkuuid.h

$(X06).x:		$(X06OBJ)
	$(CC) -o $@ $(X06OBJ) $(XLIBINFO)


X07= testhasallof

X07OBJ= $(X07).o hasallof.o

$(X07).o:		$(X07).c

$(X07).x:		$(X07OBJ)
	$(CC) -o $@ $(X07OBJ) $(XLIBINFO)


X08= testnifinfo

X08OBJ= $(X08).o nifinfo.o

$(X08).o:	$(X08).c $(X08).h

$(X08).x:	$(X08OBJ)
	$(CC) -o $@ $(X08OBJ) $(XLIBINFO)


X09= testjunker

X09OBJ= $(X09).o 

$(X09).o:	$(X09).c

$(X09).x:	$(X09OBJ)
	$(CC) -o $@ $(X09OBJ) $(XLIBINFO)


X10= testishex

X10OBJ= $(X10).o ishexlatin.o

$(X10).o:	$(X10).c 

$(X10).x:	$(X10OBJ)
	$(CC) -o $@ $(X10OBJ) $(XLIBINFO)


X11= testminmax

X11OBJ= $(X11).o minmax.o

$(X11).o:	$(X11).c 

$(X11).x:	$(X11OBJ)
	$(CC) -o $@ $(X11OBJ) $(XLIBINFO)


X12= testigcd

X12OBJ= $(X12).o igcd.o

$(X12).o:	$(X12).c 
	$(CC) -c -g -Wall $(XCFLAGS) $(CPPFLAGS) $(X12).c

$(X12).x:	$(X12OBJ)
	$(CC) -o $@ $(X12OBJ) $(XLIBINFO)


X13= testpartitionai

X13OBJ= $(X13).o partitionai.o arrswap.o

$(X13).o:	$(X13).c 
	$(CC) -c -g -Wall $(XCFLAGS) $(CPPFLAGS) $(X13).c

$(X13).x:	$(X13OBJ)
	$(CC) -o $@ $(X13OBJ) $(XLIBINFO)



/* libdam HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* general purpose utilities */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-13, David A-D- Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2017 David A­D­ Morano.  All rights reserved. */
/* Use is subject to license terms. */

#ifndef	LIBDAM_INCLUDE
#define	LIBDAM_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>

#include	<acctab.h>
#include	<ansigr.h>
#include	<arrswap.h>
#include	<ass.h>
#include	<authfile.h>
#include	<ba.h>
#include	<bfliner.h>
#include	<burn.h>
#include	<calcite.h>
#include	<calent.h>
#include	<calmgr.h>
#include	<calstrs.h>
#include	<cfroman.h>
#include	<cgi.h>
#include	<checkonc.h>
#include	<cksum.h>
#include	<clientinfo.h>
#include	<cm.h>
#include	<cmbuf.h>
#include	<cmdmap.h>
#include	<codebal.h>
#include	<compactstr.h>
#include	<conallof.h>
#include	<configfile.h>
#include	<connection.h>
#include	<conslog.h>
#include	<cvttemperature.h>
#include	<dayofmonth.h>
#include	<deffile.h>
#include	<density.h>
#include	<densitydb.h>
#include	<densitydbe.h>
#include	<detdupai.h>
#include	<devpermfile.h>
#include	<dialcprogmsg.h>
#include	<dialtab.h>
#include	<digsmall.h>
#include	<dircount.h>
#include	<dirdb.h>
#include	<dirlist.h>
#include	<dirseen.h>
#include	<dispatcher.h>
#include	<ds.h>
#include	<dw.h>
#include	<ecmsg.h>
#include	<ee.h>
#include	<egs.h>
#include	<eigendb.h>
#include	<envstandards_darwin.h>
#include	<envstandards_mac.h>
#include	<envstandards_solaris.h>
#include	<expandline.h>
#include	<fhm.h>
#include	<filecounts.h>
#include	<finduid.h>
#include	<freadln.h>
#include	<fsdirtreestat.h>
#include	<fsi.h>
#include	<ftpsession.h>
#include	<getclusters.h>
#include	<getehostname.h>
#include	<getfiledirs.h>
#include	<getfstype.h>
#include	<getlogx.h>
#include	<getostype.h>
#include	<getprogexec.h>
#include	<getprogroot.h>
#include	<getprovider.h>
#include	<getseed.h>
#include	<getsrcname.h>
#include	<getuserterm.h>
#include	<holidayer.h>
#include	<holidays.h>
#include	<htm.h>
#include	<ifaddrlist.h>
#include	<incfile_rfilewrite.h>
#include	<inetping.h>
#include	<ismatstar.h>
#include	<ismmclass.h>
#include	<issue.h>
#include	<jobdb.h>
#include	<kbdinfo.h>
#include	<keyopt.h>
#include	<keysym.h>
#include	<keysymer.h>
#include	<keyvals.h>
#include	<kinfo.h>
#include	<ktag.h>
#include	<langstate.h>
#include	<lfm.h>
#include	<librandom.h>
#include	<linecenter.h>
#include	<linefold.h>
#include	<linehist.h>
#include	<loadave.h>
#include	<localmisc.h>
#include	<lockmemalloc.h>
#include	<logbase.h>
#include	<logzones.h>
#include	<lookaddr.h>
#include	<lookword.h>
#include	<mailaddrquote.h>
#include	<mailalias.h>
#include	<mailboxappend.h>
#include	<mailenvelope.h>
#include	<makedirs.h>
#include	<md5calc.h>
#include	<mkdirlist.h>
#include	<motd.h>
#include	<msflag.h>
#include	<msgbuf.h>
#include	<msgdata.h>
#include	<msgidx.h>
#include	<msumsg.h>
#include	<muximsg.h>
#include	<mxalias.h>
#include	<naturalwords.h>
#include	<nchr.h>
#include	<netfile.h>
#include	<nettime.h>
#include	<newobj.h>
#include	<ng.h>
#include	<nistinfo.h>
#include	<nlspeername.h>
#include	<nodesearch.h>
#include	<nodesfile.h>
#include	<numincr.h>
#include	<offindex.h>
#include	<onc.h>
#include	<optval.h>
#include	<outfmt.h>
#include	<outline.h>
#include	<outstore.h>
#include	<paramopt.h>
#include	<passfd.h>
#include	<pdb.h>
#include	<percache.h>
#include	<pingstatdb.h>
#include	<poller.h>
#include	<procse.h>
#include	<proginfo.h>
#include	<progsig.h>
#include	<querystr.h>
#include	<quote.h>
#include	<randmwc.h>
#include	<readcmdkey.h>
#include	<recipient.h>
#include	<removename.h>
#include	<rex.h>
#include	<rfile.h>
#include	<rijndael.h>
#include	<rmsesfiles.h>
#include	<rsaglobal.h>
#include	<rsaref.h>
#include	<rtags.h>
#include	<schedvar.h>
#include	<searchkeys.h>
#include	<sesnotes.h>
#include	<sfill.h>
#include	<shadowent.h>
#include	<shiftreg.h>
#include	<shio.h>
#include	<shmalloc.h>
#include	<siletter.h>
#include	<simsg.h>
#include	<sockpeername.h>
#include	<sortvec.h>
#include	<spellchecks.h>
#include	<statmsg.h>
#include	<statvfsdir.h>
#include	<stdsio.h>
#include	<straltwchar.h>
#include	<streamsync.h>
#include	<strfilter.h>
#include	<stubrpc.h>
#include	<syshelper.h>
#include	<sysnamedb.h>
#include	<sysrealname.h>
#include	<systems.h>
#include	<taginfo.h>
#include	<td.h>
#include	<termcharsets.h>
#include	<termenq.h>
#include	<terment.h>
#include	<termnote.h>
#include	<termout.h>
#include	<termtrans.h>
#include	<textlook.h>
#include	<tmpmailboxes.h>
#include	<ts.h>
#include	<tse.h>
#include	<txtindex.h>
#include	<useraccdb.h>
#include	<userinfo.h>
#include	<utmpx_darwin.h>
#include	<utmpx_portable.h>
#include	<utmpx_solaris.h>
#include	<uux.h>
#include	<votdc.h>
#include	<wdt.h>
#include	<wordfill.h>
#include	<writeto.h>
#include	<xwords.h>


#endif /* LIBDAM_INCLUDE */



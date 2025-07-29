/* comsat_csmsg SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* process a COMSAT message */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* compile-time */
#define	CF_DEBUG	0		/* switchable debug print-outs */
#define	CF_OUTINFO	1		/* object OUTINFO */

/* revision history:

	= 1998-03-01, David A­D­ Morano
	The subroutine was written from scratch.

	= 2017-11-03, David A­D­ Morano
	I swapped out |strftime(3c)| for |sntmtime(3dam)| but it
	was, in the end, quite unnecessary. I changed the desired
	time-string specification on |strftime(3c)| and it failed
	properly as it was supposed to (due to result buffer overrun),
	but it took me a while to figure it out. I changed to
	|sntmtime(3dam)| to get a clear error condition (which I
	then fixed by increasing the result buffer size). In the
	end, I left the desired time-string specifiction the way
	it was, so this whole foray was not really necessary.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Here we process a COMSAT message.

	Notes:

	= What is the format of the note printed out to the user terminals?

¶ <username> <time> « <from> - <subj>

	where:

	field		characters	sum
	--------------------------------------------
	¶		1		1
	_		1		2
	<username)	8		10
	_		1		11
	<time>		5		16
	_		1		17
	«		1		18
	_		1		19
	<from>		35		54
	_		1		55
	-		1		56
	_		1		57
	<subj>		80-everything_previous	80-57=23


	= What kind of concurrency do we get here?

	Not as much as I would have liked!  We do not get nearly
	as much concurrency as I would have liked.  Why not?  Because
	the object TERMNOTE is not thread-safe.  It is reentrant
	(as all of our code is and always has been) but that does
	not make it thread-safe subroutine-by-subroutine.  It is
	not thread-safe because it uses an underlying object named
	TMPX which is not thread-safe itself.  Both of these objects
	could be made -- rather easily -- thread-safe but it just
	has not been done.  If someday they (TMPX and TERMNOTE)
	become thread-safe, then this whole enchilada would become
	almost completely concurrent.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<sys/timeb.h>		/* for |struct timeb| */
#include	<unistd.h>
#include	<fcntl.h>
#include	<ctime>			/* |strftime(3c)| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strcpy(3c)| */
#include	<new>			/* placement-new + |nothrow(3c++)| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<bfile.h>
#include	<mailmsg.h>
#include	<mailmsghdrs.h>
#include	<termnote.h>
#include	<tmtime.hh>
#include	<sntmtime.h>
#include	<mkx.h>
#include	<wsix.h>
#include	<timestr.h>
#include	<compactstr.h>
#include	<localmisc.h>
#include	<comsatmsg.h>

#include	"defs.h"
#include	"progcs.h"


/* local defines */

#ifndef	SUBJBUFLEN
#define	SUBJBUFLEN	(2*COLUMNS)
#endif

#ifndef	NOTEBUFLEN
#define	NOTEBUFLEN	COLUMNS
#endif

#define	DATEBUFLEN	16		/* enough for "dd HH:MM" */
#define	MAXOVERLEN	22
#define	MAXFROMLEN	35

#define	PI		proginfo

#define	OI		outinfo


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */

extern "C" {
    extern int	wciswhite(wchar_t) noex ;
}

extern int	progerr_printf(PI *,cchar *,...) noex ;
extern int	progloglock_printf(PI *,cchar *,...) noex ;
extern int	prognote_write(PI *,cchar **,int,int,cchar *,int) noex ;

#if	CF_DEBUGS || CF_DEBUG
extern int	debugprintf(cchar *,...) noex ;
extern int	strlinelen(cchar *,int,int) noex ;
#endif


/* external variables */


/* local structures */

#if	CF_OUTINFO
struct outinfo {
	PROGINFO	*pip ;
	MAILMSG		*mmp ;
	cchar		*un ;		/* supplied by caller */
	wchar_t		*fbuf ;		/* wide-char */
	wchar_t		*sbuf ;		/* wide-char */
	char		*ofbuf ;
	char		*osbuf ;
	int		dl, ul ;
	int		flen, fl ;
	int		slen, sl ;
	int		oflen ;
	int		oslen ;
	int		ll ;
	int		tl ;		/* "total" length */
	int		dlen ;
	char		dbuf[DATEBUFLEN+1] ;
} ;
#endif /* CF_OUTINFO */


/* forward references */

static int	progcsmsger(PI *,int,off_t,cchar *) ;
static int	proclogmsg(PI *,COMSATMSG_MO *) ;
static int	procmsginfo(PI *,MAILMSG *,cchar *) ;
static int	procmsgbad(PI *,cchar *,off_t,int) ;
static int	getdateinfo(PI *,char *,int,cchar *,int,int) ;

#if	CF_OUTINFO
static int	outinfo_start(OI *,PI *,MAILMSG *,cchar *) ;
static int	outinfo_finish(OI *) ;
static int	outinfo_hdrs(OI *) ;
static int	outinfo_getfrom(OI *,cchar **) ;
static int	outinfo_mkfrom(OI *) ;
static int	outinfo_cvtfrom(OI *,cchar *,int) ;
static int	outinfo_getsubj(OI *,cchar **) ;
static int	outinfo_mksubj(OI *) ;
static int	outinfo_cvtsubj(OI *,cchar *,int) ;
static int	outinfo_mkdate(OI *) ;
static int	outinfo_adjust(OI *) ;
static int	outinfo_cols(OI *) ;
static int	outinfo_termbegin(OI *) ;
static int	outinfo_termend(OI *) ;
static int	outinfo_termbeginfrom(OI *) ;
static int	outinfo_termbeginsubj(OI *) ;
static int	outinfo_print(OI *) ;
#endif /* CF_OUTINFO */

static int	wsfnormfrom(const wchar_t *,int) noex ;

static bool	isNoMsg(int) noex ;
static bool	isBadMsg(int) noex ;
static bool	isBadTime(int) noex ;


/* external variables */


/* local structures */


/* forward references */


/* local variables */

constexpr int		rsnomsg[] = {
	SR_NOMSG,
	SR_NOENT,
	0
} ;

constexpr int		rsbadmsg[] = {
	SR_BADMSG,
	SR_INVALID,
	SR_DOM,
	SR_RANGE,
	SR_OVERFLOW,
	0
} ;

constexpr int		rsbadtime[] = {
	SR_BADMSG,
	SR_INVALID,
	SR_DOM,
	SR_RANGE,
	0
} ;


/* exported variables */


/* exported subroutines */

int progcsmsg(PI *pip,cchar *mbuf,int mlen) noex {
	COMSATMSG_MO	m0 ;
	int		rs ;
	int		ml = mlen ;
	int		wlen = 0 ;
	cchar		*pn = pip->progname ;
	char		*mp = (char *) mbuf ;

#if	CF_DEBUG
	if (DEBUGLEVEL(5)) {
	    debugprintf("progcsmsg: ent\n") ;
	    debugprintf("progcsmsg: msg=>%r<\n",mbuf,strlinelen(mbuf,mlen,40)) ;
	}
#endif

	if (mbuf == nullptr) return SR_FAULT ;
	if (mbuf[0] == '\0') return SR_INVALID ;

#ifdef	COMMENT
	if (pip->debuglevel > 0) {
	    progerr_printf(pip,"%s: msg=>%r<\n",pn,
	        mbuf,strlinelen(mbuf,mlen,40)) ;
	}
#endif /* COMMENT */

	if ((rs = comsatmsg_mo(&m0,1,mp,ml)) >= 0) {

#if	CF_DEBUG
	    if (DEBUGLEVEL(5)) {
	        debugprintf("progcsmsg: comsatmsg_mo() rs=%d\n",rs) ;
	        debugprintf("progcsmsg: m0.username=%s\n",m0.username) ;
	        debugprintf("progcsmsg: m0.fname=%s\n",m0.fname) ;
	        debugprintf("progcsmsg: m0.offset=%lu\n",m0.offset) ;
	    }
#endif /* CF_DEBUG */

	    if ((rs = proclogmsg(pip,&m0)) >= 0) {
	        cchar	*maildname = pip->maildname ;
		cchar	*un = m0.username ;
		cchar	*cn = m0.username ;
	        char	mailfname[MAXPATHLEN+1] ;

/* continue to process this in the usual way */

	        if (m0.fname[0] != '\0') cn = m0.fname ;

	        if ((rs = mkpath2(mailfname,maildname,cn)) >= 0) {
		    cchar	*fmt ;

#if	CF_DEBUG
	            if (DEBUGLEVEL(4))
	                debugprintf("progcsmsg: mailfname=%s\n",mailfname) ;
#endif

#if	CF_DEBUG
	            if (DEBUGLEVEL(4)) {
	                ustat	sb ;
	                int		rs1 = u_stat(mailfname,&sb) ;
	                debugprintf("progcsmsg: u_stat() rs1=%d msize=%llu\n",
	                    rs1,sb.st_size) ;
	            }
#endif /* CF_DEBUG */

	            if ((rs = uc_open(mailfname,O_RDONLY,0666)) >= 0) {
	                off_t	fo = (off_t) m0.offset ;
	                const int	mfd = rs ;
	                cchar	*un = m0.username ;

#if	CF_DEBUG
	                if (DEBUGLEVEL(4)) {
	                    USTAT	sb ;
	                    int		rs1 = u_fstat(mfd,&sb) ;
	                    debugprintf("progcsmsg: mfd=%u\n",mfd) ;
	                    fmt = "progcsmsg: u_fstat() rs1=%d msize=%llu\n" ;
	                    debugprintf(fmt,rs1,sb.st_size) ;
	                }
#endif /* CF_DEBUG */

	                if (fo > 0) rs = u_seek(mfd,fo,SEEK_SET) ;

	                if (rs >= 0) {
			    rs = progcsmsger(pip,mfd,fo,un) ;
			    wlen = rs ;
	                } /* end if (seek) */

	                u_close(mfd) ;
		    } else if (isNotPresent(rs)) {
	        	fmt = "inaccessible mailbox=%s (%d)" ;
	        	progloglock_printf(pip,fmt,un,rs) ;
	        	fmt = "%s: inaccessible mailbox=%s (%d)\n" ;
			bprintf(pip->efp,fmt,pn,un,rs) ;
			rs = SR_OK ;
	            } /* end if (opened mail file) */

	        } /* end if (mkpath) */

	    } /* end if (proclogmsg) */

	} else if (isBadMsg(rs)) {
	    if (pip->debuglevel > 0) {
	        progerr_printf(pip,"%s: bad-msg (%d)\n",pn,rs) ;
	    }
	    if (pip->open.logprog) {
	        progloglock_printf(pip,"bad-msg (%d)",rs) ;
	    }
	    rs = SR_OK ;
	} /* end if (comsatmsg_mo) */

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("progcsmsg: ret rs=%d wlen=%u\n",rs,wlen) ;
#endif

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (progcsmsg) */


/* local subroutines */


static int progcsmsger(PI *pip,int mfd,off_t fo,cchar *un)
{
	MAILMSG		mm ;
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("progcsmsger: ent\n") ;
#endif
	if ((rs = mailmsg_start(&mm)) >= 0) {
	    const int	rsi = SR_INVALID ;
	    if ((rs = mailmsg_loadfd(&mm,mfd,fo)) > 0) {
		rs = procmsginfo(pip,&mm,un) ;
		wlen = rs ;
	    } else if ((rs == SR_OK) || (rs == rsi)) {
		rs = procmsgbad(pip,un,fo,rs) ;
	    } else if (isNotPresent(rs)) {
		rs = SR_OK ;
	    }
	    rs1 = mailmsg_finish(&mm) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (mailmsg) */
#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("progcsmsger: ret rs=%d wlen=%u\n",rs,wlen) ;
#endif
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (progcsmsger) */


static int proclogmsg(PI *pip,COMSATMSG_MO *m0p)
{
	int		rs = SR_OK ;
	cchar	*pn = pip->progname ;

	progerr_printf(pip,"%s: comsat-msg¬\n",pn) ;
	progerr_printf(pip,"%s:   u=%s\n",pn,m0p->username) ;
	progerr_printf(pip,"%s:   f=%s\n",pn,m0p->fname) ;
	progerr_printf(pip,"%s:   o=%lu\n",pn,m0p->offset) ;

	progloglock_printf(pip,"comsat-msg¬") ;
	progloglock_printf(pip,"  u=%s",m0p->username) ;
	progloglock_printf(pip,"  f=%s",m0p->fname) ;
	progloglock_printf(pip,"  o=%lu",m0p->offset) ;

	return rs ;
}
/* end subroutine (proclogmsg) */

#if	CF_OUTINFO
static int procmsginfo(PI *pip,MAILMSG *mmp,cchar *un) noex {
	outinfo		oi ;
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
#if	CF_DEBUG
	if (DEBUGLEVEL(5))
	    debugprintf("progcsmsg/procmsginfo: ent\n") ;
#endif
	if ((rs = outinfo_start(&oi,pip,mmp,un)) >= 0) {
	    if ((rs = outinfo_hdrs(&oi)) >= 0) {
		if ((rs = outinfo_adjust(&oi)) >= 0) {
		    if ((rs = outinfo_termbegin(&oi)) >= 0) {
			{
		            rs = outinfo_print(&oi) ;
			    wlen = rs ;
			}
			rs1 = outinfo_termend(&oi) ;
			if (rs >= 0) rs = rs1 ;
		    } /* end if (outinfo_termtrans) */
		} /* end if (outinfo_adjust) */
	    } /* end if (outinfo_hdrs) */
	    rs1 = outinfo_finish(&oi) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (outinfo) */
#if	CF_DEBUG
	if (DEBUGLEVEL(5))
	    debugprintf("progcsmsg/procmsginfo: ret rs=%d rc=%u\n",rs,wlen) ;
#endif
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (procmsginfo) */
#else /* CF_OUTINFO */
static int procmsginfo(PI *pip,MAILMSG *mmp,cchar *un)
{
	const int	dt = pip->daytime ;
	const int	fromlen = MAILADDRLEN ;
	const int	datelen = TIMEBUFLEN ;
	const int	subjlen = TIMEBUFLEN ;
	const int	notelen = NOTEBUFLEN ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		hl ;
	int		fl = 0 ;
	int		sl = 0 ;
	int		rc = 0 ;
	cchar	*hp ;
	char		tbuf[TIMEBUFLEN+1] ;
	char		datebuf[TIMEBUFLEN+1] = { 0 } ;
	char		frombuf[MAILADDRLEN+1] = { 0 } ;
	char		subjbuf[SUBJBUFLEN+1] = { 0 } ;
	char		notebuf[NOTEBUFLEN+1] = { 0 } ;

	if (rs >= 0) {
	    int	f_edate = false ;
	    rs1 = mailmsg_hdrival(mmp,HN_DATE,0,&hp) ;
	    hl = rs1 ;
	    if (rs1 == SR_NOENT) {
	        f_edate = TRUE ;
	        rs1 = mailmsg_envdate(mmp,0,&hp) ;
	        hl = rs1 ;
	    }
	    if (rs1 >= 0) {
	        rs = getdateinfo(pip,datebuf,datelen,hp,hl,f_edate) ;
	    }
	}

#if	CF_DEBUG
	if (DEBUGLEVEL(5))
	    debugprintf("progcsmsg/procmsginfo: rs=%d date=%s\n",rs,datebuf) ;
#endif

	if (rs >= 0) {
	    rs1 = mailmsg_hdrival(mmp,HN_FROM,0,&hp) ;
	    hl = rs1 ;
	    if (rs1 >= 0) {
	        rs = mkaddrname(frombuf,fromlen,hp,hl) ;
	        fl = rs ;
	    }
	}

#if	CF_DEBUG
	if (DEBUGLEVEL(5))
	    debugprintf("progcsmsg/procmsginfo: rs=%d from=>%r<\n",
	        rs,frombuf,strlinelen(frombuf,-1,40)) ;
#endif

	if (rs >= 0) {
	    rs1 = mailmsg_hdrival(mmp,HN_SUBJECT,0,&hp) ;
	    hl = rs1 ;
	    if (rs1 >= 0) {
	        sl = strdcpy1w(subjbuf,subjlen,hp,hl) - subjbuf ;
	    }
	}

#if	CF_DEBUG
	if (DEBUGLEVEL(5))
	    debugprintf("progcsmsg/procmsginfo: rs=%d subj=>%r<\n",
	        rs,subjbuf,strlinelen(subjbuf,-1,40)) ;
#endif

	if (rs >= 0) {
	    cnothrow	nt{} ;
	    cnullptr	np{} ;
	    cint	tlen = max(fl,sl) ;
	    if (char *tbuf ; (tbuf = new(nt) char[tlen + 1]) != np) {
		cint	m = (linecleanoptm.nonsub) ;
	        fl = compactstr(frombuf,fl) ;
	        fl = mklineclean(tbuf,tlen,m,frombuf,fl) ;
		strcpy(frombuf,tbuf) ;
	        sl = compactstr(subjbuf,sl) ;
	        sl = mklineclean(tbuf,tlen,m,subjbuf,fl) ;
		strcpy(subjbuf,tbuf) ;
	        delete [] tbuf ;
	    } /* end if (new-char) */
	} /* end if (OK) */

#if	CF_DEBUG
	if (DEBUGLEVEL(5)) {
	    debugprintf("progcsmsg/procmsginfo: compaction rs=%d\n",rs) ;
	    debugprintf("progcsmsg/procmsginfo: rs=%d subj=>%r<\n",
	        rs,subjbuf,strlinelen(subjbuf,-1,40)) ;
	    debugprintf("progcsmsg/procmsginfo: rs=%d from=>%r<\n",
	        rs,frombuf,strlinelen(frombuf,-1,40)) ;
	}
#endif

	if (rs >= 0) {
	    int	nl = (COLUMNS - MAXOVERLEN) ;

	    if (fl > MAXOVERLEN) {
	        cchar	*tp = strnchr(frombuf,fl,',') ;
	        if (tp != nullptr) {
	            fl = (tp-frombuf) ;
	            while (fl && CHAR_ISWHITE(frombuf[fl-1])) fl -= 1 ;
	        }
	        if (fl > MAXOVERLEN) fl = MAXOVERLEN ;
	        frombuf[fl] = '\0' ;
	    }
	    nl -= fl ;
	    if (sl > nl) {
	        sl = nl ;
	        subjbuf[sl] = '\0' ;
	    }

	    timestr_logz(dt,tbuf) ;
	    progerr_printf(pip,"%s: %s u=%s time=%s\n",
	        pip->progname,tbuf,un,datebuf) ;

	    timestr_logz(dt,timebuf) ;
	    progloglock_printf(pip,"%s u=%s time=%s",timebuf,un,datebuf) ;
	    progloglock_printf(pip,"  from=»%s«",frombuf) ;
	    progloglock_printf(pip,"  subj=»%s«",subjbuf) ;

	    {
	        cchar	*fmt = "¶ %s %s « %r - %r" ;
	        cchar	*db = datebuf ;
	        cchar	*fb = frombuf ;
	        cchar	*sb = subjbuf ;
	        rs = bufprintf(notebuf,notelen,fmt,db,un,fb,fl,sb,sl) ;
	        nl = rs ;
	    }

	    if (rs >= 0) {
	        const int	max = pip->notesmax ;
	        const int	o = (TERMNOTE_OBIFF | TERMNOTE_OBELL) ;
	        int		i = 0 ;
	        cchar		*recips[2] ;
	        recips[i++] = un ;
	        recips[i] = nullptr ;
	        rs = prognote_write(pip,recips,max,o,notebuf,nl) ;
	        rc = rs ;
	        if (rs >= 0) {
	            rs = progloglock_printf(pip,"  termnote (%d)",rs) ;
	        }
	    } /* end if (ok) */

	} /* end if (ok) */

	return (rs >= 0) ? rc : rs ;
}
/* end subroutine (procmsginfo) */
#endif /* CF_OUTINFO */

static int procmsgbad(PI *pip,cchar *un,off_t fo,int rsl) noex {
	int		rs = SR_OK ;
	cchar		*pn = pip->progname ;
	cchar		*fmt ;
	if (rsl == SR_OK) {
	    fmt = "missing msg u=%s offset=%llu" ;
	} else {
	    fmt = "invalid msg u=%s offset=%llu (%d)" ;
	}
	progloglock_printf(pip,fmt,un,fo,rsl) ;
	if (pip->debuglevel > 0) {
	    if (rsl == SR_OK) {
	        fmt = "%s: missing msg u=%s offset=%llu" ;
	    } else {
	        fmt = "%s: invalid msg u=%s offset=%llu (%d)" ;
	    }
	    progerr_printf(pip,fmt,pn,un,fo,rsl) ;
	}
	return rs ;
}
/* end subroutine (procmsgbad) */


static int getdateinfo(PI *pip,char *abuf,int alen,cchar *ap,int al,
		int f_edate)
{
	int		rs = SR_OK ;
	int		len = 0 ;

#if	CF_DEBUG
	if (DEBUGLEVEL(5)) {
	debugprintf("progcsmsg/getdateinfo: ent a=>%r<\n",ap,al) ;
	debugprintf("progcsmsg/getdateinfo: alen=%d\n",alen) ;
	}
#endif

	if (abuf == nullptr) return SR_FAULT ;
	if (ap == nullptr) return SR_FAULT ;

	abuf[0] = '\0' ;
	if (al < 0) al = strlen(ap) ;

	if (al >= 0) {
	    if (f_edate) {
	        rs = dater_setstd(&pip->d,ap,al) ;
	    } else {
	        rs = dater_setmsg(&pip->d,ap,al) ;
	    }
	    if (rs >= 0) {
	        time_t	mt ;
	        if ((rs = dater_gettime(&pip->d,&mt)) >= 0) {
		    TMTIME	tm ;
		    if ((rs = tmtime_localtime(&tm,mt)) >= 0) {
			rs = sntmtime(abuf,alen,&tm,"%R") ;
			len = rs ;
#if	CF_DEBUG
	if (DEBUGLEVEL(5))
	debugprintf("progcsmsg/getdateinfo: len=%d ab=>%r<\n",len,abuf,len) ;
#endif
	            }
	        }
	    } else if (isBadTime(rs)) {
		rs = SR_OK ;
		len = sncpy1(abuf,alen,"99:99") ;
	    } /* end if (dater) */
	} else {
	    abuf[0] = '\0' ;
	} /* end if (non-zero) */

#if	CF_DEBUG
	if (DEBUGLEVEL(5))
	debugprintf("progcsmsg/getdateinfo: ret rs=%d\n",rs) ;
#endif

	return (rs >= 0) ? len : rs ;
}
/* end subroutine (getdateinfo) */


#if	CF_OUTINFO

static int outinfo_start(OI *op,PI *pip,MAILMSG *mmp,cchar *un) noex {
	int		rs = SR_OK ;

	memclear(op) ;
	op->pip = pip ;
	op->mmp = mmp ;
	op->un = un ;
	op->dlen = DATEBUFLEN ;
	op->ul = strlen(un) ;
	op->ll = COLUMNS ;

	return rs ;
}
/* end subroutine (outinfo_start) */

static int outinfo_finish(OI *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (op->sbuf != nullptr) {
	    rs1 = uc_free(op->sbuf) ;
	    if (rs >= 0) rs = rs1 ;
	    op->sbuf = nullptr ;
	}

	if (op->fbuf != nullptr) {
	    rs1 = uc_free(op->fbuf) ;
	    if (rs >= 0) rs = rs1 ;
	    op->fbuf = nullptr ;
	}

	op->pip = nullptr ;
	return rs ;
}
/* end subroutine (outinfo_finish) */


static int outinfo_hdrs(OI *oip)
{
	int		rs ;
	if ((rs = outinfo_mkdate(oip)) >= 0) {
	    if ((rs = outinfo_mkfrom(oip)) >= 0) {
	        rs = outinfo_mksubj(oip) ;
	    }
	}
	return rs ;
}
/* end subroutine (outinfo_hdrs) */


static int outinfo_mkdate(OI *oip)
{
	PROGINFO	*pip = oip->pip ;
	MAILMSG		*mmp = oip->mmp ;
	int		rs ;
	int		hl = 0 ;
	int		f_edate = false ;
	cchar		*hp ;
#if	CF_DEBUG
	if (DEBUGLEVEL(5))
	    debugprintf("progcsmsg/outinfo_mkdate: ent\n") ;
#endif
	if (pip == nullptr) return SR_FAULT ;
	if ((rs = mailmsg_hdrival(mmp,HN_DATE,0,&hp)) > 0) {
	    hl = rs ;
	} else if ((rs == 0) || isNoMsg(rs)) {
	    if ((rs = mailmsg_envdate(mmp,0,&hp)) > 0) {
	        f_edate = TRUE ;
	        hl = rs ;
	    } else if (isNoMsg(rs)) {
		rs = SR_OK ;
	    }
	}
#if	CF_DEBUG
	if (DEBUGLEVEL(5))
	    debugprintf("progcsmsg/outinfo_mkdate: mid3 rs=%d hl=%u\n",rs,hl) ;
#endif
	if ((rs >= 0) && (hl > 0)) {
	    PROGINFO	*pip = oip->pip ;
	    const int	dlen = oip->dlen ;
	    char	*dbuf = oip->dbuf ;
	    rs = getdateinfo(pip,dbuf,dlen,hp,hl,f_edate) ;
	}
#if	CF_DEBUG
	if (DEBUGLEVEL(5))
	    debugprintf("progcsmsg/outinfo_mkdate: ret rs=%d\n",rs) ;
#endif
	return rs ;
}
/* end subroutine (outinfo_mkdate) */


static int outinfo_getfrom(OI *oip,cchar **rpp)
{
	MAILMSG		*mmp = oip->mmp ;
	int		rs ;
	int		hl = 0 ;
	cchar		*hn = HN_FROM ;
	cchar		*hp ;
	if ((rs = mailmsg_hdrval(mmp,hn,&hp)) > 0) {
	    hl = rs ;
	} else if ((rs == 0) || isNoMsg(rs)) {
	    hn = HN_RETURNPATH ;
	    if ((rs = mailmsg_hdrval(mmp,hn,&hp)) > 0) {
	        hl = rs ;
	    } else if ((rs == 0) || isNoMsg(rs)) {
	        hn = HN_REPLYTO ;
	        if ((rs = mailmsg_hdrval(mmp,hn,&hp)) > 0) {
	            hl = rs ;
	        } else if (isNoMsg(rs)) {
		    rs = SR_OK ;
		}
	    }
	}
	if (rpp != nullptr) {
	    *rpp = (rs >= 0) ? hp : nullptr ;
	}
	return (rs >= 0) ? hl : rs ;
}
/* end subroutine (outinfo_getfrom) */


static int outinfo_mkfrom(OI *oip)
{
	int		rs ;
	int		len = 0 ;
	cchar		*hp ;
	if ((rs = outinfo_getfrom(oip,&hp)) >= 0) {
	    const int	hl = rs ;
	    const int	rlen = rs ;
	    char	*rbuf ;
	    if ((rs = uc_malloc((rlen+1),&rbuf)) >= 0) {
		if ((rs = mkaddrname(rbuf,rlen,hp,hl)) > 0) {
		    rs = outinfo_cvtfrom(oip,rbuf,rs) ;
		    len = rs ;
		} /* end if (mkaddrname) */
		uc_free(rbuf) ;
	    } /* end if (m-a-f) */
	} /* end if (have) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (outinfo_mkfrom) */


static int outinfo_cvtfrom(OI *oip,cchar *sp,int sl)
{
	PROGINFO	*pip = oip->pip ;
	const int	size = ((sl+1) * sizeof(wchar_t)) ;
	const int	ilen = sl ;
	int		rs ;
	int		rcols = 0 ;
	wchar_t		*ibuf ;
#if	CF_DEBUGS
	debugprintf("progcsmsg/outinfo_cvt: ent sl=%d\n",sl) ;
#endif
	if ((rs = uc_malloc(size,&ibuf)) >= 0) {
	    if ((rs = progcs_trans(pip,ibuf,ilen,sp,sl)) >= 0) {
	        oip->flen = rs ;
		oip->fbuf = ibuf ;
		rcols = rs ;
	    } /* end if (progcs_trans) */
	    if (rs < 0) {
		uc_free(ibuf) ;
	    }
	} /* end if (m-a) */
#if	CF_DEBUGS
	debugprintf("progcsmsg/outinfo_cvt: ret rs=%d rcols=%d\n",rs,rcols) ;
#endif
	return (rs >= 0) ? rcols : rs ;
}
/* end subroutine (outinfo_cvtfrom) */


static int outinfo_getsubj(OI *oip,cchar **rpp)
{
	MAILMSG		*mmp = oip->mmp ;
	int		rs ;
	int		hl = 0 ;
	cchar		*hn = HN_SUBJECT ;
	cchar		*hp ;
	if ((rs = mailmsg_hdrval(mmp,hn,&hp)) > 0) {
	    hl = rs ;
	} else if ((rs == 0) || isNoMsg(rs)) {
	    hp = hn ; /* something and static */
	    rs = SR_OK ;
	}
	if (rpp != nullptr) {
	    *rpp = (rs >= 0) ? hp : nullptr ;
	}
	return (rs >= 0) ? hl : rs ;
}
/* end subroutine (outinfo_getsubj) */


static int outinfo_mksubj(OI *oip)
{
	int		rs ;
	int		len = 0 ;
	cchar		*hp ;
	if ((rs = outinfo_getsubj(oip,&hp)) >= 0) {
	    int		cl ;
	    cchar	*cp ;
	    if ((cl = sfshrink(hp,rs,&cp)) >= 0) {
		    rs = outinfo_cvtsubj(oip,cp,cl) ;
		    len = rs ;
	    }
	} /* end if (have) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (outinfo_mksubj) */


static int outinfo_cvtsubj(OI *oip,cchar *sp,int sl)
{
	PROGINFO	*pip = oip->pip ;
	const int	size = ((sl+1) * sizeof(wchar_t)) ;
	const int	ilen = sl ;
	int		rs ;
	int		rcols = 0 ;
	wchar_t		*ibuf ;
#if	CF_DEBUGS
	debugprintf("progcsmsg/outinfo_cvt: ent sl=%d\n",sl) ;
#endif
	if ((rs = uc_malloc(size,&ibuf)) >= 0) {
	    if ((rs = progcs_trans(pip,ibuf,ilen,sp,sl)) >= 0) {
	        oip->slen = rs ;
		oip->sbuf = ibuf ;
		rcols = rs ;
	    } /* end if (progcs_trans) */
	    if (rs < 0) {
		uc_free(ibuf) ;
	    }
	} /* end if (m-a) */
#if	CF_DEBUGS
	debugprintf("progcsmsg/outinfo_cvt: ret rs=%d rcols=%d\n",rs,rcols) ;
#endif
	return (rs >= 0) ? rcols : rs ;
}
/* end subroutine (outinfo_cvtsubj) */


static int outinfo_adjust(OI *oip)
{
	int		rs = SR_OK ;
	int		rl = 0 ;
	int		tl ;

#if	CF_DEBUGS
	debugprintf("progcsmsg/outinfo_adjust: ent\n") ;
#endif

/* setup */

	oip->fl = oip->flen ;
	oip->sl = oip->slen ;
	oip->ul = strlen(oip->un) ;

#if	CF_DEBUGS
	debugprintf("progcsmsg/outinfo_adjust: fl=%u\n",oip->fl) ;
	debugprintf("progcsmsg/outinfo_adjust: sl=%u\n",oip->sl) ;
	debugprintf("progcsmsg/outinfo_adjust: ul=%u\n",oip->ul) ;
#endif

/* reductions */

	tl = outinfo_cols(oip) ;

#if	CF_DEBUGS
	debugprintf("progcsmsg/outinfo_adjust: mid1 tl=%u\n",tl) ;
#endif

	if (tl > oip->ll) {
	    if ((rl = wsfnormfrom(oip->fbuf,oip->fl)) >= 0) {
	        oip->fl = rl ;
		tl = outinfo_cols(oip) ;
	    }
	}


#if	CF_DEBUGS
	debugprintf("progcsmsg/outinfo_adjust: mid2 tl=%d\n",tl) ;
#endif

	if (tl > oip->ll) {
	    rl = (tl - oip->ll) ;
	    if ((oip->fl - rl) >= MAXFROMLEN) {
	        oip->fl -= rl ;
	        tl = outinfo_cols(oip) ;
	    }
	}

#if	CF_DEBUGS
	debugprintf("progcsmsg/outinfo_adjust: mid3 tl=%d\n",tl) ;
#endif

	if (tl > oip->ll) {
	    if (oip->fl > MAXFROMLEN) {
	        oip->fl = MAXFROMLEN ;
	        tl = outinfo_cols(oip) ;
	    }
	}

#if	CF_DEBUGS
	debugprintf("progcsmsg/outinfo_adjust: mid4 tl=%d\n",tl) ;
#endif

	if (tl > oip->ll) {
	    rl = (tl - oip->ll) ;
	    oip->sl -= rl ;
	    tl = outinfo_cols(oip) ;
	}

	if (rs >= 0) {
	   oip->tl = tl ;
	}

#if	CF_DEBUGS
	debugprintf("progcsmsg/outinfo_adjust: ret rs=%d tl=%d\n",rs,tl) ;
#endif

	return (rs >= 0) ? tl : rs ;
}
/* end if (outinfo_adjust) */


static int outinfo_cols(OI *oip)
{
	int	cols = (3+5+3+3) ; /* non-field columns in output string */
	cols += (oip->dl+oip->ul+oip->fl+oip->sl) ;
	return cols ;
}
/* end if (outinfo_cols) */


static int outinfo_termbegin(OI *oip)
{
	int		rs = SR_OK ;
	if (rs >= 0) {
	    rs = outinfo_termbeginfrom(oip) ;
	} /* end if (ok) */
	if (rs >= 0) {
	    rs = outinfo_termbeginsubj(oip) ;
	} /* end if (ok) */
	return rs ;
}
/* end if (outinfo_termbegin) */


static int outinfo_termend(OI *oip)
{
	int		rs = SR_OK ;
	int		rs1 ;
	if (oip->osbuf != nullptr) {
	    rs1 = uc_free(oip->osbuf) ;
	    if (rs >= 0) rs = rs1 ;
	    oip->osbuf = nullptr ;
	    oip->oslen = 0 ;
	}
	if (oip->ofbuf != nullptr) {
	    rs1 = uc_free(oip->ofbuf) ;
	    if (rs >= 0) rs = rs1 ;
	    oip->ofbuf = nullptr ;
	    oip->oflen = 0 ;
	}
	return rs ;
}
/* end if (outinfo_termend) */


static int outinfo_termbeginfrom(OI *oip)
{
	const int	fl = oip->flen ;
	const int	oflen = (2*oip->flen) ;
	int		rs ;
	const wchar_t	*fp = oip->fbuf ;
	char		*ofbuf ;
	if ((rs = uc_malloc((oflen+1),&ofbuf)) >= 0) {
		const int	tlen = oflen ;
		char		*tbuf ;
		if ((rs = uc_malloc((tlen+1),&tbuf)) >= 0) {
		    if ((rs = snwcpywidehdr(tbuf,tlen,fp,fl)) >= 0) {
		        if ((rs = mkaddrdisp(ofbuf,oflen,tbuf,rs)) >= 0) {
		            oip->ofbuf = ofbuf ;
		            oip->oflen = rs ;
		        }
		    }
		    uc_free(tbuf) ;
		} /* end if (m-a-f) */
		if (rs < 0) {
		    uc_free(ofbuf) ;
		}
	} /* end if (m-a) */
	return rs ;
}
/* end subroutine (outinfo_termbeginfrom) */


static int outinfo_termbeginsubj(OI *oip)
{
	const int	sl = oip->slen ;
	const int	oslen = (2*oip->slen) ;
	int		rs ;
	const wchar_t	*sp = oip->sbuf ;
	char		*osbuf ;
	    if ((rs = uc_malloc((oslen+1),&osbuf)) >= 0) {
		const int	tlen = oslen ;
		char		*tbuf ;
		if ((rs = uc_malloc((tlen+1),&tbuf)) >= 0) {
		    if ((rs = snwcpywidehdr(tbuf,tlen,sp,sl)) >= 0) {
		        if ((rs = snwcpycompact(osbuf,oslen,tbuf,rs)) >= 0) {
		            oip->osbuf = osbuf ;
		            oip->oslen = rs ;
			}
		    }
		    uc_free(tbuf) ;
		} /* end if (m-a-f) */
		if (rs < 0) {
		    uc_free(osbuf) ;
		}
	    } /* end if (m-a) */
	return rs ;
}
/* end subroutine (outinfo_termbeginsubj) */


static int outinfo_print(OI *oip)
{
	PROGINFO	*pip = oip->pip ;
	const int	olen = oip->tl ;
	int		rs ;
	int		wlen = 0 ;
	char		*obuf ;

#if	CF_DEBUG
	if (DEBUGLEVEL(5))
	    debugprintf("progcsmsg/outinfo_print: ent olen=%d\n",olen) ;
#endif

	if ((rs = uc_malloc((olen+1),&obuf)) >= 0) {
	    time_t	dt = pip->daytime ;
	    const int	fl = oip->fl ;
	    const int	sl = oip->sl ;
	    cchar	*pn = pip->progname ;
	    cchar	*un = oip->un ;
	    cchar	*db = oip->dbuf ;
	    cchar	*fb = oip->ofbuf ;
	    cchar	*sb = oip->osbuf ;
	    cchar	*fmt ;
	    char	tbuf[TIMEBUFLEN+1] ;

	    fmt = "%s: %s u=%s time=%s\n" ;
	    timestr_logz(dt,tbuf) ;
	    progerr_printf(pip,fmt,pn,tbuf,un,db) ;
	    fmt = "%s: from=>%r<\n" ;
	    progerr_printf(pip,fmt,pn,fb,fl) ;
	    fmt = "%s: subj=>%r<\n" ;
	    progerr_printf(pip,fmt,pn,sb,sl) ;

	    progloglock_printf(pip,"%s u=%s time=%s",tbuf,un,db) ;
	    progloglock_printf(pip,"  from=»%s«",fb) ;
	    progloglock_printf(pip,"  subj=»%s«",sb) ;

	    fmt = "¶ %s %s « %r - %r" ;
	    if ((rs = bufprintf(obuf,olen,fmt,db,un,fb,fl,sb,sl)) >= 0) {
	        const int	max = 3 ;
	        const int	o = (TERMNOTE_OBIFF | TERMNOTE_OBELL) ;
		const int	ol = rs ;
	        int		i = 0 ;
	        cchar		*recips[2] ;
	        wlen = rs ;
	        recips[i++] = un ;
	        recips[i] = nullptr ;
	        rs = prognote_write(pip,recips,max,o,obuf,ol) ;
	        if (rs >= 0) {
	            rs = progloglock_printf(pip,"  termnotes=%u",rs) ;
	        }
	    } /* end if (ok) */

	    uc_free(obuf) ;
	} /* end of (m-a-f) */

#if	CF_DEBUG
	if (DEBUGLEVEL(5))
	    debugprintf("progcsmsg/outinfo_print: ret rs=%d wlen=%d\n",
		rs,wlen) ;
#endif

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (outinfo_print) */

static int wsfnormfrom(const wchar_t *fp,int fl) noex {
	if (int si ; (si = wsichr(fp,fl,',')) >= 0) {
	    fl = si ;
	    while (fl && wciswhite(fp[fl-1])) {
		fl -= 1 ;
	    }
	}
	return fl ;
}
/* end subroutine (wsfnormfrom) */

#endif /* CF_OUTINFO */

static bool isNoMsg(int rs) noex {
	return isOneOf(rsnomsg,rs) ;
}
/* end subroutine (isNoMsg) */

static bool isBadMsg(int rs) noex {
	bool		f = false ;
	f = f || isOneOf(rsbadmsg,rs) ;
	f = f || isNoMsg(rs) ;
	return f ;
}
/* end subroutine (isBadMsg) */

static bool isBadTime(int rs) noex {
	return isOneOf(rsbadtime,rs) ;
}
/* end subroutine (isBadTime) */



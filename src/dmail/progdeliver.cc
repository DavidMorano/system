/* progdeliver SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* progdelivers mail messages (data) to a mailbox spool file */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* compile-time debugging */
#define	CF_DEBUG	0		/* switchable debug print-outs */

/* revision history:

	= 1998-07-01, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	progdeliver

	Description:
	This subroutine is used to deliver new mail to the mail
	spool file for a given recipient.

	Synopsis:
	int progdeliver(progifo *pip,int tfd,recip *rp) noex

	Arguments:
	pip		program information pointer 
	tfd		file descriptor (FD) to target spool file
	rp		pointer to recipient container

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<ctime>
#include	<csignal>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<sbuf.h>
#include	<bfile.h>
#include	<sigblocker.h>
#include	<localmisc.h>

#include	"mailspool.h"
#include	"lkmail.h"
#include	"recip.h"
#include	"config.h"
#include	"defs.h"
#include	"proglog.h"


/* local defines */

#ifndef	BUFLEN
#define	BUFLEN		256
#endif

#define	LOCKBUFLEN	512

#define	FORWARDED	"Forward to "


/* external subroutines */

extern int	mkpath2(char *,cchar *,cchar *) ;

#if	CF_DEBUGS || CF_DEBUG
extern int	debugprintf(cchar *,...) ;
extern int	debugprintfsize(cchar *,int) ;
extern int	strlinelen(cchar *,int,int) ;
#endif

extern char	*strwcpy(char *,cchar *,int) ;
extern char	*timestr_logz(time_t,char *) ;


/* external variables */


/* local structures */


/* forward references */

static int	progdeliverer(proginfo *,int,RECIP *,cchar *,int) ;
static int	progdeliver_mbo(proginfo *,RECIP *,int,int) ;
static int	progdeliver_lockinfo(proginfo *,MAILSPOOL *) ;
static int	mklockinfo(proginfo *,char *,int,time_t) ;


/* local variables */

constexpr int	sigblocks[] = {
	SIGALRM,
	SIGPOLL,
	SIGHUP,
	SIGTERM,
	SIGINT,
	SIGQUIT,
	SIGWINCH,
	SIGURG,
	0
} ;


/* exported variables */


/* exported subroutines */

int progdeliver(proginfo *pip,int tfd,RECIP *rp) noex {
	int		rs ;
	int		tlen = 0 ;
	cchar		*md = rp->maildname ;

#if	CF_DEBUG
	if (DEBUGLEVEL(4)) {
	    debugprintf("progdeliver: ent\n") ;
	    debugprintf("progdeliver: recip md=%s\n",rp->maildname) ;
	    debugprintf("progdeliver: recip name=%s\n",rp->recipient) ;
	    debugprintfsize("progdeliver tfd",tfd);
	}
#endif

	if ((rs = progdeliverer(pip,tfd,rp,md,TRUE)) >= 0) {
	    md = pip->copymaildname ;
	    tlen = rs ;
	    if (pip->fl.optcopy && (md != NULL)) {
		    cchar *fmt ;
	            if ((rs = progdeliverer(pip,tfd,rp,md,FALSE)) >= 0) {
			fmt = "  msg-copy (%d)" ;
	    		proglog_printf(pip,fmt,rs) ;
		    } else if (rs == SR_FBIG) {
			fmt = "  msg-copy too big (%d)" ;
	    		proglog_printf(pip,fmt,rs) ;
			rs = SR_OK ;
		    }
	    } /* end if (opt-copy) */
	} /* end if (progdeliverer-primary) */

	if (rs < 0) {
	    proglog_printf(pip,"progdeliver (%d)",rs) ;
	}

#if	CF_DEBUG
	if (DEBUGLEVEL(4)) {
	    debugprintf("progdeliver: ret rs=%d tlen=%u\n",rs,tlen) ;
	    debugprintfsize("progdeliver tfd",tfd);
	}
#endif

	return (rs >= 0) ? tlen : rs ;
}
/* end subroutine (progdeliver) */


/* local subroutines */

static int progdeliverer(proginfo *pip,int tfd,RECIP *rp,cc *md,int f) noex {
	sigblocker	blocks ;
	int		rs ;
	int		rs1 ;
	int		tlen = 0 ;

#if	CF_DEBUG
	if (DEBUGLEVEL(4)) {
	    cchar	*r = rp->recipient ;
	    debugprintf("progdeliverer: ent\n") ;
	    debugprintf("progdeliverer: md=%s\n",md) ;
	    debugprintf("progdeliverer: r=%s\n",r) ;
	}
#endif

	if ((rs = sigblocker_start(&blocks,sigblocks)) >= 0) {
	    if ((rs = u_rewind(tfd)) >= 0) {
	        MAILSPOOL	ms ;
		mode_t		om = 0660 ;
		const int	to = pip->to_spool ;
		int		of = (O_CREAT|O_RDWR|O_APPEND) ;
		cchar		*un = rp->recipient ;
	        if ((rs = mailspool_open(&ms,md,un,of,om,to)) >= 0) {
		    const int	mfd = rs ;
#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("progdeliverer: mailspool_open() rs=%d\n",rs) ;
#endif
		    if ((rs = progdeliver_lockinfo(pip,&ms)) >= 0) {
			if ((rs = u_seek(mfd,0L,SEEK_END)) >= 0) {
			    if ((rs = progdeliver_mbo(pip,rp,rs,f)) >= 0) {
			        rs = recip_copyparts(rp,tfd,mfd) ;
			        tlen = rs ;
				if (rs < 0) {
	    			    cchar	*fmt ;
				    fmt = "recipcopypars (%d)" ;
	    			    proglog_printf(pip,fmt,rs) ;
				}
			    } /* end if (progdeliver_mbo) */
			} /* end if (u_seek) */
		    } /* end if (progdeliver_lockinfo) */
		    rs1 = mailspool_close(&ms) ;
	    	    if (rs >= 0) rs = rs1 ;
	        } /* end if (mailspool) */
	    } /* end if (u_rewind) */
	    rs1 = sigblocker_finish(&blocks) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (sigblock) */

	if (rs < 0) {
	    proglog_printf(pip,"progdeliverer (%d)",rs) ;
	}

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("progdeliverer: ret rs=%d tlen=%u\n",rs,tlen) ;
#endif

	return (rs >= 0) ? tlen : rs ;
}
/* end subroutine (progdeliverer) */

/* ARGSUSED */
static int progdeliver_mbo(proginfo *pip,RECIP *rp,int mbo,int f) noex {
	int		rs = SR_OK ;
	if (f) {
	    rs = recip_mbo(rp,mbo) ;
	}
	return rs ;
}
/* end subroutine (progdeliver_mbo) */

static int progdeliver_lockinfo(proginfo *pip,MAILSPOOL *msp) noex {
	custime		dt = pip->daytime ;
	cint		llen = LOCKBUFLEN ;
	int		rs ;
	char		lbuf[LOCKBUFLEN+1] ;
	if ((rs = mklockinfo(pip,lbuf,llen,dt)) > 0) {
#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("progdeliver: mklockinfo() rs=%d\n",rs) ;
#endif
	    rs = mailspool_setlockinfo(msp,lbuf,rs) ;
	}
#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("progdeliver: ret rs=%d\n",rs) ;
#endif
	return rs ;
}
/* end subroutine (progdeliver_lockinfo) */

/* make (create) the information that goes into the mail-lock file */
static int mklockinfo(proginfo *pip,char *rbuf,int rlen,time_t ti_now) noex {
	int		rs ;
	int		rs1 ;
	if (sbuf b ; (rs = sbuf_start(&b,rbuf,rlen)) >= 0) {
	    /* line 1 */
	    sbuf_deci(&b,pip->pid) ;
	    sbuf_chr(&b,'\n') ;
	    /* line 2 */
	    sbuf_strw(&b,pip->lockaddr,-1) ;
	    sbuf_chr(&b,'\n') ;
	    /* line 3 */
	    {
		char	tbuf[TIMEBUFLEN + 1] ;
	        timestr_logz(ti_now,tbuf) ;
	        sbuf_strw(&b,tbuf,-1) ;
	        sbuf_chr(&b,' ') ;
	        sbuf_strw(&b,pip->progname,-1) ;
	        sbuf_chr(&b,'\n') ;
	    }
	    /* line 4 */
	    sbuf_strw(&b,"logid=",-1) ;
	    sbuf_strw(&b,pip->logid,-1) ;
	    sbuf_chr(&b,'\n') ;
	    /* done */
	    rs1 = sbuf_finish(&b) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (sbuf) */
	return rs ;
}
/* end subroutine (mklockinfo) */



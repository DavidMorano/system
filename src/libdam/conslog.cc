/* conslog SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* send log messages to the system logger device */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A�D� Morano
	This object module was originally written to create a logging
	mechanism for PCS application programs.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	These are the subroutines in this module:

		conslog_open
		conslog_write
		conslog_printf
		conslog_vprintf
		conslog_check
		conslog_close

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<sys/log.h>		/* for LOG_MAXPS */
#include	<sys/strlog.h>		/* interface definitions */
#include	<sys/syslog.h>		/* for all other 'LOG_xxx' */
#include	<unistd.h>
#include	<fcntl.h>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstdarg>
#include	<cstring>
#include	<usystem.h>
#include	<ascii.h>
#include	<char.h>
#include	<ischarx.h>
#include	<strdcpy.h>
#include	<localmisc.h>

#include	"conslog.h"


/* local defines */

#define	CONSLOG_LOGLEN	LOG_MAXPS
#define	CONSLOG_EXTRA	100
#define	CONSLOG_NMSGS	10

#ifndef	LOGDEV
#define	LOGDEV		"/dev/conslog"
#endif

#undef	OUTBUFLEN
#define	OUTBUFLEN	(CONSLOG_LINELEN +2)

#define	TO_OPEN		(60 * 60)
#define	TO_WRITE	30
#define	TO_LOCK		4


/* external subroutines */


/* external variables */


/* local structures */

struct colstate {
	int		ncols ;
	int		ncol ;
} ;


/* forward references */

static int	conslog_fileopen(CONSLOG *) noex ;
static int	conslog_fileclose(CONSLOG *) noex ;
static int	conslog_logdevice(CONSLOG *,int,cchar *,int) noex ;

static boolt	isLogFac(int) noex ;


/* local variables */

static cint	logfacs[] = {
	LOG_KERN,
	LOG_USER,
	LOG_MAIL,
	LOG_DAEMON,
	LOG_AUTH,
	LOG_SYSLOG,
	LOG_LPR,
	LOG_NEWS,
	LOG_UUCP,
	LOG_CRON,
	LOG_LOCAL0,
	LOG_LOCAL1,
	LOG_LOCAL2,
	LOG_LOCAL3,
	LOG_LOCAL4,
	LOG_LOCAL5,
	LOG_LOCAL6,
	LOG_LOCAL7,
	-1
} ;


/* exported subroutines */


int conslog_open(CONSLOG *op,int logfac)
{
	int		rs = SR_OK ;

	if (op == nullptr) return SR_FAULT ;

	if (! isLogFac(logfac)) return SR_INVALID ;

	memclear(op) ;
	op->lfd = -1 ;

	op->magic = CONSLOG_MAGIC ;
	return rs ;
}
/* end subroutine (conslog_open) */


int conslog_close(CONSLOG *op)
{
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != CONSLOG_MAGIC) return SR_NOTOPEN ;

	rs1 = conslog_fileclose(op) ;
	if (rs >= 0) rs = rs1 ;

	op->magic = 0 ;
	return rs ;
}
/* end subroutine (conslog_close) */


/* make a log entry */
int conslog_printf(CONSLOG *op,int logpri,cchar fmt[],...)
{
	int		rs ;

	{
	    va_list	ap ;
	    va_begin(ap,fmt) ;
	    rs = conslog_vprintf(op,logpri,fmt,ap) ;
	    va_end(ap) ;
	} /* end block */

	return rs ;
}
/* end subroutine (conslog_printf) */


/* make a log entry */
int conslog_vprintf(CONSLOG *op,int logpri,cchar *fmt,va_list ap)
{
	int		rs = SR_OK ;
	int		sl ;
	int		ol ;
	int		len = 0 ;
	cchar	*tp, *sp ;
	char		outbuf[OUTBUFLEN + 1] ;

#if	CF_DEBUGS
	debugprintf("conslog_printf: ent\n") ;
#endif

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != CONSLOG_MAGIC) return SR_BADF ;

/* format the user's portion */

	ol = bufvprintf(outbuf,OUTBUFLEN,fmt,ap) ;
	if (ol < 0) ol = 0 ;

#ifdef	COMMENT
	if (ol > CONSLOG_USERLEN)
	    ol = CONSLOG_USERLEN ;
#endif

	sp = outbuf ;
	sl = ol ;
	while ((tp = strnchr(sp,sl,'\n')) != nullptr) {

	    rs = conslog_write(op,logpri,sp,(tp - sp)) ;
	    len += rs ;

	    sl -= ((tp + 1) - sp) ;
	    sp = (tp + 1) ;

	    if (rs < 0) break ;
	} /* end while */

	if ((rs >= 0) && (sl > 0)) {
	    rs = conslog_write(op,logpri,sp,sl) ;
	    len += rs ;
	}

	return (rs >= 0) ? len : rs ;
}
/* end subroutine (conslog_vprintf) */


int conslog_check(CONSLOG *op,time_t dt)
{
	int		rs = SR_OK ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != CONSLOG_MAGIC) return SR_NOTOPEN ;

	if (op->lfd >= 0) {
	    int		f = false ;
	    f = f || ((dt - op->ti_write) >= TO_WRITE) ;
	    f = f || ((dt - op->ti_open) >= TO_OPEN) ;
	    if (f) {
	        rs = conslog_fileclose(op) ;
	    }
	} /* end if (open) */

	return rs ;
}
/* end subroutine (conslog_check) */


int conslog_write(CONSLOG *op,int logpri,cchar *wbuf,int wlen)
{
	int		rs = SR_OK ;

	if (op == nullptr) return SR_FAULT ;
	if (wbuf == nullptr) return SR_FAULT ;

	if (wlen < 0)
	    wlen = lenstr(wbuf) ;

	while (wlen && iseol(wbuf[wlen-1])) {
	    wlen -= 1 ;
	}

	while (wlen && CHAR_ISWHITE(wbuf[wlen-1])) {
	    wlen -= 1 ;
	}

	if (wlen > 0) {
	    if ((rs = conslog_logdevice(op,logpri,wbuf,wlen)) >= 0) {
	        op->c += 1 ;
	    }
	}

#if	CF_DEBUGS
	debugprintf("conslog_printf: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (conslog_write) */


int conslog_count(CONSLOG *op)
{
	int		rs ;

	if (op == nullptr) return SR_FAULT ;

	rs = op->c ;
	return rs ;
}
/* end subroutine (conslog_count) */


/* private subroutines */

static int conslog_fileopen(CONSLOG *op) noex {
	int		rs = SR_OK ;
	if (op->lfd < 0) {
	    cint	of = O_WRONLY ;
	    cmode	om = 0666 ;
	    cchar	*logdev = LOGDEV ;
	    if ((rs = u_open(logdev,of,om)) >= 0) {
	        op->lfd = rs ;
	        rs = uc_closeonexec(op->lfd,true) ;
	        op->ti_open = time(nullptr) ;
	        if (rs < 0) {
	            u_close(op->lfd) ;
	            op->lfd = -1 ;
	        }
	    } /* end if (open) */
	} /* end if (need an open) */
	return rs ;
}
/* end subroutine (conslog_fileopen) */

static int conslog_fileclose(CONSLOG *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->lfd >= 0) {
	    rs1 = uc_close(op->lfd) ;
	    if (rs >= 0) rs = rs1 ;
	    op->lfd = -1 ;
	}
	return rs ;
}
/* end subroutine (conslog_fileclose) */

static int conslog_logdevice(CONSLOG *op,int logpri,cchar *bp,int bl) noex {
	struct strbuf	cmsg ;
	struct strbuf	dmsg ;
	struct log_ctl	lc{} ;
	int		rs = SR_OK ;

	if (op->lfd < 0) rs = conslog_fileopen(op) ;

	if (rs >= 0) {
	    logpri &= LOG_PRIMASK ;		/* truncate any garbage */
	    /* write it to the LOG device */
	    lc.flags = SL_CONSOLE ;
	    lc.level = 0 ;
	    lc.pri = (op->logfac | logpri) ;
	    /* set up the strbufs */
	    cmsg.maxlen = szof(struct log_ctl) ;
	    cmsg.len = szof(struct log_ctl) ;
	    cmsg.buf = (caddr_t) &lc ;

	    dmsg.maxlen = (bl+1) ;
	    dmsg.len = bl ;
	    dmsg.buf = (char *) bp ;

/* output the message to the local logger */
	    rs = u_putmsg(op->lfd,&cmsg,&dmsg,0) ;

	} /* end if (ok) */

	return rs ;
}
/* end subroutine (conslog_logdevice) */

static bool isLogFac(int fac) noex {
	bool		f = false ;
	for (int i = 0 ; logfacs[i] >= 0 ; i += 1) {
	    f = (fac == logfacs[i]) ;
	    if (f) break ;
	} /* end if */
	return f ;
}
/* end subroutine (isLogFac) */



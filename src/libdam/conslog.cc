/* conslog SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* send log messages to the system logger device */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This object module was originally written to create a logging
	mechanism for PCS application programs.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	conslog

	Description:
	These are the subroutines in this module:
		conslog_open
		conslog_write
		conslog_printf
		conslog_vprintf
		conslog_check
		conslog_close

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
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
#include	<mallocxx.h>
#include	<ascii.h>
#include	<strn.h>
#include	<strdcpy.h>
#include	<char.h>
#include	<ischarx.h>
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

template<typename ... Args>
static int conslog_ctor(conslog *op,Args ... args) noex {
    	CONSLOG		*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = memclear(hop) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (conslog_ctor) */

static int conslog_dtor(conslog *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (conslog_dtor) */

template<typename ... Args>
static inline int conslog_magic(conslog *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == CONSLOG_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (conslog_magic) */

static int	conslog_fileready(conslog *) noex ;
static int	conslog_fileopen(conslog *) noex ;
static int	conslog_fileclose(conslog *) noex ;
static int	conslog_logdevice(conslog *,int,cchar *,int) noex ;

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
} ; /* end array (logfacs) */


/* exported variables */


/* exported subroutines */

int conslog_open(conslog *op,int logfac) noex {
	int		rs ;
	if ((rs = conslog_ctor(op)) >= 0) {
	    rs = SR_INVALID ;
	    memclear(op) ;
	    if (isLogFac(logfac)) {
	        op->lfd = -1 ;
	        op->magic = CONSLOG_MAGIC ;
	    } /* end if (valid) */
	    if (rs < 0) {
		conslog_dtor(op) ;
	    }
	} /* end if (conslog_ctor) */
	return rs ;
}
/* end subroutine (conslog_open) */

int conslog_close(conslog *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = conslog_magic(op)) >= 0) {
	    {
	        rs1 = conslog_fileclose(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = conslog_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (conslog_close) */

int conslog_printf(conslog *op,int logpri,cchar *fmt,...) noex {
	va_list		ap ;
	int		rs ;
	if ((rs = conslog_magic(op,fmt)) >= 0) {
	    va_begin(ap,fmt) ;
	    rs = conslog_vprintf(op,logpri,fmt,ap) ;
	    va_end(ap) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (conslog_printf) */

int conslog_vprintf(conslog *op,int logpri,cchar *fmt,va_list ap) noex {
    	cnullptr	np{} ;
	int		rs ;
	int		len = 0 ; /* return-value */
	if ((rs = conslog_magic(op,fmt,ap)) >= 0) {
	    if (char *lbuf ; (rs = malloc_ml(&lbuf)) >= 0) {
		cint	llen = rs ;
		if ((rs = snwvprintf(lbuf,llen,fmt,ap)) >= 0) {
		    int		sl = rs ;
		    cchar	*sp = lbuf ;
	            for (cchar *tp ; (tp = strnchr(sp,sl,'\n')) != np ; ) {
	                {
		            cint tl = intconv(tp - sp) ;
	                    rs = conslog_write(op,logpri,sp,tl) ;
	                    len += rs ;
	                }
	                sl -= intconv((tp + 1) - sp) ;
	                sp = (tp + 1) ;
	                if (rs < 0) break ;
		    } /* end while */
		    if ((rs >= 0) && (sl > 0)) {
	    	        rs = conslog_write(op,logpri,sp,sl) ;
	    	        len += rs ;
		    }
		} /* end if (snwvprintf) */
		rs = rsfree(rs,lbuf) ;
	    } /* end if (m-a-f) */
	} /* end if (magic) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (conslog_vprintf) */

int conslog_check(conslog *op,time_t dt) noex {
	int		rs ;
	if ((rs = conslog_magic(op)) >= 0) {
	    if (op->lfd >= 0) {
	        bool	f = false ;
	        f = f || ((dt - op->ti_write) >= TO_WRITE) ;
	        f = f || ((dt - op->ti_open) >= TO_OPEN) ;
	        if (f) {
	            rs = conslog_fileclose(op) ;
	        }
	    } /* end if (open) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (conslog_check) */

int conslog_write(conslog *op,int logpri,cchar *wbuf,int wlen) noex {
	int		rs ;
	int		len = 0 ; /* return-value */
	if ((rs = conslog_magic(op,wbuf)) >= 0) {
	    if (wlen < 0) wlen = lenstr(wbuf) ;
	    while (wlen && iseol(wbuf[wlen-1])) {
	        wlen -= 1 ;
	    }
	    while (wlen && CHAR_ISWHITE(wbuf[wlen-1])) {
	        wlen -= 1 ;
	    }
	    if (wlen > 0) {
		if ((rs = conslog_logdevice(op,logpri,wbuf,wlen)) >= 0) {
		    len = rs ;
		    op->c += 1 ;
		}
	    }
	} /* end if (magic) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (conslog_write) */

int conslog_count(conslog *op) noex {
	int		rs ;
	if ((rs = conslog_magic(op)) >= 0) {
	    rs = op->c ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (conslog_count) */


/* private subroutines */

static int conslog_fileready(conslog *op) noex {
    	int		rs = SR_OK ;
	if (op->lfd < 0) {
	    rs = conslog_fileopen(op) ;
	}
	return rs ;
}

static int conslog_fileopen(conslog *op) noex {
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

static int conslog_fileclose(conslog *op) noex {
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

static int conslog_logdevice(conslog *op,int logpri,cchar *bp,int bl) noex {
	struct log_ctl	lc{} ;
	STRBUF		cmsg ;
	STRBUF		dmsg ;
	int		rs ;
	if ((rs = conslog_fileready(op)) >= 0) {
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



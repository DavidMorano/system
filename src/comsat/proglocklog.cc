/* progloglock SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* process a locked-log-file note */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* switchable debug print-outs */

/* revision history:

	= 1998-03-01, David A­D­ Morano
	The subroutine was written from scratch.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	progloglock

	Description:
	These subroutine process log-file messages, but we have a
	lock around them because we can be called from multiple
	threads.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<cstdarg>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<localmisc.h>

#include	"config.h"
#include	"defs.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* external variables */


/* local variables */


/* exported variables */


/* exported subroutines */

int progloglock_begin(PROGINFO *pip) noex {
	PROGINFO_LOG	*ldp = &pip->logdata ;
	int		rs = SR_OK ;

	if (pip->open.logprog) {
	    ldp->ti_logsize = pip->daytime ;
	    ldp->ti_logcheck = pip->daytime ;
	    ldp->ti_logflush = pip->daytime ;

	    ldp->intlogsize = DEFINTLOGSIZE ;
	    ldp->intlogcheck = DEFINTLOGCHECK ;
	    ldp->intlogflush = DEFINTLOGFLUSH ;

	    if (! pip->open.lm) {
	        ptm *lmp = &ldp->lm ;
	        rs = lmp->create ;
	        pip->open.lm = (rs >= 0) ;
	    }
	}

	return rs ;
}
/* end subroutine (progloglock_begin) */

int progloglock_end(PROGINFO *pip) noex {
	PROGINFO_LOG	*ldp = &pip->logdata ;
	int		rs = SR_OK ;
	int		rs1 ;

	if (pip->open.logprog) {
	    if (pip->open.lm) {
	        pip->open.lm = false ;
	        ptm *lmp = &ldp->lm ;
	        rs1 = lmp->destroy ;
		if (rs >= 0) rs = rs1 ;
	    }
	}

	return rs ;
}
/* end subroutine (progloglock_end) */

int progloglock_printf(PROGINFO *pip,cchar *fmt,...) noex {
	va_list		ap ;
	PROGINFO_LOG	*ldp = &pip->logdata ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		len = 0 ;

	if (pip->open.logprog && pip->open.lm) {
	    ptm *lmp = &ldp->lm ;
	    if ((rs = lmp->lockbegin) >= 0) {
		{
	            va_begin(ap,fmt) ;
	            rs = logfile_vprintf(&pip->lh,fmt,ap) ;
		    len = rs ;
	            va_end(ap) ;
		}
	        rs1 = lmp->lockend ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (log-lock) */
	} /* end block */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (progloglockprintf) */

int progloglock_maint(PROGINFO *pip) noex {
	PROGINFO_LOG	*ldp = &pip->logdata ;
	int		rs = SR_OK ;
	int		rs1 ;
	if (pip->open.logprog && pip->open.lm) {
	    ptm *lmp = &ldp->lm ;
	    if ((rs = lmp->lockbegin) >= 0) {
		custime	dt = pip->daytime ;

	        if ((dt - ldp->ti_logsize) >= ldp->intlogsize) {
	            ldp->ti_logsize = pip->daytime ;
	            logfile_checksize(&pip->lh,pip->logsize) ;
		} else if ((dt - ldp->ti_logcheck) >= ldp->intlogcheck) {
	            ldp->ti_logcheck = pip->daytime ;
	            logfile_check(&pip->lh,pip->daytime) ;
	        } else if ((dt - ldp->ti_logflush) >= ldp->intlogflush) {
	            ldp->ti_logflush = pip->daytime ;
	            logfile_flush(&pip->lh) ;
	        }

	        rs1 = lmp->lockend ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (log-lock) */
	} /* end if (log open) */

	return rs ;
}
/* end subroutine (progloglock_maint) */



/* libprog_progpid SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* PID lock management */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* switchable at invocation */

/* revision history:

	= 2008-10-10, David A­D­ Morano
	This was adapted from the BACKGROUND program.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Description:
	These subroutines manage the main process PID lock.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/param.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<climits>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<lfm.h>			/* LIBUC */
#include	<strtime.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"proginfo.hh"

/* local defines */

#ifndef	VBUFLEN
#define	VBUFLEN		(4 * MAXPATHLEN)
#endif

#ifndef	EBUFLEN
#define	EBUFLEN		(4 * MAXPATHLEN)
#endif


/* external subroutines */

extern "C" {
    extern int	proglog_printf(PI *,cchar *,...) noex ;
    extern int	proglog_flush(PI *) noex ;
} /* end extern (C) */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int progpidbegin(PI *pip,int to) noex {
	int		rs = SR_OK ;
	int		f ;

	f = (pip->fl.named || pip->fl.passfd) ;
	if (! f) {
	    int		cl = -1 ;
	    cchar	*cp = pip->pidfname ;
	    char	tmpfname[MAXPATHLEN + 1] ;

	    if ((cp == nullptr) || (cp[0] == '+')) {
	        cp = pip->searchname ;
	        cl = -1 ;
	    }

	    if (cp[0] != '-') {

	        if (cp[0] != '/') {
	            if (strchr(cp,'/') != nullptr) {
	                cl = mkpath2(tmpfname,pip->pr,cp) ;
	            } else {
	                cl = mkpath3(tmpfname,pip->pr,RUNDNAME,cp) ;
	            }
	            cp = tmpfname ;
	        }

	        if (cp != nullptr) {
	            cchar	**vpp = &pip->pidfname ;
	            rs = proginfo_setentry(pip,vpp,cp,cl) ;
	        }

	        if (rs >= 0) {
	            lfm		*lmp = &pip->pidlock ;
	            cint	lt = LFM_TRECORD ;
	            cchar	*pf = pip->pidfname ;
	            cchar	*nn = pip->nodename ;
	            cchar	*un = pip->username ;
	            cchar	*bn = pip->banner ;
	            if ((rs = lfm_start(lmp,pf,lt,to,nullptr,nn,un,bn)) >= 0) {
	                pip->open.pidlock = true ;
	            }
	        }

	    } /* end if */

	} /* end if */

	return rs ;
} /* end subroutine (progpidbegin) */

int progpidcheck(PI *pip) noex {
	int		rs = SR_OK ;
	if (pip->open.pidlock) {
	    lfm			*lmp = &pip->pidlock ;
	    lfm_check		lc ;
	    custime		dt = pip->daytime ;
	    rs = lfm_check(lmp,&lc,dt) ;
	    if (rs < 0) {
	        cchar	*fmt ;
	        char	tbuf[TIMEBUFLEN + 1] ;
	        strtime_logz(dt,tbuf) ;
	        if (pip->open.logprog) {
	            fmt = "%s lost PIDLOCK other PID=%d\n" ;
	            proglog_printf(pip,fmt,tbuf,lc.pid) ;
	        }
	        if (pip->debuglevel > 0) {
	            fmt = "%s: %s lost PIDLOCK other PID=%d\n" ;
	            bprintf(pip->efp,tbuf,pip->progname,tbuf,lc.pid) ;
	        }
	    } /* end if */
	} /* end if */
	return rs ;
} /* end subroutine (progpidcheck) */

int progpidend(PI *pip) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (pip->open.pidlock) {
	    lfm	*lmp = &pip->pidlock ;
	    pip->open.pidlock = false ;
	    rs1 = lfm_finish(lmp) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if */
	return rs ;
} /* end subroutine (progpidend) */



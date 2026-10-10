/* libprog_progpid SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* PID lock management */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* non-switchable debug print-outs */
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
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<lfm.h>
#include	<strtime.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"config.h"
#include	"defs.h"


/* local defines */

#ifndef	VBUFLEN
#define	VBUFLEN		(4 * MAXPATHLEN)
#endif

#ifndef	EBUFLEN
#define	EBUFLEN		(4 * MAXPATHLEN)
#endif


/* external subroutines */

extern int	snsds(char *,int,cchar *,cchar *) ;
extern int	snwcpy(char *,int,cchar *,int) ;
extern int	mkpath1(char *,cchar *) ;
extern int	mkpath2(char *,cchar *,cchar *) ;
extern int	mkpath3(char *,cchar *,cchar *,cchar *) ;
extern int	cfdecti(cchar *,int,int *) ;

extern int	proglog_printf(PROGINFO *,cchar *,...) ;
extern int	proglog_flush(PROGINFO *) ;

extern char	*strwcpy(char *,cchar *,int) ;


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int progpidbegin(PROGINFO *pip,int to) noex {
	int		rs = SR_OK ;
	int		f ;

	f = (pip->fl.named || pip->fl.passfd) ;
	if (! f) {
	    int		cl = -1 ;
	    cchar	*cp = pip->pidfname ;
	    char	tmpfname[MAXPATHLEN + 1] ;

	    if ((cp == NULL) || (cp[0] == '+')) {
	        cp = pip->searchname ;
	        cl = -1 ;
	    }

	    if (cp[0] != '-') {

	        if (cp[0] != '/') {
	            if (strchr(cp,'/') != NULL) {
	                cl = mkpath2(tmpfname,pip->pr,cp) ;
	            } else {
	                cl = mkpath3(tmpfname,pip->pr,RUNDNAME,cp) ;
	            }
	            cp = tmpfname ;
	        }

	        if (cp != NULL) {
	            cchar	**vpp = &pip->pidfname ;
	            rs = proginfo_setentry(pip,vpp,cp,cl) ;
	        }

	        if (rs >= 0) {
	            LFM		*lmp = &pip->pidlock ;
	            cint	lt = LFM_TRECORD ;
	            cchar	*pf = pip->pidfname ;
	            cchar	*nn = pip->nodename ;
	            cchar	*un = pip->username ;
	            cchar	*bn = pip->banner ;
	            if ((rs = lfm_start(lmp,pf,lt,to,NULL,nn,un,bn)) >= 0) {
	                pip->open.pidlock = TRUE ;
	            }
	        }

	    } /* end if */

	} /* end if */

	return rs ;
}
/* end subroutine (progpidbegin) */


int progpidcheck(PROGINFO *pip)
{
	int		rs = SR_OK ;

	if (pip->open.pidlock) {
	    LFM			*lmp = &pip->pidlock ;
	    LFM_CHECK		lc ;
	    const time_t	dt = pip->daytime ;
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
	            printf(pip->efp,tbuf,pip->progname,tbuf,lc.pid) ;
	        }
	    } /* end if */
	} /* end if */

	return rs ;
}
/* end subroutine (progpidcheck) */


int progpidend(PROGINFO *pip)
{
	int		rs = SR_OK ;
	int		rs1 ;

	if (pip->open.pidlock) {
	    LFM		*lmp = &pip->pidlock ;
	    pip->open.pidlock = FALSE ;
	    rs1 = lfm_finish(lmp) ;
	    if (rs >= 0) rs = rs1 ;
	}

	return rs ;
}
/* end subroutine (progpidend) */



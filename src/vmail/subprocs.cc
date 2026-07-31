/* subprocs SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* manage a list of sub-process PIDs */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* non-switchable debug print-outs */

/* revision history:

	= 1998-07-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Description:
	This object manages a list of sub-processes that are desired
	to be tracked.

	Synopsis:
	int subprocs_start(SUBPROCS *op)

	Arguments:
	op		pointer to object

	Returns:
	>=0		OK
	<0		error (system-error)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<sys/wait.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>
#include	<cstddef>		/* |unllptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<localmisc.h>
#include	<exitcodes.h>

#include	"subprocs.h"


/* local defines */

#define	DEFENTRIES	4
#define	DEFPATH		"/usr/preroot/bin:/usr/xpg4/bin:/usr/bin:/usr/extra/bin"

#ifndef	ENVBUFLEN
#define	ENVBUFLEN	(MAXPATHLEN + 40)
#endif

#ifndef	PATHBUFLEN
#define	PATHBUFLEN	(8 * MAXPATHLEN)
#endif

#ifndef	VARPATH
#define	VARPATH		"PATH"
#endif


/* external subroutines */

#if	CF_DEBUGS
extern int	debugprintf(cchar *,...) ;
extern int	strlinelen(cchar *,int,int) ;
#endif


/* external variables */


/* local structures */


/* forward reference */


/* local variables */


/* exported variables */


/* exported subroutines */

int subprocs_start(SUBPROCS *op) noex {
	cint		n = DEFENTRIES ;
	cint		vo = (VECINT_OCOMPACT|VECINT_OSWAP|VECINT_OREUSE) ;
	int		rs ;

	if (op == NULL) return SR_FAULT ;

	memset(op,0,sizeof(SUBPROCS)) ;

	if ((rs = vecint_start(&op->pids,n,vo)) >= 0) {
	    op->magic = SUBPROCS_MAGIC ;
	}

	return rs ;
}
/* end subroutine (subprocs_start) */


int subprocs_finish(SUBPROCS *op)
{
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == NULL) return SR_FAULT ;
	if (op->magic != SUBPROCS_MAGIC) return SR_NOTOPEN ;

	rs1 = vecint_finish(&op->pids) ;
	if (rs >= 0) rs = rs1 ;

	op->magic = 0 ;
	return rs ;
}
/* end subroutine (subprocs_finish) */


int subprocs_add(SUBPROCS *op,pid_t pid)
{
	int		rs ;
	int		v = pid ;
	if (op == NULL) return SR_FAULT ;
	if (op->magic != SUBPROCS_MAGIC) return SR_NOTOPEN ;
	rs = vecint_add(&op->pids,v) ;
	return rs ;
}
/* end subroutine (subprocs_add) */


int subprocs_poll(SUBPROCS *op)
{
	VECINT		*vlp ;
	int		rs ;
	int		rs1 ;
	int		f = FALSE ;
	if (op == NULL) return SR_FAULT ;
	if (op->magic != SUBPROCS_MAGIC) return SR_NOTOPEN ;
	vlp = &op->pids ;
	if ((rs = vecint_count(vlp)) > 0) {
	    int		pi = op->pi ;
	    int		v ;
	    while ((rs = vecint_getval(vlp,pi,&v)) >= 0) {
		const pid_t	pid = v ;
		int		cs = 0 ;
		if (v != INT_MIN) {
		    if ((rs1 = u_waitpid(pid,&cs,WNOHANG)) > 0) {
		        f = TRUE ;
		        rs = vecint_del(vlp,pi--) ;
		    } else if (rs1 == SR_CHILD) {
		        rs = vecint_del(vlp,pi--) ;
		    } else {
		        rs = rs1 ;
		    }
		} /* end if (valid value) */
		pi = (pi+1) ;
#if	CF_DEBUGS
	debugprintf("subprocs_poll: waitpid-out pid=%d rs=%d\n",v,rs) ;
#endif
	    } /* end if (vecint-get) */
	    if (rs == SR_NOTFOUND) {
		rs = SR_OK ;
		op->pi = 0 ;
	    } else {
		op->pi = pi ;
	    }
	} /* end if (vecint-count) */
#if	CF_DEBUGS
	debugprintf("subprocs_poll: waitpid-out rs=%d f=%u\n",rs,f) ;
#endif
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (subprocs_poll) */


int subprocs_count(SUBPROCS *op)
{
	int		rs ;
	if (op == NULL) return SR_FAULT ;
	if (op->magic != SUBPROCS_MAGIC) return SR_NOTOPEN ;
	rs = vecint_count(&op->pids) ;
	return rs ;
}
/* end subroutine (subprocs_count) */



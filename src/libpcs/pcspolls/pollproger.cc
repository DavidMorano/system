/* pollproger SUPPORT (POLLPROG) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* this is a PCSPOLLS module for running the PCSPOLL program */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-06-01, David A­D­ Morano
	This code was originally written.

	= 2008-10-97, David A­D­ Morano
	Changed somewhat to fit into the new polling structure.

*/

/* Copyright © 1998,2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:

	Description:
	This object is a PCSPOLLS module for running the PCSPOLL
	program.

	Synopsis:
	int pollprog_start(op,pr,sn,envv,pcp)
	PCSPOLLS	*op ;
	cchar	*pr ;
	cchar	*sn ;
	cchar	**envv ;
	PCSCONF		*pcp ;

	Arguments:
	op		object pointer
	pr		program-root
	sn		search-name (of program calling us)
	envv		calling environment
	pcp		PCSCONF pointer

	Returns:
	>=0		OK
	<0		error code (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/param.h>
#include	<unistd.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<pcsconf.h>
#include	<storebuf.h>
#include	<upt.h>
#include	<localmisc.h>

#include	"pcspolls.h"


/* local defines */

#define	POLLPROG	struct pollprog_head
#define	POLLPROG_FL	struct pollprog_flags
#define	POLLPROG_MAGIC	0x88773422


/* imported namespaces */


/* local typedefs */

extern "C" {
    typedef int	(*thrsub_f)(void *) noex ;
}

typedef mainv	mv ;


/* external subroutines */

extern "C" {
    extern int	pollprogcheck(cchar *,cchar *,cchar **,PCSCONF *) noex ;
}


/* external variables */

extern cchar	**environ ;


/* local structures */

struct pollprog_flags {
	uint		working:1 ;
} ;

struct pollprog_head {
	cchar		*a ;		/* memory allocation */
	cchar		*pr ;
	cchar		*sn ;
	mainv		envv ;
	PCSCONF		*pcp ;
	pid_t		pid ;
	pthread_t	tid ;
	POLLPROG_FL	f ;
	uint		magic ;
	volatile int	f_exiting ;
} ;

enum cmds {
	cmd_noop,
	cmd_exit,
	cmd_overlast
} ;


/* forward references */

static int	pollprog_argsbegin(POLLPROG *,cchar *,cchar *) ;
static int	pollprog_argsend(POLLPROG *) ;
static int	pollprog_worker(POLLPROG *) ;


/* local variables */


/* exported variables */

pcspolls_name	pollprog_mod = {
	"pollprog",
	sizeof(pollprog),
	0
} ;


/* exported subroutines */

int pollprog_start(POLLPROG *op,cc *pr,cc *sn,mv envv,PCSCONF *pcp) noex {
	int		rs ;

	if (op == NULL) return SR_FAULT ;

	if (envv == NULL) envv = environ ;

	memclear(op) ;
	op->envv = envv ;
	op->pcp = pcp ;
	op->pid = getpid() ;

	if ((rs = pollprog_argsbegin(op,pr,sn)) >= 0) {
	    if ((pr != NULL) && (sn != NULL)) {
	        pthread_t	tid ;
	        thrsub_f	thr = (thrsub_f) pollprog_worker ;
	        if ((rs = uptcreate(&tid,NULL,thr,op)) >= 0) {
	            op->fl.working = TRUE ;
		    op->tid = tid ;
	        }
	    } /* end if (non-null) */
	    if (rs >= 0) {
	        op->magic = POLLPROG_MAGIC ;
	    }
	    if (rs < 0)
		pollprog_argsend(op) ;
	} /* end if (pollprog_argsbegin) */

#if	CF_DEBUGS
	debugprintf("pollprog_start: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (pollprog_start) */


int pollprog_finish(POLLPROG *op)
{
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == NULL) return SR_FAULT ;

	if (op->magic != POLLPROG_MAGIC) return SR_NOTOPEN ;

#if	CF_DEBUGS
	debugprintf("pollprog_finish: f_working=%d\n",op->fl.working) ;
#endif

	if (op->fl.working) {
	    const pid_t	pid = getpid() ;
	    if (pid == op->pid) {
	        int	trs = 0 ;
	        op->fl.working = FALSE ;
	        rs1 = uptjoin(op->tid,&trs) ;
	        if (rs >= 0) rs = rs1 ;
	        if (rs >= 0) rs = trs ;
	    } else {
		op->fl.working = FALSE ;
		op->tid = 0 ;
	    }
	}

	rs1 = pollprog_argsend(op) ;
	if (rs >= 0) rs = rs1 ;

#if	CF_DEBUGS
	debugprintf("pollprog_finish: ret rs=%d\n",rs) ;
#endif

	op->magic = 0 ;
	return rs ;
}
/* end subroutine (pollprog_finish) */


int pollprog_check(POLLPROG *op)
{
	int		rs = SR_OK ;
	int		rs1 ;
	int		f = FALSE ;

	if (op == NULL) return SR_FAULT ;

	if (op->magic != POLLPROG_MAGIC) return SR_NOTOPEN ;

	if (op->fl.working) {
	    const pid_t	pid = getpid() ;
	    if (pid == op->pid) {
	        if (op->f_exiting) {
	            int		trs = 0 ;
	            op->fl.working = FALSE ;
	            rs1 = uptjoin(op->tid,&trs) ;
	            if (rs >= 0) rs = rs1 ;
	            if (rs >= 0) rs = trs ;
	            f = TRUE ;
		}
	    } else {
		op->fl.working = FALSE ;
	    }
	}

	return (rs >= 0) ? f : rs ;
}
/* end subroutine (pollprog_check) */


/* private subroutines */


static int pollprog_argsbegin(POLLPROG *op,cchar *pr,cchar *sn)
{
	int		rs ;
	int		size = 0 ;
	char		*bp ;
	size += (((pr !=NULL)?strlen(pr):0)+1) ;
	size += (((sn !=NULL)?strlen(sn):0)+1) ;
	if ((rs = uc_malloc(size,&bp)) >= 0) {
	    op->a = bp ;
	    if (pr != NULL) {
	        op->pr = bp ;
	        bp = (strwcpy(bp,pr,-1)+1) ;
	    }
	    if (sn != NULL) {
	        op->sn = bp ;
	        bp = (strwcpy(bp,sn,-1)+1) ;
	    }
	} /* end if (m-a) */
	return rs ;
}
/* end subroutine (pollprog_argsbegin) */


static int pollprog_argsend(POLLPROG *op)
{
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->a != NULL) {
	    rs1 = uc_free(op->a) ;
	    if (rs >= 0) rs = rs1 ;
	    op->a = NULL ;
	}
	return rs ;
}
/* end subroutine (pollprog_argsend) */


static int pollprog_worker(POLLPROG *op)
{
	PCSCONF		*pcp = op->pcp ;
	int		rs ;
	cchar	**envv = op->envv ;
	cchar	*pr = op->pr ;
	cchar	*sn = op->sn ;

#if	CF_DEBUGS
	debugprintf("pollprog_worker: ent\n") ;
#endif

	    rs = pollprogcheck(pr,sn,envv,pcp) ;

#if	CF_DEBUGS
	debugprintf("pollprog/work_start: ret rs=%d\n",rs) ;
#endif

	op->f_exiting = TRUE ;
	return rs ;
}
/* end subroutine (pollprog_worker) */



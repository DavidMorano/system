/* pollproger SUPPORT (PP) */
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
	int pollprog_start(pcspolls *op,cc *pr,cc *sn,
		mainv envv,pcsconf *pcp) noex

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
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<pcsconf.h>
#include	<storebuf.h>
#include	<upt.h>
#include	<localmisc.h>

#include	"pcspolls.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */

#define	PP	pollprog_head
#define	PP_FL	pollprog_flags
#define	PP_MAGIC	0x88773422


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


/* local structures */

struct pollprog_flags {
	uint		working:1 ;
} ; /* end struct */

struct pollprog_head {
	char		*a ;		/* memory allocation */
	cchar		*pr ;
	cchar		*sn ;
	mainv		envv ;
	PCSCONF		*pcp ;
	pid_t		pid ;
	pthread_t	tid ;
	PP_FL		fl ;
	uint		magval ;
	volatile int	f_exiting ;
} ; /* end struct */

enum cmds {
	cmd_noop,
	cmd_exit,
	cmd_overlast
} ; /* end enum */


/* forward references */

local int	pollprog_envv(pollprog *,mainv) noex ;
local int	pollprog_argsbegin(PP *,cchar *,cchar *) ;
local int	pollprog_argsend(PP *) ;
local int	pollprog_worker(PP *) ;


/* local variables */


/* exported variables */

pcspolls_name	pollprog_mod = {
	"pollprog",
	szof(pollprog),
	0
} ;


/* exported subroutines */

int pollprog_start(PP *op,cc *pr,cc *sn,mainv ev,PCSCONF *pcp) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    memclear(op) ;
	    if ((pollprog_envv(op,ev)) >= 0) {
	        op->pcp = pcp ;
	        op->pid = getpid() ;
	        if ((rs = pollprog_argsbegin(op,pr,sn)) >= 0) {
	            if ((pr != nullptr) && (sn != nullptr)) {
	                pthread_t	tid ;
	                thrsub_f	thr = (thrsub_f) pollprog_worker ;
	                if ((rs = uptcreate(&tid,nullptr,thr,op)) >= 0) {
	                    op->fl.working = true ;
		            op->tid = tid ;
	                }
	            } /* end if (non-null) */
	            if (rs >= 0) {
	                op->magval = PP_MAGIC ;
	            }
	            if (rs < 0) {
		        pollprog_argsend(op) ;
	            }
	        } /* end if (pollprog_argsbegin) */
	    /* end if (pollprog_envv) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (pollprog_start) */

int pollprog_finish(PP *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magval != PP_MAGIC) return SR_NOTOPEN ;

#if	CF_DEBUGS
	debugprintf("pollprog_finish: f_working=%d\n",op->fl.working) ;
#endif

	if (op->fl.working) {
	    const pid_t	pid = getpid() ;
	    if (pid == op->pid) {
	        int	trs = 0 ;
	        op->fl.working = false ;
	        rs1 = uptjoin(op->tid,&trs) ;
	        if (rs >= 0) rs = rs1 ;
	        if (rs >= 0) rs = trs ;
	    } else {
		op->fl.working = false ;
		op->tid = 0 ;
	    }
	}
	{
	rs1 = pollprog_argsend(op) ;
	if (rs >= 0) rs = rs1 ;
	}

#if	CF_DEBUGS
	debugprintf("pollprog_finish: ret rs=%d\n",rs) ;
#endif

	op->magval = 0 ;
	return rs ;
}
/* end subroutine (pollprog_finish) */

int pollprog_check(PP *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		f = false ;
	if (op) {
	    rs = SR_NOTOPEN ;
	    if (op->magval == PP_MAGIC) {
	        if (op->fl.working) {
	            static const pid_t	pid = getpid() ;
	            if (pid == op->pid) {
	                if (op->f_exiting) {
	                    int		trs = 0 ;
	                    op->fl.working = false ;
	                    rs1 = uptjoin(op->tid,&trs) ;
	                    if (rs >= 0) rs = rs1 ;
	                    if (rs >= 0) rs = trs ;
	                    f = true ;
		        }
	            } else {
		        op->fl.working = false ;
	            }
	        }
	    } /* end if (magval) */
	} /* end if (non-null) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (pollprog_check) */


/* private subroutines */

local int pollprog_envv(pollprog *op,mainv ev) noex {
    	int	rs = SR_OK ;
	if ((op->envv = ev) == nullptr) {
	    if ((rs = u_getenviron(&ev)) >= 0) {
		op->envv = ev ;
	    }
	}
	return rs ;
} /* end subroutine (pollprog_envv) */

local int pollprog_argsbegin(PP *op,cchar *pr,cchar *sn) noex {
	int		rs ;
	int		sz = 0 ;
	char		*bp ;
	sz += (((pr !=nullptr)?lenstr(pr):0)+1) ;
	sz += (((sn !=nullptr)?lenstr(sn):0)+1) ;
	if ((rs = uc_malloc(sz,&bp)) >= 0) {
	    op->a = bp ;
	    if (pr != nullptr) {
	        op->pr = bp ;
	        bp = (strwcpy(bp,pr,-1)+1) ;
	    }
	    if (sn != nullptr) {
	        op->sn = bp ;
	        bp = (strwcpy(bp,sn,-1)+1) ;
	    }
	} /* end if (m-a) */
	return rs ;
}
/* end subroutine (pollprog_argsbegin) */

local int pollprog_argsend(PP *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->a != nullptr) {
	    rs1 = uc_free(op->a) ;
	    if (rs >= 0) rs = rs1 ;
	    op->a = nullptr ;
	}
	return rs ;
}
/* end subroutine (pollprog_argsend) */

local int pollprog_worker(PP *op) noex {
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

	op->f_exiting = true ;
	return rs ;
}
/* end subroutine (pollprog_worker) */

local int pollprog_envv(pollprog *op,mainv ev) noex {
    	int	rs = SR_OK ;
	if ((op->envv = ev) == nullptr) {
	    if ((rs = u_getenviron(&ev)) >= 0) {
		op->envv = ev ;
	    }
	}
	return rs ;
} /* end subroutine (pollprog_envv) */



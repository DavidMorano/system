/* thrbase SUPPORT */
/* lang=C++20 */

/* this object implements a sort of base thread manager (THRBASE) */
/* version %I% last-modified %G% */


/* revision history:

	= 2008-10-07, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	thrbase

	Description:
	This object manages a basic thread.

	Synopsis:
	typedef int (*worker_f)(THRBASE *,void *) ;
	int thrbase_start(THRBASE *op,worker_f,ap) noex

	Arguments:
	op		object pointer
	worker_f	subroutine to start as a thread
	ap		argument to the worker thread

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* must be before others */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<limits.h>
#include	<unistd.h>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<usupport.h>		/* <- for |memclear(3uc)| */
#include	<upt.h>
#include	<localmisc.h>

#include	"thrbase.h"


/* local defines */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

extern "C" {
    static int	startworker(THRBASE_SI *) noex ;
}


/* local variables */


/* exported variables */


/* exported subroutines */

int thrbase_start(THRBASE *tip,thrbase_sub worker,void *ap) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (tip && worker) {
	    memclear(tip) ;
	    tip->ap = ap ;
	    if ((rs = thrcomm_start(&tip->tc,0)) >= 0) {
	        sigset_t	osm{} ;
	        sigset_t	nsm{} ;
	        if ((rs = uc_sigsetfill(&nsm)) >= 0) {
		    if ((rs = pt_sigmask(SIG_BLOCK,&nsm,&osm)) >= 0) {
	                THRBASE_SI	*sip ;
	                cint		size = sizeof(THRBASE_SI) ;
	                if ((rs = uc_malloc(size,&sip)) >= 0) {
	                    uptsub_f	thrsub = uptsub_f(startworker) ;
	                    pthread_t	tid ;
		            {
		                memclear(sip) ;
		                sip->worker = worker ;
		                sip->tip = tip ;
		            }
	                    if ((rs = uptcreate(&tid,NULL,thrsub,sip)) >= 0) {
	                        tip->tid = tid ;
			    }
		            if (rs < 0) {
		                uc_free(sip) ;
			    }
	                } /* end if (memory-allocation) */
		        rs1 = pt_sigmask(SIG_SETMASK,&osm,NULL) ;
		        if (rs >= 0) rs = rs1 ;
		    } /* end if (sigmask) */
	        } /* end if (signal handling) */
	    } /* end if (thrcomm-start) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (thrbase_start) */

int thrbase_finish(THRBASE *tip) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (tip) {
	    if ((rs = thrbase_cmdexit(tip)) >= 0) {
	        rs = thrbase_waitexit(tip) ;
	        if (rs >= 0) rs = tip->trs ;
	    }
	    {
	        rs1 = thrcomm_finish(&tip->tc) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (thrbase_finish) */

int thrbase_cmdsend(THRBASE *tip,int cmd,int to) noex {
	int		rs = SR_FAULT ;
	if (tip) {
	    rs = SR_OK ;
	    if (! tip->f_exiting) {
	        rs = thrcomm_cmdsend(&tip->tc,cmd,to) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (thrbase_cmdsend) */

int thrbase_cmdexit(THRBASE *tip) noex {
	int		rs = SR_FAULT ;
	if (tip) {
	    cint	cmd = thrbasecmd_exit ;
	    rs = SR_OK ;
	    if (! tip->f_exiting) {
	        rs = thrbase_cmdsend(tip,cmd,-1) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (thrbase_cmdexit) */

/* called by child-thread */
int thrbase_cmdrecv(THRBASE *tip,int to) noex {
	int		rs = SR_FAULT ;
	int		cmd = 0 ;
	if (tip) {
	    if ((rs = thrcomm_cmdrecv(&tip->tc,to)) >= 0) {
	        cmd = rs ;
	    } else if (rs == SR_TIMEDOUT) {
	        rs = SR_OK ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? cmd : rs ;
}
/* end subroutine (thrbase_cmdrecv) */

/* called by child-thread */
int thrbase_exiting(THRBASE *tip) noex {
	int		rs = SR_FAULT ;
	if (tip) {
	    tip->f_exiting = true ;
	    rs = thrcomm_exiting(&tip->tc) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (thrbase_exiting) */

int thrbase_waitexit(THRBASE *tip) noex {
	int		rs = SR_FAULT ;
	int		f_exited = false ;
	if (tip) {
	    rs = SR_OK ;
	    if (! tip->f_exited) {
	        int	trs = SR_INPROGRESS ;
	        if ((rs = uptjoin(tip->tid,&trs)) >= 0) {
	            tip->trs = trs ;
	            tip->f_exited = true ;
		    f_exited = true ;
	        }
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? f_exited : rs ;
}
/* end subroutine (thrbase_waitexit) */

#ifdef	COMMENT
static int thrbase_cmddone(THRBASE *tip) noex {
	int		rs = SR_FAULT ;
	if (yip) {
	    cint	rrs = 1 ;
	    rs = thrcomm_rspsend(&tip->tc,rrs,-1) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (thrbase_setdone) */
#endif /* COMMENT */


/* private subroutines */

static int startworker(THRBASE_SI *sip) noex {
	THRBASE		*tip = sip->tip ;
	int		(*worker)(THRBASE *,void *) = sip->worker ;
	int		rs ;
	int		rs1 ;
	int		rsw = 0 ;
	if ((rs = uc_free(sip)) >= 0) {
	    {
	        rs = (*worker)(tip,tip->ap) ;
		rsw = rs ;
	    }
	    rs1 = thrbase_exiting(tip) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if */
	return (rs >= 0) ? rsw : rs ;
}
/* end subroutine (startworker) */



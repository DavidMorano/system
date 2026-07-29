/* thrbase SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* this object implements a sort of base thread manager (THRBASE) */
/* version %I% last-modified %G% */


/* revision history:

	= 2008-10-07, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	thrbase

	Description:
	This object manages a basic thread.

	Synopsis:
	typedef int (*worker_f)(thrbase *,void *) ;
	int thrbase_start(thrbase *op,worker_f,ap) noex

	Arguments:
	op		object pointer
	worker_f	subroutine to start as a thread
	ap		argument to the worker thread

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<climits>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<ucsig.h>		/* LIBUC */
#include	<upt.h>			/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"thrbase.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */


/* imported namespaces */

using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
local int thrbase_ctor(thrbase *op,Args ... args) noex {
	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = SR_NOMEM ;
	    op->ap = np ;
	    op->sip = np ;
	    op->tcp = np ;
	    op->tid = 0 ;
	    op->trs = 0 ;
	    op->f_exiting = {} ;
	    op->f_exited = {} ;
	    if ((op->tcp = new(nothrow) thrcomm) != np) ylikely {
		rs = SR_OK ;
	    } /* end if (new-thrcomm) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (thrbase_ctor) */

local int thrbase_dtor(thrbase *op) noex {
	int		rs = SR_OK ;
	if (op->tcp) ylikely {
	    delete op->tcp ;
	    op->tcp = nullptr ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (thrbase_dtor) */

extern "C" {
    local int	startworker(THRBASE_SI *) noex ;
}


/* local variables */


/* exported variables */


/* exported subroutines */

int thrbase_start(thrbase *op,thrbase_sub worker,void *ap) noex {
	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	int		rs1 ;
	if ((rs = thrbase_ctor(op,worker)) >= 0) ylikely {
	    op->ap = ap ;
	    if ((rs = thrcomm_start(op->tcp,0)) >= 0) {
	        if (sigset_t nsm{} ; (rs = uc_sigsetfill(&nsm)) >= 0) {
	            sigset_t	osm{} ;
		    if ((rs = u_sigmask(SIG_BLOCK,&nsm,&osm)) >= 0) {
	                THRBASE_SI	*sip ;
	                cint		sz = szof(THRBASE_SI) ;
	                if ((rs = lm_mall(sz,&sip)) >= 0) {
	                    uptsub_f	thrsub = uptsub_f(startworker) ;
	                    pthread_t	tid ;
		            {
		                memclear(sip) ;
		                sip->worker = worker ;
		                sip->op = op ;
		            }
	                    if ((rs = uptcreate(&tid,np,thrsub,sip)) >= 0) {
	                        op->tid = tid ;
			    }
		            if (rs < 0) {
		                lm_free(sip) ;
			    } /* end if (error) */
	                } /* end if (memory-acquire) */
		        rs1 = u_sigmask(SIG_SETMASK,&osm,np) ;
		        if (rs >= 0) rs = rs1 ;
		    } /* end if (sigmask) */
	        } /* end if (signal handling) */
	    } /* end if (thrcomm-start) */
	    if (rs < 0) {
		thrbase_dtor(op) ;
	    }
	} /* end if (thrbase_ctor) */
	return rs ;
} /* end subroutine (thrbase_start) */

int thrbase_finish(thrbase *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) ylikely {
	    if ((rs = thrbase_cmdexit(op)) >= 0) {
	        rs = thrbase_waitexit(op) ;
	        if (rs >= 0) rs = op->trs ;
	    }
	    {
	        rs1 = thrcomm_finish(op->tcp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = thrbase_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (thrbase_finish) */

int thrbase_cmdsend(thrbase *op,int cmd,int to) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (! op->f_exiting) {
	        rs = thrcomm_cmdsend(op->tcp,cmd,to) ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (thrbase_cmdsend) */

int thrbase_cmdexit(thrbase *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    cint	cmd = thrbasecmd_exit ;
	    rs = SR_OK ;
	    if (! op->f_exiting) {
	        rs = thrbase_cmdsend(op,cmd,-1) ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (thrbase_cmdexit) */

/* called by child-thread */
int thrbase_cmdrecv(thrbase *op,int to) noex {
	int		rs = SR_FAULT ;
	int		cmd = 0 ;
	if (op) {
	    if ((rs = thrcomm_cmdrecv(op->tcp,to)) >= 0) {
	        cmd = rs ;
	    } else if (rs == SR_TIMEDOUT) {
	        rs = SR_OK ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? cmd : rs ;
} /* end subroutine (thrbase_cmdrecv) */

/* called by child-thread */
int thrbase_exiting(thrbase *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    op->f_exiting = true ;
	    rs = thrcomm_exiting(op->tcp) ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (thrbase_exiting) */

int thrbase_waitexit(thrbase *op) noex {
	int		rs = SR_FAULT ;
	int		f_exited = false ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (! op->f_exited) {
	        int	trs = SR_INPROGRESS ;
	        if ((rs = uptjoin(op->tid,&trs)) >= 0) {
	            op->trs = trs ;
	            op->f_exited = true ;
		    f_exited = true ;
	        }
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? f_exited : rs ;
} /* end subroutine (thrbase_waitexit) */

#ifdef	COMMENT
local int thrbase_cmddone(thrbase *op) noex {
	int		rs = SR_FAULT ;
	if (yip) ylikely {
	    cint	rrs = 1 ;
	    rs = thrcomm_rspsend(op->tcp,rrs,-1) ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (thrbase_setdone) */
#endif /* COMMENT */


/* private subroutines */

local int startworker(THRBASE_SI *sip) noex {
	thrbase		*op = sip->op ;
	int		(*worker)(thrbase *,void *) = sip->worker ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		rsf ; /* <- gyration for auditing */
	int		rsw = 0 ; /* return-value */
	if ((rsf = lm_free(sip)) >= 0) ylikely {
	    {
	        rs = (*worker)(op,op->ap) ;
		rsw = rs ;
	    }
	    rs1 = thrbase_exiting(op) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if */
	if (rs >= 0) rs = rsf ;
	return (rs >= 0) ? rsw : rs ;
} /* end subroutine (startworker) */



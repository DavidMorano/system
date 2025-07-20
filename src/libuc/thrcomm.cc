/* thrcomm SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* Thread-Communication (THRCOMM) */
/* version %I% last-modified %G% */


/* revision history:

	= 2008-10-07, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	thrcomm

	Description:
	This object provides some minimal communication between a
	controller thread and a separate parallel thread.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<ctime>
#include	<usystem.h>
#include	<localmisc.h>

#include	"thrcomm.h"


/* local defines */


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* forward references */

template<typename ... Args>
static int thrcomm_ctor(thrcomm *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    rs = SR_NOMEM ;
	    op->magic = 0 ;
	    op->cmd = 0 ;
	    op->rrs = 0 ;
	    op->f_cmd = {} ;
	    op->f_exiting = {} ;
	    if ((op->mxp = new(nothrow) ptm) != np) {
	        if ((op->cvp = new(nothrow) ptc) != np) {
		    rs = SR_OK ;
		}
		if (rs < 0) {
		    delete op->mxp ;
		    op->mxp = nullptr ;
		}
	    } /* end if (new-ptm) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (thrcomm_ctor) */

static int thrcomm_dtor(thrcomm *op) noex {
	int		rs = SR_OK ;
	if (op->cvp) {
	    delete op->cvp ;
	    op->cvp = nullptr ;
	}
	if (op->mxp) {
	    delete op->mxp ;
	    op->mxp = nullptr ;
	}
	return rs ;
}
/* end subroutine (thrcomm_dtor) */

template<typename ... Args>
static int thrcomm_magic(thrcomm *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == THRCOMM_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (thrcomm_magic) */

static int	thrcomm_ptminit(thrcomm *,int) noex ;
static int	thrcomm_ptcinit(thrcomm *,int) noex ;


/* local variables */

constexpr clockid_t	cid = CLOCK_REALTIME ;


/* exported variables */


/* exported subroutines */

int thrcomm_start(thrcomm *op,int f_shared) noex {
	int		rs ;
	if ((rs = thrcomm_ctor(op)) >= 0) {
	    if ((rs = thrcomm_ptminit(op,f_shared)) >= 0) {
	        if ((rs = thrcomm_ptcinit(op,f_shared)) >= 0) {
		    op->magic = THRCOMM_MAGIC ;
		}
	        if (rs < 0) {
		    ptm_destroy(op->mxp) ;
		}
	    } /* end if (PTM created) */
	    if (rs < 0) {
		thrcomm_dtor(op) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (thrcomm_start) */

int thrcomm_finish(thrcomm *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = thrcomm_magic(op)) >= 0) {
	    {
	        rs1 = ptc_destroy(op->cvp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = ptm_destroy(op->mxp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = thrcomm_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (thrcomm_finish) */

int thrcomm_cmdsend(thrcomm *op,int cmd,int to) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = thrcomm_magic(op)) >= 0) {
	    TIMESPEC	ts{} ;
	    if (to >= 0) {
	        clock_gettime(cid,&ts) ;
	        ts.tv_sec += to ;
	    }
	    if ((rs = ptm_lockto(op->mxp,to)) >= 0) {
	        op->f_cmd = true ;
	        while ((rs >= 0) && (op->cmd != 0) && (! op->f_exiting)) {
		    if (to >= 0) {
	                rs = ptc_timedwait(op->cvp,op->mxp,&ts) ;
		    } else {
	                rs = ptc_wait(op->cvp,op->mxp) ;
		    }
	        } /* end while */
	        if (rs >= 0) {
	            if (! op->f_exiting) {
	                op->cmd = cmd ;
	                op->rrs = SR_INPROGRESS ;
                        rs = ptc_broadcast(op->cvp) ;
		    } else {
		        cmd = 0 ;
		    }
	        } /* end if (ok) */
	        op->f_cmd = false ;
	        rs1 = ptm_unlock(op->mxp) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (ptm) */
	} /* end if (magic) */
	return (rs >= 0) ? cmd : rs ;
}
/* end subroutine (thrcomm_cmdsend) */

int thrcomm_cmdrecv(thrcomm *op,int to) noex {
	int		rs ;
	int		rs1 ;
	int		cmd = 0 ;
	if ((rs = thrcomm_magic(op)) >= 0) {
	    TIMESPEC	ts{} ;
	    if (to >= 0) {
	        clock_gettime(cid,&ts) ;
	        ts.tv_sec += to ;
	    }
	    if ((rs = ptm_lockto(op->mxp,to)) >= 0) {
	        while ((rs >= 0) && (op->cmd == 0)) {
		    if (to >= 0) {
	                rs = ptc_timedwait(op->cvp,op->mxp,&ts) ;
		    } else {
	                rs = ptc_wait(op->cvp,op->mxp) ;
		    }
	        } /* end while */
	        if (rs >= 0) {
		    cmd = op->cmd ;
		    op->cmd = 0 ;
	        }
	        rs1 = ptm_unlock(op->mxp) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (ptm) */
	} /* end if (magic) */
	return (rs >= 0) ? cmd : rs ;
}
/* end subroutine (thrcomm_cmdrecv) */

int thrcomm_rspsend(thrcomm *op,int rrs,int to) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = thrcomm_magic(op)) >= 0) {
	    TIMESPEC	ts{} ;
	    if (to >= 0) {
	        clock_gettime(cid,&ts) ;
	        ts.tv_sec += to ;
	    }
	    if ((rs = ptm_lockto(op->mxp,to)) >= 0) {
	        while ((rs >= 0) && (op->rrs != SR_INPROGRESS)) {
		    if (to >= 0) {
	                rs = ptc_timedwait(op->cvp,op->mxp,&ts) ;
		    } else {
	                rs = ptc_wait(op->cvp,op->mxp) ;
		    }
	        } /* end while */
	        if (rs >= 0) {
		    op->rrs = rrs ;
                    rs = ptc_broadcast(op->cvp) ;
	        }
	        rs1 = ptm_unlock(op->mxp) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (ptm) */
	} /* end if (magic) */
	return (rs >= 0) ? rrs : rs ;
}
/* end subroutine (thrcomm_rspsend) */

int thrcomm_rsprecv(thrcomm *op,int to) noex {
	int		rs ;
	int		rs1 ;
	int		rrs = 0 ;
	if ((rs = thrcomm_magic(op)) >= 0) {
	    TIMESPEC	ts{} ;
	    if (to >= 0) {
	        clock_gettime(cid,&ts) ;
	        ts.tv_sec += to ;
	    }
	    if ((rs = ptm_lockto(op->mxp,to)) >= 0) {
	        while ((rs >= 0) && (op->rrs == SR_INPROGRESS)) {
		    if (to >= 0) {
	                rs = ptc_timedwait(op->cvp,op->mxp,&ts) ;
		    } else {
	                rs = ptc_wait(op->cvp,op->mxp) ;
		    }
	        } /* end while */
	        if (rs >= 0) {
		    rrs = op->rrs ;
		    op->rrs = SR_INPROGRESS ;
	        }
	        rs1 = ptm_unlock(op->mxp) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (ptm) */
	} /* end if (magic) */
	return (rs >= 0) ? rrs : rs ;
}
/* end subroutine (thrcomm_rsprecv) */

int thrcomm_exiting(thrcomm *op) noex {
	int		rs ;
	if ((rs = thrcomm_magic(op)) >= 0) {
	    op->f_exiting = true ;
	    op->cmd = 0 ;
	    if (op->f_cmd) {
    	        rs = ptc_broadcast(op->cvp) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (thrcomm_exiting) */


/* private subroutines */

static int thrcomm_ptminit(thrcomm *op,int f_shared) noex {
	int		rs ;
	int		rs1 ;
	if (ptma a ; (rs = ptma_create(&a)) >= 0) {
	    int		f_ptm = false ;
	    if (f_shared) {
		cint	v = PTHREAD_PROCESS_SHARED ;
		rs = ptma_setpshared(&a,v) ;
	    }
	    if (rs >= 0) {
	        rs = ptm_create(op->mxp,&a) ;
		f_ptm = (rs >= 0) ;
	    }
	    rs1 = ptma_destroy(&a) ;
	    if (rs >= 0) rs = rs1 ;
	    if ((rs < 0) && f_ptm) ptm_destroy(op->mxp) ;
	} /* end if (ptma) */
	return rs ;
}
/* end subroutine (thrcomm_ptminit) */

static int thrcomm_ptcinit(thrcomm *op,int f_shared) noex {
	int		rs ;
	int		rs1 ;
	if (ptca a ; (rs = ptca_create(&a)) >= 0) {
	    int		f_ptc = false ;
	    if (f_shared) {
		cint	v = PTHREAD_PROCESS_SHARED ;
		rs = ptca_setpshared(&a,v) ;
	    }
	    if (rs >= 0) {
	        rs = ptc_create(op->cvp,&a) ;
		f_ptc = (rs >= 0) ;
	    }
	    rs1 = ptca_destroy(&a) ;
	    if (rs >= 0) rs = rs1 ;
	    if ((rs < 0) && f_ptc) ptc_destroy(op->cvp) ;
	} /* end if (ptca) */
	return rs ;
}
/* end subroutine (thrcomm_ptcinit) */



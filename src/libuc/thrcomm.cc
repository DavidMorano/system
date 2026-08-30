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
#include	<sys/types.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<ctime>			/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"thrcomm.h"


/* local defines */


/* imported namespaces */

using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* forward references */

template<typename ... Args>
local int thrcomm_ctor(thrcomm *op,Args ... args) noex {
	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = SR_NOMEM ;
	    op->magval = 0 ;
	    op->cmd = 0 ;
	    op->rrs = 0 ;
	    op->f_cmd = {} ;
	    op->f_exiting = {} ;
	    if ((op->mxp = new(nothrow) ptm) != np) ylikely {
	        if ((op->cvp = new(nothrow) ptc) != np) ylikely {
		    rs = SR_OK ;
		}
		if (rs < 0) {
		    delete op->mxp ;
		    op->mxp = nullptr ;
		} /* end if (error) */
	    } /* end if (new-ptm) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (thrcomm_ctor) */

local int thrcomm_dtor(thrcomm *op) noex {
	int		rs = SR_OK ;
	if (op->cvp) ylikely {
	    delete op->cvp ;
	    op->cvp = nullptr ;
	} /* end if (delete-ptc) */
	if (op->mxp) ylikely {
	    delete op->mxp ;
	    op->mxp = nullptr ;
	} /* end if (delete-ptm) */
	return rs ;
} /* end subroutine (thrcomm_dtor) */

template<typename ... Args>
local inline int thrcomm_magic(thrcomm *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == THRCOMM_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (thrcomm_magic) */

local int	thrcomm_ptminit(thrcomm *,int) noex ;
local int	thrcomm_ptcinit(thrcomm *,int) noex ;


/* local variables */

constexpr clockid_t	cid = CLOCK_REALTIME ;


/* exported variables */


/* exported subroutines */

int thrcomm_start(thrcomm *op,int f_shared) noex {
	int		rs ;
	if ((rs = thrcomm_ctor(op)) >= 0) ylikely {
	    ptm *mxp = op->mxp ;
	    if ((rs = thrcomm_ptminit(op,f_shared)) >= 0) ylikely {
	        if ((rs = thrcomm_ptcinit(op,f_shared)) >= 0) ylikely {
		    op->magval = THRCOMM_MAGIC ;
		}
	        if (rs < 0) {
		    mxp->destroy() ;
		} /* end if (error) */
	    } /* end if (PTM created) */
	    if (rs < 0) {
		thrcomm_dtor(op) ;
	    } /* end if (error) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (thrcomm_start) */

int thrcomm_finish(thrcomm *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = thrcomm_magic(op)) >= 0) ylikely {
	    if (op->cvp) ylikely {
		ptc *cnp = op->cvp ;
	        rs1 = cnp->destroy ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->mxp) ylikely {
		ptm *mxp = op->mxp ;
	        rs1 = mxp->destroy ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = thrcomm_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magval = 0 ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (thrcomm_finish) */

int thrcomm_cmdsend(thrcomm *op,int cmd,int to) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = thrcomm_magic(op)) >= 0) ylikely {
	    ptm *mxp = op->mxp ;
	    TIMESPEC	ts{} ; /* used-afterwards */
	    if (to >= 0) {
	        clock_gettime(cid,&ts) ;
	        ts.tv_sec += to ;
	    } /* end if (time-out requested) */
	    if ((rs = mxp->lockbegin(to)) >= 0) ylikely {
		ptc *cnp = op->cvp ;
	        op->f_cmd = true ;
	        while ((rs >= 0) && (op->cmd != 0) && (! op->f_exiting)) {
		    if (to >= 0) {
	                rs = cnp->waiter(op->mxp,&ts) ;
		    } else {
	                rs = cnp->wait(op->mxp) ;
		    }
	        } /* end while */
	        if (rs >= 0) ylikely {
	            if (! op->f_exiting) {
	                op->cmd = cmd ;
	                op->rrs = SR_INPROGRESS ;
                        rs = cnp->broadcast ;
		    } else {
		        cmd = 0 ;
		    }
	        } /* end if (ok) */
	        op->f_cmd = false ;
	        rs1 = mxp->lockend ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (ptm) */
	} /* end if (magic) */
	return (rs >= 0) ? cmd : rs ;
} /* end subroutine (thrcomm_cmdsend) */

int thrcomm_cmdrecv(thrcomm *op,int to) noex {
	int		rs ;
	int		rs1 ;
	int		cmd = 0 ;
	if ((rs = thrcomm_magic(op)) >= 0) ylikely {
	    ptm *mxp = op->mxp ;
	    TIMESPEC	ts{} ; /* used-afterwards */
	    if (to >= 0) {
	        clock_gettime(cid,&ts) ;
	        ts.tv_sec += to ;
	    }
	    if ((rs = mxp->lockbegin(to)) >= 0) ylikely {
		ptc *cnp = op->cvp ;
	        while ((rs >= 0) && (op->cmd == 0)) {
		    if (to >= 0) {
	                rs = cnp->waiter(op->mxp,&ts) ;
		    } else {
	                rs = cnp->wait(op->mxp) ;
		    }
	        } /* end while */
	        if (rs >= 0) ylikely {
		    cmd = op->cmd ;
		    op->cmd = 0 ;
	        }
	        rs1 = mxp->lockend ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (ptm) */
	} /* end if (magic) */
	return (rs >= 0) ? cmd : rs ;
} /* end subroutine (thrcomm_cmdrecv) */

int thrcomm_rspsend(thrcomm *op,int rrs,int to) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = thrcomm_magic(op)) >= 0) ylikely {
	    ptm *mxp = op->mxp ;
	    TIMESPEC	ts{} ; /* used-afterwards */
	    if (to >= 0) {
	        clock_gettime(cid,&ts) ;
	        ts.tv_sec += to ;
	    }
	    if ((rs = mxp->lockbegin(to)) >= 0) ylikely {
		ptc *cnp = op->cvp ;
	        while ((rs >= 0) && (op->rrs != SR_INPROGRESS)) {
		    if (to >= 0) {
	                rs = cnp->waiter(op->mxp,&ts) ;
		    } else {
	                rs = cnp->wait(op->mxp) ;
		    }
	        } /* end while */
	        if (rs >= 0) ylikely {
		    op->rrs = rrs ;
                    rs = cnp->broadcast ;
	        } /* end if (ok) */
	        rs1 = mxp->lockend ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (ptm) */
	} /* end if (magic) */
	return (rs >= 0) ? rrs : rs ;
} /* end subroutine (thrcomm_rspsend) */

int thrcomm_rsprecv(thrcomm *op,int to) noex {
	int		rs ;
	int		rs1 ;
	int		rrs = 0 ;
	if ((rs = thrcomm_magic(op)) >= 0) ylikely {
	    ptm *mxp = op->mxp ;
	    TIMESPEC	ts{} ; /* used-afterwards */
	    if (to >= 0) {
	        clock_gettime(cid,&ts) ;
	        ts.tv_sec += to ;
	    } /* end if (time-out requested) */
	    if ((rs = mxp->lockbegin(to)) >= 0) ylikely {
		ptc *cnp = op->cvp ;
	        while ((rs >= 0) && (op->rrs == SR_INPROGRESS)) {
		    if (to >= 0) {
	                rs = cnp->waiter(op->mxp,&ts) ;
		    } else {
	                rs = cnp->wait(op->mxp) ;
		    }
	        } /* end while */
	        if (rs >= 0) ylikely {
		    rrs = op->rrs ;
		    op->rrs = SR_INPROGRESS ;
	        }
	        rs1 = mxp->lockend ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (ptm) */
	} /* end if (magic) */
	return (rs >= 0) ? rrs : rs ;
} /* end subroutine (thrcomm_rsprecv) */

int thrcomm_exiting(thrcomm *op) noex {
	int		rs ;
	if ((rs = thrcomm_magic(op)) >= 0) ylikely {
	    ptc *cnp = op->cvp ;
	    op->f_exiting = true ;
	    op->cmd = 0 ;
	    if (op->f_cmd) {
    	        rs = cnp->broadcast ;
	    }
	} /* end if (magic) */
	return rs ;
} /* end subroutine (thrcomm_exiting) */


/* private subroutines */

local int thrcomm_ptminit(thrcomm *op,int f_shared) noex {
	int		rs ;
	int		rs1 ;
	if (ptma a ; (rs = ptma_create(&a)) >= 0) ylikely {
	    ptm		*mxp = op->mxp ;
	    int		f_ptm = false ;
	    {
	        if (f_shared) {
		    cint	v = PTHREAD_PROCESS_SHARED ;
		    rs = ptma_setpshared(&a,v) ;
	        }
	        if (rs >= 0) ylikely {
	            rs = mxp->create(&a) ;
		    f_ptm = (rs >= 0) ;
	        }
	    } /* end block */
	    rs1 = ptma_destroy(&a) ;
	    if (rs >= 0) rs = rs1 ;
	    if ((rs < 0) && f_ptm) {
		mxp->destroy() ;
	    } /* end if (error) */
	} /* end if (ptma) */
	return rs ;
} /* end subroutine (thrcomm_ptminit) */

local int thrcomm_ptcinit(thrcomm *op,int f_shared) noex {
	int		rs ;
	int		rs1 ;
	if (ptca a ; (rs = ptca_create(&a)) >= 0) {
	    ptc		*cnp = op->cvp ;
	    int		f_ptc = false ;
	    {
	        if (f_shared) {
		    cint	v = PTHREAD_PROCESS_SHARED ;
		    rs = ptca_setpshared(&a,v) ;
	        }
	        if (rs >= 0) {
	            rs = cnp->create(&a) ;
		    f_ptc = (rs >= 0) ;
	        }
	    } /* end block */
	    rs1 = ptca_destroy(&a) ;
	    if (rs >= 0) rs = rs1 ;
	    if ((rs < 0) && f_ptc) {
		cnp->destroy() ;
	    } /* end if (error) */
	} /* end if (ptca) */
	return rs ;
} /* end subroutine (thrcomm_ptcinit) */



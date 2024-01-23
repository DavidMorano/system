/* thrcomm SUPPORT */
/* lang=C++20 */

/* Thread-Communication (THRCOMM) */
/* version %I% last-modified %G% */


/* revision history:

	= 2008-10-07, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This object provides some minimal communication between a
	controller thread and a separate parallel thread.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<cstring>
#include	<ctime>
#include	<usystem.h>
#include	<usupport.h>
#include	<localmisc.h>

#include	"thrcomm.h"


/* local defines */

#define	TO_NOSPC	5
#define	TO_MFILE	5
#define	TO_INTR		5


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* forward references */

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


/* exported variables */


/* exported subroutines */

int thrcomm_start(thrcomm *psp,int f_shared) noex {
	int		rs = SR_FAULT ;
	if (psp) {
	    memclear(psp) ;
	    if ((rs = thrcomm_ptminit(psp,f_shared)) >= 0) {
	        if ((rs = thrcomm_ptcinit(psp,f_shared)) >= 0) {
		    psp->magic = THRCOMM_MAGIC ;
		}
	        if (rs < 0) {
		    ptm_destroy(&psp->m) ;
		}
	    } /* end if (PTM created) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (thrcomm_start) */

int thrcomm_finish(thrcomm *psp) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = thrcomm_magic(psp)) >= 0) {
	    {
	        rs1 = ptc_destroy(&psp->c) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = ptm_destroy(&psp->m) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    psp->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (thrcomm_finish) */

int thrcomm_cmdsend(thrcomm *psp,int cmd,int to) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = thrcomm_magic(psp)) >= 0) {
	    TIMESPEC	ts{} ;
	    if (to >= 0) {
	        clock_gettime(CLOCK_REALTIME,&ts) ;
	        ts.tv_sec += to ;
	    }
	    if ((rs = ptm_lockto(&psp->m,to)) >= 0) {
	        psp->f_cmd = true ;
	        while ((rs >= 0) && (psp->cmd != 0) && (! psp->f_exiting)) {
		    if (to >= 0) {
	                rs = ptc_timedwait(&psp->c,&psp->m,&ts) ;
		    } else {
	                rs = ptc_wait(&psp->c,&psp->m) ;
		    }
	        } /* end while */
	        if (rs >= 0) {
	            if (! psp->f_exiting) {
	                psp->cmd = cmd ;
	                psp->rrs = SR_INPROGRESS ;
                        rs = ptc_broadcast(&psp->c) ;
		    } else {
		        cmd = 0 ;
		    }
	        } /* end if (ok) */
	        psp->f_cmd = false ;
	        rs1 = ptm_unlock(&psp->m) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (ptm) */
	} /* end if (magic) */
	return (rs >= 0) ? cmd : rs ;
}
/* end subroutine (thrcomm_cmdsend) */

int thrcomm_cmdrecv(thrcomm *psp,int to) noex {
	int		rs ;
	int		rs1 ;
	int		cmd = 0 ;
	if ((rs = thrcomm_magic(psp)) >= 0) {
	    TIMESPEC	ts{} ;
	    if (to >= 0) {
	        clock_gettime(CLOCK_REALTIME,&ts) ;
	        ts.tv_sec += to ;
	    }
	    if ((rs = ptm_lockto(&psp->m,to)) >= 0) {
	        while ((rs >= 0) && (psp->cmd == 0)) {
		    if (to >= 0) {
	                rs = ptc_timedwait(&psp->c,&psp->m,&ts) ;
		    } else {
	                rs = ptc_wait(&psp->c,&psp->m) ;
		    }
	        } /* end while */
	        if (rs >= 0) {
		    cmd = psp->cmd ;
		    psp->cmd = 0 ;
	        }
	        rs1 = ptm_unlock(&psp->m) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (ptm) */
	} /* end if (magic) */
	return (rs >= 0) ? cmd : rs ;
}
/* end subroutine (thrcomm_cmdrecv) */

int thrcomm_rspsend(thrcomm *psp,int rrs,int to) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = thrcomm_magic(psp)) >= 0) {
	    TIMESPEC	ts{} ;
	    if (to >= 0) {
	        clock_gettime(CLOCK_REALTIME,&ts) ;
	        ts.tv_sec += to ;
	    }
	    if ((rs = ptm_lockto(&psp->m,to)) >= 0) {
	        while ((rs >= 0) && (psp->rrs != SR_INPROGRESS)) {
		    if (to >= 0) {
	                rs = ptc_timedwait(&psp->c,&psp->m,&ts) ;
		    } else {
	                rs = ptc_wait(&psp->c,&psp->m) ;
		    }
	        } /* end while */
	        if (rs >= 0) {
		    psp->rrs = rrs ;
                    rs = ptc_broadcast(&psp->c) ;
	        }
	        rs1 = ptm_unlock(&psp->m) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (ptm) */
	} /* end if (magic) */
	return (rs >= 0) ? rrs : rs ;
}
/* end subroutine (thrcomm_rspsend) */

int thrcomm_rsprecv(thrcomm *psp,int to) noex {
	int		rs ;
	int		rs1 ;
	int		rrs = 0 ;
	if ((rs = thrcomm_magic(psp)) >= 0) {
	    TIMESPEC	ts{} ;
	    if (to >= 0) {
	        clock_gettime(CLOCK_REALTIME,&ts) ;
	        ts.tv_sec += to ;
	    }
	    if ((rs = ptm_lockto(&psp->m,to)) >= 0) {
	        while ((rs >= 0) && (psp->rrs == SR_INPROGRESS)) {
		    if (to >= 0) {
	                rs = ptc_timedwait(&psp->c,&psp->m,&ts) ;
		    } else {
	                rs = ptc_wait(&psp->c,&psp->m) ;
		    }
	        } /* end while */
	        if (rs >= 0) {
		    rrs = psp->rrs ;
		    psp->rrs = SR_INPROGRESS ;
	        }
	        rs1 = ptm_unlock(&psp->m) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (ptm) */
	} /* end if (magic) */
	return (rs >= 0) ? rrs : rs ;
}
/* end subroutine (thrcomm_rsprecv) */

int thrcomm_exiting(thrcomm *psp) noex {
	int		rs ;
	if ((rs = thrcomm_magic(psp)) >= 0) {
	    psp->f_exiting = true ;
	    psp->cmd = 0 ;
	    if (psp->f_cmd) {
    	        rs = ptc_broadcast(&psp->c) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (thrcomm_exiting) */


/* private subroutines */

static int thrcomm_ptminit(thrcomm *psp,int f_shared) noex {
	ptma		a ;
	int		rs ;
	int		rs1 ;
	int		f_ptm = false ;
	if ((rs = ptma_create(&a)) >= 0) {
	    if (f_shared) {
		cint	v = PTHREAD_PROCESS_SHARED ;
		rs = ptma_setpshared(&a,v) ;
	    }
	    if (rs >= 0) {
	        rs = ptm_create(&psp->m,&a) ;
		f_ptm = (rs >= 0) ;
	    }
	    rs1 = ptma_destroy(&a) ;
	    if (rs >= 0) rs = rs1 ;
	    if ((rs < 0) && f_ptm) ptm_destroy(&psp->m) ;
	} /* end if (ptma) */
	return rs ;
}
/* end subroutine (thrcomm_ptminit) */

static int thrcomm_ptcinit(thrcomm *psp,int f_shared) noex {
	ptca		a ;
	int		rs ;
	int		rs1 ;
	int		f_ptc = false ;
	if ((rs = ptca_create(&a)) >= 0) {
	    if (f_shared) {
		cint	v = PTHREAD_PROCESS_SHARED ;
		rs = ptca_setpshared(&a,v) ;
	    }
	    if (rs >= 0) {
	        rs = ptc_create(&psp->c,&a) ;
		f_ptc = (rs >= 0) ;
	    }
	    rs1 = ptca_destroy(&a) ;
	    if (rs >= 0) rs = rs1 ;
	    if ((rs < 0) && f_ptc) ptc_destroy(&psp->c) ;
	} /* end if (ptca) */
	return rs ;
}
/* end subroutine (thrcomm_ptcinit) */



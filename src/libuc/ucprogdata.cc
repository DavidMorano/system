/* ucprogdata SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* interface components for UNIX® library-3c */
/* set or get some program (process) data */
/* version %I% last-modified %G% */


/* revision history:

	= 2004-11-22, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 2004 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	NameL
	ucprogdata

	Description:
	We get (or set) some program (process) data.

	Synopsis:
	int ucprogdataget(char *rbuf,int rlen,int spec) noex

	Arguments:
	rbuf		buffer to receive the requested cluster name
	rlen		length of supplied buffer
	spec		integer specifying what data to get

	Returns:
	>=0		string length of cluster name
	SR_OK		if OK
	SR_NOTFOUND	if could not get something needed for correct operation
	SR_ISDIR	database file was a directory (admin error)
	<0		some other error

	Design note:

	Q. Is this (mess) multi-thread safe?
	A. Duh!

	Q. Does this need to be so complicated?
	A. Yes.

	Q. Was the amount of complication warranted by the need?
	A. Looking at it now ... maybe not.

	Q. Were there any alternatives?
	A. Yes; the predecessor to this present implementation was an 
	   implementation that was quite simple, but it had a lot of static
	   memory storage.  It was the desire to eliminate the static memory
	   storage that led to this present implementation.

	Q. Are there ways to clean this up further?
	A. Probably, but it looks I have already done more to this simple
	   function than may have been ever warranted to begin with!

	Q. Did these subroutines have to be Async-Signal-Safe?
	A. Not really.

	Q. Then why did you do it?
	A. The system-call |uname(2)| is Async-Signal-Safe.  Since these
	   subroutines sort of look like |uname(2)| (of a sort), I thought 
	   it was a good idea.

	Q. Was it really a good idea?
	A. I guess not.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<csignal>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* <- |strlen(3c)| */
#include	<usystem.h>
#include	<sigblocker.h>
#include	<ptm.h>
#include	<ptc.h>
#include	<varray.h>
#include	<strwcpy.h>
#include	<sncpyxw.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"ucprogdata.h"


/* local defines */

#define	UCPROGDATA	struct ucprogdata_head
#define	UCPROGDATA_ENT	struct ucprogdata_e

#define	PD		UCPROGDATA

#define	TO_TTL		(2*3600)	/* two hours */


/* imported namespaces */


/* local typedefs */

typedef volatile sig_atomic_t	vaflag ;


/* external subroutines */

extern "C" {
    int		ucprogdata_init() noex ;
    int		ucprogdata_fini() noex ;
}


/* external variables */


/* local structures */

struct ucprogdata_head {
	ptm		mx ;		/* data mutex */
	ptc		cv;		/* condition variable */
	varray		*ents ;
	vaflag		waiters ;
	vaflag		f_void ;
	vaflag		f_init ;
	vaflag		f_initdone ;
	vaflag		f_capture ;	/* capture flag */
} ;

struct ucprogdata_e {
	cchar		*vp ;
	time_t		et ;		/* entry-time (load-time) */
	int		vl ;
	int		ttl ;		/* time-to-live */
} ;


/* forward references */

extern "C" {
    static void	ucprogdata_atforkbefore() noex ;
    static void	ucprogdata_atforkafter() noex ;
    static void	ucprogdata_exit() noex ;
}

static int	ucprogdata_struct(PD *) noex ;
static int	ucprogdata_begin(PD *) noex ;
static int	ucprogdata_end(PD *) noex ;
static int	ucprogdata_entfins(PD *) noex ;
static int	ucprogdata_capbegin(PD *,int) noex ;
static int	ucprogdata_capend(PD *) noex ;
static int	ucprogdata_seter(PD *,int,cchar *,int,int) noex ;
static int	ucprogdata_geter(PD *,int,char *,int) noex ;

static int entry_start(UCPROGDATA_ENT *,cchar *,int,int) noex ;
static int entry_reload(UCPROGDATA_ENT *,cchar *,int,int) noex ;
static int entry_finish(UCPROGDATA_ENT *) noex ;


/* local variables */

static UCPROGDATA	ucprogdata_data ;


/* exported subroutines */

int ucprogdata_init() noex {
	UCPROGDATA	*uip = &ucprogdata_data ;
	int		rs = SR_NXIO ;
	int		f = false ;
	if (! uip->f_void) {
	    if (! uip->f_init) {
	        uip->f_init = true ;
	        if ((rs = ptm_create(&uip->mx,nullptr)) >= 0) {
	            if ((rs = ptc_create(&uip->cv,nullptr)) >= 0) {
	                void_f	b = ucprogdata_atforkbefore ;
	                void_f	a = ucprogdata_atforkafter ;
	                if ((rs = uc_atfork(b,a,a)) >= 0) {
	                    if ((rs = uc_atexit(ucprogdata_exit)) >= 0) {
	                        uip->f_initdone = true ;
	                        f = true ;
	                    }
	                    if (rs < 0) {
	                        uc_atforkexpunge(b,a,a) ;
			    }
	                } /* end if (uc_atfork) */
	                if (rs < 0) {
	                    ptc_destroy(&uip->cv) ;
			}
	            } /* end if (ptc_create) */
	            if (rs < 0) {
	                ptm_destroy(&uip->mx) ;
		    }
	        } /* end if (ptm_create) */
	        if (rs < 0) {
	            uip->f_init = false ;
		}
	    } else {
	        while ((rs >= 0) && uip->f_init && (! uip->f_initdone)) {
	            rs = msleep(1) ;
	            if (rs == SR_INTR) rs = SR_OK ;
	        }
	        if ((rs >= 0) && (! uip->f_init)) rs = SR_LOCKLOST ;
	    }
	} /* end if (not-void) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (ucprogdata_init) */

int ucprogdata_fini() noex {
	UCPROGDATA	*uip = &ucprogdata_data ;
	int		rs = SR_OK ;
	int		rs1 ;
	if (uip->f_initdone && (! uip->f_void)) {
	    uip->f_void = true ;
	    {
	        rs1 = ucprogdata_end(uip) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
	        void_f	b = ucprogdata_atforkbefore ;
	        void_f	a = ucprogdata_atforkafter ;
	        rs1 = uc_atforkexpunge(b,a,a) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = ptc_destroy(&uip->cv) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = ptm_destroy(&uip->mx) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    uip->f_init = false ;
	    uip->f_initdone = false ;
	} /* end if (was initialized) */
	return rs ;
}
/* end subroutine (ucprogdata_fini) */
 
int ucprogdata_set(int di,cchar *cbuf,int clen,int ttl) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		rc = 0 ;
	if (cbuf) {
	    rs = SR_INVALID ;
	    if ((di >= 0) && cbuf[0]) {
	        if (ttl < 0) ttl = TO_TTL ;
	        if (sigblocker b ; (rs = b.start) >= 0) {
	            if ((rs = ucprogdata_init()) >= 0) {
		        UCPROGDATA	*uip = &ucprogdata_data ;
		        if ((rs = ucprogdata_capbegin(uip,-1)) >= 0) {
		            if ((rs = ucprogdata_struct(uip)) >= 0) {
			        rs = ucprogdata_seter(uip,di,cbuf,clen,ttl) ;
				rc = rs ;
		            } /* end if (ucprogdata_struct) */
		            rs1 = ucprogdata_capend(uip) ;
		            if (rs >= 0) rs = rs1 ;
		        } /* end if (ucprogdata_cap) */
	            } /* end if (ucprogdata_init) */
	            rs1 = b.finish ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (sigblock) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? rc : rs ;
}
/* end subroutine (ucprogdata_set) */

int ucprogdata_get(int di,char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ;
	if (rbuf) {
	    rs = SR_INVALID ;
	    rbuf[0] = '\0' ;
	    if (di >= 0) {
	        if (sigblocker b ; (rs = b.start) >= 0) {
	            if ((rs = ucprogdata_init()) >= 0) {
	                UCPROGDATA	*uip = &ucprogdata_data ;
	                if ((rs = ucprogdata_capbegin(uip,-1)) >= 0) {
		            if ((rs = ucprogdata_struct(uip)) >= 0) {
	                        rs = ucprogdata_geter(uip,di,rbuf,rlen) ;
			        len = rs ;
		            } /* end if (ucprogdata_struct) */
	                    rs1 = ucprogdata_capend(uip) ;
	                    if (rs >= 0) rs = rs1 ;
		        } /* end if (ucprogdata_cap) */
	            } /* end if (ucprogdata_init) */
	            rs1 = b.finish ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (sigblock) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (ucprogdata_get) */


/* local subroutines */

static int ucprogdata_struct(PD *uip) noex {
	int		rs = SR_OK ;
	if (uip->ents == nullptr) {
	    rs = ucprogdata_begin(uip) ;
	}
	return rs ;
}
/* end subroutine (ucprogdata_struct) */

static int ucprogdata_begin(PD *uip) noex {
	int		rs = SR_OK ;
	if (uip->ents == nullptr) {
	    cint	osize = sizeof(varray) ;
	    void	*p ;
	    if ((rs = uc_libmalloc(osize,&p)) >= 0) {
	        cint		esize = sizeof(UCPROGDATA_ENT) ;
	        cint		n = 4 ;
	        varray		*ents = (varray *) p ;
	        if ((rs = varray_start(ents,esize,n)) >= 0) {
	            uip->ents = ents ;
		}
	        if (rs < 0)
	            uc_libfree(p) ;
	    } /* end if (memory-allocation) */
	} /* end if (needed initialization) */
	return rs ;
}
/* end subroutine (ucprogdata_begin) */

static int ucprogdata_end(PD *uip) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (uip->ents) {
	    {
	        rs1 = ucprogdata_entfins(uip) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        varray	*ents = (varray *) uip->ents ;
	        rs1 = varray_finish(ents) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = uc_libfree(uip->ents) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    uip->ents = nullptr ;
	}
	return rs ;
}
/* end subroutine (ucprogdata_end) */

static int ucprogdata_entfins(PD *uip) noex {
	varray		*vap = (varray *) uip->ents ;
	UCPROGDATA_ENT	*ep ;
	int		rs = SR_OK ;
	int		rs1 ;
	for (int i = 0 ; varray_enum(vap,i,&ep) >= 0 ; i += 1) { 
	    if (ep != nullptr) {
	        rs1 = entry_finish(ep) ;
		if (rs >= 0) rs = rs1 ;
	    }
	} /* end for */
	return rs ;
}
/* end subroutine (ucprogdata_entfins) */

static int ucprogdata_capbegin(PD *uip,int to) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = ptm_lockto(&uip->mx,to)) >= 0) {
	    uip->waiters += 1 ;
	    while ((rs >= 0) && uip->f_capture) { /* busy */
	        rs = ptc_waiter(&uip->cv,&uip->mx,to) ;
	    } /* end while */
	    if (rs >= 0) {
	        uip->f_capture = true ;
	    }
	    uip->waiters -= 1 ;
	    rs1 = ptm_unlock(&uip->mx) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (ptm) */
	return rs ;
}
/* end subroutine (ucprogdata_capbegin) */

static int ucprogdata_capend(PD *uip) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = ptm_lock(&uip->mx)) >= 0) {
	    uip->f_capture = false ;
	    if (uip->waiters > 0) {
	        rs = ptc_signal(&uip->cv) ;
	    }
	    rs1 = ptm_unlock(&uip->mx) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (ptm) */
	return rs ;
}
/* end subroutine (ucprogdata_capend) */

static int ucprogdata_seter(PD *uip,int di,cchar *cbuf,int clen,int ttl) noex {
	varray		*vap = (varray *) uip->ents ;
	UCPROGDATA_ENT	*ep ;
	int		rs ;
	if ((rs = varray_acc(vap,di,&ep)) > 0) {
	    rs = entry_reload(ep,cbuf,clen,ttl) ;
	} else if (rs == SR_OK) {
	    if ((rs = varray_mk(vap,di,&ep)) >= 0) {
		rs = entry_start(ep,cbuf,clen,ttl) ;
	    } /* end if (varray_mk) */
	} /* end if (array access) */
	return rs ;
}
/* end subroutine (ucprogdata_seter) */

static int ucprogdata_geter(PD *uip,int di,char *rbuf,int rlen) noex {
	varray		*vap = (varray *) uip->ents ;
	UCPROGDATA_ENT	*ep ;
	int		rs ;
	int		len = 0 ;
	if ((rs = varray_acc(vap,di,&ep)) > 0) {
	    custime	dt = getustime ;
	    if ((ep->et > 0) && ((dt-ep->et) < ep->ttl)) {
		rs = sncpy1w(rbuf,rlen,ep->vp,ep->vl) ;
		len = rs ;
	    }
	}
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (ucprogdata_geter) */

static void ucprogdata_atforkbefore() noex {
	UCPROGDATA	*uip = &ucprogdata_data ;
	ucprogdata_capbegin(uip,-1) ;
}
/* end subroutine (ucprogdata_atforkbefore) */

static void ucprogdata_atforkafter() noex {
	UCPROGDATA	*uip = &ucprogdata_data ;
	ucprogdata_capend(uip) ;
}
/* end subroutine (ucprogdata_atforkafter) */

static void ucprogdata_exit() noex {
	if (cint rs = ucprogdata_fini() ; rs < 0) {
	    ulogerror("ucprogdata",rs,"exit-fini") ;
	}
}
/* end subroutine (ucprogdata_exit) */


static int entry_start(UCPROGDATA_ENT *ep,cchar *vp,int vl,int ttl) noex {
	custime		dt = getustime ;
	int		rs ;
	if (vl < 0) vl = strlen(vp) ;
	if (char *bp{} ; (rs = uc_libmalloc((vl+1),&bp)) >= 0) {
	    ep->vp = bp ;
	    ep->vl = vl ;
	    strwcpy(bp,vp,vl) ;
	    ep->ttl = ttl ;
	    ep->et = dt ;
	} /* end if (m-a) */
	return rs ;
}
/* end subroutine (entry_start) */

static int entry_finish(UCPROGDATA_ENT *ep) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (ep->vp) {
	    rs1 = uc_libfree(ep->vp) ;
	    if (rs >= 0) rs = rs1 ;
	    ep->vp = nullptr ;
	}
	return rs ;
}
/* end subroutine (entry_finish) */

static int entry_reload(UCPROGDATA_ENT *ep,cchar *vp,int vl,int ttl) noex {
	int		rs = SR_OK ;
	if (ep->vp != nullptr) {
	    rs = uc_libfree(ep->vp) ;
	    ep->vp = nullptr ;
	}
	if (rs >= 0) {
	    rs = entry_start(ep,vp,vl,ttl) ;
	}
	return rs ;
}
/* end subroutine (entry_reload) */



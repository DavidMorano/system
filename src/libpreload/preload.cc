/* preload SUPPORT */
/* lang=C++20 */

/* set or get some program (process) data */
/* version %I% last-modified %G% */


/* revision history:

	= 2004-11-22, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 2004 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	We serve out some (meant to be) preloaded subroutines.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<atomic>
#include	<usystem.h>
#include	<sigblocker.h>
#include	<ptm.h>
#include	<ptc.h>
#include	<varray.h>
#include	<localmisc.h>

#include	"preload.h"


/* local defines */

#define	PRELOAD		struct preload_head
#define	PRELOAD_ENT	struct preload_entry

#define	TO_CAP		5		/* 5 seconds */
#define	TO_TTL		(2*3600)	/* two hours */


/* imported namespaces */


/* local typedefs */


/* external subroutines */

extern "C" {
    int		preload_init() noex ;
    int		preload_fini() noex ;
    int		preload_set(int,cchar *,int,int) noex ;
    int		preload_get(int,char *,int) noex ;
}


/* external variables */


/* local structures */

struct preload_head {
	ptm		m ;		/* data mutex */
	ptc		c ;		/* condition variable */
	varray		*ents ;
	atomic_int	waiters ;
	volatile int	fvoid ;
	volatile int	f_init ;
	volatile int	f_initdone ;
	volatile int	f_capture ;	/* capture flag */
} ;

struct preload_entry {
	time_t		et ;		/* entry-time (load-time) */
	cchar		*vp ;
	int		vl ;
	int		ttl ;		/* time-to-live */
} ;

typedef PRELOAD		preload ;
typedef PRLOAD_ENT	preload_ent ;


/* forward references */

extern "C" {
    static void	preload_atforkbefore() noex ;
    static void	preload_atforkafter() noex ;
}

static int	preload_struct(preload *) noex ;
static int	preload_begin(preload *) noex ;
static int	preload_end(preload *) noex ;
static int	preload_entryntfins(preload *) noex ;
static int	preload_capbegin(preload *,int) noex ;
static int	preload_capend(preload *) noex ;
static int	preload_seter(preload *,int,cchar *,int,int) noex ;
static int	preload_geter(preload *,int,char *,int) noex ;

static int	entry_start(preload_ent *,cchar *,int,int) noex ;
static int	entry_reload(preload_ent *,cchar *,int,int) noex ;
static int	entry_finish(preload_ent *) noex ;


/* local variables */

static preload	preload_data ;


/* exported variables */


/* exported subroutines */

int preload_init() noex {
	PRELOAD		*uip = &preload_data ;
	int		rs = SR_NXIO ;
	if (! uip->fvoid) {
	    rs = SR_OK ;
	    if (! uip->f_init) {
	        uip->f_init = true ;
	        if ((rs = ptm_create(&uip->m,nullptr)) >= 0) {
	            if ((rs = ptc_create(&uip->c,nullptr)) >= 0) {
	    	        void	(*b)() = preload_atforkbefore ;
	    	        void	(*a)() = preload_atforkafter ;
	                if ((rs = uc_atfork(b,a,a)) >= 0) {
	                    if ((rs = uc_atexit(preload_fini)) >= 0) {
	                        rs = 1 ;
	                        uip->f_initdone = true ;
	                    }
	                    if (rs < 0) {
	                        uc_atforkexpunge(b,a,a) ;
			    }
	                } /* end if (uc_atfork) */
	                if (rs < 0) {
	                    ptc_destroy(&uip->c) ;
		        }
	            } /* end if (ptc_create) */
	            if (rs < 0) {
	                ptm_destroy(&uip->m) ;
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
	    } /* end if */
	} /* end if (not-voided) */
	return rs ;
}
/* end subroutine (preload_init) */

int preload_fini() noex {
	PRELOAD		*uip = &preload_data ;
	int		rs = SR_OK ;
	int		rs1 ;
	if (uip->f_initdone && (!uip->fvoid)) {
	    uip->fvoid = true ;
	    {
	        rs1 = preload_end(uip) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
	        void_f	b = preload_atforkbefore ;
	        void_f	a = preload_atforkafter ;
	        rs1 = uc_atforkexpunge(b,a,a) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = ptc_destroy(&uip->c) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = ptm_destroy(&uip->m) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    uip->f_init = false ;
	    uip->f_initdone = false ;
	} /* end if (was initialized) */
	return rs ;
}
/* end subroutine (preload_fini) */

int preload_set(int di,cchar *cbuf,int clen,int ttl) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (cbuf) {
	    rs = SR_INVALID ;
	    if ((di >= 0) && cbuf[0]) {
	        sigblocker	b ;
	        if ((rs = b.start) >= 0) {
	            if ((rs = preload_init()) >= 0) {
		        preload		*uip = &preload_data ;
		        cint		to = TO_CAP ;
		        if ((rs = preload_capbegin(uip,to)) >= 0) {
		            if ((rs = preload_struct(uip)) >= 0) {
			        rs = preload_seter(uip,di,cbuf,clen,ttl) ;
		            } /* end if (preload_struct) */
		            rs1 = preload_capend(uip) ;
		            if (rs >= 0) rs = rs1 ;
		        } /* end if (preload_cap) */
	            } /* end if (preload_init) */
	            rs1 = b.finish ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (sigblocker) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (preload_set) */

int preload_get(int di,char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ;
	if (rbuf) {
	    rs = SR_INVALID ;
	    if (di >= 0) {
	        sigblocker	b ;
	        rbuf[0] = '\0' ;
	        if ((rs = b.start) >= 0) {
	            if ((rs = preload_init()) >= 0) {
	                preload		*uip = &preload_data ;
	                cint		to = TO_CAP ;
	                if ((rs = preload_capbegin(uip,to)) >= 0) {
		            if (uip->ents != nullptr) {
	                        rs = preload_geter(uip,di,rbuf,rlen) ;
			        len = rs ;
		            } /* end if (entries-present) */
	                    rs1 = preload_capend(uip) ;
	                    if (rs >= 0) rs = rs1 ;
		        } /* end if (preload_cap) */
	            } /* end if (preload_init) */
	            rs1 = b.finish ;
	        } /* end if (sigblock) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (preload_get) */


/* local subroutines */

static int preload_struct(preload *uip) noex {
	int		rs = SR_OK ;
	if (uip->ents == nullptr) {
	    rs = preload_begin(uip) ;
	}
	return rs ;
}
/* end subroutine (preload_struct) */

static int preload_begin(preload *uip) noex {
	int		rs = SR_OK ;
	if (uip->ents == nullptr) {
	    cint	osize = sizeof(varray) ;
	    void	*p{} ;
	    if ((rs = lm_mall(osize,&p)) >= 0) {
	        varray		*ents = (varray *) p ;
	        cint		esize = sizeof(preload_ent) ;
	        cint		n = 4 ;
	        if ((rs = varray_start(ents,esize,n)) >= 0) {
	            uip->ents = ents ;
		}
	        if (rs < 0) {
	            lm_free(p) ;
		}
	    } /* end if (memory-allocation) */
	} /* end if (needed initialization) */
	return rs ;
}
/* end subroutine (preload_begin) */

static int preload_end(preload *uip) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (uip->ents != nullptr) {
	    {
	        rs1 = preload_entryntfins(uip) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        varray	*ents = (varray *) uip->ents ;
	        rs1 = varray_finish(ents) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = lm_free(uip->ents) ;
	        if (rs >= 0) rs = rs1 ;
	        uip->ents = nullptr ;
	    }
	}
	return rs ;
}
/* end subroutine (preload_end) */

static int preload_entryntfins(preload *uip) noex {
	varray		*vap = (varray *) uip->ents ;
	preload_ent	*ep ;
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
/* end subroutine (preload_entryntfins) */

static int preload_capbegin(preload *uip,int to) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = ptm_lockto(&uip->m,to)) >= 0) {
	    uip->waiters += 1 ;
	    while ((rs >= 0) && uip->f_capture) { /* busy */
	        rs = ptc_waiter(&uip->c,&uip->m,to) ;
	    } /* end while */
	    if (rs >= 0) {
	        uip->f_capture = true ;
	    }
	    uip->waiters -= 1 ;
	    rs1 = ptm_unlock(&uip->m) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (ptm) */
	return rs ;
}
/* end subroutine (preload_capbegin) */

static int preload_capend(preload *uip) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = ptm_lock(&uip->m)) >= 0) {
	    uip->f_capture = false ;
	    if (uip->waiters > 0) {
	        rs = ptc_signal(&uip->c) ;
	    }
	    rs1 = ptm_unlock(&uip->m) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (ptm) */
	return rs ;
}
/* end subroutine (preload_capend) */

static int preload_seter(preload *uip,int di,cc *cbuf,int clen,int ttl) noex {
	varray		*vap = (varray *) uip->ents ;
	preload_ent	*ep ;
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
/* end subroutine (preload_seter) */

static int preload_geter(preload *uip,int di,char *rbuf,int rlen) noex {
	varray		*vap = (varray *) uip->ents ;
	preload_ent	*ep ;
	int		rs ;
	int		len = 0 ;
	if ((rs = varray_acc(vap,di,&ep)) > 0) {
	    custime	dt = time(nullptr) ;
	    if ((ep->et > 0) && ((dt-ep->et) < ep->ttl)) {
		rs = sncpy1w(rbuf,rlen,ep->vp,ep->vl) ;
		len = rs ;
	    }
	}
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (preload_geter) */

static void preload_atforkbefore() noex {
	preload		*uip = &preload_data ;
	preload_capbegin(uip,-1) ;
}
/* end subroutine (preload_atforkbefore) */

static void preload_atforkafter() noex {
	preload		*uip = &preload_data ;
	preload_capend(uip) ;
}
/* end subroutine (preload_atforkafter) */

static int entry_start(preload_ent *ep,cchar *vp,int vl,int ttl) noex {
	custime		dt = time(nullptr) ;
	int		rs ;
	char		*bp{} ;
	if (vl < 0) vl = strlen(vp) ;
	if ((rs = lm_mall((vl+1),&bp)) >= 0) {
	    ep->vp = bp ;
	    ep->vl = vl ;
	    strwcpy(bp,vp,vl) ;
	    ep->ttl = ttl ;
	    ep->et = dt ;
	} /* end if (m-a) */
	return rs ;
}
/* end subroutine (entry_start) */

static int entry_finish(preload_ent *ep) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (ep->vp != nullptr) {
	    rs1 = lm_free(ep->vp) ;
	    if (rs >= 0) rs = rs1 ;
	    ep->vp = nullptr ;
	}
	return rs ;
}
/* end subroutine (entry_finish) */

static int entry_reload(preload_ent *ep,cchar *vp,int vl,int ttl) noex {
	int		rs = SR_OK ;
	if (ep->vp != nullptr) {
	    rs = lm_free(ep->vp) ;
	    ep->vp = nullptr ;
	}
	if (rs >= 0) {
	    rs = entry_start(ep,vp,vl,ttl) ;
	}
	return rs ;
}
/* end subroutine (entry_reload) */



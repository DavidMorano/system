/* ucprogdata SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

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
	<0		error (system-return)

	Design note:

	Q. Is this (mess) multi-thread safe?
	A. Duh!

	Q. Does this need to be so complicated?
	A. Yes.

	Q. Was the amount of complication warranted by the need?
	A. Looking at it now ... maybe not.

	Q. Were there any alternatives?
	A. Yes; the predecessor to this present implementation was an 
	   implementation that was quite simple, but it had a lot
	   of static memory storage.  It was the desire to eliminate
	   the static memory storage that led to this present
	   implementation.

	Q. Are there ways to clean this up further?
	A. Probably, but it looks I have already done more to this 
	   simple function than may have been ever warranted to
	   begin with!

	Q. Did these subroutines have to be Async-Signal-Safe?
	A. Not really.

	Q. Then why did you do it?
	A. The system-call |uname(2)| is Async-Signal-Safe.  Since 
	   these subroutines sort of look like |uname(2)| (of a
	   sort), I thought it was a good idea.

	Q. Was it really a good idea?
	A. I guess not.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<ctime>			/* CSTD */
#include	<csignal>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<sigblocker.h>		/* LIBU */
#include	<ptm.h>			/* LIBU */
#include	<ptc.h>			/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<ucatexit.h>		/* LIBUC */
#include	<ucatfork.h>		/* LIBUC */
#include	<varray.h>		/* LIBUC */
#include	<strwcpy.h>		/* LIBUC */
#include	<sncpyxw.h>		/* LIBUC */
#include	<isnot.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"ucprogdata.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |getlenstr(3u)| */

/* local defines */

#define	UCPD		ucprogdata_head
#define	UCPD_ENT	ucprogdata_entry

#define	TO_TTL		(2*3600)	/* two hours */


/* imported namespaces */

using libuc::libmem ;			/*  variable */


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
	ptc		cn ;		/* condition variable */
	varray		*ents ;
	vaflag		waiters ;
	vaflag		f_void ;
	vaflag		f_init ;
	vaflag		f_initdone ;
	vaflag		f_capture ;	/* capture flag */
} ; /* end struct */

struct ucprogdata_entry {
	cchar		*vp ;
	time_t		et ;		/* entry-time (load-time) */
	int		vl ;
	int		ttl ;		/* time-to-live */
} ; /* end struct */


/* forward references */

extern "C" {
    local void	ucprogdata_atforkbefore() noex ;
    local void	ucprogdata_atforkafter() noex ;
    local void	ucprogdata_exit() noex ;
}

local int	ucprogdata_struct	(UCPD *) noex ;
local int	ucprogdata_begin	(UCPD *) noex ;
local int	ucprogdata_end		(UCPD *) noex ;
local int	ucprogdata_entfins	(UCPD *) noex ;
local int	ucprogdata_capbegin	(UCPD *,int) noex ;
local int	ucprogdata_capend	(UCPD *) noex ;
local int	ucprogdata_seter	(UCPD *,int,cchar *,int,int) noex ;
local int	ucprogdata_geter	(UCPD *,int,char *,int) noex ;

local int entry_start	(UCPD_ENT *,cchar *,int,int) noex ;
local int entry_reload	(UCPD_ENT *,cchar *,int,int) noex ;
local int entry_finish	(UCPD_ENT *) noex ;


/* local variables */

static UCPD	ucprogdata_data ;


/* exported variables */


/* exported subroutines */

int ucprogdata_init() noex {
	UCPD		*uip = &ucprogdata_data ;
	int		rs = SR_NXIO ;
	int		f = false ;
	if (! uip->f_void) {
	    if (! uip->f_init) {
	        ptm *mxp = &uip->mx ;
	        uip->f_init = true ;
	        if ((rs = mxp->create) >= 0) {
		    ptc *cnp = &uip->cn ;
	            if ((rs = cnp->create) >= 0) {
	                void_f	b = ucprogdata_atforkbefore ;
	                void_f	a = ucprogdata_atforkafter ;
	                if ((rs = uc_atforkrec(b,a,a)) >= 0) {
	                    if ((rs = uc_atexit(ucprogdata_exit)) >= 0) {
	                        uip->f_initdone = true ;
	                        f = true ;
	                    }
	                    if (rs < 0) {
	                        uc_atforkexp(b,a,a) ;
			    } /* end if (error) */
	                } /* end if (uc_atfork) */
	                if (rs < 0) {
	                    cnp->destroy() ;
			} /* end if (error) */
	            } /* end if (ptc_create) */
	            if (rs < 0) {
	                mxp->destroy() ;
		    } /* end if (error) */
	        } /* end if (ptm_create) */
	        if (rs < 0) {
	            uip->f_init = false ;
		} /* end if (error) */
	    } else {
	        while ((rs >= 0) && uip->f_init && (! uip->f_initdone)) {
	            rs = msleep(1) ;
	            if (rs == SR_INTR) rs = SR_OK ;
	        }
	        if ((rs >= 0) && (! uip->f_init)) rs = SR_LOCKFAIL ;
	    } /* end if */
	} /* end if (not-void) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (ucprogdata_init) */

int ucprogdata_fini() noex {
	UCPD		*uip = &ucprogdata_data ;
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
	        rs1 = uc_atforkexp(b,a,a) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
		ptc *cnp = &uip->cn ;
	        rs1 = cnp->destroy ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
	        ptm *mxp = &uip->mx ;
	        rs1 = mxp->destroy ;
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
		        UCPD	*uip = &ucprogdata_data ;
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
	                UCPD	*uip = &ucprogdata_data ;
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

local int ucprogdata_struct(UCPD *uip) noex {
	int		rs = SR_OK ;
	if (uip->ents == nullptr) {
	    rs = ucprogdata_begin(uip) ;
	}
	return rs ;
} /* end subroutine (ucprogdata_struct) */

local int ucprogdata_begin(UCPD *uip) noex {
	int		rs = SR_OK ;
	if (uip->ents == nullptr) {
	    cint	osz = szof(varray) ;
	    if (void *vp ; (rs = libmem.mall(osz,&vp)) >= 0) {
	        cint	esz = szof(UCPD_ENT) ;
	        cint	en = 4 ;
	        varray	*ents = (varray *) vp ;
	        if ((rs = varray_start(ents,esz,en)) >= 0) {
	            uip->ents = ents ;
		} /* end if (varray_start) */
	        if (rs < 0) {
	            libmem.free(vp) ;
		} /* end if (error) */
	    } /* end if (memory-acquire) */
	} /* end if (needed initialization) */
	return rs ;
} /* end subroutine (ucprogdata_begin) */

local int ucprogdata_end(UCPD *uip) noex {
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
	        rs1 = libmem.free(uip->ents) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (memory-release) */
	    uip->ents = nullptr ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (ucprogdata_end) */

local int ucprogdata_entfins(UCPD *uip) noex {
	varray		*vap = (varray *) uip->ents ;
	int		rs = SR_OK ;
	int		rs1 ;
	UCPD_ENT	*ep ;
	for (int i = 0 ; varray_enumer(vap,i,&ep) >= 0 ; i += 1) { 
	    if (ep) {
	        rs1 = entry_finish(ep) ;
		if (rs >= 0) rs = rs1 ;
	    }
	} /* end for */
	return rs ;
} /* end subroutine (ucprogdata_entfins) */

local int ucprogdata_capbegin(UCPD *uip,int to) noex {
	int		rs ;
	int		rs1 ;
	ptm *mxp = &uip->mx ;
	if ((rs = mxp->lockbegin(to)) >= 0) {
	    ptc *cnp = &uip->cn ;
	    {
	        uip->waiters += 1 ;
	        while ((rs >= 0) && uip->f_capture) { /* busy */
	            rs = cnp->wait(mxp,to) ;
	        } /* end while */
	        if (rs >= 0) {
	            uip->f_capture = true ;
	        }
	        uip->waiters -= 1 ;
	    } /* end block */
	    rs1 = mxp->lockend ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (ptm) */
	return rs ;
} /* end subroutine (ucprogdata_capbegin) */

local int ucprogdata_capend(UCPD *uip) noex {
	int		rs ;
	int		rs1 ;
	ptm *mxp = &uip->mx ;
	if ((rs = mxp->lockbegin) >= 0) {
	    {
		ptc *cnp = &uip->cn ;
	        uip->f_capture = false ;
	        if (uip->waiters > 0) {
	            rs = cnp->signal ;
	        }
	    } /* end block */
	    rs1 = mxp->lockend ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (ptm) */
	return rs ;
} /* end subroutine (ucprogdata_capend) */

local int ucprogdata_seter(UCPD *uip,int di,cc *cbuf,int clen,int ttl) noex {
	varray		*vap = (varray *) uip->ents ;
	UCPD_ENT	*ep ;
	int		rs ;
	if ((rs = varray_acc(vap,di,&ep)) > 0) {
	    rs = entry_reload(ep,cbuf,clen,ttl) ;
	} else if (rs == SR_OK) {
	    if ((rs = varray_mk(vap,di,&ep)) >= 0) {
		rs = entry_start(ep,cbuf,clen,ttl) ;
	    } /* end if (varray_mk) */
	} /* end if (array access) */
	return rs ;
} /* end subroutine (ucprogdata_seter) */

local int ucprogdata_geter(UCPD *uip,int di,char *rbuf,int rlen) noex {
	varray		*vap = (varray *) uip->ents ;
	int		rs ;
	int		len = 0 ;
	if (UCPD_ENT *ep ; (rs = varray_acc(vap,di,&ep)) > 0) {
	    custime	dt = getustime ;
	    if ((ep->et > 0) && ((dt-ep->et) < ep->ttl)) {
		rs = sncpy1w(rbuf,rlen,ep->vp,ep->vl) ;
		len = rs ;
	    }
	}
	return (rs >= 0) ? len : rs ;
} /* end subroutine (ucprogdata_geter) */

local void ucprogdata_atforkbefore() noex {
	UCPD	*uip = &ucprogdata_data ;
	ucprogdata_capbegin(uip,-1) ;
} /* end subroutine (ucprogdata_atforkbefore) */

local void ucprogdata_atforkafter() noex {
	UCPD	*uip = &ucprogdata_data ;
	ucprogdata_capend(uip) ;
} /* end subroutine (ucprogdata_atforkafter) */

local void ucprogdata_exit() noex {
	if (cint rs = ucprogdata_fini() ; rs < 0) {
	    ulogerror("ucprogdata",rs,"exit-fini") ;
	}
} /* end subroutine (ucprogdata_exit) */

local int entry_start(UCPD_ENT *ep,cchar *vp,int µvl,int ttl) noex {
	custime		dt = getustime ;
	int		rs = SR_FAULT ;
	if (int vl ; (vl = getlenstr(vp,µvl)) >= 0) {
	    if (char *bp ; (rs = libmem.mall((vl + 1),&bp)) >= 0) {
	        ep->vp = bp ;
	        ep->vl = vl ;
	        strwcpy(bp,vp,vl) ;
	        ep->ttl = ttl ;
	        ep->et = dt ;
	    } /* end if (m-a) */
	} /* end if (getlenstr) */
	return rs ;
} /* end subroutine (entry_start) */

local int entry_finish(UCPD_ENT *ep) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (ep->vp) {
	    void *p = voidp(ep->vp) ;
	    rs1 = libmem.free(p) ;
	    if (rs >= 0) rs = rs1 ;
	    ep->vp = nullptr ;
	} /* end if (memory-release) */
	return rs ;
} /* end subroutine (entry_finish) */

local int entry_reload(UCPD_ENT *ep,cc *vp,int vl,int ttl) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (ep->vp) {
	    void *p = voidp(ep->vp) ;
	    rs1 = libmem.free(p) ;
	    if (rs >= 0) rs = rs1 ;
	    ep->vp = nullptr ;
	} /* end if (memory-release) */
	if (rs >= 0) {
	    rs = entry_start(ep,vp,vl,ttl) ;
	}
	return rs ;
} /* end subroutine (entry_reload) */



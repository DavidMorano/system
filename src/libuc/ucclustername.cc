/* ucclustername SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface components for UNIX® library-3c */
/* set or get a cluster name given a nodename */
/* version %I% last-modified %G% */


/* revision history:

	= 2004-11-22, David A­D­ Morano
	This code was originally written.

*/

/* Copyright Â© 2004 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	ucclusternameget

	Description:
	Get a cluster name given a nodename.

	Synopsis:
	int ucclusternameget(char *rbuf,int rlen,cchar *nodename) noex

	Arguments:
	rbuf		buffer to receive the requested cluster name
	rlen		length of supplied buffer
	nodename	nodename used to find associated cluster

	Returns:
	==0		could not get a cluster name
	>0		string length of cluster name
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
	   implementation that was quite simple, but it had a lot of static
	   memory storage.  It was the desire to eliminate the static memory
	   storage that led to this present implementation.

	Q. Are there ways to clean this up further?
	A. Probably, but it looks like I have already done more to this simple
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
#include	<ctime>
#include	<csignal>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<sigblocker.h>
#include	<aflag.hh>
#include	<ptm.h>
#include	<sncpyx.h>
#include	<strwcpy.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"ucclustername.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| + |getlenstr(3u)| */

/* local defines */

#define	UCLUSTERNAME	clustername
#define	UCLUSTERNAME_A	clustername_a

#define	SI		subinfo

#define	TO_TTL		(2*3600) /* two hours */


/* imported namespaces */

using libuc::libmem ;			/* variable */


/* local typedefs */

typedef volatile sig_atomic_t	vaflag ;


/* external subroutines */

extern "C" {
    int		ucclustername_init() noex ;
    int		ucclustername_fini() noex ;
}


/* external variables */


/* local structures */

struct clustername {
	ptm		mx ;		/* data mutex */
	cchar		*nn ;		/* node-name */
	cchar		*cn ;		/* cluster-name */
	char		*a ;		/* memory-allocation */
	time_t		et ;
	int		ttl ;		/* time-to-live */
	vaflag		f_void ;
	vaflag		f_init ;
	vaflag		f_initdone ;
	vaflag		f_allocget ;
} ;

struct clustername_a {
	time_t		et ;
	cchar		*nn ;
	cchar		*cn ;
	char		*a ;		/* memory-allocation */
	int		ttl ;		/* time-to-live */
} ;

struct subinfo {
	cchar		*nn ;
	char		*rbuf ;
	time_t		dt ;
	int		rlen ;
	int		to ;
} ;


/* forward references */

extern "C" {
    static void	ucclustername_atforkbefore() noex ;
    static void	ucclustername_atforkafter() noex ;
    static void	ucclustername_exit() noex ;
}

static int	ucclustername_end(UCLUSTERNAME *) noex ;
static int	ucclustername_allocbegin(UCLUSTERNAME *,time_t,int) noex ;
static int	ucclustername_allocend(UCLUSTERNAME *,UCLUSTERNAME_A *) noex ;

static int	subinfo_start(SI *,char *,int,cchar *) noex ;
static int	subinfo_finish(SI *) noex ;
static int	subinfo_cacheget(SI *,UCLUSTERNAME *) noex ;
static int	subinfo_cacheset(SI *,UCLUSTERNAME *,int) noex ;


/* local variables */

static UCLUSTERNAME	ucclustername_data ;


/* exported variables */


/* exported subroutines */

int ucclustername_init() noex {
	UCLUSTERNAME	*uip = &ucclustername_data ;
	int		rs = SR_NXIO ;
	int		f = false ;
	if (! uip->f_void) {
	    if (! uip->f_init) {
	        uip->f_init = true ;
	        if ((rs = ptm_create(&uip->mx,nullptr)) >= 0) ylikely {
	            void_f	b = ucclustername_atforkbefore ;
	            void_f	a = ucclustername_atforkafter ;
	            if ((rs = uc_atfork(b,a,a)) >= 0) ylikely {
			cauto esub = ucclustername_exit ;
	                if ((rs = uc_atexit(esub)) >= 0) ylikely {
	                    uip->f_initdone = true ;
	                    f = true ;
	                }
	                if (rs < 0) {
	                    uc_atforkexpunge(b,a,a) ;
			}
	            } /* end if (uc_atfork) */
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
	} /* end if (not-voided) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (ucclustername_init) */

int ucclustername_fini() noex {
	UCLUSTERNAME	*uip = &ucclustername_data ;
	int		rs = SR_OK ;
	int		rs1 ;
	if (uip->f_initdone && (! uip->f_void)) {
	    uip->f_void = true ;
	    {
	        rs1 = ucclustername_end(uip) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
	        void_f	b = ucclustername_atforkbefore ;
	        void_f	a = ucclustername_atforkafter ;
	        rs1 = uc_atforkexpunge(b,a,a) ;
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
/* end subroutine (ucclustername_fini) */

int ucclustername_get(char *rbuf,int rlen,cchar *nn) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ;
	if (rbuf && nn) ylikely {
	    rs = SR_INVALID ;
	    if (nn[0]) ylikely {
	        rbuf[0] = '\0' ;
	        if (sigblocker b ; (rs = b.start) >= 0) ylikely {
	            if ((rs = ucclustername_init()) >= 0) ylikely {
	                SI si ;
	                if ((rs = subinfo_start(&si,rbuf,rlen,nn)) >= 0) {
	                    UCLUSTERNAME	*uip = &ucclustername_data ;
		            {
	                        rs = subinfo_cacheget(&si,uip) ;
	                        len = rs ;
		            }
	                    rs1 = subinfo_finish(&si) ;
	                    if (rs >= 0) rs = rs1 ;
	                } /* end if (subinfo) */
	            } /* end if (ucclustername_init) */
	            rs1 = b.finish ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (sigblock) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (ucclustername_get) */

int ucclustername_set(cchar *cbuf,int µclen,cchar *nn,int to) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		rc = 0 ;
	if (cbuf && nn) ylikely {
	    rs = SR_INVALID ;
	    if (cbuf[0] && nn[0]) ylikely {
		if (int clen ; (clen = getlenstr(cbuf,µclen)) >= 0) {
	            if (sigblocker b ; (rs = b.start) >= 0) ylikely {
	                if ((rs = ucclustername_init()) >= 0) ylikely {
	                    SI si ;
		            char *sbuf = charp(cbuf) ;
	                    if ((rs = subinfo_start(&si,sbuf,clen,nn)) >= 0) {
	                        UCLUSTERNAME *uip = &ucclustername_data ;
		                {
	                            rs = subinfo_cacheset(&si,uip,to) ;
				    rc = rs ;
		                }
	                        rs1 = subinfo_finish(&si) ;
	                        if (rs >= 0) rs = rs1 ;
	                    } /* end if (subinfo) */
	                } /* end if (ucclustername_init) */
	                rs1 = b.finish ;
		        if (rs >= 0) rs = rs1 ;
	            } /* end if (sigblock) */
		} /* end if (getlenstr) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? rc : rs ;
}
/* end subroutine (ucclustername_set) */


/* local subroutines */

static int ucclustername_end(UCLUSTERNAME *uip) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (uip->a) {
	    rs1 = libmem.free(uip->a) ;
	    if (rs >= 0) rs = rs1 ;
	    uip->a = nullptr ;
	    uip->nn = nullptr ;
	    uip->cn = nullptr ;
	}
	return rs ;
}
/* end subroutine (ucclustername_end) */

static int ucclustername_allocbegin(UCLUSTERNAME *uip,time_t dt,int ttl) noex {
	int		rs ;
	int		rs1 ;
	int		f = false ;
	if ((rs = uc_forklockbegin(-1)) >= 0) ylikely {
	    if ((rs = ptm_lock(&uip->mx)) >= 0) ylikely {
	        if ((uip->a == nullptr) || ((dt-uip->et) >= ttl)) {
	            if (! uip->f_allocget) {
	                uip->f_allocget = true ;
	                f = true ; /* indicate "got" */
	            }
	        } /* end if (need) */
	        rs1 = ptm_unlock(&uip->mx) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (mutex) */
	    rs1 = uc_forklockend() ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (forklock) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (ucclustername_allocbegin) */

static int ucclustername_allocend(UCLUSTERNAME *uip,UCLUSTERNAME_A *ap) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (ap && ap->a) ylikely {
	    if ((rs = uc_forklockbegin(-1)) >= 0) ylikely {
	        if ((rs = ptm_lock(&uip->mx)) >= 0) ylikely {
	            {
	                uip->f_allocget = false ;
	                uip->a = ap->a ;
	                uip->nn = ap->nn ;
	                uip->cn = ap->cn ;
	                uip->et = ap->et ;
	                uip->ttl = ap->ttl ;
	            }
	            rs1 = ptm_unlock(&uip->mx) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (mutex) */
	        rs1 = uc_forklockend() ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (forklock) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ucclustername_allocend) */

static void ucclustername_atforkbefore() noex {
	UCLUSTERNAME	*uip = &ucclustername_data ;
	ptm_lock(&uip->mx) ;
}
/* end subroutine (ucclustername_atforkbefore) */

static void ucclustername_atforkafter() noex {
	UCLUSTERNAME	*uip = &ucclustername_data ;
	ptm_unlock(&uip->mx) ;
}
/* end subroutine (ucclustername_atforkafter) */

static void ucclustername_exit() noex {
	if (cint rs = ucclustername_fini() ; rs < 0) {
	    ulogerror("ucclustername",rs,"exit-fini") ;
	}
}

static int subinfo_start(SI *sip,char *rbuf,int rlen,cchar *nn) noex {
    	int		rs = SR_FAULT ;
	if (sip && rbuf && nn) ylikely {
	    rs = SR_OK ;
	    sip->to = TO_TTL ;
	    sip->dt = time(nullptr) ;
	    sip->rbuf = rbuf ;
	    sip->rlen = rlen ;
	    sip->nn = nn ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (subinfo_start) */

static int subinfo_finish(SI *sip) noex {
	int		rs = SR_FAULT ;
	if (sip) {
	    rs = SR_OK ;
	}
	return rs ;
}
/* end subroutine (subinfo_finish) */

static int subinfo_cacheget(SI *sip,UCLUSTERNAME *uip) noex {
	int		rs ;
	int		rs1 ;
	int		len = 0 ;
	if ((rs = uc_forklockbegin(-1)) >= 0) ylikely {
	    if ((rs = ptm_lock(&uip->mx)) >= 0) ylikely {
	        if (uip->a != nullptr) {
	            if ((uip->et > 0) && ((sip->dt-uip->et) < uip->ttl)) {
	                if (strcmp(sip->nn,uip->nn) == 0) {
	                    rs = sncpy1(sip->rbuf,sip->rlen,uip->cn) ;
	                    len = rs ;
	                }
	            } /* end if (not timed-out) */
	        } /* end if (was loaded) */
	        rs1 = ptm_unlock(&uip->mx) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (mutex) */
	    rs1 = uc_forklockend() ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (forklock) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (subinfo_cacheget) */

static int subinfo_cacheset(SI *sip,UCLUSTERNAME *uip,int ttl) noex {
	int		rs ;
	int		rs1 ;
	int		f = false ;
	char		*aprev = nullptr ;
	if (ttl < 0) ttl = sip->to ;
	if ((rs = ucclustername_allocbegin(uip,sip->dt,ttl)) > 0) ylikely {
	    UCLUSTERNAME_A	uca{} ;
	    int			sz = 0 ;
	    f = true ;
	    sz += (lenstr(sip->nn) + 1) ;
	    sz += (lenstr(sip->rbuf,sip->rlen) + 1) ;
	    if (char *bp ; (rs = libmem.mall(sz,&bp)) >= 0) ylikely {
	        uca.a = bp ;
	        aprev = uip->a ;
	        uca.nn = bp ;
	        bp = (strwcpy(bp,sip->nn,-1) + 1) ;
	        uca.cn = bp ;
	        bp = (strwcpy(bp,sip->rbuf,sip->rlen) + 1) ;
	        uca.et = sip->dt ;
	        uca.ttl = ttl ;
	    } /* end if (m-a) */
	    rs1 = ucclustername_allocend(uip,&uca) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (alloc) */
	if ((rs >= 0) && aprev) {
	    libmem.free(aprev) ;
	}
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (subinfo_cacheset) */



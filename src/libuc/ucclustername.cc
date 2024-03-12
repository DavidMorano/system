/* ucclustername SUPPORT */
/* lang=C++20 */

/* set or get a cluster name given a nodename */
/* version %I% last-modified %G% */


/* revision history:

	= 2004-11-22, David A­D­ Morano
	This subroutine was originally written.

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
#include	<sys/types.h>
#include	<sys/param.h>
#include	<csignal>
#include	<cstdlib>
#include	<cstring>		/* <- for |strlen(3c)| */
#include	<ctime>
#include	<usystem.h>
#include	<sigblocker.h>
#include	<ptm.h>
#include	<sncpyx.h>
#include	<strwcpy.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"ucclustername.h"


/* local defines */

#define	UCLUSTERNAME	struct clustername
#define	UCLUSTERNAME_A	struct clustername_a

#define	SUBINFO		struct subinfo

#define	TO_TTL		(2*3600) /* two hours */


/* local namespaces */


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
	ptm		m ;		/* data mutex */
	time_t		et ;
	cchar		*nn ;		/* node-name */
	cchar		*cn ;		/* cluster-name */
	char		*a ;		/* memory-allocation */
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

static int	subinfo_start(SUBINFO *,char *,int,cchar *) noex ;
static int	subinfo_finish(SUBINFO *) noex ;
static int	subinfo_cacheget(SUBINFO *,UCLUSTERNAME *) noex ;
static int	subinfo_cacheset(SUBINFO *,UCLUSTERNAME *,int) noex ;


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
	        if ((rs = ptm_create(&uip->m,nullptr)) >= 0) {
	            void_f	b = ucclustername_atforkbefore ;
	            void_f	a = ucclustername_atforkafter ;
	            if ((rs = uc_atfork(b,a,a)) >= 0) {
	                if ((rs = uc_atexit(ucclustername_exit)) >= 0) {
	                    uip->f_initdone = true ;
	                    f = true ;
	                }
	                if (rs < 0) {
	                    uc_atforkexpunge(b,a,a) ;
			}
	            } /* end if (uc_atfork) */
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
	        rs1 = ptm_destroy(&uip->m) ;
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
	if (rbuf && nn) {
	    rs = SR_INVALID ;
	    if (nn[0]) {
	        sigblocker	b ;
	        rbuf[0] = '\0' ;
	        if ((rs = sigblocker_start(&b,nullptr)) >= 0) {
	            if ((rs = ucclustername_init()) >= 0) {
	                SUBINFO		si ;
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
	            rs1 = sigblocker_finish(&b) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (sigblock) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (ucclustername_get) */

int ucclustername_set(cchar *cbuf,int clen,cchar *nn,int to) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		rc = 0 ;
	if (cbuf && nn) {
	    rs = SR_INVALID ;
	    if (cbuf[0] && nn[0]) {
	        sigblocker	b ;
	        if (clen < 0) clen = strlen(cbuf) ;
	        if ((rs = sigblocker_start(&b,nullptr)) >= 0) {
	            if ((rs = ucclustername_init()) >= 0) {
	                SUBINFO		si ;
		        char		*sbuf = (char *) cbuf ;
	                if ((rs = subinfo_start(&si,sbuf,clen,nn)) >= 0) {
	                    UCLUSTERNAME	*uip = &ucclustername_data ;
		            {
	                        rs = subinfo_cacheset(&si,uip,to) ;
				rc = rs ;
		            }
	                    rs1 = subinfo_finish(&si) ;
	                    if (rs >= 0) rs = rs1 ;
	                } /* end if (subinfo) */
	            } /* end if (ucclustername_init) */
	            rs1 = sigblocker_finish(&b) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (sigblock) */
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
	    rs1 = uc_libfree(uip->a) ;
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
	if ((rs = uc_forklockbegin(-1)) >= 0) {
	    if ((rs = ptm_lock(&uip->m)) >= 0) {
	        if ((uip->a == nullptr) || ((dt-uip->et) >= ttl)) {
	            if (! uip->f_allocget) {
	                uip->f_allocget = true ;
	                f = true ; /* indicate "got" */
	            }
	        } /* end if (need) */
	        rs1 = ptm_unlock(&uip->m) ;
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
	if (ap && ap->a) {
	    if ((rs = uc_forklockbegin(-1)) >= 0) {
	        if ((rs = ptm_lock(&uip->m)) >= 0) {
	            {
	                uip->f_allocget = false ;
	                uip->a = ap->a ;
	                uip->nn = ap->nn ;
	                uip->cn = ap->cn ;
	                uip->et = ap->et ;
	                uip->ttl = ap->ttl ;
	            }
	            rs1 = ptm_unlock(&uip->m) ;
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
	ptm_lock(&uip->m) ;
}
/* end subroutine (ucclustername_atforkbefore) */

static void ucclustername_atforkafter() noex {
	UCLUSTERNAME	*uip = &ucclustername_data ;
	ptm_unlock(&uip->m) ;
}
/* end subroutine (ucclustername_atforkafter) */

static void ucclustername_exit() noex {
	int	rs = ucclustername_fini() ;
	if (rs < 0) {
	    ulogerror("ucclustername",rs,"exit-fini") ;
	}
}

static int subinfo_start(SUBINFO *sip,char *rbuf,int rlen,cchar *nn) noex {
	sip->to = TO_TTL ;
	sip->dt = time(nullptr) ;
	sip->rbuf = rbuf ;
	sip->rlen = rlen ;
	sip->nn = nn ;
	return SR_OK ;
}
/* end subroutine (subinfo_start) */

static int subinfo_finish(SUBINFO *sip) noex {
	int		rs = SR_FAULT ;
	if (sip) {
	    rs = SR_OK ;
	}
	return rs ;
}
/* end subroutine (subinfo_finish) */

static int subinfo_cacheget(SUBINFO *sip,UCLUSTERNAME *uip) noex {
	int		rs ;
	int		rs1 ;
	int		len = 0 ;
	if ((rs = uc_forklockbegin(-1)) >= 0) {
	    if ((rs = ptm_lock(&uip->m)) >= 0) {
	        if (uip->a != nullptr) {
	            if ((uip->et > 0) && ((sip->dt-uip->et) < uip->ttl)) {
	                if (strcmp(sip->nn,uip->nn) == 0) {
	                    rs = sncpy1(sip->rbuf,sip->rlen,uip->cn) ;
	                    len = rs ;
	                }
	            } /* end if (not timed-out) */
	        } /* end if (was loaded) */
	        rs1 = ptm_unlock(&uip->m) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (mutex) */
	    rs1 = uc_forklockend() ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (forklock) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (subinfo_cacheget) */

static int subinfo_cacheset(SUBINFO *sip,UCLUSTERNAME *uip,int ttl) noex {
	int		rs ;
	int		rs1 ;
	int		f = false ;
	char		*aprev = nullptr ;
	if (ttl < 0) ttl = sip->to ;
	if ((rs = ucclustername_allocbegin(uip,sip->dt,ttl)) > 0) {
	    UCLUSTERNAME_A	uca ;
	    int			size = 0 ;
	    char		*bp ;
	    f = true ;
	    memset(&uca,0,sizeof(UCLUSTERNAME_A)) ;
	    size += (strlen(sip->nn) + 1) ;
	    size += (strnlen(sip->rbuf,sip->rlen) + 1) ;
	    if ((rs = uc_libmalloc(size,&bp)) >= 0) {
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
	    uc_libfree(aprev) ;
	}
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (subinfo_cacheset) */



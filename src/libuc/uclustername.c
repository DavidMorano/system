/* uclustername SUPPORT */
/* lang=C20 */

/* set or get a cluster name given a nodename */
/* version %I% last-modified %G% */


/* revision history:

	= 2004-11-22, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 2004 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Description:
	Get a cluster name given a nodename.

	Synopsis:
	int uclusternameget(char *rbuf,int rlen,cchar *nodename) noex

	Arguments:
	rbuf		buffer to receive the requested cluster name
	rlen		length of supplied buffer
	nodename	nodename used to find associated cluster

	Returns:
	>0		string length of cluster name
	==0		could not get a cluster name
	<0		some other error (system-return)

	Design note:

	Q. Is this (mess) multi-thread safe?
	A. Duh!

	Q. Does this need to be so complicated?
	A. Yes.

	Q. Was the amount of complication warranted by the need?
	A. Looking at it now ... maybe not.

	Q. Were there any alternatives?
	A. Yes; the predicessor to this present implementation was an 
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
#include	<time.h>
#include	<stdlib.h>
#include	<string.h>
#include	<usystem.h>
#include	<usupport.h>
#include	<sigblocker.h>
#include	<ptm.h>
#include	<sncpyx.h>
#include	<strwcpy.h>
#include	<isnot.h>
#include	<localmisc.h>


/* local defines */

#define	UCLUSTERNAME	struct clustername
#define	UCLUSTERNAME_A	struct clustername_a

#define	SUBINFO		struct subinfo

#define	TO_TTL		(2*3600) /* two hours */


/* external subroutines */


/* external variables */


/* local structures */

struct clustername {
	ptm		m ;		/* data mutex */
	time_t		et ;
	cchar		*nn ;		/* node-name */
	cchar		*cn ;		/* cluster-name */
	char		*a ;		/* memory-allocation */
	int		ttl ;		/* time-to-live */
	volatile int	f_init ; 	/* race-condition, blah, blah */
	volatile int	f_initdone ;
	volatile int	f_allocget ;
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

int		uclustername_init() noex ;
int		uclustername_fini() noex ;

static void	uclustername_atforkbefore() noex ;
static void	uclustername_atforkafter() noex ;
static void	uclustername_exit() noex ;

static int	uclustername_end(UCLUSTERNAME *) noex ;
static int	uclustername_allocbegin(UCLUSTERNAME *,time_t,int) noex ;
static int	uclustername_allocend(UCLUSTERNAME *,UCLUSTERNAME_A *) noex ;

static int	subinfo_start(SUBINFO *,char *,int,cchar *) noex ;
static int	subinfo_finish(SUBINFO *) noex ;
static int	subinfo_cacheget(SUBINFO *,UCLUSTERNAME *) noex ;
static int	subinfo_cacheset(SUBINFO *,UCLUSTERNAME *,int) noex ;


/* local variables */

static UCLUSTERNAME	uclustername_data ;


/* exported subroutines */

int uclustername_init() noex {
	UCLUSTERNAME	*uip = &uclustername_data ;
	int		rs = SR_OK ;
	int		f = false ;
	if (! uip->f_init) {
	    uip->f_init = TRUE ;
	    if ((rs = ptm_create(&uip->m,NULL)) >= 0) {
	        void_f	b = uclustername_atforkbefore ;
	        void_f	a = uclustername_atforkafter ;
	        if ((rs = uc_atfork(b,a,a)) >= 0) {
	            if ((rs = uc_atexit(uclustername_exit)) >= 0) {
	                uip->f_initdone = TRUE ;
	                f = TRUE ;
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
		if (rs == SR_INTR) break ;
	    }
	    if ((rs >= 0) && (! uip->f_init)) rs = SR_LOCKLOST ;
	}
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (uclustername_init) */

int uclustername_fini() noex {
	UCLUSTERNAME	*uip = &uclustername_data ;
	int		rs = SR_OK ;
	int		rs1 ;
	if (uip->f_initdone) {
	    uip->f_initdone = false ;
	    {
	        rs1 = uclustername_end(uip) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
	        void	(*b)() = uclustername_atforkbefore ;
	        void	(*a)() = uclustername_atforkafter ;
	        rs1 = uc_atforkexpunge(b,a,a) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = ptm_destroy(&uip->m) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    uip->f_init = false ;
	} /* end if (was initialized) */
	return rs ;
}
/* end subroutine (uclustername_fini) */

int uclustername_get(char *rbuf,int rlen,cchar *nn) noex {
	sigblocker	b ;
	int		rs ;
	int		rs1 ;
	int		len = 0 ;

	if (rbuf == NULL) return SR_FAULT ;
	if (nn == NULL) return SR_FAULT ;

	if (nn[0] == '\0') return SR_INVALID ;

	rbuf[0] = '\0' ;
	if ((rs = sigblocker_start(&b,NULL)) >= 0) {
	    if ((rs = uclustername_init()) >= 0) {
	        SUBINFO		si ;
	        if ((rs = subinfo_start(&si,rbuf,rlen,nn)) >= 0) {
	            UCLUSTERNAME	*uip = &uclustername_data ;
		    {
	                rs = subinfo_cacheget(&si,uip) ;
	                len = rs ;
		    }
	            rs1 = subinfo_finish(&si) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (subinfo) */
	    } /* end if (uclustername_init) */
	    rs1 = sigblocker_finish(&b) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (sigblock) */

	return (rs >= 0) ? len : rs ;
}
/* end subroutine (uclustername_get) */

int uclustername_set(cchar *cbuf,int clen,cchar *nn,int to) noex {
	sigblocker	b ;
	int		rs ;
	int		rs1 ;

	if (cbuf == NULL) return SR_FAULT ;
	if (nn == NULL) return SR_FAULT ;

	if (cbuf[0] == '\0') return SR_INVALID ;
	if (nn[0] == '\0') return SR_INVALID ;

	if (clen < 0) clen = strlen(cbuf) ;

	if ((rs = sigblocker_start(&b,NULL)) >= 0) {
	    if ((rs = uclustername_init()) >= 0) {
	        SUBINFO		si ;
	        if ((rs = subinfo_start(&si,(char *) cbuf,clen,nn)) >= 0) {
	            UCLUSTERNAME	*uip = &uclustername_data ;
		    {
	                rs = subinfo_cacheset(&si,uip,to) ;
		    }
	            rs1 = subinfo_finish(&si) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (subinfo) */
	    } /* end if (uclustername_init) */
	    rs1 = sigblocker_finish(&b) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (sigblock) */

	return rs ;
}
/* end subroutine (uclustername_set) */


/* local subroutines */

static int uclustername_end(UCLUSTERNAME *uip) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (uip->a) {
	    rs1 = uc_libfree(uip->a) ;
	    if (rs >= 0) rs = rs1 ;
	    uip->a = NULL ;
	    uip->nn = NULL ;
	    uip->cn = NULL ;
	}
	return rs ;
}
/* end subroutine (uclustername_end) */

static int subinfo_start(SUBINFO *sip,char *rbuf,int rlen,cchar *nn) noex {
	int		rs = SR_FAULT ;
	if (sip) {
	    sip->to = TO_TTL ;
	    sip->dt = time(NULL) ;
	    sip->rbuf = rbuf ;
	    sip->rlen = rlen ;
	    sip->nn = nn ;
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
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
	        if (uip->a != NULL) {
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
	char		*aprev = NULL ;
	if (ttl < 0) ttl = sip->to ;
	if ((rs = uclustername_allocbegin(uip,sip->dt,ttl)) > 0) {
	    UCLUSTERNAME_A	uca ;
	    int			size = 0 ;
	    char		*bp ;

	    f = TRUE ;
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

	    rs1 = uclustername_allocend(uip,&uca) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (alloc) */
	if ((rs >= 0) && (aprev != NULL)) uc_libfree(aprev) ;
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (subinfo_cacheset) */

static int uclustername_allocbegin(UCLUSTERNAME *uip,time_t dt,int ttl) noex {
	int		rs ;
	int		rs1 ;
	int		f = false ;
	if ((rs = uc_forklockbegin(-1)) >= 0) {
	    if ((rs = ptm_lock(&uip->m)) >= 0) {
	        if ((uip->a == NULL) || ((dt-uip->et) >= ttl)) {
	            if (! uip->f_allocget) {
	                uip->f_allocget = TRUE ;
	                f = TRUE ; /* indicate "got" */
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
/* end subroutine (subinfo_allocbegin) */

static int uclustername_allocend(UCLUSTERNAME *uip,UCLUSTERNAME_A *ap) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if ((ap != NULL) && (ap->a != NULL)) {
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
/* end subroutine (subinfo_allocend) */

static void uclustername_atforkbefore() noex {
	UCLUSTERNAME	*uip = &uclustername_data ;
	ptm_lock(&uip->m) ;
}
/* end subroutine (uclustername_atforkbefore) */

static void uclustername_atforkafter() noex {
	UCLUSTERNAME	*uip = &uclustername_data ;
	ptm_unlock(&uip->m) ;
}
/* end subroutine (uclustername_atforkafter) */

static void uclustername_exit() noex {
	return ;
}
/* end subroutine (uclustername_exit) */



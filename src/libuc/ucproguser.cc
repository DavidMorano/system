/* ucproguser SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* interface components for UNIX® library-3c */
/* get or set a cached username given a UID */
/* version %I% last-modified %G% */


/* revision history:

	= 2004-11-22, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 2004 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	ucproguser

	Description:
	Set (as if for a cache) and get a username given a UID.

	Synopsis:
	int ucproguser_nameset(cchar *cbuf,int clen,uid_t uid,int ttl) noex
	int ucproguser_nameget(char *rbuf,int rlen,uid_t uid) noex

	Arguments:
	rbuf		buffer to receive the requested username
	rlen		length of supplied buffer
	uid		UID of user to get name for
	ttl		time-to-live

	Returns:
	==0		could not get a name
	>0		string length of found username
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
#include	<ctime>
#include	<csignal>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<sigblocker.h>
#include	<ptm.h>
#include	<sncpyx.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"ucproguser.h"


/* local defines */

#define	UCPU		ucproguser

#define	TO_TTL		(2*3600) /* two hours */


/* imported namespaces */

using libuc::libmem ;			/* variable */


/* local typedefs */

typedef volatile sig_atomic_t	vaflag ;


/* external namespaces */


/* external subroutines */

extern "C" {
    int		ucproguser_init() noex ;
    int		ucproguser_fini() noex ;
}


/* external variables */


/* local structures */

namespace {
    struct ucproguser {
	ptm		mx ;		/* data mutex */
	time_t		et ;
	cchar		*username ;	/* memory-allocated */
	cchar		*userhome ;	/* memory-allocated */
	uid_t		uid ;
	int		ttl ;		/* time-to-live */
	vaflag		f_void ;
	vaflag		f_init ;
	vaflag		f_initdone ;
	void dtor() noex ;
	destruct ucproguser() {
	    dtor() ;
	} ;
    } ; /* end struct (ucproguser) */
} /* end namespace */


/* forward references */


extern "C" {
    local void	ucproguser_atforkbefore() noex ;
    local void	ucproguser_atforkafter() noex ;
    local void	ucproguser_exit() noex ;
}

static int	ucproguser_namer(UCPU *,cchar *,int) noex ;
static int	ucproguser_namealloc(UCPU *,cchar *,int) noex ;
static int	ucproguser_namefree(UCPU *) noex ;
static int	ucproguser_nameend(UCPU *) noex ;


/* local variables */

static UCPU	ucproguser_data ;


/* exported variables */


/* exported subroutines */

int ucproguser_init() noex {
	UCPU		*uip = &ucproguser_data ;
	int		rs = SR_NXIO ;
	int		f = false ;
	if (! uip->f_void) {
	    rs = SR_OK ;
	    if (! uip->f_init) {
	        uip->f_init = true ;
	        if ((rs = ptm_create(&uip->mx,nullptr)) >= 0) {
	            void_f	b = ucproguser_atforkbefore ;
	            void_f	a = ucproguser_atforkafter ;
	            if ((rs = uc_atfork(b,a,a)) >= 0) {
	                if ((rs = uc_atexit(ucproguser_exit)) >= 0) {
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
	    } else if (! uip->f_initdone) {
	        while ((rs >= 0) && uip->f_init && (! uip->f_initdone)) {
	            rs = msleep(1) ;
	            if (rs == SR_INTR) rs = SR_OK ;
	        }
	        if ((rs >= 0) && (! uip->f_init)) rs = SR_LOCKLOST ;
	    } /* end if */
	} /* end if (not voided) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (ucproguser_init) */

int ucproguser_fini() noex {
	UCPU		*uip = &ucproguser_data ;
	int		rs = SR_OK ;
	int		rs1 ;
	if (uip->f_initdone && (! uip->f_void)) {
	    uip->f_void = true ;
	    {
	        rs1 = ucproguser_nameend(uip) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
	        void_f	b = ucproguser_atforkbefore ;
	        void_f	a = ucproguser_atforkafter ;
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
/* end subroutine (ucproguser_fini) */

int ucproguser_nameset(cchar *cbuf,int clen,uid_t uid,int ttl) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		ul = 0 ;
	if (cbuf) {
	    rs = SR_INVALID ;
	    if (cbuf[0]) {
	        if (uid == 0) uid = getuid() ;
	        if (ttl < 0) ttl = TO_TTL ;
		if (sigblocker b ; (rs = b.start) >= 0) {
	            if ((rs = ucproguser_init()) >= 0) {
			typedef int (*un_f)(UCPU *,cchar *,int) noex ;
	                UCPU	*uip = &ucproguser_data ;
	                if ((rs = uc_forklockbegin(-1)) >= 0) {
	                    if ((rs = ptm_lock(&uip->mx)) >= 0) {
				un_f	unr = ucproguser_namer ;
				if ((rs = unr(uip,cbuf,clen)) >= 0) {
				    ul = rs ;
				    uip->et = time(nullptr) ;
				    uip->uid = uid ;
				    uip->ttl = ttl ;
				}
	                        rs1 = ptm_unlock(&uip->mx) ;
	                        if (rs >= 0) rs = rs1 ;
	                    } /* end if (mutex) */
	                    rs1 = uc_forklockend() ;
	                    if (rs >= 0) rs = rs1 ;
	                } /* end if (forklock) */
	            } /* end if (ucproguser_init) */
	            rs1 = b.finish ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (sigblock) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? ul : rs ;
}
/* end subroutine (ucproguser_set) */

int ucproguser_nameget(char *rbuf,int rlen,uid_t uid) noex {
	UCPU		*uip = &ucproguser_data ;
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ;
	if (rbuf) {
	    if (uid == 0) uid = getuid() ;
	    rbuf[0] = '\0' ;
	    if (uip->username[0] != '\0') {
	        if (sigblocker b ; (rs = b.start) >= 0) {
	            if ((rs = ucproguser_init()) >= 0) {
	                if ((rs = uc_forklockbegin(-1)) >= 0) {
	                    if ((rs = ptm_lock(&uip->mx)) >= 0) {
	                        if (uip->username[0] != '\0') {
	                            if (uip->et > 0) {
		                        custime		dt = time(nullptr) ;
				        if ((dt-uip->et) < uip->ttl) {
	                                    if (uip->uid == uid) {
					        cchar	*un = uip->username ;
	                                        rs = sncpy1(rbuf,rlen,un) ;
	                                        len = rs ;
	                                    }
	                                } /* end if (not timed-out) */
				    } /* end if (possible) */
			        } /* end if (nul-terminated) */
	                        rs1 = ptm_unlock(&uip->mx) ;
	                        if (rs >= 0) rs = rs1 ;
	                    } /* end if (mutex) */
	 		    rs1 = uc_forklockend() ;
	                    if (rs >= 0) rs = rs1 ;
	                } /* end if (forklock) */
	            } /* end if (ucproguser_init) */
	            rs1 = b.finish ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (sigblock) */
	    } /* end if (possible match) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (ucproguser_nameget) */


/* local subroutines */

static int ucproguser_namer(UCPU *uip,cchar *cbuf,int clen) noex {
	int		rs ;
	if ((rs = ucproguser_namefree(uip)) >= 0) {
	    rs = ucproguser_namealloc(uip,cbuf,clen) ;
	}
	return rs ;
}
/* end subnroutines (ucproguser_namer) */

static int ucproguser_namealloc(UCPU *uip,cchar *cbuf,int clen) noex {
	int		rs = SR_BUGCHECK ;
	if (uip->username == nullptr) {
	    if (cchar *up ; (rs =  libmem.strw(cbuf,clen,&up)) >= 0) {
		uip->username = up ;
	    } /* end if (m-a) */	
	} /* end if (non-null) */
	return rs ;
}
/* end subnroutines (ucproguser_namelloc) */

static int ucproguser_namefree(UCPU *uip) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (uip->username) {
	    char *bp = cast_const<charp>(uip->username) ;
	    rs1 = libmem.free(bp) ;
	    if (rs >= 0) rs = rs1 ;
	    uip->username = nullptr ;
	}
	return rs ;
}
/* end subroutine (ucproguser_namefree) */

static int ucproguser_nameend(UCPU *uip) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (uip->userhome) {
	    char *bp = cast_const<charp>(uip->userhome) ;
	    rs1 = libmem.free(bp) ;
	    if (rs >= 0) rs = rs1 ;
	    uip->userhome = nullptr ;
	}
	if (uip->username) {
	    char *bp = cast_const<charp>(uip->username) ;
	    rs1 = libmem.free(bp) ;
	    if (rs >= 0) rs = rs1 ;
	    uip->username = nullptr ;
	}
	return rs ;
}
/* end subroutine (ucproguser_nameend) */

local void ucproguser_atforkbefore() noex {
	UCPU	*uip = &ucproguser_data ;
	ptm_lock(&uip->mx) ;
}
/* end subroutine (ucproguser_atforkbefore) */

local void ucproguser_atforkafter() noex {
	UCPU	*uip = &ucproguser_data ;
	ptm_unlock(&uip->mx) ;
}
/* end subroutine (ucproguser_atforkafter) */

local void ucproguser_exit() noex {
	if (cint rs = ucproguser_fini() ; rs < 0) {
	    ulogerror("ucproguser",rs,"exit-fini") ;
	}
}
/* end subroutine (ucproguser_exit) */

void ucproguser::dtor() noex {
	if (cint rs = ucproguser_fini() ; rs < 0) {
	    ulogerror("ucproguser",rs,"dtor-fini") ;
	}
} /* end method (ucproguser::dtor) */



/* ucproguser SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* interface components for UNIX® library-3c */
/* get or set a cached username given a UID */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* debugging */

/* revision history:

	= 2004-11-22, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 2004 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	ucproguser{x}

	Description:
	Set (as if for a cache) and get a username given a UID.

	Synopsis:
    	int ucproguser_init() noex
    	int ucproguser_fini() noex
	int ucproguser_nameset(cchar *cbuf,int clen,uid_t uid,int ttl) noex
	int ucproguser_nameget(char *rbuf,int rlen,uid_t uid) noex

	Arguments:
	cbuf		installing username pointer
	clen		installing username length
	rbuf		result buffer pointer
	rlen		result buffer length
	uid		UID of user to store or get
	ttl		time-to-live

	Returns:
	>0		string length of found username
	==0		could not get a name
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
	A. Probably, but it looks like I have already done more to 
	   this simple function than may have been ever warranted
	   to begin with!

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
#include	<usyscalls.h>		/* LIBU |ulogerror(3u)| */
#include	<uclibmem.h>		/* LIBUC */
#include	<ucatexit.h>		/* LIBUC */
#include	<ucatfork.h>		/* LIBUC */
#include	<ucfork.h>		/* LIBUC */
#include	<sigblocker.h>		/* LIBU */
#include	<ptm.h>			/* LIBU */
#include	<sncpyx.h>		/* LIBUC */
#include	<isnot.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */
#include	<dprint.hh>		/* LIBU |DPRINTF(3u)| */

#include	"ucproguser.h"


/* local defines */

#define	TO_TTL		(2*3600) /* two hours */

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* debugging */
#endif


/* imported namespaces */

using libuc::libmem ;			/* variable */


/* local typedefs */

typedef volatile sig_atomic_t	vaflag ;


/* external namespaces */


/* external subroutines */

extern "C" {
    int		ucproguser_init() noex ;
    int		ucproguser_fini() noex ;
} /* end extern (C) */


/* external variables */


/* local structures */

namespace {
    struct pumgr {
	ptm		mx ;		/* data mutex */
	time_t		et ;
	cchar		*username ;	/* memory-allocated */
	cchar		*userhome ;	/* memory-allocated */
	uid_t		muid ;		/* managed UID */
	int		ttl ;		/* time-to-live */
	vaflag		f_void ;
	vaflag		f_init ;
	vaflag		f_initdone ;
	int namer	(cchar *,int) noex ;
	int namemall	(cchar *,int) noex ;
	int namefree	() noex ;
	int namefin	() noex ;
	int namegeter	(char *,int,uid_t) noex ;
	void dtor() noex ;
	destruct pumgr() {
	    if (username || userhome) dtor() ;
	} ; /* end dtor */
    } ; /* end struct (pumgr) */
} /* end namespace */


/* forward references */

extern "C" {
    local void	ucproguser_atforkbefore() noex ;
    local void	ucproguser_atforkafter() noex ;
    local void	ucproguser_exit() noex ;
} /* end extern (C) */


/* local variables */

static pumgr	ucproguser_data ;

cbool		f_debug		= CF_DEBUG ;


/* exported variables */


/* exported subroutines */

int ucproguser_init() noex {
	pumgr		*uip = &ucproguser_data ;
	int		rs = SR_NXIO ;
	int		f = false ;
	if (! uip->f_void) {
	    rs = SR_OK ;
	    if (! uip->f_init) {
		ptm *mxp = &uip->mx ;
	        uip->f_init = true ;
	        if ((rs = mxp->create) >= 0) {
	            void_f	b = ucproguser_atforkbefore ;
	            void_f	a = ucproguser_atforkafter ;
	            if ((rs = uc_atforkrec(b,a,a)) >= 0) {
	                if ((rs = uc_atexit(ucproguser_exit)) >= 0) {
	                    uip->f_initdone = true ;
	                    f = true ;
	                } /* end if (good) */
	                if (rs < 0) {
	                    uc_atforkexp(b,a,a) ;
			} /* end if (error) */
	            } /* end if (uc_atfork) */
	            if (rs < 0) {
	                mxp->destroy() ;
		    } /* end if (error) */
	        } /* end if (ptm_create) */
	        if (rs < 0) {
	            uip->f_init = false ;
		} /* end if (error) */
	    } else if (! uip->f_initdone) {
	        while ((rs >= 0) && uip->f_init && (! uip->f_initdone)) {
	            rs = msleep(1) ;
	            if (rs == SR_INTR) rs = SR_OK ;
	        } /* end while */
	        if ((rs >= 0) && (! uip->f_init)) {
		    rs = SR_LOCKFAIL ;
		}
	    } /* end if */
	} /* end if (not voided) */
	return (rs >= 0) ? f : rs ;
} /* end subroutine (ucproguser_init) */

int ucproguser_fini() noex {
	pumgr		*uip = &ucproguser_data ;
	int		rs = SR_OK ;
	int		rs1 ;
	if (uip->f_initdone && (! uip->f_void)) {
	    uip->f_void = true ;
	    {
	        rs1 = uip->namefin() ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
	        void_f	b = ucproguser_atforkbefore ;
	        void_f	a = ucproguser_atforkafter ;
	        rs1 = uc_atforkexp(b,a,a) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
		ptm *mxp = &uip->mx ;
	        rs1 = mxp->destroy() ;
		if (rs >= 0) rs = rs1 ;
	    }
	    uip->f_init = false ;
	    uip->f_initdone = false ;
	} /* end if (was initialized) */
	return rs ;
} /* end subroutine (ucproguser_fini) */

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
	                pumgr	*uip = &ucproguser_data ;
	                if ((rs = uc_forklockbegin(-1)) >= 0) {
			    ptm *mxp = &uip->mx ;
	                    if ((rs = mxp->lockbegin) >= 0) {
				if ((rs = uip->namer(cbuf,clen)) >= 0) {
				    ul = rs ;
				    uip->et = time(nullptr) ;
				    uip->muid = uid ;
				    uip->ttl = ttl ;
				} /* end if */
	                        rs1 = mxp->lockend ;
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
} /* end subroutine (ucproguser_nameset) */

int ucproguser_nameget(char *rbuf,int rlen,uid_t uid) noex {
	pumgr		*uip = &ucproguser_data ;
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ; /* return-value */
	DPRINTF("ent uid=%u\n",uid) ;
	if (rbuf) {
	    if (uid == 0) uid = getuid() ;
	    rbuf[0] = '\0' ;
	    rs = SR_OK ;
	    if (uip->username) {
	        if (uip->username[0] != '\0') {
	            if (sigblocker b ; (rs = b.start) >= 0) {
	                if ((rs = ucproguser_init()) >= 0) {
	                    if ((rs = uc_forklockbegin(-1)) >= 0) {
			        ptm *mxp = &uip->mx ;
	                        if ((rs = mxp->lockbegin) >= 0) {
				    {
				        rs = uip->namegeter(rbuf,rlen,uid) ;
				        len = rs ;
				    }
	                            rs1 = mxp->lockend ;
	                            if (rs >= 0) rs = rs1 ;
	                        } /* end if (mutex) */
	 		        rs1 = uc_forklockend() ;
	                        if (rs >= 0) rs = rs1 ;
	                    } /* end if (forklock) */
	                } /* end if (ucproguser_init) */
	                rs1 = b.finish ;
		        if (rs >= 0) rs = rs1 ;
	            } /* end if (sigblock) */
	        } /* end if (have some username) */
	    } /* end if (non-null) */
	} /* end if (non-null) */
	DPRINTF("ret rs=%d len=%d\n",rs,len) ;
	return (rs >= 0) ? len : rs ;
} /* end subroutine (ucproguser_nameget) */


/* local subroutines */

int pumgr::namer(cchar *cbuf,int clen) noex {
	int		rs ;
	if ((rs = namefree()) >= 0) {
	    rs = namemall(cbuf,clen) ;
	}
	return rs ;
} /* end method (pumgr::namer) */

int pumgr::namemall(cchar *cbuf,int clen) noex {
	int		rs = SR_BUGCHECK ;
	if (username == nullptr) {
	    if (cchar *up ; (rs =  libmem.strw(cbuf,clen,&up)) >= 0) {
		username = up ;
	    } /* end if (memory-acquire) */	
	} /* end if (non-null) */
	return rs ;
} /* end method (pumgr::namemall) */

int pumgr::namefree() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (username) {
	    char *bp = cast_const<charp>(username) ;
	    rs1 = libmem.free(bp) ;
	    if (rs >= 0) rs = rs1 ;
	    username = nullptr ;
	} /* end if (memory-release) */
	return rs ;
} /* end method (pumgr::namefree) */

int pumgr::namefin() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (userhome) {
	    char *bp = cast_const<charp>(userhome) ;
	    rs1 = libmem.free(bp) ;
	    if (rs >= 0) rs = rs1 ;
	    userhome = nullptr ;
	} /* end if (memory-release) */
	if (username) {
	    rs1 = namefree() ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (memory-release) */
	return rs ;
} /* end method (pumgr::namefin) */

int pumgr::namegeter(char *rbuf,int rlen,uid_t uid) noex {
    	int		rs = SR_OK ;
	int		len = 0 ; /* return-value */
        if (username[0] != '\0') {
            if (et > 0) {
                custime         dt = time(nullptr) ;
                if ((dt - et) < ttl) {
                    if (muid == uid) {
                        rs = sncpy1(rbuf,rlen,username) ;
                        len = rs ;
                    } /* end if (match) */
                } /* end if (not timed-out) */
            } /* end if (possible) */
        } /* end if (not empty) */
	return (rs >= 0) ? len : rs ;
} /* end method (pumgr::namegeter) */

local void ucproguser_atforkbefore() noex {
	pumgr	*uip = &ucproguser_data ;
	{
	    ptm *mxp = &uip->mx ;
	    mxp->lockbegin() ;
	}
} /* end subroutine (ucproguser_atforkbefore) */

local void ucproguser_atforkafter() noex {
	pumgr	*uip = &ucproguser_data ;
	{
	    ptm *mxp = &uip->mx ;
	    mxp->lockend() ;
	}
} /* end subroutine (ucproguser_atforkafter) */

local void ucproguser_exit() noex {
	if (cint rs = ucproguser_fini() ; rs < 0) {
	    ulogerror("ucproguser",rs,"exit-fini") ;
	}
} /* end subroutine (ucproguser_exit) */

void pumgr::dtor() noex {
	if (cint rs = ucproguser_fini() ; rs < 0) {
	    ulogerror("ucproguser",rs,"dtor-fini") ;
	}
} /* end method (pumgr::dtor) */



/* ucrand SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface components for UNIX® library-3c */
/* get random data from the system */
/* version %I% last-modified %G% */

#define	CF_GETRANDOM	1		/* compile-define |getrandom(2)| */

/* revision history:

	= 2010-08-28, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2010 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	uc_rand

	Description:
	This is an emulated operating system kernel call that retrieves
	random data from a pool of random data (in user space).

	Synopsis:
	int uc_rand(void *rbuf,int rlen) noex

	Arguments:
	rbuf		result buffer pointer
	rlen		result buffer length

	Returns:
	>=0		number of bytes returns
	<0		error code (system return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<unistd.h>
#include	<climits>		/* |CHAR_BIT| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<sigblocker.h>
#include	<timewatch.hh>
#include	<ptm.h>
#include	<ptc.h>
#include	<randomvar.h>
#include	<stdintx.h>
#include	<mkchar.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"getrandom.h"


/* local defines */

#define	RBUFLEN		64

#ifndef	CF_GETRANDOM
#define	CF_GETRANDOM	1		/* compile-define |getrandom(2)| */
#endif


/* imported namespaces */

using libuc::libmem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

enum randermems {
	randermem_init,
	randermem_fini,
	randermem_randcheck,
	randermem_randbegin,
	randermem_randend,
	randermem_capend,
	randermem_addnoise,
	randermem_overlast
} ;

namespace {
    struct rander ;
    struct rander_co {
	rander		*op = nullptr ;
	int		w = -1 ;
	void operator () (rander *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
    } ;
    struct rander {
	friend		rander_co ;
	ptm		mx ;		/* data mutex */
	ptc		cv ;		/* condition variable */
	void		*rvarp ;	/* allocated object */
	time_t		ti_noise ;	/* last addition of noise */
	aflag		fvoid ;
	aflag		finit ;
	aflag		finitdone ;
	aflag		fcapture ;	/* capture flag */
	rander_co	init ;
	rander_co	fini ;
	rander_co	randcheck ;
	rander_co	randbegin ;
	rander_co	randend ;
	rander_co	capend ;
	rander_co	addnoise ;
	int		waiters ;
	rander() noex {
	    init	(this,randermem_init) ;
	    fini	(this,randermem_fini) ;
	    randcheck	(this,randermem_randcheck) ;
	    randbegin	(this,randermem_randbegin) ;
	    randend	(this,randermem_randend) ;
	    capend	(this,randermem_capend) ;
	    addnoise	(this,randermem_addnoise) ;
	} ; /* end ctor */
	int capbegin(int = -1) noex ;
	int get(char *,int) noex ;
	int geter(char *,int) noex ;
	void atbefore() noex {
	    mx.lockbegin() ;
	}
	void atafter() noex {
	    mx.lockend() ;
	}
    private:
	int iinit() noex ;
	int ifini() noex ;
	int irandcheck() noex ;
	int irandbegin() noex ;
	int irandend() noex ;
	int icapend() noex ;
	int iaddnoise() noex ;
    } ; /* end struct (rander) */
} /* end namespace */


/* forward references */

extern "C" {
    static void	rander_atforkbefore() noex ;
    static void	rander_atforkafter() noex ;
    static void	rander_exit() noex ;
}


/* local variables */

static rander		rander_data ;

constexpr bool		f_getrandom = CF_GETRANDOM ;


/* exported variables */


/* exported subroutines */

sysret_t uc_rand(void *arbuf,int arlen) noex {
	cint		rlen = arlen ;
	char		*rbuf = charp(arbuf) ;
	return rander_data.get(rbuf,rlen) ;
}
/* end subroutine (uc_rand) */


/* local subroutines */

int rander::iinit() noex {
	int		rs = SR_NXIO ;
	int		f = false ;
	if (! fvoid) {
	    cint	to = utimeout[uto_busy] ;
	    rs = SR_OK ;
	    if (! finit.testandset) {
	        if ((rs = mx.create) >= 0) {
	            if ((rs = cv.create) >= 0) {
	    	        void_f	b = rander_atforkbefore ;
	    	        void_f	a = rander_atforkafter ;
	                if ((rs = uc_atforkrecord(b,a,a)) >= 0) {
	                    if ((rs = uc_atexit(rander_exit)) >= 0) {
	                        finitdone = true ;
		 	        f = true ;
	                    }
	                    if (rs < 0) {
	                        uc_atforkexpunge(b,a,a) ;
			    }
	                } /* end if (uc_atfork) */
	                if (rs < 0) {
	                    cv.destroy() ;
			}
	            } /* end if (ptc_create) */
	            if (rs < 0) {
	                mx.destroy() ;
		    }
	        } /* end if (ptm_create) */
	        if (rs < 0) {
	            finit = false ;
		}
	    } else if (! finitdone) {
	        timewatch	tw(to) ;
	        auto lamb = [this] () -> int {
	            int		lrs = SR_OK ;
	            if (!finit) {
		        lrs = SR_LOCKLOST ;		/* <- failure */
	            } else if (finitdone) {
		        lrs = 1 ;			/* <- OK ready */
	            }
	            return lrs ;
	        } ; /* end lambda (lamb) */
	        rs = tw(lamb) ;		/* <- time-watching occurs in there */
	    } /* end if (initialization) */
	} /* end if (non-voided) */
	return (rs >= 0) ? f : rs ;
}
/* end method (rander::iinit) */

int rander::ifini() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (finitdone && (! fvoid.testandset)) {
	    {
	        rs1 = randend ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
	        void_f	b = rander_atforkbefore ;
	        void_f	a = rander_atforkafter ;
	        rs1= uc_atforkexpunge(b,a,a) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = cv.destroy ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = mx.destroy ;
		if (rs >= 0) rs = rs1 ;
	    }
	    finit = false ;
	    finitdone = false ;
	} /* end if (was initialized) */
	return rs ;
}
/* end method (rander::ifini) */

int rander::get(char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ;
	if (rbuf) {
	    rbuf[0] = '\0' ;
	    if (sigblocker b ; (rs = b.start) >= 0) {
	        if ((rs = init) >= 0) {
	            cint	to = utimeout[uto_capture] ;
	            if ((rs = capbegin(to)) >= 0) {
		        if ((rs = randcheck) >= 0) {
	                    rs = geter(rbuf,rlen) ;
			    len = rs ;
		        } /* end if (rander::randcheck) */
	                rs1 = capend ;
	                if (rs >= 0) rs = rs1 ;
		    } /* end if (rander_cap) */
	        } /* end if (rander_init) */
	        rs1 = b.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (sigblock) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end method (rander::get) */

int rander::irandcheck() noex {
	int		rs = SR_OK ;
	if (rvarp == nullptr) {
	    rs = randbegin ;
	}
	return rs ;
}
/* end method (rander::irandcheck) */

int rander::geter(char *rbuf,int rlen) noex {
	custime		dt = time(nullptr) ;
	cint		to_noise = utimeout[uto_busy] ;
	int		rs = SR_OK ;
	int		rl = 0 ;
	if ((dt - ti_noise) >= to_noise) {
	    ti_noise = dt ;
	    rs = addnoise ;
	} /* end if */
	if (rs >= 0) {
	    randomvar	*rvp = cast_static<randomvar *>(rvarp) ;
	    while ((rs >= 0) && (rlen > 0)) {
		if (ulong uv ; (rs = randomvar_getulong(rvp,&uv)) >= 0) {
	    	    cint	usz = szof(ulong) ;
		    for (int i = 0 ; (rlen > 0) && (i < usz) ; i += 1) {
			rbuf[rl++] = char(uv) ;
			uv >>= CHAR_BIT ;
			rlen -= 1 ;
		    } /* end for */
		} /* end if (randomvar_getulong) */
	    } /* end while */
	} /* end if */
	return (rs >= 0) ? rl : rs ;
}
/* end method (randero::geter) */

int rander::iaddnoise() noex {
	int		rs ;
	int		rs1 ;
	int		rl = 0 ;
	cint		rlen = RBUFLEN ;
	char		rbuf[RBUFLEN+1] ;
	if_constexpr (f_getrandom) {
	    if ((rs = uc_getrandom(rbuf,rlen,0)) >= 0) {
		randomvar	*rvp = cast_static<randomvar *>(rvarp) ;
		cint		len = rs ;
		rs = randomvar_addnoise(rvp,rbuf,len) ;
		rl = rs ;
	    } /* end if (uc_getrandom) */
	} else {
	    cint	of = O_RDONLY ;
	    cchar	*dev = sysword.w_devrandom ;
	    if ((rs = u_open(dev,of,0666)) >= 0) {
	        cint	fd = rs ;
	        if ((rs = u_read(fd,rbuf,rlen)) >= 0) {
		    randomvar	*rvp = cast_static<randomvar *>(rvarp) ;
		    cint	len = rs ;
		    rs = randomvar_addnoise(rvp,rbuf,len) ;
		    rl = rs ;
	        } /* end if (read) */
	        rs1 = u_close(fd) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (file-random) */
	} /* end if_constexpr (f_getrandom) */
	return (rs >= 0) ? rl : rs ;
}
/* end method (rander::iaddnoise) */

int rander::irandbegin() noex {
	int		rs = SR_OK ;
	if (rvarp == nullptr) {
	    cint	osz = szof(randomvar) ;
	    if (void *vp{} ; (rs = libmem.mall(osz,&vp)) >= 0) {
	        randomvar	*rvp = cast_static<randomvar *>(vp) ;
	        if ((rs = randomvar_start(rvp,0,0)) >= 0) {
	            rvarp = vp ;	/* <- store object pointer */
		}
	        if (rs < 0) {
	            libmem.free(vp) ;
		}
	    } /* end if (memory-allocation) */
	} /* end if (needed initialization) */
	return rs ;
}
/* end method (rander::irandbegin) */

int rander::irandend() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (rvarp) {
	    {
	        randomvar	*rvp = cast_static<randomvar *>(rvarp) ;
	        rs1 = randomvar_finish(rvp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = libmem.free(rvarp) ;
	        if (rs >= 0) rs = rs1 ;
	        rvarp = nullptr ;
	    }
	}
	return rs ;
}
/* end method (rander::irandend) */

int rander::capbegin(int to) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = mx.lockbegin(to)) >= 0) {
	    waiters += 1 ;
	    while ((rs >= 0) && fcapture) { /* busy */
	        rs = cv.wait(&mx,to) ;
	    } /* end while */
	    if (rs >= 0) {
	        fcapture = true ;
	    }
	    waiters -= 1 ;
	    rs1 = mx.lockend ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (ptm) */
	return rs ;
}
/* end method (rander::capbegin) */

int rander::icapend() noex {
	int		rs ;
	int		rs1 ;
	if ((rs = mx.lockbegin) >= 0) {
	    fcapture = false ;
	    if (waiters > 0) {
	        rs = cv.signal ;
	    }
	    rs1 = mx.lockend ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (ptm) */
	return rs ;
}
/* end method (rander::icapend) */

static void rander_atforkbefore() noex {
	rander_data.atbefore() ;
}
/* end subroutine (rander_atforkbefore) */

static void rander_atforkafter() noex {
	rander_data.atafter() ;
}
/* end subroutine (rander_atforkafter) */

static void rander_exit() noex {
	if (cint rs = rander_data.fini() ; rs < 0) {
	    ulogerror("ucrand",rs,"exit-fini") ;
	}
}
/* end subroutine (rander_exit) */

rander_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case randermem_init:
	        rs = op->iinit() ;
	        break ;
	    case randermem_fini:
	        rs = op->ifini() ;
	        break ;
	    case randermem_randcheck:
	        rs = op->irandcheck() ;
	        break ;
	    case randermem_randbegin:
	        rs = op->irandbegin() ;
	        break ;
	    case randermem_randend:
	        rs = op->irandend() ;
	        break ;
	    case randermem_capend:
	        rs = op->icapend() ;
	        break ;
	    case randermem_addnoise:
	        rs = op->iaddnoise() ;
	        break ;
	    } /* end switch */
	}
	return rs ;
}
/* end method (rander_co::operator) */



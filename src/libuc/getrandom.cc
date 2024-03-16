/* getrandom SUPPORT */
/* lang=C++20 */

/* Get-Random-data UNIX® System interposer */
/* version %I% last-modified %G% */

#define	CF_GETENTROPY	0		/* compile-define |getentropy(2)| */

/* revision history:

	= 2010-08-28, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2010 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This is a version of |getrandom(2)| that is preloaded to
	over-ride the standard UNIX® system version.

	Q. Is this multi-thread safe?
	A. Since it is a knock-off of an existing UNIX® system LIBC (3c)
	   subroutine that is already multi-thread safe -- then of course
	   it is!

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<sys/random.h>		/* |getentropy(2)| */
#include	<unistd.h>
#include	<cerrno>
#include	<csignal>
#include	<cstdlib>
#include	<cstring>
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

#ifndef	RANDOMDEV
#define	RANDOMDEV	"/dev/urandom"
#endif

#define	RBUFLEN		64

#ifndef	GRAND_RANDOM
#define	GRAND_RANDOM	(1<<1)
#define	GRAND_NONBLOCK	(1<<2)
#endif /* GRAND_RANDOM */

#define	GETRANDOM_MAXENT	256	/* maximum bytes per call */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct rander {
	ptm		mx ;		/* data mutex */
	ptc		cv ;		/* condition variable */
	void		*rv ;
	time_t		ti_noise ;	/* last addition of noise */
	int		waiters ;
	aflag		fvoid ;
	aflag		finit ;
	aflag		finitdone ;
	aflag		fcapture ;	/* capture flag */
	int init() noex ;
	int fini() noex ;
	int randcheck() noex ;
	int randbegin() noex ;
	int randend() noex ;
	int capbegin(int = -1) noex ;
	int capend() noex ;
	int get(char *,int,uint) noex ;
	int geter(char *,int,uint) noex ;
	int addnoise(uint) noex ;
	void atbefore() noex {
	    mx.lockbegin() ;
	}
	void atafter() noex {
	    mx.lockend() ;
	}
    } ; /* end struct (rander) */
}


/* forward references */

extern "C" {
    static void	rander_atforkbefore() noex ;
    static void	rander_atforkafter() noex ;
    static void	rander_exit() noex ;
}


/* local variables */

static rander		rander_data ;

static constexpr cpcchar	devs[] = {
	"/dev/urandom",
	"/dev/random",
	nullptr
} ;


/* exported variables */


/* exported subroutines */

int getrandom(void *arbuf,size_t arlen,uint fl) noex {
	cint		rlen = (int) arlen ;
	int		rs ;
	char		*rbuf = charp(arbuf) ;
	if ((rs = rander_data.get(rbuf,rlen,fl)) < 0) {
	    errno = (-rs) ;
	    rs = -1 ;
	}
	return rs ;
}
/* end subroutine (getrandom) */

#if	 CF_GETENTROPY
int getentropy(void *rbuf,size_t rlen) noex {
	int		rc = 0 ;
	if (rlen < GETRANDOM_MAXENT) {
	    if ((rc = getrandom(rbuf,rlen,0)) >= 0) {
	        rc = 0 ;
	    }
	} else {
	    errno = EIO ;
	    rc = -1 ;
	}
	return rc ;
}
/* end subroutine (getentropy) */
#endif /* CF_GETENTROPY */


/* local subroutines */

int rander::init() noex {
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
	            int		rs = SR_OK ;
	            if (!finit) {
		        rs = SR_LOCKLOST ;		/* <- failure */
	            } else if (finitdone) {
		        rs = 1 ;			/* <- OK ready */
	            }
	            return rs ;
	        } ; /* end lambda (lamb) */
	        rs = tw(lamb) ;		/* <- time-watching occurs in there */
	    } /* end if (initialization) */
	} /* end if (non-voided) */
	return (rs >= 0) ? f : rs ;
}
/* end method (rander::init) */

int rander::fini() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (finitdone && (! fvoid.testandset)) {
	    {
	        rs1 = randend() ;
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
/* end method (rander::fini) */

int rander::get(char *rbuf,int rlen,uint fl) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ;
	if (rbuf) {
	    sigblocker	b ;
	    rbuf[0] = '\0' ;
	    if ((rs = b.start) >= 0) {
	        if ((rs = init()) >= 0) {
	            cint	to = utimeout[uto_capture] ;
	            if ((rs = capbegin(to)) >= 0) {
		        if ((rs = randcheck()) >= 0) {
	                    rs = geter(rbuf,rlen,fl) ;
			    len = rs ;
		        } /* end if (rander::randcheck) */
	                rs1 = capend() ;
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

int rander::randcheck() noex {
	int		rs = SR_OK ;
	if (rv == nullptr) {
	    rs = randbegin() ;
	}
	return rs ;
}
/* end method (rander::randcheck) */

int rander::geter(char *rbuf,int rlen,uint fl) noex {
	const time_t	dt = time(nullptr) ;
	cint		to_noise = utimeout[uto_busy] ;
	int		rs = SR_OK ;
	int		rl = 0 ;
	if ((dt - ti_noise) >= to_noise) {
	    ti_noise = dt ;
	    rs = addnoise(fl) ;
	} /* end if */
	if (rs >= 0) {
	    randomvar	*rvp = static_cast<randomvar *>(rv) ;
	    ulong	uv{} ;
	    cint	usize = sizeof(ulong) ;
	    while ((rs >= 0) && (rlen > 0)) {
		if ((rs = randomvar_getulong(rvp,&uv)) >= 0) {
		    for (int i = 0 ; (rlen > 0) && (i < usize) ; i += 1) {
			rbuf[rl++] = (char) uv ;
			uv >>= 8 ;
			rlen -= 1 ;
		    } /* end for */
		} /* end if (randomvar_getulong) */
	    } /* end while */
	} /* end if */
	return (rs >= 0) ? rl : rs ;
}
/* end method (randero::geter) */

int rander::addnoise(uint fl) noex {
	cint		ri = bool(fl & GRAND_RANDOM) ;
	cint		of = O_RDONLY ;
	cchar		*dev ;
	int		rs ;
	int		rs1 ;
	dev = devs[ri] ;
	if ((rs = u_open(dev,of,0666)) >= 0) {
	    cint	rlen = RBUFLEN ;
	    cint	fd = rs ;
	    char	rbuf[RBUFLEN+1] ;
	    if ((rs = u_read(fd,rbuf,rlen)) >= 0) {
		randomvar	*rvp = static_cast<randomvar *>(rv) ;
		cint		len = rs ;
		rs = randomvar_addnoise(rvp,rbuf,len) ;
	    } /* end if (read) */
	    rs1 = u_close(fd) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (file-random) */
	return rs ;
}
/* end method (rander::addnoise) */

int rander::randbegin() noex {
	int		rs = SR_OK ;
	if (rv == nullptr) {
	    cint	osize = sizeof(randomvar) ;
	    void	*vp{} ;
	    if ((rs = uc_libmalloc(osize,&vp)) >= 0) {
	        randomvar	*rvp = static_cast<randomvar *>(vp) ;
	        if ((rs = randomvar_start(rvp,0,0)) >= 0) {
	            rv = vp ;
		}
	        if (rs < 0) {
	            uc_libfree(vp) ;
		}
	    } /* end if (memory-allocation) */
	} /* end if (needed initialization) */
	return rs ;
}
/* end method (rander::randbegin) */

int rander::randend() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (rv) {
	    {
	        randomvar	*rvp = static_cast<randomvar *>(rv) ;
	        rs1 = randomvar_finish(rvp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = uc_libfree(rv) ;
	        if (rs >= 0) rs = rs1 ;
	        rv = nullptr ;
	    }
	}
	return rs ;
}
/* end method (rander::randend) */

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

int rander::capend() noex {
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
/* end method (rander::capend) */

static void rander_atforkbefore() noex {
	rander_data.atbefore() ;
}
/* end subroutine (rander_atforkbefore) */

static void rander_atforkafter() noex {
	rander_data.atafter() ;
}
/* end subroutine (rander_atforkafter) */

static void rander_exit() noex {
	int		rs ;
	if ((rs = rander_data.fini()) < 0) {
	    ulogerror("getrandom",rs,"exit-fini") ;
	}
}
/* end subroutine (rander_exit) */



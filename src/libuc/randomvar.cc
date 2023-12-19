/* randomvar */
/* lang=C++20 */

/* random number generation object */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-08-11, David A­D­ Morano
	This object module was originally written.

	= 1999-10-08, David A­D­ Morano
	This module was updated to use 64-bit long integers that
	have been out for a while now (on Alpha and Sparc).

*/

/* Copyright © 1998,1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This object provides a random number generator.  This is
	similar to the UNIX® System 'random(3)' subroutine except
	that no global variables are used.  This version also
	provides a means to get truly random numbers by periodically
	mixing in garbage collected from the machine environment.

	We implement a standard cyclical shift register with feedback
	taps to create our random numbers (like 'random(3)' does)
	but we do it with a 128 degree polynomial.  Our polynomial
	is currently:

		x**128 + x**67 + x**23 + 1

	If you have a better one (which is likely) or even a good
	one, let me know!  Some known good polynomials for lower
	degrees than what we are working with are:

		 x**7 + x**3 + 1
		x**15 + x**1 + 1
		x**31 + x**3 + 1
		x**63 + x**1 + 1


*******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/time.h>		/* |gettimeofday(3c)| */
#include	<unistd.h>
#include	<climits>
#include	<cstdlib>
#include	<ctime>
#include	<usystem.h>
#include	<cfdec.h>
#include	<randlc.h>
#include	<localmisc.h>

#include	"randomvar.h"


/* local defines */

#define	MOD(n)		((n) % ndeg)
#define	COF(n)		MOD(n)

#define	STIRINTERVAL	(5 * 60)
#define	MAINTCOUNT	1000

#ifndef	VARRANDOM
#define	VARRANDOM	"RANDOM"
#endif

/* external subroutines */


/* forward references */

int		randomvar_getulong(randomvar *,ulong *) noex ;

template<typename ... Args>
static inline int randomvar_magic(randomvar *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == RANDOMVAR_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}

static int	randomvar_startreal(randomvar *,uint) noex ;
static int	randomvar_maint(randomvar *) noex ;

static void	addnoise(randomvar_st *,TIMEVAL *tvp) noex ;

static int	rdulong(cchar *,int,ulong *) noex ;
static int	wrulong(char *,int,ulong) noex ;


/* local variables */

constexpr int	digsize = sizeof(RANDOMVAR_DIGIT) ;
constexpr int	ndeg = sizeof(RANDOMVAR_DEGREE) ;
constexpr int	slen = RANDOMVAR_STATELEN ;


/* exported subroutines */

int randomvar_start(randomvar *rdp,int f_pseudo,uint seed) noex {
	int		rs = SR_FAULT ;
	if (rdp) {
	    ulong	dummy{} ;
	    rs = SR_OK ;
	    if (seed == 0) seed = 31415926 ;
	    rdp->f.flipper = false ;
	    rdp->f.pseudo = f_pseudo ;
	    rdp->laststir = 0 ;
	    rdp->maintcount = 0 ;
	    if (rdp->f.pseudo) {
	        rdp->state.is[0] = randlc(seed) ;
	        for (int i = 1 ; i < (ndeg * 2) ; i += 1) {
	            rdp->state.is[i] = randlc(rdp->state.is[i - 1]) ;
	        }
	    } else {
		rs = randomvar_startreal(rdp,seed) ;
	    } /* end if (initializing state) */
/* our polynomial --  x**128 + x**67 + x**23 + 1  */
	    rdp->a = COF(67) ;
	    rdp->b = COF(23) ;
	    rdp->c = COF(0) ;
	    rdp->magic = RANDOMVAR_MAGIC ;
/* stir the pot at least one cycle */
	    for (int i = 0 ; i < (ndeg * 20) ; i += 1) {
	        randomvar_getulong(rdp,&dummy) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (randomvar_start) */

int randomvar_finish(randomvar *rdp) noex {
	int		rs ;
	if ((rs = randomvar_magic(rdp)) >= 0) {
	    rdp->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (randomvar_finish) */

int randomvar_stateload(randomvar *rdp,cchar *state,int sl) noex {
	int		rs ;
	if ((rs = randomvar_magic(rdp,state)) >= 0) {
	    rs = SR_INVALID ;
	    if (sl >= slen) {
	        cchar	*sp = (cchar *) state ;
		rs = SR_OK ;
	        for (int i = 0 ; i < ndeg ; i += 1) {
	            ulong	ulw{} ;
		    int		r ;
	            if ((r = rdulong(sp,sl,&ulw)) > 0) {
	                rdp->state.ls[i] = ulw ;
		        sp += r ;
		        sl -= r ;
	            } else {
		        break ;
	            }
	        } /* end for */
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (randomvar_stateload) */

int randomvar_statesave(randomvar *rdp,char *state,int bl) noex {
	int		rs ;
	if ((rs = randomvar_magic(rdp,state)) >= 0) {
	    char	*bp = (char *) state ;
	    rs = SR_INVALID ;
	    if (bl >= slen) {
		rs = SR_OK ;
	        for (int i = 0 ; i < ndeg ; i += 1) {
	            ulong	ulw = rdp->state.ls[i]  ;
	            int		r ;
	            if ((r = wrulong(bp,bl,ulw)) > 0) {
		        bp += r ;
		        bl -= r ;
	            } else {
		        break ;
	            }
	        } /* end for */
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (randomvar_statesave) */

int randomvar_addnoise(randomvar *rdp,cvoid *noise,int sl) noex {
	int		rs ;
	if ((rs = randomvar_magic(rdp,noise)) >= 0) {
	    cint	nmax = (sl / digsize) ;
	    cchar	*sp = (cchar *) noise ;
	    for (int i = 0 ; i < nmax ; i += 1) {
	        ulong	ulw{} ;
	        int	r ;
	        if ((r = rdulong(sp,sl,&ulw)) > 0) {
	            int		ii = MOD(i) ;
	            rdp->state.ls[ii] ^= ulw ;
		    sp += r ;
		    sl -= r ;
	        } else {
		    break ;
	        }
	    } /* end for */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (randomvar_addnoise) */

/* set the polynomial to use (second highest degree to next lowest) */
int randomvar_setpoly(randomvar *rdp,int a,int b) noex {
	int		rs ;
	if ((rs = randomvar_magic(rdp)) >= 0) {
	    rs = SR_INVALID ;
	    if ((a > 0) && (a < ndeg)) {
	        if ((b > 0) && (b < ndeg)) {
		    rdp->a = COF(a) ;
		    rdp->b = COF(b) ;
		}
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (randomvar_setpoly) */

int randomvar_getlong(randomvar *rdp,long *rp) noex {
	int		rs ;
	if ((rs = randomvar_magic(rdp,rp)) >= 0) {
	    rdp->a = MOD(rdp->a) ;
	    rdp->b = MOD(rdp->b) ;
	    rdp->c = MOD(rdp->c) ;
	    rdp->state.ls[rdp->a] += rdp->state.ls[rdp->b] ;
	    rdp->state.ls[rdp->a] += rdp->state.ls[rdp->c] ;
	    *rp = (rdp->state.ls[rdp->a] >> 1) & LONG_MAX ;
	    rdp->a = MOD(rdp->a + 1) ;
	    rdp->b = MOD(rdp->b + 1) ;
	    rdp->c = MOD(rdp->c + 1) ;
	    if (++rdp->maintcount >= MAINTCOUNT) {
	        randomvar_maint(rdp) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (randomvar_getlong) */

int randomvar_getulong(randomvar *rdp,ulong *rp) noex {
	int		rs ;
	if ((rs = randomvar_magic(rdp,rp)) >= 0) {
	    rdp->a = MOD(rdp->a) ;
	    rdp->b = MOD(rdp->b) ;
	    rdp->c = MOD(rdp->c) ;
	    rdp->state.ls[rdp->a] += rdp->state.ls[rdp->b] ;
	    rdp->state.ls[rdp->a] += rdp->state.ls[rdp->c] ;
	    *rp = rdp->state.ls[rdp->a] ;
	    rdp->a = MOD(rdp->a + 1) ;
	    rdp->b = MOD(rdp->b + 1) ;
	    rdp->c = MOD(rdp->c + 1) ;
	    if (++rdp->maintcount >= MAINTCOUNT) {
	        randomvar_maint(rdp) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (randomvar_getulong) */

int randomvar_getint(randomvar *rdp,int *rp) noex {
	int		rs ;
	if ((rs = randomvar_magic(rdp,rp)) >= 0) {
	    rdp->a = MOD(rdp->a) ;
	    rdp->b = MOD(rdp->b) ;
	    rdp->c = MOD(rdp->c) ;
	    rdp->f.flipper = (! rdp->f.flipper) ;
	    if (rdp->f.flipper) {
	        rdp->state.ls[rdp->a] += rdp->state.ls[rdp->b] ;
	        rdp->state.ls[rdp->a] += rdp->state.ls[rdp->c] ;
	        *rp = (rdp->state.is[(rdp->a << 1) + 1] >> 1) & INT_MAX ;
	        rdp->a = MOD(rdp->a + 1) ;
	        rdp->b = MOD(rdp->b + 1) ;
	        rdp->c = MOD(rdp->c + 1) ;
	    } else {
	        *rp = (rdp->state.is[rdp->a << 1] >> 1) & INT_MAX ;
	    }
	    if (++rdp->maintcount >= MAINTCOUNT) {
	        randomvar_maint(rdp) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (randomvar_getint) */

int randomvar_getuint(randomvar *rdp,uint *rp) noex {
	int		rs ;
	if ((rs = randomvar_magic(rdp,rp)) >= 0) {
	    rdp->a = MOD(rdp->a) ;
	    rdp->b = MOD(rdp->b) ;
	    rdp->c = MOD(rdp->c) ;
	    rdp->f.flipper = (! rdp->f.flipper) ;
	    if (rdp->f.flipper) {
	        rdp->state.ls[rdp->a] += rdp->state.ls[rdp->b] ;
	        rdp->state.ls[rdp->a] += rdp->state.ls[rdp->c] ;
	        *rp = rdp->state.is[(rdp->a << 1) + 1] ;
	        rdp->a = MOD(rdp->a + 1) ;
	        rdp->b = MOD(rdp->b + 1) ;
	        rdp->c = MOD(rdp->c + 1) ;
	    } else {
	        *rp = rdp->state.is[rdp->a << 1] ;
	    }
	    if (++rdp->maintcount >= MAINTCOUNT) {
	        randomvar_maint(rdp) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (randomvar_getuint) */


/* private subroutines */

static int randomvar_startreal(randomvar *rdp,uint seed) noex {
	        TIMEVAL		tv ;
	        cint		pid = getpid() ;
	        cint		uid = getuid() ;
	        uint		v1 = getppid() ;
	        uint		v2 = getpgrp() ;
	        uint		v3 = 0 ;
	        int		i = 0 ;
  	        cchar		*cp ;
	        gettimeofday(&tv,nullptr) ;
	        if ((cp = getenv(VARRANDOM)) != nullptr) {
		    cfdecui(cp,-1,&v3) ;
	        }
	        rdp->state.is[i++] += randlc(tv.tv_usec) ;
	        rdp->state.is[i++] += randlc(pid) ;
	        rdp->state.is[i++] += randlc(v1) ;
	        rdp->state.is[i++] += randlc(v2) ;
	        rdp->state.is[i++] += randlc(v3) ;
	        rdp->state.is[i++] += randlc(tv.tv_sec) ;
	        rdp->state.is[i++] += randlc(uid) ;
	        for (int j = 0 ; j < 6 ; j += 1) {
	            seed ^= rdp->state.is[j] ;
	        }
	        rdp->state.is[i++] += randlc(seed) ;
	        for (int j = i ; j < (ndeg * 2) ; j += 1) {
	            rdp->state.is[j] += randlc(rdp->state.is[j - 1]) ;
	        }
	return SR_OK ;
}
/* end subroutine (randomvar_startreal) */

static int randomvar_maint(randomvar *rdp) noex {
	rdp->maintcount = 0 ;
	for (int i = 0 ; i < (ndeg * 2) ; i += 2) {
	    uint	v0 = randlc(rdp->state.is[i + 0]) ;
	    uint	v1 = randlc(rdp->state.is[i + 1]) ;
	    rdp->state.is[i + 0] += v1 ;
	    rdp->state.is[i + 1] += v0 ;
	} /* end for */
	if (! rdp->f.pseudo) {
	    TIMEVAL	tv ;
	    gettimeofday(&tv,nullptr) ;
	    if ((tv.tv_sec - rdp->laststir) >= RANDOMVAR_STIRTIME) {
	        rdp->laststir = tv.tv_sec ;
	        addnoise(&rdp->state,&tv) ;
	    }
	} /* end if (not-pseudo) */
	return SR_OK ;
}
/* end subroutine (randomvar_maint) */

static void addnoise(randomvar_st *sp,TIMEVAL *tvp) noex {
	sp->is[0] ^= randlc(tvp->tv_sec ^ sp->is[0]) ;
	sp->is[1] ^= randlc(tvp->tv_usec ^ sp->is[1]) ;
}
/* end subroutine (addnoise) */

static int rdulong(cchar *sp,int sl,ulong *lp) noex {
	int		r = 0 ;
	if (sl > 0) {
	    ulong	ulw = 0 ;
	    int		mlen = MIN(sl,digsize) ;
	    int		i ;
	    for (i = 0 ; i < mlen ; i += 1) {
		ulw <<= 8 ;
		ulw |= ((uchar) *sp++) ;
		r += 1 ;
	    }
	    *lp = ulw ;
	} else {
	    *lp = 0 ;
	}
	return r ;
}
/* end subroutine (rdulong) */

static int wrulong(char *bp,int bl,ulong ulw) noex {
	cint		n = digsize ;
	int		i = 0 ;
	for (i = 0 ; (i < n) && (i < bl) ; i += 1) {
	    *bp = (char) ulw ; ulw >>= 8 ;
	} /* end for */
	return i ;
}
/* end subroutine (wrulong) */



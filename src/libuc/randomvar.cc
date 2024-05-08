/* randomvar SUPPORT */
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
	similar to the UNIX® System |random(3)| subroutine except
	that no global variables are used.  This version also
	provides a means to get truly random numbers by periodically
	mixing in garbage collected from the machine environment.

	We implement a standard cyclical shift register with feedback
	taps to create our random numbers (like |random(3)| does)
	but we do it with a 128 degree polynomial.  Our polynomial
	is currently:

		x**127 + x**67 + x**23 + 1

	If you have a better one (which is likely) or even a good
	one, let me know!  Some known good polynomials for lower
	degrees than what we are working with are:

		 x**7 + x**3 + 1
		x**15 + x**1 + 1
		x**31 + x**3 + 1
		x**63 + x**1 + 1

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/time.h>		/* |gettimeofday(3c)| */
#include	<unistd.h>
#include	<climits>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* UNIX® system subroutines */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<ucvariables.hh>
#include	<cfdec.h>
#include	<randlc.h>
#include	<localmisc.h>

#include	"randomvar.h"


/* local defines */

#define	MOD(n)		((n) % slen)
#define	COF(n)		MOD(n)

#define	STIRINTERVAL	(5 * 60)
#define	MAINTCOUNT	1000

#define	NINITS		6		/* number initiailization vars */
#define	NLS		4		/* number of longs in array(of) */


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    template<int N> struct procrand {
	cint		n = N ;
	uint		v[N] ;
    } ;
    template<int N> struct arrlongs {
	ulong		v[N] = {} ;
	cint		n = N ;
	int		c = 0 ;
	bool		flipped = false ;
	void add(uint iv) noex {
	    if (c < n) {
	        if (flipped) {
		    ulong	tv = (ulong(iv) << 32) ;
		    v[c++] |= tv ;
		    flipped = false ;
	        } else {
		    v[c] = ulong(iv) ;
		    flipped = true ;
	        }
	    } /* end if */
	} ;
    } ; /* end struct (arrlongs) */
}


/* forward references */

template<typename ... Args>
static inline int randomvar_magic(randomvar *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == RANDOMVAR_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}

static int	randomvar_initpseudo(randomvar *,uint) noex ;
static int	randomvar_initreal(randomvar *,uint) noex ;
static int	randomvar_maint(randomvar *) noex ;
static int	randomvar_swapone(randomvar *) noex ;

static void	addtime(ulong *,TIMEVAL *tvp) noex ;

static int	rdulong(cchar *,int,ulong *) noex ;
static int	wrulong(char *,int,ulong) noex ;

static int mkprocrand() noex ;


/* local variables */

constexpr int	digsize = sizeof(RANDOMVAR_DIGIT) ;
constexpr int	slen = RANDOMVAR_STATELEN ;

static constexpr ulong	randtbl[] = {
	0x991539b116a5bce3UL, 0x6774a4cd3e01511eUL, 
	0x4e508aaa61048c05UL, 0xf5500617846b7115UL, 
	0x6a19892c896a97afUL, 0xdb48f93614898454UL,
	0x37ffd106b58bff9cUL, 0x59e17104cf918a49UL, 
	0x09378c8352c7a471UL, 0x8d293ea91f4fc301UL, 
	0xc3db71be39b44e1cUL, 0xf8a44ef94c8b80b1UL,
	0x19edc32887bf4bddUL, 0xc9b240e5e9ee4b1bUL, 
	0x4382aee7535b6b41UL, 0xf3bec5da31415926UL
} ;

static procrand<NINITS>	initrv ;


/* exported variables */


/* exported subroutines */

int randomvar_start(randomvar *op,int f_pseudo,uint seed) noex {
	int		rs = SR_FAULT ;
	if (seed == 0) seed = 31415926 ;
	if (op) {
	    void	*vp{} ;
	    csize	sz = (slen * sizeof(ulong)) ;
	    memclear(op) ;
	    if ((rs = uc_malloc(sz,&vp)) >= 0) {
		op->state = ulongp(vp) ;
	        op->f.flipper = false ;
	        op->f.pseudo = f_pseudo ;
	        op->laststir = 0 ;
	        op->maintcount = 0 ;
	        if (op->f.pseudo) {
		    rs = randomvar_initpseudo(op,seed) ;
	        } else {
		    rs = randomvar_initreal(op,seed) ;
	        } /* end if (initializing state) */
    /* our polynomial --  x**128 + x**67 + x**23 + 1  */
		if (rs >= 0) {
		    cint	n = (slen * 10) ;
	            op->a = COF(67) ;
	            op->b = COF(23) ;
	            op->c = COF(0) ;
	            op->magic = RANDOMVAR_MAGIC ;
		    for (int i = 0 ; (rs >= 0) && (i < n) ; i += 1) {
			ulong	dummy ;
			rs = randomvar_getulong(op,&dummy) ;
		    }
		} /* end if (ok) */
		if (rs < 0) {
		    uc_free(op->state) ;
		    op->state = nullptr ;
		    op->magic = 0 ;
		}
	    } /* end if (memory-allocation) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (randomvar_start) */

int randomvar_finish(randomvar *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = randomvar_magic(op)) >= 0) {
	    if (op->state) {
		rs1 = uc_free(op->state) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (randomvar_finish) */

int randomvar_stateload(randomvar *op,cchar *state,int sl) noex {
	int		rs ;
	if ((rs = randomvar_magic(op,state)) >= 0) {
	    rs = SR_INVALID ;
	    if (sl >= slen) {
	        cchar	*sp = charp(state) ;
		rs = SR_OK ;
	        for (int i = 0 ; i < slen ; i += 1) {
	            ulong	ulw{} ;
	            if (int r ; (r = rdulong(sp,sl,&ulw)) > 0) {
	                op->state[i] = ulw ;
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

int randomvar_statesave(randomvar *op,char *state,int bl) noex {
	int		rs ;
	if ((rs = randomvar_magic(op,state)) >= 0) {
	    char	*bp = charp(state) ;
	    rs = SR_INVALID ;
	    if (bl >= slen) {
		rs = SR_OK ;
	        for (int i = 0 ; i < slen ; i += 1) {
	            ulong	ulw = op->state[i]  ;
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

int randomvar_addnoise(randomvar *op,cvoid *noise,int sl) noex {
	int		rs ;
	if ((rs = randomvar_magic(op,noise)) >= 0) {
	    cint	nmax = (sl / digsize) ;
	    cchar	*sp = charp(noise) ;
	    for (int i = 0 ; i < nmax ; i += 1) {
	        ulong	ulw{} ;
	        if (int r ; (r = rdulong(sp,sl,&ulw)) > 0) {
	            int		ii = MOD(i) ;
	            op->state[ii] ^= ulw ;
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
int randomvar_setpoly(randomvar *op,int a,int b) noex {
	int		rs ;
	if ((rs = randomvar_magic(op)) >= 0) {
	    rs = SR_INVALID ;
	    if ((a > 0) && (a < slen)) {
	        if ((b > 0) && (b < slen)) {
		    op->a = COF(a) ;
		    op->b = COF(b) ;
		}
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (randomvar_setpoly) */

int randomvar_getlong(randomvar *op,long *rp) noex {
	ulong		res ;
	int		rs ;
	if ((rs = randomvar_getulong(op,&res)) >= 0) {
	    if (rp) *rp = long(res >> 1) ;
	} /* end if (randomvar_getulong) */
	return rs ;
}
/* end subroutine (randomvar_getlong) */

int randomvar_getulong(randomvar *op,ulong *rp) noex {
	int		rs ;
	if ((rs = randomvar_magic(op,rp)) >= 0) {
	    op->state[op->a] += op->state[op->b] ;
	    op->state[op->a] += op->state[op->c] ;
	    *rp = op->state[op->a] ;
	    op->a = MOD(op->a + 1) ;
	    op->b = MOD(op->b + 1) ;
	    op->c = MOD(op->c + 1) ;
	    if (op->maintcount++ >= MAINTCOUNT) {
	        randomvar_maint(op) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (randomvar_getulong) */

int randomvar_getint(randomvar *op,int *rp) noex {
	uint		res ;
	int		rs ;
	if ((rs = randomvar_getuint(op,&res)) >= 0) {
	    if (rp) *rp = (res >> 1) ;
	} /* end if (randomvar_getuint) */
	return rs ;
}
/* end subroutine (randomvar_getint) */

int randomvar_getuint(randomvar *op,uint *rp) noex {
	int		rs ;
	if ((rs = randomvar_magic(op,rp)) >= 0) {
	    ulong	rv ;
	    if (op->f.flipper) {
		rv = op->state[op->a] ;
		*rp = uint(rv >> 32) ;
		op->f.flipper = false ;
	    } else {
		if ((rs = randomvar_getulong(op,&rv)) >= 0) {
		    *rp = uint(rv) ;
		    op->f.flipper = true ;
		}
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (randomvar_getuint) */


/* private subroutines */

static int randomvar_initpseudo(randomvar *op,uint seed) noex {
	ulong		lseed = ulong(seed) ;
	ulong		tv = ulong(randlc(seed)) ;
	cint		tl = nelem(randtbl) ;
	lseed |= (tv << 32) ;
	for (int i = 0 ; i < min(tl,slen) ; i += 1) {
	    op->state[i] = randtbl[i] ;
	}
	for (int i = 0 ; i < slen ; i += 1) {
	    op->state[i] |= lseed ;
	}
	return SR_OK ;
}
/* end subroutine (randomvar_initpseudo) */

static int randomvar_initreal(randomvar *op,uint seed) noex {
	static int	rsr = mkprocrand() ;
	arrlongs<NLS>	al ;
	int		rs ;
	if ((rs = rsr) >= 0) {
	    uint	v ;
	    cint	ninit = rs ;
	    {
	        TIMEVAL	tv ;
	        gettimeofday(&tv,nullptr) ;
	        v = randlc(tv.tv_usec) ; al.add(v) ;
	        v = randlc(tv.tv_sec) ; al.add(v) ;
	    }
	    for (int i = 0 ; i < ninit ; i += 1) {
	        v = initrv.v[i] ;
		al.add(v) ;
	    }
	    v = randlc(seed) ;
	    al.add(v) ;
	    for (int i = 0 ; i < min(al.c,slen) ; i += 1) {
	        op->state[i] += al.v[i] ;
	    }
	} /* end if (mkprocrand) */
	return rs ;
}
/* end subroutine (randomvar_initreal) */

static int randomvar_maint(randomvar *op) noex {
	int		rs ;
	if ((rs = randomvar_swapone(op)) >= 0) {
	    op->maintcount = 0 ;
	    if (! op->f.pseudo) {
	        TIMEVAL	tv ;
	        gettimeofday(&tv,nullptr) ;
	        if ((tv.tv_sec - op->laststir) >= RANDOMVAR_STIRINT) {
	            op->laststir = tv.tv_sec ;
	            addtime((op->state+0),&tv) ;
	        }
	    } /* end if (not-pseudo) */
	} /* end if (swapone) */
	return rs ;
}
/* end subroutine (randomvar_maint) */

static int randomvar_swapone(randomvar *op) noex {
	ulong		one = op->state[0] ;
	ulong		one0, one1 ;
	uint		s0, s1, t0, t1 ;
	int		rs = SR_OK ;
	s0 = uint(one >> 00) ;
	s1 = uint(one >> 32) ;
	t0 = randlc(s0) ;
	t1 = randlc(s1) ;
	one0 = ulong(t1) ;	/* swapped */
	one1 = ulong(t0) ;	/* swapped */
	one = ((one1 << 32) | (one0 << 00)) ;
	op->state[0] = one ;
	return rs ;
}
/* end subroutine (randomvar_swapone) */

static void addtime(ulong *lp,TIMEVAL *tvp) noex {
	ulong		cur = *lp ;
	uint		t0 = uint(tvp->tv_sec) ;
	uint		t1 = uint(tvp->tv_usec) ;
	uint		c0, c1, n0, n1 ;
	c0 = (cur >> 0) ;
	c1 = (cur >> 32) ;
	n0 = c0 ^ randlc(c0 ^ t0) ;
	n1 = c1 ^ randlc(c1 ^ t1) ;
	{
	    ulong	ln0 = ulong(n0) ;
	    ulong	ln1 = ulong(n1) ;
	    ulong	stage ;
	    stage = ((ln1 << 32) | ln0) ;
	    *lp = stage ;
	}
}
/* end subroutine (addtime) */

static int rdulong(cchar *sp,int sl,ulong *lp) noex {
	int		r = 0 ;
	if (sl > 0) {
	    ulong	ulw = 0 ;
	    int		mlen = min(sl,digsize) ;
	    for (int i = 0 ; i < mlen ; i += 1) {
		ulw <<= 8 ;
		ulw |= uchar(*sp++) ;
		r += 1 ;
	    } /* end for */
	    *lp = ulw ;
	} else {
	    *lp = 0 ;
	}
	return r ;
}
/* end subroutine (rdulong) */

static int wrulong(char *bp,int bl,ulong ulw) noex {
	cint		n = digsize ;
	int		i = 0 ; /* used afterwards */
	for (i = 0 ; (i < n) && (i < bl) ; i += 1) {
	    *bp = char(ulw) ; ulw >>= 8 ;
	} /* end for */
	return i ;
}
/* end subroutine (wrulong) */

/* this creates the process-constant randoness */
static int mkprocrand() noex {
	uint		v ;
	int		rs = SR_OK ;
	int		i = 0 ;
	cchar		*var = getenv(varname.random) ;
	initrv.v[i++] = (v = getpid(),randlc(v)) ;
	initrv.v[i++] = (v = getuid(),randlc(v)) ;
	initrv.v[i++] = (v = getppid(),randlc(v)) ;
	initrv.v[i++] = (v = getpgrp(),randlc(v)) ;
	if (var) {
	    if (uint v{} ; cfdecui(var,-1,&v) >= 0) {
		initrv.v[i++] = v ;
	    }
	}
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (mkprocrand) */



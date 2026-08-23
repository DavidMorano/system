/* randomvar SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* random number generation object */
/* UNIX® standard-library support functions */
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

  	Object:
	randomvar

	Description:
	This object provides a random number generator.  This is
	similar to the UNIX® System |random(3)| subroutine except
	that no global variables are used.  Rather this is an onject
	and can be instantiated wherever one wants.  This version
	also provides a means to get truly random numbers (besides
	pseudo-random numbers) by periodically mixing in garbage
	supplied by the caller.  Of couse, like all normal objects,
	it is thread-safe.

	We implement a standard cyclical shift register with feedback
	taps to create our random numbers (like |random(3)| does)
	but we do it with a 127 degree polynomial.  Our polynomial
	is currently:

		x**127 + x**67 + x**23 + x**0

	If you have a better one (which is likely) or even a good
	one, let me know!  Some known good polynomials for lower
	degrees than what we are working with are:

		 x**7 + x**3 + 1
		x**15 + x**1 + 1
		x**31 + x**3 + 1
		x**63 + x**1 + 1

	Implementation-notes:
	This is an object.  It is -- of course -- thread-safe, as
	all of my code is and always has been these last decades.
	This object supports two kinds of uses.  It can be used to
	provide pseudo-random numbers, or it can be used to provide
	real random numbers (a little bit harder, but not by too
	much). Both used (just mentioned) require some initialization
	of the object state.  In the case of pseudo-random numbers,
	the state is just some static data stored below that is
	loaded into the object state variable.  In the case of real
	random numbers, the initialization has two phases. One:
	some initial random data is extracted that is constant over
	the lifetime of the process.  This data is collected once
	and is then available for further object initializations
	after the first (see the |mkprocrand()| subroutine below).
	And two: the second phase of real random number initialization
	comes from dynamic system resources, and has to occur every
	time a new object is instantiated.

	Note:
	This code is part of the UNIX® standard-library interface layer
	library ("libuc").

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/time.h>		/* POSIX® |gettimeofday(3c)| */
#include	<unistd.h>		/* POSIX® */
#include	<ctime>			/* CSTD */
#include	<climits>		/* CSTD |CHAR_BIT| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD UNIX® system subroutines */
#include	<algorithm>		/* C++STD |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usupport.h>		/* LIBU |libu::cfdec(3u)| */
#include	<uclibmem.h>		/* LIBUC */
#include	<ucgetx.h>		/* LIBUC */
#include	<randlc.h>		/* LIBUC */
#include	<isnot.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"randomvar.h"

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/uconstants.ccm"

import libutil ;			/* |memclear(3u)| */
import uconstants ;

/* local defines */

#define	MOD(n)		((n) % slen)
#define	COF(n)		MOD(n)

#define	STIRINTERVAL	(5 * 60)
#define	MAINTCOUNT	1000
#define	LOOPMULT	10

#define	NINITS		8		/* number initiailization vars */
#define	NLS		8		/* number of longs in array(of) */


/* imported namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using libu::cfdec ;			/* subroutine (overloaded) */
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
	ulong		v[N] ;
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
	int addtime() noex {
	    cnullptr	np{} ;
	    int		rs ;
	    if (TIMEVAL tv ; (rs = uc_gettimeofday(&tv,np)) >= 0) {
		int		val ;
		int		dum ;
		dum = intconv(tv.tv_usec) ;
	        val = randlc(dum) ; add(val) ;
		dum = intconv(tv.tv_sec) ;
	        val = randlc(dum) ; add(val) ;
	    } /* end if (uc_gettimeofday) */
	    return rs ;
        } ; /* end method (addtime) */
    } ; /* end struct (arrlongs) */
} /* end namespace */


/* forward references */

template<typename ... Args>
local inline int randomvar_magic(randomvar *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == RANDOMVAR_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (randomvar_magic) */

local int	randomvar_initpseudo(randomvar *,uint) noex ;
local int	randomvar_initreal(randomvar *,uint) noex ;
local int	randomvar_maint(randomvar *) noex ;
local int	randomvar_swapone(randomvar *) noex ;

static void	addtime(ulong *,TIMEVAL *tvp) noex ;

local int	rdulong(cchar *,int,ulong *) noex ;
local int	wrulong(char *,int,ulong) noex ;

local int	mkprocrand() noex ;


/* local variables */

constexpr int	digsize = szof(RANDOMVAR_DIGIT) ;
constexpr int	slen = RANDOMVAR_STATELEN ;

constexpr ulong	randtbl[] = {
	0x991539b116a5bce3UL, 0x6774a4cd3e01511eUL, 
	0x4e508aaa61048c05UL, 0xf5500617846b7115UL, 
	0x6a19892c896a97afUL, 0xdb48f93614898454UL,
	0x37ffd106b58bff9cUL, 0x59e17104cf918a49UL, 
	0x09378c8352c7a471UL, 0x8d293ea91f4fc301UL, 
	0xc3db71be39b44e1cUL, 0xf8a44ef94c8b80b1UL,
	0x19edc32887bf4bddUL, 0xc9b240e5e9ee4b1bUL, 
	0x4382aee7535b6b41UL, 0xf3bec5da31415926UL
} ; /* end array (randtbl) */

static procrand<NINITS>	initrv ;


/* exported variables */


/* exported subroutines */

int randomvar_start(randomvar *op,int f_pseudo,uint seed) noex {
	int		rs = SR_FAULT ;
	if (seed == 0) seed = 31415926 ;
	if (op) ylikely {
	    csize	sz = (slen * szof(ulong)) ;
	    memclear(op) ;
	    if (void *vp ; (rs = lm_mall(sz,&vp)) >= 0) ylikely {
		op->state = ulongp(vp) ;
	        op->fl.flipper = false ;
	        op->fl.pseudo = !!f_pseudo ;
	        op->laststir = 0 ;
	        op->maintcount = 0 ;
	        if (op->fl.pseudo) {
		    rs = randomvar_initpseudo(op,seed) ;
	        } else {
		    rs = randomvar_initreal(op,seed) ;
	        } /* end if (initializing state) */
    		/* our polynomial --  x**127 + x**67 + x**23 + 1  */
		if (rs >= 0) ylikely {
		    cint	n = (slen * LOOPMULT) ;
	            op->a = COF(67) ;
	            op->b = COF(23) ;
	            op->c = COF(0) ;
	            op->magval = RANDOMVAR_MAGIC ;
		    for (int i = 0 ; (rs >= 0) && (i < n) ; i += 1) {
			ulong	dummy ;
			rs = randomvar_getulong(op,&dummy) ;
		    }
		} /* end if (ok) */
		if (rs < 0) {
		    lm_free(op->state) ;
		    op->state = nullptr ;
		    op->magval = 0 ;
		} /* end if (error) */
	    } /* end if (memory-acquire) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (randomvar_start) */

int randomvar_finish(randomvar *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = randomvar_magic(op)) >= 0) ylikely {
	    if (op->state) ylikely {
		rs1 = lm_free(op->state) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    op->magval = 0 ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (randomvar_finish) */

int randomvar_stateload(randomvar *op,cchar *nstate,int sl) noex {
	int		rs ;
	if ((rs = randomvar_magic(op,nstate)) >= 0) ylikely {
	    rs = SR_INVALID ;
	    if (sl >= slen) {
	        cchar	*sp = charp(nstate) ;
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
} /* end subroutine (randomvar_stateload) */

int randomvar_statesave(randomvar *op,char *state,int bl) noex {
	int		rs ;
	if ((rs = randomvar_magic(op,state)) >= 0) ylikely {
	    char	*bp = charp(state) ;
	    rs = SR_INVALID ;
	    if (bl >= slen) {
		rs = SR_OK ;
	        for (int i = 0 ; i < slen ; i += 1) {
	            const ulong		ulw = op->state[i]  ;
	            if (int r ; (r = wrulong(bp,bl,ulw)) > 0) {
		        bp += r ;
		        bl -= r ;
	            } else {
		        break ;
	            }
	        } /* end for */
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
} /* end subroutine (randomvar_statesave) */

int randomvar_addnoise(randomvar *op,cvoid *noise,int sl) noex {
	int		rs ;
	if ((rs = randomvar_magic(op,noise)) >= 0) ylikely {
	    cint	nmax = (sl / digsize) ;
	    cchar	*sp = charp(noise) ;
	    for (int i = 0 ; i < nmax ; i += 1) {
	        ulong	ulw{} ;
	        if (int r ; (r = rdulong(sp,sl,&ulw)) > 0) {
	            cint	ii = MOD(i) ;
	            op->state[ii] ^= ulw ;
		    sp += r ;
		    sl -= r ;
	        } else {
		    break ;
	        }
	    } /* end for */
	} /* end if (magic) */
	return rs ;
} /* end subroutine (randomvar_addnoise) */

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
} /* end subroutine (randomvar_setpoly) */

int randomvar_getlong(randomvar *op,long *rp) noex {
	int		rs ;
	if (ulong res ; (rs = randomvar_getulong(op,&res)) >= 0) ylikely {
	    if (rp) *rp = long(res >> 1) ;
	} /* end if (randomvar_getulong) */
	return rs ;
} /* end subroutine (randomvar_getlong) */

int randomvar_getulong(randomvar *op,ulong *rp) noex {
	int		rs ;
	if ((rs = randomvar_magic(op,rp)) >= 0) ylikely {
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
} /* end subroutine (randomvar_getulong) */

int randomvar_getint(randomvar *op,int *rp) noex {
	uint		res ;
	int		rs ;
	if ((rs = randomvar_getuint(op,&res)) >= 0) ylikely {
	    if (rp) *rp = (res >> 1) ;
	} /* end if (randomvar_getuint) */
	return rs ;
} /* end subroutine (randomvar_getint) */

int randomvar_getuint(randomvar *op,uint *rp) noex {
	int		rs ;
	if ((rs = randomvar_magic(op,rp)) >= 0) ylikely {
	    ulong	rv ; /* used-multiple */
	    if (op->fl.flipper) {
		rv = op->state[op->a] ;
		*rp = uint(rv >> 32) ;
		op->fl.flipper = false ;
	    } else {
		if ((rs = randomvar_getulong(op,&rv)) >= 0) {
		    *rp = uint(rv) ;
		    op->fl.flipper = true ;
		}
	    }
	} /* end if (magic) */
	return rs ;
} /* end subroutine (randomvar_getuint) */

int randomvar_get(randomvar *op,void *rbuf,int rlen) noex {
	int		rs ;
	if ((rs = randomvar_magic(op,rbuf)) >= 0) ylikely {
	    rs = SR_INVALID ;
	    if (rlen >= 0) ylikely {
	        cint	wl = szof(ulong) ;
	        char	*rp = charp(rbuf) ;
		rs = SR_OK ;
	        while ((rs >= 0) && (rlen > 0)) {
		    if (ulong rv ; (rs = randomvar_getulong(op,&rv)) >= 0) {
		        cint	ml = min(rlen,wl) ;
		        for (int i = 0 ; i < ml ; i += 1) {
			    *rp++ = char(rv) ;
			    rv >>= CHAR_BIT ;
			    rlen -= 1 ;
		        } /* end for */
		    } /* end if (randomvar_getulong) */
	        } /* end while */
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
} /* end subroutine (randomvar_getuint) */


/* private subroutines */

local int randomvar_initpseudo(randomvar *op,uint seed) noex {
	const ulong	tv = ulong(randlc(seed)) ;
	ulong		lseed = ulong(seed) ;
	cint		tl = nelem(randtbl) ;
	lseed |= (tv << 32) ;
	for (int i = 0 ; i < min(tl,slen) ; i += 1) {
	    op->state[i] = randtbl[i] ;
	}
	for (int i = 0 ; i < slen ; i += 1) {
	    op->state[i] |= lseed ;
	}
	return SR_OK ;
} /* end subroutine (randomvar_initpseudo) */

local int randomvar_initreal(randomvar *op,uint seed) noex {
	arrlongs<NLS>	al ;
	local int	rsr = mkprocrand() ;
	int		rs ;
	if ((rs = rsr) >= 0) ylikely {
	    cint	ninit = rs ;
	    uint	v ; /* used-multiple */
	    if ((rs = al.addtime()) >= 0) {
	        for (int i = 0 ; i < ninit ; i += 1) {
	            v = initrv.v[i] ;
		    al.add(v) ;
	        } /* end for */
	        v = randlc(seed) ;
	        al.add(v) ;
	        for (int i = 0 ; i < min(al.c,slen) ; i += 1) {
	            op->state[i] += al.v[i] ;
	        } /* end for */
	    } /* emd if (addtime) */
	} /* end if (mkprocrand) */
	return rs ;
} /* end subroutine (randomvar_initreal) */

local int randomvar_maint(randomvar *op) noex {
	int		rs ;
	if ((rs = randomvar_swapone(op)) >= 0) ylikely {
	    op->maintcount = 0 ;
	    if (! op->fl.pseudo) {
	        TIMEVAL		tv ;
	        gettimeofday(&tv,nullptr) ;
	        if ((tv.tv_sec - op->laststir) >= RANDOMVAR_STIRINT) {
	            op->laststir = tv.tv_sec ;
	            addtime((op->state+0),&tv) ;
	        }
	    } /* end if (not-pseudo) */
	} /* end if (swapone) */
	return rs ;
} /* end subroutine (randomvar_maint) */

local int randomvar_swapone(randomvar *op) noex {
	const ulong	one = op->state[0] ;
	uint		s0, s1 ;
	uint		t0, t1 ;
	int		rs = SR_OK ;
	s0 = uint(one >> 00) ;
	s1 = uint(one >> 32) ;
	t0 = randlc(s0) ;
	t1 = randlc(s1) ;
	{
	    const ulong	one0 = ulong(t1) ;	/* swapped */
	    const ulong	one1 = ulong(t0) ;	/* swapped */
	    op->state[0] = ((one1 << 32) | (one0 << 00)) ;
	}
	return rs ;
} /* end subroutine (randomvar_swapone) */

static void addtime(ulong *lp,TIMEVAL *tvp) noex {
	const ulong	cur = *lp ;
	const uint	t0 = uint(tvp->tv_sec) ;
	const uint	t1 = uint(tvp->tv_usec) ;
	uint		c0, c1, n0, n1 ;
	c0 = uint(cur >> 0) ;
	c1 = uint(cur >> 32) ;
	n0 = c0 ^ randlc(c0 ^ t0) ;
	n1 = c1 ^ randlc(c1 ^ t1) ;
	{
	    const ulong	ln0 = ulong(n0) ;
	    const ulong	ln1 = ulong(n1) ;
	    ulong	stage ;
	    stage = ((ln1 << 32) | ln0) ;
	    *lp = stage ;
	}
} /* end subroutine (addtime) */

local int rdulong(cchar *sp,int sl,ulong *lp) noex {
	int		r = 0 ;
	if (sl > 0) {
	    cint	mlen = min(sl,digsize) ;
	    ulong	ulw = 0 ;
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
} /* end subroutine (rdulong) */

local int wrulong(char *bp,int bl,ulong ulw) noex {
	cint		n = digsize ;
	int		i = 0 ; /* used afterwards */
	for (i = 0 ; (i < n) && (i < bl) ; i += 1) {
	    *bp = char(ulw) ; ulw >>= 8 ;
	} /* end for */
	return i ;
} /* end subroutine (wrulong) */

/* this extracts process-constant randoness (done only once) */
local int mkprocrand() noex {
	uint		uv ;
	int		rs = SR_OK ;
	int		c = 0 ;
	cchar		*var = getenv(varname.random) ;
	initrv.v[c++] = (uv = getpid(),randlc(uv)) ;
	initrv.v[c++] = (uv = getuid(),randlc(uv)) ;
	initrv.v[c++] = (uv = getppid(),randlc(uv)) ;
	initrv.v[c++] = (uv = getpgrp(),randlc(uv)) ;
	initrv.v[c++] = (uv = getsid(0),randlc(uv)) ;
	if (var) {
	    if (int v{} ; (rs = cfdec(var,-1,&v)) >= 0) {
		initrv.v[c++] = uint(v) ;
	    } else if (isNotValid(rs)) {
		rs = SR_OK ;
	    }
	}
	return (rs >= 0) ? c : rs ;
} /* end subroutine (mkprocrand) */

int randomvar_st::operator () (int fpseudo,uint seed) noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    switch (w) {
	    case 0:
	        rs = randomvar_start(op,fpseudo,seed) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
} /* end method (randomvar_st::operator) */

randomvar_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case randomvarmem_finish:
	        rs = randomvar_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
} /* end method (randomvar_co::operator) */

int randomvar::stateload(cchar *ms,int ml) noex {
    	return randomvar_stateload(this,ms,ml) ;
} /* end method */

int randomvar::statesave(char *msp,int msl) noex {
    	return randomvar_statesave(this,msp,msl) ;
} /* end method */

int randomvar::setpoly(int µa,int µb) noex {
    	return randomvar_setpoly(this,µa,µb) ;
} /* end method */

int randomvar::addnoise(cvoid *nbuf,int nlen) noex {
    	return randomvar_addnoise(this,nbuf,nlen) ;
} /* end method */

int randomvar::getlong(long *slp) noex {
    	return randomvar_getlong(this,slp) ;
} /* end method */

int randomvar::getulong(ulong *ulp) noex {
    	return randomvar_getulong(this,ulp) ;
} /* end method */

int randomvar::getint(int *sip) noex {
    	return randomvar_getint(this,sip) ;
} /* end method */

int randomvar::getuint(uint *uip) noex {
    	return randomvar_getuint(this,uip) ;
} /* end method */

int randomvar::get(void *rbuf,int rlen) noex {
    	return randomvar_get(this,rbuf,rlen) ;
} /* end method */

int randomvar::get(long *slp) noex {
    	return randomvar_getlong(this,slp) ;
} /* end method */

int randomvar::get(ulong *ulp) noex {
    	return randomvar_getulong(this,ulp) ;
} /* end method */

int randomvar::get(int *sip) noex {
    	return randomvar_getint(this,sip) ;
} /* end method */

int randomvar::get(uint *uip) noex {
    	return randomvar_getuint(this,uip) ;
} /* end method */

void randomvar::dtor() noex {
	if (cint rs = int(finish) ; rs < 0) {
	    ulogerror("randomvar",rs,"fini-finish") ;
	}
} /* end method */



/* cfdecmf SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* Convert-From-Decimal-Multiply-Factor */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-17, David A­D­ Morano
	This code was originally written.

	= 2011-03-23, David A­D­ Morano
	I folded some constants (power of 1024) into an array for
	later reference use.  Admittedly this was not a big deal.

	= 2025-12-04, David A­D­ Morano
	I added the check for multiply overflow (see subroutine-template
	|cfdecmfx()| below) using the C++26 |ckd_mul(3c++)| subroutine.
	Yes, I got to use the check subroutine early due to its
	early implementation in GCC-14 (or GCC-15).  Actually, it
	was available some time ago now I think.  Note that the
	subroutine |ckd_maul(3c++)| is *not* in the 'std' namespace!
	Yes, a little bit weird.

*/

/* Copyright © 1998,2011,2025 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Names:
	cfdecmfi
	cfdecmfl
	cfdecmfll
	cfdecmfui
	cfdecmful
	cfdecmfull

	Description:
	These are subroutines to convert decimal strings to binary
	integer value but with optional multiplying factors (letters)
	attached to the end of the given string.  The value represented
	by the string (without the multiplying factor) is multiplied
	by the multiplying factor.

	multiply-factor		value
	-------------------------------

	b	block		512
	d	deca		10
	h	hecto		100
	k	kila		1024
	m	mega		1024*1024
	g	giga		1024*1024*1024
	t	tera		1024*1024*1024*1024
	p	peta		1024*1024*1024*1024*1024
	e	exa		1024*1024*1024*1024*1024*1024
	kb	kilo-blocks	1024*512
	mb	mega-blocks	1024*1024*512

	Synopsis:
	int cfdecmf{x}(cchar *sbuf,int slen,{x} *rp) noex

	Arguments:
	{x}		i = |int|, l = |long|, ll = |longlong|
	{x}		ui = |int|, ul = |long|, ull = |longlong|
	sbuf		source string pointer
	slen		source string length
	rp		point to receive result

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* |UCHAR_MAX| + |CHAR_BIT| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstdint>		/* |int64_t| */
#include	<cstdckdint>		/* |ckd_mul(3c++)| (global namespace) */
#include	<utility>		/* |in_range(3c++)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<intsat.h>
#include	<ascii.h>
#include	<cfdec.h>
#include	<six.h>			/* |sialpha(3uc)| + |sichr(3uc)| */
#include	<rmx.h>			/* |rmwht(3uc)| */
#include	<char.h>		/* |CHAR_ISWHITE(3uc)| */
#include	<toxc.h>		/* |tolc(3uc)| */
#include	<stdintx.h>		/* |int128_t| */
#include	<localmisc.h>

#include	"cfdecmf.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

constexpr char		xfacts[] = " kmgtpe" ;

constexpr int		nfacts = lenstr(xfacts) ;

namespace {
    struct efactors {
	int64_t		v[nfacts] ;
	consteval efactors() noex ;
    } ;  /* end struct (efactors) */
} /* end namespace */


/* forward references */

local int	getmf(cchar *,int,int64_t *) noex ;

local inline bool ourwht(cchar c) noex {
    	cint ch = (c & UCHAR_MAX) ;
    	return (CHAR_ISWHITE(ch) || (ch == CH_NBSP)) ;
} /* end subroutine (ourwht) */

template<typename T> local int inrange(int64_t v) noex {
    	cint		nbx = szof(int64_t) ;
    	cint		nb = szof(T) ;
    	int		rs = SR_OK ;
	if (nb <= nbx) {
	    if (nb >= 2) {
	        if (v >> ((nb * CHAR_BIT) - 1)) rs = SR_RANGE ;
	    } else {
	        rs = SR_DOMAIN ;
	    }
	} /* end if (testable size) */
    	return rs ;
} /* end subroutine (inrange) */

template<typename T> local int cfdecmfx(cc *sp,int µsl,T *rp) noex {
	int		rs = SR_FAULT ;
	int		rv = 0 ; /* return-value */
	if (sp && rp) ylikely {
	    rs = SR_INVALID ;
	    if (int sl ; (sl = getlenstr(sp,µsl)) > 0) ylikely {
	        if (int64_t mf{} ; (rs = getmf(sp,sl,&mf)) > 0) ylikely {
	            cint	ml = rs ;
	            if ((rs = inrange<T>(mf)) >= 0) ylikely {
	                if (T v ; (rs = cfdec(sp,ml,&v)) >= 0) ylikely {
		            const T	mfv = (T) mf ;
		            if (T res{} ; (! ckd_mul(&res,v,mfv))) ylikely {
	                        if (rp) *rp = res ;
				rv = intsat(res) ; /* return-value */
		            } else {
			        rs = SR_RANGE ;
		            }
	                } /* end if cfdecx) */
	            } /* end if (inrange) */
	        } /* end if (getmf) */
	    } /* end if (getlenstr) */
	} /* end if (non-null) */
	return (rs >= 0) ? rv : rs ;
} /* end subroutine-template (cfdecmfx) */

consteval efactors::efactors() noex {
    	int64_t	val = 1 ;
	v[0] = val ;
	for (int i = 1 ; i < nfacts ; i += 1) {
	    val *= 1024L ;
	    v[i] = val ;
	} /* end for */
} ; /* end ctor (efactors) */


/* local variables */

constexpr efactors	ef ;

constexpr int		ubsz = 512 ;	/* UNIX® block size */


/* exported variables */


/* exported subroutines */

int cfdecmfi(cchar *sbuf,int slen,int *rp) noex {
	return cfdecmfx(sbuf,slen,rp) ;
} /* end subroutine (cfdecmfi) */

int cfdecmfl(cchar *sbuf,int slen,long *rp) noex {
	return cfdecmfx(sbuf,slen,rp) ;
} /* end subroutine (cfdecmfl) */

int cfdecmfll(cchar *sbuf,int slen,longlong *rp) noex {
	return cfdecmfx(sbuf,slen,rp) ;
} /* end subroutine (cfdecmfll) */

int cfdecmfui(cchar *sbuf,int slen,uint *rp) noex {
	return cfdecmfx(sbuf,slen,rp) ;
} /* end subroutine (cfdecmfui) */

int cfdecmful(cchar *sbuf,int slen,ulong *rp) noex {
	return cfdecmfx(sbuf,slen,rp) ;
} /* end subroutine (cfdecmful) */

int cfdecmfull(cchar *sbuf,int slen,ulonglong *rp) noex {
	return cfdecmfx(sbuf,slen,rp) ;
} /* end subroutine (cfdecmfull) */


/* local subroutines */

/* get the "multiply-factor" */
local int getmf(cchar *sbuf,int slen,int64_t *rp) noex {
	int64_t		mf = 1 ; /* return-result */
	int		rs = SR_INVALID ;
	int		rl = 0 ; /* return-value */
	if (int sl ; (sl = rmwht(sbuf,slen)) > 0) ylikely {
	    rs = SR_OK ;
	    if (int si ; (si = sialpha(sbuf,sl)) > 0) {
	        cint	chx = tolc(sbuf[si]) ;
		cchar	*xp = (sbuf + si) ;
		rl = si ; /* <- return-value */
		switch (cint xl = (sl - si) ; xl) {
		case 1:
	            if (chx == 'b') {
	                mf = ubsz ;
	            } else if (chx == 'd') {
	                mf = 10L ;
	            } else if (chx == 'h') {
	                mf = 100L ;
	            } else if ((si = sichr(xfacts,-1,chx)) > 0) {
	                mf = ef.v[si] ;
		    }
		    break ;
		case 2:
		    if (cint chy = tolc(xp[1]) ; chy == 'b') {
	                if (chx == 'k') {
	                    mf = 1024L * ubsz ;
	                } else if (chx == 'm') {
	                    mf = 1024L * 1024L * ubsz ;
	                } else {
		            rs = SR_NOTSUP ;
		        } /* end if */
	            } else {
		        rs = SR_NOTSUP ;
		    } /* end if */
		    break ;
		default:
		    rs = SR_NOTSUP ;
		    break ;
	        } /* end switch */
	    } /* end if (have extension) */
	} /* end if (non-zero positive) */
	*rp = mf ; /* <- return-result */
	return (rs >= 0) ? rl : rs ;
} /* end subroutine (getmf) */



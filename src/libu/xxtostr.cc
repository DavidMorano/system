/* xxtostr SUPPORT (X-To-String) */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* subroutines to convert an integer to a c-string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This code was originally written, based on code I wrote
	from the old days at the Labs.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Group:
	{xx}tostr

	Names:
	uxxtostr
	sxxtostr

	Description:
	The API of there subroutines is semi-standardized in the
	industry and I had nothing to do with it (whatsoever).  So
	we are just recreating the same API as is already out there
	in the wilderness.  These subroutine templates convert an
	integer (signed or unsigned of various sized types) into a
	c-string digit representation that is placed into the caller
	supplied buffer (of specified length).  A numerical base
	is also supplied by the caller and must be between 2 and
	64.  For number bases at or below 26, only upper-case
	hexadecimal digit charactes are supported (characters '0'
	through '9' and 'A' through 'Z').  Convert your resuling
	hexadecimal digit c-string to lower case afterwards if you
	want.  For bases between 27 and 62 inclusive, the resulting
	digits are in the range of the lower-case alpha characters
	'a' to 'z'.  Only the following integer types are supported:
		|int|		(32-bits), 
		|long|		(64-bits), 
		|longlong|	(128-bits),
       	and their associated unsigned cousins.

	Synopsis:
	template<typename UT>
	char *uxxtostr(char **endp,int b,UT v) noex
	template<typename UT,typename ST>
	char *sxxtostr(char **endp,int b,ST v) noex

	Arguments:
	UT		an unsgined type:	uint, ulong, ulonglong
	ST		a signed type:		int, long, longlong
	endp		pointer to one past the end of the supplied buffer
	b		number base
	v		integer value to be converted

	Returns:
	>=0		length of buffer used by the conversion
	<0		error in the conversion

	Names:
    	itostr
    	ltostr
    	lltostr
    	uitostr
    	ultostr
    	ulltostr

	Description:
	These subroutines (w/ C-language linkage) are supplied for
	base-10 operations.  The same integer types as above are
	supported.  As might be noted (by some astute observers)
	subroutines with these same names are supplied on (pretty
	much now historical) System V Release 4 based operating
	systems (an example of which was Solaris® and the more
	modern Illumos®).

	Synopsis:
    	char *itostr(int v,		char *endp) noex
    	char *ltostr(long v,		char *endp) noex
    	char *lltostr(longlong v,	char *endp) noex
    	char *uitostr(uint uv,		char *endp) noex
    	char *ultostr(ulong uv,		char *endp) noex
    	char *ulltostr(ulonglong uv,	char *endp) noex

	Arguments:
	v	signed value to convert
	uv	unsigned value to convert
	endp	the ending pointer position of a supplied buffer

	Notes:
	1. As it stands now, these subroutines do not perform any
	special tenchiques to try to make this process faster.
	These subroutines are, therefore, probably the slowest such
	conversions routinely available.  To really move (execute)
	quickly through the division-related aspects of the required
	algorithm, one would have to use assembly language where
	both the quotient and the reminder of a division are produced
	simultaneously (since each are needed to continue).  This,
	of course, assumes that the underlying machine architecture
	has such instructions.  But short of assembly (and and the
	required machine instructions) this present implemtnation
	is adequate.
	2. Just a note about the double division loop (one loop
	after the other) for the longer division operands below.
	This is an old trick from almost the beginning (1979 or
	1980 or so) to use the best built-in division operator given
	the division operands (dividend and divisor).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cerrno>		/* CSTD |errno| */
#include	<climits>		/* CSTD |ULONG_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<stdintx.h>		/* LIBU extended integers */
#include	<localmisc.h>		/* LIBU ?? not-needed ?? */

#include	"xxtostr.h"

#pragma		GCC dependency		"mod/digtab.ccm"

import digtab ;				/* |getdig(3u)| + |digtab_enc(3u)| */

/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

constexpr int		div100 = 100 ;
constexpr int		xxtostr_maxbase = digtab.maxbase ; /* standard value */

local inline char getdig_lo(int) noex ;
local inline char getdig_hi(int) noex ;

template<typename UT>
constexpr int uxxtostr(char *endp,int b,UT v) noex {
    int             rs = SR_FAULT ;
    char            *rp = endp ;
    if (endp) ylikely {
        rs = SR_INVALID ;
        *rp = '\0' ;
        if ((b >= 2) && (b <= xxtostr_maxbase)) ylikely {
            const uint ub = uint(b) ;
	    rs = SR_OK ;
            if (v != 0) ylikely {
                int di ;
                if_constexpr (szof(UT) > szof(ulong)) {
                    const UT vmask = (compl UT(ULONG_MAX)) ;
                    for (UT nv ; v & vmask ; v = nv) {
                        nv = v / ub ;
                        di = int(v % ub) ;
                        *--rp = digtab_enc(di) ;
                    } /* end for (slower) */
                    for (ulong nlv, lv = ulong(v) ; lv ; lv = nlv) {
                        nlv = lv / ub ;
                        di = int(lv % ub) ;
                        *--rp = digtab_enc(di) ;
                    } /* end for (faster) */
                } else {
                    for (UT nv ; v ; v = nv) {
                        nv = v / ub ;
                        di = int(v % ub) ;
                        *--rp = digtab_enc(di) ;
                    } /* end for (regular) */
                } /* end if_constexpr (size-of-operand) */
            } else {
                *--rp = '0' ;
            } /* end if */
        } /* end if (base supported) */
    } /* end if (non-null) */
    return (rs >= 0) ? intconv(endp - rp) : rs ;
} /* end subroutine-template (uxxtostr) */

template<typename UT,typename ST>
constexpr int sxxtostr(char *endp,int b,ST v) noex {
    UT              uv = (UT) v ;
    int             rs = SR_FAULT ;
    char            *rp = nullptr ;
    if (v < 0) uv = (neg uv) ;
    if (endp) ylikely {
        if ((rs = uxxtostr(endp,b,uv)) >= 0) {
            rp = (endp - rs) ;
            if (v < 0) *--rp = '-' ;
        }
    } /* end if (non-null) */
    return (rs >= 0) ? intconv(endp - rp) : rs ;
} /* end subroutine (sxxtostr) */

template<typename UT>
constexpr int uxxtostr10(char *endp,UT v) noex {
    constexpr uint	udiv100	= uint(div100) ;
    int             rs = SR_FAULT ;
    char            *rp = endp ;
    if (endp) ylikely {
        rs = SR_OK ;
        *rp = '\0' ;
        if (v != 0) ylikely {
            int di ;
            if_constexpr (szof(UT) > szof(ulong)) {
                const UT vmask = (compl UT(ULONG_MAX)) ;
                for (UT nv ; v & vmask ; v = nv) {
                    nv = v / udiv100 ;
                    di = int(v % udiv100) ;
                    *--rp = getdig_lo(di) ;
                    *--rp = getdig_hi(di) ;
                } /* end for (slower) */
                for (ulong nlv, lv = ulong(v) ; lv ; lv = nlv) {
                    nlv = lv / udiv100 ;
                    di = int(lv % udiv100) ;
                    *--rp = getdig_lo(di) ;
                    *--rp = getdig_hi(di) ;
                } /* end for (faster) */
            } else {
                for (UT nv ; v ; v = nv) {
                    nv = v / udiv100 ;
                    di = int(v % udiv100) ;
                    *--rp = getdig_lo(di) ;
                    *--rp = getdig_hi(di) ;
                } /* end for (regular) */
            } /* end if_constexpr (size-of-operand) */
	    if ((rs = int(endp - rp)) > 1) {
		while ((rs > 1) && (*rp == '0')) {
		    rp += 1 ;
		    rs -= 1 ;
		} /* end while */
	    } /* end if */
        } else {
            *--rp = '0' ;
        } /* end if */
    } /* end if (non-null) */
    return (rs >= 0) ? intconv(endp - rp) : rs ;
} /* end subroutine-template (uxxtostr10) */

template<typename UT,typename ST>
constexpr int sxxtostr10(char *endp,ST v) noex {
    UT              uv = (UT) v ;
    int             rs = SR_FAULT ;
    char            *rp = nullptr ;
    if (v < 0) uv = (neg uv) ;
    if (endp) ylikely {
        if ((rs = uxxtostr10(endp,uv)) >= 0) {
            rp = (endp - rs) ;
            if (v < 0) *--rp = '-' ;
        }
    } /* end if (non-null) */
    return (rs >= 0) ? intconv(endp - rp) : rs ;
} /* end subroutine (sxxtostr10) */

template<typename UT,typename T> 
constexpr char *stostr(T v,char *endp,int b) noex {
    int             rs ;
    char            *rp = nullptr ;
    cauto sxx = [&endp,&b] (cauto &vv) -> int {
        int rsl ;
        if (b == 10) {
            rsl = sxxtostr10<UT>(endp,vv) ;
        } else {
            rsl = sxxtostr<UT>(endp,b,vv) ;
        }
        return rsl ;
    } ; /* end lambda (sxx) */
    if ((rs = sxx(v)) >= 0) ylikely {
        rp = (endp - rs) ;
    } else {
        errno = (neg rs) ;
    }
    return (rs >= 0) ? rp : nullptr ;
} /* end subroutine (stostr) */

template<typename UT> 
constexpr char *utostr(UT uv,char *endp,int b) noex {
    char            *rp = nullptr ;
    int             rs ;
    cauto uxx = [&endp,&b] (cauto &vv) -> int {
        int rsl ;
        if (b == 10) {
            rsl = uxxtostr10<UT>(endp,vv) ;
        } else {
            rsl = uxxtostr<UT>(endp,b,vv) ;
        }
        return rsl ;
    } ; /* end lambda (uxx) */
    if ((rs = uxx(uv)) >= 0) ylikely {
        rp = (endp - rs) ;
    } else {
        errno = (neg rs) ;
    }
    return (rs >= 0) ? rp : nullptr ;
} /* end subroutine (utostr) */


/* local variables */

constexpr int		b10 = 10 ;


/* exported variables */


/* exported subroutines */

char *itostr	(int v,         char *endp)     	noex {
    return stostr<uint>(v,endp,b10) ;
} /* end */
char *ltostr	(long v,        char *endp)     	noex {
    return stostr<ulong>(v,endp,b10) ;
} /* end */
char *lltostr	(longlong v,    char *endp)     	noex {
    return stostr<ulonglong>(v,endp,b10) ;
} /* end */
char *uitostr	(uint uv,       char *endp)     	noex {
    return utostr(uv,endp,b10) ;
} /* end */
char *ultostr	(ulong uv,      char *endp)     	noex {
    return utostr(uv,endp,b10) ;
} /* end */
char *ulltostr	(ulonglong uv,  char *endp)     	noex {
    return utostr(uv,endp,b10) ;
} /* end */

char *itostr	(int v,         char *endp,int b)	noex {
    return stostr<uint>(v,endp,b) ;
} /* end */
char *ltostr	(long v,        char *endp,int b)	noex {
    return stostr<ulong>(v,endp,b) ;
} /* end */
char *lltostr	(longlong v,    char *endp,int b)	noex {
    return stostr<ulonglong>(v,endp,b) ;
} /* end */
char *uitostr	(uint uv,       char *endp,int b)	noex {
    return utostr(uv,endp,b) ;
} /* end */
char *ultostr	(ulong uv,      char *endp,int b)	noex {
    return utostr(uv,endp,b) ;
} /* end */
char *ulltostr	(ulonglong uv,  char *endp,int b)	noex {
    return utostr(uv,endp,b) ;
} /* end */


/* local subroutines */

local char getdig_lo(int r) noex {
        return digtab.dig100[r][0] ;
} /* end subroutine */

local char getdig_hi(int r) noex {
        return digtab.dig100[r][1] ;
} /* end subroutine */



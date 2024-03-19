/* digbufsizes HEADER */
/* lang=C++20 */

/* provide buffer sizes for c-string digits */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	DIGBUFSIZES_INCLUDE
#define	DIGBUFSIZES_INCLUDE
#ifdef	__cplusplus /* everything is C++ only */


#include	<envstandards.h>	/* MUST be first to configure */
#include	<bit>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<stdintx.h>
#include	<valuelims.hh>


extern const valuelimits	valuelimit ;


constexpr int	digbufsizes_maxbase = 64 ;	/* current max-base */

constexpr int	digbufsizes_ntypes = 10 ;	/* up to types w/ 1024 bits */
constexpr int	digbufsizes_nbases = (digbufsizes_maxbase+1) ;

template<typename T>
constexpr int ndigitx(T v,int b) noex {
	const uint	ub(b) ;	/* unsigned division faster than signed */
	int		n = 0 ;
	if (b >= 2) {
	    if (v == 0) {
		n = 1 ;
	    } else {
	        while (v != 0) {
	            n += 1 ;
	            v = (v / ub) ;
	        } /* end while */
	    } /* end if */
	} /* end if (valid base) */
	return n ;
}
/* end subroutine-template (xdigitx) */

constexpr int ndigits(int ntype,int base) noex {
	int	n = 0 ;
	if (base >= 2) {
	    switch (ntype) {
	    case 0:
		{
		    uchar	uc = 0xFF ;
		    n = ndigitx(uc,base) ;
		}
		break ;
	    case 1:
		{
		    ushort	us = 0xFFFF ;
		    n = ndigitx(us,base) ;
		}
		break ;
	    case 2:
		{
		    uint	ui = 0xFFFFFFFFU ;
		    n = ndigitx(ui,base) ;
		}
		break ;
	    case 3:
		{
		    ulong	ul = 0xFFFFFFFFFFFFFFFFUL ;
		    n = ndigitx(ul,base) ;
		}
		break ;
	    case 4:
		{
		    ulonglong	ull = valuelimit.ullmax ;
		    n = ndigitx(ull,base) ;
		}
		break ;
	    case 5:
	        switch (base) {
		case 2:
		    n = 256 ;
		    break ;
		case 4:
		    n = 128 ;
		    break ;
		case 8:
		    n = 64 ;
		    break ;
		case 16:
		    n = 32 ;
		    break ;
		case 32:
		    n = 16 ;
		    break ;
		case 256:
		    n = 8 ;
		    break ;
		default:
		    n = ((1 << ntype) * 8) ;
		    break ;
		} /* end switch */
		break ;
	    default:
		n = ((1 << ntype) * 8) ;
		break ;
	    } /* end switch */
	} /* end if (valid base) */
	return n ;
} /* end subroutine (ndigits) */

struct digbufsizes {
	cint		nbases = digbufsizes_nbases ;
	cint		ntypes = digbufsizes_ntypes ;
	ushort		bufsize[digbufsizes_ntypes][digbufsizes_nbases] ;
	constexpr digbufsizes() noex {
	    for (int b = 0 ; b < digbufsizes_nbases ; b += 1) {
	        for (int t = 0 ; t < digbufsizes_ntypes ; t += 1) {
		    bufsize[t][b] = (ndigits(t,b) + 1) ; /* plus sign char */
	        }
	    }
	} ; /* end constructor (digbufsizes) */
} ; /* end subroutine (digbufsizes) */


#endif	/* __cplusplus */
#endif /* DIGBUFSIZES_INCLUDE */



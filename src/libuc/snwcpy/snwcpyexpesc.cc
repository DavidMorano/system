/* wnwcpyexpesc SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* copy a source string to a destination while expanding C-escapes */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-08, David A­D­ Morano
	This was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	wnwcpyexpesc

	Description:
	This is a copy-translate type of operation where we copy
	source string characters to the destination while intercepting
	C-language escape sequence and convert them to the characters
	that they are supposed to represent (in binary).

	Synopsis:
	int wnwcpyexpesc(char *dp,int dl,cchar *sp,int sl) noex

	Arguments:
	dp		destination string buffer
	dl		destination string buffer length
	sp		source string
	sl		source string length

	Returns:
	>=0		number of bytes in result
	<0		error (system-return)

	See-also:
	snwcpy(3uc),
	snwcpylatin(3uc), 
	snwcpyopaque(3uc), 
	snwcpycompact(3uc), 
	wnwcpyexpesc(3uc), 
	snwcpyhyphen(3uc), 

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<ascii.h>
#include	<storebuf.h>
#include	<strn.h>		/* |strnchr(3uc)| */
#include	<cfx.h>			/* |cf{xxx}(3uc)| */
#include	<six.h>			/* |isnon{xxx}(3uc)| */
#include	<mkchar.h>
#include	<ischarx.h>
#include	<localmisc.h>

#include	"snwcpyx.h"

#pragma		GCC dependency	"mod/libutil.ccm"

import libutil ;

/* local defines */


/* imported namespaces */


/* local typedefs */

extern "C" {
    typedef int (*cfx_f)(cchar *,int,int *) noex ;
    typedef int (*sinonx_f)(cchar *,int) noex ;
}


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct expmgr {
	storebuf	sb ;
	cchar		*sp ;
	int		sl ;
	expmgr(char *dp,int dl,cc *asp,int asl) noex : sb(dp,dl) {
	    sp = asp ;
	    sl = asl ;
	} ; /* end ctor */
	operator int () noex ;
	int handle() noex ;
	int handle_octal() noex ;
	int handle_o() noex ;
	int handle_x() noex ;
	int handle_u() noex ;
	int handle_U() noex ;
	int handle_N() noex ;
	int storeval(cfx_f,cc *,int) noex ;
    } ; /* end struct (expmgr) */
} /* end namespace */


/* forwards references */

static int siourx(sinonx_f sio,cchar *sp,int sl) noex {
    	int		rs ;
	int		si = 0 ; /* return-value */
	if ((rs = sio(sp,sl)) >= 0) {
	    si = rs ;
	} else {
	    si = sl ;
	}
	return (rs >= 0) ? si : rs ;
} /* end subroutine (siourx) */


/* local variables */

constexpr cint		chx_sub = mkchar('¿') ;


/* exported variables */


/* exported subroutines */

int snwcpyexpesc(char *dbuf,int dlen,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	if (dbuf && sp) {
	    if (sl < 0) sl = lenstr(sp) ;
	    {
	        expmgr eo(dbuf,dlen,sp,sl) ;
	        rs = eo ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (snwcpyexpesc) */


/* local subroutine */

expmgr::operator int () noex {
    	cnullptr	np{} ;
    	cint		chx = CH_SLASH ;
    	int		rs = SR_OK ;
	for (cc *tp ; (rs >= 0) && ((tp = strnchr(sp,sl,chx)) != np) ; ) {
	    cint tl = intconv(tp - sp) ;
	    if ((rs = sb.strw(sp,tl)) >= 0) {
		sl -= (tl + 1) ;
		sp += (tl + 1) ;
		rs = handle() ;
	    }
	} /* end for */
	if (rs >= 0) rs = sb.idx ;
    	return rs ;
} /* end method (expmgr::operator) */

int expmgr::handle() noex {
    	int		rs = SR_OK ;
	if (sl && *sp) {
	    int	chs = 0 ;
	    switch (cint ch = mkchar(*sp++) ; (sl-- , ch)) {
	    case 'a':
		chs = CH_BELL ;
		break ;
	    case 'b':
		chs = CH_BS ;
		break ;
	    case 'f':
		chs = CH_FF ;
		break ;
	    case 'n':
		chs = CH_NL ;
		break ;
	    case 'r':
		chs = CH_CR ;
		break ;
	    case 't':
		chs = CH_TAB ;
		break ;
	    case 'v':
		chs = CH_VT ;
		break ;
	    case 'e':
		chs = CH_ESC ;
		break ;
	    case '0' ... '7':
		rs = handle_octal() ;
		break ;
	    case 'o':
		rs = handle_o() ;
		break ;
	    case 'x':
		rs = handle_x() ;
		break ;
	    case 'u':
		rs = handle_u() ;
		break ;
	    case 'U':
		rs = handle_U() ;
		break ;
	    case 'N':
		rs = handle_N() ;
		break ;
	    case CH_APOSTROPHE:
	    case CH_DQUOTE:
	    case CH_BSLASH:
	    case '?':
		chs = ch ;
		break ;
	    default:
		chs = chx_sub ;
		break ;
	    } /* end switch */
	    if ((rs >= 0) && chs) {
		rs = sb.chr(chs) ;
	    }
	} /* end if (has another character) */
    	return rs ;
} /* end method (expmgr::handle) */

int expmgr::handle_octal() noex {
    	int		rs ;
	if ((rs = siourx(sinonoct,sp,sl)) >= 0) {
	    if (cint n = (rs + 1) ; (rs = storeval(cfoct,(sp - 1),n)) >= 0) {
	        sp += (n - 1) ;
	        sl -= (n - 1) ;
	    }
	} /* end if (sioutx) */
	return rs ;
} /* end method (expmgr::handle_octal) */

int expmgr::handle_o() noex {
    	cnullptr	np{} ;
    	int		rs = SR_OK ;
	if (sl > 0) {
	    if (*sp == CH_LBRACE) {
		( sp += 1 , sl - 1 ) ;
		if (cc *tp ; (tp = strnchr(sp,sl,CH_RBRACE)) != np) {
		    cint tl = intconv(tp - sp) ;
		    if ((rs = storeval(cfoct,sp,tl)) >= 0) {
			sl -= (tl + 1) ;
			sp += (tl + 1) ;
		    }
		} else {
	            rs = sb.chr(chx_sub) ;
		}
	    } else {
	        rs = sb.chr(chx_sub) ;
	    }
	} else {
	    rs = sb.chr(chx_sub) ;
	}
    	return rs ;
} /* end method (expmgr::handle_o) */

int expmgr::handle_x() noex {
    	cnullptr	np{} ;
    	int		rs = SR_OK ;
	if (sl > 0) {
	    if (*sp == CH_LBRACE) {
		( sp += 1 , sl - 1 ) ;
		if (cc *tp ; (tp = strnchr(sp,sl,CH_RBRACE)) != np) {
		    cint tl = intconv(tp - sp) ;
		    if ((rs = storeval(cfhex,sp,tl)) >= 0) {
			sl -= (tl + 1) ;
			sp += (tl + 1) ;
		    }
		} else {
	            rs = sb.chr(chx_sub) ;
		}
	    } else {
		if ((rs = siourx(sinonhex,sp,sl)) > 0) {
		    if (cint si = rs ; (rs = storeval(cfhex,sp,si)) >= 0) {
			sl -= si ;
			sp += si ;
		    }
		} else {
	            rs = sb.chr(chx_sub) ;
		}
	    } /* end if (braces or not) */
	} else {
	    rs = sb.chr(chx_sub) ;
	}
    	return rs ;
} /* end method (expmgr::handle_x) */

int expmgr::handle_u() noex {
    	return SR_NOSYS ;
}

int expmgr::handle_U() noex {
    	return SR_NOSYS ;
}

int expmgr::handle_N() noex {
    	return SR_NOSYS ;
}

int expmgr::storeval(cfx_f cfx,cc *op,int ol) noex {
    	int		rs ;
	if (int v ; (rs = cfx(op,ol,&v)) >= 0) {
	    rs = sb.chr(v) ;
	}
	return rs ;
} /* end method (expmgr::storeval) */



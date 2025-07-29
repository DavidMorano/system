/* snxxx SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* specialized x-string formatting */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	snxxx

	Description:
	These subroutines construct a single c-string from three
	concatenated component c-strings.  The first and last
	component can be a string or a decimal digit.  The middle
	component can be elided, or some specified character.

	Synopsis:
	int snxxx(char *dbuf,int dlen,X1 x1,X2 x2) noex

	Arguments:
	dbuf		destination buffer poinster
	dlen 		destination buffer length
	x1		of type X1=|char *| or X1=|uint|
	x2		of type X2=|char *| or X2=|uint|

	Returns:
	>=0		length of created string in destination
	<0		error (system-return)

	See-also:
	snfsflags(3uc)
	snopenflags(3uc)
	snpollflags(3uc)
	snxtilook(3uc)
	sninetaddr(3uc)
	snsigabbr(3uc)
	snabbr(3uc)
	snshellunder(3uc)
	snfilemode(3uc)
	sntid(3uc)
	snerrabbr(3uc)
	snrealname(3uc)
	snloadavg(3uc)
	snkeyval(3uc)
	snwvprintf(3uc)
	snwprintf(3uc)
	snkeyval(3uc)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<storebuf.h>
#include	<localmisc.h>

#include	"snxxx.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct snxxx {
	cchar	*s1 = nullptr ;
	cchar	*s2 = nullptr ;
	uint	d1 = 0 ;
	uint	d2 = 0 ;
	int	mch = 0 ;
	int operator () (char *,int) noex ;
    } ; /* end struct (snxxx) */
} /* end namespace */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int snsdd(char *dbuf,int dlen,cchar *s1,uint d2) noex {
	snxxx		sxo ;
	sxo.s1 = s1 ;
	sxo.mch = '.' ;
	sxo.d2 = d2 ;
	return sxo(dbuf,dlen) ;
}

int snddd(char *dbuf,int dlen,uint d1,uint d2) noex {
	snxxx		sxo ;
	sxo.d1 = d1 ;
	sxo.mch = '.' ;
	sxo.d2 = d2 ;
	return sxo(dbuf,dlen) ;
}

int snses(char *dbuf,int dlen,cchar *s1,cchar *s2) noex {
	snxxx		sxo ;
	sxo.s1 = s1 ;
	sxo.mch = '=' ;
	sxo.s2 = s2 ;
	return sxo(dbuf,dlen) ;
}

int snscs(char *dbuf,int dlen,cchar *s1,cchar *s2) noex {
	snxxx		sxo ;
	sxo.s1 = s1 ;
	sxo.mch = ':' ;
	sxo.s2 = s2 ;
	return sxo(dbuf,dlen) ;
}

int snsds(char *dbuf,int dlen,cchar *s1,cchar *s2) noex {
	snxxx		sxo ;
	sxo.s1 = s1 ;
	sxo.mch = '.' ;
	sxo.s2 = s2 ;
	return sxo(dbuf,dlen) ;
}

int snsd(char *dbuf,int dlen,cchar *s1,uint d2) noex {
	snxxx		sxo ;
	sxo.s1 = s1 ;
	sxo.mch = 0 ;
	sxo.d2 = d2 ;
	return sxo(dbuf,dlen) ;
}

int snchrs(char *dp,int dl,int ch,int n) noex {
    	int		rs = SR_FAULT ;
	int		rl = 0 ;
	if (dp) ylikely {
	    rs = SR_INVALID ;
	    if (n >= 0) ylikely {
		if (storebuf sb(dp,dl) ; (rs = sb.chrs(ch,n)) >= 0) ylikely {
		    rl = sb.idx ;
		} /* end if (storebuf) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (snchrs) */


/* local subroutines */

int snxxx::operator () (char *dbuf,int dlen) noex {
	int		rs = SR_FAULT ;
	int		rl = 0 ;
	if (dbuf) ylikely {
	    storebuf	sb(dbuf,dlen) ;
	    rs = SR_OK ;
	    if (rs >= 0) ylikely {
	        if (s1) {
	            rs = sb.str(s1) ;
	        } else {
	            rs = sb.dec(d1) ;
	        }
	    }
	    if ((rs >= 0) && mch) ylikely {
	        rs = sb.chr(mch) ;
	    }
	    if (rs >= 0) ylikely {
	        if (s2) {
	            rs = sb.str(s2) ;
	        } else {
	            rs = sb.dec(d2) ;
	        }
	    }
	    rl = sb.idx ;
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
}
/* end method (snxxx::operator) */



/* snxxx SUPPORT */
/* lang=C++20 */

/* specialized x-string formatting */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was originally written.

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

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<usystem.h>
#include	<storebuf.h>

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
    } ;
}


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


/* local subroutines */

int snxxx::operator () (char *dbuf,int dlen) noex {
	int		rs = SR_FAULT ;
	int		i = 0 ;
	if (dbuf) {
	    rs = SR_OK ;
	    if (rs >= 0) {
	        if (s1) {
	            rs = storebuf_strw(dbuf,dlen,i,s1,-1) ;
	            i += rs ;
	        } else {
	            rs = storebuf_decui(dbuf,dlen,i,d1) ;
	            i += rs ;
	        }
	    }
	    if ((rs >= 0) && mch) {
	        rs = storebuf_char(dbuf,dlen,i,mch) ;
	        i += rs ;
	    }
	    if (rs >= 0) {
	        if (s2) {
	            rs = storebuf_strw(dbuf,dlen,i,s2,-1) ;
	            i += rs ;
	        } else {
	            rs = storebuf_decui(dbuf,dlen,i,d2) ;
	            i += rs ;
	        }
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? i : rs ;
}
/* end method (snxxx::operator) */



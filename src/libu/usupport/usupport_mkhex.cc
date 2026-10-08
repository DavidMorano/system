/* susupport_mkhex SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* make a string of hexadecimal digits */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-08-15, David A­D­ Morano
	This was written to debug the REXEC program.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	mkhex{x}

	Names:
	mkhexstr

	Description:
	This module provides subroutines that create strings of
	hexadecimal digits.

	Synopsis:
	int mkhexstr(char *dbuf,int dlen,cvoid *vdp,int vdl) noex

	Arguments:
	dbuf		destination buffer pointer
	dlen		destination buffer legnth
	vdp		source data pointer
	vdl		source data length

	Returnrs:
	>=0		number of bytes in result buffer
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<mkchar.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"usupport_mkhex.hh"

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/digtab.ccm"

import libutil ;			/* |lenstr(3u)| */
import digtab ;				/* |getdig(3u)| */

/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

#ifdef	COMMENT
local char	getdig(int v) noex ;
#endif /* COMMENT */


/* local variables */

cint		b10	= 10 ;
cint		b16	= 16 ;
cint		twodig	= 2 ;


/* exported variables */


/* exported subroutines */

namespace libu {
    int mkhexstr(char *dbuf,int dlen,cvoid *vdp,int vdl) noex {
    	int		rs = SR_FAULT ;
	int		j = 0 ; /* return-value */
	if (dbuf && vdp) ylikely {
	    ccharp	sp = ccharp(vdp) ;
	    rs = SR_INVALID ;
	    dbuf[0] = '\0' ;
	    if (dlen >= 0) ylikely {
		cint	m = (b16 - 1) ;
	        cint	sl = (vdl < 0) ? lenstr(sp) : vdl ;
	        rs = SR_OK ;
	        for (int i = 0 ; (dlen >= twodig) && (i < sl) ; i += 1) {
	            cint ch = mkchar(sp[i]) ;
	            dbuf[j++] = getdig((ch>>4)& m) ;
	            dbuf[j++] = getdig((ch>>0)& m) ;
	            dlen -= twodig ;
	        } /* end for */
	        dbuf[j] = '\0' ;
		if (j < (sl * twodig)) rs = SR_OVERFLOW ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? j : rs ;
    } /* end subroutine (mkhexstr) */
} /* end namespace (libu) */


/* local subroutines */

#ifdef	COMMENT
local char getdig(int v) noex {
    	char	c = '¿' ;
	if ((v >= 0) && (v < b10)) {
	    c = '0' + char(v) ;
	} else if ((v >= b10) && (v < b16)) {
	    c = 'a' + char(v) ;
	} /* end if */
	return c ;
} /* end subroutine (getdig) */
#endif /* COMMENT */



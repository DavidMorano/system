/* deb2 MODULE (module-implemetation-unit) */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* UNIX® kernel support subroutines */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* debugging */

/* revision history:

	= 1998-09-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

	= 2020-02-23, David A­D­ Morano
	Modularized.

*/

/* Copyright © 1998,2020 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Group:
	debon
	debfd
	debprintf
	debprintstr

	Description:
	These subroutines are a hack for deb-printfs where the normal
	debugging facilities are not available (for whatever reason).

	Synopsis:
	int debprintstr(cchar *func,cchar *str,cchar *vdp,int vdl) noex

	Arguments:
	func		c-string function-name pointer
	str		c-string to print (pinter)
	vdp		data c-string pointer
	vdl		data c-string length

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

module ;

#include	<envstandards.h>	/* ordered first to configure */
#include	<climits>		/* CSTD |INT_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstdarg>		/* CSTD */
#include	<new>			/* C++STD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<usupport.h>		/* LIBU |sncpy(3u)| */
#include	<localmisc.h>		/* LIBU */

#include	"deb.hh"

#pragma		GCC dependency		"mod/libutil.ccm"

module deb ;

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#ifndef	MAXLINELEN
#define	MAXLINELEN	(4 * 1024)
#endif

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* debugging */
#endif


/* imported namespaces */

using libu::snaddw ;			/* subroutine */


/* external subroutines */


/* local variables */

constexpr int	tlen		= MAXLINELEN ;
cbool		f_debug		= CF_DEBUG ;


/* exported variables */


/* exported subroutines */

int debprintstr(cchar *idp,cchar *str,cchar *vdp,int vdl) noex {
    	cnothrow	nt{} ;
    	int		rs = SR_FAULT ;
	int		len = 0 ; /* return-value */
	if (idp && str && vdp) ylikely {
	    rs = SR_NOMEM ;
	    if (char *tbuf = new(nt) char[tlen + 1]) ylikely {
		int	tl = 0 ;
		if ((rs = snaddw(tbuf,tlen,tl,str)) >= 0) ylikely {
		    tl += rs ;
		    if ((rs = snaddw(tbuf,tlen,tl,vdp,vdl)) >= 0) ylikely {
			tl += rs ;
			if ((rs = snaddw(tbuf,tlen,tl,"\n")) >= 0) {
			    tl += rs ;
	                    rs = debprint(idp,tbuf,tl) ;
			    len = rs ;
			}
		    } /* end if (sncpy) */
		} /* end if (sncpy) */
		delete [] tbuf ;
	    } /* end if (m-a-f) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (debprintstr) */


/* local subroutines */



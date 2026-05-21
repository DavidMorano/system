/* cfinet SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* convert from Internet-Address */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* debugging */

/* revision history:

	= 1998-04-17, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	cfinet

	Description:
	This subroutine converts a character string representing a
	time-interval to an integer |int| tpye variable.  The
	time-interval is returned in an integer.  The character
	string representing a time-interval has the form:
		<mum>[<alpha>[<num>[<alpha>[<num>]]]] ...

	Synopsis:
	int cfinet(cchar *sbuf,int slen,uint *rp) noex

	Arguments:
	sbuf		source specification string pointer
	slen		source specification string length
	rp		pointer to integer to receive result

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* CSTD |UCHAR_MAX| */
#include	<cstddef>		/* CSTD |nullptr_t| */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<usupport.h>		/* LIBU */
#include	<intsat.h>		/* LIBU |intsat(3u)| */
#include	<mkchar.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */
#include	<dprint.hh>		/* LIBU |DPRINT(3u)| */

#include	"cfinet.hh"		/* local */
	
/* local defines */


/* imported namespaces */

using libu::cfdec ;			/* subroutine */
using libu::cfhex ;			/* subroutine */


/* local typedefs */


/* external subroutines */


/* external varaibles */


/* local structures */


/* forward references */


/* local variables */

const bool		f_debug	= CF_DEBUG ;


/* exported variables */


/* exported subroutines */

    int cfinet(cchar *sp,int sl,uint *rp) noex {
	int		rs = SR_FAULT ;
	uint		res = 0 ; /* return-value */
	if (sp) ylikely {
	    sp += 2 ;
	    sl -= 2 ;
	    rs = SR_INVALID ;
	    if (sl > 0) {
		rs = cfhex(sp,sl,&res) ;
	    }
	} /* end if (non-null) */
	if (rp) {
	    *rp = ((rs >= 0) ? res : 0) ;
	}
	return rs ;
    } /* end subroutine (cfinet) */


/* local subroutines */



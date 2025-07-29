/* snuuid SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* string-UUID (String-UUID) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	snuuid

	Description:
	This subroutine creates a string (in the standard representation)
	from a UUID.

	Synopsis:
	int snuuid(char *rbuf,int rlen,uuid_dat *up) noex

	Arguments:
	rbuf		result buffer pointer
	rlen		result buffer length
	up		pointer to UUID_DAT object holding a UUID

	Returns:
	>=0		resulting length
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
#include	<climits>		/* |UINT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstdint>		/* |uint64_t| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<sbuf.h>
#include	<mkuuid.h>		/* |uuid_dat| below */
#include	<localmisc.h>		/* |DIGBUFLEN| */

#include	"snx.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int snuuid(char *dbuf,int dlen,uuid_dat *up) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (dbuf && up) ylikely {
	    if (sbuf b ; (rs = b.start(dbuf,dlen)) >= 0) ylikely {
	        uint64_t	v = (up->time & UINT_MAX) ;
	        if (rs >= 0) rs = b.hexp(v,4) ;
	        if (rs >= 0) rs = b.chr('-') ;
	        v = ((up->time >> 32) & UINT_MAX) ;
	        if (rs >= 0) rs = b.hexp(v,2) ;
	        if (rs >= 0) rs = b.chr('-') ;
	        if (rs >= 0) ylikely {
		    uint64_t	tv ;
		    v = 0 ;
		    tv = ((up->time >> 48) & UINT_MAX) ;
		    v |= (tv & UCHAR_MAX) ;
		    tv = (up->version & USHORT_MAX) ;
		    v |= (tv << 8) ;
		    tv = ((up->time >> 48) & UINT_MAX) ;
		    v |= (tv << (8+4)) ;
	            rs = b.hexp(v,2) ;
	        } /* end if (ok) */
	        if (rs >= 0) rs = b.chr('-') ;
	        v = (up->clk & UINT_MAX) ;
	        v &= (~ (3 << 14)) ;
	        v |= (2 << 14) ;		/* standardized variant */
	        if (rs >= 0) rs = b.hexp(v,2) ;
	        if (rs >= 0) rs = b.chr('-') ;
	        v = up->node ;
	        if (rs >= 0) rs = b.hexp(v,6) ;
	        len = b.finish ;
	        if (rs >= 0) rs = len ;
	    } /* end if (sbuf) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (snuuid) */



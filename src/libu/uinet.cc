/* uinet SUPPORT */
/* lang=C20 */

/* Internet-Address functions */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
        This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/******************************************************************************* 
	These are support subroutines for some Internet-Address (IP)
	operations.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<localmisc.h>

#include	"uinet.h"


/* local defines */


/* external subroutines */


/* external variables */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

bool isin4mapped(ushort *in6addr) noex {
	bool		f = false ;
	int		i ; /* used afterwards */
	for (i = 0 ; i < 5 ; i += 1) {
	    if (in6addr[i] != 0) break ;
	}
	if (i == 5) {
	    const ushort	v = in6addr[5] ;
	    f = ((v == 0) || (v == 0xFFFF)) ;
	}
	return f ;
}
/* end subroutine (isin4mapped) */

bool isin6loopback(ushort *in6addr) noex {
	bool		f = false ;
	int		i ; /* used-afterwards */
	for (i = 0 ; i < 7 ; i += 1) {
	    if (in6addr[i] != 0) break ;
	}
	if (i == 7) {
	    const ushort	v = ntohs(in6addr[7]) ;
	    f = (v == 0x0001) ;
	}
	return f ;
}
/* end subroutine (isin6loopback) */



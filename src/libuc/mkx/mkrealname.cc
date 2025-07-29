/* mkrealname SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* make a realname */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This code was originally written.  

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mkrealname

	Description:
	This subroutine creates a realname from a GECOS-name or
	some other non-processed or non-canonical name.

	Synopsis:
	int mkrealname(char *rbuf,int rlen,cchar *gnp,int gnl) noex

	Arguments:
	rbuf		supplied buffer to receive result
	rlen		length of supplied buffer to receive result
	gnp		name to convert (usually a GECOS-name)
	gnl		length of name to convert

	Returns:
	>=0		length of result
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<realname.h>
#include	<localmisc.h>

#include	"mkx.h"


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

int mkrealname(char *rbuf,int rlen,cchar *gnp,int gnl) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ;
	if (rbuf && gnp) ylikely {
	    if (realname rn ; (rs = rn.start(gnp,gnl)) >= 0) ylikely {
	        {
	            rs = rn.name(rbuf,rlen) ;
	            len = rs ;
	        }
	        rs1 = rn.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (realname) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (mkrealname) */



/* inetntop */
/* lang=C20 */

/* convert strings to binary INET addresses */
/* version %I% last-modified %G% */


/* revision history:

	= 2003-11-06, David A­D­ Morano
	This subroutine exists to make some sensible version out
	of the combination of |inet_addr(3n)| and |inet_pton(3socket)|.

*/

/* Copyright © 2003 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	inetntop

	Description:
	This subroutines converts a string representation of an
	INET (either v4 or v6) address into its binary form.

	Synopsis:
	int inetntop(char *rbuf,int rlen,int af,vcoid *binaddr) noex

	Arguments:
	rbuf		result buffer pointer
	rlen		result buffer length
	af		address-family (AF) of source address
	binaddr		byte-array representing the source address

	Returns:
	>=0		length of resulting string
	<0		error

	Notes:
	There is a mistake in the standard.
	The error-value SR_NOSPC can be returned when it should
	have been SR_OVERFLOW.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<usystem.h>
#include	<localmisc.h>

#include	"inetconv.h"


/* local defines */


/* external subroutines */


/* local variables */


/* exported subroutines */

int inetntop(char *rbuf,int rlen,int af,cvoid *binaddr) noex {
	int		rs = SR_FAULT ;
	if (rbuf && binaddr) {
	    rs = SR_INVALID ;
	    if (af >= 0) {
		rs = uc_inetntop(rbuf,rlen,af,binaddr) ;
		if (rs == SR_NOSPC) rs = SR_OVERFLOW ; 
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (inetntop) */



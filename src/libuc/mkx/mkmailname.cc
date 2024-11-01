/* mkmailname SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* fix a name to make it like a mail-name */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-08-12, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mkmailname

	Description:
	This subroutine takes the GECOS version of a user name (this
	is from the fifth field of the system password file) and
	creates the associated 'mailname' from that.

	Synopsis:
	int mkmailname(char *rbuf,int rlen,cchar *gn,int gl) noex

	Arguments:
	rbuf		supplied buffer to receive result
	rlen		length of supplied buffer
	gn		source text to work from (should be GECOS-name)
	gl		source text length

	Returns:
	>=0		length of extracted mailname string
	<0		error (system-return)

*	SUBROUTINES CALLED:						
*		Only system routines are called.
*
*	GLOBAL VARIABLES USED:						
*		None!!  AS IT SHOULD BE!!
*
*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<realname.h>

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

int mkmailname(char *rbuf,int rlen,cchar *gp,int gl) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		rl = 0 ;
	if (rbuf && gp) {
	    realname	rn ;
	    if ((rs = realname_start(&rn,gp,gl)) >= 0) {
	        {
	            rs = realname_mailname(&rn,rbuf,rlen) ;
		    rl = rs ;
	        }
	        rs1 = realname_finish(&rn) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (readlname) */
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (mkmailname) */



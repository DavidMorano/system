/* bminmod SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* "Basic I-O" package similiar to some other thing whose initials is "stdio" */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Sset a minimum mode on the opened file.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<localmisc.h>

#include	"bfile.h"


/* local defines */


/* external subroutines */


/* external variables */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int bminmod(bfile *op,mode_t om) noex {
	int		rs ;
	if ((rs = bfile_magic(op)) >= 0) {
	    if (op->fd >= 0) {
	        rs = uc_fminmod(op->fd,om) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end routine (bminmod) */



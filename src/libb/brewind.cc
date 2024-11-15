/* brewind SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* "Basic I-O" package similiar to "stdio" */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	We rewind the BFILE stream.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<usystem.h>
#include	<localmisc.h>

#include	"bfile.h"


/* local defines */


/* external subroutines */


/* external variables */


/* exported variables */


/* exported subroutines */

int brewind(bfile *fp) noex {
	return bseek(fp,0z,SEEK_SET) ;
}
/* end subroutine (brewind) */



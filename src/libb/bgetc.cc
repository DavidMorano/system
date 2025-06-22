/* bgetc SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* "Basic I-O" package similiar to "stdio" */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Subroutine to read a single character (within the BFILE
	facility.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<usystem.h>
#include	<mkchar.h>
#include	<localmisc.h>

#include	"bfile.h"


/* local defines */


/* external subroutines */


/* external variables */


/* exported variables */


/* exported subroutines */

int bgetc(bfile *op) noex {
	int		rs ;
	int		ch = 0 ;
	char		buf[2] ;
	if ((rs = bread(op,buf,1)) > 0) {
	    ch = mkchar(buf[0]) ;
	} else if (rs == 0) {
	    rs = SR_EOF ;
	}
	return (rs > 0) ? ch : rs ;
}
/* end subroutine (bgetc) */



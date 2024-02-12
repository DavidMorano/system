/* bgetc SUPPORT */
/* lang=C++20 */

/* "Basic I/O" package similiar to "stdio" */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* compile-time debugging */

/* revision history:

	= 1998-07-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Subroutine to read a single character (within the BFILE
	facility.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<usystem.h>
#include	<localmisc.h>

#include	"bfile.h"


/* local defines */


/* external subroutines */


/* external variables */


/* exported subroutines */

int bgetc(bfile *fp) noex {
	int		rs ;
	int		ch ;
	char		buf[2] ;

	rs = bread(fp,buf,1) ;
	if (rs == 0) rs = SR_EOF ;

	ch = (buf[0] & 0xff) ;
	return (rs > 0) ? ch : rs ;
}
/* end subroutine (bgetc) */



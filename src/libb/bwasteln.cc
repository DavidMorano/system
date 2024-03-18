/* bwasteln SUPPORT */
/* lang=C++20 */

/* "Basic I/O" package similiar to "stdio" */
/* last modifed %G% version %I% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This subroutine was originally written.

	= 1999-01-10, David A­D­ Morano
	I added the little extra code to allow for memory mapped
	I/O. It is all a waste because it is way slower than without
	it! This should teach me to leave old programs alone!

*/

/* Copyright © 1998,1998 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

	This subroutine reads and throws away a single line from
	the file.

******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<usystem.h>
#include	<localmisc.h>

#include	"bfile.h"


/* local defines */


/* external subroutines */


/* external variables */


/* exported variables */


/* exported subroutines */

int bwasteln(bfile *fp,char *linebuf,int linelen) noex {
	int		rs ;
	int		tlen = 0 ;
	while ((rs = breadln(fp,linebuf,linelen)) > 0) {
	    int	len = rs ;
	    tlen += len ;
	    if (linebuf[len - 1] == '\n') break ;
	} /* end while */
	return (rs >= 0) ? tlen : rs ;
}
/* end subroutine (bwasteln) */



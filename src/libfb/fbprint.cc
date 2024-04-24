/* fbprint SUPPORT */
/* lang=C++20 */

/* print some (binary) data out to a STDIO stream */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-08-17, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

	This is a knock-off of the 'fwrite(3)' subroutine, only
	sensible!

******************************************************************************/

#include	<envstandards.h>
#include	<cstdio>
#include	<usystem.h>


/* local defines */


/* exported variables */


/* exported subroutines */

int fbprint(FILE *fp,char *lbuf,int llen) noex {
	int		rs = SR_FAULT ;
	int		wlen = 0 ;
	if (fp && lbuf) {
	    rs = SR_OK ;
	    if (llen < 0) llen = strlen(lbuf) ;
	    if (llen > 0) {
	        rs = fbwrite(fp,lbuf,llen) ;
	        wlen += rs ;
	    }
	    if ((rs >= 0) && ((llen == 0) || (lbuf[llen-1] != '\n'))) {
	        rs = fbwrite(fp,"\n",1) ;
	        wlen += rs ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (fbprint) */



/* bputc SUPPORT */
/* lang=C++20 */

/* "Basic I/O" package similiar to "stdio" */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Print out a single character (within the BFILE framework).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<usystem.h>
#include	<localmisc.h>
#include	"bfile.h"


/* local defines */


/* external subroutines */


/* external variables */


/* exported subroutines */

int bputc(bfile *fp,int ch) noex {
	int		rs ;
	int		wlen = 0 ;
	char		buf[2] ;

#if	CF_SAFE
	if (fp == NULL) return SR_FAULT ;
#endif /* CF_SAFE */

	buf[0] = ch ;
	if ((rs = bwrite(fp,buf,1)) > 0) {
	    wlen = rs ;
	    if ((ch == '\n') && (fp->bm == bfile_bmline)) {
	        rs = bfile_flush(fp) ;
	    }
	}

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (bputc) */



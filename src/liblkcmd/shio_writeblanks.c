/* shio_writeblanks SUPPORT */
/* lang=C++20 */

/* write blacks to a SHIO object */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Write a specified number of blanks to a SHIO object.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */

#if	defined(SFIO) && (SFIO > 0)
#define	CF_SFIO	1
#else
#define	CF_SFIO	0
#endif

#if	(defined(KSHBUILTIN) && (KSHBUILTIN > 0))
#include	<shell.h>
#endif

#include	<sys/types.h>
#include	<usystem.h>
#include	<strn.h>		/* |strnset(3uc)| */
#include	<localmisc.h>

#include	"shio.h"


/* local defines */

#undef	NBLANKS
#define	NBLANKS		8


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int shio_writeblanks(SHIO *fp,int n) noex {
	int		rs = SR_FAULT ;
	int		ml ;
	int		wlen = 0 ;
	char		blanks[NBLANKS] ;
	if (fp) {
	    rs = SR_OK ;
	    strnblanks(blanks,NBLANKS) ;
	    while ((rs >= 0) && (wlen < n)) {
	        ml = MIN((n-wlen),NBLANKS) ;
	        rs = shio_write(fp,blanks,ml) ;
	        wlen += rs ;
	    } /* end while */
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (shio_writeblanks) */



/* mkshlibname SUPPORT */
/* lang=C20 */

/* make the filename for a shared library (shared object) */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* compile-time print-outs */

/* revision history:

	= 2008-07-01, David A�D� Morano
	This program was originally written.

*/

/* Copyright � 2008 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	This subroutine formulates (makes) the file-name for a
	shared library (shared object).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<stdlib.h>
#include	<string.h>
#include	<usystem.h>
#include	<storebuf.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */


/* external variables */


/* forward references */


/* local variables */


/* exported subroutines */

int mkshlibname(char *shlibname,cchar *pnp,int pnl) noex {
	const int	shliblen = MAXNAMELEN ;
	int		rs = SR_OK ;
	int		i = 0 ;
	int		f ;
	const char	*lc = "lib" ;

	f = ((pnl >= 3) && (strncmp(pnp,lc,3) == 0)) ;
	if (! f) {
	    rs = storebuf_strw(shlibname,shliblen,i,lc,3) ;
	    i += rs ;
	}

	if (rs >= 0) {
	    rs = storebuf_strw(shlibname,shliblen,i,pnp,pnl) ;
	    i += rs ;
	}

	if (rs >= 0) {
	    rs = storebuf_char(shlibname,shliblen,i,'.') ;
	    i += rs ;
	}

	if (rs >= 0) {
	    rs = storebuf_strw(shlibname,shliblen,i,"so",2) ;
	    i += rs ;
	}

	return (rs >= 0) ? i : rs ;
}
/* end subroutine (mkshlibname) */



/* snclean SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* clean up a c-string of text */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	snclean

	Description:
	This subroutine cleans up a counted c-string.

	Synopsis:
	int snclean(char *dbuf,int dlen,cchar *sp,int sl) noex

	Arguments:
	dbuf		result buffer pointer
	dlen		result buffer length
	sp		source c-string pointer
	sl		source c-string length

	Returns:
	>=0		length of result c-string
	<0		error code (system-return)

	See-also:
	snfsflags(3uc)
	snopenflags(3uc)
	snpollflags(3uc)
	snxtilook(3uc)
	sninetaddr(3uc)
	snsigabbr(3uc)
	snabbr(3uc)
	snshellunder(3uc)
	snfilemode(3uc)
	sntid(3uc)
	snerrabbr(3uc)
	snrealname(3uc)
	snloadavg(3uc)
	snkeyval(3uc)
	snwvprintf(3uc)
	snwprintf(3uc)
	snkeyval(3uc)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<mkchar.h>
#include	<ischarx.h>		/* |isprintlatin(3uc)| */
#include	<localmisc.h>

#include	"snclean.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static inline bool	ischarok(int) noex ;


/* local variables */

cint		ch_sub = mkchar('¿') ;


/* exported variables */


/* exported subroutines */

int snclean(char *dbuf,int dlen,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		dl = 0 ; /* return-value */
	if (dbuf && sp) ylikely {
	    rs = SR_INVALID ;
	    if (dlen >= 0) ylikely {
	        rs = SR_OK ;
		while (dlen-- && sl && *sp) {
	            if (cint ch = mkchar(*sp) ; ischarok(ch)) {
		        dbuf[dl++] = char(ch) ;
		    } else {
		        dbuf[dl++] = char(ch_sub) ;
		    }
		    sp += 1 ;
		    sl -= 1 ;
		} /* end while */
		if (sl && *sp) rs = SR_OVERFLOW ;
	    } /* end if (valid) */
	    dbuf[dl] = '\0' ;
	} /* end if (non-null) */
	return (rs >= 0) ? dl : rs ;
}
/* end subroutine (snclean) */


/* local subroutines */

static bool ischarok(int ch) noex {
    	bool	f = isprintlatin(ch) ;
	f = f || (ch == '\t') || (ch == '\n') ;
	return f ;
}
/* end subroutine (ischarok) */



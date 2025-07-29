/* strdcpyclean SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* copy a source string to a destination while cleaning it up */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-08, David A­D­ Morano
	This was written to clean up some problems with printing garbage
	characters in a few places in some PCS utilities.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	strdcpyclean

	Description:
	This is essentially the same as the |strdcpy(3dam)| subroutine
	except that garbage characters are replaced with a specified
	substitute character.

	Synopsis:
	int strdcpyclean(char *dp,int dl,cchar *sp,int sl) noex

	Arguments:
	dp		destination string buffer
	dl		destination string buffer length
	sp		source string
	sl		source string length

	Returns:
	-		pointer to NUL at end of destination string

	See-also:
	strdcpy(3dam),
	strdwcpyopaque(3dam), 
	strdcpycompact(3dam), 

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<ascii.h>
#include	<mkchar.h>
#include	<ischarx.h>

#include	"strdcpy.h"


/* local defines */


/* external subroutines */


/* external variables */


/* forward references */

static bool	isour(int) noex ;


/* exported variables */


/* exported subroutines */

char *strdcpyclean(char *dbuf,int dlen,int sch,cchar *sp,int sl) noex {
	char		*rp = nullptr ; /* return-value */
	if (dbuf && sp) {
	    int		dl = 0 ; /* used-afterwards */
	    while (dlen-- && sl-- && *sp) {
	        cint ch = mkchar(*sp) ;
	        if (isour(ch)) {
		    dbuf[dl++] = char(ch) ;
	        } else if (sch != 0) {
		    dbuf[dl++] = char(sch) ;
	        }
	        sp += 1 ;
	    } /* end while */
	    dbuf[dl] = '\0' ;
	    rp = (dbuf + dl) ;
	} /* end if (non-null) */
	return rp ;
}
/* end subroutine (strdcpyclean) */


/* local subroutines */

static bool isour(int ch) noex {
	bool		f = false ;
	f = f || isprintlatin(ch) ;
	f = f || (ch == CH_NL) ;
	f = f || (ch == CH_BS) ;
	f = f || (ch == CH_BEL) ;
	return f ;
}
/* end subroutine (isour) */



/* snwcpyopaque SUPPORT */
/* lang=C20 */

/* special (excellent) string-copy type of subroutine! */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-08, David A�D� Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	snwcpyopaque

	Description:
	This is essentially the same as the 'snwcpy(3dam)' subroutine
	except that white-space characters not copied over to the
	result.

	Synopsis:
	int snwcpyopaque(char *dp,int dl,cchar *sp,int sl) noex

	Arguments:
	dp		destination string buffer
	dl		destination string buffer length
	sp		source string
	sl		source string length

	Returns:
	>=0		number of bytes in result
	<0		error

	See-also:
	snwcpy(3dam),
	snwcpylatin(3dam), 
	snwcpyopaque(3dam), 
	snwcpycompact(3dam), 
	snwcpyclean(3dam), 
	snwcpyhyphen(3dam)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<usystem.h>
#include	<char.h>
#include	<mkchar.h>

#include	"snwcpyx.h"


/* local defines */


/* external subroutines */


/* external variables */


/* exported variables */


/* exported subroutines */

int snwcpyopaque(char *dbuf,int dlen,cchar *sp,int sl) noex {
	int		rs = SR_OK ;
	int		dl = 0 ;
	while (sl && *sp) {
	    cint	ch = mkchar(*sp) ;
	    if (! CHAR_ISWHITE(ch)) {
		if (dlen-- == 0) break ;
		dbuf[dl++] = char(ch) ;
	    } /* end if */
	    sp += 1 ;
	    sl -= 1 ;
	} /* end while */
	if ((sl != 0) && (*sp != '\0')) rs = SR_OVERFLOW ;
	dbuf[dl] = '\0' ;
	return (rs >= 0) ? dl : rs ;
}
/* end subroutine (snwcpyopaque) */



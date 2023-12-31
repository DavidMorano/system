/* snwcpyclean */
/* lang=C20 */

/* copy a source string to a destination while cleaning it up */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-08, David A�D� Morano
	This was written to clean up some problems with printing garbage
	characters in a few places in some PCS utilities.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	snwcpyclean

	Description:
	This is essentially the same as the 'snwcpy(3dam)' subroutine
	except that garbage characters are replaced with a specified
	substitute character.

	Synopsis:
	int snwcpyclean(char *dp,int dl,cchar *sp,int sl) noex

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
	snwcpyhyphen(3dam), 

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<string.h>
#include	<usystem.h>
#include	<ascii.h>
#include	<localmisc.h>

#include	"snwcpyx.h"


/* external subroutines */

extern int	isprintlatin(int) noex ;


/* external variables */


/* forward references */

static int	isour(int) noex ;


/* exported subroutines */

int snwcpyclean(char *dbuf,int dlen,int sch,cchar *sp,int sl) noex {
	int		ch ;
	int		dl = 0 ;
	int		rs = SR_OK ;
	while (dlen-- && sl && *sp) {
	    ch = MKCHAR(*sp) ;
	    if (isour(ch)) {
		dbuf[dl++] = (char) ch ;
	    } else if (sch != 0) {
		dbuf[dl++] = (char) sch ;
	    }
	    sp += 1 ;
	    sl -= 1 ;
	} /* end while */
	if ((sl != 0) && (*sp != '\0')) rs = SR_OVERFLOW ;
	dbuf[dl] = '\0' ;
	return (rs >= 0) ? dl : rs ;
}
/* end subroutine (snwcpyclean) */


/* local subroutines */

static int isour(int ch) noex {
	int		f = FALSE ;
	f = f || isprintlatin(ch) ;
	f = f || (ch == CH_BEL) ;
	f = f || (ch == CH_BS) ;
	f = f || (ch == CH_NL) ;
	return f ;
}
/* end subroutine (isour) */



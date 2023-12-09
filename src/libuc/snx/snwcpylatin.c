/* snwcpylatin */
/* lang=C20 */

/* special (excellent) string-copy type of subroutine! */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-08, David A­D­ Morano
	This was written to clean up some problems with printing
	garbage characters in a few place in some PCS utilities.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	snwcpylatin

	Description:
	This is essentially the same as the |snwcpy(3dam)| subroutine
	except that non-printable characters are replaced with a
	stuff-mark character.

	Synopsis:
	int snwcpylatin(char *dp,int dl,cchar *sp,int sl) noex

	Arguments:
	dp		destination string buffer
	dl		destination string buffer length
	sp		source string
	sl		source string length

	Returns:
	>=0		number of bytes in result
	<0		error


	Notes:
	This subroutine just calls either the 'sncpy1(3dam)' or the
	'strwcpy(3dam)' subroutine based on the arguments.

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
#include	<usystem.h>
#include	<mkchar.h>
#include	<localmisc.h>

#include	"snwcpyx.h"


/* external subroutines */

extern int	isprintlatin(int) noex ;


/* external variables */


/* exported subroutines */

int snwcpylatin(char *dbuf,int dlen,cchar *sp,int sl) {
	int		ch ;
	int		dl = 0 ;
	int		rs = SR_OK ;
	while (dlen-- && sl && *sp) {
	    ch = MKCHAR(*sp++) ;
	    if (! isprintlatin(ch)) ch = ('-' + 128) ;
	    dbuf[dl++] = (char) ch ;
	    sl -= 1 ;
	} /* end while */
	if ((sl != 0) && (*sp != '\0')) rs = SR_OVERFLOW ;
	dbuf[dl] = '\0' ;
	return (rs >= 0) ? dl : rs ;
}
/* end subroutine (snwcpylatin) */



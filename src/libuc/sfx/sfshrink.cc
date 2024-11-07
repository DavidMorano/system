/* sfshrink SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* remove leading and trailing white space */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-05-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sfshrink

	Description:
	This subroutine will identify the non-white-space portion
	of the buffer by ignoring white space at the beginning and
	at the end of the given buffer.  No modifications to the
	buffer are made.

	Synopsis:
	int sfshrink(cchar *sp,int sl,cchar **rpp) noex ;

	Arguments:
	sp	buffer
	sl	buffer length
	rpp	pointer to prointer to resulting string

	Returns:
	+ non-white-space string length (if OK), otherwise 0

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstring>		/* <- for |strlen(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<char.h>

#include	"sfx.h"


/* local defines */


/* exported variables */


/* exported subroutines */

int sfshrink(cchar *sp,int sl,cchar **rpp) noex {
	if (sl < 0) {
	    while (CHAR_ISWHITE(*sp)) {
	        sp += 1 ;
	    }
	    sl = strlen(sp) ;
	} else {
	    while ((sl > 0) && CHAR_ISWHITE(*sp)) {
	        sp += 1 ;
	        sl -= 1 ;
	    } /* end while */
	    if (sp[0] == '\0') sl = 0 ;
	} /* end if */
	while ((sl > 0) && CHAR_ISWHITE(sp[sl - 1])) {
	    sl -= 1 ;
	}
	if (rpp) *rpp = sp ;
	return sl ;
}
/* end subroutine (sfshrink) */



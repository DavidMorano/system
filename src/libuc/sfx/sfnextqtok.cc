/* sfnextqtok SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* find the next quoted string token */
/* version %I% last-modified %G% */


/* revision history:

	= 2009-04-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 2009 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This subroutine finds the next quoted string token in a
	given string.

	Synopsis:

	int sfnextqtok(cchar *sp,int sl,cchar **rpp) noex

	Arguments:
	sp		pointer to source string
	sl		length of given source string
	rpp		pointer to result pointer

	Returns:
	>0		lenght of found string token
	==0		no more token strings were found

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>
#include	<cstdlib>
#include	<cstring>		/* |strlen(3c)| */
#include	<usystem.h>
#include	<ascii.h>
#include	<char.h>
#include	<localmisc.h>

#include	"sfx.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */

extern "C" {
    extern int	sidquote(cchar *,int) noex ;
}


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int sfnextqtok(cchar *sp,int sl,cchar **rpp) noex {
	int		len = 0 ;
	if (sl < 0) sl = strlen(sp) ;
/* skip over whitespace */
	while (sl && CHAR_ISWHITE(sp[0])) {
	    sp += 1 ;
	    sl -= 1 ;
	} /* end while */
	if (rpp) *rpp = sp ;
/* skip over the non-whitespace */
	len = sl ;
	while (sl && sp[0] && (! CHAR_ISWHITE(sp[0]))) {
	    int		si ;
	    if (sp[0] == CH_DQUOTE) {
	        sp += 1 ;
	        sl -= 1 ;
	        si = sidquote(sp,sl) ;
	    } else {
	        si = 1 ;
	    } /* end if */
	    sp += si ;
	    sl -= si ;
	} /* end while */
	len = (len - sl) ;
	return len ;
}
/* end subroutine (sfnextqtok) */



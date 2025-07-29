/* sfnextqtok SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* find the Next-Quoted-String-Token */
/* version %I% last-modified %G% */


/* revision history:

	= 2009-04-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 2009 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	sfnextqtok

	Description:
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
	<0		not curently possible

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<ascii.h>		/* |CH_{xx}| */
#include	<six.h>			/* |sidquote(3uc)| */
#include	<char.h>		/* |char_iswhite(3uc)| */
#include	<localmisc.h>

#include	"sfx.h"
#include	"sfnext.h"

import libutil ;

/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int sfnextqtok(cchar *sp,int sl,cchar **rpp) noex {
	int		len = -1 ;
	if (sp) ylikely {
	    if (sl < 0) sl = lenstr(sp) ;
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
	    len -= sl ;
	} /* end if (non-null) */
	return len ;
}
/* end subroutine (sfnextqtok) */



/* hasfext */
/* lang=C20 */

/* determine if file-name (just a string) has an approved file-extension */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	hasfext

	Description:
        Given a file-name (a counted string) we determine if it has a
        file-extension of a given class.

	Synopsis:
	int hasfext(cchar **exts,cchar *fp,int fl) noex

	Arguments:
	exts		arrays of strings (allowable extensions)
	fp		file-name string
	fl		file-name length

	Returns:
	==0		does not have an approved extension
	>0              has an approved extension (and this is
			base-str length)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<string.h>		/* <- for |strlen(3c)| */
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<matstr.h>
#include	<strn.h>
#include	<localmisc.h>

#include	"hasfext.h"


/* local defines */


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */

int hasfext(cchar **exts,cchar *fp,int fl) noex {
	cchar		*tp ;
	int		si = 0 ;
	bool		f = false ;
	if (fl < 0) fl = strlen(fp) ;
	if ((tp = strnrchr(fp,fl,'.')) != nullptr) {
	    cint	el = ((fp+fl)-(tp+1)) ;
	    cchar	*ep = (tp+1) ;
	    si = (tp-fp) ;
	    if (el > 0) {
	        f = (matstr(exts,ep,el) >= 0) ;
	    }
	} else {
	    si = fl ;
	    f = (matstr(exts,fp,0) >= 0) ;
	}
	return (f) ? si : 0 ;
}
/* end subroutine (hasfext) */



/* sifext SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* determine if file-name (just a c-string) has an approved file-extension */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sifext

	Description:
        Given a file-name (a counted c-string) we determine if it has a
        file-extension of a given set.

	Synopsis:
	int sifext(cchar *fp,int fl,mainv exts) noex

	Arguments:
	fp		file-name string
	fl		file-name length
	exts		arrays of strings (allowable extensions)

	Returns:
	<0		does not have an approved extension
	==0		has an approved extension, but has no base part
	>0              has an approved extension (and this is
			base-str length)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* <- for |strlen(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<matstr.h>
#include	<strn.h>
#include	<localmisc.h>

#include	"sifext.h"


/* local defines */


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int sifext(cchar *fp,int fl,mainv exts) noex {
	int		si = -1 ;
	if (fl < 0) fl = strlen(fp) ;
	if (cchar *tp ; (tp = strnrchr(fp,fl,'.')) != nullptr) {
	    cint	el = ((fp+fl)-(tp+1)) ;
	    cchar	*ep = (tp+1) ;
	    if ((el > 0) && (matstr(exts,ep,el) >= 0)) {
	    	si = (tp-fp) ;
	    }
	} /* end if(had extension) */
	return si ;
}
/* end subroutine (sifext) */



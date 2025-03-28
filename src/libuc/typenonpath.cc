/* typenonpath SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* determine the type of nonpath in given counted c-string (filename) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	typenonpath

	Description:
	We test a string to see if it is a floating or non-path
	filename.  If we find a non-path filename, we determie
	the type (and return it).

	Synopsis:
	int typenonpath(cchar *pp,int pl) noex

	Arguments:
	- pp	pointer to path string
	- pl	length of given path string

	Returns:
	0	no non-path filename found
	>0	type of non-path filename determined
	<0	error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* <- for |strlen(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<six.h>
#include	<strn.h>
#include	<localmisc.h>

#include	"typenonpath.h"


/* local defines */


/* imported namespaces */

using std::nullptr_t ;			/* type */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* local variables */

static constexpr cchar		nonpaths[] = "/¥§~" ;


/* exported variables */


/* exported subroutines */

bool typenonpath(cchar *fp,int fl) noex {
    	int		rs = SR_FAULT ;
	int		t = 0 ; /* assume not non-path */
	if (fp) {
	    rs = SR_OK ;
	    if (fl < 0) fl = strlen(fp) ;
	    if ((fl > 0) && (fp[0] != '/')) {
	        cnullptr	np{} ;
	        if (cchar *tp ; (tp = strnpbrk(fp,fl,nonpaths)) != np) {
	            if (((tp - fp) > 0) && (tp[1] != '\0')) {
	                t = sichr(nonpaths,-1,*tp) ;
	            }
	        }
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? t : rs ; /* type-of-nonpath */
}
/* end subroutine (typenonpath) */



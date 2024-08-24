/* hasnonpath SUPPORT */
/* lang=C++20 */

/* determine if the given string represent a non-path filename */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	hasnonpath

	Description:
	We test a string to see if it is a floating or non-path filename.

	Synopsis:
	int hasnonpath(cchar *pp,int pl) noex

	Arguments:
	- pp	pointer to path string
	- pl	length of given path string

	Returns:
	true
	false

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstring>		/* <- for |strlen(3c)| */
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>
#include	<six.h>
#include	<strn.h>
#include	<localmisc.h>

#include	"hasnonpath.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

enum nonpaths {
	nonpath_not,
	nonpath_dialer,
	nonpath_fsvc,
	nonpath_usvc,
	nonpath_overlast
} ;


/* forward references */


/* local variables */

static constexpr cchar		nonpaths[] = "/¥§~" ;


/* exported variables */


/* exported subroutines */

bool hasnonpath(cchar *fp,int fl) noex {
	bool		f = (fp[0] != '/') ;
	if (fl < 0) fl = strlen(fp) ;
	if (f) {
	    f = false ;
	    if (cchar *tp ; (tp = strnpbrk(fp,fl,nonpaths)) != nullptr) {
	        if (((tp - fp) > 0) && (tp[1] != '\0')) {
	            f = sichr(nonpaths,-1,*tp) ;
	        }
	    }
	}
	return f ;
}
/* end subroutine (hasnonpath) */



/* delenv SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* delete an environment variable */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

  	Name:
	delenv

  	Description:
	This subroutine is used to delete an environment variable
	permanently from the process environment list.  There is no
	convenient way to un-delete this later.  This is an incredibly
	stupid hack.  This subroutine should *never* be used.  It
	was only written to satisfy some stupid open-source programs
	that I occassionally stoop too low to want to use!

******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<localmisc.h>

#include	"delenv.h"


/* local defines */


/* external subroutines */


/* external variables */

extern char	**environ ;


/* local structures */


/* forward references */

static bool	namecmp(cchar *,int,cchar *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

cchar *delenv(cchar *name) noex {
	cchar		*cp = nullptr ;
	if (name || environ) {
	    cchar	**env = (cchar **) environ ;
	    cchar	**p2 ;
	    cchar	**p1 ;
	    {
		cchar	*sp = name ; /* used-afterwards */
	        while ((*sp != '\0') && (*sp != '=')) {
	       	    sp += 1 ;
	        }
	        {
	            cint len = intconv(sp - name) ;
	            for (p1 = env ; *p1 != nullptr ; p1 += 1) {
	                if (namecmp(name,len,*p1) == 0) break ;
	            }
	        } /* end block */
	    } /* end block */
	    if (*p1) {
	        cp = *p1 ;
	        /* find the last entry */
	        for (p2 = p1 ; *p2 != nullptr ; p2 += 1) ;
	        p2 -= 1 ;
	        /* swap the last entry with this one */
	        if (p1 != p2) *p1 = *p2 ;
	        *p2 = nullptr ;
	    } else {
	        cp = nullptr ;
	    }
	} /* end if (non-null) */
	return cp ;
}
/* end subroutine (delenv) */


/* local subroutines */

static bool namecmp(cchar *name,int namelen,cchar *entry) noex {
	cchar	*nep = (name + namelen) ;
	cchar	*ep = entry ;
	bool	f = false ;
	for (cc *sp = name ; (sp < nep) && *ep && (*ep != '=') ; ) {
	    f = (*sp++ != *ep++) ;
	    if (f) break ;
	} /* end while */
	return f ;
}
/* end subroutine (namecmp) */



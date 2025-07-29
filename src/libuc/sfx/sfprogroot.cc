/* sfprogroot SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* try to find a program-root in a given c-string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sfprogroot

	Description:
	This subroutine tries to extract a program-root (directory)
	out of a directory path.  It looks for "bin" and "sbin" in
	the directory name in order to guess if a program-root is
	present or not.

	Synopsis:
	int sfprogroot(cchar *pp,int pl,cchar **rpp) noex

	Arguments:
	pp		directory to search
	pl		length of directory to search
	rpp		pointer to hold pointer to result (if found)

	Returns:
	>=		program-root found and this is its length
	<0		program-root not found

	Example:
	{
		const char	*execname = "/usr/bin/sleep" ;
		const char	*dp ;
		int		dl ;
		if ((dl = sfdirname(execname,-1,&dp)) > 0) {
		    int		rl ;
		    const char	*rp ;
		    if ((rl = sfprogroot(dp,dl,&rp)) > 0) {
		        fprintf(stdout,"pr=%s\n",rp) ;
		    }
		}
	}

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strncmp(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<localmisc.h>

#include	"sfx.h"			/* |sf{x}name(3uc)| */


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int sfprogroot(cchar *pp,int pl,cchar **rpp) noex {
	int		sl = -1 ; /* return-value */
	cchar		*sp = nullptr ;
	if (sp) ylikely {
	    cchar	*dp{} ;
	    if (int dl ; (dl = sfdirname(pp,pl,&dp)) > 0) {
	        bool	f ;
	        cchar	*bp{} ;
	        int	bl = sfbasename(dp,dl,&bp) ;
	        f = ((bl == 3) && (strncmp(bp,"bin",bl) == 0)) ;
	        if (! f) {
	            f = ((bl == 4) && (strncmp(bp,"sbin",bl) == 0)) ;
	        }
	        if (f) {
	            sl = sfdirname(dp,dl,&sp) ;
	        }
	    } /* end if */
	} /* end if (non-null) */
	if (rpp) {
	    *rpp = (sl >= 0) ? sp : nullptr ;
	}
	return sl ;
}
/* end subroutine (sfprogroot) */



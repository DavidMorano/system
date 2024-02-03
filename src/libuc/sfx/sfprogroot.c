/* sfprogroot SUPPORT */
/* lang=C20 */

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
#include	<string.h>		/* <- for |strncmp(3c)| */
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<strnxcmp.h>

#include	"sfx.h"


/* local defines */


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int sfprogroot(cchar *pp,int pl,cchar **rpp) noex {
	int		dl ;
	int		sl = -1 ;
	cchar		*dp ;
	cchar		*sp = nullptr ;
	if ((dl = sfdirname(pp,pl,&dp)) > 0) {
	    int		f ;
	    cchar	*bp ;
	    int		bl ;
	    bl = sfbasename(dp,dl,&bp) ;
	    f = ((bl == 3) && (strncmp(bp,"bin",bl) == 0)) ;
	    if (! f) {
	        f = ((bl == 4) && (strncmp(bp,"sbin",bl) == 0)) ;
	    }
	    if (f) {
	        sl = sfdirname(dp,dl,&sp) ;
	    }
	} /* end if */
	if (rpp) {
	    *rpp = (sl >= 0) ? sp : nullptr ;
	}
	return sl ;
}
/* end subroutine (sfprogroot) */

int getpr(cchar *pp,int pl,cchar **rpp) noex {
	return sfprogroot(pp,pl,rpp) ;
}
/* end subroutine (getpr) */



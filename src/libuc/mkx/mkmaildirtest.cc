/* mkmaildirtest SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* make a test file-directory name for testing a mail directory */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-12-03, David A­D­ Morano
	This code was born out of frustration with cleaning up bad
	legacy code (of which there is quite a bit -- like almost
	all of it).

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	mkmaildirtest

	Description:
	This subroutine constructs a filename suitable for testing
	whether a mail-directory is accessible or not.

	Synopsis:
	int mkmaildirtest(char *rbuf,cchar *dp,int dl) noex

	Arguments:
	rbuf		result buffer (must be MAXPATHLEN long)
	dp		directory-path c-string pointer
	dl		directory-path c-string length

	Returns:
	>=0		length of resuling file-name path
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<bufsizevar.hh>
#include	<storebuf.h>
#include	<localmisc.h>

#include	"mkx.h"


/* local defines */

#define	MAILDIR_SAVED	":saved"


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

static bufsizevar	maxpathlen(getbufsize_mp) ;

constexpr char		saved[] = MAILDIR_SAVED ;


/* exported variables */


/* exported subroutines */

int mkmaildirtest(char *rbuf,cchar *dp,int dl) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (rbuf && dp) ylikely {
	    if ((rs = maxpathlen) >= 0) ylikely {
		if (storebuf sb(rbuf,rs) ; (rs = sb.strw(dp,dl)) >= 0) ylikely {
	            if ((rs > 0) && (rbuf[rs - 1] != '/')) {
	                rs = sb.chr('/') ;
		    }
	            if (rs >= 0) ylikely {
	                rs = sb.strw(saved) ;
			len = sb.idx ;
		    }
	        } /* end if (storebuf) */
	    } /* end if (maxpathlen) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (mkmaildirtest) */



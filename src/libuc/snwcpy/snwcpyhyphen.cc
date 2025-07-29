/* snwcpyhyphen SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* similar to |snwcpy(3uc)| w/ exceptions */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-03-14, David A­D­ Morano
	This subroutine was written as a hack to get a subroutine
	with the calling sematic of |snwcpy(3uc)| but with the
	twist that any underscore characters found in the source
	string are converted into hyphen characters in the destination.

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	snwcpyhyphen

	Description:
	This subroutine is similar to |snwcpy(3uc)| except that
	any underscore characters found in the source string are
	substituted for hyphen characters in the target destination
	string.

	Why would anyone make something like this?

	We needed a hack for real-names that are grabbed from the
	GECOS field of the PASSWD database.  The names in GECOS
	cannot have hyphens in them when the AT&T GECOS convention
	is used (since it uses hyphens to delimit subfields itself.
	Instead real-names that are put into AT&T conforming GECOS
	fields have underscrore characters where hyphens might
	otherwise be.  This subroutine undoes that character
	substitution that is already in the GECOS field of the
	PASSWD databases.

	See-also:
	snwcpy(3uc),
	snwcpylatin(3uc), 
	snwcpyopaque(3uc), 
	snwcpycompact(3uc), 
	snwcpyclean(3uc), 
	snwcpyhyphen(3uc),

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<mkchar.h>
#include	<localmisc.h>

#include	"snwcpyx.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int snwcpyhyphen(char *dbuf,int dlen,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		dl = 0 ; /* return-value */
	if (dbuf && sp) {
	    rs = SR_INVALID ;
	    if (dlen >= 0) {
	        rs = SR_OK ;
	        while (dlen-- && sl && *sp) {
	            int ch = mkchar(*sp++) ;
	            if (ch == '_') ch = '-' ;
	            dbuf[dl++] = char(ch) ;
	            sl -= 1 ;
	        } /* end while */
	        if (sl && *sp) rs = SR_OVERFLOW ;
	        dbuf[dl] = '\0' ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? dl : rs ;
}
/* end subroutine (snwcpyhyphen) */



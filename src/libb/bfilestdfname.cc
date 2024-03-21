/* bfilestdfname SUPPORT */
/* lang=C++20 */

/* BFILE standard-file-name determination */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A�D� Morano
	This subroutine was originally written.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	bfilestdfname

	Description:
	Test and figure out if the caller intent was to really get
	the standard-input for a BFILE file-open operation.

	Synopsis:
	cchar *bfilestfname(cchar *atf) noex

	Arguments:
	atf	file-name to check to see if a BFILE standard-input wanted

	Returns:
	-	a (possibly) new file-name that is recognized as BFILE STDIN

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstring>
#include	<usystem.h>
#include	<localmisc.h>

#include	"bfile.h"
#include	"bfilestdfname.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

cchar *bfilestfname(cchar *atf) noex {
        if ((atf == nullptr) || (atf[0] == '\0') || (atf[0] == '-')) {
            atf = BFILE_STDIN ;
	}
	return atf ;
}
/* end subroutine (bfilestdfname) */



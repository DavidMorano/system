/* sfdirname SUPPORT */
/* lang=C++20 */

/* get the directory part out of a file name path */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A�D� Morano
	This subroutine was originally written in order to provide
	an "SF" interface for the |dirname(3c)| type of function.
	This subroutine might have different (and possibly more
	desirable) semantics than |dirname(3c)| also.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sfdirname

	Description:
	This routine returns the directory portion of a file name path.

	Synopsis:
	int sfdirname(cchar *sp,int sl,cchar **rpp) noex

	Arguments:
	sp	given path string
	sl	length of given path string (can be -1)
	rpp	result pointer of beginning of found string

	Returns:
	>0	length of found string
	==0	not found (no dir-name)


	Examples:

	input		output		output_len
	-----------------------------------------------

	usr/lib		usr		3
	/usr/lib	/usr		4
	/usr/		/		1
	./something	.		1
	../something	..		2
	usr		-		0
	/		-		0
	.		-		0
	..		-		0
	//usr//		/		1

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstring>		/* <- for |strlen(3c)| */
#include	<utypedefs.h>
#include	<clanguage.h>

#include	"sfx.h"


/* local defines */


/* exported variables */


/* exported subroutines */

int sfdirname(cchar *sp,int sl,cchar **rpp) noex {
	int		rs ;
	int		i ;
	if (sl < 0) sl = strlen(sp) ;
	while ((sl > 0) && (sp[sl - 1] == '/'))  {
	    sl -= 1 ;
	}
	for (i = sl ; i > 0 ; i -= 1) {
	    if (sp[i - 1] == '/') break ;
	}
	if (rpp) {
	    *rpp = sp ;
	}
	if (i == 1) {
	    rs = 1 ;
	} else if (i <= 0) {
	    rs = 0 ;
	} else {
	    rs = (i - 1) ;
	}
	return rs ;
}
/* end subroutine (sfdirname) */



/* mkstrunique SUPPORT */
/* lang=C++11 */

/* test whether a string consists of all unique characters */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mkstrunique

	Description:
	We modify the given sring in place to remove any duplicates
	found.

	Synopsis:
	int mkstrunique(char *bp,int bl) noex

	Arguments:
	bp		string to test
	bl		length of string to test

	Returns:
	>=0		length of given string
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<mkchar.h>
#include	<localmisc.h>

#include	"mkx.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static void	bool_init(bool *,int) noex ;


/* local variables */


/* exported subroutines */

int mkstrunique(char *bp,int bl) noex {
	if (bl > 1) {
	    bool	seen[256] ;
	    int		tail = 1 ;
	    bool_init(seen,256) ;
	    while (bl-- && *bp) {
	        cint	ch = mkchar(*bp) ;
		if (!seen[ch]) {
		    bp[tail++] = ch ;
	            seen[ch] = true ;
		}
	        bp += 1 ;
	    } /* end while */
	    bp[tail] = '\0' ;
	} /* end if */
	return bl ;
}
/* end subroutine (mkstrunique) */


/* local subroutines */

static void bool_init(bool *bp,int bl) noex {
	for (int i = 0 ; i < bl ; i += 1) bp[i] = false ;
}
/* end subroutine (bool_init) */



/* sichr */

/* subroutine to find the index of a character in a given string */


/* revision history:

	= 1998-03-23, David A�D� Morano
	This subroutine was originally written.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	This subroutine searches for a character within a given string and
	returns the index to that character (if it is found).  It returns (-1)
	if the character does not exist within the given string.

	Synopsis:
	int sichr(sp,sl,sch)
	const char	sp[] ;
	int		sl ;
	int		sch ;

	Arguments:
	sp	string to be examined
	sl	length of string of break character to break on
	sch	character to search for

	Returns:
	>=0	index of search character in the given string
	<0	the search character was not found


*******************************************************************************/


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<string.h>
#include	<mkchar.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */


/* exported subroutines */

int sichr(const char *sp,int sl,int sch) {
	int		i ;
	int		f = FALSE ;
	sch &= 0xff ;
	for (i = 0 ; sl-- && sp[i] ; i += 1) {
	    int	ch = MKCHAR(sp[i]) ;
	    f = (ch == sch) ;
	    if (f) break ;
	} /* end for */
	return (f) ? i : -1 ;
}
/* end subroutine (sichr) */



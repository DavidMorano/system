/* strcpyxc */
/* lang=C++20 */

/* copy a text string converting to *converted* case */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	strcpylc

	Description:
	This subroutine converts a string to *lower* case.

	Synopsis:
	char *strcpylc(char *dst,cchar *src)

	Arguments:
	dst		pointer to destination buffer
	src		pointer to source buffer

	Returns:
	-		pointer to the end of the modified destination buffer


	Name:
	strcpyuc

	Description:
	This subroutine converts a string to *upper* case, otherwise identical
	to the above..


	Name:
	strcpyfc

	Description:
	This subroutine converts a string to *folded* case, otherwise identical
	to the above..


	Notes:
	Notes:
	1. Rationale for using templates rather than passing a 
	function-pointer:
	The thinking is that I want the template instantiation to
	"see" any inlined subroutine and optimize it all out rather
	than being forced to actually call a subroutine (if only a
	pointer was supplied). Anywat, that was the thinking, even
	if misguided. Also note that this goes against one of the
	major coding standard rules which is: to optimize for space
	rather than time (without the compiler doing that operation
	itself).


*******************************************************************************/


#include	<envstandards.h>	/* MUST be first to configure */
#include	<toxc.h>
#include	<localmisc.h>

#include	"strcpyxc.h"


/* local defines */


/* typedefs */

typedef int (*toxc_f)(int) ;


/* external subroutines */


/* local variables */


/* subroutine templates */

template <toxc_f toxc>
char *strcpyxc(char *dst,cchar *src) {
	while (*src != '\0') {
	    *dst++ = toxc(*src++) ;
	} /* end while */
	*dst = '\0' ;
	return dst ;
}
/* end subroutine-template (strcpyxc) */


/* exported subroutines */

char *strcpylc(char *dst,cchar *src) {
	return strcpyxc<tolc>(dst,src) ;
}
/* end subroutine (strcpylc) */

char *strcpyuc(char *dst,cchar *src) {
	return strcpyxc<touc>(dst,src) ;
}
/* end subroutine (strcpyuc) */

char *strcpyfc(char *dst,cchar *src) {
	return strcpyxc<tofc>(dst,src) ;
}
/* end subroutine (strcpyfc) */



/* wsixchr SUPPORT */
/* lang=C++20 */

/* search for a character in a wide-string */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	wsiochr
	wsirchr

	Description:
	Search for a character in a wide-string, either forward
	(obverse) or backwards (reverse), returning an index of the
	found location, if found, and -1 otherwise.

	Synopsis:
	int wsiochr(const wchar_t *sp,int sl,int sch) noex
	int wsirchr(const wchar_t *sp,int sl,int sch) noex

	Arguments:
	sp	the source wide-string that is to be copied
	sl	length of wide-string
	sch	the search character (as an integer)

	Returns:
	>=0	the character pointer to the end of the destination
	<0	character was not found

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<stddef.h>		/* for 'wchar_t' */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>

#include	"wsix.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local variables */


/* exported variables */


/* exported subroutines */

int wsiochr(const wchar_t *wsp,int wsl,int sch) noex {
	int		i ;
	bool		f = false ;
	if (wsl < 0) wsl = wsinul(wsp) ;
	for (i = 0 ; wsl-- && wsp[i] ; i += 1) {
	    f = (wsp[i] == sch) ;
	    if (f) break ;
	} /* end for */
	return (f) ? i : -1 ;
}
/* end subroutine (wsiochr) */

int wsirchr(const wchar_t *wsp,int wsl,int sch) noex {
	int		i ;
	bool		f = false ;
	if (wsl < 0) wsl = wsinul(wsp) ;
	for (i = (wsl-1) ; i >= 0 ; i -= 1) {
	    f = (wsp[i] == sch) ;
	    if (f) break ;
	} /* end for */
	return (f) ? i : -1 ;
}
/* end subroutine (wsirchr) */




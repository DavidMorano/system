/* sihyphen SUPPORT */
/* lang=C++20 */

/* find a hyphen (a fake hyphen of two minus characters) in a string */
/* version %I% last-modified %G% */

#define	CF_STRNCHR	0		/* use |strnchr(3dam)| */

/* revision history:

	= 1998-03-23, David A-D- Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sihyphen

	Description:
	This finds the string-index of a fake hyphen in the given
	string.  A fake hyphen is two minus characters in a row.

	Synopsis:
	int sihyphen(cchar *sp,int sl) noex

	Arguments:
	sp	string to be examined
	sl	length of string to be examined

	Returns:
	>=0	index of found hyphen
	-1	hyphen not found

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstring>		/* |strlen(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<ascii.h>
#include	<strn.h>
#include	<toxc.h>
#include	<toxc.h>
#include	<localmisc.h>

#include	"six.h"


/* local defines */


/* forward references */


/* local variables */


/* external subroutines */


/* exported subroutines */

#if	CF_STRNCHR
int sihyphen(cchar *sp,int sl) noex {
	int		si = 0 ;
	bool		f = false ;
	cchar		*tp ;
	if (sl < 0) sl = strlen(sp) ;
	while ((sl >= 2) && ((tp = strnchr(sp,sl,'-')) != nullptr) {
	    si += (tp-sp) ;
	    f = (((tp-sp)+1) < sl) && (tp[1] == '-') ;
	    if (f) break ;
	    si += 1 ;
	    sl -= ((tp+1)-sp) ;
	    sp = (tp+1) ;
	} /* end while */
	return (f) ? si : -1 ;
}
/* end subroutine (sihyphen) */
#else /* CF_STRNCHR */
int sihyphen(cchar *sp,int sl) noex {
	int		i ;
	bool		f = false ;
	if (sl < 0) sl = strlen(sp) ;
	for (i = 0 ; (i < (sl-1)) && sp[i] ; i += 1) {
	    f = (sp[i] == '-') && (sp[i + 1] == '-') ;
	    if (f) break ;
	} /* end for */
	return (f) ? i : -1 ;
}
/* end subroutine (sihyphen) */
#endif /* CF_STRNCHR */



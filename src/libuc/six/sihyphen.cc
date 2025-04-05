/* sihyphen SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* find a hyphen (a fake hyphen of two minus characters) in a string */
/* version %I% last-modified %G% */

#define	CF_STRNCHR	0		/* use |strnchr(3dam)| */

/* revision history:

	= 1998-03-23, David A-D- Morano
	This code was originally written.

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
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strlen(3c)| + |strnchr(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<ascii.h>
#include	<strn.h>
#include	<libutil.hh>		/* |xstrlen(3u)| */
#include	<toxc.h>
#include	<localmisc.h>

#include	"six.h"


/* local defines */

#ifndef	CF_STRNCHR
#define	CF_STRNCHR	0		/* use |strnchr(3dam)| */
#endif


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* forward references */


/* local variables */

cbool		f_strnchr = CF_STRNCHR ;


/* external subroutines */


/* exported subroutines */

int sihyphen(cchar *sp,int sl) noex {
	int		si = 0 ;
	bool		f = false ;
	if (sl < 0) sl = xstrlen(sp) ;
	if_constexpr (f_strnchr) {
	    cchar	*tp ;
	    while ((sl >= 2) && ((tp = strnchr(sp,sl,'-')) != nullptr)) {
	        si += (tp-sp) ;
	        f = (((tp-sp)+1) < sl) && (tp[1] == '-') ;
	        if (f) break ;
	        si += 1 ;
	        sl -= ((tp+1)-sp) ;
	        sp = (tp+1) ;
	    } /* end while */
	} else {
	    for (si = 0 ; (si < (sl-1)) && sp[si] ; si += 1) {
	        f = (sp[si] == '-') && (sp[si + 1] == '-') ;
	        if (f) break ;
	    } /* end for */
	} /* end if_constexpr (f_strnchr) */
	return (f) ? si : -1 ;
}
/* end subroutine (sihyphen) */



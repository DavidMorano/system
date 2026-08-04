/* strwhite SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* find the first white-space character in a c-string */
/* version %I% last-modified %G% */

#define	CF_STRBRK	1		/* use |strbrk(3c)| */

/* revision history:

	= 1998-03-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	strwhite

	Description:
	This subroutine will scan a string and return the first
	white-space-like character found.  This subroutine is really
	just a short-cut for something like:

		char *strpbrk(cchar *s," \t\v\f\r\n")

	Synopsis:
	char *strwhite(cchar *sp,int sl) noex
	char *strwhite(cchar *sp) noex
	char *strwht(cchar *sp,int sl) noex
	char *strwht(cchar *sp) noex

	Arguments:
	sp		search-string pointer
	sl		search-string length

	Returns:
	NULL		if no white space was found
	!= NULL		the pointer to the first white space character

	Notes:
	1. The existence of this subroutine (oringally |strwhite(3uc)|
	is historical.  Although it is not deprecated, it is rarely
	used and other alternatives exist.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD |strbrk(3c)| */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<ascii.h>		/* LIBU |CH_{x}| */
#include	<strn.h>		/* LIBUC |strnbrk(3uc)| */
#include	<char.h>		/* LIBUC |CHAR_{x}(3uc)| */
#include	<mkchar.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"strwhite.h"


/* local defines */

#ifndef	CF_STRBRK
#define	CF_STRBRK	0		/* use |strbrk(3c)| */
#endif

#define	ISW(ch)		CHAR_ISWHITE(ch)


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

constexpr char	whites[] = " \t\v\f\r\n" ;

cbool		f_strbrk = CF_STRBRK ;


/* exported variables */


/* exported subroutines */

char *strwht(cchar *sp,int sl) noex {
    	char		*rsp = nullptr ;
	if (sp) {
	    if_constexpr (f_strbrk) {
		if (sl < 0) {
		    rsp = strbrk(sp,whites) ;
		} else {
		    rsp = strnbrk(sp,sl,whites) ;
		}
	    } else {
	        for (int ch ; sl-- && ((ch = mkchar(*sp))) ; sp += 1) {
		    if (ISW(ch) || (ch == CH_NL)) break ;
	        }
	    } /* end if_constexpr (f_strbrk) */
	    rsp = charp(sp) ;
	} /* end if (non-null) */
	return rsp ;
} /* end subroutine (strwht) */



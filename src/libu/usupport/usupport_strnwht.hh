/* usupport_strnwht HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* search for a possible break-point among possiblw hite-space */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-06-08, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	strnwht

	Description:
	These subroutines search for a break-point that is either
	a given character or a character within a given c-string,
	or failing that, it finds the first occurance of white-space
	instead.  So preference is given to finding the given
	character first (either the specified single character or
	a chracter within a character-class), and only failing that
	is the first character of white-space returned.

	Synopsis:
	char *stnrwht(cchar *sp,int sl) noex
	char *strnwhtchr(cchar *sp,int sl,int sch) noex
	char *strnwhtbrk(cchar *sp,int sl,cchar *ss) noex
	char *strnwhtbrk(cchar *sp,int sl,chrset &sset) noex

*******************************************************************************/

#ifndef	USUPPORTSTRNWHT_INCLUDE
#define	USUPPORTSTRNWHT_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


namespace libu {
    extern char *strnwht	(cchar *,int) noex ;
    extern char *strnwhtchr	(cchar *,int,int) noex ;
    extern char *strnwhtbrk	(cchar *,int,cchar *) noex ;
    inline char *strnwhite	(cchar *sp,int sl) noex {
	return strnwht(sp,sl) ;
    }
} /* end namespace (libu) */


#endif /* __cplusplus */
#endif /* USUPPORTSTRNWHT_INCLUDE */



/* fnm HEADER (File-Name-Match) */
/* charset=ISO8859-1 */
/* lang=C20 */

/* vector of regular expressions */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-16, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	fnm

	Description:
	These are variables that describe how to call and then
	interpret the returns from the standard C-language
	library subroutine |fnmatch(3c)|.

*******************************************************************************/

#ifndef	FNM_INCLUDE
#define	FNM_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


struct fnmflags {
	statcexpr int	noesacpe	= FNM_NOESCAPE ;
	statcexpr int	pathname	= FNM_PATHNAME ;
	statcexpr int	period		= FNM_PERIOD ;
	statcexpr int	leadingdir	= FNM_LEADINGDIR ;
	statcexpr int	casefold	= FNM_CASEFOLD ;
	statcexpr int	augmented	= FNM_AUGMENTED ;
	statcexpr int	extmatch	= FNM_EXTMATCH ;
} ; /* end struct (fnmflags) */

struct fnmrets {
	statcexpr int	match		= FNM_RETMATCH ;
	statcexpr int	nomatch		= FNM_RETNOMATCH ;
	statcexpr int	error		= FNM_RETERROR ;
	statcexpr int	nosys		= FNM_RETNOSYS ;
} ; /* end struct (fnmrets) */

extern const fnmflags		fnmflag ;
extern const fnmrets		fnmret ;


#endif /* FNM_INCLUDE */



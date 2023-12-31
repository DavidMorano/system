/* fieldterm HEADER */
/* lang=C20 */

/* convenience object for handling "field" terminators ("terms") */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-05-01, David A�D� Morano
	This code module was originally written in VAX assembly
	(back around 1980 or so). It was then written into C-language
	perhaps around 1983 (at AT&T Bell Laboratories).  This
	(below) is a new incarnation based on that older C-language
	code.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	This is a classic, ported forward from the old VAX-11/70
	days. This was translated from VAX assembly language.

*******************************************************************************/

#ifndef	FIELDTERM_INCLUDE
#define	FIELDTERM_INCLUDE


#include	<envstandards.h>
#include	<climits>		/* <- for |UCHAR_MAX| + |CHAR_BIT| */
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<baops.h>
#include	<mkchar.h>


#ifdef	__cplusplus

constexpr int		fieldterm_termsize = ((UCHAR_MAX+1)/CHAR_BIT) ;

struct fieldterm {
	char		terms[fieldterm_termsize] = {} ;
	constexpr fieldterm(cchar *s) noex {
	    while (*s) {
		cint	ch = mkchar(*s++) ;
		baset(terms,ch) ;
	    }
	} ;
} ; /* end struct (fieldterm) */

#else /* __cplusplus */

static int		fieldterm_termsize = ((UCHAR_MAX+1)/CHAR_BIT) ;

#endif /* __cplusplus */


#endif /* FIELDTERM_INCLUDE */



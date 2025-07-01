/* fieldterminit HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* initialize a field-terminator block */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-05-01, David A­D­ Morano
	This code module was originally written in VAX assembly
	(back around 1980 or so). It was then written into C-language
	perhaps around 1983 (at AT&T Bell Laboratories).  This
	(below) is a new incarnation based on that older C-language
	code.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	fieldterminit

	Description:
	This object facilitates initializeing a field-terminator-block
	given a c-string of characters.

	Synopsis:
	constexpr fieldterminit	qterms("...xxx...") ;

	Returns:
	There are no returns as this object substitutes as a C++ type.

*******************************************************************************/

#ifndef	FIELDTERMINIT_INCLUDE
#define	FIELDTERMINIT_INCLUDE
#ifdef	__cplusplus /* everything is C++ only */


#include	<envstandards.h>	/* ordered first to configure */
#include	<climits>		/* <- for |UCHAR_MAX| + |CHAR_BIT| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<mkchar.h>
#include	<baops.h>


constexpr int	fieldterminit_size = ((UCHAR_MAX+1)/CHAR_BIT) ;

struct fieldterminit {
	char		terms[fieldterminit_size] = {} ;
	constexpr fieldterminit(cchar *s) noex {
	    while (*s) {
		cint	ch = mkchar(*s++) ;
		baset(terms,ch) ;
	    }
	} ; /* end ctor (fieldterminit) */
} ; /* end struct (fieldterminit) */


#endif	/* __cplusplus */
#endif /* FIELDTERMINIT_INCLUDE */



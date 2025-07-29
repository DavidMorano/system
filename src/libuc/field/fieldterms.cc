/* fieldterms SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* manage field-terminator blocks */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-05-01, David A­D­ Morano
	This code module was originally written in C language modeled
	(roughly) from a prior VAX assembly language version (written
	circa 1980 perhaps).  This is why this looks so "ugly"!
	This code comes from stuff dated back to almost the pre-dawn
	era of modern computer languages!  I wrote the original VAX
	assembly stuff also.  This code below was then written into
	C-language perhaps around 1983 (at AT&T Bell Laboratories).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	fieldterms

	Description:
	Initialize a field-terminator (character array) block.

	Synopsis:
	int fieldterms(char *terms,cchar *strterms) noex

	Arguments:
	terms		field-terminator character) block (of 32 bytes)
	strterms	c-string of terminator characters

	Returns:
	>=0	OK
	<0	error or not-found (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* <- for |UCHAR_MAX| + |CHAR_BIT| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdarg>
#include	<cstring>
#include	<usystem.h>
#include	<baops.h>
#include	<mkchar.h>
#include	<localmisc.h>

#include	"fieldterms.h"

import libutil ;

/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int fieldterms(char *terms,int f_retain,cchar *s) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (terms && s) {
	    if (! f_retain) {
		memclear(terms,fieldterms_termsize) ;
	    } /* end if */
	    while (*s) {
		cint	ch = mkchar(*s++) ;
	        baset(terms,ch) ;
	        c += 1 ;
	    } /* end while */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (fieldterms) */

int fieldtermsx(char *terms,int f,int na,...) noex {
	va_list		ap ;
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (terms) {
	    rs = SR_OK ;
	    if (! f) {
		memclear(terms,fieldterms_termsize) ;
	    } /* end if */
	    if (na > 0) {
		va_begin(ap,na) ;
	        for (int i = 0 ; i < na ; i += 1) {
		    int	ch = (int) va_arg(ap,int) ;
		    ch &= UCHAR_MAX ;
	            baset(terms,ch) ;
	            c += 1 ;
		} /* end for */
		va_end(ap) ;
	    } /* end if (non-zero-positive) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (fieldtermx) */



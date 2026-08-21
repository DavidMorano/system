/* ureserve6 MODULE (fieldterm - implementation) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* field terminator operations - fieldterm */
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

module ;

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* CSTD |UCHAR_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD |strchr(3c)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usupport.h>		/* LIBU */
#include	<ulogerror.h>		/* LIBU */
#include	<baops.h>		/* LIBU */
#include	<mkchar.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#pragma		GCC dependency		"mod/libutil.ccm"

module ureserve ;

import libutil ;			/* |memclear(3u)| */

/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* external variables */


/* exported subroutines */

int fieldterms(char *terms,int f_retain,cchar *s) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (terms && s) ylikely {
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
} /* end subroutine (fieldterms) */

int fieldtermsx(char *terms,int f,int na,...) noex {
	va_list		ap ;
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (terms) ylikely {
	    rs = SR_OK ;
	    if (! f) {
		memclear(terms,fieldterms_termsize) ;
	    } /* end if */
	    if (na > 0) ylikely {
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
} /* end subroutine (fieldtermx) */



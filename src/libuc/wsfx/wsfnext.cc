/* wsfnext SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* get the next field in a white-space dilineated record */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This code was originally written.

	= 2018-10-18, David A-D- Morano
	Enhanced (or modified) to be similar to |sfnext(3uc)|.

*/

/* Copyright (c) 1998,2018 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	wsfnext

	Description:
	This subroutine will extract the next white-space-separated
	field from the input buffer.  If an NL is encountered, it
	is treated like a delimiter and all characters before it
	(including zero characters) are returned as the delimited
	field.  The caller should test for zero characters being
	returned.  This condition could indicate either that the
	line has no additional characters, or that a NL was
	encountered.  If the caller determines that there are
	characters left in the line but that zero was returned, a
	check by the caller for a NL character is indicated.  If
	the caller determines that a NL was present, the caller
	should arrange to "step over" the NL before making new calls
	in order to get the fields beyond the NL.

	Synopsis:
	int wsfnext(const wchar_t *wsp,int wsl,const wchar_t **rpp) noex

	Arguments:
	wsp		pointer to start of user supplied buffer
	wsl		length of user supplied buffer
	rpp		pointer to pointer of the found field

	Returns:
	>0		length of found field
	==0		no field found or a NL character was encountered

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstring>		/* |UCHAR_MAX| */
#include	<cstddef>		/* |nullptr_t| + |wchar_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<char.h>		/* |CHAR_ISWHITE(3uc)| */
#include	<localmisc.h>


/* local defines */


/* imported namespaces */


/* local typedefs */

#ifndef	CWCHAR_TYPEDEF
#define	CWCHAR_TYPEDEF
typedef const wchar_t		cwchar ;
#endif


/* external subroutines */


/* external variables */


/* forward references */

static bool iswwhite(cwchar wch) noex {
	return (((wch) <= UCHAR_MAX) && CHAR_ISWHITE(wch)) ;
}


/* local variables */


/* exported variables */


/* exported subroutines */

int wsfnext(cwchar *wsp,int wsl,cwchar **rpp) noex {
    	int		rl = -1 ;
	cwchar		*rp = nullptr ;
	if (wsp && rpp) {
	    int		ch ; /* used-multiple */
	    while (wsl && *wsp) {
	        ch = int(*wsp) ;
	        if (! iswwhite(ch)) break ;
	        wsp += 1 ;
	        wsl -= 1 ;
	    } /* end while */
	    rp = wsp ;
	    /* skip the non-white space */
	    while (wsl && *wsp && (*wsp != '\n')) {
	        ch = int(*wsp) ;
	        if  (iswwhite(ch)) break ;
	        wsp += 1 ;
	        wsl -= 1 ;
	    } /* end while */
	    rl = intconv(wsp - rp) ;
	    *rpp = rp ;
	} /* end if (non-null) */
	return rl ;
}
/* end subroutine (wsfnext) */



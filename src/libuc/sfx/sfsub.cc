/* sfsub SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* match a substring within a larger string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A�D� Morano
	This code was originally written.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sfsub

	Description:
	This subroutine determines if the parameter string (argument
	'ss') is or is not in the buffer specified by the first two
	arguments.  This subroutine either returns (-1) or it returns
	the length of the found substring.

	Synopsis:
	int sfsub(cchar *sbuf,int slen,cchar *ss,cchar **rpp) noex

	Arguments:
	sbuf	string to be examined
	slen	length of string to be examined
	ss	null terminated substring to search for
	rpp	result pointer of beginning of found string

	Returns:
	>=0	length of found substring
	<0	the substring was not found in the main string buffer

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* <- for |strlen(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<strnxcmp.h>
#include	<localmisc.h>

#include	"sfx.h"

import libutil ;

/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int sfsub(cchar *sp,int sl,cchar *ss,cchar **rpp) noex {
	int		sslen = 0 ;
	bool		f = false ;
	if (sp && ss) {
	    int		i = 0 ;
	    f = true ;
	    sslen = lenstr(ss) ;
	    if (sslen > 0) {
		f = false ;
	        if (sl < 0) sl = lenstr(sp) ;
	        if (sslen <= sl) {
	            for (i = 0 ; i <= (sl-sslen) ; i += 1) {
		        f = ((sslen == 0) || (sp[i] == ss[0])) ;
	                f = f && (strncmp((sp+i),ss,sslen) == 0) ;
	                if (f) break ;
	            } /* end for */
	        } /* end if (possible) */
	    } /* end if (non-empty) */
	    if (rpp) {
	        *rpp = ((f) ? (sp+i) : nullptr) ;
	    }
	} /* end if (non-null) */
	return (f) ? sslen : -1 ;
}
/* end subroutine (sfsub) */



/* issamehostname SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* rough equivalency hostname check */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-21, David A­D­ Morano
	This program was started by copying from the RSLOW program.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	issamehostname

	Description:
	This subroutine can be used to determine if two hosts are
	really the same host because one may be qualified with the
	local domain and the other may not be.

	Synopsis:
	int issamehostname(cc *h1,cc *h2,cc *localdomain) noex

	Arguments:
	h1		one host name
	h2		another host name
	localdomain	the local domain name

	Returns:
	true		the two host names are the same
	false		the two host names are different

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<strings.h>		/* |strncasecmp(3c)| + |strlen(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<localmisc.h>		/* |LEQUIV(3dam)| */

#include	"ismisc.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct samehelp {
	cchar		*h1 ;
	cchar		*h2 ;
	cchar		*localdomain ;
	cchar		*cp1 ;
	cchar		*cp2 ;
	samehelp(cchar *ah1,cchar *ah2,cchar *ald) noex {
	    h1 = ah1 ;
	    h2 = ah2 ;
	    localdomain = ald ;
	} ;
	operator bool () noex ;
	bool tryh1() noex ;
	bool tryh2() noex ;
    } ; /* end struct (samehelp) */
}


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

bool issamehostname(cchar *h1,cchar *h2,cchar *localdomain) noex {
	samehelp	ho(h1,h2,localdomain) ;
	return ho ;
}
/* end subroutine (issamehostname) */


/* local subroutines */

samehelp::operator bool () noex {
	bool		f = false ;
	bool		f_h1 = false ;
	bool		f_h2 = false ;
	if (((cp1 = strchr(h1,'.')) != nullptr) && (cp1[1] != '\0')) {
		f_h1 = true ;
	}
	if (((cp2 = strchr(h2,'.')) != nullptr) && (cp2[1] != '\0')) {
		f_h2 = true ;
	}
	if (LEQUIV(f_h1,f_h2)) {
	    f = (strcasecmp(h1,h2) == 0) ;
	} else {
	    if (f_h1) {
	        f = tryh1() ;
	    } else {
	        f = tryh2() ;
	    }
	}
	return f ;
}

bool samehelp::tryh1() noex {
	cint		len1 = cp1 - h1 ;
	cint		len2 = strlen(h2) ;
	bool		f = false ;
	if (len1 == len2) {
	    cp1 += 1 ;
	    if (strcasecmp(cp1,localdomain) == 0) {
		f = (strncasecmp(h1,h2,len1) == 0) ;
	    }
	}
	return f ;
} 
/* end method (tryh1) */

bool samehelp::tryh2() noex {
	cint		len1 = strlen(h1) ;
	cint		len2 = cp2 - h2 ;
	bool		f = false ;
	if (len1 == len2) {
	    cp2 += 1 ;
	    if (strcasecmp(cp2,localdomain) == 0) {
		f = (strncasecmp(h1,h2,len2) == 0) ;
	    }
	} /* end if */
	return f ;
}
/* end method (tryh2) */



/* issamehostname SUPPORT */
/* lang=C++20 */

/* rough equivalent host name check */
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
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstdlib>
#include	<strings.h>		/* |strncasecmp(3c)| */
#include	<netdb.h>
#include	<usystem.h>
#include	<localmisc.h>		/* |LEQUIV(3dam)| */

#include	"ismisc.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward subroutines */

extern "C" {
    int samehost(cchar *,cchar *,cchar *) noex ;
}


/* local variables */


/* exported variables */


/* exported subroutines */

bool issamehostname(cchar *h1,cchar *h2,cchar *localdomain) noex {
	int		len1, len2 ;
	int		f_h1 = false ;
	int		f_h2 = false ;
	cchar		*cp1, *cp2 ;

	if (((cp1 = strchr(h1,'.')) != NULL) && (cp1[1] != '\0'))
		f_h1 = true ;

	if (((cp2 = strchr(h2,'.')) != NULL) && (cp2[1] != '\0'))
		f_h2 = true ;

	if (LEQUIV(f_h1,f_h2))
	    return (! strcasecmp(h1,h2)) ;

	if (f_h1) {

	    len1 = cp1 - h1 ;
	    len2 = strlen(h2) ;

	    if (len1 != len2) 
		return false ;

	    cp1 += 1 ;
	    if (strcasecmp(cp1,localdomain) != 0)
		return false ;

	    return (strncasecmp(h1,h2,len1) == 0) ;

	} /* end if */

	len1 = strlen(h1) ;

	len2 = cp2 - h2 ;
	if (len1 != len2) 
		return false ;

	cp2 += 1 ;
	if (strcasecmp(cp2,localdomain) != 0) 
		return false ;

	return (strncasecmp(h1,h2,len2) == 0) ;
}
/* end subroutine (samehost) */

#if	CF_SAMEHOST

int samehostname(cchar *h1,cchar *h2,cchar *localdomain) noex {
	return issamehostname(h1,h2,localdomain) ;
}

int samehost(cchar *h1,cchar *h2,cchar *localdomain) noex {
	return issamehostname(h1,h2,localdomain) ;
}

#endif /* CF_SAMEHOST */



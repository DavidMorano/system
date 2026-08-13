/* stdclib SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* utility subroutines */
/* version %I% last-modified %G% */


/* revision history:

	= 2023-10-08, David A-D- Morano
	This is written as some interim hack for some code that
	cannot otherwise be linked with the main LIBUC code library.

*/

/* Copyright © 2023 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

	Group:
	stdclib

	Description:

	Synopsis:

	Arguments:

	Returns:

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstrings>		/* CSTD |str{x}casecmp(3c)| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD |strcmp(3c)| */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU |DIGBUFLEN| */

#include	"stdclib.hh"


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

namespace stdclib {
    int std_strbasecmp(cchar *s1,cchar *s2) noex {
	int rc = 0 ;
	if (s1 && s2) ylikely {
	    rc = strcmp(s1,s2) ;
	} /* end if (non-null) */
	return rc ;
    } /* end subroutine */
    int std_strcasecmp(cchar *s1,cchar *s2) noex {
	int rc = 0 ;
	if (s1 && s2) ylikely {
	    rc = strcasecmp(s1,s2) ;
	} /* end if (non-null) */
	return rc ;
    } /* end subroutine */
} /* end namespace (stdclib) */

namespace stdclib {
    int std_strnbasecmp(cchar *s1,cchar *s2,int n) noex {
	int rc = 0 ;
	if (s1 && s2) ylikely {
	    csize nsize = size_t(n) ;
	    rc = strncmp(s1,s2,nsize) ;
	} /* end if (non-null) */
	return rc ;
    } /* end subroutine */
    int std_strncasecmp(cchar *s1,cchar *s2,int n) noex {
	int rc = 0 ;
	if (s1 && s2) ylikely {
	    csize nsize = size_t(n) ;
	    rc = strncasecmp(s1,s2,nsize) ;
	} /* end if (non-null) */
	return rc ;
    } /* end subroutine */
} /* end namespace (stdclib) */


/* local subroutines */



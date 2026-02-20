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
#include	<strings.h>		/* |strcasecmp(3c)| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strcmp(3c)|*/
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<localmisc.h>		/* |DIGBUFLEN| */

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
	return strcmp(s1,s2) ;
    }
    int std_strcasecmp(cchar *s1,cchar *s2) noex {
	return strcasecmp(s1,s2) ;
    }
} /* end namespace (stdclib) */

namespace stdclib {
    int std_strnbasecmp(cchar *s1,cchar *s2,int n) noex {
	return strncmp(s1,s2,n) ;
    }
    int std_strncasecmp(cchar *s1,cchar *s2,int n) noex {
	return strncasecmp(s1,s2,n) ;
    }
} /* end namespace (stdclib) */


/* local subroutines */



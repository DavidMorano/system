/* putheap SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* put a c-string into the heap */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* must be ordered first to configure */
#include	<sys/types.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<localmisc.h>


/* local defines */

#define		HEAPSIZE	1024


/* local variables */

static int	heap_len = 0 ;

static char	*heap_start ;
static char	*heap_current ;


/* exported variables */


/* exported subroutines */

char *putheap(cchar *s) noex {
	int	l = lenstr(s) + 1 ;
	/* allocate more space if necessary */
	if (heap_len < l) {
	    heap_len = (HEAPSIZE > l) ? HEAPSIZE : l ;
	    if ((heap_start = (char *) malloc(heap_len)) == nullptr)
	        return nullptr ;

	    heap_current = heap_start ;
	}

	/* store the string */

	strcpy(heap_current,s) ;

	heap_len -= l ;
	heap_current += l ;
	return (heap_current - l) ;
}
/* end subroutine (putheap) */



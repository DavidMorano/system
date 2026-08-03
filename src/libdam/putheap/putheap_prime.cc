/* putheap_prime SUPPORT */
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
#include	<sys/types.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"putheap.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#define	HEAPSIZE	1024


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

local int	heap_len ;

local char	*heap_start ;
local char	*heap_current ;


/* exported variables */


/* exported subroutines */

char *putheap(cchar *s) noex {
    	cnullptr	np{} ;
    	char		*rp = nullptr ;
	if (s) ylikely {
	    cint	l = lenstr(s) + 1 ;
	    /* allocate more space if necessary */
	    if (heap_len < l) {
	        heap_len = (HEAPSIZE > l) ? HEAPSIZE : l ;
	        if ((heap_start = (char *) malloc(heap_len)) != np) {
	            heap_current = heap_start ;
	        }
	    }
	    if (heap_start) {
	        /* store the string */
	        strcpy(heap_current,s) ;
	        heap_len -= l ;
	        heap_current += l ;
	        rp = charp(heap_current - l) ;
	    } /* end if (non-null) */
	} /* end if (non-null) */
	return rp ;
} /* end subroutine (putheap) */



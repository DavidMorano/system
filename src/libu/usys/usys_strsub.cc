/* usys_strsub SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* miscelllaneous (SNX) operating system support */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-06-16, David A­D­ Morano
	This subroutine was written to get some commonality across
	our most used operating systems. 

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Names:
	strsub
	strsubn

	Description:
	These are string-find operations.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD |strstr(3c)| + |strnstr(3c) */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"usys_strnstr.h"	/* in case system does not have */
#include	"usys_strsub.hh"


namespace usys {
    extern char *strsub		(cchar *sp,cchar *ss)		noex {
    	return strstr(sp,ss) ;
    } /* end */
} /* end namespace (usys) */

namespace usys {
    extern char *strsubn	(cchar *sp,cchar *ss,int n)	noex {
    	csize nsize = size_t(n) ;
    	return strnstr(sp,ss,nsize) ;
    } /* end */
} /* end namespace (usys) */



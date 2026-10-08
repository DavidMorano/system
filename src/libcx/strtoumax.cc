/* strtoumax SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* C-language c-string support */
/* version %I% last-modified %G% */


/* revision history:

	= 2017-09-07, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2017 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* ordered first to configure */
#include	<inttypes.h>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	<strtoxmax.h>


#if defined(_LP64)

uintmax_t strtoumax(cchar *str,char **endptr,int base)  {
	return strtoul(str,endptr,base) ;
} /* end */

uintmax_t strtouintmax(cchar *str,char **endptr,int base)  {
	return strtoul(str,endptr,base) ;
} /* end */

#else /* defined(_LP64) */

uintmax_t strtoumax(cchar *str,char **endptr,int base)  {
	return strtoull(str,endptr,base) ;
} /* end */

uintmax_t strtouintmax(cchar *str,char **endptr,int base)  {
	return strtoull(str,endptr,base) ;
} /* end */

#endif /* defined(_LP64) */



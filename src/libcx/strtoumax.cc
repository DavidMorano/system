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
#include	<inttypes.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usyscalls.h>
#include	<localmisc.h>

#include	<strtoxmax.h>


#if defined(_LP64)

uintmax_t strtoumax(cchar *str,char **endptr,int base)  {
	return strtoul(str,endptr,base) ;
}

uintmax_t strtouintmax(cchar *str,char **endptr,int base)  {
	return strtoul(str,endptr,base) ;
}

#else /* defined(_LP64) */

uintmax_t strtoumax(cchar *str,char **endptr,int base)  {
	return strtoull(str,endptr,base) ;
}

uintmax_t strtouintmax(cchar *str,char **endptr,int base)  {
	return strtoull(str,endptr,base) ;
}

#endif /* defined(_LP64) */



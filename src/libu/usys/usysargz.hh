/* usysargx HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* retrievd the ARGZ value from the system for the current process */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-03, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	USYSARGZ_INCLUDE
#define	USYSARGZ_INCLUDE
#ifdef	__cplusplus /* C++ only! */


#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<atomic>		/* |once_flag(3c++)| */
#include	<mutex>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>

using std::once_flag ;

namespace usys {
    struct usysargz {
    	char		*name{} ;
	once_flag	ox ;
	int		nlen ;
	int		rss = SR_OK ;
	usysargz() = default ;
	operator int () noex ;
	int get(char *,int) noex ;
	void dtor() noex ;
	destruct usysargz() {
	    if (name) dtor() ;
	} ;
	void initonce() noex ;
    } ; /* end struct (usysargz) */
} /* end namespace (usys) */


#endif /* __cplusplus (C++ only) */
#endif /* USYSARGZ_INCLUDE */



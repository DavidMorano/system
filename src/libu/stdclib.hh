/* stdclib HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* LIBU system utility subroutines */
/* version %I% last-modified %G% */


/* revision history:

	= 2023-10-08, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	STDCLIB_INCLUDE
#define	STDCLIB_INCLUDE
#ifdef	__cplusplus /* C++ only) */


#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>


namespace stdclib {
    extern int	std_strbasecmp(cchar *,cchar *) noex ;
    extern int	std_strcasecmp(cchar *,cchar *) noex ;
} /* end namespace (stdclib) */

namespace stdclib {
    extern int	std_strnbasecmp(cchar *,cchar *,int) noex ;
    extern int	std_strncasecmp(cchar *,cchar *,int) noex ;
} /* end namespace (stdclib) */


#endif /* __cplusplus (C++ only) */
#endif /* STDCLIB_INCLUDE */



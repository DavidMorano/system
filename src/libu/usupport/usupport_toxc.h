/* usupport_toxc HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* convert a single character to a given case */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	USUPPORTTOXC_INCLUDE
#define	USUPPORTTOXC_INCLUDE
#ifdef	__cplusplus /* (C++ only) */


#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* |UCHAR_MAX| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>


namespace libu {
    inline int tobc(int ch) noex {
	return (ch & UCHAR_MAX) ;
    }
    extern int tolc(int ch) noex ;
    extern int touc(int ch) noex ;
    extern int tofc(int ch) noex ;
    inline char chtobc(int ch) noex {
	return char(tobc(ch)) ;
    }
    inline char chtolc(int ch) noex {
	return char(tolc(ch)) ;
    }
    inline char chtouc(int ch) noex {
	return char(touc(ch)) ;
    }
    inline char chtofc(int ch) noex {
	return char(tofc(ch)) ;
    }
} /* end namespace (libu) */


#endif /* __cplusplus (C++ only) */
#endif /* USUPPORTTOXC_INCLUDE */



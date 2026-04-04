/* udiv HEADER (Unsigned-Division) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* unsigned division */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	UDIV_INCLUDE
#define	UDIV_INCLUDE
#ifdef	__cplusplus /* C++ only */


#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>
#include	<cstdlib>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<stdintx.h>


template<typename TU = uint>
struct udiv {
    TU quo ;
    TU rem ;
    void div(TU nom,TU dem) noex {
	quo = nom / dem ;
	rem = nom % dem ;
    } ; /* end method */
    udiv() = default ;
    udiv(TU n,TU d) noex {
	div(n,d) ;
    } ;
    void operator () (TU n,TU d) noex {
	div(n,d) ;
    } ;
} ; /* end struct (udiv) */


#endif /* __cplusplus */
#endif /* UDIV_INCLUDE */



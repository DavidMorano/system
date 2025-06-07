/* timecount HEADER */
/* encoding=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* count-down timer */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-11, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 2001 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	timecount

	Description:
	This object provides a glorified count-down timer.

*******************************************************************************/

#ifndef	TIMECOUNT_INCLUDE
#define	TIMECOUNT_INCLUDE
#ifdef	__cplusplus /* C++ only! */


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#ifndef	POLL_INTMULT
#define	POLL_INTMULT	1000
#endif


class timecount {
	int		to ;		/* milliseconds */
public:
	timecount(int a) : to(a * POLL_INTMULT) { } ;
	timecount(const timecount &) = delete ;
	timecount &operator = (const timecount &) = delete ;
	operator int () noex ;
} ; /* end class (timecount) */


#endif /* __cplusplus (C++ only) */
#endif /* TIMECOUNT_INCLUDE */



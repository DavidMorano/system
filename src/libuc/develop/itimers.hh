/* itimers HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* constants used in identifying internal-timers */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-13, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	itimer

	Description:
	This object simply provides the identifying values used in
	selecting a process interval-timer.

*******************************************************************************/

#ifndef	ITIMERS_INCLUDE
#define	ITIMERS_INCLUDE
#ifdef	__cplusplus	/* C++ only! */


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


struct itimers {
    constexpr static int	real		= ITIMER_REAL ;
    constexpr static int	virt		= ITIMER_VIRTUAL ;
    constexpr static int	prof		= ITIMER_PROF ;
} ; /* end struct (timers) */

extern const itimers	itimer ;


#endif /* __cplusplus */
#endif /* ITIMERS_INCLUDE */



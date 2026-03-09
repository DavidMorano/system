/* clockids HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* provides identifiers for UINX® system clock IDs */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-13, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	clockids

	Description:
	This object simply provides the identifying values used in
	selecting a UNIX® system clock.

*******************************************************************************/

#ifndef	CLOCKIDS_INCLUDE
#define	CLOCKIDS_INCLUDE
#ifdef	__cplusplus	/* C++ only! */


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


struct clockids {
    static const clockid_t	real		;
    static const clockid_t	montonic	;
    static const clockid_t	montonicraw	;
    static const clockid_t	montonicrawappr	;
    static const clockid_t	cpuprocess	;
    static const clockid_t	cputhread	;
    static const clockid_t	boot		;
    static const clockid_t	bootappr	;
} ; /* end struct (clockids) */

extern const clockids		clockid ;


#endif /* __cplusplus (C++ only) */
#endif /* CLOCKIDS_INCLUDE */



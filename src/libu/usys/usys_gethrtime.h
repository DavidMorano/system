/* usys_gethrtime HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* miscelllaneous (XXX) operating system support */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This file contains the UNIX® system types that the brain-damaged
	MacOS operating system does NOT have.  We are trying in a very
	small way to make up for some of the immense brain-damage within
	the Apple Darwin operating system.

*******************************************************************************/

#ifndef	USYSGETHRTIME_INCLUDE
#define	USYSGETHRTIME_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/time.h>		/* POSIX® <- |gethrtime(3c)| */
#include	<time.h>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


/* GETHRTIME begin */
#if	defined(SYSHAS_GETHRTIME) && (SYSHAS_GETHRTIME > 0)

/* » nothing needed « */

#else /* defined(SYSHAS_GETHRTIME) && (SYSHAS_GETHRTIME > 0) */

#ifndef	TYPEDEF_HRTIME
#define	TYPEDEF_HRTIME

typedef unsigned long		hrtime_t ;

#endif /* TYPEDEF_HRTIME */

#ifndef	DECLARATION_GETHRTIME
#define	DECLARATION_GETHRTIME
EXTERNC_begin
extern hrtime_t gethrtime() noex ;
EXTERNC_end
#endif /* DECLARATION_GETHRTIME */

#endif /* defined(SYSHAS_GETHRTIME) && (SYSHAS_GETHRTIME > 0) */
/* GETHRTIME end */


#endif /* USYSGETHRTIME_INCLUDE */



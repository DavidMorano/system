/* proginfo HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* program information */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-13, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	proginfo

	Description:
	This object simply provides the identifying values used in
	selecting a process interval-timer.

*******************************************************************************/

#ifndef	PROGINFO_INCLUDE
#define	PROGINFO_INCLUDE
#ifdef	__cplusplus	/* C++ only! */


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


#define	PI		proginfo
#define	PI_FL		proginfo_fl
#define	PI_VALS		proginfo_vals

/* this is nothing in particular (from anyway in particular) */
#ifndef	NOPROGNAME
#define	NOPROGNAME	"NP"
#endif

#ifndef	LOGCNAME
#define	LOGCNAME	"log"
#endif


#endif /* __cplusplus (C++ only) */
#endif /* PROGINFO_INCLUDE */



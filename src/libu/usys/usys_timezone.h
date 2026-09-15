/* usys_timezone HEADER */
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

  	Description:
	This file contains the UNIX® system types that the brain-damaged
	MacOS operating system does NOT have.  We are trying in a very
	small way to make up for some of the immense brain-damage within
	the Apple Darwin operating system.

*******************************************************************************/

#ifndef	USYSTIMEZONE_INCLUDE
#define	USYSTIMEZONE_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/time.h>		/* POSIX® |gettimeofday(3c)| */
#include	<sys/timeb.h>		/* |ftime(3c)| */
#include	<unistd.h>		/* POSIX® for Solaris® */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */

/* STIME begin */
#if	defined(SYSHAS_TIMEZONE) && (SYSHAS_TIMEZONE > 0)
/******************************************************************************/


/* SOeone (Apple-Darwin) already has the |timezone| structure */


/******************************************************************************/
#else /* defined(SYSHAS_TIMEZONE) && (SYSHAS_TIMEZONE > 0) */
/******************************************************************************/


#ifndef	TIMEZONE_STRUCTURE
#define	TIMEZONE_STRUCTURE
struct timezone {
	int     tz_minuteswest ; /* minutes west of Greenwich */
	int     tz_dsttime ;     /* type of dst correction to apply */
} ; /* end struct (timezone) */
#endif /* DECLARATION_TIMEZONE */


/******************************************************************************/
#endif /* defined(SYSHAS_TIMEZONE) && (SYSHAS_TIMEZONE > 0) */
/* STIME end */


#endif /* USYSTIMEZONE_INCLUDE */



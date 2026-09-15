/* usys_stime HEADER */
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

#ifndef	USYSSTIME_INCLUDE
#define	USYSSTIME_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/time.h>		/* POSIX® |gettimeofday(3c)| */
#include	<unistd.h>		/* POSIX® for Solaris® */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */

/* STIME begin */
#if	defined(SYSHAS_STIME) && (SYSHAS_STIME > 0)
/******************************************************************************/


/* SunOS (Solaris®) already has the |stime(2)| system call */


/******************************************************************************/
#else /* defined(SYSHAS_STIME) && (SYSHAS_STIME > 0) */
/******************************************************************************/


#ifndef	DECLARATION_STIME
#define	DECLARATION_STIME
EXTERNC_begin

extern unixret_t stime(const time_t *) noex ;

EXTERNC_end
#endif /* DECLARATION_STIME */


/******************************************************************************/
#endif /* defined(SYSHAS_STIME) && (SYSHAS_STIME > 0) */
/* STIME end */


#endif /* USYSSTIME_INCLUDE */



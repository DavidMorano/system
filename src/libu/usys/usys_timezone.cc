/* usys_timezone SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* UNIX® system emulated support */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-11, David A-D- Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 2001 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

	We defines some system (global) variables in this module.

	Important note:
	It looks like only SunOS (really Solaris®) and Illumos®
	have the |stime(2)| system call.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/time.h>		/* type |timeval| */
#include	<sys/timeb.h>		/* type |ftime(3c)| */
#include	<ctime>			/* CSTD */
#include	<cerrno>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */

#include	"usys_timezone.h"

/* TIMEZONE start */
#if	defined(SYSHAS_TIMEZONE) && (SYSHAS_TIMEZONE > 0)
/******************************************************************************/

/* nothing */

/******************************************************************************/
#else /* defined(SYSHAS_TIMEZONE) && (SYSHAS_TIMEZONE > 0) */
/******************************************************************************/

/* nothing */

/******************************************************************************/
#endif /* (!defined(SYSHAS_TIMEZONE)) || (SYSHAS_TIMEZONE == 0) */
/* TIMEZONE end */



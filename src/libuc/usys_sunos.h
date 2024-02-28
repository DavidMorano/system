/* usys_sunos HEADER */
/* lang=C20 */

/* Apple Darwin operating system support */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

	= 2017-08-01, David A­D­ Morano
	Updated for lack of interfaces in MacOS Darwin

*/

/* Copyright © 1998,2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This file contains the UNIX® system types and preprocessor symbols
	that may not be provided.

*******************************************************************************/

#ifndef	USYS_SUNOS_INCLUDE
#define	USYS_SUNOS_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */

/* USYS_SUNOS start */
#if	defined(OSNAME_SunOS) && (OSNAME_SunOS > 0)

#include	<sys/types.h>
#include	<sys/wait.h>		/* <- type |idtype_t| is there */
#include	<sys/time.h>		/* <- |TIMESPEC| is there */
#include	<signal.h>
#include	<pthread.h>
#include	<time.h>
#include	<clanguage.h>

#include	<sys/loadavg.h>		/* <- for |__getloadavg(2)| */
#include	<sys/lock.h>		/* <- for |plock(2)| */
#include	<user_attr.h>		/* <- money shot (one of) */


/*----------------------------------------------------------------------------*/
/* USERATTR begin */
#if	(!defined(SYSHAS_USERATTR)) || (SYSHAS_USERATTR == 0)

#ifndef	TYPEDEF_USERATTR
#define	TYPEDEF_USERATTR
typedef userattr_t	userattr ;
#endif

#endif /* (!defined(SYSHAS_USERATTR)) || (SYSHAS_USERATTR == 0) */
/* USERATTR end */
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/* UCTIMEOUT begin */

#ifndef	SIGTIMEOUT
#define	SIGTIMEOUT	SIGRTMAX	/* hack for now */
#endif

/* UCTIMEOUT end */
/*----------------------------------------------------------------------------*/


#endif /* defined(OSNAME_SunOS) && (OSNAME_SunOS > 0) */
/* USYS_SUNOS finish */


#endif /* USYS_SUNOS_INCLUDE */



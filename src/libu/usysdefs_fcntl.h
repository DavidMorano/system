/* usysdefs_fcntl HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* these are (a multitude) of various UNIX® system pre-processor defines */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-09, David A-D- Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

	Description:
	This file defines various preprocessor variables for use
	in setting up the UNIX®-system default for various buffer
	sizes.  The real run-time system buffer sizes are (indeed)
	determined at runtime where they may exist.

	Usage:
	Among the most important uses for the data contained herein
	is the BUFSIZE management facility.  It uses the data below
	to set default buffer sizes for those systems that do not
	have dynamicly managed buffer size support in the system
	kernel.

*******************************************************************************/

#ifndef	USYSDEFSFCNTL_INCLUDE
#define	USYSDEFSFCNTL_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/utsname.h>		/* POSIX® */
#include	<sys/param.h>		/* POSIX® */
#include	<sys/stat.h>		/* |S_IS{x}| + S_IF{x}| */
#include	<limits.h>		/* |{xxx}_MIN| + |{xxx}_MAX| */
#include	<signal.h>		/* |SIG{x}| */
#include	<unistd.h>		/* |_SC_{x}| + |_PC_{x}| */
#include	<fcntl.h>		/* |O_{x}| */


/* extra system flags for |uc_lockfile(3uc)| */
#ifndef	F_UNLOCK
#define	F_UNLOCK	F_ULOCK
#endif
#ifndef	F_WLOCK	
#define	F_WLOCK		F_LOCK
#endif
#ifndef	F_TWLOCK
#define	F_TWLOCK	F_TLOCK
#endif
#ifndef	F_WTEST
#define	F_WTEST		F_TEST
#endif
#ifndef	F_RLOCK
#define	F_RLOCK		10		/* new! (watch UNIX® for changes) */
#endif
#ifndef	F_TRLOCK
#define	F_TRLOCK	11		/* new! (watch UNIX® for changes) */
#endif
#ifndef	F_RTEST
#define	F_RTEST		12		/* new! (watch UNIX® for changes) */
#endif


#endif /* USYSDEFSFCNTL_INCLUDE */



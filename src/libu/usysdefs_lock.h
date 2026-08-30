/* usysdefs_lock HEADER */
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

#ifndef	USYSDEFSLOCK_INCLUDE
#define	USYSDEFSLOCK_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/utsname.h>		/* POSIX® */
#include	<sys/param.h>		/* POSIX® */
#include	<sys/stat.h>		/* |S_IS{x}| + S_IF{x}| */
#include	<limits.h>		/* |{xxx}_MIN| + |{xxx}_MAX| */
#include	<signal.h>		/* |SIG{x}| */
#include	<unistd.h>		/* |_SC_{x}| + |_PC_{x}| */
#include	<fcntl.h>		/* |O_{x}| */


enum missingplocks {
	missingplock_unlock,
	missingplock_txtlock,
	missingplock_datlock,
	missingplock_proclock,
	missingplock_overlast
} ; /* end enum */

/* possibly missing aommand-operations for |plock(2solaris)| */
#ifndef	UNLOCK
#define	UNLOCK		missingplock_unlock
#endif
#ifndef	TXTLOCK
#define	TXTLOCK		missingplock_txtlock
#endif
#ifndef	DATLOCK
#define	DATLOCK		missingplock_datlock
#endif
#ifndef	PROCLOCK
#define	PROCLOCK	missingplock_proclock
#endif


#endif /* USYSDEFSLOCK_INCLUDE */



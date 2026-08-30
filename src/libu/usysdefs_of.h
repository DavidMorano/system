/* usysdefs_of HEADER */
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

#ifndef	USYSDEFSOF_INCLUDE
#define	USYSDEFSOF_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/utsname.h>		/* POSIX® */
#include	<sys/param.h>		/* POSIX® */
#include	<sys/stat.h>		/* |S_IS{x}| + S_IF{x}| */
#include	<limits.h>		/* |{xxx}_MIN| + |{xxx}_MAX| */
#include	<signal.h>		/* |SIG{x}| */
#include	<unistd.h>		/* |_SC_{x}| + |_PC_{x}| */
#include	<fcntl.h>		/* |O_{x}| */


/* extra "open" flags */
enum extraopenflags {
	extraopenflag_minmode = 28,
	extraopenflag_minfd,
	extraopenflag_network,
	extraopenflag_overlast,
	extraopenflag_start = extraopenflag_minmode
} ; /* end enum */

/* mask for extended open-options*/
#ifndef	OM_SPECIAL
#define	OM_SPECIAL	((~ 0) << extraopenflag_start)
#endif
/* missing file open-flags */
#ifndef	O_LARGEFILE
#define	O_LARGEFILE	0
#endif
#ifndef	O_DIRECTORY
#define	O_DIRECTORY	0
#endif
#ifndef	O_DIRECT
#define	O_DIRECT	0
#endif
#ifndef	O_TMPFILE
#define	O_TMPFILE	0
#endif
#ifndef	O_PRIV
#define	O_PRIV		0
#endif
#ifndef	O_MINMODE
#define	O_MINMODE	(1 << extraopenflag_minmode)
#endif
#ifndef	O_MINFD
#define	O_MINFD		(1 << extraopenflag_minfd)
#endif
#ifndef	O_NETWORK
#define	O_NETWORK	(1 << extraopenflag_network)
#endif


#endif /* USYSDEFSOF_INCLUDE */



/* localmisc_dev HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* miscellaneous pre-processor definitions */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-15, David A­D­ Morano
	This code is a collection of miscellaneious pre-processor
	defintions that were eidely used on the various embedded
	code projects and also on many of the support tool projects.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	LOCALMISCDEV_INCLUDE
#define	LOCALMISCDEV_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */


#ifndef	NULLDEV
#define	NULLDEV		"/dev/null"
#endif
#ifndef	ZERODEV
#define	ZERODEV		"/dev/zero"
#endif
#ifndef	RANDOMDEV
#define	RANDOMDEV	"/dev/random"
#endif
#ifndef	TTYDEV
#define	TTYDEV		"/dev/tty"
#endif
#ifndef	AUDIODEV
#define	AUDIODEV	"/dev/audio"
#endif


#endif /* LOCALMISCDEV_INCLUDE */



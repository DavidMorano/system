/* localmisc_truth HEADER */
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

#ifndef	LOCALMISCTRUTH_INCLUDE
#define	LOCALMISCTRUTH_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */


#define	NULLDEV		"/dev/null"
#define	TTYDEV		"/dev/tty"

#ifndef	PASSWDLEN
#define	PASSWDLEN	64
#endif

#ifndef	ORGLEN
#define	ORGLEN		80
#endif
#ifndef	ORGCODELEN
#define	ORGCODELEN	40
#endif


#endif /* LOCALMISCTRUTH_INCLUDE */



/* localmisc_defs HEADER */
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

#ifndef	LOCALMISCDEFS_INCLUDE
#define	LOCALMISCDEFS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */


#ifndef	PASSWDLEN
#define	PASSWDLEN	64
#endif

#ifndef	ORGLEN
#define	ORGLEN		80		/* length organization-name */
#endif
#ifndef	ORGLOCLEN
#define	ORLOCGLEN	80		/* length organization-location */
#endif
#ifndef	ORGCODELEN
#define	ORGCODELEN	40		/* length organization-ode */
#endif


#endif /* LOCALMISCDEFS_INCLUDE */



/* localmisc_nums HEADER */
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

/*******************************************************************************

  	Description:
	These are definitions for really just some random numbers.

*******************************************************************************/

#ifndef	LOCALMISCNUMS_INCLUDE
#define	LOCALMISCNUMS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */


/* this is (long) depracated from UNIX® */
#ifndef	NOFILE
#define	NOFILE		20		/* UNIX® number of files */
#endif

#ifndef	POLL_INTMULT
#define	POLL_INTMULT	1000		/* poll-interval-multiplier (to secs) */
#endif

#ifndef	NYEARS_CENTURY
#define	NYEARS_CENTURY	100		/* years in a century */
#endif

#ifndef	COLUMNS
#define	COLUMNS		80		/* historical terminal columns */
#endif

#ifndef	NTABCOLS
#define	NTABCOLS	8		/* eight columns per TAB character */
#endif


#endif /* LOCALMISCNUMS_INCLUDE */



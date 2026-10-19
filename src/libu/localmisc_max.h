/* localmisc_max HEADER */
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

#ifndef	LOCALMISCMAX_INCLUDE
#define	LOCALMISCMAX_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>		/* POSIX */
#include	<limits.h>		/* POSIX® */


#ifndef	MAXARGLEN
#ifdef	ARG_MAX
#define	MAXARGLEN	ARG_MAX
#else
#define	MAXARGLEN	(256 * 1024)
#endif
#endif

#ifndef	MAXLINELEN
#ifdef	LINE_MAX
#define	MAXLINELEN	LINE_MAX
#else
#define	MAXLINELEN	(2*1024)
#endif
#endif

#ifndef	MAXNAMELEN
#ifdef	NAME_MAX
#define	MAXNAMELEN	NAME_MAX
#else
#define	MAXNAMELEN	256
#endif
#endif

#ifndef	MAXPATHLEN
#ifdef	PATH_MAX
#define	MAXPATHLEN	PATH_MAX
#else
#define	MAXPATHLEN	1024
#endif
#endif


#endif /* LOCALMISCMAX_INCLUDE */



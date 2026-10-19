/* localmisc_len HEADER */
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

#ifndef	LOCALMISCLEN_INCLUDE
#define	LOCALMISCLEN_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>		/* POSIX */
#include	<limits.h>		/* POSIX® */
#include	<localmisc_minmax.h>	/* LIBU */


#ifndef	NODENAMELEN
#define	NODENAMELEN	256
#endif

#ifndef	HOSTNAMELEN
#define	HOSTNAMELEN	1024
#endif

/* timezone abbreviation */
#ifndef	TZNAMELEN
#ifdef	TZNAME_MAX
#define	TZNAMELEN	TZNAME_MAX
#else
#define	TZNAMELEN	8
#endif
#endif /* TZNAMELEN */

/* timezone abbreviation */
#ifndef	TZABBRLEN
#ifdef	TZABBR_MAX
#define	TZABBRLEN	TZABBR_MAX
#else
#define	TZABBRLEN	8
#endif
#endif /* TZABBRLEN */

/* log-ID (for logging) */
#ifndef	LOGIDLEN
#define	LOGIDLEN	15
#endif

/* mail alias-length */
#ifndef	MAILALIASLEN
#define	MAILALIASLEN	64
#endif

#ifndef	TIMEBUFLEN
#define	TIMEBUFLEN	80		/* can hold? all known date strings */
#endif

/* common digit base (2, 8, 10, 16) buffer lengths (convenience defines) */

#ifndef	BINBUFLEN
#define	BINBUFLEN	256		/* can hold |int256_t| in binary */
#endif

#ifndef	OCTBUFLEN
#define	OCTBUFLEN	86		/* can hold |int256_t| in octal */
#endif

#ifndef	DECBUFLEN
#define	DECBUFLEN	78		/* can hold |int256_t| in decimal */
#endif

#ifndef	HEXBUFLEN
#define	HEXBUFLEN	64		/* can hold |int256_t| in hexadecimal */
#endif

#ifndef	DIGBUFLEN
#define	DIGBUFLEN	MAX(MAX(MAX(BINBUFLEN,OCTBUFLEN),DECBUFLEN),HEXBUFLEN)
#endif

#ifndef	REALNAMELEN
#define	REALNAMELEN	100		/* "real" name length */
#endif


#endif /* LOCALMISCLEN_INCLUDE */



/* usysdefs_len HEADER */
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

#ifndef	USYSDEFSLEN_INCLUDE
#define	USYSDEFSLEN_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/param.h>
#include	<limits.h>		/* |{xxx}_MIN| + |{xxx}_MAX| */


/* some stuff that not all systems (like GNU-Linux) have */

/* max-linelen */
#ifndef	MAXLINELEN
#ifdef	LINE_MAX
#define	MAXLINELEN	LINE_MAX
#else
#define	MAXLINELEN	2048		/* common value */
#endif
#endif /* MAXLINELEN */

/* max-nodelen */
#ifndef	NODENAMELEN
#ifdef	SYS_NMLN
#define	NODENAMELEN	(SYS_NMLN-1)	/* usually 256 for SVR4! */
#else
#define	NODENAMELEN	256		/* should be at least 256 for SVR4! */
#endif
#endif /* NODENAMELEN */

/* max-namelen (file-system) */
#ifndef	MAXNAMELEN
#ifdef	NAME_MAX
#define	MAXNAMELEN	NAME_MAX
#else
#define	MAXNAMELEN	256		/* common value */
#endif
#endif /* MAXNAMELEN */

/* max-pathlen (file-system) */
#ifndef	MAXPATHLEN
#ifdef	PATH_MAX
#define	MAXPATHLEN	PATH_MAX
#else
#define	MAXPATHLEN	2048		/* common value */
#endif
#endif /* MAXPATHLEN */

/* max-logname-length */
#ifndef	LOGNAMELEN
#ifdef	LOGNAME_MAX
#define	LOGNAMELEN	LOGNAME_MAX
#else
#define	LOGNAMELEN	32
#endif
#endif

/* max-username-length */
#ifndef	USERNAMELEN
#ifdef	LOGNAME_MAX
#define	USERNAMELEN	LOGNAME_MAX
#else
#define	USERNAMELEN	32
#endif
#endif

/* max-groupname-length */
#ifndef	GROUPNAMELEN
#ifdef	LOGNAME_MAX
#define	GROUPNAMELEN	LOGNAME_MAX
#else
#define	GROUPNAMELEN	32
#endif
#endif

/* max-projname-length (Solaris®) */
#ifndef	PROJNAMELEN
#ifdef	LOGNAME_MAX
#define	PROJNAMELEN	LOGNAME_MAX
#else
#define	PROJNAMELEN	32
#endif
#endif

/* max-protocol-name length */
#ifndef	PROTNAMELEN
#define	PROTNAMELEN	NODENAMELEN
#endif

/* max-network-name length */
#ifndef	NETWNAMELEN
#define	NETWNAMELEN	NODENAMELEN
#endif

/* max-hostname-name length */
#ifndef	HOSTNAMELEN
#ifdef	MAXHOSTNAMELEN
#define	HOSTBUFLEN	MAXHOSTNAMELEN
#else
#define	HOSTNAMELEN	(8 * NODENAMELEN)
#endif
#endif /* HOSTNAMELEN */

/* max-sericename-name length */
#ifndef	SERVNAMELEN
#define	SERVNAMELEN	NODENAMELEN
#endif

/* max-symname-length (this is really 255 on most platforms) */
#ifndef	SYMNAMELEN
#define	SYMNAMELEN	100		/* symbol-name-length (really 255) */
#endif

#ifndef	ARGBUFLEN
#define	ARGBUFLEN	MAXARGLEN
#endif

#ifndef	LINEBUFLEN
#define	LINEBUFLEN	MAXLINELEN
#endif

#ifndef	NAMEBUFLEN
#define	NAMEBUFLEN	MAXNAMELEN
#endif

#ifndef	PATHBUFLEN
#define	PATHBUFLEN	MAXPATHLEN
#endif

#ifndef	MSGBUFLEN
#define	MSGBUFLEN	(16 * 1024)
#endif /* MSGBUFLEN */

#ifndef	PASSWORDLEN
#define	PASSWORDLEN	8
#endif

/* service name */
#ifndef	SVCNAMELEN
#define	SVCNAMELEN	32
#endif

/* everyone (GNU/Linux, Solaris-Illumos, and macOS) seems to have this!  */
#ifndef	MAXNETNAMELEN		/* this is for ONC operations */
#define	MAXNETNAMELEN	255	/* maximum length of network user-name */
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


#endif /* USYSDEFSLEN_INCLUDE */



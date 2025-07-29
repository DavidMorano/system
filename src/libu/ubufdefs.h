/* ubufdefs HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* define the various system buffer sizes */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-11, David A-D- Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 2001 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

	Description:
	This file defines various preprocessor variables for use
	in setting up the UNIX®-system default for various buffer
	sizes.  The real run-time system buffer sizes are (indeed)
	determined at runtime where they may exist.

	Usage:
	Among the most important uses for the data contained herein
	is the BUFSIZE management facility. It uses the data below
	to set default buffer sizes for those systems that do not 
	have dynamicly managed buffer size support in the system kernel.

*******************************************************************************/

#ifndef	UBUFDEFS_INCLUDE
#define	UBUFDEFS_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/param.h>		/* system maximums */
#include	<limits.h>		/* POSIX® maximums */
#include	<usysdefs.h>		/* common maximums */


/* arguments (and environment) */
#ifndef	ARBUFLEN
#ifdef	ARG_MAX
#define	ARBUFLEN	ARG_MAX
#else
#define	ARBUFLEN	(2*1024*1024)	/* default for many systems */
#endif
#endif

/* max-name */
#ifndef	MNBUFLEN
#ifdef	NAME_MAX
#define	MNBUFLEN	NAME_MAX
#else
#define	MNBUFLEN	(1*1024)
#endif
#endif

/* max-path */
#ifndef	MPBUFLEN
#ifdef	PATH_MAX
#define	MPBUFLEN	PATH_MAX
#else
#define	MPBUFLEN	(4*1024)
#endif
#endif

/* max-line */
#ifndef	MLBUFLEN
#ifdef	LINE_MAX
#define	MLBUFLEN	LINE_MAX
#else
#define	MLBUFLEN	(2*2048)
#endif
#endif

/* node-name */
#ifndef	NNBUFLEN
#ifdef	_POSIX_HOST_NAME_MAX
#define	NNBUFLEN	(_POSIX_HOST_NAME_MAX-1)
#else
#ifdef	SYS_NMLN
#define	NNBUFLEN	(SYS_NMLN-1)
#else
#define	NNBUFLEN	256
#endif
#endif
#endif

/* host-name */
#ifndef	HNBUFLEN
#ifdef	MAXHOSTNAMELEN
#define	HNBUFLEN	MAXHOSTNAMELEN
#else
#define	HNBUFLEN	(1*1024)
#endif
#endif

/* msg-len */
#ifndef	MMBUFLEN
#define	MMBUFLEN	(16*1024)	/* (Max-Message-Buf-Len) */
#endif

/* user-name */
#ifndef	UNBUFLEN
#ifdef	_POSIX_LOGIN_NAME_MAX
#define	UNBUFLEN	_POSIX_LOGIN_NAME_MAX
#else
#define	UNBUFLEN	255		/* picked value from MacOS! */
#endif
#endif

/* group-name */
#ifndef	GNBUFLEN
#define	GNBUFLEN	255		/* picked value from MacOS! */
#endif

/* project-name */
#ifndef	PNBUFLEN
#define	PNBUFLEN	255		/* picked value from MacOS! */
#endif

/* password entry */
#ifndef	PWBUFLEN
#ifdef	_SC_GETPW_R_SIZE_MAX
#define	PWBUFLEN	_SC_GETPW_R_SIZE_MAX
#else
#define	PWBUFLEN	1024		/* Solaris® _SC_GETPW_R_SIZE_MAX */
#endif
#endif

/* shadow-password entry */
#ifndef	SPBUFLEN
#define	SPBUFLEN	1024		/* same as PWBUFLEN for now */
#endif

/* user-attribute entry */
#ifndef	UABUFLEN
#define	UABUFLEN	(2*1024)	/* this is just a suggestion */
#endif

/* group entry */
#ifndef	GRBUFLEN
#ifdef	_SC_GETGR_R_SIZE_MAX
#define	GRBUFLEN	_SC_GETGR_R_SIZE_MAX
#else
#define	GRBUFLEN	7296		/* Solaris® _SC_GETGR_R_SIZE_MAX */
#endif
#endif

/* project entry */
#ifndef	PJBUFLEN
#define	PJBUFLEN	(4 * 1024)	/* Solaris® recommends (4*1024) */
#endif

/* protocol entry */
#ifndef	PRBUFLEN
#define	PRBUFLEN	256
#endif

/* network entry */
#ifndef	NWBUFLEN
#define	NWBUFLEN	(4 * 1024)
#endif

/* host entry */
#ifndef	HOBUFLEN
#define	HOBUFLEN	(8 * 1024)
#endif

/* service entry */
#ifndef	SVBUFLEN
#define	SVBUFLEN	(1 * 1024)
#endif

/* filesystem-type buffer length */
#ifndef	FSBUFLEN
#ifdef	MFSNAMELEN
#define	FSBUFLEN	MFSNAMELEN
#else
#ifdef	MFSTYPENAMELEN
#define	FSBUFLEN	(MFSTYPENAMELEN-1) /* Apple-Darwin value */
#else
#ifdef	_ST_FSTYPSZ
#define	FSBUFLEN	_ST_FSTYPSZ	/* Sun-Solaris value */
#else
#define	FSBUFLEN	15		/* Apple-Darwin value */
#endif
#endif
#endif
#endif /* FSBUFLEN */

#ifndef	SNBUFLEN
#define	SNBUFLEN	SYMNAMELEN	/* symbol-name-length (really 255) */
#endif

/* time-zone-name (this is the abbreviated time-zone from time immemorial) */
#ifndef	ZNBUFLEN
#ifdef	TZNAME_MAX
#define	ZNBUFLEN	TZNAME_MAX
#else
#ifdef	_POSIX_TZNAME_MAX
#define	ZNBUFLEN	_POSIX_TZNAME_MAX	/* POSIX®-2012 value */
#else
#define	ZNBUFLEN	6			/* POSIX®-2 value */
#endif /* _POSIX_TZNAME_MAX */
#endif /* TZNAME_MAX */
#endif /* ZNBUFLEN */

/* zone-info-name (from Zone-Info; a newer concept) */
#ifndef	ZIBUFLEN
#define	ZIBUFLEN	MAXNAMELEN	/* what some others have suggested */
#endif


#endif /* UBUFDEFS_INCLUDE */



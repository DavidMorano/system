/* usysdefs_max HEADER */
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

#ifndef	USYSDEFSMAX_INCLUDE
#define	USYSDEFSMAX_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<sys/utsname.h>
#include	<sys/param.h>
#include	<sys/stat.h>		/* |S_IS{x}| + S_IF{x}| */
#include	<limits.h>		/* |{xxx}_MIN| + |{xxx}_MAX| */
#include	<signal.h>		/* |SIG{x}| */
#include	<unistd.h>		/* |_SC_{x}| + |_PC_{x}| */
#include	<fcntl.h>		/* |O_{x}| */


/* maximum numeric base for normal numeric conversions */
#ifndef	MAXBASE
#define	MAXBASE		64		/* standard (common) value */
#endif
#ifndef	DIGBASE_MAX
#define	DIGBASE_MAX	64		/* standard (common) value */
#endif

/* maximum type (number of bits) for normal numeric conversions */
#ifndef	MAXTYPE
#define	MAXTYPE		10		/* (2 ** 10 power) bits in max type */
#endif

/* maximum PID on the system (could be wrong) */
#ifndef	PID_MAX
#define	PID_MAX		999999		/* historic value (in decimal) */
#endif

#ifndef	LINE_MAX
#define	LINE_MAX	4096
#endif /* LINE_MAX */

#ifndef	LINK_MAX
#ifdef	MAXLINK
#define	LINK_MAX	MAXLINK
#else
#ifdef	_POSIX_LINK_MAX
#define	LINK_MAX	_POSIX_LINK_MAX
#else
#define	LINK_MAX	8
#endif /* _POSIX_LINK_MAX */
#endif /* MAXLINK */
#endif /* LINK_MAX */

#ifndef	NGROUPS_MAX
#ifdef	NGROUPS_UMAX
#define	NGROUPS_MAX	NGROUPS_UMAX
#else
#ifdef	NGROUPS
#define	NGROUPS_MAX	NGROUPS
#else
#ifdef	_POSIX_NGROUPS_MAX
#define	NGROUPS_MAX	_POSIX_NGROUPS_MAX
#else
#ifdef	NGROUPS_MAX_DEFAULT
#define	NGROUPS_MAX	NGROUPS_MAX_DEFAULT
#else
#define	NGROUPS_MAX	32		/* typical normal value */
#endif
#endif
#endif /* NGROUPS */
#endif /* NGROUPS_UMAX */
#endif /* NGROUPS_MAX */

#ifndef	SYMLOOP_MAX
#ifdef	_POSIX_SYMLOOP_MAX
#define	SYMLOOP_MAX	_POSIX_SYMLOOP_MAX
#else
#ifdef	MAXSYMLINKS
#define	SYMLOOP_MAX	MAXSYMLINKS
#else
#define	SYMLOOP_MAX	32
#endif
#endif
#endif /* SYMLOOP_MAX */

#ifndef	SYMBOL_MAX
#define	SYMBOL_MAX	255
#endif /* SYMBOL_MAX */

#ifndef	NODENAME_MAX
#define	NODENAME_MAX	255		/* pretty-much de-facto standard */
#endif /* NODENAME_MAX */

#ifndef	NAME_MAX
#define	NAME_MAX	255		/* pretty-much de-factor standard */
#endif /* NAME_MAX */

#ifndef	TZNAME_MAX
#define	TZNAME_MAX	255
#endif /* TZNAME_MAX */

#ifndef	TZABBR_MAX
#define	TZABBR_MAX	8
#endif /* TZABBR_MAX */

/* maximum SHM name (as per POSIX®) */
#ifndef	SHMNAME_MAX
#define	SHMNAME_MAX	14		/* historic value (in decimal) */
#endif

#ifndef	SYSUID_MAX
#define	SYSUID_MAX	(500-1)		/* really from Apple-Darwin */
#endif


#endif /* USYSDEFSMAX_INCLUDE */



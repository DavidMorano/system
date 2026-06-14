/* usysdefs_lim HEADER */
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

#ifndef	USYSDEFSLIM_INCLUDE
#define	USYSDEFSLIM_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* POSIX */
#include	<sys/utsname.h>		/* POSIX */
#include	<sys/param.h>		/* POSIX */
#include	<sys/stat.h>		/* POSIX |S_IS{x}| + S_IF{x}| */
#include	<limits.h>		/* |{xxx}_MIN| + |{xxx}_MAX| */
#include	<signal.h>		/* |SIG{x}| */
#include	<unistd.h>		/* |_SC_{x}| + |_PC_{x}| */
#include	<fcntl.h>		/* |O_{x}| */


/* various limits (that might be missing) */

/* strange ones */

#ifndef	UCHAR_MIN
#define	UCHAR_MIN	0
#endif

#ifndef	USHORT_MIN
#define	USHORT_MIN	0
#endif

#ifndef	UINT_MIN
#define	UINT_MIN	0
#endif

#ifndef	ULONG_MIN
#define	ULONG_MIN	0
#endif

#ifndef	ULONGLONG_MIN
#define	ULONGLONG_MIN	0
#endif

/* regular ones */

#ifndef	SHORT_MIN
#ifdef	SHRT_MIN
#define	SHORT_MIN	SHRT_MIN
#else
#define	SHORT_MIN	(-32768)	/* min value of a "short int" */
#endif
#endif

#ifndef	SHORT_MAX
#ifdef	SHRT_MAX
#define	SHORT_MAX	SHRT_MAX
#else
#define	SHORT_MAX	32767		/* max value of a "short int" */
#endif
#endif

#ifndef	USHORT_MAX
#ifdef	USHRT_MAX
#define	USHORT_MAX	USHRT_MAX
#else
#define	USHORT_MAX	65535		/* max value of "unsigned short int" */
#endif
#endif

#ifndef	SIZE_MAX
#define	SIZE_MAX	ULONG_MAX
#endif

#ifndef	SSIZE_MAX
#define	SSIZE_MAX	LONG_MAX
#endif

/* the following could be determined dynamically, but we choose not to */
#ifndef	TIME_MIN
#define	TIME_MIN	LONG_MIN
#endif
#ifndef	TIME_MAX
#define	TIME_MAX	LONG_MAX
#endif


#endif /* USYSDEFSLIM_INCLUDE */



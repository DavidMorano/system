/* localmisc_lims HEADER */
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
	These are some historical C-language type-value limits.

*******************************************************************************/

#ifndef	LOCALMISCLIMS_INCLUDE
#define	LOCALMISCLIMS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */


#ifndef	INT64_MIN
#define	INT64_MIN	(-9223372036854775807L-1LL)
#endif

#ifndef	INT64_MAX
#define	INT64_MAX	9223372036854775807LL
#endif

#ifndef	UINT64_MAX
#define	UINT64_MAX	18446744073709551615ULL
#endif

/* it would be nice if the implemenation had these */

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


#endif /* LOCALMISCLIMS_INCLUDE */



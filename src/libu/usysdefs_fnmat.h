/* usysdefs_fnmat HEADER */
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

*******************************************************************************/

#ifndef	USYSDEFSFNMAT_INCLUDE
#define	USYSDEFSFNMAT_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<stdlib.h>		/* CSTD */
#include	<fnmatch.h>		/* CSTD */


enum missingfnms {
	missingfnm_BEGIN = 10,
	missingfnm_ignorecase,
	missingfnm_icase,
	missingfnm_argumented,
	missingfnm_extmatch,
	missingfnm_overlast
} ; /* end enum (missingfnms)) */

/* flags */

#ifndef	FNM_IGNORECASE
#ifdef	FNM_CASEFOLD
#define	FNM_IGNORECASE		FNM_CASEFOLD
#else
#define	FNM_IGNORECASE		missingfnm_ignorecase
#endif
#endif /* FNM_IGNORECASE */

#ifndef	FNM_ICASE
#ifdef	FNM_CASEFOLD
#define	FNM_ICASE		FNM_CASEFOLD
#else
#define	FNM_ICASE		missingfnm_icase
#endif
#endif /* FNM_ICASE */

#ifndef	FNM_AUGMENTED
#define	FNM_AUGMENTED		missingfnm_augmented
#endif /* FNM_AUGMENTED */

#ifndef	FNM_EXTMATCH
#define	FNM_EXTMATCH		missingfnm_extmatch
#endif /* FNM_EXTMATCH */

/* flag-aliases */

#ifndef	FNM_FILENAME
#define	FNM_FILENAME		FNM_PATHNAME
#endif
#ifndef	FNM_LEADINGDIR
#define	FNM_LEADINGDIR		FNM_LEADING_DIR
#endif

/* returns */

#ifndef	FNM_MATCH
#define	FNM_MATCH		0
#endif /* FNM_MATCH */
#ifndef	FNM_NOSYS
#define	FNM_NOSYS		-1		/* from GLIBC */
#endif
#ifndef	FNM_ERROR
#define	FNM_ERROR		2		/* from Solaris® */
#endif

/* return-aliases */

#ifndef	FNM_RETMATCH
#define	FNM_RETMATCH		0
#endif
#ifndef	FNM_RETNOMATCH
#define	FNM_RETNOMATCH		FNM_NOMATCH
#endif
#ifndef	FNM_RETNOSYS
#define	FNM_RETNOSYS		-1
#endif
#ifndef	FNM_RETERROR
#define	FNM_RETERROR		FNM_ERROR
#endif


#endif /* USYSDEFSFNMAT_INCLUDE */



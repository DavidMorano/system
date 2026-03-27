/* fmtopts HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* header file for the FMTOPT subroutine */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This file was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	FMTOPTS_INCLUDE
#define	FMTOPTS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


enum fmtopts {
	fmtopt_clean,			/* clean up the string-data */
	fmtopt_nooverr,			/* no-overflow-error */
	fmtopt_overlast
} ; /* end enum (fmtopts) */

#ifdef	__cplusplus

struct fmtoptms {
	static cint	clean ;
	static cint	nooverr ;
} ;

#endif /* __cplusplus */

#define	FMTOPT_OCLEAN		(1 << fmtopt_clean)
#define	FMTOPT_ONOOVERR		(1 << fmtopt_nooverr)


#endif /* FMTOPTS_INCLUDE */



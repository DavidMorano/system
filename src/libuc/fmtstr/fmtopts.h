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
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


enum fmtopts {
	fmtopt_clean,			/* clean up the string-data */
	fmtopt_nooverr,			/* no-overflow-error */
	fmtopt_minfill,			/* perform minimum zero-filling */
	fmtopt_overlast
} ; /* end enum (fmtopts) */

#ifdef	__cplusplus

struct fmtoptms {
	static constexpr int	clean	= (1 << fmtopt_clean) ;
	static constexpr int	nooverr	= (1 << fmtopt_nooverr) ;
	static constexpr int	minfill	= (1 << fmtopt_minfill) ;
} ; /* end struct (fmtoptms) */

extern const fmtoptms	fmtoptm ;

#endif /* __cplusplus */

#define	FMTOPT_OCLEAN		(1 << fmtopt_clean)
#define	FMTOPT_ONOOVERR		(1 << fmtopt_nooverr)
#define	FMTOPT_OMINFILL		(1 << fmtopt_minfill)


#endif /* FMTOPTS_INCLUDE */



/* strtime_val HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* c-string comparisons */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	STRTIMEVAL_INCLUDE
#define	STRTIMEVAL_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


extern char *strtime_val(CTIMEVAL &,char *) noex ;

inline char *strtimeval(CTIMEVAL &tv,char *tbuf) noex {
	return strtime_val(tv,tbuf) ;
} /* end subroutine (strtimeval) */


#endif /* __cplusplus */
#endif /* STRTIMEVAL_INCLUDE */



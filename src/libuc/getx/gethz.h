/* gethz HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* get the HZ ("hertz") value from the operating system */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	GETHZ_INCLUDE
#define	GETHZ_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


enum gethzes {
	gethz_any,
	gethz_env,
	gethz_conf,
	gethz_const,
	gethz_tck,
	gethz_guess,
	gethz_overlast
} ; /* end enum (gethzes) */

EXTERNC_begin

extern int	gethz(int) noex ;

EXTERNC_end

#ifdef	__cplusplus

namespace ucgetx {
    struct gethzer {
	int operator () (gethzes = gethz_any) noex ;
	operator int () noex {
	    return operator () (gethz_any) ;
	} ;
    } ; /* end struct (gethzer) */
} /* end namespace (ucgetx) */

extern ucgetx::gethzer	syshz ;

#endif /* __cplusplus */


#endif /* GETHZ_INCLUDE */



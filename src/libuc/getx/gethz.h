/* gethz HEADER */
/* encoding=ISO8859-1 */
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
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


enum gethzes {
	gethz_any,
	gethz_env,
	gethz_conf,
	gethz_const,
	gethz_tck,
	gethz_guess,
	gethz_overlast
} ;

#ifdef	__cplusplus

EXTERNC_begin

namespace ucgetx {
    struct gethzer {
	int operator () (int = 0) noex ;
	operator int () noex {
	    return operator () (0) ;
	} ;
    } ; /* end struct (gethzer) */
}

extern ucgetx::gethzer	gethz ;

EXTERNC_end

#endif /* __cplusplus */


#endif /* GETHZ_INCLUDE */



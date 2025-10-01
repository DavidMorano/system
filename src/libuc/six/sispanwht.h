/* sispanwht HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* count leading white-space characters */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SISPANWHT_INCLUDE
#define	SISPANWHT_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


EXTERNC_begin

extern int	sispanwht(cchar *,int) noex ;

static inline int	siskipwhite(cchar *sp,int sl) noex {
    	return sispanwht(sp,sl) ;
}

EXTERNC_end


#endif /* SISPANWHT_INCLUDE */



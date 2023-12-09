/* siletter INCLUDE */
/* lang=C20 */

/* perform some kind of processsing */
/* version %I% last-modified %G% */


/* revision history:

	= 2009-04-01, David A­D­ Morano
	This subroutine was written as an enhancement for adding
	back-matter (end pages) to the output document.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SILETTER_INCLUDE
#define	SILETTER_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<localmisc.h>


#define	SILETTER	struct siletter


struct siletter {
	cchar		*lp ;
	int		ll ;
} ;

#ifdef	__cplusplus
extern "C" {
#endif

extern int	siletter(SILETTER *,cchar *,int) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* SILETTER_INCLUDE */



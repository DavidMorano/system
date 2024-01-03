/* optval HEADER */
/* lang=C20 */

/* divine a value (boolean or otherwise) out of a c-sring */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright (c) 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	OPTVAL_INCLUDE
#define	OPTVAL_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<utypedefs.h>
#include	<clanguage.h>


#ifdef	__cplusplus
extern "C" {
#endif

extern int	optval(cchar *,int) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* OPTVAL_INCLUDE */



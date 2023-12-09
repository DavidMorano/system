/* ucgetus INCLUDE */
/* lang=C20 */

/* UNIX® C-language system database access (UCGET) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	UCGETUS_INCLUDE
#define	UCGETUS_INCLUDE


#include	<envstandards.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>


#ifdef	__cplusplus
extern "C" {
#endif

extern int	uc_getusbegin() noex ;
extern int	uc_getusent(char *,int) noex ;
extern int	uc_getusend() noex ;

#ifdef	__cplusplus
}
#endif


#endif /* UCGETUS_INCLUDE */



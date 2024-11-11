/* getarchitecture HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* get the best approximation of the username of the current user */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	GETARCHITECTURE_INCLUDE
#define	GETARCHITECTURE_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


EXTERNC_begin

extern int getarchitecture(char *,int) noex ;

static inline int getarch(char *rb,int rl) noex {
	return getarchitecture(rb,rl) ;
}

EXTERNC_end


#endif /* GETARCHITECTURE_INCLUDE */



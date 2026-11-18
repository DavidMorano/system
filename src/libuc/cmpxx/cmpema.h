/* cmpema HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* find a substring within a larger string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CMPEMA_INCLUDE
#define	CMPEMA_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


EXTERNC_begin
extern int	cmpema		(cchar *,cchar *) noex ;
extern int	cmpemaø		(cchar *,cchar *) noex ;
EXTERNC_end


#endif /* CMPEMA_INCLUDE */



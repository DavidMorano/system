/* passfd HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* subroutine to pass a file-descriptor to a file */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-04-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	PASSFD_INCLUDE
#define	PASSFD_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


EXTERNC_begin

extern int	passfd(cchar *,int) noex ;

EXTERNC_end


#endif /* PASSFD_INCLUDE */



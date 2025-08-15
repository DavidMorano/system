/* ucopy HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* copy a file (to another file) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	UCOPY_INCLUDE
#define	UCOPY_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


EXTERNC_begin

extern int	u_writefd(int,int) noex ;
extern int	u_writefile(int,cchar *) noex ;
extern int	u_copy(cchar *,cchar *) noex ;

EXTERNC_end


#endif /* UCOPY_INCLUDE */



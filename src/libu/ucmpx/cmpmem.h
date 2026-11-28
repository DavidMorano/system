/* cmpmem HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* LIBU comparison operations */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CMPMEM_INCLUDE
#define	CMPMEM_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


EXTERNC_begin
    extern int cmpmem(cvoid *,cvoid *,int) noex ;
EXTERNC_end


#endif /* CMPMEM_INCLUDE */



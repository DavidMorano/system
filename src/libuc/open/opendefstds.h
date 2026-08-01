/* openddefstads HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* open default standard files */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	This subroutine was written for Rightcore Network Services
	(RNS).

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

#ifndef	OPENDEFSTDS_INCLUDE
#define	OPENDEFSTDS_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


EXTERNC_begin

extern int opendial(int,int,cc *,cc *,cc *,mainv,mainv,int,int) noex ;

EXTERNC_end


#endif /* OPENDEFSTDS_INCLUDE */



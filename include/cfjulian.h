/* cfjulain HEADER */
/* encoding=ISO8859-1 */
/* lang=C++11 */

/* convert from an elapsed time format to a time-type value */
/* version %I% last-modified %G% */


/* revision history:

	= 2004-08-01, David A­D­ Morano
        This subroutine was modeled (adapated) from assembly language
        (although there isn't the shred of similarity between the two).

*/

/* Copyright © 2004 David A­D­ Morano.  All rights reserved. */

#ifndef	CFJULIAN_INCLUDE
#define	CFJULIAN_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<time.h>		/* |time_t| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


EXTERNC_begin

extern int cfjulian(cchar *,int,time_t *) noex ;

EXTERNC_end


#endif /* CFJULIAN_INCLUDE */



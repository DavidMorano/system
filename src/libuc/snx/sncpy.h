/* sncpy HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* counted c-string copy operations */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SNCPY_INCLUDE
#define	SNCPY_INCLUDE


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */

#include	<sncpyx.h>
#include	<sncpyxw.h>
#include	<sncpyxc.h>


EXTERNC_begin

extern int sncpyarray(char *,int,con mainv,int) noex ;

EXTERNC_end


#endif /* SNCPY_INCLUDE */



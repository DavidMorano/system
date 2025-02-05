/* ourmjd HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* test if the given counted c-string has "our" Modified-Julian-Date (MJD) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	OURMJD_INCLUDE
#define	OURMJD_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


EXTERNC_begin

extern int	ourmjd(cchar *,int) noex ;

EXTERNC_end


#endif /* OURMJD_INCLUDE */



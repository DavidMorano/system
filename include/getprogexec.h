/* getprogexec HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* get program root directory */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A�D� Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

#ifndef	GETPROGEXEC_INCLUDE
#define	GETPROGEXEC_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


EXTERNC_begin

extern int	getprogexec(char *,int) noex ;

EXTERNC_end


#endif /* GETPROGEXEC_INCLUDE */



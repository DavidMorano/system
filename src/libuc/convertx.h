/* convertx HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* convert a double floating point value to various representations */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A�D� Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	These subroutines are similar to the |ecvt(3c)| |fcvt(3c)|,
	and |gcvt(3c)| POSIX� subroutines, but are reentrant. 

*******************************************************************************/

#ifndef	CONVETX_INCLUDE
#define	CONVETX_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


EXTERNC_begin

extern int converte(double,int,int *,int *,char *) noex ;
extern int convertf(double,int,int *,int *,char *) noex ;
extern int convertg(double,int,int,char *) noex ;

EXTERNC_end


#endif /* CONVETX_INCLUDE */



/* filereadstrs HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* read a string(s) from a file! */
/* version %I% last-modified %G% */


/* revision history:

	= 2017-11-24, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	filereadstrs

	Description:
	This subroutine reads strings from a file (the string
	of which means something to someone).

*******************************************************************************/

#ifndef	FILEREADSTRS_INCLUDE
#define	FILEREADSTRS_INCLUDE


#include	<envstandards.h>	/* must be before others */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


EXTERNC_begin

extern int	filereadstrs(char *,int,cchar *) noex ;

EXTERNC_end


#endif /* FILEREADSTRS_INCLUDE */



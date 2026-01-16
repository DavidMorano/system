/* hasonly INCLUDE */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* has a counted c-string some characteristic? */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-10-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	hasonly{x}

	Description:
	These subroutines check if a specified c-string has any of
	some characteristic we are looking for.

*******************************************************************************/

#ifndef	HASONLY_INCLUDE
#define	HASONLY_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>


EXTERNC_begin

extern bool hasonlyminus(cchar *,int) noex ;
extern bool hasonlyplusminus(cchar *,int) noex ;

EXTERNC_end


#endif /* HASONLY_INCLUDE */



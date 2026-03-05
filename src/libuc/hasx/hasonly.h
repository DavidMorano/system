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

	Names:
	hasonlypl
	hasonlymi
	hasonlypm
	hasonlyme

	Description:
	These subroutines check if the specified c-string has only
	the characteristic we are looking for.

*******************************************************************************/

#ifndef	HASONLY_INCLUDE
#define	HASONLY_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>


EXTERNC_begin

extern bool hasonlypl(cchar *,int) noex ;	/* has-plus */
extern bool hasonlymi(cchar *,int) noex ;	/* has-minus */
extern bool hasonlypm(cchar *,int) noex ;	/* has-plus-minus */
extern bool hasonlyme(cchar *,int) noex ;	/* has-me */

EXTERNC_end


#endif /* HASONLY_INCLUDE */



/* mallcompactstr HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* memocy-allocate a compacted c-string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-03, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	MALLCOMPACTSTR_INCLUDE
#define	MALLCOMPACTSTR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


EXTERNC_begin

extern int	mallcompactstr(cchar *,int,char **) noex ;

EXTERNC_end


#endif /* MALLCOMPACTSTR_INCLUDE */



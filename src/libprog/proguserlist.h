/* proguserlist HEADER */
/* lang=C20 */

/* program-user-list-ing */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-03-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Manage a program user-list operation.


*******************************************************************************/

#ifndef	PROGUSERLIST_INCLUDE
#define	PROGUSERLIST_INCLUDE


#include	<envstandards.h>	/* must be before others */
#include	<sys/types.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>

#include	"defs.h"


EXTERNC_begin

extern int	proguserlist_begin(PROGINFO *) ;
extern int	proguserlist_end(PROGINFO *) ;
 
EXTERNC_end


#endif /* PROGUSERLIST_INCLUDE */



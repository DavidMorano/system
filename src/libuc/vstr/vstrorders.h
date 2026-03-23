/* vstrorders HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* these are values (constants) for specifying comparison order */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	VSTRORDERS_INCLUDE
#define	VSTRORDERS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>


enum vstrorders {
    vstrorder_obverse,
    vstrorder_reverse,
    vstrorder_overlast
} ; /* end enum (vstrorders) */


#endif /* VSTRORDERS_INCLUDE */



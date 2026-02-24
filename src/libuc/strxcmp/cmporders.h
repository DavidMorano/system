/* cmporders HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* convert a character string to a given case */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CMPORDERS_INCLUDE
#define	CMPORDERS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>


enum cmporders {
    cmporder_obverse,
    cmporder_reverse,
    cmporder_overlast
} ; /* end enum (cmporders) */


#endif /* CMPORDERS_INCLUDE */



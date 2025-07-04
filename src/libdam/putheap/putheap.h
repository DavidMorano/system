/* putheap HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* put a c-string on the heap */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	PUTHEAP_INCLUDE
#define	PUTHEAP_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<limits.h>		/* |UCHAR_MAX| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>


EXTERNC_begin

extern char	*putheap(cchar *) noex ;

EXTERNC_end


#endif /* PUTHEAP_INCLUDE */



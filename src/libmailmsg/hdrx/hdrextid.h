/* hdrextid HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* extract an ID from a header value */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	HDREXTID_INCLUDE
#define	HDREXTID_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


EXTERNC_begin

extern int	hdrextid(char *,int,cchar *,int) noex ;

EXTERNC_end


#endif /* HDREXTID_INCLUDE */



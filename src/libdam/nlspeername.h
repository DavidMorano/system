/* nlspeername HEADER */
/* lang=C20 */

/* try to find the peername for an NLS (XTI) connection */
/* version %I% last-modified %G% */


/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

#ifndef	NLSPEERNAME_INCLUDE
#define	NLSPEERNAME_INCLUDE


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


EXTERNC_begin

extern int	nlspeername(cchar *,cchar *,char *) noex ;

EXTERNC_end


#endif /* NLSPEERNAME_INCLUDE */



/* getheour HEADER */
/* lang=C20 */

/* get a host name that has an INET address (of some sort: name or address) */
/* version %I% last-modified %G% */


/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

#ifndef	GETHEOUR_INCLUDE
#define	GETHEOUR_INCLUDE


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>

EXTERNC_begin

extern int	getheour(HOSTENT *,char *,int,char *,cc *) noex ;

EXTERNC_end


#endif /* GETHEOUR_INCLUDE */



/* gethename HEADER */
/* lang=C20 */

/* try to find the peername for an NLS (XTI) connection */
/* version %I% last-modified %G% */


/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

#ifndef	GETHENAME_INCLUDE
#define	GETHENAME_INCLUDE


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<usystem.h>

EXTERNC_begin

extern int	gethename(ucentho *,char *,int,cchar *) noex ;

EXTERNC_end


#endif /* GETHENAME_INCLUDE */



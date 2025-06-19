/* inetping HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* PING an INET machine */
/* version %I% last-modified %G% */


/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

#ifndef	INETPING_INCLUDE
#define	INETPING_INCLUDE


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


EXTERNC_begin

extern int	inetping(cchar *,int) noex ;

EXTERNC_end


#endif /* INETPING_INCLUDE */



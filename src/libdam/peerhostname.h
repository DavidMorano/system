/* peerhostname HEADER */
/* lang=C20 */

/* get a peer host name if there is one */
/* version %I% last-modified %G% */


/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

#ifndef	PEERHOSTNAME_INCLUDE
#define	PEERHOSTNAME_INCLUDE


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


EXTERNC_begin

extern int	peerhostname(int,cchar *,char *) noex ;

EXTERNC_end


#endif /* PEERHOSTNAME_INCLUDE */



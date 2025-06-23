/* conallof HEADER (contains-all-of) */
/* charset=ISO8859-1 */
/* lang=C20 */

/* does the given string have all of the given characters? */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CONALLOF_INCLUDE
#define	CONALLOF_INCLUDE


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


EXTERNC_begin

extern int	conallof(cchar *,int,cchar *) noex ;

EXTERNC_end


#endif /* CONALLOF_INCLUDE */



/* getus HEADER */
/* lang=C20 */

/* get user-shell */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	GETUS_INCLUDE
#define	GETUS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysrets.h>


EXTERNC_begin

extern int getus_begin() noex ;
extern int getus_ent(char *,int) noex ;
extern int getus_end() noex ;

EXTERNC_end


#endif /* GETUS_INCLUDE */



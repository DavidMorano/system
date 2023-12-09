/* getus INCLUDE */
/* lang=C20 */

/* get user-shell */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	GETUS_INCLUDE
#define	GETUS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<clanguage.h>


#ifdef	__cplusplus
extern "C" {
#endif

extern int getus_begin() noex ;
extern int getus_ent(char *,int) noex ;
extern int getus_end() ;

#ifdef	__cplusplus
}
#endif


#endif /* GETUS_INCLUDE */



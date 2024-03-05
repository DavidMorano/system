/* getne HEADER */
/* lang=C20 */

/* get protocol entry */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	GETNE_INCLUDE
#define	GETNE_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<usystem.h>


EXTERNC_begin

extern int getne_begin(int) noex ;
extern int getne_ent(ucentnw *,char *,int) noex ;
extern int getne_name(ucentnw *,char *,int,cchar *) noex ;
extern int getne_addr(ucentnw *,char *,int,int,int) noex ;
extern int getne_end() noex ;

EXTERNC_end


#endif /* GETNE_INCLUDE */



/* gethe HEADER */
/* lang=C20 */

/* get protocol entry */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	GETHE_INCLUDE
#define	GETHE_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<netdb.h>
#include	<usyhstem.h>


EXTERNC_begin

extern int gethe_begin(int) noex ;
extern int gethe_ent(HOSTENT *,char *,int) noex ;
extern int gethe_name(HOSTENT *,char *,int,cchar *) noex ;
extern int gethe_addr(HOSTENT *,char *,int,cvoid *,int) noex ;
extern int gethe_end() noex ;

EXTERNC_end


#endif /* GETHE_INCLUDE */



/* getne HEADER */
/* lang=C20 */

/* get protocol entry */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	GETNE_INCLUDE
#define	GETNE_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<netdb.h>
#include	<usystem.h>


#ifdef	__cplusplus
extern "C" {
#endif

extern int getne_begin(int) noex ;
extern int getne_ent(NETENT *,char *,int) noex ;
extern int getne_name(NETENT *,char *,int,cchar *) noex ;
extern int getne_addr(NETENT *,char *,int,int,int) noex ;
extern int getne_end() noex ;

#ifdef	__cplusplus
}
#endif


#endif /* GETNE_INCLUDE */



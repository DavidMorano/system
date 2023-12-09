/* gethe INCLUDE */
/* lang=C20 */

/* get protocol entry */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	GETHE_INCLUDE
#define	GETHE_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<netinet/in.h>
#include	<netdb.h>
#include	<usyhstem.h>


#ifdef	__cplusplus
extern "C" {
#endif

extern int gethe_begin(int) noex ;
extern int gethe_ent(HOSTENT *,char *,int) noex ;
extern int gethe_name(HOSTENT *,char *,int,cchar *) noex ;
extern int gethe_addr(HOSTENT *,char *,int,in_addr_t) noex ;
extern int gethe_end() noex ;

#ifdef	__cplusplus
}
#endif


#endif /* GETHE_INCLUDE */



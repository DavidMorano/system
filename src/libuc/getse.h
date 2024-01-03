/* getse HEADER */
/* lang=C20 */

/* get service entry */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	GETSE_INCLUDE
#define	GETSE_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<netdb.h>
#include	<usystem.h>


#ifdef	__cplusplus
extern "C" {
#endif

extern int getse_begin(int) noex ;
extern int getse_ent(SERVENT *,char *,int) noex ;
extern int getse_name(SERVENT *,char *,int,cchar *,cchar *) noex ;
extern int getse_port(SERVENT *,char *,int,cchar *,int) noex ;
extern int getse_end() noex ;

#ifdef	__cplusplus
}
#endif


#endif /* GETSE_INCLUDE */



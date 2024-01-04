/* getpe HEADER */
/* lang=C20 */

/* get protocol entry */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	GETPE_INCLUDE
#define	GETPE_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<netdb.h>
#include	<usystem.h>
#include	<localmisc.h>


#ifdef	__cplusplus
extern "C" {
#endif

extern int getpe_begin(int) noex ;
extern int getpe_ent(PROTOENT *,char *,int) noex ;
extern int getpe_name(PROTOENT *,char *,int,cchar *) noex ;
extern int getpe_num(PROTOENT *,char *,int,int) noex ;
extern int getpe_end() noex ;

#ifdef	__cplusplus
}
#endif


#endif /* GETPE_INCLUDE */



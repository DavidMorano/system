/* getpe INCLUDE */
/* lang=C20 */

/* get protocol entry */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	GETPE_INCLUDE
#define	GETPE_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<netdb.h>


#ifdef	__cplusplus
extern "C" {
#endif

extern int getpe_begin(int) noex ;
extern int getpe_ent(struct protoent *,char *,int) noex ;
extern int getpe_end() noex ;
extern int getpe_name(struct protoent *,char *,int,const char *) noex ;
extern int getpe_num(struct protoent *,char *,int,int) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* GETPE_INCLUDE */



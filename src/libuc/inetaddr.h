/* inetaddr HEADER */
/* lang=C20 */

/* object to manipulate INET adresses */
/* version %I% last-modified %G% */


/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

#ifndef	INETADDR_INCLUDE
#define	INETADDR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<netinet/in.h>
#include	<netdb.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>


#define	INETADDR	union inetaddr_head


union inetaddr_head {
	struct in_addr	a ;
	char		straddr[sizeof(struct in_addr)] ;
} ;

typedef union inetaddr_head	inetaddr ;

#ifdef	__cplusplus
extern "C" {
#endif

extern int inetaddr_start(inetaddr *,cvoid *) noex ;
extern int inetaddr_startstr(inetaddr *,cchar *,int) noex ;
extern int inetaddr_startdot(inetaddr *,cchar *,int) noex ;
extern int inetaddr_gethexaddr(inetaddr *,char *,int) noex ;
extern int inetaddr_getdotaddr(inetaddr *,char *,int) noex ;
extern int inetaddr_finish(inetaddr *) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* INETADDR_INCLUDE */



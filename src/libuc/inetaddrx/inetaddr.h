/* inetaddr HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* object to manipulate INET adresses */
/* version %I% last-modified %G% */

/* revision history:

	= 1998-02-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	INETADDR_INCLUDE
#define	INETADDR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<netinet/in.h>		/* POSIX */
#include	<netdb.h>		/* POSIX */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<inetaddrs.h>		/* LIBU */


#define	INETADDR	struct inetaddr_head


struct inetaddr_head {
	union {
	    INADDR4	a ;
	    char	straddr[szof(INADDR4)] ;
	} ;
} ; /* end struct (inetaddr_head) */

#ifdef	__cplusplus
enum inetaddrmems {
	inetaddrmem_finish,
	inetaddrmem_overlast
} ; /* end (enum inetaddrmems) */
struct inetaddr ;
struct inetaddr_co {
	inetaddr	*op = nullptr ;
	int		w = -1 ;
	void operator () (inetaddr *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
} ; /* end struct (inetaddr_co) */
struct inetaddr : inetaddr_head {
	inetaddr_co	finish ;
	inetaddr() noex {
	    finish	(this,inetaddrmem_finish) ;
	} ;
	inetaddr(const inetaddr &) = delete ;
	inetaddr &operator = (const inetaddr &) = delete ;
	int start	(inetaddrs,cvoid *,int = -1) noex ;
	int gethexaddr	(char *,int) noex ;
	int getdotaddr	(char *,int) noex ;
} ; /* end struct (inetaddr) */
#else /* __cplusplus */
typedef INETADDR	inetaddr ;
#endif /* __cplusplus */

EXTERNC_begin

extern int inetaddr_start	(inetaddr *,inetaddrs,cvoid *,int) noex ;
extern int inetaddr_gethexaddr	(inetaddr *,char *,int) noex ;
extern int inetaddr_getdotaddr	(inetaddr *,char *,int) noex ;
extern int inetaddr_finish	(inetaddr *) noex ;

EXTERNC_end


#endif /* INETADDR_INCLUDE */



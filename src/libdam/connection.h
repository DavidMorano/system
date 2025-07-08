/* connection HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* manipulate INET connection information */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CONNECTION_INCLUDE
#define	CONNECTION_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>
#include	<netdb.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<vecstr.h>
#include	<sockaddress.h>
#include	<localmisc.h>		/* various constants (see below) */


#define	CONNECTION		struct connection_head
#define	CONNECTION_FL		struct connection_flags


struct connection_flags {
	uint		inet:1 ;
	uint		trans:1 ;
	uint		sa:1 ;
	uint		addr:1 ;
	uint		inetdomain:1 ;	/* allocated */
} ;

struct connection_head {
	cchar		*inetdomain ;	/* local domain name */
	cchar		*name ;		/* dynamically allocated */
	cchar		*pr ;		/* dynamically allocated */
	sockaddress	*sap ;
	INADDR		netipaddr ;
	CONNECTION_FL	fl ;
	int		sal ;
	int		sock ;
} ;

#ifdef	__cplusplus
enum connectionmems {
	connectionmem_finish,
	connectionmem_overlast
} ;
struct connection ;
struct connection_co {
	connection	*op = nullptr ;
	int		w = -1 ;
	void operator () (connection *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
} ; /* end struct (connection_co) */
struct connection : connection_head {
	connection_co	finish ;
	connection() noex {
	    finish(this,connectionmem_finish) ;
	    sap = nullptr ;
	} ;
	connection(const connection &) = delete ;
	connection &operator = (const connection &) = delete ;
	int start(cchar *) noex ;
	int socklocname(char *,int,int) noex ;
	int sockremname(char *,int,int) noex ;
	int peername(sockaddress *,int,char *,int) noex ;
	int mknames(vecstr *) noex ;
	void dtor() noex ;
	destruct connection() {
	    if (sap) dtor() ;
	} ;
} ; /* end struct (connection) */
#else	/* __cplusplus */
typedef CONNECTION	connection ;
#endif /* __cplusplus */

typedef	CONNECTION_FL	connection_fl ;

EXTERNC_begin

extern int connection_start(connection *,cchar *) noex ;
extern int connection_socklocname(connection *,char *,int,int) noex ;
extern int connection_sockremname(connection *,char *,int,int) noex ;
extern int connection_peername(connection *,sockaddress *,int,char *,int) noex ;
extern int connection_mknames(connection *,vecstr *) noex ;
extern int connection_finish(connection *) noex ;

EXTERNC_end


#endif /* CONNECTION_INCLUDE */



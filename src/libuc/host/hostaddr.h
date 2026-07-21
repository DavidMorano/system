/* hostaddr HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* manipulate host entry structures */
/* version %I% last-modified %G% */


/* Copyright © 2005 David A­D­ Morano.  All rights reserved. */

#ifndef	HOSTADDR_INCLUDE
#define	HOSTADDR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/socket.h>		/* POSIX® */
#include	<netinet/in.h>		/* POSIX® */
#include	<arpa/inet.h>		/* POSIX® */
#include	<netdb.h>		/* POSIX® */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


#define	HOSTADDR		struct hostaddr_head
#define	HOSTADDR_CUR		struct hostaddr_cursor
#define	HOSTADDR_MAGIC		0x73625196


struct hostaddr_head {
	ADDRINFO	*aip ;
	ADDRINFO	**resarr ;
	cchar		*ehostname ;
	uint		magval ;
	int		n ;
} ; /* end struct (hostaddr_head) */

struct hostaddr_cursor {
	int		i ;
} ; /* end struct (hostaddr_cursor) */

typedef HOSTADDR		hostaddr ;
typedef HOSTADDR_CUR		hostaddr_cur ;

EXTERNC_begin

extern int hostaddr_start(hostaddr *,cchar *,cchar *,ADDRINFO *) noex ;
extern int hostaddr_getcanonical(hostaddr *,cchar **) noex ;
extern int hostaddr_curbegin(hostaddr *,hostaddr_cur *) noex ;
extern int hostaddr_curend(hostaddr *,hostaddr_cur *) noex ;
extern int hostaddr_curenum(hostaddr *,hostaddr_cur *,ADDRINFO **) noex ;
extern int hostaddr_finish(hostaddr *) noex ;

EXTERNC_end


#endif /* HOSTADDR_INCLUDE */



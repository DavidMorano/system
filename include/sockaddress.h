/* sockaddress HEADER */
/* lang=C20 */



/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SOCKADDRESS_INCLUDE
#define	SOCKADDRESS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/socket.h>
#include	<sys/un.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>
#include	<usystem.h>
#include	<localmisc.h>


#define	SOCKADDRESS		union sockaddress_head
#define	SOCKADDRESS_INET	struct sockaddress_inet6

#define	SOCKADDRESS_LEN		sizeof(union sockaddress_head)
#define	SOCKADDRESS_NAMELEN	sizeof(union sockaddress_head)


struct sockaddress_path {
	ushort	naf ;			/* address space */
	char	path[MAXPATHLEN + 1] ;
} ;

struct sockaddress_inet4 {
	ushort	naf ;			/* address space */
	ushort	nport ;			/* port */
	uchar	a[4] ;			/* 32-bit address */
} ;

struct sockaddress_inet6 {
	ushort	naf ;			/* address space */
	ushort	nport ;			/* port */
	uint	nflow ;			/* flow label */
	uchar	a[16] ;			/* 128-bit address */
	uint	scope ;			/* scope crap */
	uchar	dummy[4] ;		/* implementation defined */
} ;

union sockaddress_head {
	struct sockaddr			a_unspec ;
	struct sockaddr_un		a_unix ;
	struct sockaddr_in		a_in ;
	struct sockaddr_in6		a_in6 ;
	struct sockaddress_inet4	a_inet4 ;
	struct sockaddress_inet6	a_inet6 ;
	struct sockaddress_path		a_path ;
	unsigned char			str[32] ; /* general future */
} ;

typedef	SOCKADDRESS		sockaddress ;
typedef	SOCKADDRESS_INET	sockaddress_inet ;

EXTERNC_begin

extern int sockaddress_start(SOCKADDRESS *,int,cvoid *,int,int) noex ;
extern int sockaddress_startaddr(SOCKADDRESS *,int,cvoid *,int,int,int) noex ;
extern int sockaddress_getlen(SOCKADDRESS *) noex ;
extern int sockaddress_getaddrlen(SOCKADDRESS *) noex ;
extern int sockaddress_gethex(SOCKADDRESS *,char *,int) noex ;
extern int sockaddress_getaf(SOCKADDRESS *) noex ;
extern int sockaddress_getport(SOCKADDRESS *) noex ;
extern int sockaddress_getflow(SOCKADDRESS *,uint *) noex ;
extern int sockaddress_getaddr(SOCKADDRESS *,void *,int) noex ;
extern int sockaddress_getscope(SOCKADDRESS *,uint *) noex ;
extern int sockaddress_getextra(SOCKADDRESS *,uint *) noex ;
extern int sockaddress_putaf(SOCKADDRESS *,int) noex ;
extern int sockaddress_putport(SOCKADDRESS *,int) noex ;
extern int sockaddress_putaddr(SOCKADDRESS *,cvoid *) noex ;
extern int sockaddress_finish(SOCKADDRESS *) noex ;

EXTERNC_end


#endif /* SOCKADDRESS_INCLUDE */



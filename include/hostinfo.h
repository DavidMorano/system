/* hostinfo HEADER */
/* lang=C20 */

/* manipulate host entry structures */
/* version %I% last-modified %G% */


/* Copyright � 2005 David A�D� Morano.  All rights reserved. */

#ifndef	HOSTINFO_INCLUDE
#define	HOSTINFO_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>
#include	<usystem.h>
#include	<vecobj.h>
#include	<uinet.h>
#include	<localmisc.h>


#define	HOSTINFO_MAGIC		0x73625196
#define	HOSTINFO		struct hostinfo_head
#define	HOSTINFO_FL		struct hostinfo_flags
#define	HOSTINFO_ADDR		struct hostinfo_addr
#define	HOSTINFO_ARGS		struct hostinfo_args
#define	HOSTINFO_CUR		struct hostinfo_cursor


struct hostinfo_flags {
	uint		inet4:1 ;
	uint		inet6:1 ;
	uint		addr:1 ;	/* was given an address */
} ;

struct hostinfo_addr {
	cchar		*addr ;
	int		addrlen ;
	int		af ;
} ;

struct hostinfo_args {
	cchar		*hostname ;	/* might be allocated */
	int		af ;		/* caller-supplied argument */
	int		hostnamelen ;
	int		f_alloc ;
} ;

struct hostinfo_head {
	uint		magic ;
	HOSTINFO_FL	init, f ;
	HOSTINFO_ARGS	arg ;
	HOSTINFO_ADDR	addr ;
	vecobj		names ;
	vecobj		addrs ;
	cchar		*domainname ;	/* dynamically allocated */
	void		*a ;		/* holds allocation for host-bufs */
	char		*ehostname ;	/* allocated on start */
	char		*chostname ;	/* allocated on start */
} ;

struct hostinfo_cursor {
	int		i ;
} ;

typedef HOSTINFO		hostinfo ;
typedef HOSTINFO_FL		hostinfo_fl ;
typedef HOSTINFO_ADDR		hostinfo_addr ;
typedef HOSTINFO_ARGS		hostinfo_args ;
typedef HOSTINFO_CUR		hostinfo_cur ;

#ifdef	__cplusplus
extern "C" {
#endif

extern int hostinfo_start(hostinfo *,int,cchar *) noex ;
extern int hostinfo_geteffective(hostinfo *,cchar **) noex ;
extern int hostinfo_getcanonical(hostinfo *,cchar **) noex ;
extern int hostinfo_curbegin(hostinfo *,hostinfo_cur *) noex ;
extern int hostinfo_curend(hostinfo *,hostinfo_cur *) noex ;
extern int hostinfo_enumname(hostinfo *,hostinfo_cur *,cchar **) noex ;
extern int hostinfo_enumaddr(hostinfo *,hostinfo_cur *,const uchar **) noex ;
extern int hostinfo_finish(hostinfo *) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* HOSTINFO_INCLUDE */



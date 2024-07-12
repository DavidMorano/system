/* nifinfo HEADER */
/* lang=C++20 */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	NIFINFO_INCLUDE
#define	NIFINFO_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<net/if.h>
#include	<arpa/inet.h>
#include	<netdb.h>
#include	<string.h>
#include	<usystem.h>
#include	<localmisc.h>	/* for 'uint' */


#define	NIFINFO_MAGIC		0x73638196
#define	NIFINFO			struct nifinfo_head
#define	NIFINFO_ENT		struct nifinfo_entry
#define	NIFINFO_CUR		struct nifinfo_cursor
#define	NIFINFO_ADDR		union nifinfo_address


union nifinfo_address {
	struct in6_addr	a6 ;
	struct in_addr	a4 ;
} ;

struct nifinfo_entry {
	uint64_t	flags ;		/* interface flags */
	NIFINFO_ADDR	addr ;		/* interface address */
	int		index ;		/* interface index */
	int		af ;		/* address family */
	int		alen ;		/* address length */
	char		inter[LIFNAMSIZ+1] ;	/* interface name */
} ;

struct nifinfo_head {
	uint		magic ;
	NIFINFO_ENT	*a ;
	int		n ;
} ;

struct nifinfo_cursor {
	int		i ;
} ;

typedef	NIFINFO			nifinfo ;
typedef	NIFINFO_ENT		nifinfo_ent ;
typedef	NIFINFO_CUR		nifinfo_cur ;
typedef	NIFINFO_ADDR		nifinfo_addr ;

EXTERNC_begin

extern int nifinfo_start(nifinfo *) noex ;
extern int nifinfo_get(nifinfo *,int,nifinfo_ent **) noex ;
extern int nifinfo_match(nifinfo *,int,void *,int) noex ;
extern int nifinfo_finish(nifinfo *) noex ;

EXTERNC_end


#endif /* NIFINFO_INCLUDE */



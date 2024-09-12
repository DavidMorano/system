/* nifinfo */


/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

#ifndef	NIFINFO_INCLUDE
#define	NIFINFO_INCLUDE		1


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


/* local object defines */

#define	NIFINFO			struct nifinfo_head
#define	NIFINFO_ENT		struct nifinfo_e
#define	NIFINFO_CUR		struct nifinfo_c


union nifinfo_addr {
	struct in6_addr	a6 ;
	struct in_addr	a4 ;
} ;

struct nifinfo_e {
	uint64_t		flags ;		/* interface flags */
	union nifinfo_addr	addr ;		/* interface address */
	int			index ;		/* interface index */
	int			af ;		/* address family */
	int			alen ;		/* address length */
	char			device[LIFNAMSIZ + 1];	/* interface name */
} ;

struct nifinfo_head {
	uint			magic ;
	struct nifinfo_e	*a ;
	int			n ;
} ;

struct nifinfo_c {
	int		i ;
} ;


#if	(! defined(NIFINFO_MASTER)) || (NIFINFO_MASTER == 0)

#ifdef	__cplusplus
extern "C" {
#endif

extern int nifinfo_start(NIFINFO *,const char *) ;
extern int nifinfo_get(NIFINFO *,int,NIFINFO_ENT **) ;
extern int nifinfo_match(NIFINFO *,int,void *,int) ;
extern int nifinfo_finish(NIFINFO *) ;

#ifdef	__cplusplus
}
#endif

#endif /* NIFINFO_MASTER */

#endif /* NIFINFO_INCLUDE */



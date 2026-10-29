/* SRVENTRY */

/* expanded server entry */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */


#ifndef	SRVENTRY_INCLUDE
#define	SRVENTRY_INCLUDE	1


#include	<envstandards.h>	/* MUST be first to configure */

#include	<varsub.h>
#include	<srvtab.h>


/* local object defines */

#define	SRVENTRY	struct srventry_head
#define	SRVENTRY_ARGS	struct srventry_a


struct srventry_a {
	cchar	*version ;	/* %V */
	cchar	*searchname ;	/* %S */
	cchar	*programroot ;	/* %R */
	cchar	*nodename ;	/* %N */
	cchar	*domainname ;	/* %D */
	cchar	*hostname ;	/* %H */
	cchar	*username ;	/* %U */
	cchar	*service ;
	cchar	*subservice ;
	cchar	*svcargs ;	/* service arguments! */
	cchar	*peername ;
	cchar	*ident ;	/* IDENT name if available */
	cchar	*nethost ;	/* reverse lookup (what is this ??) */
	cchar	*netuser ;	/* network username if available */
	cchar	*netpass ;	/* network password (encrypted ?) */
} ;

struct srventry_head {
	cchar	*program ;	/* server program path */
	cchar	*srvargs ;	/* server program arguments */
	cchar	*username ;
	cchar	*groupname ;
	cchar	*options ;
	cchar	*access ;
} ;


#if	(! defined(SRVENTRY_MASTER)) || (SRVENTRY_MASTER == 0)

#ifdef	__cplusplus
extern "C" {
#endif

extern int srventry_start(SRVENTRY *) ;
extern int srventry_finish(SRVENTRY *) ;
extern int srventry_process(SRVENTRY *,varsub *,char **,
			SRVTAB_ENTRY *,SRVENTRY_ARGS *) ;
extern int srventry_addprogram(SRVENTRY *,char *) ;
extern int srventry_addsrvargs(SRVENTRY *,char *) ;
extern int srventry_addusername(SRVENTRY *,char *) ;
extern int srventry_addgroupname(SRVENTRY *,char *) ;
extern int srventry_addoptions(SRVENTRY *,char *) ;

#ifdef	__cplusplus
}
#endif

#endif /* SRVENTRY_MASTER */

#endif /* SRVENTRY_INCLUDE */



/* srventry HEADER */
/* lang=C20 */

/* expanded server entry */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SRVENTRY_INCLUDE
#define	SRVENTRY_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<varsub.h>
#include	<srvtab.h>


/* local object defines */
#define	SRVENTRY	struct srventry_head
#define	SRVENTRY_ARGS	struct srventry_a


struct srventry_a {
	cchar		*version ;	/* %V */
	cchar		*searchname ;	/* %S */
	cchar		*programroot ;	/* %R */
	cchar		*nodename ;	/* %N */
	cchar		*domainname ;	/* %D */
	cchar		*hostname ;	/* %H */
	cchar		*username ;	/* %U */
	cchar		*service ;
	cchar		*subservice ;
	cchar		*svcargs ;	/* service arguments! */
	cchar		*peername ;
	cchar		*ident ;	/* IDENT name if available */
	cchar		*nethost ;	/* reverse lookup (what is this ??) */
	cchar		*netuser ;	/* network username if available */
	cchar		*netpass ;	/* network password (encrypted ?) */
} ;

struct srventry_head {
	cchar		*program ;	/* server program path */
	cchar		*srvargs ;	/* server program arguments */
	cchar		*username ;
	cchar		*groupname ;
	cchar		*options ;
	cchar		*access ;
} ;

typedef	SRVENTRY	srventry ;
typedef	SRVENTRY_ARGS	srventry_args ;

EXTERNC_begin

extern int srventry_start(seventry *) noex ;
extern int srventry_finish(seventry *) noex ;
extern int srventry_process(seventry *,varsub *,char **,
			SRVTAB_ENTRY *,srventry_args *) noex ;
extern int srventry_addprogram(seventry *,char *) noex ;
extern int srventry_addsrvargs(seventry *,char *) noex ;
extern int srventry_addusername(seventry *,char *) noex ;
extern int srventry_addgroupname(seventry *,char *) noex ;
extern int srventry_addoptions(seventry *,char *) noex ;

EXTERNC_end


#endif /* SRVENTRY_INCLUDE */



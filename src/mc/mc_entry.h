/* mc_entry SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* program entry */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	PROGENTRY_INCLUDE
#define	PROGENTRY_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usyscalls.h>
#include	<varsub.h>
#include	<svcfile.h>
#include	<localmisc.h>


#define	PROGENTRY		struct progentry_head
#define	PROGENTRY_FL		struct progentry_flags
#define	PROGENTRY_A		struct progentry_args

#define	PROGENTRY_TMPDIR	"/tmp"
#define	PROGENTRY_IDLEN		14


struct progentry_a {
	cchar		*version ;	/* %V */
	cchar		*programroot ;	/* %R */
	cchar		*nodename ;	/* %N */
	cchar		*domainname ;	/* %D */
	cchar		*hostname ;	/* %H */
	cchar		*username ;	/* %U */
	cchar		*groupname ;	/* %G */
	cchar		*service ;	/* %s service name */
	cchar		*interval ;	/* %i interval (decimal secs) */
	cchar		*logid ;	/* ID for logging */
	cchar		*tmpdname ;
	time_t		daytime ;	/* time of day (UNIX) */
} ;

struct progentry_flags {
	uint		srvargs:1 ;		/* initialized? */
} ;

struct progentry_head {
	char		*program ;		/* server program path */
	char		*username ;
	char		*groupname ;
	char		*options ;
	char		*access ;		/* access hosts or groups */
	vecstr		srvargs ;		/* server program arguments */
	varsub		*ssp ;			/* string substitutions */
	time_t		atime ;			/* job arrival time */
	pid_t		pid ;			/* run flag */
	PROGENTRY_FL	fl ;
	uint		magic ;
	int		interval ;		/* interval (seconds) */
	char		name[MAXNAMELEN + 1] ;	/* service name */
	char		jobid[PROGENTRY_IDLEN + 1] ;
	char		ofname[MAXPATHLEN + 1] ;
	char		efname[MAXPATHLEN + 1] ;
} ;

typedef PROGENTRY	progentry ;
typedef PROGENTR_A	progentry_a ;

EXTERNC_begin

extern int progentry_init(PROGENTRY *,VARSUB *,
			SVCFILE_ENT *,PROGENTRY_ARGS *) noex ;
extern int progentry_expand(PROGENTRY *,SVCFILE_ENT *,PROGENTRY_ARGS *) noex ;
extern int progentry_getinterval(PROGENTRY *,int *) noex ;
extern int progentry_getaccess(PROGENTRY *,char **) noex ;
extern int progentry_free(PROGENTRY *) noex ;

EXTERNC_end


#endif /* PROGENTRY_INCLUDE */




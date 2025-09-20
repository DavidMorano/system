/* svcentry HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* service entry */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-01, David A­D­ Morano
	This code was written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SVCENTRY_INCLUDE
#define	SVCENTRY_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* |pid_t| */
#include	<time.h>		/* |time_t| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<vecstr.h>
#include	<varsub.h>
#include	<svcfile.h>


#define	SVCENTRY		struct svcentry_head
#define	SVCENTRY_FL		struct svcentry_flags
#define	SVCENTRY_ARGS		struct svcentry_arguments
#define	SVCENTRY_MAGIC		0x76452376
#define	SVCENTRY_IDLEN		15


struct svcentry_arguments {
	cchar		*version ;	/* %V */
	cchar		*programroot ;	/* %R */
	cchar		*nodename ;	/* %N */
	cchar		*domainname ;	/* %D */
	cchar		*hostname ;	/* %H */
	cchar		*username ;	/* %U */
	cchar		*groupname ;	/* %G */
	cchar		*service ;	/* %s service name */
	cchar		*interval ;	/* %i interval (decimal secs) */
	cchar		*jobid ;	/* ID for logging */
	cchar		*tmpdname ;
	time_t		daytime ;	/* time of day (UNIX) */
} ;

struct svcentry_flags {
	uint		srvargs:1 ;	/* initialized? */
} ;

struct svcentry_head {
	vecstr		*sap ;		/* server program arguments */
	varsub		*ssp ;		/* string substitutions */
	char		*name ;		/* service name (allocated) */
	cchar		*program ;	/* server program path */
	cchar		*username ;
	cchar		*groupname ;
	cchar		*options ;
	cchar		*access ;	/* access hosts or groups */
	cchar		*ofname ;
	cchar		*efname ;
	time_t		atime ;		/* job arrival time */
	time_t		stime ;		/* job start time */
	pid_t		pid ;		/* run flag */
	SVCENTRY_FL	fl ;
	uint		magic ;
	int		interval ;	/* interval (seconds) */
	char		jobid[SVCENTRY_IDLEN + 1] ;
} ;


typedef SVCENTRY	svcentry ;
typedef SVCENTRY_FL	svcentry_fl ;
typedef SVCENTRY_ARGS	svcentry_args ;

EXTERNC_begin

extern int svcentry_start(svcentry *,varsub *,svcfile_ent *,
		svcentry_args *) noex ;
extern int svcentry_expand(svcentry *,svcfile_ent *,svcentry_args *) noex ;
extern int svcentry_getinterval(svcentry *,int *) noex ;
extern int svcentry_arrival(svcentry *,time_t *) noex ;
extern int svcentry_getaccess(svcentry *,cchar **) noex ;
extern int svcentry_getargs(svcentry *,mainv *) noex ;
extern int svcentry_stime(svcentry *,time_t) noex ;
extern int svcentry_finish(svcentry *) noex ;

EXTERNC_end


#endif /* SVCENTRY_INCLUDE */



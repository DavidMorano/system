/* ugetpw */

/* UNIX� password entry fetching */


/* revision history:

	= 1998-02-01, David A�D� Morano
	This subroutine was originally written.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

#ifndef	UGETPW_INCLUDE
#define	UGETPW_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<pwd.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<localmisc.h>


#define	UGETPW_STATS	struct ugetpw_stats

#define	UGETPW_MAX	120		/* max cache entries */
#define	UGETPW_TTL	(20*60)		/* entry time-out in seconds */
#define	UGETPW_TOLOCK	5


struct ugetpw_stats {
	uint		max ;
	uint		ttl ;
	uint		nent ;
	uint		acc ;
	uint		phit, nhit ;
	uint		pmis, nmis ;
} ;

#ifdef	__cplusplus
extern "C" {
#endif

extern int ugetpw_init() ;
extern int ugetpw_name(PASSWD *,char *,int,const char *) ;
extern int ugetpw_uid(PASSWD *,char *,int,uid_t) ;
extern int ugetpw_stats(struct ugetpw_stats *) ;
extern int ugetpw_setparam(int,int) ;
extern int ugetpw_fini() ;

#ifdef	__cplusplus
}
#endif


#endif /* UGETPW_INCLUDE */



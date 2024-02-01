/* ucygetpw HEADER */
/* lang=C20 */

/* UNIX® password entry fetching */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	UCYGETPW_INCLUDE
#define	UCYGETPW_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<pwd.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<localmisc.h>


#define	UCYGETPW_STATS	struct ugetpw_stats

#define	UCYGETPW_MAX	120		/* max cache entries */
#define	UCYGETPW_TTL	(20*60)		/* entry time-out in seconds */
#define	UCYGETPW_TOLOCK	5


struct ugetpw_stats {
	uint		max ;
	uint		ttl ;
	uint		nent ;
	uint		acc ;
	uint		phit, nhit ;
	uint		pmis, nmis ;
} ;

EXTERNC_begin

extern int ugetpw_init() noex ;
extern int ugetpw_name(PASSWD *,char *,int,cchar *) noex ;
extern int ugetpw_uid(PASSWD *,char *,int,uid_t) noex ;
extern int ugetpw_stats(UCYGETPW *) noex ;
extern int ugetpw_setparam(int,int) noex ;
extern int ugetpw_fini() noex ;

EXTERNC_end


#endif /* UCYGETPW_INCLUDE */



/* prqotd_subinfo HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* open a channel (file-descriptor) to the quote-of-the-day (QOTD) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	prqotd_maint

	Description:
	This subroutine sort of forms the back-end maintenance end
	of the QOTD mechansim.

	Synopsis:
	int prqotd_maint(cchar *pr,int mjd,int of,int to) noex

	Arguments:
	pr		program-root
	mjd		modified-julian-day
	of		open-flags
	to		time-out

	Returns:
	>=0		FD of QOTD
	<0		error (system-return)

*******************************************************************************/

#ifndef	PRQOTDSUBINFO_INCLUDE
#define	PRQOTDSUBINFO_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* system types */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<ids.h>
#include	<vecstr.h>
#include	<vecpstr.h>
#include	<paramfile.h>
#include	<expcook.h>
#include	<localmisc.h>		/* |DIGBUFLEN| */


#ifndef	SUBINFO
#define	SUBINFO		subinfo_head
#endif
#ifndef	SUBINFO_FL
#define	SUBINFO_FL	subinfo_flags
#endif


namespace prqotd {
    struct subinfo_flags {
	uint		stores:1 ;
	uint		id:1 ;
	uint		hosts:1 ;
	uint		sources:1 ;
	uint		logsub:1 ;
	uint		logsize:1 ;
	uint		spooldir:1 ;
	uint		lfname:1 ;
	uint		to:1 ;
    } ; /* end struct (subinfo_flags) */
    struct subinfo_head {
	vecstr		stores ;
	vecpstr		hosts ;
	vecpstr		sources ;
	ids		id ;
	SUBINFO_FL	fl, have, changed, finval ;
	SUBINFO_FL	open ;
	cchar		*pr ;
	cchar		*pn ;		/* program-name */
	cchar		*sn ;		/* search-name */
	cchar		*nn ;		/* node-name */
	cchar		*dn ;		/* domain-name */
	cchar		*un ;		/* user-name */
	cchar		*logid ;	/* log-ID */
	cchar		*lfname ;	/* log file-name */
	cchar		*hostname ;
	cchar		*spooldname ;	/* spool directory */
	cchar		*qdname ;	/* particular quote directory */
	void		*cfp ;		/* config-file-pointer */
	void		*lfp ;		/* log-file-pointer state */
	uid_t		euid ;
	uid_t		uid_pr ;
	gid_t		gid_pr ;
	time_t		dt ;
	int		of ;
	int		to ;
	int		mjd ;
	int		logsize ;
	mode_t		om ;
    } ; /* end struct (subinfo_head) */
} /* end namespace (prqotd) */

namespace prqotd {
    typedef SUBINFO	subinfo ;
    extern int	subinfo_start(subinfo *,time_t,cchar *,int,int,int) noex ;
    extern int	subinfo_finish(subinfo *) noex ;
    extern int	subinfo_logbegin(subinfo *) noex ;
    extern int	subinfo_logend(subinfo *) noex ;
    extern int	subinfo_defaults(subinfo *) noex ;
    extern int	subinfo_spoolcheck(subinfo *) noex ;
    extern int	subinfo_qdirname(subinfo *,int) noex ;
    extern int	subinfo_gather(subinfo *,cchar *,mode_t) noex ;
    extern int	subinfo_prog(subinfo *,cchar *,cchar *) noex ;
    extern int	subinfo_setfname(subinfo *,char *,cc *,int,int,
	    		cc *,cc *,cc *) noex ;
    extern int	subinfo_setentry(subinfo *,cchar **,cchar *,int) noex ;
    extern int	subinfo_spooldir(subinfo *,cchar *,int) noex ;
    extern int	subinfo_hostname(subinfo *sip,cchar *,int) noex ;
    extern int	subinfo_source(subinfo *,cchar *,int) noex ;
} /* end namespace (prqotd) */


#endif /* PRQOTDSUBINFO_INCLUDE */



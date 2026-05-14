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
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<tzfile.h>		/* for TM_YEAR_BASE */
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<uclibmem.h>
#include	<ucgetpid.h>
#include	<estrings.h>
#include	<mkfnamesuf.h>
#include	<ids.h>
#include	<sigman.h>
#include	<tmtime.hh>
#include	<storebuf.h>
#include	<vecstr.h>
#include	<vecpstr.h>
#include	<ascii.h>
#include	<paramfile.h>
#include	<expcook.h>
#include	<logfile.h>
#include	<isnot.h>
#include	<ischarx.h>
#include	<localmisc.h>		/* |DIGBUFLEN| */


#define	SUBINFO		subinfo_head

#ifndef	SI
#define	SI		subinfo_head
#endif
#ifndef	SI_FL
#define	SI_FL		subinfo_flags
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
	SI_FL		fl, have, changed, finval ;
	SI_FL		open ;
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
	void		*config ;	/* configuration state */
	void		*logsub ;	/* log-file state */
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
    extern int	subinfo_start(SI *,time_t,cchar *,int,int,int) noex ;
    extern int	subinfo_finish(SI *) noex ;
    extern int	subinfo_confbegin(SI *) noex ;
    extern int	subinfo_confend(SI *) noex ;
    extern int	subinfo_setentry(SI *,cchar **,cchar *,int) noex ;
    extern int	subinfo_envbegin(SI *) noex ;
    extern int	subinfo_envend(SI *) noex ;
    extern int	subinfo_logfile(SI *,cchar *,int) noex ;
    extern int	subinfo_hostname(SI *,cchar *,int) noex ;
    extern int	subinfo_source(SI *,cchar *,int) noex ;
    extern int	subinfo_logbegin(SI *) noex ;
    extern int	subinfo_logend(SI *) noex ;
    extern int	subinfo_logenv(SI *) noex ;
    extern int	subinfo_defaults(SI *) noex ;
    extern int	subinfo_spooldir(SI *,cchar *,int) noex ;
    extern int	subinfo_spoolcheck(SI *) noex ;
    extern int	subinfo_qdirname(SI *,int) noex ;
    extern int	subinfo_gather(SI *,cchar *,mode_t) noex ;
    extern int	subinfo_opensource(SI *,cchar *,cchar *) noex ;
    extern int	subinfo_opensourceprog(SI *,cchar *,cchar *) noex ;
    extern int subinfo_defprog(SI *,cchar *) noex ;
    extern int subinfo_defproger(SI *,vecstr *,cchar *,cchar *) noex ;
    extern int subinfo_addourpath(SI *,vecstr *) noex ;
    extern int subinfo_addprbins(SI *,vecstr *) noex ;
    extern int subinfo_addprbin(SI *,vecstr *,cchar *,cchar *) noex ;
    extern int subinfo_id(SI *) noex ;
    extern int subinfo_dircheck(SI *,cchar *) noex ;
    extern int subinfo_dirminmode(SI *,cchar *,mode_t) noex ;
    extern int subinfo_setfname(SI *sip,char *,cc *,int,int,
	    cc *,cc *,cc *) noex ;
} /* end namespace (prqotd) */

namespace prqotd {
    typedef SUBINFO	subinfo ;
} /* end namespace (prqotd) */


#endif /* PRQOTDSUBINFO_INCLUDE */



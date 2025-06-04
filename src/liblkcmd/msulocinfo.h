/* msu-locinfo HEADER */
/* encoding=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* MSU-locinfo (extra code) */
/* version %I% last-modified %G% */


/* revision history:

   	= 1998-02-15, David A­D­ Morano
	I originally wrote this.

	= 2011-01-25, David A­D­ Morano
	I had to separate this code due to AST-code conflicts over
	the system socket structure definitions.

*/

/* Copyright © 1998,2011 David A­D­ Morano.  All rights reserved. */

#ifndef	MSULOCINFO_INCLUDE
#define	MSULOCINFO_INCLUDE


#include	<envstandards.h>	/* must be ordered fist to configure */
#include	<sys/types.h>
#include	<vecstr.h>
#include	<msfile.h>
#include	<lfm.h>

#include	"msumain.h"
#include	"defs.h"		/* for PROGINFO */


#define	LOCINFO		struct locinfo
#define	LOCINFO_FL	struct locinfo_flags


struct locinfo_flags {
	uint		stores:1 ;
	uint		lockinfo:1 ;
	uint		intspeed:1 ;
	uint		intconfig:1 ;
	uint		zerospeed:1 ;
	uint		quick:1 ;
	uint		listen:1 ;	/* listen on IPC */
	uint		reuseaddr:1 ;	/* reuse for multiple listeners */
	uint		msfname:1 ;
	uint		tmpfname:1 ;	/* TMP lock-file */
	uint		reqfname:1 ;
	uint		mntfname:1 ;
	uint		pidlock:1 ;	/* this is LFM on PID-file */
	uint		tmplock:1 ;	/* this is LFM on TMPPID-file */
	uint		sec_root:1 ;
	uint		sec_conf:1 ;
	uint		secure:1 ;
	uint		fg:1 ;
	uint		speedname:1 ;
	uint		reqexit:1 ;
} ;

struct locinfo {
	vecstr		stores ;
	cchar		*msnode ;
	cchar		*tmpourdname ;
	cchar		*msfname ;
	cchar		*tmpfname ;	/* TMP lock-file */
	cchar		*reqfname ;
	cchar		*mntfname ;
	cchar		*speedname ;
	PROGINFO	*pip ;
	LOCINFO_FL	have, f, changed, final ;
	LOCINFO_FL	open ;
	LFM		pidlock, tmplock ;
	time_t		ti_lastlock ;
	time_t		ti_start ;
	time_t		ti_marklog ;
	time_t		ti_boot ;
	uid_t		uid_rootname ;
	gid_t		gid_rootname ;
	int		intconfig ;	/* interval configuration changed */
	int		intspeed ;	/* interval speed update */
	int		nu ;		/* n-updates */
	int		rfd ;		/* request file-descriptor */
	int		to_cache ;
	int		to_lock ;
	char		cmd[LOGIDLEN + 1] ;	/* for MSU */
} ;

EXTERNC_begin

extern int	locinfo_start(LOCINFO *,PROGINFO *) noex ;
extern int	locinfo_finish(LOCINFO *) noex ;
extern int	locinfo_setentry(LOCINFO *,cchar **,cchar *,int) noex ;
extern int	locinfo_rootname(LOCINFO *) noex ;
extern int	locinfo_defs(LOCINFO *) noex ;
extern int	locinfo_lockbegin(LOCINFO *) noex ;
extern int	locinfo_lockcheck(LOCINFO *) noex ;
extern int	locinfo_lockend(LOCINFO *) noex ;
extern int	locinfo_defreg(LOCINFO *) noex ;
extern int	locinfo_defdaemon(LOCINFO *) noex ;
extern int	locinfo_tmpourdname(LOCINFO *) noex ;
extern int	locinfo_msfile(LOCINFO *) noex ;
extern int	locinfo_reqfname(LOCINFO *) noex ;
extern int	locinfo_ipcpid(LOCINFO *,int) noex ;
extern int	locinfo_gidrootname(LOCINFO *) noex ;
extern int	locinfo_reqexit(LOCINFO *,cchar *) noex ;
extern int	locinfo_isreqexit(LOCINFO *) noex ;

EXTERNC_end


#endif /* MSULOCINFO_INCLUDE */



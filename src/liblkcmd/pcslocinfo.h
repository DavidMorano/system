/* pcs-locinfo HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* PCS-locinfo (extra code) */
/* version %I% last-modified %G% */


/* revision history:

   	= 1998-02-15, David A­D­ Morano
	I originally wrote this.

	= 2011-01-25, David A­D­ Morano
	I modified this in some way.

*/

/* Copyright © 1998,2011 David A­D­ Morano.  All rights reserved. */

#ifndef	PCSLOCINFO_INCLUDE
#define	PCSLOCINFO_INCLUDE


#include	<envstandards.h>	/* must be ordered fist to configure */
#include	<sys/types.h>
#include	<vecstr.h>
#include	<keyopt.h>
#include	<msfile.h>
#include	<pcsns.h>
#include	<lfm.h>

#include	"pcsmain.h"
#include	"defs.h"		/* for PROGINFO */


#define	LOCINFO		struct locinfo
#define	LOCINFO_FL	struct locinfo_flags


struct locinfo_flags {
	uint		stores:1 ;
	uint		lockinfo:1 ;
	uint		intconf:1 ;
	uint		intsvcs:1 ;
	uint		intwait:1 ;
	uint		intcache:1 ;
	uint		intmaint:1 ;
	uint		intspeed:1 ;
	uint		intdirmaint:1 ;
	uint		intclient:1 ;
	uint		zerospeed:1 ;
	uint		quick:1 ;
	uint		adj:1 ;		/* activate adjunct */
	uint		reuseaddr:1 ;	/* reuse for multiple listeners */
	uint		tmpfname:1 ;	/* TMP lock-file */
	uint		reqfname:1 ;
	uint		mntfname:1 ;
	uint		msfname:1 ;
	uint		locking:1 ;
	uint		pidlock:1 ;	/* this is LFM on PID-file */
	uint		tmplock:1 ;	/* this is LFM on TMPPID-file */
	uint		sec_root:1 ;
	uint		sec_conf:1 ;
	uint		secure:1 ;
	uint		fg:1 ;
	uint		speedname:1 ;
	uint		ns:1 ;
	uint		cmds:1 ;
	uint		reqexit:1 ;
	uint		runasprn:1 ;
	uint		maint:1 ;
} ;

struct locinfo {
	vecstr		stores ;
	cchar		*msnode ;
	cchar		*tmpourdname ;
	cchar		*tmpfname ;	/* TMP lock-file */
	cchar		*reqfname ;
	cchar		*mntfname ;
	cchar		*msfname ;
	cchar		*speedname ;
	PROGINFO	*pip ;
	void		*adj ;		/* adjunct local data */
	LOCINFO_FL	have, f, changed, final ;
	LOCINFO_FL	open ;
	LFM		pidlock, tmplock ;
	PCSNS		ns ;
	KEYOPT		cmds ;
	time_t		ti_lastlock ;
	time_t		ti_start ;
	time_t		ti_marklog ;
	time_t		ti_boot ;
	time_t		ti_lastreq ;
	time_t		ti_tmpmaint ;
	uid_t		uid_rootname ;
	gid_t		gid_rootname ;
	int		kserial ;
	int		serial ;
	int		nu ;		/* n-updates */
	int		rfd ;		/* request file-descriptor */
	int		intconf ;
	int		intsvcs ;
	int		intwait ;
	int		intcache ;
	int		intmaint ;
	int		intspeed ;
	int		intdirmaint ;	/* interval dir-maintenance */
	int		intclient ;	/* interval client sockets */
	int		to_cache ;
	int		to_lock ;
	int		nreqs ;		/* count of requests */
	char		cmd[LOGIDLEN + 1] ;	/* for PCS */
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
extern int	locinfo_tmpourdir(LOCINFO *) noex ;
extern int	locinfo_msfile(LOCINFO *) noex ;
extern int	locinfo_reqfname(LOCINFO *) noex ;
extern int	locinfo_ipcpid(LOCINFO *,int) noex ;
extern int	locinfo_rootids(LOCINFO *) noex ;
extern int	locinfo_nsbegin(LOCINFO *) noex ;
extern int	locinfo_nsend(LOCINFO *) noex ;
extern int	locinfo_nslook(LOCINFO *,char *,int,cchar *,int) noex ;
extern int	locinfo_dirmaint(LOCINFO *) noex ;
extern int	locinfo_cmdsload(LOCINFO *,cchar *,int) noex ;
extern int	locinfo_cmdscount(LOCINFO *) noex ;
extern int	locinfo_reqexit(LOCINFO *,cchar *) noex ;
extern int	locinfo_isreqexit(LOCINFO *) noex ;
extern int	locinfo_newreq(LOCINFO *,int) noex ;
extern int	locinfo_getreqs(LOCINFO *) noex ;

EXTERNC_end


#endif /* PCSLOCINFO_INCLUDE */



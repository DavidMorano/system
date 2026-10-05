/* defs (VMAIL) */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */


#ifndef	DEFS_INCLUDE
#define	DEFS_INCLUDE	1


#include	<envstandards.h>	/* MUST be first to configure */

#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/timeb.h>

#include	<vecstr.h>
#include	<logfile.h>
#include	<ids.h>
#include	<userinfo.h>
#include	<expcook.h>
#include	<localmisc.h>


#ifndef	FD_STDIN
#define	FD_STDIN	0
#define	FD_STDOUT	1
#define	FD_STDERR	2
#endif

#ifndef	MAXPATHLEN
#define	MAXPATHLEN	1024
#endif

#ifndef	LINEBUFLEN
#ifdef	LINE_MAX
#define	LINEBUFLEN	MAX(LINE_MAX,2048)
#else
#define	LINEBUFLEN	2048
#endif
#endif

#ifndef	ARCHBUFLEN
#define	ARCHBUFLEN	80
#endif

#ifndef	DIGBUFLEN
#define	DIGBUFLEN	40		/* can hold int128_t in decimal */
#endif

#ifndef	ZNAMELEN
#define	ZNAMELEN	8
#endif

#ifndef	TZLEN
#define	TZLEN		60
#endif

#ifndef	REALNAMELEN
#define	REALNAMELEN	100		/* real-name length */
#endif

#ifndef	KBUFLEN
#define	KBUFLEN		120
#endif

#ifndef	VBUFLEN
#define	VBUFLEN		(6 * MAXPATHLEN)
#endif

#ifndef	TIMEBUFLEN
#define	TIMEBUFLEN	80
#endif

#ifndef	DEBUGLEVEL
#define	DEBUGLEVEL(n)	(pip->debuglevel >= (n))
#endif

#ifndef	BUFLEN
#define	BUFLEN		(2 * MAXPATHLEN)
#endif

#define	NUO		4

#define	UOV_DEBUG	0
#define	UOV_BB		1

#define	NSTAT		4

#define	STATV_LOCKFAIL	0
#define	STATV_READONLY	1
#define	STATV_WINCHANGE 2
#define	STATV_INPUT	3
#define	STATV_SVR4	4
#define	STATV_630	5
#define	STATV_SYSVCT	6

#define	PROGINFO	struct proginfo
#define	PROGINFO_FL	struct proginfo_flags

#define	PIVARS		struct pivars

#define	ARGINFO		struct arginfo


struct proginfo_flags {
	uint		progdash:1 ;
	uint		akopts:1 ;
	uint		akparams:1 ;
	uint		quiet:1 ;
	uint		errfile:1 ;
	uint		sysv_rt:1 ;
	uint		sysv_ct:1 ;
	uint		svars:1 ;
	uint		pc:1 ;		/* program-configuration */
	uint		pcsconf:1 ;	/* PCS configuration */
	uint		pcspoll:1 ;
	uint		proglocal:1 ;
	uint		cooks:1 ;
	uint		secure_root:1 ;
	uint		secure_conf:1 ;
	uint		cfname:1 ;
	uint		lfname:1 ;
	uint		cmdfname:1 ;
	uint		nosysconf:1 ;
	uint		logprog:1 ;
	uint		logsize:1 ;
	uint		bb:1 ;
	uint		useclen:1 ;	/* use content-length? */
	uint		useclines:1 ;	/* use content-lines? */
	uint		mailusers:1 ;	/* mail-users */
	uint		mailget:1 ;	/* get new mail? */
	uint		mailcheck:1 ;	/* check for new mail */
	uint		mesgs:1 ;	/* existing MESG setting */
	uint		mailnew:1 ;	/* new mail is available */
	uint		clock:1 ;	/* run the clock */
	uint		nextdel:1 ;	/* use 'next-delete' behavior */
	uint		nextmov:1 ;	/* use 'next-move' behavior */
	uint		svpercent:1 ;	/* scan-view percent */
	uint		sjpercent:1 ;	/* scan-jump percent */
	uint		svspec:1 ;	/* scan-view spec */
	uint		sjspec:1 ;	/* scan-jump spec */
	uint		shell:1 ;	/* shell */
	uint		linelen:1 ;	/* line-length */
	uint		passfd:1 ;
	uint		named:1 ;
	uint		winadj:1 ;
	uint		deldup:1 ;	/* delete duplication messages */
} ;

struct proginfo {
	vecstr		stores ;
	vecstr		mailusers ;
	logfile		lh ;
	IDS		id ;
	vecstr		svars ;
	EXPCOOK		cooks ;
	USERINFO	*uip ;
	void		*iap ;
	cchar	**envv ;
	cchar	*pwd ;
	cchar	*progename ;
	cchar	*progdname ;
	cchar	*progname ;
	cchar	*pr ;
	cchar	*searchname ;
	cchar	*version ;
	cchar	*banner ;
	cchar	*usysname ;	/* UNAME OS system-name */
	cchar	*umachine ;	/* UNAME machine name */
	cchar	*urelease ;	/* UNAME OS release */
	cchar	*uversion ;	/* UNAME OS version */
	cchar	*architecture ;	/* UAUX machine architecture */
	cchar	*platform ;	/* UAUX machine platform */
	cchar	*provider ;	/* UAUX machine provider */
	cchar	*hz ;		/* OS HZ */
	cchar	*nodename ;	/* USERINFO */
	cchar	*domainname ;	/* USERINFO */
	cchar	*username ;	/* USERINFO */
	cchar	*userhome ;	/* USERINFO */
	cchar	*shell ;	/* USERINFO */
	cchar	*org ;		/* USERINFO */
	cchar	*gecosname ;	/* USERINFO */
	cchar	*realname ;	/* USERINFO */
	cchar	*name ;		/* USERINFO */
	cchar	*fullname ;	/* USERINFO full-name */
	cchar	*mailname ;	/* USERINFO mail-abbreviated-name */
	cchar	*tz ;		/* USERINFO */
	cchar	*maildname ;	/* USERINFO */
	cchar	*logid ;	/* USERINFO ID for logging purposes */
	cchar	*groupname ;
	cchar	*rootname ;
	cchar	*hostname ;	/* concatenation of N + D */
	cchar	*tmpdname ;	/* temporary directory */
	cchar	*folderdname ;	/* user mail-folder directory */
	cchar	*vmdname ;	/* MSG-body cache directory */
	cchar	*hfname ;	/* help file-name */
	cchar	*cfname ;	/* config file-name */
	cchar	*lfname ;	/* log file-name */
	cchar	*cmdfname ;	/* help file-name */
	cchar	*pidfname ;
	cchar	*mbname_def ;	/* mailbox default (starting) */
	cchar	*mbname_in ;	/* mailbox incoming */
	cchar	*mbname_spam ;	/* mailbox spam */
	cchar	*mbname_trash ;	/* mailbox trash */
	cchar	*mbname_cur ;	/* mailbox current */
	cchar	*termtype ;	/* terminal type 'TERM' */
	cchar	*kbdtype ;	/* keyboard type 'KEYBOARD' */
	cchar	*prog_shell ;
	cchar	*prog_getmail ;
	cchar	*prog_mailer ;
	cchar	*prog_editor ;
	cchar	*prog_metamail ;
	cchar	*prog_pager ;
	cchar	*prog_postspam ;
	cchar	*svspec ;
	cchar	*sjspec ;
	cchar	*testmsg ;
	void		*pcsconf ;
	void		*pcspoll ;
	void		*config ;
	void		*userlist ;
	void		*efp ;
	void		*ofp ;
	void		*pcp ;
	struct timeb	now ;
	PROGINFO_FL	have, f, changed, finval ;
	PROGINFO_FL	open ;
	time_t		daytime ;
	time_t		ti_start ;
	time_t		ti_mailcheck ;	/* last mail check */
	time_t		ti_poll ;	/* last poll */
	pid_t		pid ;
	uid_t		uid, euid ;
	gid_t		gid, egid ;
	gid_t		gid_mail ;
	int		to_config ;
	int		to_clock ;
	int		to_read ;
	int		to_info ;
	int		to_child ;
	int		pwdlen ;
	int		debuglevel ;	/* debugging level */
	int		verboselevel ;	/* verbosity level */
	int		logsize ;
	int		providerid ;
	int		ncpu ;
	int		tfd ;		/* terminal file descriptor */
	int		mailcheck ;	/* timer interval between mail checks */
	int		linelen ;	/* line-length (columns) */
	int		lines_term ;	/* actual terminal lines */
	int		lines_req ;	/* number of lines to use */
	int		svlines ;	/* number of lines for scan-view */
	int		sjlines ;	/* number of lines for scan-jump */
	char		zname[ZNAMELEN + 1] ;	/* local time zone */
} ;

struct pivars {
	cchar	*vpr1 ;
	cchar	*vpr2 ;
	cchar	*vpr3 ;
	cchar	*pr ;
	cchar	*vprname ;
} ;

struct arginfo {
	cchar	**argv ;
	int		argc ;
	int		ai, ai_max, ai_pos ;
} ;


#ifdef	__cplusplus
extern "C" {
#endif

extern int proginfo_start(PROGINFO *,cchar **,cchar *,cchar *) ;
extern int proginfo_setentry(PROGINFO *,cchar **,cchar *,int) ;
extern int proginfo_setversion(PROGINFO *,cchar *) ;
extern int proginfo_setbanner(PROGINFO *,cchar *) ;
extern int proginfo_setsearchname(PROGINFO *,cchar *,cchar *) ;
extern int proginfo_setprogname(PROGINFO *,cchar *) ;
extern int proginfo_setexecname(PROGINFO *,cchar *) ;
extern int proginfo_setprogroot(PROGINFO *,cchar *,int) ;
extern int proginfo_pwd(PROGINFO *) ;
extern int proginfo_rootname(PROGINFO *) ;
extern int proginfo_progdname(PROGINFO *) ;
extern int proginfo_progename(PROGINFO *) ;
extern int proginfo_nodename(PROGINFO *) ;
extern int proginfo_getpwd(PROGINFO *,char *,int) ;
extern int proginfo_getename(PROGINFO *,char *,int) ;
extern int proginfo_getenv(PROGINFO *,cchar *,int,cchar **) ;
extern int proginfo_finish(PROGINFO *) ;

#ifdef	__cplusplus
}
#endif

#endif /* DEFS_INCLUDE */



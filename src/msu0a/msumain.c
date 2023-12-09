/* msu-main */

/* update the machine status for the current machine */
/* version %I% last-modified %G% */


#define	CF_DEBUGS	0		/* non-switchable debug print-outs */
#define	CF_DEBUG	0		/* switchable at invocation */
#define	CF_DEBUGMALL	1		/* debug memory-allocations */
#define	CF_CPUSPEED	1		/* calculate CPU speed */
#define	CF_UGETPW	1		/* use |ugetpw(3uc)| */


/* revision history:

	= 2004-03-01, David A�D� Morano
	This subroutine was originally written.  

	= 2005-04-20, David A�D� Morano
	I changed the program so that the configuration file is consulted even
	if the program is not run in daemon-mode.  Previously, the
	configuration file was only consulted when run in daemon-mode.  The
	thinking was that running the program in regular (non-daemon) mode
	should be quick.  The problem is that the MS file had to be guessed
	without the aid of consulting the configuration file.  Although not a
	problem in most practice, it was not aesthetically appealing.  It meant
	that if the administrator changed the MS file in the configuration
	file, it also had to be changed by specifying it explicitly at
	invocation in non-daemon-mode of the program.  This is the source of
	some confusion (which the world really doesn't need).  So now the
	configuration is always consulted.  The single one-time invocation is
	still fast enough for the non-smoker aged under 40!

	= 2011-01-25, David A�D� Morano
	Code was removed and placed in other files (so that they can be
	compiled differently) due to AST-code conflicts over the system
	socket-library structure definitions.

*/

/* Copyright � 2004,2005,2011 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	This is a built-in command to the KSH shell.  It should also be able to
	be made into a stand-alone program without much (if almost any)
	difficulty, but I have not done that yet (we already have a MSU program
	out there).

	Note that special care needed to be taken with the child processes
	because we cannot let them ever return normally!  They cannot return
	since they would be returning to a KSH program that thinks it is alive
	(!) and that geneally causes some sort of problem or another.  That is
	just some weird thing asking for trouble.  So we have to take care to
	force child processes to exit explicitly.  Child processes are only
	created when run in "daemon" mode.

	Synopsis:

	$ msu [-speed[=<name>]] [-zerospeed] [-db <file>]


	Implementation note:

	It is difficult to close files when run as a SHELL builtin!  We want to
	close files when we run in the background, but when running as a SHELL
	builtin, we cannot close file descriptors untill after we fork (else we
	corrupt the enclosing SHELL).  However, we want to keep the files
	associated with persistent objects open across the fork.  This problem
	is under review.  Currently, there is not an adequate self-contained
	solution.


*******************************************************************************/


#include	<envstandards.h>	/* MUST be first to configure */

#if	defined(SFIO) && (SFIO > 0)
#define	CF_SFIO	1
#else
#define	CF_SFIO	0
#endif

#if	(defined(KSHBUILTIN) && (KSHBUILTIN > 0))
#include	<shell.h>
#endif

#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<sys/socket.h>
#include	<sys/uio.h>
#include	<sys/msg.h>
#include	<unistd.h>
#include	<signal.h>
#include	<fcntl.h>
#include	<stropts.h>
#include	<poll.h>
#include	<limits.h>
#include	<stdlib.h>
#include	<string.h>
#include	<time.h>
#include	<netdb.h>

#include	<usystem.h>
#include	<ugetpid.h>
#include	<bits.h>
#include	<keyopt.h>
#include	<vecstr.h>
#include	<msfile.h>
#include	<lfm.h>
#include	<getax.h>
#include	<ugetpw.h>
#include	<getxusername.h>
#include	<ascii.h>
#include	<toxc.h>
#include	<spawner.h>
#include	<sysmemutil.h>
#include	<ucmallreg.h>
#include	<exitcodes.h>
#include	<localmisc.h>

#include	"shio.h"
#include	"kshlib.h"
#include	"msumain.h"
#include	"msuconfig.h"
#include	"msulocinfo.h"
#include	"msuadj.h"
#include	"msulog.h"
#include	"defs.h"
#include	"msflag.h"
#include	"msumsg.h"


/* local typedefs */


/* local defines */

#if	CF_UGETPW
#define	GETPW_NAME	ugetpw_name
#else
#define	GETPW_NAME	getpw_name
#endif /* CF_UGETPW */

#ifndef	POLLMULT
#define	POLLMULT	1000
#endif

#ifndef	PBUFLEN
#define	PBUFLEN		(4 * MAXPATHLEN)
#endif

#ifndef	VBUFLEN
#define	VBUFLEN		(4 * MAXPATHLEN)
#endif

#ifndef	EBUFLEN
#define	EBUFLEN		(3 * MAXPATHLEN)
#endif

#ifndef	ENVBUFLEN
#define	ENVBUFLEN	2048
#endif

#ifndef	DIGBUFLEN
#define	DIGBUFLEN	40		/* can hold int128_t in decimal */
#endif

#define	DEBUGFNAME	"/tmp/msu.deb"

#ifndef	REQCNAME
#define	REQCNAME	"req"
#endif


/* external subroutines */

extern int	snsd(char *,int,cchar *,uint) ;
extern int	snsds(char *,int,cchar *,cchar *) ;
extern int	sncpy1(char *,int,cchar *) ;
extern int	sncpy2(char *,int,cchar *,cchar *) ;
extern int	sncpy3(char *,int,cchar *,cchar *,cchar *) ;
extern int	mkpath1w(char *,cchar *,int) ;
extern int	mkpath1(char *,cchar *) ;
extern int	mkpath2(char *,cchar *,cchar *) ;
extern int	mkpath3(char *,cchar *,cchar *,cchar *) ;
extern int	mknpath2(char *,int,cchar *,cchar *) ;
extern int	mkfnamesuf1(char *,cchar *,cchar *) ;
extern int	sfdirname(cchar *,int,cchar **) ;
extern int	sfshrink(cchar *,int,cchar **) ;
extern int	matstr(cchar **,cchar *,int) ;
extern int	matostr(cchar **,int,cchar *,int) ;
extern int	cfdeci(cchar *,int,int *) ;
extern int	cfdecui(cchar *,int,uint *) ;
extern int	cfdecti(cchar *,int,int *) ;
extern int	cfdecmfi(cchar *,int,int *) ;
extern int	ctdeci(char *,int,int) ;
extern int	optbool(cchar *,int) ;
extern int	optvalue(cchar *,int) ;
extern int	getnodedomain(char *,char *) ;
extern int	mkdirs(cchar *,mode_t) ;
extern int	vecstr_envadd(vecstr *,cchar *,cchar *,int) ;
extern int	vecstr_envset(vecstr *,cchar *,cchar *,int) ;
extern int	perm(cchar *,uid_t,gid_t,gid_t *,int) ;
extern int	perm(cchar *,uid_t,gid_t,gid_t *,int) ;
extern int	sperm(IDS *,struct ustat *,int) ;
extern int	permsched(cchar **,vecstr *,char *,int,cchar *,int) ;
extern int	securefile(cchar *,uid_t,gid_t) ;
extern int	mkplogid(char *,int,cchar *,int) ;
extern int	mksublogid(char *,int,cchar *,int) ;
extern int	getnprocessors(cchar **,int) ;
extern int	prgetprogpath(cchar *,char *,cchar *,int) ;
extern int	bufprintf(char *,int,cchar *,...) ;
extern int	hasnonwhite(cchar *,int) ;
extern int	isdigitlatin(int) ;
extern int	isFailOpen(int) ;
extern int	isNotPresent(int) ;
extern int	isStrEmpty(cchar *,int) ;

extern int	printhelp(void *,cchar *,cchar *,cchar *) ;
extern int	proginfo_setpiv(PROGINFO *,cchar *,const struct pivars *) ;

#if	CF_CPUSPEED
extern int	cpuspeed(cchar *,cchar *,int) ;
#endif

#if	CF_DEBUGS || CF_DEBUG
extern int	debugopen(cchar *) ;
extern int	debugprintf(cchar *,...) ;
extern int	debugclose() ;
extern int	strlinelen(cchar *,int,int) ;
extern int	debugprinthexblock(cchar *,int,const void *,int) ;
extern int	msudebug_lockprint(PROGINFO *,cchar *) ;
#endif

extern char	*getourenv(cchar **,cchar *) ;
extern char	*strwcpy(char *,cchar *,int) ;
extern char	*strnrpbrk(cchar *,int,cchar *) ;
extern char	*timestr_log(time_t,char *) ;
extern char	*timestr_logz(time_t,char *) ;
extern char	*timestr_elapsed(time_t,char *) ;


/* external variables */

extern char	**environ ;


/* local structures */


/* forward references */

static int	msumain(int,cchar *[],cchar *[],void *) ;

static int	usage(PROGINFO *) ;

static int	procopts(PROGINFO *,KEYOPT *) ;
static int	procdefargs(PROGINFO *) ;
static int	procextras(PROGINFO *) ;
static int	procfindconf(PROGINFO *) ;

static int	procourconf_begin(PROGINFO *) ;
static int	procourconf_end(PROGINFO *) ;

static int	procourdefs(PROGINFO *) ;
static int	proclogid(PROGINFO *) ;

static int	process(PROGINFO *) ;
static int	procregular(PROGINFO *) ;
static int	procbackinfo(PROGINFO *) ;
static int	procback(PROGINFO *) ;
static int	procbacks(PROGINFO *) ;
static int	procbackcheck(PROGINFO *) ;
static int	procbacker(PROGINFO *,cchar *,cchar **) ;
static int	procbackenv(PROGINFO *,SPAWNER *) ;
static int	procmntcheck(PROGINFO *) ;
static int	procdaemon(PROGINFO *) ;
static int	procdaemoncheck(PROGINFO *) ;

static int	procbackdefs(PROGINFO *) ;
static int	procdaemondefs(PROGINFO *) ;
static int	procpidfname(PROGINFO *) ;
static int	procupdate(PROGINFO *) ;
static int	procupspeed(PROGINFO *,MSFILE_ENT *) ;
static int	procfcmd(PROGINFO *) ;
static int	procexecname(PROGINFO *,char *,int) ;

static int	entry_mem(MSFILE_ENT *,int) ;


/* local variables */

static const char	*argopts[] = {
	"ROOT",
	"VERSION",
	"VERBOSE",
	"HELP",
	"LOGFILE",
	"PID",
	"pid",
	"REQ",
	"req",
	"sn",
	"ef",
	"cf",
	"lf",
	"db",
	"msfile",
	"mspoll",
	"speed",
	"speedint",
	"intspeed",
	"intconfig",
	"zerospeed",
	"caf",
	"disable",
	"ra",
	"fg",
	"daemon",
	NULL
} ;

enum argopts {
	argopt_root,
	argopt_version,
	argopt_verbose,
	argopt_help,
	argopt_logfile,
	argopt_pid0,
	argopt_pid1,
	argopt_req0,
	argopt_req1,
	argopt_sn,
	argopt_ef,
	argopt_cf,
	argopt_lf,
	argopt_db,
	argopt_msfile,
	argopt_mspoll,
	argopt_speed,
	argopt_speedint,
	argopt_intspeed,
	argopt_intconfig,
	argopt_zerospeed,
	argopt_caf,
	argopt_disable,
	argopt_ra,
	argopt_fg,
	argopt_daemon,
	argopt_overlast
} ;

static const PIVARS	initvars = {
	VARPROGRAMROOT1,
	VARPROGRAMROOT2,
	VARPROGRAMROOT3,
	PROGRAMROOT,
	VARPRNAME
} ;

static const MAPEX	mapexs[] = {
	{ SR_NOMEM, EX_OSERR },
	{ SR_NOENT, EX_NOUSER },
	{ SR_AGAIN, EX_TEMPFAIL },
	{ SR_ALREADY, EX_TEMPFAIL },
	{ SR_DEADLK, EX_TEMPFAIL },
	{ SR_NOLCK, EX_TEMPFAIL },
	{ SR_TXTBSY, EX_TEMPFAIL },
	{ SR_ACCESS, EX_NOPERM },
	{ SR_NOSPC, EX_TEMPFAIL },
	{ SR_INTR, EX_INTR },
	{ SR_EXIT, EX_TERM },
	{ 0, 0 }
} ;

static const char	*progopts[] = {
	"lockinfo",
	"quiet",
	"intspeed",
	"intconfig",
	"intrun",
	"intpoll",
	"quick",
	"listen",
	"ra",
	"reuse",
	"daemon",
	"pidfile",
	"mntfile",
	"msfile",
	NULL
} ;

enum progopts {
	progopt_lockinfo,
	progopt_quiet,
	progopt_intspeed,
	progopt_intconfig,
	progopt_intrun,
	progopt_intpoll,
	progopt_quick,
	progopt_listen,
	progopt_ra,
	progopt_reuse,
	progopt_daemon,
	progopt_pidfile,
	progopt_mntfile,
	progopt_msfile,
	progopt_overlast
} ;

static const char	*sched1[] = {
	"%p/%e/%n/%n.%f",
	"%p/%e/%n/%f",
	"%p/%e/%n.%f",
	"%p/%n.%f",
	NULL
} ;

static const char	*cmds[] = {
	"exit",
	"mark",
	"report",
	NULL
} ;

enum cmds {
	cmd_exit,
	cmd_mark,
	cmd_report,
	cmd_overlast
} ;

/* thses are for the spawned child */
static const int	sigignores[] = {
	SIGHUP,
	SIGPIPE,
	SIGPOLL,
	0
} ;


/* exported subroutines */


int b_msu(int argc,cchar *argv[],void *contextp)
{
	int		rs ;
	int		rs1 ;
	int		ex = EX_OK ;

	if ((rs = lib_kshbegin(contextp,NULL)) >= 0) {
	    cchar	**envv = (cchar **) environ ;
	    ex = msumain(argc,argv,envv,contextp) ;
	    rs1 = lib_kshend() ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (ksh) */

	if ((rs < 0) && (ex == EX_OK)) ex = EX_DATAERR ;

	return ex ;
}
/* end subroutine (b_msu) */


int p_msu(int argc,cchar *argv[],cchar *envv[],void *contextp)
{
	return msumain(argc,argv,envv,contextp) ;
}
/* end subroutine (p_msu) */


/* ARGSUSED */
static int msumain(int argc,cchar *argv[],cchar *envv[],void *contextp)
{
	PROGINFO	pi, *pip = &pi ;
	LOCINFO		li, *lip = &li ;
	BITS		pargs ;
	KEYOPT		akopts ;
	SHIO		errfile ;

#if	(CF_DEBUGS || CF_DEBUG) && CF_DEBUGMALL
	uint		mo_start = 0 ;
#endif

	int		argr, argl, aol, akl, avl, kwi ;
	int		ai, ai_max, ai_pos ;
	int		rs, rs1 ;
	int		v ;
	int		vl ;
	int		ex = EX_INFO ;
	int		f_optminus, f_optplus, f_optequal ;
	int		f_version = FALSE ;
	int		f_usage = FALSE ;
	int		f_help = FALSE ;

	cchar		*argp, *aop, *akp, *avp ;
	cchar		*argval = NULL ;
	cchar		*pr = NULL ;
	cchar		*sn = NULL ;
	cchar		*efname = NULL ;
	cchar		*vp ;
	cchar		*cp ;


#if	CF_DEBUGS || CF_DEBUG
	if ((cp = getourenv(envv,VARDEBUGFNAME)) != NULL) {
	    rs = debugopen(cp) ;
	    debugprintf("msumain: starting DFD=%d\n",rs) ;
	}
#endif /* CF_DEBUGS */

#if	(CF_DEBUGS || CF_DEBUG) && CF_DEBUGMALL
	uc_mallset(1) ;
	uc_mallout(&mo_start) ;
#endif

	rs = proginfo_start(pip,envv,argv[0],VERSION) ;
	if (rs < 0) {
	    ex = EX_OSERR ;
	    goto badprogstart ;
	}

	if ((cp = getourenv(envv,VARBANNER)) == NULL) cp = BANNER ;
	proginfo_setbanner(pip,cp) ;

/* initialize */

	pip->verboselevel = 1 ;
	pip->daytime = time(NULL) ;
	pip->pid = ugetpid() ;
	pip->uid = getuid() ;
	pip->euid = geteuid() ;
	pip->gid = getgid() ;
	pip->egid = getegid() ;

	pip->f.logprog = OPT_LOGPROG ;

	pip->lip = lip ;
	rs = locinfo_start(lip,pip) ;
	if (rs < 0) {
	    ex = EX_OSERR ;
	    goto badlocstart ;
	}

/* start parsing the arguments */

	if (rs >= 0) rs = bits_start(&pargs,1) ;
	if (rs < 0) goto badpargs ;

	rs = keyopt_start(&akopts) ;
	pip->open.akopts = (rs >= 0) ;

	ai_max = 0 ;
	ai_pos = 0 ;
	argr = argc ;
	for (ai = 0 ; (ai < argc) && (argv[ai] != NULL) ; ai += 1) {
	    if (rs < 0) break ;
	    argr -= 1 ;
	    if (ai == 0) continue ;

	    argp = argv[ai] ;
	    argl = strlen(argp) ;

	    f_optminus = (*argp == '-') ;
	    f_optplus = (*argp == '+') ;
	    if ((argl > 1) && (f_optminus || f_optplus)) {
	        const int	ach = MKCHAR(argp[1]) ;

	        if (isdigitlatin(ach)) {

	            argval = (argp + 1) ;

	        } else if (ach == '-') {

	            ai_pos = ai ;
	            break ;

	        } else {

	            aop = argp + 1 ;
	            akp = aop ;
	            aol = argl - 1 ;
	            f_optequal = FALSE ;
	            if ((avp = strchr(aop,'=')) != NULL) {
	                f_optequal = TRUE ;
	                akl = avp - aop ;
	                avp += 1 ;
	                avl = aop + argl - 1 - avp ;
	                aol = akl ;
	            } else {
	                avp = NULL ;
	                avl = 0 ;
	                akl = aol ;
	            }

	            if ((kwi = matostr(argopts,2,akp,akl)) >= 0) {

	                vp = NULL ;
	                vl = 0 ;
	                switch (kwi) {

/* version */
	                case argopt_version:
	                    f_version = TRUE ;
	                    if (f_optequal)
	                        rs = SR_INVALID ;
	                    break ;

/* verbose mode */
	                case argopt_verbose:
	                    pip->verboselevel = 2 ;
	                    if (f_optequal) {
	                        f_optequal = FALSE ;
	                        if (avl) {
	                            rs = optvalue(avp,avl) ;
	                            pip->verboselevel = rs ;
	                        }
	                    }
	                    break ;

/* program root */
	                case argopt_root:
	                    if (f_optequal) {
	                        f_optequal = FALSE ;
	                        if (avl)
	                            pr = avp ;
	                    } else {
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl)
	                                pr = argp ;
	                        } else
	                            rs = SR_INVALID ;
	                    }
	                    break ;

	                case argopt_logfile:
	                case argopt_lf:
	                    pip->have.lfname = TRUE ;
	                    if (f_optequal) {
	                        f_optequal = FALSE ;
	                        if (avl) {
	                            pip->final.lfname = TRUE ;
	                            pip->have.lfname = TRUE ;
	                            pip->lfname = avp ;
	                        }
	                    }
	                    break ;

	                case argopt_help:
	                    f_help = TRUE ;
	                    break ;

/* PID file */
	                case argopt_pid0:
	                case argopt_pid1:
	                    cp = NULL ;
	                    if (f_optequal) {
	                        f_optequal = FALSE ;
	                        if (avl)
	                            cp = avp ;
	                    } else {
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl)
	                                cp = argp ;
	                        } else
	                            rs = SR_INVALID ;
	                    }
	                    if ((rs >= 0) && (cp != NULL)) {
	                        pip->final.pidfname = TRUE ;
	                        pip->have.pidfname = TRUE ;
	                        pip->pidfname = cp ;
	                    }
	                    break ;

/* REQ file */
	                case argopt_req0:
	                case argopt_req1:
	                    cp = NULL ;
	                    if (f_optequal) {
	                        f_optequal = FALSE ;
	                        if (avl)
	                            cp = avp ;
	                    } else {
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl)
	                                cp = argp ;
	                        } else
	                            rs = SR_INVALID ;
	                    }
	                    if ((rs >= 0) && (cp != NULL)) {
	                        lip->final.reqfname = TRUE ;
	                        lip->have.reqfname = TRUE ;
	                        lip->reqfname = cp ;
	                    }
	                    break ;

/* program search-name */
	                case argopt_sn:
	                    if (f_optequal) {
	                        f_optequal = FALSE ;
	                        if (avl)
	                            sn = avp ;
	                    } else {
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl)
	                                sn = argp ;
	                        } else
	                            rs = SR_INVALID ;
	                    }
	                    break ;

/* error file name */
	                case argopt_ef:
	                    if (f_optequal) {
	                        f_optequal = FALSE ;
	                        if (avl)
	                            efname = avp ;
	                    } else {
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl)
	                                efname = argp ;
	                        } else
	                            rs = SR_INVALID ;
	                    }
	                    break ;

/* conf-filename */
	                case argopt_cf:
	                    if (f_optequal) {
	                        f_optequal = FALSE ;
	                        if (avl)
	                            pip->cfname = avp ;
	                    } else {
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl)
	                                pip->cfname = argp ;
	                        } else
	                            rs = SR_INVALID ;
	                    }
	                    break ;

/* MS file name */
	                case argopt_db:
	                case argopt_msfile:
	                    if (f_optequal) {
	                        f_optequal = FALSE ;
	                        if (avl) {
	                            lip->final.msfname = TRUE ;
	                            lip->have.msfname = TRUE ;
	                            lip->msfname = avp ;
	                        }
	                    } else {
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl) {
	                                lip->final.msfname = TRUE ;
	                                lip->have.msfname = TRUE ;
	                                lip->msfname = argp ;
	                            }
	                        } else
	                            rs = SR_INVALID ;
	                    }
	                    break ;

/* MS poll interval */
	                case argopt_mspoll:
	                    if (f_optequal) {
	                        f_optequal = FALSE ;
	                        if (avl) {
	                            vp = avp ;
	                            vl = avl ;
	                        }
	                    } else {
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl) {
	                                vp = argp ;
	                                vl = argl ;
	                            }
	                        } else
	                            rs = SR_INVALID ;
	                    }
	                    if ((rs >= 0) && (vp != NULL) && vl) {
	                        pip->final.intpoll = TRUE ;
	                        rs = cfdecti(vp,vl,&v) ;
	                        pip->intpoll = v ;
	                    }
	                    break ;

	                case argopt_speed:
	                    lip->f.speedname = TRUE ;
	                    if (f_optequal) {
	                        f_optequal = FALSE ;
	                        if (avl)
	                            lip->speedname = avp ;
	                    }
	                    break ;

	                case argopt_speedint:
	                case argopt_intspeed:
	                    if (f_optequal) {
	                        f_optequal = FALSE ;
	                        if (avl) {
	                            vp = avp ;
	                            vl = avl ;
	                        }
	                    } else {
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl) {
	                                vp = argp ;
	                                vl = argl ;
	                            }
	                        } else
	                            rs = SR_INVALID ;
	                    }
	                    if ((rs >= 0) && (vp != NULL) && vl) {
	                        lip->final.intspeed = TRUE ;
	                        rs = cfdecti(vp,vl,&v) ;
	                        lip->intspeed = v ;
	                    }
	                    break ;

	                case argopt_intconfig:
	                    if (f_optequal) {
	                        f_optequal = FALSE ;
	                        if (avl) {
	                            vp = avp ;
	                            vl = avl ;
	                        }
	                    } else {
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl) {
	                                vp = argp ;
	                                vl = argl ;
	                            }
	                        } else
	                            rs = SR_INVALID ;
	                    }
	                    if ((rs >= 0) && (vp != NULL) && vl) {
	                        lip->final.intconfig = TRUE ;
	                        rs = cfdecti(vp,vl,&v) ;
	                        lip->intconfig = v ;
	                    }
	                    break ;

	                case argopt_zerospeed:
	                    lip->f.zerospeed = TRUE ;
	                    break ;

/* disable interval */
	                case argopt_disable:
	                    pip->f.disable = TRUE ;
	                    if (f_optequal) {
	                        f_optequal = FALSE ;
	                        if (avl) {
	                            pip->final.intdis = TRUE ;
	                            rs = cfdecti(avp,avl,&v) ;
	                            pip->intdis = v ;
	                        }
	                    }
	                    break ;

/* reuse address */
	                case argopt_ra:
	                    lip->have.reuseaddr = TRUE ;
	                    lip->final.reuseaddr = TRUE ;
	                    lip->f.reuseaddr = TRUE ;
	                    if (f_optequal) {
	                        f_optequal = FALSE ;
	                        if (avl) {
	                            rs = optbool(avp,avl) ;
	                            lip->f.reuseaddr = (rs > 0) ;
	                        }
	                    }
	                    break ;

/* run in the foreground */
	                case argopt_fg:
	                    lip->have.fg = TRUE ;
	                    lip->final.fg = TRUE ;
	                    lip->f.fg = TRUE ;
	                    if (f_optequal) {
	                        f_optequal = FALSE ;
	                        if (avl) {
	                            rs = optbool(avp,avl) ;
	                            lip->f.fg = (rs > 0) ;
	                        }
	                    }
	                    break ;

/* daemon mode */
	                case argopt_daemon:
	                    pip->have.daemon = TRUE ;
	                    pip->final.daemon = TRUE ;
	                    pip->f.daemon = TRUE ;
	                    if (f_optequal) {
	                        f_optequal = FALSE ;
	                        if (avl) {
				    rs = cfdecti(avp,avl,&v) ;
				    pip->intrun = v ;
	                        }
	                    }
	                    break ;

/* handle all keyword defaults */
	                default:
	                    rs = SR_INVALID ;
	                    break ;

	                } /* end switch */

	            } else {

	                while (akl--) {
	                    const int kc = MKCHAR(*akp) ;

	                    switch (kc) {

	                    case 'C':
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl) {
	                                pip->have.cfname = TRUE ;
	                                pip->final.cfname = TRUE ;
	                                pip->cfname = argp ;
	                            }
	                        } else
	                            rs = SR_INVALID ;
	                        break ;

/* debug */
	                    case 'D':
	                        pip->debuglevel = 1 ;
	                        if (f_optequal) {
	                            f_optequal = FALSE ;
	                            if (avl) {
	                                rs = optvalue(avp,avl) ;
	                                pip->debuglevel = rs ;
	                            }
	                        }
	                        break ;

/* pid-file */
	                    case 'P':
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl) {
	                                pip->final.pidfname = TRUE ;
	                                pip->have.pidfname = TRUE ;
	                                pip->pidfname = argp ;
	                            }
	                        } else
	                            rs = SR_INVALID ;
	                        break ;

/* program-root */
	                    case 'R':
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl)
	                                pr = argp ;
	                        } else
	                            rs = SR_INVALID ;
	                        break ;

/* version */
	                    case 'V':
	                        f_version = TRUE ;
	                        break ;

	                    case 'Q':
	                        pip->f.quiet = TRUE ;
	                        pip->have.quiet = TRUE ;
	                        pip->final.quiet = TRUE ;
	                        if (f_optequal) {
	                            f_optequal = FALSE ;
	                            if (avl) {
	                                rs = optbool(avp,avl) ;
	                                pip->f.quiet = (rs > 0) ;
	                            }
	                        }
	                        break ;

/* number-updates */
	                    case 'c':
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl) {
	                                rs = cfdecmfi(argp,argl,&v) ;
	                                lip->nu = v ;
	                            }
	                        } else
	                            rs = SR_INVALID ;
	                        break ;

/* daemon mode */
	                    case 'd':
	                        pip->final.background = TRUE ;
	                        pip->f.background = TRUE ;
	                        if (f_optequal) {
	                            f_optequal = FALSE ;
	                            if (avl) {
	                                int	ch = MKCHAR(*avp) ;
	                                if (isdigitlatin(ch)) {
	                                    pip->final.intrun = TRUE ;
	                                    pip->have.intrun = TRUE ;
	                                    rs = cfdecti(avp,avl,&v) ;
	                                    pip->intrun = v ;
	                                } else if (tolc(ch) == 'i') {
	                                    pip->intrun = 0 ;
	                                } else
	                                    rs = SR_INVALID ;
	                            }
	                        }
	                        break ;

/* MS-node */
	                    case 'n':
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl)
	                                lip->msnode = argp ;
	                        } else
	                            rs = SR_INVALID ;
	                        break ;

/* options */
	                    case 'o':
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl) {
					KEYOPT	*kop = &akopts ;
	                                rs = keyopt_loads(kop,argp,argl) ;
				    }
	                        } else
	                            rs = SR_INVALID ;
	                        break ;

	                    case 'q':
	                        pip->verboselevel = 0 ;
	                        if (f_optequal) {
	                            f_optequal = FALSE ;
	                            if (avl) {
	                                rs = optbool(avp,avl) ;
	                                pip->verboselevel = (rs > 0) ;
	                            }
	                        }
	                        break ;

/* reuse listener port */
	                    case 'r':
	                        lip->have.reuseaddr = TRUE ;
	                        lip->final.reuseaddr = TRUE ;
	                        lip->f.reuseaddr = TRUE ;
	                        if (f_optequal) {
	                            f_optequal = FALSE ;
	                            if (avl) {
	                                rs = optbool(avp,avl) ;
	                                lip->f.reuseaddr = (rs > 0) ;
	                            }
	                        }
	                        break ;

/* verbose mode */
	                    case 'v':
	                        pip->verboselevel = 2 ;
	                        if (f_optequal) {
	                            f_optequal = FALSE ;
	                            if (avl) {
	                                rs = optvalue(avp,avl) ;
	                                pip->verboselevel = rs ;
	                            }
	                        }
	                        break ;

	                    case '?':
	                        f_usage = TRUE ;
	                        break ;

	                    default:
	                        rs = SR_INVALID ;
	                        break ;

	                    } /* end switch */
	                    akp += 1 ;

	                    if (rs < 0) break ;
	                } /* end while */

	            } /* end if (individual option key letters) */

	        } /* end if (digits as argument or not) */

	    } else {

	        rs = bits_set(&pargs,ai) ;
	        ai_max = ai ;

	    } /* end if (key letter/word or positional) */

	    ai_pos = ai ;

	} /* end while (all command line argument processing) */

	if (efname == NULL) efname = getourenv(envv,VAREFNAME) ;
	if (efname == NULL) efname = STDFNERR ;
	if ((rs1 = shio_open(&errfile,efname,"wca",0666)) >= 0) {
	    pip->efp = &errfile ;
	    pip->open.errfile = TRUE ;
	    shio_control(&errfile,SHIO_CSETBUFLINE,TRUE) ;
	} else if (! isFailOpen(rs1)) {
	    if (rs >= 0) rs = rs1 ;
	}

	if (rs < 0) goto badarg ;

	if (pip->debuglevel == 0) {
	    if ((cp = getourenv(envv,VARDEBUGLEVEL)) != NULL) {
	        if (hasnonwhite(cp,-1)) {
		    rs = optvalue(cp,-1) ;
		    pip->debuglevel = rs ;
	        }
	    }
	} /* end if */

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    debugprintf("msumain: debuglevel=%u\n",pip->debuglevel) ;
#endif

	if (f_version) {
	    shio_printf(pip->efp,"%s: version %s\n",pip->progname,VERSION) ;
	}

/* get the program root */

	if (rs >= 0) {
	    if ((rs = proginfo_setpiv(pip,pr,&initvars)) >= 0) {
	        rs = proginfo_setsearchname(pip,VARSEARCHNAME,sn) ;
	    }
	}

	if (rs < 0) {
	    ex = EX_OSERR ;
	    goto retearly ;
	}

	if (pip->debuglevel > 0) {
	    shio_printf(pip->efp,"%s: pr=%s\n", pip->progname,pip->pr) ;
	    shio_printf(pip->efp,"%s: sn=%s\n", pip->progname,pip->searchname) ;
	} /* end if */

	if (f_usage)
	    usage(pip) ;

/* help file */

	if (f_help) {
#if	CF_SFIO
	    printhelp(sfstdout,pip->pr,pip->searchname,HELPFNAME) ;
#else
	    printhelp(NULL,pip->pr,pip->searchname,HELPFNAME) ;
#endif
	} /* end if */

	if (f_version || f_help || f_usage)
	    goto retearly ;


	ex = EX_OK ;

/* continue with prepatory initialization */

	if ((rs >= 0) && ((ai_max < 0) || (ai_pos < 0))) rs = SR_BUGCHECK ;

	if ((rs >= 0) && (pip->intpoll == 0) && (argval != NULL)) {
	    rs = cfdecti(argval,-1,&v) ;
	    pip->intpoll = v ;
	}

#if	CF_DEBUGS
	debugprintf("msumain: pid=%d\n",pip->pid) ;
#endif

	rs1 = securefile(pip->pr,pip->euid,pip->egid) ;
	lip->f.sec_root = (rs1 > 0) ;

/* process program options */

	if (rs >= 0) {
	    rs = procopts(pip,&akopts) ;
	}

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    debugprintf("main: procopts() rs=%d\n",rs) ;
#endif

/* defaults */

	if (pip->cfname == NULL) pip->cfname = getourenv(envv,VARCFNAME) ;
	if (pip->cfname == NULL) pip->cfname = getourenv(envv,VARCONFIG) ;

	if (pip->lfname == NULL) pip->lfname = getourenv(envv,VARLFNAME) ;

	if (lip->msnode == NULL) lip->msnode = getourenv(envv,VARMSNODE) ;

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    debugprintf("main: mid2 rs=%d\n",rs) ;
#endif

/* go */

	if (rs >= 0) {
	    if ((rs = procdefargs(pip)) >= 0) {
	        if ((rs = procextras(pip)) >= 0) {
	                if ((rs = procourconf_begin(pip)) >= 0) {
	                    if ((rs = procourdefs(pip)) >= 0) {
	                        if ((rs = logbegin(pip)) >= 0) {
	                            {
	                                rs = process(pip) ;
	                            }
	                            rs1 = logend(pip) ;
	                            if (rs >= 0) rs = rs1 ;
	                        } /* end if (log) */
	                    } /* end if (procourdefs) */
	                    rs1 = procourconf_end(pip) ;
	                    if (rs >= 0) rs = rs1 ;
	                } /* end if (procourconf) */
	        } /* end if (procextras) */
	    } /* end if (procdefargs) */
	} else if (ex == EX_OK) {
	    cchar	*pn = pip->progname ;
	    ex = EX_USAGE ;
	    shio_printf(pip->efp,"%s: usage (%d)\n",pn,rs) ;
	    usage(pip) ;
	}

/* done */
	if ((rs < 0) && (ex == EX_OK)) {
	    cchar	*pn = pip->progname ;
	    cchar	*fmt ;
	    char	timebuf[TIMEBUFLEN+1] ;
	    timestr_logz(pip->daytime,timebuf) ;
	    switch (rs) {
	    case SR_ALREADY:
	    case SR_AGAIN:
	        ex = EX_MUTEX ;
	        if ((! pip->f.quiet) && (pip->efp != NULL)) {
		    fmt = "%s: existing lock (%d)\n" ;
	            shio_printf(pip->efp,fmt,pn,rs) ;
	        }
	        break ;
	    default:
	        ex = mapex(mapexs,rs) ;
	        if ((! pip->f.quiet) && (pip->efp != NULL)) {
		    fmt = "%s: could not perform update (%d)\n" ;
	            shio_printf(pip->efp,fmt,pn,rs) ;
	        }
	        break ;
	    } /* end switch */
	} else if ((rs = lib_sigterm()) < 0) {
	    ex = EX_TERM ;
	} else if ((rs = lib_sigintr()) < 0) {
	    ex = EX_INTR ;
	}

/* early return thing */
retearly:
	if (pip->debuglevel > 0) {
	    cchar	*pn = pip->progname ;
	    cchar	*fmt ;
	    if (pip->f.background || pip->f.daemon) {
	        cchar	*w = ((pip->f.daemon) ? "child" : "parent") ;
		fmt = "%s: (%s) exiting ex=%u (%d)\n" ;
	        shio_printf(pip->efp,fmt,pn,w,ex,rs) ;
	    } else {
		fmt = "%s: exiting ex=%u (%d)\n" ;
	        shio_printf(pip->efp,fmt,ex,rs) ;
	    }
	} /* end if */

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    debugprintf("msumain: exiting ex=%u (%d)\n",ex,rs) ;
#endif

	if ((pip->efp != NULL) && pip->open.errfile) {
	    pip->open.errfile = FALSE ;
	    shio_close(pip->efp) ;
	    pip->efp = NULL ;
	}

	if (pip->open.akopts) {
	    pip->open.akopts = FALSE ;
	    keyopt_finish(&akopts) ;
	}

	bits_finish(&pargs) ;

badpargs:
	locinfo_finish(lip) ;

badlocstart:
	proginfo_finish(pip) ;

badprogstart:

#if	(CF_DEBUGS || CF_DEBUG) && CF_DEBUGMALL
	{
	    uint	mi[12] ;
	    uint	mo ;
	    uint	mdiff ;
	    uc_mallout(&mo) ;
	    mdiff = (mo-mo_start) ;
	    debugprintf("msumain: final mallout=%u\n",mdiff) ;
	    if (mdiff > 0) {
	        UCMALLREG_CUR	cur ;
	        UCMALLREG_REG	reg ;
	        const int	size = (10*sizeof(uint)) ;
	        cchar		*ids = "main" ;
	        uc_mallinfo(mi,size) ;
	        debugprintf("main: MIoutnum=%u\n",mi[ucmallreg_outnum]) ;
	        debugprintf("main: MIoutnummax=%u\n",mi[ucmallreg_outnummax]) ;
	        debugprintf("main: MIoutsize=%u\n",mi[ucmallreg_outsize]) ;
	        debugprintf("main: MIoutsizemax=%u\n",
	            mi[ucmallreg_outsizemax]) ;
	        debugprintf("main: MIused=%u\n",mi[ucmallreg_used]) ;
	        debugprintf("main: MIusedmax=%u\n",mi[ucmallreg_usedmax]) ;
	        debugprintf("main: MIunder=%u\n",mi[ucmallreg_under]) ;
	        debugprintf("main: MIover=%u\n",mi[ucmallreg_over]) ;
	        debugprintf("main: MInotalloc=%u\n",mi[ucmallreg_notalloc]) ;
	        debugprintf("main: MInotfree=%u\n",mi[ucmallreg_notfree]) ;
	        ucmallreg_curbegin(&cur) ;
	        while (ucmallreg_enum(&cur,&reg) >= 0) {
	            debugprintf("main: MIreg.addr=%p\n",reg.addr) ;
	            debugprintf("main: MIreg.size=%u\n",reg.size) ;
	            debugprinthexblock(ids,80,reg.addr,reg.size) ;
	        } /* end while */
	        ucmallreg_curend(&cur) ;
	    } /* end if (positive) */
	    uc_mallset(0) ;
	}
#endif /* CF_DEBUGMALL */

#if	(CF_DEBUGS || CF_DEBUG)
	debugclose() ;
#endif

	return ex ;

/* the bad things */
badarg:
	ex = EX_USAGE ;
	shio_printf(pip->efp,"%s: invalid argument specified (%d)\n",
	    pip->progname,rs) ;
	usage(pip) ;
	goto retearly ;

}
/* end subroutine (msumain) */


/* print out lock-check information */
int msumainlockprint(PROGINFO *pip,cchar *lfname,LFM_CHECK *lcp)
{
	int		rs = SR_OK ;
	cchar		*np ;
	char		timebuf[TIMEBUFLEN + 1] ;

	switch (lcp->stat) {
	case SR_AGAIN:
	    np = "busy" ;
	    break ;
	case SR_LOCKLOST:
	    np = "lost" ;
	    break ;
	default:
	    np = "unknown" ;
	    break ;
	} /* end switch */

	if (pip->open.logprog) {
	    loglock(pip,lcp,lfname,np) ;
	}

	if ((pip->debuglevel > 0) && (pip->efp != NULL)) {
	    cchar	*pn = pip->progname ;
	    cchar	*fmt ;

	    fmt = "%s: %s lock %s\n" ;
	    timestr_logz(pip->daytime,timebuf) ;
	    shio_printf(pip->efp,fmt,pn,timebuf,np) ;
	        
	    fmt = "%s: other_pid=%d\n" ;
	    shio_printf(pip->efp,fmt,pn,lcp->pid) ;

	    if (lcp->nodename != NULL) {
		fmt = "%s: other_node=%s\n" ;
	        shio_printf(pip->efp,fmt,pn,lcp->nodename) ;
	    }

	    if (lcp->username != NULL) {
		fmt = "%s: other_user=%s\n" ;
	        rs = shio_printf(pip->efp,fmt,lcp->username) ;
	    }

	    if (lcp->banner != NULL) {
		fmt = "%s: other_banner=>%s<\n" ;
	        shio_printf(pip->efp,fmt,lcp->banner) ;
	    }

	} /* end if (standard-error) */

	return rs ;
}
/* end subroutine (msumainlockprint) */


/* local subroutines */


static int usage(PROGINFO *pip)
{
	int		rs = SR_OK ;
	int		wlen = 0 ;
	cchar		*pn = pip->progname ;
	cchar		*fmt ;

	fmt = "%s: USAGE> %s [-speed[=<name>]] [-db <dbfile>]\n" ;
	if (rs >= 0) rs = shio_printf(pip->efp,fmt,pn,pn) ;
	wlen += rs ;

	fmt = "%s:  [-d[=<intrun>]] [-mspoll <int>] [-zerospeed]\n" ;
	if (rs >= 0) rs = shio_printf(pip->efp,fmt,pn) ;
	wlen += rs ;

	fmt = "%s:  [-Q] [-D] [-v[=<n>]] [-HELP] [-V]\n" ;
	if (rs >= 0) rs = shio_printf(pip->efp,fmt,pn) ;
	wlen += rs ;

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (usage) */


static int procopts(PROGINFO *pip,KEYOPT *kop)
{
	LOCINFO		*lip = pip->lip ;
	int		rs = SR_OK ;
	int		c = 0 ;
	cchar		*cp ;

	if ((cp = getourenv(pip->envv,VAROPTS)) != NULL) {
	    rs = keyopt_loads(kop,cp,-1) ;
	}

	if (rs >= 0) {
	    KEYOPT_CUR	kcur ;
	    if ((rs = keyopt_curbegin(kop,&kcur)) >= 0) {
	        int	oi ;
	        int	kl, vl ;
	        cchar	*kp, *vp ;

	        while ((kl = keyopt_enumkeys(kop,&kcur,&kp)) >= 0) {

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	    debugprintf("main/procopts: k=%t\n",kp,kl) ;
#endif

	            if ((oi = matostr(progopts,2,kp,kl)) >= 0) {
	                int	v = 0 ;

	                vl = keyopt_enumvalues(kop,kp,NULL,&vp) ;

#if	CF_DEBUG
			if (DEBUGLEVEL(3) && (vl > 0)) {
	    		debugprintf("main/procopts: v=%t\n",vp,vl) ;
			}
#endif

	                switch (oi) {
	                case progopt_quiet:
	                    if (! pip->final.quiet) {
	                        pip->final.quiet = TRUE ;
	                        pip->have.quiet = TRUE ;
	                        pip->f.quiet = TRUE ;
	                        if (vl > 0) {
	                            rs = optbool(vp,vl) ;
	                            pip->f.quiet = (rs > 0) ;
	                        }
	                    }
	                    break ;
	                case progopt_intspeed:
	                    if (! lip->final.intspeed) {
	                        lip->final.intspeed = TRUE ;
	                        lip->have.intspeed = TRUE ;
	                        if (vl > 0) {
	                            rs = cfdecti(vp,vl,&v) ;
	                            lip->intspeed = v ;
	                        }
	                    }
	                    break ;
	                case progopt_intconfig:
	                    if (! lip->final.intconfig) {
	                        lip->final.intconfig = TRUE ;
	                        lip->have.intconfig = TRUE ;
	                        if (vl > 0) {
	                            rs = cfdecti(vp,vl,&v) ;
	                            lip->intconfig = v ;
	                        }
	                    }
	                    break ;
	                case progopt_intrun:
	                    if (! pip->final.intrun) {
	                        pip->final.intrun = TRUE ;
	                        pip->have.intrun = TRUE ;
	                        if (vl > 0) {
	                            rs = cfdecti(vp,vl,&v) ;
	                            pip->intrun = v ;
	                        }
	                    }
	                    break ;
	                case progopt_intpoll:
	                    if (! pip->final.intpoll) {
	                        pip->final.intpoll = TRUE ;
	                        pip->have.intpoll = TRUE ;
	                        if (vl > 0) {
	                            rs = cfdecti(vp,vl,&v) ;
	                            pip->intpoll = v ;
	                        }
	                    }
	                    break ;
	                case progopt_lockinfo:
	                    if (! lip->final.lockinfo) {
	                        lip->final.lockinfo = TRUE ;
	                        lip->have.lockinfo = TRUE ;
	                        lip->f.lockinfo = TRUE ;
	                        if (vl > 0) {
	                            rs = optbool(vp,vl) ;
	                            lip->f.lockinfo = (rs > 0) ;
	                        }
	                    }
	                    break ;
	                case progopt_quick:
	                    if (! lip->final.quick) {
	                        lip->final.quick = TRUE ;
	                        lip->have.quick = TRUE ;
	                        lip->f.quick = TRUE ;
	                        if (vl > 0) {
	                            rs = optbool(vp,vl) ;
	                            lip->f.quick = (rs > 0) ;
	                        }
	                    }
	                    break ;
	                case progopt_listen:
	                    if (! lip->final.listen) {
	                        lip->final.listen = TRUE ;
	                        lip->have.listen = TRUE ;
	                        lip->f.listen = TRUE ;
	                        if (vl > 0) {
	                            rs = optbool(vp,vl) ;
	                            lip->f.listen = (rs > 0) ;
	                        }
	                    } /* end if */
	                    break ;
	                case progopt_ra:
	                case progopt_reuse:
	                    if (! lip->final.reuseaddr) {
	                        lip->final.reuseaddr = TRUE ;
	                        lip->have.reuseaddr = TRUE ;
	                        lip->f.reuseaddr = TRUE ;
	                        if (vl > 0) {
	                            rs = optbool(vp,vl) ;
	                            lip->f.reuseaddr = (rs > 0) ;
	                        }
	                    } /* end if */
	                    break ;
	                case progopt_daemon:
	                    if (! pip->final.daemon) {
	                        pip->final.daemon = TRUE ;
	                        pip->have.daemon = TRUE ;
	                        pip->f.daemon = TRUE ;
	                        if (vl > 0) {
	                            rs = cfdecti(vp,vl,&v) ;
	                            pip->intrun = v ;
				    if (v == 0) {
					pip->f.daemon = FALSE ;
				    }
	                        }
	                    } /* end if */
	                    break ;
	                case progopt_pidfile:
	                    if (! pip->final.pidfname) {
	                        if (vl > 0) {
	                            cchar **vpp = &pip->pidfname ;
	                            pip->have.pidfname = TRUE ;
	                            pip->final.pidfname = TRUE ;
	                            rs = proginfo_setentry(pip,vpp,vp,vl) ;
	                        }
	                    }
	                    break ;
	                case progopt_mntfile:
	                    if (! lip->final.mntfname) {
	                        if (vl > 0) {
	                            cchar **vpp = &lip->mntfname ;
	                            lip->have.mntfname = TRUE ;
	                            lip->final.mntfname = TRUE ;
	                            rs = locinfo_setentry(lip,vpp,vp,vl) ;
	                        }
	                    }
	                    break ;
	                case progopt_msfile:
	                    if (! lip->final.msfname) {
	                        if (vl > 0) {
	                            cchar **vpp = &lip->msfname ;
	                            lip->have.msfname = TRUE ;
	                            lip->final.msfname = TRUE ;
	                            rs = locinfo_setentry(lip,vpp,vp,vl) ;
	                        }
	                    }
	                    break ;
	                } /* end switch */

			c += 1 ;
	            } else
			rs = SR_INVALID ;

	            if (rs < 0) break ;
	        } /* end while (looping through key options) */

	        keyopt_curend(kop,&kcur) ;
	    } /* end if (keyopt-cur) */
	} /* end if (ok) */

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	    debugprintf("main/procopts: ret rs=%d\n",rs) ;
#endif

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (procopts) */


static int procextras(PROGINFO *pip)
{
	int		rs = SR_OK ;

	if ((rs >= 0) && (pip->username == NULL)) {
	    const int	ulen = USERNAMELEN ;
	    char	ubuf[USERNAMELEN + 1] ;
	    if ((rs = getusername(ubuf,ulen,-1)) >= 0) {
	        cchar	**vpp = &pip->username ;
	        rs = proginfo_setentry(pip,vpp,ubuf,rs) ;
	    }
	} /* end if (username) */

	if ((rs >= 0) && 
	    ((pip->nodename == NULL) || (pip->domainname == NULL))) {
	    char	nbuf[NODENAMELEN + 1] ;
	    char	dbuf[MAXHOSTNAMELEN + 1] ;
	    if ((rs = getnodedomain(nbuf,dbuf)) >= 0) {
	        cchar	**vpp = &pip->nodename ;
	        if ((rs = proginfo_setentry(pip,vpp,nbuf,-1)) >= 0) {
	            cchar	**vpp = &pip->domainname ;
	            rs = proginfo_setentry(pip,vpp,dbuf,-1) ;
	        }
	    } /* end if (getnodedomain) */
	} /* end if (nodename-domainname) */

	pip->name = getourenv(pip->envv,VARNAME) ;

	return rs ;
}
/* end subroutine (procextras) */


static int procdefargs(PROGINFO *pip)
{
	int		rs = SR_OK ;
	cchar		**envv = pip->envv ;
	cchar		*cp ;

	if (pip->tmpdname == NULL) pip->tmpdname = getourenv(envv,VARTMPDNAME) ;
	if (pip->tmpdname == NULL) pip->tmpdname = TMPDNAME ;

	if ((rs >= 0) && (! pip->final.quiet)) {
	    if ((cp = getourenv(envv,VARQUIET)) != NULL) {
	        rs = optbool(cp,-1) ;
	        pip->final.quiet = TRUE ;
	        pip->have.quiet = TRUE ;
	        pip->f.quiet = (rs > 0) ;
	    }
	}

	return rs ;
}
/* end subroutine (procdefargs) */


static int procfindconf(PROGINFO *pip)
{
	int		rs = SR_OK ;
	int		rl = 0 ;

	if (pip->cfname == NULL) {
	    vecstr	sv ;
	    int		rs1 ;
	    cchar	*cfn = CONFIGFNAME ;
	    if ((rs = vecstr_start(&sv,4,0)) >= 0) {
	        const int	tlen = MAXPATHLEN ;
	        int		i ;
	        int		vl ;
	        cchar		keys[] = "pen" ;
	        cchar		*vp ;
	        char		ks[2] = { 0, 0 } ;
	        char		tbuf[MAXPATHLEN+1] ;
	        for (i = 0 ; keys[i] != '\0' ; i += 1) {
	            const int	kch = MKCHAR(keys[i]) ;
	            vp = NULL ;
	            vl = -1 ;
	            switch (kch) {
	            case 'p':
	                vp = pip->pr ;
	                break ;
	            case 'e':
	                vp = "etc" ;
	                break ;
	            case 'n':
	                vp = pip->searchname ;
	                break ;
	            } /* end switch */
	            if ((rs >= 0) && (vp != NULL)) {
	                ks[0] = kch ;
	                rs = vecstr_envadd(&sv,ks,vp,vl) ;
	            }
	            if (rs < 0) break ;
	        } /* end for */
	        if (rs >= 0) {
	            if ((rs = permsched(sched1,&sv,tbuf,tlen,cfn,R_OK)) >= 0) {
	                cchar	**vpp = &pip->cfname ;
	                rl = rs ;
	                rs = proginfo_setentry(pip,vpp,tbuf,rs) ;
	            } else if (isNotPresent(rs)) {
	                rs = SR_OK ;
		    }
	        }
	        rs1 = vecstr_finish(&sv) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (vecstr) */
	} /* end if (specified) */

	if ((pip->debuglevel > 0) && (pip->cfname != NULL)) {
	    cchar	*pn = pip->progname ;
	    shio_printf(pip->efp,"%s: conf=%s\n",pn,pip->cfname) ;
	}

	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (procfindconf) */


static int procourconf_begin(PROGINFO *pip)
{
	LOCINFO		*lip = pip->lip ;
	int		rs = SR_OK ;

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	    debugprintf("msumain/procourconf_begin: ent\n") ;
#endif

	if ((rs = procfindconf(pip)) > 0) {
	    const int	size = sizeof(CONFIG) ;
	    void	*p ;
	    if ((rs = uc_malloc(size,&p)) >= 0) {
	        const int	ic = lip->intconfig ;
	        cchar		*cfname = pip->cfname ;
	        pip->config = p ;
	        if ((rs = config_start(pip->config,pip,cfname,ic)) >= 0) {
	            pip->open.config = TRUE ;
	        } /* end if (config_start) */
	        if (rs < 0) {
	            uc_free(pip->config) ;
	            pip->config = NULL ;
	        }
	    } /* end if (m-a) */
	} /* end if (procfindconf) */

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	    debugprintf("msumain/procourconf_begin: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (procourconf_begin) */


static int procourconf_end(PROGINFO *pip)
{
	int		rs = SR_OK ;
	int		rs1 ;

	if (pip->config != NULL) {
	    if (pip->open.config) {
	        pip->open.config = FALSE ;
	        rs1 = config_finish(pip->config) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    rs1 = uc_free(pip->config) ;
	    if (rs >= 0) rs = rs1 ;
	    pip->config = NULL ;
	}
#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	    debugprintf("msumain/procourconf_end: ret rs=%d\n",rs) ;
#endif
	return rs ;
}
/* end subroutine (procourconf_end) */


static int procourdefs(PROGINFO *pip)
{
	LOCINFO		*lip = pip->lip ;
	int		rs ;
	cchar		**envv = pip->envv ;

	if (pip->logsize == 0) pip->logsize = LOGSIZE ;

	if (pip->intpoll == 0) pip->intpoll = TO_POLL ;

	if (lip->msfname == NULL) {
	    cchar	*cp ;
	    if ((cp = getourenv(envv,VARMSFNAME)) != NULL) {
	        lip->final.msfname = TRUE ;
	        lip->have.msfname = TRUE ;
	        lip->msfname = cp ;
	    }
	}

	if ((rs = proclogid(pip)) >= 0) {
	    rs = locinfo_defs(lip) ;
	} /* end if (proclogid) */

	return rs ;
}
/* end subroutine (procourdefs) */


static int procbackdefs(PROGINFO *pip)
{
	int		rs = SR_OK ;
	cchar		**envv = pip->envv ;
	cchar		*cp ;

	if (pip->pidfname == NULL) {
	    if ((cp = getourenv(envv,VARPIDFNAME)) != NULL) {
	        pip->final.pidfname = TRUE ;
	        pip->have.pidfname = TRUE ;
	        pip->pidfname = cp ;
	    }
	}

	return rs ;
}
/* end subroutine (procbackdefs) */


static int procdaemondefs(PROGINFO *pip)
{
	LOCINFO		*lip = pip->lip ;
	int		rs = SR_OK ;
	cchar		**envv = pip->envv ;
	cchar		*cp ;

	if (pip->pidfname == NULL) {
	    if ((cp = getourenv(envv,VARPIDFNAME)) != NULL) {
	        pip->final.pidfname = TRUE ;
	        pip->have.pidfname = TRUE ;
	        pip->pidfname = cp ;
	    }
	}

	if (lip->reqfname == NULL) {
	    if ((cp = getourenv(envv,VARREQFNAME)) != NULL) {
	        lip->final.reqfname = TRUE ;
	        lip->have.reqfname = TRUE ;
	        lip->reqfname = cp ;
	    }
	}

	if (pip->intpoll == 0) pip->intpoll = TO_POLL ;

	if (pip->intidle == 0) pip->intidle = TO_IDLE ;

	if (pip->intlock == 0) pip->intlock = TO_LOCK ;

	if (pip->intmark == 0) pip->intmark = TO_MARK ;

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    debugprintf("msumain: daemon=%u logging=%u\n",
	        pip->f.daemon,pip->have.logprog) ;
#endif
#if	CF_DEBUGS
	debugprintf("msumain: daemon=%u logging=%u\n",
	    pip->f.daemon,pip->have.logprog) ;
#endif

	if (pip->debuglevel > 0) {
	    cchar	*pn = pip->progname ;
	    cchar	*fmt ;
	    if (pip->intpoll >= 0) {
	        fmt = "%s: mspoll=%u\n" ;
	    } else {
	        fmt = "%s: mspoll=Inf\n" ;
	    }
	    shio_printf(pip->efp,fmt,pn,pip->intpoll) ;
	    shio_printf(pip->efp,"%s: intspeed=%u\n",pn,lip->intspeed) ;
	}

	return rs ;
}
/* end subroutine (procdaemondefs) */


static int proclogid(PROGINFO *pip)
{
	int		rs ;
	if ((rs = lib_runmode()) >= 0) {
	    if (rs & KSHLIB_RMKSH) {
	        if ((rs = lib_serial()) >= 0) {
	            const int	s = rs ;
	            const int	plen = LOGIDLEN ;
	            const int	pv = pip->pid ;
	            cchar	*nn = pip->nodename ;
	            char	pbuf[LOGIDLEN+1] ;
	            if ((rs = mkplogid(pbuf,plen,nn,pv)) >= 0) {
	                const int	slen = LOGIDLEN ;
	                char		sbuf[LOGIDLEN+1] ;
	                if ((rs = mksublogid(sbuf,slen,pbuf,s)) >= 0) {
	                    cchar	**vpp = &pip->logid ;
	                    rs = proginfo_setentry(pip,vpp,sbuf,rs) ;
	                }
	            }
	        } /* end if (lib_serial) */
	    } /* end if (runmode-KSH) */
	} /* end if (lib_runmode) */
	return rs ;
}
/* end subroutine (proclogid) */


static int procpidfname(PROGINFO *pip)
{
	int		rs = SR_OK ;
	int		pfl = -1 ;
	int		f_changed = FALSE ;
	cchar		*pfp ;
	char		rundname[MAXPATHLEN+1] ;
	char		cname[MAXNAMELEN + 1] ;
	char		tmpfname[MAXPATHLEN + 1] ;

	pfp = pip->pidfname ;
	if ((pfp == NULL) || (pfp[0] == '+')) {
	    cchar	*sn = pip->searchname ;

	    f_changed = TRUE ;
	    if ((rs = mkpath2(rundname,pip->pr,RUNDNAME)) >= 0) {
	        struct ustat	sb ;
		const int	rsn = SR_NOENT ;
	        if ((rs = uc_stat(rundname,&sb)) >= 0) {
	      	    if (! S_ISDIR(sb.st_mode)) rs = SR_NOTDIR ;
	        } else if (rs == rsn) {
	            rs = mkdirs(rundname,0777) ;
	        }
	    } /* end if (mkpath) */

	    if (rs >= 0) {
		cchar	*nn = pip->nodename ;
	        if ((rs = snsds(cname,MAXNAMELEN,nn,sn)) >= 0) {
	            pfp = tmpfname ;
	            rs = mkpath2(tmpfname,rundname,cname) ;
	            pfl = rs ;
		}
	    }

	} /* end if (creating a default PID file-name) */

	if ((rs >= 0) && (pfp != NULL) && (pfp[0] == '-')) {
	    pfp = NULL ;
	    pip->have.pidfname = FALSE ;
	    pip->f.pidfname = FALSE ;
	    f_changed = FALSE ;
	}

	if ((rs >= 0) && (pfp != NULL) && f_changed) {
	    cchar	**vpp = &pip->pidfname ;
	    rs = proginfo_setentry(pip,vpp,pfp,pfl) ;
	}

	return (rs >= 0) ? f_changed : rs ;
}
/* end subroutine (procpidfname) */


static int process(PROGINFO *pip)
{
	LOCINFO		*lip = pip->lip ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;

	if ((rs = locinfo_msfile(lip)) >= 0) {
	    if (pip->f.background || pip->f.daemon) {
	        if ((rs = procbackdefs(pip)) >= 0) {
	            if ((rs = procpidfname(pip)) >= 0) {
	                if ((rs = procbackinfo(pip)) >= 0) {
	                    if ((rs = ids_load(&pip->id)) >= 0) {
	                        if (pip->f.background) {
	                            rs = procback(pip) ;
	                        } else if (pip->f.daemon) {
	                            rs = procdaemon(pip) ;
	                            if (rs > 0) c = rs ;
	                        }
	                        rs1 = ids_release(&pip->id) ;
	                        if (rs >= 0) rs = rs1 ;
	                    } /* end if (ids) */
	                } /* end if (procbackinfo) */
	            } /* end if (procpidfname) */
	        } /* end if (procbackdefs) */
	    } else {
	        rs = procregular(pip) ;
	        c = rs ;
	    }
	} /* end if (locinfo_msfile) */

	if ((rs >= 0) && (pip->debuglevel > 0)) {
	    cchar	*pn = pip->progname ;
	    shio_printf(pip->efp, "%s: MS updates=%u\n",pn,c) ;
	} /* end if */

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("msumain/process: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (process) */


static int procbackinfo(PROGINFO *pip)
{
	LOCINFO		*lip = pip->lip ;
	int		rs = SR_OK ;
	cchar		*pn = pip->progname ;

	if ((rs >= 0) && (pip->debuglevel > 0)) {
	    cchar	*mntfname = lip->mntfname ;

	    if (pip->pidfname != NULL)
	        shio_printf(pip->efp,"%s: pid=%s\n",
	            pn,pip->pidfname) ;

	    shio_printf(pip->efp,"%s: intpoll=%u\n",
	        pn, pip->intpoll) ;

	    shio_printf(pip->efp,"%s: intmark=%u\n",
	        pn, pip->intmark) ;

	    shio_printf(pip->efp,"%s: intrun=%u\n",
	        pn, pip->intrun) ;

	    if (mntfname != NULL) {
	        shio_printf(pip->efp,"%s: mntfile=%s\n",
	            pip->progname,mntfname) ;
	    }

	    shio_flush(pip->efp) ;
	} /* end if (debugging information) */

	if (rs >= 0) {
	    rs = loginfo(pip) ;
	}

	return rs ;
}
/* end subroutine (procbackinfo) */


static int procback(PROGINFO *pip)
{
	int		rs ;

	if (pip->open.logprog) {
	    logprintf(pip,"mode=background") ;
	    logflush(pip) ;
	}

	if (pip->debuglevel > 0) {
	    shio_printf(pip->efp,"%s: mode=background\n",pip->progname) ;
	    bflush(pip->efp) ;
	}

	if ((rs = procbackcheck(pip)) >= 0) {
	    rs = procbacks(pip) ;
	}

	return rs ;
}
/* end subroutine (procback) */


static int procbackcheck(PROGINFO *pip)
{
	LOCINFO		*lip = pip->lip ;
	int		rs ;
	int		rs1 ;
	cchar		*pn = pip->progname ;
	cchar		*fmt ;

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("msumain/procbackcheck: ent\n") ;
#endif

	if ((rs = locinfo_lockbegin(lip)) >= 0) {
	    {
	        rs = procmntcheck(pip) ;
	    }
	    rs1 = locinfo_lockend(lip) ;
	    if (rs >= 0) rs = rs1 ;
	} else {
	    if (! pip->f.quiet) {
	        fmt = "%s: could not acquire PID lock (%d)\n" ;
	        shio_printf(pip->efp,fmt,pn,rs) ;
	    }
	}

	return rs ;
}
/* end subroutine (procbackcheck) */


static int procmntcheck(PROGINFO *pip)
{
	LOCINFO		*lip = pip->lip ;
	int		rs = SR_OK ;
	if (lip->mntfname != NULL) {
	    struct ustat	usb ;
	    cchar		*pn = pip->progname ;
	    cchar		*fmt ;
	    if ((rs = u_stat(lip->mntfname,&usb)) >= 0) {
	        if (S_ISREG(usb.st_mode)) {
	            rs = sperm(&pip->id,&usb,W_OK) ;
	        } else
	            rs = SR_BUSY ;
	        if (rs < 0) {
	            if (! pip->f.quiet) {
	                fmt = "%s: inaccessible mount point (%d)\n" ;
	                shio_printf(pip->efp,fmt,pn,rs) ;
	            }
	        }
	    }
	} /* end if (mntfname) */
	return rs ;
}
/* end subroutine (procmntcheck) */


static int procbacks(PROGINFO *pip)
{
	const int	elen = MAXPATHLEN ;
	int		rs ;
	cchar		*pn = pip->progname ;
	cchar		*fmt ;
	char		ebuf[MAXPATHLEN+1] ;

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	    debugprintf("main/procbacks: ent\n") ;
#endif

	if ((rs = procexecname(pip,ebuf,elen)) >= 0) {
	    int		el = rs ;
	    cchar	*pf = ebuf ;
	    cchar	*tp ;
	    char	pbuf[MAXPATHLEN+1] ;

	    if (pip->debuglevel > 0) {
	        fmt = "%s: execname=%t\n" ;
	        shio_printf(pip->efp,fmt,pn,ebuf,el) ;
	    }

	    if ((tp = strnrpbrk(ebuf,el,"/.")) != NULL) {
	        if (tp[0] == '.') {
	            el = (tp-ebuf) ;
	            ebuf[el] = '\0' ;
	        }
	    }

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	    debugprintf("main/procbacks: prog=%t\n",ebuf,el) ;
#endif

	    if ((rs = prgetprogpath(pip->pr,pbuf,ebuf,el)) > 0) {
	        pf = pbuf ;
	    }

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	    debugprintf("main/procbacks: mid rs=%d\n",rs) ;
#endif

	    if (rs >= 0) {
	        int	i = 0 ;
	        cchar	*av[5] ;
	        char	dbuf[10+1] ;
	        if (pip->debuglevel > 0) {
	            fmt = "%s: pf=%s\n" ;
	            shio_printf(pip->efp,fmt,pn,pf) ;
	        }
	        av[i++] = pip->progname ;
	        av[i++] = "-daemon" ;
	        if (pip->debuglevel > 0) {
	            bufprintf(dbuf,10,"-D=%u",pip->debuglevel) ;
	            av[i++] = dbuf ;
	        }
	        av[i++] = NULL ;
	        rs = procbacker(pip,pf,av) ;
	    } /* end if (ok) */
	} /* end if (procexecname) */

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	    debugprintf("main/procbacks: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (procbacks) */


static int procbacker(PROGINFO *pip,cchar *pf,cchar **av)
{
	SPAWNER		s ;
	int		rs ;
	int		rs1 ;
	int		pid = 0 ;
	cchar		**ev = pip->envv ;
#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	    debugprintf("main/procbacker: ent\n") ;
#endif
	if ((rs = spawner_start(&s,pf,av,ev)) >= 0) {
	    if ((rs = procbackenv(pip,&s)) >= 0) {
	        int	i ;
	        for (i = 0 ; sigignores[i] > 0 ; i += 1) {
	            spawner_sigignore(&s,sigignores[i]) ;
	        }
	        spawner_setsid(&s) ;
	        if (pip->uid != pip->euid) {
	            spawner_seteuid(&s,pip->uid) ;
		}
	        if (pip->gid != pip->egid) {
	            spawner_setegid(&s,pip->gid) ;
		}
	        for (i = 0 ; i < 2 ; i += 1) {
	            spawner_fdclose(&s,i) ;
	        }
	        if ((rs = spawner_run(&s)) >= 0) {
	            cchar	*fmt ;
	            pid = rs ;
	            if (pip->open.logprog) {
	                fmt = "backgrounded (%u)" ;
	                logprintf(pip,fmt,pid) ;
	            }
	        }
	    } /* end if (procbackenv) */
	    rs1 = spawner_finish(&s) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (spawner) */
#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	    debugprintf("main/procbacker: ret rs=%d pid=%u\n",rs,pid) ;
#endif
	return (rs >= 0) ? pid : rs ;
}
/* end subroutine (procbacker) */


static int procbackenv(PROGINFO *pip,SPAWNER *srp)
{
	LOCINFO		*lip = pip->lip ;
	BUFFER		b ;
	int		rs ;
	int		rs1 ;
	cchar		*varopts = VAROPTS ;

	if ((rs = buffer_start(&b,ENVBUFLEN)) >= 0) {
	    cchar	*np ;
	    int		v ;
	    int		i ;
	    int		c = 0 ;

	    for (i = 0 ; i < 6 ; i += 1) {
	        np = NULL ;
	        switch (i) {
	        case 0:
	            v = pip->intrun ;
	            if (v > 0) np = "intrun" ;
	            break ;
	        case 1:
	            v = pip->intidle ;
	            if (v > 0) np = "intidle" ;
	            break ;
	        case 2:
	            v = pip->intpoll ;
	            if (v > 0) np = "intpoll" ;
	            break ;
	        case 3:
	            v = lip->intconfig ;
	            if (v > 0) np = "intconfig" ;
	            break ;
	        case 4:
	            v = lip->intspeed ;
	            if (v > 0) np = "intspeed" ;
	            break ;
	        case 5:
	            v = (pip->f.reuseaddr&1) ;
	            if (v > 0) np = "resueaddr" ;
	            break ;
	        } /* end switch */
	        if (np != NULL) {
	            if (c++ > 0) {
	                buffer_char(&b,CH_COMMA) ;
	            }
	            rs = buffer_printf(&b,"%s=%d",np,v) ;
	        }
	        if (rs < 0) break ;
	    } /* end for */

	    if (rs >= 0) {
	        cchar	*vp ;
	        for (i = 0 ; i < 4 ; i += 1) {
	            np = NULL ;
	            switch (i) {
	            case 0:
	                if (pip->pidfname != NULL) {
	                    np = "pidfile" ;
	                    vp = pip->pidfname ;
	                }
	                break ;
	            case 1:
	                if (lip->reqfname != NULL) {
	                    np = "reqfile" ;
	                    vp = lip->reqfname ;
	                }
	                break ;
	            case 2:
	                if (lip->mntfname != NULL) {
	                    np = "mntfile" ;
	                    vp = lip->mntfname ;
	                }
	                break ;
	            case 3:
	                if (lip->msfname != NULL) {
	                    np = "msfile" ;
	                    vp = lip->msfname ;
	                }
	                break ;
	            } /* end switch */
	            if (np != NULL) {
	                if (c++ > 0) {
	                    buffer_char(&b,CH_COMMA) ;
	                }
	                rs = buffer_printf(&b,"%s=%s",np,vp) ;
	            } /* end if (non-null) */
	            if (rs < 0) break ;
	        } /* end for */
	    } /* end if (ok) */

	    if ((rs >= 0) && (c > 0)) {
	        if ((rs = buffer_get(&b,&np)) >= 0) {
	            rs = spawner_envset(srp,varopts,np,rs) ;
	        }
	    }

	    rs1 = buffer_finish(&b) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (buffer) */

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	    debugprintf("main/procbackenv: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (procbackenv) */


static int procdaemon(PROGINFO *pip)
{
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	cchar		*pn = pip->progname ;
	cchar		*fmt ;

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	    debugprintf("main/procdaemon: ent\n") ;
#endif

	if (pip->open.logprog) {
	    logprintf(pip,"mode=daemon") ;
	}
	if (pip->debuglevel > 0) {
	    shio_printf(pip->efp,"%s: mode=daemon\n",pip->progname) ;
	}

	if ((rs = procdaemondefs(pip)) >= 0) {
	    if ((rs = procdaemoncheck(pip)) >= 0) {
	        LOCINFO		*lip = pip->lip ;
	        if ((rs = locinfo_defdaemon(lip)) >= 0) {
	            if ((rs = locinfo_lockbegin(lip)) >= 0) {
	                {
	                    rs = procupdate(pip) ;
	                    if (rs >= 0) c = rs ;
	                }
	                rs1 = locinfo_lockend(lip) ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (lock) */
	        } /* end if (locinfo_defdaemon) */
	    } /* end if (procdaemoncheck) */
	} /* end if (procdaemondefs) */

	if ((pip->debuglevel > 0) && (pip->efp != NULL)) {
	    fmt = "%s: daemon exiting (%d)\n" ;
	    shio_printf(pip->efp,fmt,pn,rs) ;
	}
	if (pip->open.logprog) {
	    char	tbuf[TIMEBUFLEN+1] ;
	    fmt = "%s exiting c=%u (%d)" ;
	    timestr_logz(pip->daytime,tbuf) ;
	    logprintf(pip,tbuf,c,rs) ;
	}

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (procdaemon) */


static int procdaemoncheck(PROGINFO *pip)
{
	int		rs ;

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("msumain/procdaemoncheck: ent\n") ;
#endif

	rs = procmntcheck(pip) ;

	return rs ;
}
/* end subroutine (procdaemoncheck) */


static int procregular(PROGINFO *pip)
{
	LOCINFO		*lip = pip->lip ;
	int		rs ;
	int		c = 1 ;
#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("msumain/procregular: ent\n") ;
#endif

	if ((rs = locinfo_defreg(lip)) >= 0) {
	    rs = procupdate(pip) ;
	}

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("msumain/procregular: ret rs=%d\n",rs) ;
#endif
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (procregular) */


static int procupdate(PROGINFO *pip)
{
	LOCINFO		*lip = pip->lip ;
	CONFIG		*cop = (CONFIG *) pip->config ;
	struct pollfd	fds[4] ;
	MSFILE		ms ;
	MSFILE_ENT	e, etmp ;
	const int	pagesize = getpagesize() ;
	int		rs, rs1 ;
	int		ppm ;
	int		nsecs, secs ;
	int		i ;
	int		nfds = 0 ;
	int		oflags ;
	int		c = 0 ;
	char		timebuf[TIMEBUFLEN + 1] ;

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("msumain/procupdate: ent msfname=%s\n",lip->msfname) ;
#endif

	ppm = ((1024 * 1024) / pagesize) ;
	oflags = O_RDWR ;
	rs = msfile_open(&ms,lip->msfname,oflags,0666) ;

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("msumain/procupdate: msfile_open() rs=%d\n",rs) ;
#endif

	if (rs < 0)
	    goto ret0 ;

	if (pip->f.daemon && lip->f.listen) {
	    if ((rs = procipcbegin(pip)) >= 0) {
	        fds[nfds].fd = lip->rfd ;
	        fds[nfds].events = POLLIN | POLLPRI ;
	        nfds += 1 ;
	    }
	} /* end if */

	if (rs < 0)
	    goto ret1 ;

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("msumain/procupdate: enter2") ;
#endif

#if	CF_DEBUG
	if (DEBUGLEVEL(4)) {
	    debugprintf("msumain/procupdate: msfile_match rs=%d\n",rs) ;
	    debugprintf("msumain/procupdate: msnode=%s\n",lip->msnode) ;
	}
#endif
	rs1 = msfile_match(&ms,pip->daytime,lip->msnode,-1,&e) ;

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("msumain/procupdate: msfile_match() rs=%d\n",rs1) ;
#endif
	if (rs1 == SR_NOTFOUND) {
	    memset(&e,0,sizeof(MSFILE_ENT)) ;
	    strwcpy(e.nodename,lip->msnode,MSFILEE_LNODENAME) ;
	} else
	    rs = rs1 ;

	if (rs < 0) goto ret2 ;

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("msumain/procupdate: enter3\n") ;
#endif

/* get some updated information */

	if (! lip->f.zerospeed) {
	    rs = procupspeed(pip,&e) ;
	} else {
	    e.speed = 0 ;
	    e.stime = pip->daytime ;
	} /* end if (zero-speed) */

/* were we requested to do a disable? */

	if (pip->f.disable) {
	    e.flags |= MSFLAG_MDISABLED ;
	    if (pip->intdis > 0) {
	        e.dtime = pip->daytime + pip->intdis ;
	    }
	} /* end if */

/* do some load-ave updates */

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("msumain/procupdate: for-before rs=%d\n",rs) ;
#endif

	for (c = 0 ; rs >= 0 ; c += 1) {

	    if (pip->f.daemon) {

	        e.pid = pip->pid ;
#if	CF_DEBUGS
	        msudebug_lockprint(pip,"top0") ;
#endif
	        if ((c & 15) == 1) {
	            rs = locinfo_lockcheck(pip->lip) ;
		}

	        if (rs < 0) break ;
	    } /* end if (daemon mode) */

/* continue with the update */

	    if (rs >= 0) {
	        e.btime = lip->ti_boot ;
	        if ((rs = getnprocessors(pip->envv,0)) >= 0) {
	            e.ncpu = rs ;
	            if ((rs = uc_nprocs(0)) >= 0) {
	                e.nproc = rs ;
	            } else if (isNotPresent(rs) || (rs == SR_NOSYS)) {
	                rs = SR_OK ;
		    }
	            if (rs >= 0) {
	                rs = u_getloadavg(e.la,3) ;
		    }
	        } /* end if */
	    } /* end if (updating SYSMISC information) */

#if	CF_DEBUG
	    if (DEBUGLEVEL(4)) {
	        double	fla[3] ;
	        for (i = 0 ; i < 3 ; i += 1) {
	            fla[i] = e.la[i] ;
	            fla[i] /= FSCALE ;
	            debugprintf("msumain/procupdate: la%u=%7.3f\n",i,fla[i]) ;
	        }
	    }
#endif /* CF_DEBUG */

/* OK, now calculate the megabytes of each type of memory */

	    entry_mem(&e,ppm) ;

/* finish off with time stamps */

	    e.atime = (uint) 0 ;
	    e.utime = (uint) pip->daytime ;

/* write-back the update */

#if	CF_DEBUGS
	    debugprintf("msumain/procupdate: msfile_update\n") ;
#endif
	    if (rs >= 0)
	        rs = msfile_update(&ms,pip->daytime,&e) ;

#if	CF_DEBUG
	    if (DEBUGLEVEL(4))
	        debugprintf("msumain/procupdate: msfile_update() rs=%d\n",rs) ;
#endif

	    if (rs < 0)
	        break ;

	    if ((! pip->f.daemon) && (lip->nu == 0))
	        break ;

	    if ((lip->nu > 0) && (c == lip->nu))
	        break ;

	    if (pip->intrun > 0) {
	        int	intrun = pip->intrun ;
	        if ((pip->daytime - lip->ti_start) >= intrun) break ;
	    }

#if	CF_DEBUG
	    if (DEBUGLEVEL(4))
	        debugprintf("msumain/procupdate: logcheck\n") ;
#endif
	    if (rs >= 0) {
	        rs = logcheck(pip) ;
	    }

#if	CF_DEBUG
	    if (DEBUGLEVEL(4))
	        debugprintf("msumain/procupdate: logcheck rs=%d\n",rs) ;
#endif

/* sleep for daemon mode */

	    nsecs = pip->intpoll ;
	    for (secs = 0 ; (rs >= 0) && (secs < nsecs) ; secs += 1) {
	        if ((rs = u_poll(fds,nfds,POLLMULT)) > 0) {
	            pip->daytime = time(NULL) ;
	            for (i = 0 ; (rs >= 0) && (i < nfds) ; i += 1) {
	                const int	pfd = fds[i].fd ;
	                const int	re = fds[i].revents ;
	                if ((lip->open.listen && (pfd == lip->rfd))) {
	                    if (re != 0) {
	                        rs = procipcreq(pip,re,&e) ;
	                    }
	                }
	            } /* end for */
		} else {
	            pip->daytime = time(NULL) ;
	            if (rs == SR_INTR) rs = SR_OK ;
	        } /* end if (got something) */

	        if (rs < 0) break ;
	    } /* end for (waiting) */

#if	CF_DEBUG
	    if (DEBUGLEVEL(4))
	        debugprintf("msumain/procupdate: mid3\n") ;
#endif

/* maintenance */

	    if ((rs >= 0) && pip->open.config && ((c & 3) == 0)) {
	        rs = config_check(cop) ;
	        if ((rs > 0) && pip->open.logprog) {
	            timestr_logz(pip->daytime,timebuf) ;
	            logprintf(pip,"%s re-configuration",timebuf) ;
	        }
	    }

	    if ((rs >= 0) && ((c & 15) == 5)) {
	        rs = msfile_check(&ms,pip->daytime) ;
	    }

	    if ((rs >= 0) && pip->open.logprog && ((c & 7) == 1)) {
	        if ((pip->daytime - lip->ti_marklog) >= pip->intmark) {
	            int		rem = 0 ;
	            lip->ti_marklog = pip->daytime ;
		    if (pip->intrun > 0) {
			rem = (pip->intrun - (pip->daytime-lip->ti_start)) ;
		    }
	            logmark(pip,rem) ;
	        } /* end if */
	    } /* end if */

/* handle the special "exit" condition */

	    if ((rs >= 0) && (lip->cmd[0] != '\0')) {
	        rs = procfcmd(pip) ;
	        if (rs > 0) break ;
	    }

/* get a flesh copy of the entry */

	    if (rs >= 0) {
	        rs1 = msfile_match(&ms,pip->daytime,lip->msnode,-1,&etmp) ;
	        if (rs1 >= 0) e = etmp ;
	    }

/* check disabled state only in daemon mode */

	    if ((e.dtime != 0) && (pip->daytime >= e.dtime)) {
	        e.dtime = 0 ;
	        e.flags &= (~ MSFLAG_MDISABLED) ;
	    }

	    if (pip->open.logprog) {
	        logflush(pip) ;
	    }

	    if (rs >= 0) rs = lib_sigquit() ;
	    if (rs >= 0) rs = lib_sigterm() ;
	    if (rs >= 0) rs = lib_sigintr() ;
	} /* end for (daemon-loop) */

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("msumain/procupdate: for-after rs=%d c=%d\n",
		rs,c) ;
#endif /* CF_DEBUG */

ret2:
	if (lip->open.listen) {
	    rs1 = procipcend(pip) ;
	    if (rs >= 0) rs = rs1 ;
	}

ret1:

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("msumain/procupdate: msfile_close rs=%d\n",rs) ;
#endif
	rs1 = msfile_close(&ms) ;
	if (rs >= 0) rs = rs1 ;

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("msumain/procupdate: msfile_close() rs=%d\n",rs1) ;
#endif

ret0:

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("msumain/procupdate: ret rs=%d c=%u\n",rs,c) ;
#endif

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (procupdate) */


static int procupspeed(PROGINFO *pip,MSFILE_ENT *ep)
{
	LOCINFO		*lip = pip->lip ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		f = FALSE ;
	cchar		*pn = pip->progname ;

#if	CF_CPUSPEED
	f = (ep->btime == 0) || (ep->speed == 0) || lip->f.speedname ;

	if (! f) {
	    f = (ep->stime == 0) || 
	        ((pip->daytime - ep->stime) >= lip->intspeed) ;
	}

	if (f && (! lip->f.quick)) {

#if	CF_DEBUG
	    if (DEBUGLEVEL(5))
	        debugprintf("msumain/procupspeed: recalculating\n") ;
#endif

	    if (pip->debuglevel > 0) {
	        shio_printf(pip->efp,
	            "%s: speed recalculation is indicated\n",pn) ;
	    }

	    shio_flush(pip->efp) ;

	    rs1 = cpuspeed(pip->pr,lip->speedname,0) ;
	    pip->daytime = time(NULL) ;

	    if (rs1 < 0) {
	        if ((! pip->f.quiet) && (pip->efp != NULL)) {
	            shio_printf(pip->efp,
	                "%s: speed name=%s\n",
	                pn,lip->speedname) ;
	            shio_printf(pip->efp,
	                "%s: speed subsystem is not available (%d)\n",
	                pn,rs1) ;
	        }
	    } else {
	        ep->speed = rs1 ;
	        ep->stime = pip->daytime ;
	    }

	} /* end if (needed speed update) */
#endif /* CF_CPUSPEED */

	return rs ;
}
/* end subroutine (procupspeed) */


static int procfcmd(PROGINFO *pip)
{
	LOCINFO		*lip = pip->lip ;
	int		rs = SR_OK ;
	int		f_exit = FALSE ;

	if (lip->cmd[0] != '\0') {
	    const int	ci = matostr(cmds,3,lip->cmd,-1) ;

	    switch (ci) {
	    case cmd_exit:
	        f_exit = TRUE ;
	        break ;
	    case cmd_mark:
		{
	            int	rem = 0 ;
		    if (pip->intrun > 0) {
		        rem = (pip->intrun - (pip->daytime-lip->ti_start)) ;
		    }
	            logmark(pip,rem) ;
		}
	        break ;
	    case cmd_report:
	        logreport(pip) ;
	        break ;
	    default:
	        loginvalidcmd(pip,lip->cmd) ;
	        break ;
	    } /* end switch */
	    lip->cmd[0] = '\0' ;

	} /* end if (non-nul) */

	return (rs >= 0) ? f_exit : rs ;
}
/* end if (procfcmd) */


static int procexecname(PROGINFO *pip,char *rbuf,int rlen)
{
	int		rs ;
	if ((rs = proginfo_progdname(pip)) >= 0) {
	    cchar	*dn = pip->progdname ;
	    cchar	*pn = pip->progname ;
	    rs = mknpath2(rbuf,rlen,dn,pn) ;
	}
	return rs ;
}
/* end subroutine (procexecname) */


static int entry_mem(MSFILE_ENT *ep,int ppm)
{
	int		rs ;
	int		mu = 0 ;

	if (ppm > 0) {
	    SYSMEMUTIL	m ;
	    if ((rs = sysmemutil(&m)) >= 0) {
	        mu = rs ;
	        ep->pmtotal = (m.mt / ppm) ;
	        ep->pmavail = (m.ma / ppm) ;
	    } else if (rs == SR_NOSYS) {
	        ep->pmtotal = 1 ;
	        ep->pmavail = 0 ;
	        rs = SR_OK ;
	    }
	} else {
	    rs = SR_INVALID ;
	}

	return (rs >= 0) ? mu : rs ;
}
/* end subroutine (entry_mem) */



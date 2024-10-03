/* b_isproc SUPPORT */
/* lang=C++20 */

/* SHELL built-in: determine if a file has certain attributes */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* non-switchable debug print-outs */
#define	CF_DEBUG	0		/* switchable at invocation */
#define	CF_DEBUGMALL	1		/* debug memory-allocations */

/* revision history:

	= 2004-03-01, David A�D� Morano
	This code was originally written.  

*/

/* Copyright � 2004 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	This is sort of a replacement for the |test(1)| program (or
	the various SHELL built-in versions).  Except that this
	version does not discriminate against a file if it is a
	symbolic link and the link is dangling.

	Synopsis:
	$ isproc <pidfile>

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
#include	<limits.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<time.h>
#include	<stdlib.h>
#include	<string.h>
#include	<usystem.h>
#include	<bits.h>
#include	<keyopt.h>
#include	<paramopt.h>
#include	<vecobj.h>
#include	<ids.h>
#include	<getax.h>
#include	<filereadln.h>
#include	<exitcodes.h>
#include	<localmisc.h>

#include	"shio.h"
#include	"kshlib.h"
#include	"b_isproc.h"
#include	"defs.h"


/* local defines */

#define	LOCINFO		struct locinfo
#define	LOCINFO_FL	struct locinfo_flags
#define	LOCINFO_PCUR	struct locinfo_pcur

#define	PO_PIDFILE	"pidfile"
#define	PO_QUERY	"query"


/* external subroutines */

extern int	sncpy2(char *,int,const char *,const char *) ;
extern int	sncpy3(char *,int,const char *,const char *,const char *) ;
extern int	mkpath2(char *,const char *,const char *) ;
extern int	mkpath3(char *,const char *,const char *,const char *) ;
extern int	sfskipwhite(const char *,int,const char **) ;
extern int	matstr(const char **,const char *,int) ;
extern int	matostr(const char **,int,const char *,int) ;
extern int	cfdeci(const char *,int,int *) ;
extern int	cfdecui(const char *,int,uint *) ;
extern int	cfdecti(const char *,int,int *) ;
extern int	optbool(const char *,int) ;
extern int	optvalue(const char *,int) ;
extern int	isdigitlatin(int) ;
extern int	isFailOpen(int) ;
extern int	isFailOpen(int) ;
extern int	isNotPresent(int) ;

extern int	printhelp(void *,cchar *,cchar *,cchar *) ;
extern int	proginfo_setpiv(PROGINFO *,cchar *,const struct pivars *) ;

#if	CF_DEBUGS || CF_DEBUG
extern int	debugopen(const char *) ;
extern int	debugprintf(const char *,...) ;
extern int	debugclose() ;
extern int	strlinelen(const char *,int,int) ;
#endif

extern cchar	*getourenv(cchar **,cchar *) ;

extern char	*strwcpy(char *,const char *,int) ;
extern char	*timestr_log(time_t,char *) ;
extern char	*timestr_logz(time_t,char *) ;
extern char	*timestr_elapsed(time_t,char *) ;


/* external variables */

extern char	**environ ;		/* definition required by AT&T AST */


/* local structures */

struct locinfo_flags {
	uint		pidfile:1 ;
	uint		query:1 ;
	uint		run:1 ;
	uint		running:1 ;
	uint		notrunning:1 ;
	uint		intage:1 ;
} ;

struct locinfo {
	LOCINFO_FL	have, f, changed, final ;
	LOCINFO_FL	open ;
	PROGINFO	*pip ;
	KEYOPT		akopts ;
	PARAMOPT	aparams ;
	int		intage ;
} ;

struct locinfo_pcur {
	PARAMOPT_CUR	cur ;
} ;


/* forward references */

static int	mainsub(int,cchar **,cchar **,void *) ;

static int	usage(PROGINFO *) ;

static int	locinfo_start(LOCINFO *,PROGINFO *) ;
static int	locinfo_pidfile(LOCINFO *,cchar *,int) ;
static int	locinfo_query(LOCINFO *,cchar *,int) ;
static int	locinfo_procopts(LOCINFO *) ;
static int	locinfo_intage(LOCINFO *,cchar *,cchar *) ;
static int	locinfo_queries(LOCINFO *) ;
static int	locinfo_procage(LOCINFO *,struct ustat *) ;
static int	locinfo_pcurbegin(LOCINFO *,LOCINFO_PCUR *) ;
static int	locinfo_pcurend(LOCINFO *,LOCINFO_PCUR *) ;
static int	locinfo_pget(LOCINFO *,LOCINFO_PCUR *,cchar **) ;
static int	locinfo_finish(LOCINFO *) ;

static int	procargs(PROGINFO *,ARGINFO *,BITS *,cchar *,cchar *) ;
static int	procfiles(PROGINFO *) ;
static int	procarg(PROGINFO *,cchar *) ;
static int	procpid(PROGINFO *,cchar *) ;
static int	procfile(PROGINFO *,cchar *) ;


/* local variables */

static const char	*argopts[] = {
	"ROOT",
	"VERSION",
	"VERBOSE",
	"HELP",
	"pm",
	"sn",
	"af",
	"ef",
	"of",
	"pid",
	NULL
} ;

enum argopts {
	argopt_root,
	argopt_version,
	argopt_verbose,
	argopt_help,
	argopt_pm,
	argopt_sn,
	argopt_af,
	argopt_ef,
	argopt_of,
	argopt_pid,
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
	{ SR_NOENT, EX_NOINPUT },
	{ SR_AGAIN, EX_TEMPFAIL },
	{ SR_DEADLK, EX_TEMPFAIL },
	{ SR_NOLCK, EX_TEMPFAIL },
	{ SR_TXTBSY, EX_TEMPFAIL },
	{ SR_ACCESS, EX_NOPERM },
	{ SR_REMOTE, EX_PROTOCOL },
	{ SR_NOSPC, EX_TEMPFAIL },
	{ SR_EXIT, EX_TERM },
	{ SR_INTR, EX_INTR },
	{ 0, 0 }
} ;

static const char	*akonames[] = {
	"quiet",
	"intage",
	NULL
} ;

enum akonames {
	akoname_quiet,
	akoname_intage,
	akoname_overlast
} ;

static const char	*progmodes[] = {
	"isproc",
	"isnotrunning",
	"isNotRunning",
	NULL
} ;

enum progmodes {
	progmode_isproc,
	progmode_isnotrunning,
	progmode_isNotRunning,
	progmode_overlast
} ;

static const char	*qopts[] = {
	"running",
	"notrunning",
	NULL
} ;

enum qopts {
	qopt_running,
	qopt_notrunning,
	qopt_overlast
} ;


/* exported variables */


/* exported subroutines */

int b_isproc(int argc,cchar *argv[],void *contextp) noex {
	int		rs ;
	int		rs1 ;
	int		ex = EX_OK ;

	if ((rs = lib_kshbegin(contextp,NULL)) >= 0) {
	    cchar	**envv = (cchar **) environ ;
	    ex = mainsub(argc,argv,envv,contextp) ;
	    rs1 = lib_kshend() ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (ksh) */

	if ((rs < 0) && (ex == EX_OK)) ex = EX_DATAERR ;

	return ex ;
}
/* end subroutine (b_isproc) */


int b_isnotrunning(int argc,cchar *argv[],void *contextp)
{
	return b_isproc(argc,argv,contextp) ;
}
/* end subroutine (b_isnotrunning) */


int b_isNotRunning(int argc,cchar *argv[],void *contextp)
{
	return b_isproc(argc,argv,contextp) ;
}
/* end subroutine (b_isNotRunning) */


int p_isproc(int argc,cchar *argv[],cchar *envv[],void *contextp)
{
	return mainsub(argc,argv,envv,contextp) ;
}
/* end subroutine (p_isproc) */


int p_isnotrunning(int argc,cchar *argv[],cchar *envv[],void *contextp)
{
	return mainsub(argc,argv,envv,contextp) ;
}
/* end subroutine (p_isnotrunning) */


int p_isNotRunning(int argc,cchar *argv[],cchar *envv[],void *contextp)
{
	return mainsub(argc,argv,envv,contextp) ;
}
/* end subroutine (p_isNotRunning) */


/* local subroutines */


/* ARGSUSED */
static int mainsub(int argc,cchar *argv[],cchar *envv[],void *contextp)
{
	PROGINFO	pi, *pip = &pi ;
	LOCINFO		li, *lip = &li ;
	ARGINFO		ainfo ;
	BITS		pargs ;
	SHIO		errfile ;

#if	(CF_DEBUGS || CF_DEBUG) && CF_DEBUGMALL
	uint		mo_start = 0 ;
#endif

	int		argr, argl, aol, akl, avl, kwi ;
	int		ai, ai_max, ai_pos ;
	int		rs, rs1 ;
	int		ex = EX_INFO ;
	int		f_optminus, f_optplus, f_optequal ;
	int		f_version = FALSE ;
	int		f_usage = FALSE ;
	int		f_help = FALSE ;
	int		f_early = FALSE ;

	const char	*argp, *aop, *akp, *avp ;
	const char	*argval = NULL ;
	const char	*pm = NULL ;
	const char	*sn = NULL ;
	const char	*pr = NULL ;
	const char	*afname = NULL ;
	const char	*ofname = NULL ;
	const char	*efname = NULL ;
	const char	*intagespec = NULL ;
	const char	*cp ;

#if	CF_DEBUGS || CF_DEBUG
	if ((cp = getourenv(envv,VARDEBUGFNAME)) != NULL) {
	    rs = debugopen(cp) ;
	    debugprintf("b_isproc: starting DFD=%d\n",rs) ;
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

#if	CF_DEBUGS
	debugprintf("b_isproc: pn=%s\n",pip->progname) ;
#endif

	if ((cp = getourenv(envv,VARBANNER)) == NULL) cp = BANNER ;
	rs = proginfo_setbanner(pip,cp) ;

/* initialize */

	pip->verboselevel = 1 ;
	pip->daytime = time(NULL) ;

	pip->lip = &li ;
	if (rs >= 0) rs = locinfo_start(lip,pip) ;
	if (rs < 0) {
	    ex = EX_OSERR ;
	    goto badlocstart ;
	}

/* start parsing the arguments */

	if (rs >= 0) rs = bits_start(&pargs,0) ;
	if (rs < 0) goto badpargs ;

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

	            argval = (argp+1) ;

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

	                switch (kwi) {

/* program-root */
	                case argopt_root:
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

	                case argopt_help:
	                    f_help = TRUE ;
	                    break ;

/* program mode */
	                case argopt_pm:
	                    if (f_optequal) {
	                        f_optequal = FALSE ;
	                        if (avl)
	                            pm = avp ;
	                    } else {
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl)
	                                pm = argp ;
	                        } else
	                            rs = SR_INVALID ;
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

/* argument file */
	                case argopt_af:
	                    if (f_optequal) {
	                        f_optequal = FALSE ;
	                        if (avl)
	                            afname = avp ;
	                    } else {
	                        if (argr > 0) {
	                        argp = argv[++ai] ;
	                        argr -= 1 ;
	                        argl = strlen(argp) ;
	                        if (argl)
	                            afname = argp ;
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

/* output file name */
	                case argopt_of:
	                    if (f_optequal) {
	                        f_optequal = FALSE ;
	                        if (avl)
	                            ofname = avp ;
	                    } else {
	                        if (argr > 0) {
	                        argp = argv[++ai] ;
	                        argr -= 1 ;
	                        argl = strlen(argp) ;
	                        if (argl)
	                            ofname = argp ;
				} else
	                            rs = SR_INVALID ;
	                    }
	                    break ;

	                case argopt_pid:
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl) {
					rs = locinfo_pidfile(lip,argp,argl) ;
	                            }
				} else
	                            rs = SR_INVALID ;
	                        break ;

/* handle all keyword defaults */
	                default:
	                    rs = SR_INVALID ;
	                    break ;

	                } /* end switch */

	            } else {

	                while (akl--) {
	                    const int	kc = MKCHAR(*akp) ;

	                    switch (kc) {

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

/* quiet mode */
	                    case 'Q':
	                        pip->f.quiet = TRUE ;
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

	                    case 'i':
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl)
	                                intagespec = argp ;
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
					KEYOPT	*kop = &lip->akopts ;
	                                rs = keyopt_loads(kop,argp,argl) ;
				    }
				} else
	                            rs = SR_INVALID ;
	                        break ;

/* queries */
	                    case 'q':
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl) {
	                                rs = locinfo_query(lip,argp,argl) ;
	                            }
				} else
	                            rs = SR_INVALID ;
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

#if	CF_DEBUG
	if (DEBUGLEVEL(2)) {
	    debugprintf("b_isproc: debuglevel=%u\n",pip->debuglevel) ;
	    debugprintf("b_isproc: pn=%s\n",pip->progname) ;
	    debugprintf("b_isproc: pm=%s\n",pm) ;
	}
#endif

	if (f_version) {
	    cchar	*pn = pip->progname ;
	    shio_printf(pip->efp,"%s: version %s\n",pn,VERSION) ;
	}

/* get our program mode */

	if (pm == NULL) pm = pip->progname ;

	if ((pip->progmode = matstr(progmodes,pm,-1)) >= 0) {
	    if (pip->debuglevel > 0) {
	        cchar	*pn = pip->progname ;
	        cchar	*fmt = "%s: pm=%s (%u)\n" ;
	        shio_printf(pip->efp,fmt,pn,pm,pip->progmode) ;
	    }
	} else {
	    cchar	*pn = pip->progname ;
	    cchar	*fmt = "%s: invalid program-mode (%s)\n" ;
	    shio_printf(pip->efp,fmt,pn,pm) ;
	    ex = EX_USAGE ;
	    rs = SR_INVALID ;
	}

/* set the program root and search-name */

	if (rs >= 0) {
	    PIVARS	vars = initvars ;
	    cchar	*varsn = VARSEARCHNAME ;
	    if ((rs = proginfo_setpiv(pip,pr,&vars)) >= 0) {
	        rs = proginfo_setsearchname(pip,varsn,sn) ;
	    }
	} /* end block */

	if (rs < 0) {
	    ex = EX_OSERR ;
	    goto retearly ;
	}

	if (pip->debuglevel > 0) {
	    shio_printf(pip->efp,"%s: pr=%s\n",pip->progname,pip->pr) ;
	    shio_printf(pip->efp,"%s: sn=%s\n",pip->progname,pip->searchname) ;
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
	}

	if (f_version || f_help || f_usage)
	    goto retearly ;


	ex = EX_OK ;

/* process the program options */

	if (afname == NULL) afname = getourenv(envv,VARAFNAME) ;

	if ((rs = locinfo_procopts(lip)) >= 0) {
	    if ((rs = locinfo_intage(lip,argval,intagespec)) >= 0) {
		if ((rs = locinfo_queries(lip)) >= 0) {
		    const int	f_notrun = (! lip->have.run) ;
		    switch (pip->progmode) {
		    case progmode_isproc:
			if (f_notrun) lip->f.running = TRUE ;
			break ;
		    case progmode_isnotrunning:
		    case progmode_isNotRunning:
			if (f_notrun) lip->f.notrunning = TRUE ;
			break ;
		    } /* end switch */
		}
	    }
	}

#if	CF_DEBUG
	if (DEBUGLEVEL(2)) {
	    char	timebuf[TIMEBUFLEN + 1] ;
	    debugprintf("b_isproc: daytime=%s\n",
	        timestr_logz(pip->daytime,timebuf)) ;
	    debugprintf("b_isproc: intage=%d\n",
	        lip->intage) ;
	}
#endif /* CF_DEBUG */

	memset(&ainfo,0,sizeof(ARGINFO)) ;
	ainfo.argc = argc ;
	ainfo.ai = ai ;
	ainfo.argv = argv ;
	ainfo.ai_max = ai_max ;
	ainfo.ai_pos = ai_pos ;

	if (rs >= 0) {
	    const char	*ofn = ofname ;
	    const char	*afn = afname ;
	    if ((rs = procargs(pip,&ainfo,&pargs,ofn,afn)) == 0) {
		rs = procfiles(pip) ;
	        f_early = rs ;
	    } else if (rs > 0) {
		f_early = TRUE ;
	    }
	} else if (ex == EX_OK) {
	    cchar	*pn = pip->progname ;
	    cchar	*fmt = "%s: invalid argument or configuration (%d)\n" ;
	    ex = EX_USAGE ;
	    shio_printf(pip->efp,fmt,pn,rs) ;
	    usage(pip) ;
	}

#if	CF_DEBUG
	if (DEBUGLEVEL(2)) {
	    debugprintf("b_isproc: f_early=%u\n",f_early) ;
	    debugprintf("b_isproc: f0 rs=%d ex=%u\n",rs,ex) ;
	}
#endif

	if ((rs >= 0) && (ex == EX_OK)) {
	    switch (pip->progmode) {
	    case progmode_isproc:
		if (f_early) ex = 1 ;
		break ;
	    case progmode_isnotrunning:
	    case progmode_isNotRunning:
		ex = (f_early) ? 0 : 1 ;
		break ;
	    } /* end switch */
	} /* end if */

	if ((rs >= 0) && (pip->debuglevel > 0)) {
	    const char	*fmt = "%s: f_early=%u\n" ;
	    shio_printf(pip->efp,fmt,pip->progname,f_early) ;
	}

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    debugprintf("b_isproc: f1 rs=%d ex=%u\n",rs,ex) ;
#endif

/* done */
	if ((rs < 0) && (ex == EX_OK)) {
	    switch (rs) {
	    case SR_INVALID:
	        ex = EX_USAGE ;
	        break ;
	    default:
	        ex = mapex(mapexs,rs) ;
	        break ;
	    } /* end switch */
	} else if ((rs >= 0) && (ex == EX_OK)) {
	    if ((rs = lib_sigterm()) < 0) {
	        ex = EX_TERM ;
	    } else if ((rs = lib_sigintr()) < 0) {
	        ex = EX_INTR ;
	    }
	} /* end if */

retearly:
	if (pip->debuglevel > 0) {
	    shio_printf(pip->efp,"%s: exiting ex=%u (%d)\n",
	        pip->progname,ex,rs) ;
	}

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    debugprintf("b_isproc: exiting ex=%u (%d)\n",ex,rs) ;
#endif

	if (pip->efp != NULL) {
	    pip->open.errfile = FALSE ;
	    shio_close(pip->efp) ;
	    pip->efp = NULL ;
	}

	bits_finish(&pargs) ;

badpargs:
	locinfo_finish(lip) ;

badlocstart:
	proginfo_finish(pip) ;

badprogstart:

#if	(CF_DEBUGS || CF_DEBUG) && CF_DEBUGMALL
	{
	    uint	mo ;
	    uc_mallout(&mo) ;
	    debugprintf("b_isproc: final mallout=%u\n",(mo-mo_start)) ;
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
/* end subroutine (mainsub) */


static int usage(PROGINFO *pip)
{
	int		rs = SR_OK ;
	int		wlen = 0 ;
	const char	*pn = pip->progname ;
	const char	*fmt ;

	fmt = "%s: USAGE> %s [-q <query(s)>] [<pid(s)>] [-pid <file(s)>]\n" ;
	if (rs >= 0) rs = shio_printf(pip->efp,fmt,pn,pn) ;
	wlen += rs ;

	fmt = "%s:  [-<intage>]\n" ;
	if (rs >= 0) rs = shio_printf(pip->efp,fmt,pn) ;
	wlen += rs ;

	fmt = "%s:  [-Q] [-D] [-v[=<n>]] [-HELP] [-V]\n" ;
	if (rs >= 0) rs = shio_printf(pip->efp,fmt,pn) ;
	wlen += rs ;

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (usage) */


/* ARGSUSED */
static int procargs(PROGINFO *pip,ARGINFO *aip,BITS *bop,cchar *ofn,cchar *afn)
{
	int		rs = SR_OK ;
	int		rs1 ;
	int		pan = 0 ;
	int		cl ;
	int		f_early = FALSE ;
	cchar		*pn = pip->progname ;
	cchar		*fmt ;
	const char	*cp ;

#if	CF_DEBUG
	if (DEBUGLEVEL(4)) {
	    debugprintf("main/procargs: ent argc=%u\n",aip->argc) ;
	    debugprintf("main/procargs: ai_max=%u\n",aip->ai_max) ;
	}
#endif

	if (rs >= 0) {
	    int	ai ;
	    int	f ;
	    for (ai = 1 ; ai < aip->argc ; ai += 1) {

	        f = (ai <= aip->ai_max) && (bits_test(bop,ai) > 0) ;
	        f = f || ((ai > aip->ai_pos) && (aip->argv[ai] != NULL)) ;
	        if (f) {
	            cp = aip->argv[ai] ;
		    if (cp[0] != '\0') {
	                pan += 1 ;
	                rs = procarg(pip,cp) ;
			f_early = (rs == 0) ;
		    }
		}

		if (f_early) break ;
	        if (rs < 0) break ;
	    } /* end for (handling positional arguments) */
	} /* end if (ok) */

	if ((rs >= 0) && (afn != NULL) && (afn[0] != '\0')) {
	    SHIO	afile, *afp = &afile ;

	    if (strcmp(afn,"-") == 0)
	        afn = STDFNIN ;

	    if ((rs = shio_open(afp,afn,"r",0666)) >= 0) {
	        const int	llen = LINEBUFLEN ;
		int		len ;
	        char		lbuf[LINEBUFLEN + 1] ;

	        while ((rs = shio_readline(afp,lbuf,llen)) > 0) {
	            len = rs ;

	            if (lbuf[len - 1] == '\n') len -= 1 ;
	            lbuf[len] = '\0' ;

		    if ((cl = sfskipwhite(lbuf,len,&cp)) > 0) {
			if (cp[0] != '#') {
			    pan += 1 ;
			    lbuf[(cp+cl)-lbuf] = '\0' ;
			    rs = procarg(pip,cp) ;
			    f_early = (rs == 0) ;
			}
		    }

		    if (f_early) break ;
		    if (rs < 0) break ;
	        } /* end while (reading lines) */

	        rs1 = shio_close(afp) ;
		if (rs >= 0) rs = rs1 ;
	    } else {
		fmt = "%s: inaccessible argument-list (%d)\n" ;
	        shio_printf(pip->efp,fmt,pn,rs) ;
	        shio_printf(pip->efp,"%s: afile=%s\n",pn,afn) ;
	    } /* end if */

	} /* end if (processing file argument file list) */

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    debugprintf("b_isproc/procargs: ret rs=%d f_early=%u\n",
		rs,f_early) ;
#endif

	return (rs >= 0) ? f_early : rs ;
}
/* end subroutine (procargs) */


static int procarg(PROGINFO *pip,cchar *spec)
{
	int		rs ;
	switch (pip->progmode) {
	case progmode_isproc:
	    rs = procpid(pip,spec) ;
	    break ;
	case progmode_isnotrunning:
	case progmode_isNotRunning:
	    rs = procfile(pip,spec) ;
	    break ;
	} /* end switch */
	return rs ;
}
/* end subroutine (procarg) */


static int procpid(PROGINFO *pip,cchar *pidspec)
{
	int		rs ;
	int		f_ok = FALSE ;

#if	CF_DEBUG
	if (DEBUGLEVEL(2)) {
	    debugprintf("b_isproc/procpid: ent\n") ;
	    debugprintf("b_isproc/procpid: f_running=%u\n",
		lip->f.running) ;
	    debugprintf("b_isproc/procpid: f_notrunning=%u\n",
		lip->f.notrunning) ;
	}
#endif

	if ((rs = optvalue(pidspec,-1)) >= 0) {
	    const pid_t	pid = rs ;
	    if ((rs = u_kill(pid,0)) >= 0) {
		f_ok = TRUE ;
	    } else if (rs == SR_PERM) {
		f_ok = TRUE ;
		rs = SR_OK ;
	    } else if (rs == SR_SRCH) {
		rs = SR_OK ;
	    }
	}

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    debugprintf("b_isproc/procpid: ret rs=%d f_ok=%u\n",rs,f_ok) ;
#endif

	return (rs >= 0) ? f_ok : rs ;
}
/* end subroutine (procpid) */


static int procfiles(PROGINFO *pip)
{
	LOCINFO		*lip = pip->lip ;
	LOCINFO_PCUR	pcur ;
	int		rs ;
	int		rs1 ;
	int		f_early = FALSE ;
	if ((rs = locinfo_pcurbegin(lip,&pcur)) >= 0) {
	    cchar	*vp ;
	    while ((rs = locinfo_pget(lip,&pcur,&vp)) > 0) {
		if ((rs = procfile(pip,vp)) == 0) {
		    f_early = TRUE ;
		    break ;
		}
		if (rs < 0) break ;
	    } /* end while */
	    rs1 = locinfo_pcurend(lip,&pcur) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (locinfo-pcur) */
#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    debugprintf("b_isproc/procfiles: ret rs=%d f_early=%u\n",
		rs,f_early) ;
#endif
	return (rs >= 0) ? f_early : rs ;
}
/* end subroutine (procfiles) */


static int procfile(PROGINFO *pip,cchar *fname)
{
	struct ustat	usb ;
	LOCINFO		*lip = pip->lip ;
	int		rs ;
	int		f_ok = FALSE ;

	if (fname == NULL) return SR_FAULT ;

#if	CF_DEBUG
	if (DEBUGLEVEL(2)) {
	    debugprintf("b/procfile: fname=%s\n",fname) ;
	    debugprintf("b/procfile: intage=%d\n",lip->intage) ;
	}
#endif /* CF_DEBUG */

	if ((rs = u_stat(fname,&usb)) >= 0) {
	    if ((rs = locinfo_procage(lip,&usb)) > 0) {
		const int	rlen = LINEBUFLEN ;
		char		rbuf[LINEBUFLEN+1] ;
		if ((rs = filereadln(fname,rbuf,rlen)) >= 0) {
		    rs = procpid(pip,rbuf) ;
		    f_ok = rs ;
		} else if (isNotPresent(rs)) {
		    rs = SR_OK ;
		}
	    }
	} else if (isNotPresent(rs)) {
	    rs = SR_OK ;
	} /* end if (u_stat) */

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    debugprintf("b_isproc/procfile: ret rs=%d f_ok=%u\n",rs,f_ok) ;
#endif

	return (rs >= 0) ? f_ok : rs ;
}
/* end subroutine (procfile) */


static int locinfo_start(LOCINFO *lip,PROGINFO *pip)
{
	int		rs ;

	memset(lip,0,sizeof(LOCINFO)) ;
	lip->pip = pip ;

	if ((rs = keyopt_start(&lip->akopts)) >= 0) {
	    pip->open.akopts = TRUE ;
	    if ((rs = paramopt_start(&lip->aparams)) >= 0) {
	        pip->open.aparams = TRUE ;
	    }
	    if (rs < 0) {
	        pip->open.akopts = FALSE ;
	        keyopt_finish(&lip->akopts) ;
	    }
	}

	return rs ;
}
/* end subroutine (locinfo_start) */


static int locinfo_finish(LOCINFO *lip)
{
	PROGINFO	*pip = lip->pip ;
	int		rs = SR_OK ;
	int		rs1 ;

	if (pip->open.aparams) {
	    pip->open.aparams = FALSE ;
	    rs1 = paramopt_finish(&lip->aparams) ;
	    if (rs >= 0) rs = rs1 ;
	}

	if (pip->open.akopts) {
	    pip->open.akopts = FALSE ;
	    rs1 = keyopt_finish(&lip->akopts) ;
	    if (rs >= 0) rs = rs1 ;
	}

	return rs ;
}
/* end subroutine (locinfo_finish) */


static int locinfo_pidfile(LOCINFO *lip,cchar *vp,int vl)
{
	PARAMOPT	*pop = &lip->aparams ;
	int		rs ;
	const char	*po = PO_PIDFILE ;
	if ((rs = paramopt_loads(pop,po,vp,vl)) >= 0) {
	     lip->f.pidfile = TRUE ;
	}
	return rs ;
}
/* end subroutine (locinfo_pidfile) */


static int locinfo_query(LOCINFO *lip,cchar *vp,int vl)
{
	PARAMOPT	*pop = &lip->aparams ;
	int		rs ;
	const char	*po = PO_QUERY ;
	if ((rs = paramopt_loads(pop,po,vp,vl)) >= 0) {
	     lip->f.query = TRUE ;
	}
	return rs ;
}
/* end subroutine (locinfo_query) */


/* process the program options */
static int locinfo_procopts(LOCINFO *lip)
{
	PROGINFO	*pip = lip->pip ;
	KEYOPT		*kop = &lip->akopts ;
	KEYOPT_CUR	kcur ;
	int		rs = SR_OK ;
	int		c = 0 ;
	const char	*cp ;

	if ((cp = getourenv(pip->envv,VAROPTS)) != NULL) {
	    rs = keyopt_loads(kop,cp,-1) ;
	}

	if (rs >= 0) {
	    if ((rs = keyopt_curbegin(kop,&kcur)) >= 0) {
	        int	oi ;
	        int	kl, vl ;
	        cchar	*kp, *vp ;

	        while ((kl = keyopt_enumkeys(kop,&kcur,&kp)) >= 0) {

	            if ((oi = matostr(akonames,2,kp,kl)) >= 0) {
	                int	v ;

	                 vl = keyopt_fetch(kop,kp,NULL,&vp) ;

	                switch (oi) {
	                case akoname_quiet:
	                    if (! pip->final.quiet) {
	                        pip->have.quiet = TRUE ;
	                        pip->final.quiet = TRUE ;
	                        pip->f.quiet = TRUE ;
	                        if (vl > 0) {
	                            rs = optbool(vp,vl) ;
	                            pip->f.quiet = (rs > 0) ;
	                        }
	                    }
	                    break ;
	                case akoname_intage:
	                    if (! lip->final.intage) {
	                        lip->have.intage = TRUE ;
	                        lip->final.intage = TRUE ;
	                        if (vl > 0) {
	                            rs = cfdecti(vp,vl,&v) ;
	                            lip->intage = v ;
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

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (locinfo_procopts) */


static int locinfo_intage(LOCINFO *lip,cchar *argval,cchar *intagespec)
{
	int		rs = SR_OK ;
	int		maxage = 0 ;
	if ((rs >= 0) && (intagespec != NULL) && (intagespec[0] != '\0')) {
	    rs = cfdecti(intagespec,-1,&maxage) ;
	}
	if ((rs >= 0) && (maxage == 0) && (argval != NULL)) {
	    rs = cfdecti(argval,-1,&maxage) ;
	}
	lip->intage = maxage ;
	return rs ;
}
/* end subroutine (locinfo_intage) */


static int locinfo_queries(LOCINFO *lip)
{
	PARAMOPT	*pop = &lip->aparams ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	cchar		*po = PO_QUERY ;
	if ((rs = paramopt_countvals(pop,po)) > 0) {
	    PARAMOPT_CUR	cur ;
	    if ((rs = paramopt_curbegin(pop,&cur)) >= 0) {
		int	vl ;
		cchar	*vp ;
	        while (rs >= 0) {
		    vl = paramopt_enumvalues(pop,po,&cur,&vp) ;
		    if (vl == SR_NOTFOUND) break ;
		    rs = vl ;
		    if ((rs >= 0) && (vl > 0)) {
			int	qi ;
			if ((qi = matostr(qopts,2,vp,vl)) >= 0) {
			    c += 1 ;
			    switch (qi) {
			    case qopt_running:
				lip->have.run = TRUE ;
				lip->f.running = TRUE ;
				break ;
			    case qopt_notrunning:
				lip->have.run = TRUE ;
				lip->f.notrunning = TRUE ;
				break ;
			    } /* end switch */
			} /* end if (match) */
		    } /* end if (positive) */
		} /* end while */
		rs1 = paramopt_curend(pop,&cur) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (paramopt-cur) */
	} /* end if (query count) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (locinfo_queries) */


static int locinfo_procage(LOCINFO *lip,struct ustat *sbp)
{
	PROGINFO	*pip = lip->pip ;
	int		rs = SR_OK ;
	int		f_ok = TRUE ;
	if (lip->f.intage && (lip->intage > 0)) {
	    time_t	dt = pip->daytime ;
	    if ((dt - sbp->st_mtime) >= lip->intage) {
		f_ok = FALSE ;
	    }
	}
	return (rs >= 0) ? f_ok : rs ;
}
/* end subroutine (locinfo_procage) */


static int locinfo_pcurbegin(LOCINFO *lip,LOCINFO_PCUR *lpcurp)
{
	PARAMOPT	*pop = &lip->aparams ;
	PARAMOPT_CUR	*pcurp = &lpcurp->cur ;
	int		rs ;
	memset(lpcurp,0,sizeof(PARAMOPT_CUR)) ;
	rs = paramopt_curbegin(pop,pcurp) ;
	return rs ;
}
/* end subroutine (locinfo_pcurbegin) */


static int locinfo_pcurend(LOCINFO *lip,LOCINFO_PCUR *lpcurp)
{
	PARAMOPT	*pop = &lip->aparams ;
	PARAMOPT_CUR	*pcurp = &lpcurp->cur ;
	int		rs ;
	rs = paramopt_curend(pop,pcurp) ;
	memset(lpcurp,0,sizeof(PARAMOPT_CUR)) ;
	return rs ;
}
/* end subroutine (locinfo_procurend) */


static int locinfo_pget(LOCINFO *lip,LOCINFO_PCUR *lpcurp,cchar **rpp)
{
	PARAMOPT	*pop = &lip->aparams ;
	PARAMOPT_CUR	*pcurp = &lpcurp->cur ;
	int		rs = SR_OK ;
	int		rl = 0 ;
	int		vl ;
	cchar		*po = PO_PIDFILE ;
	cchar		*vp ;
	while (rs >= 0) {
	    vl = paramopt_enumvalues(pop,po,pcurp,&vp) ;
	    if (vl == SR_NOTFOUND) break ;
	    rs = vl ;
	    if (rs > 0) {
	        rl = rs ;
		if (rpp != NULL) *rpp = vp ;
	    }
	    if (rl > 0) break ;
	} /* end while */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (locinfo_pget) */



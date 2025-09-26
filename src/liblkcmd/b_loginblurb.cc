/* b_loginblurb */

/* SHELL built-in which provides a blurb for use at pehaps login-time */
/* version %I% last-modified %G% */


#define	CF_DEBUGS	0		/* non-switchable debug print-outs */
#define	CF_DEBUG	0		/* switchable at invocation */
#define	CF_DEBUGMALL	1		/* debug memory-allocations */
#define	CF_DEBUGN	0		/* extra-special debugging */
#define	CF_LOCCHECK	0		/* using |locinfo_check()| */


/* revision history:

	= 2004-03-01, David A­D­ Morano
	This code was originally written.  

*/

/* Copyright © 2004 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Synopsis:

	$ loginblurb [-s <string>]


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
#include	<sys/param.h>		/* also has "FSCALE" */
#include	<sys/loadavg.h>
#include	<climits>
#include	<unistd.h>
#include	<fcntl.h>
#include	<time.h>
#include	<cstdlib>
#include	<cstring>

#include	<usystem.h>
#include	<bits.h>
#include	<keyopt.h>
#include	<field.h>
#include	<sbuf.h>
#include	<tmpx.h>
#include	<sysmemutil.h>
#include	<exitcodes.h>
#include	<localmisc.h>

#include	"shio.h"
#include	"kshlib.h"
#include	"b_loginblurb.h"
#include	"defs.h"


/* local defines */

#define	CVTBUFLEN	100
#define	STRBUFLEN	32

#define	NDEBFNAME	"loginblurb.deb"

#define	LOCINFO		struct locinfo
#define	LOCINFO_FL	struct locinfo_flags


/* external subroutines */

extern int	snwcpy(char *,int,cchar *,int) ;
extern int	sncpy1(char *,int,cchar *) ;
extern int	sncpy3(char *,int,cchar *,cchar *,cchar *) ;
extern int	sfskipwhite(cchar *,int,cchar **) ;
extern int	snfsflags(cchar *,int,ulong) ;
extern int	mkpath2(char *,cchar *,cchar *) ;
extern int	mkpath3(char *,cchar *,cchar *,cchar *) ;
extern int	matstr(cchar **,cchar *,int) ;
extern int	matostr(cchar **,int,cchar *,int) ;
extern int	matpcasestr(cchar **,int,cchar *,int) ;
extern int	cfdeci(cchar *,int,int *) ;
extern int	cfdecti(cchar *,int,int *) ;
extern int	ctdecull(char *,int,ULONG *) ;
extern int	ctdecll(char *,int,LONG *) ;
extern int	optbool(cchar *,int) ;
extern int	optvalue(cchar *,int) ;
extern int	nleadstr(cchar *,cchar *,int) ;
extern int	bufprintf(char *,int,cchar *,...) ;
extern int	getgroupname(char *,int,gid_t) ;
extern int	sbuf_termconseq(SBUF *,int,int,int,int,int) ;
extern int	isdigitlatin(int) ;
extern int	isFailOpen(int) ;
extern int	isNotPresent(int) ;

extern int	printhelp(void *,cchar *,cchar *,cchar *) ;
extern int	proginfo_setpiv(PROGINFO *,cchar *,const struct pivars *) ;

extern int	ndig(double *,int) ;
extern int	ndigmax(double *,int,int) ;

#if	CF_DEBUGS || CF_DEBUG
extern int	debugopen(cchar *) ;
extern int	debugprintf(cchar *,...) ;
extern int	debugclose() ;
extern int	nprintf(cchar *,cchar *,...) ;
extern int	strlinelen(cchar *,int,int) ;
#endif

extern cchar	*getourenv(cchar **,cchar *) ;

extern char	*strwcpy(char *,cchar *,int) ;
extern char	*strwcpyuc(char *,cchar *,int) ;
extern char	*strdcpy1(char *,int,cchar *) ;
extern char	*strdcpy1w(char *,int,cchar *,int) ;
extern char	*timestr_log(time_t,char *) ;
extern char	*timestr_logz(time_t,char *) ;
extern char	*timestr_elapsed(time_t,char *) ;


/* external variables */

extern char	**environ ;		/* definition required by AT&T AST */


/* local structures */

struct locinfo_flags {
	uint		term:1 ;
	uint		ansiterm:1 ;
	uint		tmpx:1 ;
	uint		nusers:1 ;
	uint		nprocs:1 ;
	uint		o_string:1 ;
	uint		o_nodetitle:1 ;
	uint		o_node:1 ;
	uint		o_time:1 ;
	uint		o_users:1 ;
	uint		o_procs:1 ;
	uint		o_mem:1 ;
	uint		o_load:1 ;
} ;

struct locinfo {
	PROGINFO	*pip ;
	LOCINFO_FL	have, init, f, final ;
	TMPX		ut ;
	time_t		ti_check ;
	time_t		ti_tmpx ;
	time_t		ti_nprocs ;
	uint		la[3] ;		/* raw load-averages from kernel */
	int		nusers ;
	int		nprocs ;
	cchar	*string ;
	char		strbuf[STRBUFLEN+1] ;
} ;


/* forward references */

static int	mainsub(int,cchar **,cchar **,void *) noex ;

static int	usage(PROGINFO *) noex ;

static int	procopts(PROGINFO *,KEYOPT *) noex ;
static int	process(PROGINFO *,cchar *) noex ;
static int	procprint(PROGINFO *,SHIO *) noex ;
static int	procbuf(PROGINFO *,char *,int) noex ;

static int	locinfo_start(LOCINFO *,PROGINFO *) noex ;
static int	locinfo_utmpbegin(LOCINFO *) noex ;
static int	locinfo_nusers(LOCINFO *) noex ;
static int	locinfo_nprocs(LOCINFO *) noex ;
static int	locinfo_loadavgs(LOCINFO *) noex ;
static int	locinfo_utmpend(LOCINFO *) noex ;
static int	locinfo_finish(LOCINFO *) noex ;


/* local variables */

enum argopts {
	argopt_root,
	argopt_version,
	argopt_verbose,
	argopt_help,
	argopt_sn,
	argopt_af,
	argopt_ef,
	argopt_of,
	argopt_utf,
	argopt_db,
	argopt_overlast
} ;

static cchar	*argopts[] = {
	"ROOT",
	"VERSION",
	"VERBOSE",
	"HELP",
	"sn",
	"af",
	"ef",
	"of",
	"utf",
	"db",
	"nocache",
	NULL
} ;

static const PIVARS	initvars = {
	VARPROGRAMROOT1,
	VARPROGRAMROOT2,
	VARPROGRAMROOT3,
	PROGRAMROOT,
	VARPRNAME
} ;

static const MAPEX	mapexs[] = {
	{ SR_NOENT, EX_NOUSER },
	{ SR_AGAIN, EX_TEMPFAIL },
	{ SR_DEADLK, EX_TEMPFAIL },
	{ SR_NOLCK, EX_TEMPFAIL },
	{ SR_TXTBSY, EX_TEMPFAIL },
	{ SR_ACCESS, EX_NOPERM },
	{ SR_REMOTE, EX_PROTOCOL },
	{ SR_NOSPC, EX_TEMPFAIL },
	{ SR_INTR, EX_INTR },
	{ SR_EXIT, EX_TERM },
	{ 0, 0 }
} ;

enum progopts {
	progopt_str,
	progopt_date,
	progopt_time,
	progopt_users,
	progopt_procs,
	progopt_mem,
	progopt_load,
	progopt_la,
	progopt_name,
	progopt_nodetitle,
	progopt_node,
	progopt_term,
	progopt_mesg,
	progopt_to,
	progopt_overlast
} ;

static cchar	*progopts[] = {
	"str",
	"date",
	"time",
	"users",
	"procs",
	"mem",
	"load",
	"la",
	"name",
	"nodetitle",
	"node",
	"term",
	"mesg",
	"to",
	NULL
} ;

static cchar	*ansiterms[] = {
	"ansi",
	"sun",
	"screen",
	"vt100",
	"vt101",
	"vt102",
	"vt220",
	"vt230",
	"vt240",
	"vt320",
	"vt330",
	"vt340",
	"vt420",
	"vt430",
	"vt440",
	"vt520",
	"vt530",
	"vt540",
	NULL
} ;


/* exported variables */


/* exported subroutines */

int b_loginblurb(int argc,mainv argv,void *contextp) noex {
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
/* end subroutine (b_loginblurb) */


int p_loginblurb(int argc,cchar *argv[],cchar *envv[],void *contextp)
{
	return mainsub(argc,argv,envv,contextp) ;
}
/* end subroutine (p_loginblurb) */


/* local subroutines */


/* ARGSUSED */
static int mainsub(int argc,cchar *argv[],cchar *envv[],void *contextp)
{
	PROGINFO	pi, *pip = &pi ;
	LOCINFO		li, *lip = &li ;
	ARGINFO		ainfo ;
	BITS		pargs ;
	KEYOPT		akopts ;
	SHIO		errfile ;

#if	(CF_DEBUGS || CF_DEBUG) && CF_DEBUGMALL
	uint		mo_start = 0 ;
#endif

	int		argr, argl, aol, akl, avl, kwi ;
	int		ai, ai_max, ai_pos ;
	int		ai_continue = 1 ;
	int		rs, rs1 ;
	int		n ;
	int		ex = EX_INFO ;
	int		f_optminus, f_optplus, f_optequal ;
	int		f_version = FALSE ;
	int		f_usage = FALSE ;
	int		f_help = FALSE ;

	cchar	*argp, *aop, *akp, *avp ;
	cchar	*argval = NULL ;
	cchar	*pr = NULL ;
	cchar	*sn = NULL ;
	cchar	*afname = NULL ;
	cchar	*efname = NULL ;
	cchar	*ofname = NULL ;
	cchar	*termtype = NULL ;
	cchar	*cp ;


#if	CF_DEBUGS || CF_DEBUG
	if ((cp = getourenv(envv,VARDEBUGFNAME)) != NULL) {
	    rs = debugopen(cp) ;
	    debugprintf("b_loginblurb: starting DFD=%d\n",rs) ;
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
	rs = proginfo_setbanner(pip,cp) ;

/* initialize */

	pip->verboselevel = 1 ;
	pip->to_open = -1 ;
	pip->daytime = time(NULL) ;

	pip->lip = lip ;
	if (rs >= 0) rs = locinfo_start(lip,pip) ;
	if (rs < 0) {
	    ex = EX_OSERR ;
	    goto badlocstart ;
	}

/* start parsing the arguments */

	if (rs >= 0) rs = bits_start(&pargs,0) ;
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
	                        pip->fl.quiet = TRUE ;
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

/* term-type */
	                    case 'T':
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl)
	                                termtype = argp ;
	                        } else
	                            rs = SR_INVALID ;
	                        break ;

/* version */
	                    case 'V':
	                        f_version = TRUE ;
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
	                        break ;

/* string */
	                    case 's':
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl) {
	                                lip->final.o_string = TRUE ;
	                                lip->fl.o_string = TRUE ;
	                                lip->string = argp ;
	                            }
	                        } else
	                            rs = SR_INVALID ;
	                        break ;

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

#if	CF_DEBUGN && CF_DEBUGS
	nprintf(NDEBFNAME,"loginblurb: loop-end rs=%d\n",rs) ;
#endif
#if	CF_DEBUGS
	debugprintf("b_loginblurb: arg-loop-end rs=%d ai=%u\n",rs,ai) ;
#endif

	if (efname == NULL) efname = getourenv(envv,VAREFNAME) ;
	if (efname == NULL) efname = STDFNERR ;
	if ((rs1 = shio_open(&errfile,efname,"wca",0666)) >= 0) {
	    pip->efp = &errfile ;
	    pip->open.errfile = TRUE ;
	    shio_control(&errfile,SHIO_CSETBUFLINE,TRUE) ;
	} else if (! isFailOpen(rs1)) {
	    if (rs >= 0) rs = rs1 ;
	}

	if (rs < 0)
	    goto badarg ;

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    debugprintf("b_loginblurb: debuglevel=%u\n",pip->debuglevel) ;
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
	} /* end if */

	if (f_version || f_help || f_usage)
	    goto retearly ;


	ex = EX_OK ;

/* initialization */

	if ((rs >= 0) && (pip->n == 0) && (argval != NULL)) {
	    rs = optvalue(argval,-1) ;
	    pip->n = rs ;
	}

	rs = procopts(pip,&akopts) ;

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    debugprintf("b_loginblurb: procopts() rs=%d\n",rs) ;
#endif

/* final defaults */

	if (afname == NULL) afname = getourenv(envv,VARAFNAME) ;

	if (termtype == NULL) termtype = getourenv(envv,VARTERM) ;

	if (termtype != NULL) {
	    n = matpcasestr(ansiterms,2,termtype,-1) ;
	    lip->fl.ansiterm = (n >= 0) ;
	} /* end if */

	if ((rs >= 0) && lip->fl.o_string) {
	    if (lip->string == NULL) lip->string = getourenv(envv,VARSTRING) ;
	    if (lip->string == NULL) {
	        const int	gl= GROUPNAMELEN ;
	        char		gn[GROUPNAMELEN+1] ;
	        if ((rs1 = getgroupname(gn,gl,-1)) >= 0) {
	            cchar	**vpp = &pip->groupname ;
	            if ((rs = proginfo_setentry(pip,vpp,gn,rs1)) >= 0) {
	                int	ml = MIN(STRBUFLEN,rs1) ;
	                strwcpyuc(lip->strbuf,pip->groupname,ml) ;
	                lip->string = lip->strbuf ;
	            }
	        } /* end if (groupname) */
	    } /* end if (still needed a string) */
	} /* end if (string) */

	if ((rs >= 0) && (afname != NULL) && (afname[0] == '-')) {
	    rs = SR_INVALID ;
	}

/* continue -> go */

	memset(&ainfo,0,sizeof(ARGINFO)) ;
	ainfo.argc = argc ;
	ainfo.ai = ai ;
	ainfo.argv = argv ;
	ainfo.ai_max = ai_max ;
	ainfo.ai_pos = ai_pos ;
	ainfo.ai_continue = ai_continue ;

	if (rs >= 0) {
	    if ((rs = locinfo_utmpbegin(lip)) >= 0) {
	        {
	            cchar	*ofn = ofname ;
	            rs = process(pip,ofn) ;
	        }
	        rs1 = locinfo_utmpend(lip) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (locinfo-utmp) */
	} else if (ex == EX_OK) {
	    cchar	*pn = pip->progname ;
	    cchar	*fmt = "%s: invalid argument or configuration (%d)\n" ;
	    shio_printf(pip->efp,fmt,pn,rs) ;
	    ex = EX_USAGE ;
	    usage(pip) ;
	}

#if	CF_DEBUGN && CF_DEBUGS
	nprintf(NDEBFNAME,"loginblurb: done ex=%u rs=%d\n",ex,rs) ;
#endif

/* done */
	if ((rs < 0) && (ex == EX_OK)) {
	    switch (rs) {
	    case SR_INVALID:
	        ex = EX_USAGE ;
	        if (! pip->fl.quiet) {
	            shio_printf(pip->efp,"%s: invalid query (%d)\n",
	                pip->progname,rs) ;
	        }
	        break ;
	    case SR_NOENT:
	        ex = EX_CANTCREAT ;
	        break ;
	    case SR_AGAIN:
	        ex = EX_TEMPFAIL ;
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

/* early return thing */
retearly:
	if (pip->debuglevel > 0) {
	    shio_printf(pip->efp,"%s: exiting ex=%u (%d)\n",
	        pip->progname,ex,rs) ;
	}

#if	CF_DEBUG
	debugprintf("b_loginblurb: exiting ex=%u (%d)\n",ex,rs) ;
#endif

	if (pip->efp != NULL) {
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
	    uint	mo ;
	    uc_mallout(&mo) ;
	    debugprintf("b_loginblurb: final mallout=%u\n",(mo-mo_start)) ;
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
	cchar	*pn = pip->progname ;
	cchar	*fmt ;

	fmt = "%s: USAGE> %s [-s <string>] [-of <ofile>]\n" ;
	if (rs >= 0) rs = shio_printf(pip->efp,fmt,pn,pn) ;
	wlen += rs ;

	fmt = "%s:  [-Q] [-D] [-v[=<n>]] [-HELP] [-V]\n" ;
	if (rs >= 0) rs = shio_printf(pip->efp,fmt,pn) ;
	wlen += rs ;

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (usage) */

static int procopts(PROGINFO *pip,KEYOPT *kop) noex {
	LOCINFO		*lip = pip->lip ;
	int		rs = SR_OK ;
	int		c = 0 ;
	cchar		*cp ;

	if ((cp = getourenv(pip->envv,VAROPTS)) != NULL) {
	    rs = keyopt_loads(kop,cp,-1) ;
	}

	if (rs >= 0) {
	    KEYOPT_CUR	cur ;
	    if ((rs = keyopt_curbegin(kop,&cur)) >= 0) {
	        int	v ;
	        int	ki ;
	        int	kl, vl ;
	        cchar	*kp, *vp ;

	        while ((kl = keyopt_enumkeys(kop,&cur,&kp)) >= 0) {

	            if ((ki = matostr(progopts,2,kp,kl)) >= 0) {

	                vl = keyopt_fetch(kop,kp,NULL,&vp) ;

	                switch (ki) {
	            case progopt_str:
	                if (! lip->final.o_string) {
	                    lip->fl.o_string = TRUE ;
	                    if (vl > 0) {
	                        strdcpy1w(lip->strbuf,STRBUFLEN,vp,vl) ;
	                        lip->string = lip->strbuf ;
	                    }
	                }
	                break ;
	            case progopt_date:
	            case progopt_time:
	                lip->fl.o_time = TRUE ;
	                if (vl > 0) {
	                    rs = optbool(vp,vl) ;
	                    lip->fl.o_time = (rs > 0) ;
	                }
	                break ;
	            case progopt_nodetitle:
	                lip->fl.o_nodetitle = TRUE ;
	                if (vl > 0) {
	                    rs = optbool(vp,vl) ;
	                    lip->fl.o_nodetitle = (rs > 0) ;
	                }
	                break ;
	            case progopt_name:
	            case progopt_node:
	                lip->fl.o_node = TRUE ;
	                if (vl > 0) {
	                    rs = optbool(vp,vl) ;
	                    lip->fl.o_node = (rs > 0) ;
	                }
	                break ;
	            case progopt_users:
	                lip->fl.o_users = TRUE ;
	                if (vl > 0) {
	                    rs = optbool(vp,vl) ;
	                    lip->fl.o_users = (rs > 0) ;
	                }
	                break ;
	            case progopt_procs:
	                lip->fl.o_procs = TRUE ;
	                if (vl > 0) {
	                    rs = optbool(vp,vl) ;
	                    lip->fl.o_procs = (rs > 0) ;
	                }
	                break ;
	            case progopt_mem:
	                lip->fl.o_mem = TRUE ;
	                if (vl > 0) {
	                    rs = optbool(vp,vl) ;
	                    lip->fl.o_mem = (rs > 0) ;
	                }
	                break ;
	            case progopt_load:
	            case progopt_la:
	                lip->fl.o_load = TRUE ;
	                if (vl > 0) {
	                    rs = optbool(vp,vl) ;
	                    lip->fl.o_load = (rs > 0) ;
	                }
	                break ;
	            case progopt_term:
	                if (! lip->final.term) {
	                    lip->final.term = TRUE ;
	                    lip->have.term = TRUE ;
	                    lip->fl.term = TRUE ;
	                    if (vl > 0) {
	                        rs = optbool(vp,vl) ;
	                        lip->fl.term = (rs > 0) ;
	                    }
	                }
	                break ;
	            case progopt_to:
	                if (vl > 0) {
	                    rs = cfdecti(vp,vl,&v) ;
	                    pip->to_open = v ;
	                }
	                break ;
		    default:
			rs = SR_INVALID ;
			break ;
	            } /* end switch */

			c += 1 ;
		    } else
	                rs = SR_INVALID ;

	            if (rs < 0) break ;
	        } /* end while (enumerating) */

	        keyopt_curend(kop,&cur) ;
	    } /* end if (keyopt-cur) */
	} /* end if (ok) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (procopts) */


static int process(PROGINFO *pip,cchar *ofn)
{
	SHIO		ofile, *ofp = &ofile ;
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	cchar		*pn = pip->progname ;
	cchar		*fmt ;

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    debugprintf("b_loginblurb/process: ent ofn=%s\n",ofn) ;
#endif

	if ((ofn == NULL) || (ofn[0] == '\0') || (ofn[0] == '-'))
	    ofn = STDFNOUT ;

	if ((rs = shio_opene(ofp,ofn,"wct",0666,pip->to_open)) >= 0) {
	    {
	        rs = procprint(pip,ofp) ;
		wlen += rs ;
	    }
	    rs1 = shio_close(ofp) ;
	    if (rs >= 0) rs = rs1 ;
	} else {
	    fmt = "%s: inaccessible output (%d)\n" ;
	    shio_printf(pip->efp,fmt,pn,rs) ;
	    shio_printf(pip->efp,"%s: ofile=%s\n",pn,ofn) ;
	}

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    debugprintf("b_loginblurb/process: ret rs=%d wlen=%u\n",rs,wlen) ;
#endif

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (process) */


/* print the time to the console */
static int procprint(PROGINFO *pip,SHIO *ofp)
{
	const int	llen = LINEBUFLEN ;
	int		rs ;
	int		wlen = 0 ;
	char		lbuf[LINEBUFLEN + 1] ;

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    debugprintf("b_loginblurb/procprint: ent\n") ;
#endif

	if ((rs = procbuf(pip,lbuf,llen)) >= 0) {
	    wlen = rs ;
	    rs = shio_write(ofp,lbuf,wlen) ;
	}

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    debugprintf("b_loginblurb/procprint: ret rs=%d\n",rs) ;
#endif

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (procprint) */


static int procbuf(PROGINFO *pip,char *lbuf,int llen)
{
	LOCINFO		*lip = pip->lip ;
	SBUF		b ;
	int		rs ;
	int		c = 0 ;
	int		wlen = 0 ;
	int		f_terminal ;
	f_terminal = lip->fl.term ;
	if ((rs = sbuf_start(&b,lbuf,llen)) >= 0) {

	    if (f_terminal)
	        sbuf_chr(&b,'\r') ;

/* time */

	    if (lip->fl.o_string && (lip->string != NULL)) {
	        c += 1 ;
	        sbuf_strw(&b,lip->string,-1) ;
	    } /* end if (option-string) */

	    if (lip->fl.o_time) {
	        char	timebuf[TIMEBUFLEN + 1] ;
	        timestr_logz(pip->daytime,timebuf) ;
	        if (c++ > 0) sbuf_chr(&b,' ') ;
	        sbuf_strw(&b,timebuf,23) ;
	    } /* end if (option-time) */

/* node-name */

	    if ((rs >= 0) && lip->fl.o_node) {
	        if ((rs = proginfo_nodename(pip)) >= 0) {
	            if (c++ > 0) sbuf_chr(&b,' ') ;
	            if (lip->fl.o_nodetitle)
	                sbuf_strw(&b,"node=",-1) ;
	            sbuf_strw(&b,pip->nodename,-1) ;
	        }
	    } /* end if (option-nodename) */

/* users (number of logged-in users) */

	    if ((rs >= 0) && lip->fl.o_users) {
	        if ((rs = locinfo_nusers(lip)) >= 0) {
	            if (c++ > 0) sbuf_chr(&b,' ') ;
	            sbuf_strw(&b,"users=",-1) ;
	            sbuf_decui(&b,lip->nusers) ;
	        }
	    } /* end if (option-users) */

/* number of processes */

	    if ((rs >= 0) && lip->fl.o_procs) {
	        if ((rs = locinfo_nprocs(lip)) >= 0) {
	            if (c++ > 0) sbuf_chr(&b,' ') ;
	            sbuf_strw(&b,"procs=",-1) ;
	            sbuf_decui(&b,lip->nprocs) ;
	        }
	    } /* end if (option-processes) */

/* memory usage */

	    if ((rs >= 0) && lip->fl.o_mem) {
	        if ((rs = sysmemutil(NULL)) >= 0) {
	            int	percent = rs ;
	            if (c++ > 0) sbuf_chr(&b,' ') ;
	            sbuf_strw(&b,"mem=",-1) ;
	            sbuf_decui(&b,percent) ;
	            sbuf_chr(&b,'%') ;
		} else if (rs == SR_NOSYS) {
		    rs = SR_OK ;
	        } /* end if */
	    } /* end if (memory usage) */

/* load averages */

	    if ((rs >= 0) && lip->fl.o_load) {
	        if ((rs = locinfo_loadavgs(lip)) >= 0) {
	            double	dla[3] ;
	            cchar	*fmt ;
	            for (int i = 0 ; i < 3 ; i += 1) {
	                dla[i] = ((double) lip->la[i]) / FSCALE ;
	            }
	            fmt = "la=(%4.1f %4.1f %4.1f)" ;
	            if (ndig(dla,3) > 2) {
	                fmt = "la=(%5.1f %5.1f %5.1f)" ;
	                ndigmax(dla,3,3) ;
	            }
	            if (c++ > 0) sbuf_chr(&b,' ') ;
	            sbuf_printf(&b,fmt,dla[0],dla[1],dla[2]) ;
	        } /* end if */
	    } /* end if (o-load) */

/* done */

	    if (f_terminal && lip->fl.ansiterm)
	        sbuf_termconseq(&b,'K',-1,-1,-1,-1) ;

	    if (f_terminal)
	        sbuf_chr(&b,'\r') ;

	    sbuf_chr(&b,'\n') ;

	    wlen = sbuf_finish(&b) ;
	    if (rs >= 0) rs = wlen ;
	} /* end if (sbuf) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (procbuf) */

static int locinfo_start(LOCINFO *lip,PROGINFO *pip) noex {
	int		rs = SR_FAULT ;
	if (lip) {
	    rs = memclear(lip) ;
	    lip->pip = pip ;
	    lip->fl.o_string = TRUE ;
	    lip->fl.o_time = FALSE ;
	    lip->fl.o_node = TRUE ;
	    lip->fl.o_users = TRUE ;
	    lip->fl.o_procs = TRUE ;
	    lip->fl.o_mem = TRUE ;
	    lip->fl.o_load = TRUE ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (locinfo_start) */

static int locinfo_finish(LOCINFO *lip) noex {
	int		rs = SR_OK ;
	if (lip == NULL) return SR_FAULT ;
	return rs ;
}
/* end subroutine (locinfo_finish) */

static int locinfo_utmpbegin(LOCINFO *lip) noex {
	int		rs = SR_OK ;
	if (lip == NULL) return SR_FAULT ;
	return rs ;
}
/* end subroutine (locinfo_utmpbegin) */

static int locinfo_loadavgs(LOCINFO *lip) noex {
	return u_getloadavg(lip->la,3) ;
}
/* end subroutine (Locinfo_loadavgs) */

static int locinfo_nprocs(LOCINFO *lip) noex {
	PROGINFO	*pip = lip->pip ;
	const int	to = 5 ; /* "get" time-out? */
	int		rs = SR_OK ;
	int		nproc = lip->nprocs ;

	if ((pip->daytime - lip->ti_nprocs) >= to) {
	    lip->ti_nprocs = pip->daytime ;
	    if ((rs = uc_nprocs(0)) >= 0) {
	        lip->nprocs = rs ;
	        nproc = rs ;
	    } else if (isNotPresent(rs) || (rs == SR_NOSYS)) {
	        rs = SR_OK ;
	    }
	} /* end if (time-out) */

	return (rs >= 0) ? nproc : rs ;
}
/* end subroutine (locinfo_nprocs) */


static int locinfo_nusers(LOCINFO *lip)
{
	PROGINFO	*pip = lip->pip ;
	int		rs = SR_OK ;
	int		nu = 0 ;

	if (! lip->fl.tmpx) {
	    rs = tmpx_open(&lip->ut,NULL,O_RDONLY) ;
	    lip->fl.tmpx = (rs >= 0) ;
	}

	if (rs >= 0) {
	    if ((pip->daytime - lip->ti_tmpx) >= TO_TMPX) {
	        lip->ti_tmpx = pip->daytime ;
	        rs = tmpx_nusers(&lip->ut) ;
	        nu = rs ;
	        lip->nusers = nu ;
	    }
	}

	return (rs >= 0) ? nu : rs ;
}
/* end subroutine (locinfo_nusers) */


#if	CF_LOCCHECK
static int locinfo_check(LOCINFO *lip)
{
	PROGINFO	*pip = lip->pip ;
	int		rs = SR_OK ;

	if ((rs >= 0) && lip->fl.tmpx) {
	    rs = tmpx_check(&lip->ut,pip->daytime) ;
	}

	return rs ;
}
/* end subroutine (locinfo_check) */
#endif /* CF_LOCCHECK */


static int locinfo_utmpend(LOCINFO *lip)
{
	int		rs = SR_OK ;
	int		rs1 ;

	if (lip == NULL) return SR_FAULT ;

	if (lip->fl.tmpx) {
	    lip->fl.tmpx = FALSE ;
	    rs1 = tmpx_close(&lip->ut) ;
	    if (rs >= 0) rs = rs1 ;
	}

	return rs ;
}
/* end subroutine (locinfo_utmpend) */



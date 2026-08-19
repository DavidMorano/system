/* b_mjd SUPPORT (KSH builtin) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* this is a SHELL built-in version of |mjd(1)| */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* switchable at invocation */
#define	CF_DEBUGMALL	1		/* debug memory allocation */
#define	CF_BUFLINEIN	1		/* line-buffering for STDIN */
#define	CF_READINTR	0		/* |shio_readintr(3shio)| */
#define	CF_SPECIALFIFO	0		/* employ special FIFO handling */

/* revision history:

	= 2004-03-01, David A­D­ Morano
	This code was originally written as a KSH built-in
	command.

*/

/* Copyright © 2004 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	b_mjd

	Synopsis:
	$ mjd [<day(s)>] [-af <afile>] [-V]

	Arguments:
	<day(s)>	quote for this day (default today): <mon><mday>
	-af <afile>	argument file of <day(s)>
	-V		print command version to standard-error and then exit

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
#include	<climits>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<bits.h>
#include	<keyopt.h>
#include	<vecstr.h>
#include	<tmtime.hh>
#include	<dayspec.h>
#include	<ourmjd.h>
#include	<exitcodes.h>		/* CSTD */
#include	<localmisc.h>		/* CSTD */

#include	"shio.h"
#include	"kshlib.h"
#include	"b_mjd.h"
#include	"defs.h"


/* local defines */

#ifndef	LINEBUFLEN
#ifdef	LINE_MAX
#define	LINEBUFLEN	MAX(2048,LINE_MAX)
#else
#define	LINEBUFLEN	2048
#endif
#endif /* LINEBUFLEN */

#ifndef	ABUFLEN
#ifdef	ALIASNAMELEN
#define	ABUFLEN		ALIASNAMELEN
#else
#define	ABUFLEN		64
#endif
#endif

#ifndef	VBUFLEN
#ifdef	MAILADDRLEN
#define	VBUFLEN		MAILADDRLEN
#else
#define	VBUFLEN		2048
#endif
#endif

#ifndef	NYEARS_CENTURY
#define	NYEARS_CENTURY	100
#endif

#define	LOCINFO		struct locinfo
#define	LOCINFO_FL	struct locinfo_flags


/* external subroutines */

extern int	printhelp(void *,cchar *,cchar *,cchar *) ;
extern int	proginfo_setpiv(PROGINFO *,cchar *,const struct pivars *) ;

#if	CF_DEBUG || CF_DEBUG
extern int	debugopen(const char *) ;
extern int	debugprintf(const char *,...) ;
extern int	debugprinthex(const char *,int,const char *,int) ;
extern int	debugclose() ;
extern int	strlinelen(const char *,int,int) ;
#endif


/* external variables */

extern char	**environ ;		/* definition required by AT&T AST */


/* local structures */

struct locinfo_flags {
	uint		stores:1 ;
	uint		cvtcase:1 ;
	uint		cvtuc:1 ;
	uint		cvtlc:1 ;
	uint		termout:1 ;
	uint		outer:1 ;
	uint		curdate:1 ;
	uint		curspec:1 ;
	uint		mjd:1 ;
	uint		gmt:1 ;
	uint		year:1 ;
	uint		ttl:1 ;
	uint		expire:1 ;
	uint		del:1 ;
	uint		intrun:1 ;
	uint		dgram:1 ;
} ;

struct locinfo {
	LOCINFO_FL	have, f, changed, finval ;
	LOCINFO_FL	open ;
	TMTIME		curdate ;
	dayspec		curspec ;
	vecstr		stores ;
	PROGINFO	*pip ;
	const char	*termtype ;
	const char	*qfname ;
	int		to ;
	int		year ;
	int		ttl ;
	int		intrun ;
} ;


/* forward references */

local int	mainsub(int,cchar **,cchar **,void *) ;

local int	usage(PROGINFO *) ;

local int	procopts(PROGINFO *,keyopt *) ;
local int	procargs(PROGINFO *,ARGINFO *,bits *,cchar *,cchar *) ;
local int	procquery(PROGINFO *,void *,const char *,int) ;

local int	locinfo_start(LOCINFO *,PROGINFO *) ;
local int	locinfo_finish(LOCINFO *) ;
local int	locinfo_setentry(LOCINFO *,cchar **,cchar *,int) ;
local int	locinfo_getdefyear(LOCINFO *) ;
local int	locinfo_qfname(LOCINFO *,cchar *) ;
local int	locinfo_defspec(LOCINFO *,dayspec *) ;
local int	locinfo_curspec(LOCINFO *) ;
local int	locinfo_getcentury(LOCINFO *) ;
local int	locinfo_curdate(LOCINFO *) ;


/* local variables */

static const char	*argopts[] = {
	"ROOT",
	"VERSION",
	"VERBOSE",
	"HELP",
	"sn",
	"af",
	"ef",
	"of",
	"if",
	"to",
	"tr",
	"dgram",
	NULL
} ;

enum argopts {
	argopt_root,
	argopt_version,
	argopt_verbose,
	argopt_help,
	argopt_sn,
	argopt_af,
	argopt_ef,
	argopt_of,
	argopt_if,
	argopt_to,
	argopt_tr,
	argopt_dgram,
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

static const char	*akonames[] = {
	"bufwhole",
	"bufline",
	"bufnone",
	"whole",
	"line",
	"none",
	"un",
	"termout",
	"gmt",
	"ttl",
	"intrun",
	"",
	NULL
} ;

enum akonames {
	akoname_bufwhole,
	akoname_bufline,
	akoname_bufnone,
	akoname_whole,
	akoname_line,
	akoname_none,
	akoname_un,
	akoname_termout,
	akoname_gmt,
	akoname_ttl,
	akoname_intrun,
	akoname_empty,
	akoname_overlast
} ;


/* exported subroutines */


int b_mjd(int argc,cchar *argv[],void *contextp)
{
	int		rs ;
	int		rs1 ;
	int		ex = EX_OK ;

	if ((rs = lib_kshbegin(contextp,NULL)) >= 0) {
	    const char	**envv = (const char **) environ ;
	    ex = mainsub(argc,argv,envv,contextp) ;
	    rs1 = lib_kshend() ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (ksh) */

	if ((rs < 0) && (ex == EX_OK)) ex = EX_DATAERR ;

	return ex ;
}
/* end subroutine (b_mjd) */


int p_mjd(int argc,cchar *argv[],cchar *envv[],void *contextp)
{
	return mainsub(argc,argv,envv,contextp) ;
}
/* end subroutine (p_mjd) */


/* local subroutines */


/* ARGSUSED */
local int mainsub(int argc,cchar *argv[],cchar *envv[],void *contextp)
{
	PROGINFO	pi, *pip = &pi ;
	LOCINFO		li, *lip = &li ;
	ARGINFO		ainfo ;
	bits		pargs ;
	keyopt		akopts ;
	SHIO		errfile ;

#if	(CF_DEBUG || CF_DEBUG) && CF_DEBUGMALL
	uint		mo_start = 0 ;
#endif

	int		argr, argl, aol, akl, avl, kwi ;
	int		ai, ai_max, ai_pos ;
	int		rs, rs1 ;
	int		v ;
	int		wlen = 0 ;
	int		ex = EX_INFO ;
	int		f_optminus, f_optplus, f_optequal ;
	int		f_version = false ;
	int		f_usage = false ;
	int		f_help = false ;

	const char	*argp, *aop, *akp, *avp ;
	const char	*argval = NULL ;
	const char	*pr = NULL ;
	const char	*sn = NULL ;
	const char	*afname = NULL ;
	const char	*ofname = NULL ;
	const char	*efname = NULL ;
	const char	*qfname = NULL ;
	const char	*tos_open = NULL ;
	const char	*tos_read = NULL ;
	const char	*cp ;


#if	CF_DEBUG || CF_DEBUG
	if ((cp = getourenv(envv,VARDEBUGFNAME)) != NULL) {
	    rs = debugopen(cp) ;
	    debugprintf("b_mjd: starting DFD=%u\n",rs) ;
	}
#endif /* CF_DEBUG */

#if	(CF_DEBUG || CF_DEBUG) && CF_DEBUGMALL
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
	pip->daytime = time(NULL) ;
	pip->to_open = -1 ;
	pip->to_read = -1 ;

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
	            f_optequal = false ;
	            if ((avp = strchr(aop,'=')) != NULL) {
	                f_optequal = true ;
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

/* version */
	                case argopt_version:
	                    f_version = true ;
	                    if (f_optequal)
	                        rs = SR_INVALID ;
	                    break ;

/* verbose mode */
	                case argopt_verbose:
	                    pip->verboselevel = 2 ;
	                    if (f_optequal) {
	                        f_optequal = false ;
	                        if (avl) {
	                            rs = optvalue(avp,avl) ;
	                            pip->verboselevel = rs ;
	                        }
	                    }
	                    break ;

/* program root */
	                case argopt_root:
	                    if (f_optequal) {
	                        f_optequal = false ;
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

/* open time-out */
	                case argopt_to:
	                    if (f_optequal) {
	                        f_optequal = false ;
	                        if (avl)
	                            tos_open = avp ;
	                    } else {
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl)
	                                tos_open = argp ;
	                        } else
	                            rs = SR_INVALID ;
	                    }
	                    break ;

/* read time-out */
	                case argopt_tr:
	                    if (f_optequal) {
	                        f_optequal = false ;
	                        if (avl)
	                            tos_read = avp ;
	                    } else {
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl)
	                                tos_read = argp ;
	                        } else
	                            rs = SR_INVALID ;
	                    }
	                    break ;

/* data-gram mode */
	                case argopt_dgram:
	                    lip->finval.dgram = true ;
	                    lip->fl.dgram = true ;
	                    if (f_optequal) {
	                        f_optequal = false ;
	                        if (avl) {
	                            rs = cfdecti(avp,avl,&v) ;
	                            lip->intrun = v ;
	                        }
	                    }
	                    break ;

	                case argopt_help:
	                    f_help = true ;
	                    break ;

/* program search-name */
	                case argopt_sn:
	                    if (f_optequal) {
	                        f_optequal = false ;
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

/* argument-list file */
	                case argopt_af:
	                    if (f_optequal) {
	                        f_optequal = false ;
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
	                        f_optequal = false ;
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
	                        f_optequal = false ;
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

	                case argopt_if:
	                    if (f_optequal) {
	                        f_optequal = false ;
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
	                            f_optequal = false ;
	                            if (avl) {
	                                rs = optvalue(avp,avl) ;
	                                pip->debuglevel = rs ;
	                            }
	                        }
	                        break ;

/* quiet mode */
	                    case 'Q':
	                        pip->fl.quiet = true ;
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

/* terminal-type */
	                    case 'T':
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl)
	                                lip->termtype = argp ;
	                        } else
	                            rs = SR_INVALID ;
	                        break ;

/* version */
	                    case 'V':
	                        f_version = true ;
	                        break ;

/* expiration maintenance */
	                    case 'e':
	                        lip->fl.expire = true ;
	                        if (f_optequal) {
	                            f_optequal = false ;
	                            if (avl) {
	                                rs = cfdecti(avp,avl,&v) ;
	                                lip->ttl = v ;
	                            }
	                        }
	                        break ;

	                    case 'l':
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl)
	                                qfname = argp ;
	                        } else
	                            rs = SR_INVALID ;
	                        break ;

/* specify that queries are MJDs */
	                    case 'm':
	                        lip->fl.mjd = true ;
	                        if (f_optequal) {
	                            f_optequal = false ;
	                            if (avl) {
	                                rs = optbool(avp,avl) ;
	                                lip->fl.mjd = (rs > 0) ;
	                            }
	                        }
	                        break ;

/* options */
	                    case 'o':
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl) {
					keyopt	*kop = &akopts ;
	                                rs = keyopt_loads(kop,argp,argl) ;
				    }
	                        } else
	                            rs = SR_INVALID ;
	                        break ;

	                    case 'q':
	                        pip->verboselevel = 0 ;
	                        break ;

	                    case 'r':
	                        lip->fl.del = true ;
	                        break ;

	                    case 't':
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl) {
	                                tos_open = argp ;
	                                tos_read = argp ;
	                            }
	                        } else
	                            rs = SR_INVALID ;
	                        break ;

/* line-buffered */
	                    case 'u':
	                        pip->have.bufnone = true ;
	                        pip->fl.bufnone = true ;
	                        pip->finval.bufnone = true ;
	                        break ;

/* verbose mode */
	                    case 'v':
	                        pip->verboselevel = 2 ;
	                        if (f_optequal) {
	                            f_optequal = false ;
	                            if (avl) {
	                                rs = optvalue(avp,avl) ;
	                                pip->verboselevel = rs ;
	                            }
	                        }
	                        break ;

/* default year */
	                    case 'y':
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl) {
				        rs = optvalue(argp,argl) ;
	    				lip->year = rs ;
	                            }
	                        } else
	                            rs = SR_INVALID ;
	                        break ;

	                    case 'z':
	                        lip->finval.gmt = true ;
	                        lip->have.gmt = true ;
	                        lip->fl.gmt = true ;
	                        if (f_optequal) {
	                            f_optequal = false ;
	                            if (avl) {
	                                rs = optbool(avp,avl) ;
	                                lip->fl.gmt = (rs > 0) ;
	                            }
	                        }
	                        break ;

	                    case '?':
	                        f_usage = true ;
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
	    pip->open.errfile = true ;
	    shio_control(&errfile,SHIO_CSETBUFLINE,true) ;
	} else if (! isFailOpen(rs1)) {
	    if (rs >= 0) rs = rs1 ;
	}

	if (rs < 0)
	    goto badarg ;

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    debugprintf("b_mjd: debuglevel=%u\n",pip->debuglevel) ;
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
	}

	if (f_version || f_help || f_usage)
	    goto retearly ;


	ex = EX_OK ;

/* some initialization */

	if ((rs >= 0) && (pip->n == 0) && (argval != NULL)) {
	    rs = optvalue(argval,-1) ;
	    pip->n = rs ;
	}

	if (rs >= 0) {
	    rs = procopts(pip,&akopts) ;
	}

	if (afname == NULL) afname = getourenv(envv,VARAFNAME) ;

#ifdef	COMMENT
	if (pip->tmpdname == NULL) pip->tmpdname = getourenv(envv,VARTMPDNAME) ;
	if (pip->tmpdname == NULL) pip->tmpdname = TMPDNAME ;
#endif

	pip->to_open = -1 ;
	pip->to_read = -1 ;
	if ((rs >= 0) && (tos_open != NULL)) {
	    rs = cfdecti(tos_open,-1,&v) ;
	    pip->to_open = v ;
	}

	if ((rs >= 0) && (tos_read != NULL)) {
	    rs = cfdecti(tos_read,-1,&v) ;
	    pip->to_read = v ;
	}

	if ((rs >= 0) && (qfname != NULL)) {
	    rs = locinfo_qfname(lip,qfname) ;
	}

	if (pip->debuglevel > 0) {
	    cchar	*pn = pip->progname ;
	    shio_printf(pip->efp,"%s: gmt=%u\n",pn,lip->fl.gmt) ;
	    if ((pip->to_open >= 0) || (pip->to_read >= 0)) {
	        shio_printf(pip->efp,"%s: to_open=%d\n",pn,pip->to_open) ;
	        shio_printf(pip->efp,"%s: to_read=%d\n",pn,pip->to_read) ;
	    }
	}

#if	CF_DEBUG
	if (DEBUGLEVEL(2)) {
	    debugprintf("b_mjd: to_open=%d\n",pip->to_open) ;
	    debugprintf("b_mjd: to_read=%d\n",pip->to_read) ;
	    debugprintf("b_mjd: f_bufline=%u\n",pip->fl.bufline) ;
	    debugprintf("b_mjd: f_bufnone=%u\n",pip->fl.bufnone) ;
	}
#endif /* CF_DEBUG */

	if (pip->to_open == 0)
	    pip->to_open = 1 ;

	if (pip->to_read == 0)
	    pip->to_read = 1 ;

/* go */

	memset(&ainfo,0,sizeof(ARGINFO)) ;
	ainfo.argc = argc ;
	ainfo.ai = ai ;
	ainfo.argv = argv ;
	ainfo.ai_max = ai_max ;
	ainfo.ai_pos = ai_pos ;

	if (rs >= 0) {
	    cchar	*ofn = ofname ;
	    cchar	*afn = afname ;
	    rs = procargs(pip,&ainfo,&pargs,ofn,afn) ;
	    wlen = rs ;
	} else if (ex == EX_OK) {
	    cchar	*pn = pip->progname ;
	    cchar	*fmt = "%s: invalid argument or configuration (%d)\n" ;
	    shio_printf(pip->efp,fmt,pn,rs) ;
	    ex = EX_USAGE ;
	    usage(pip) ;
	}

	if ((pip->debuglevel > 0) && (rs >= 0)) {
	    shio_printf(pip->efp,"%s: bytes=%u\n", pip->progname,wlen) ;
	}

/* done */
	if ((rs < 0) && (ex == EX_OK)) {
	    if (! pip->fl.quiet) {
	        cchar	*pn = pip->progname ;
	        cchar	*fmt = "%s: could not process file (%d)\n" ;
	        shio_printf(pip->efp,fmt,pn,rs) ;
	    }
	    ex = mapex(mapexs,rs) ;
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
	if (DEBUGLEVEL(4))
	    debugprintf("b_mjd: exiting ex=%u (%d)\n",ex,rs) ;
#endif

	if (pip->efp != NULL) {
	    pip->open.errfile = false ;
	    shio_close(pip->efp) ;
	    pip->efp = NULL ;
	}

	if (pip->open.akopts) {
	    pip->open.akopts = false ;
	    keyopt_finish(&akopts) ;
	}

	bits_finish(&pargs) ;

badpargs:
	locinfo_finish(lip) ;

badlocstart:
	proginfo_finish(pip) ;

badprogstart:

#if	(CF_DEBUG || CF_DEBUG) && CF_DEBUGMALL
	{
	    uint	mo ;
	    uc_mallout(&mo) ;
	    debugprintf("b_mjd: final mallout=%u\n",mo-mo_start) ;
	    uc_mallset(0) ;
	}
#endif

#if	(CF_DEBUG || CF_DEBUG)
	debugclose() ;
#endif

	return ex ;

/* the bad things */
badarg:
	ex = EX_USAGE ;
	shio_printf(pip->efp,
	    "%s: invalid argument specified (%d)\n",
	    pip->progname,rs) ;
	usage(pip) ;
	goto retearly ;

}
/* end subroutine (mainsub) */


local int usage(PROGINFO *pip)
{
	int		rs = SR_OK ;
	int		wlen = 0 ;
	const char	*pn = pip->progname ;
	const char	*fmt ;

	fmt = "%s: USAGE> %s [<day(s)>] [-af <afile>]\n" ;
	if (rs >= 0) rs = shio_printf(pip->efp,fmt,pn,pn) ;
	wlen += rs ;

	fmt = "%s:  [-of <ofile>] [-y <year>]\n" ;
	if (rs >= 0) rs = shio_printf(pip->efp,fmt,pn) ;
	wlen += rs ;

	fmt = "%s:  [-to <to_open>] [-tr <to_read>]\n" ;
	if (rs >= 0) rs = shio_printf(pip->efp,fmt,pn) ;
	wlen += rs ;

	fmt = "%s:  [-Q] [-D] [-v[=<n>]] [-HELP] [-V]\n" ;
	if (rs >= 0) rs = shio_printf(pip->efp,fmt,pn) ;
	wlen += rs ;

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (usage) */


/* process the program ako-options */
local int procopts(PROGINFO *pip,keyopt *kop)
{
	LOCINFO		*lip = pip->lip ;
	int		rs = SR_OK ;
	int		c = 0 ;
	const char	*cp ;

	if ((cp = getourenv(pip->envv,VAROPTS)) != NULL) {
	    rs = keyopt_loads(kop,cp,-1) ;
	}

	if (rs >= 0) {
	    keyopt_cur	kcur ;
	    if ((rs = keyopt_curbegin(kop,&kcur)) >= 0) {
	        int	oi ;
	        int	kl, vl ;
	        cchar	*kp, *vp ;

	        while ((kl = keyopt_curenumkeys(kop,&kcur,&kp)) >= 0) {

	            if ((oi = matostr(akonames,2,kp,kl)) >= 0) {

	                vl = keyopt_fetch(kop,kp,NULL,&vp) ;

	                switch (oi) {
	                case akoname_bufwhole:
	                case akoname_whole:
	                    if (! pip->finval.bufwhole) {
	                        pip->have.bufwhole = true ;
	                        pip->finval.bufwhole = true ;
	                        pip->fl.bufwhole = true ;
	                        if (vl > 0) {
	                            rs = optbool(vp,vl) ;
	                            pip->fl.bufwhole = (rs > 0) ;
	                        }
	                    }
	                    break ;
	                case akoname_bufline:
	                case akoname_line:
	                    if (! pip->finval.bufline) {
	                        pip->have.bufline = true ;
	                        pip->finval.bufline = true ;
	                        pip->fl.bufline = true ;
	                        if (vl > 0) {
	                            rs = optbool(vp,vl) ;
	                            pip->fl.bufline = (rs > 0) ;
	                        }
	                    }
	                    break ;
	                case akoname_bufnone:
	                case akoname_none:
	                case akoname_un:
	                    if (! pip->finval.bufnone) {
	                        pip->have.bufnone = true ;
	                        pip->finval.bufnone = true ;
	                        pip->fl.bufnone = true ;
	                        if (vl > 0) {
	                            rs = optbool(vp,vl) ;
	                            pip->fl.bufnone = (rs > 0) ;
	                        }
	                    }
	                    break ;
	                case akoname_termout:
	                    if (! lip->finval.termout) {
	                        lip->have.termout = true ;
	                        lip->finval.termout = true ;
	                        lip->fl.termout = true ;
	                        if (vl > 0) {
	                            rs = optbool(vp,vl) ;
	                            lip->fl.termout = (rs > 0) ;
	                        }
	                    }
	                    break ;
	                case akoname_gmt:
	                    if (! lip->finval.gmt) {
	                        lip->have.gmt = true ;
	                        lip->finval.gmt = true ;
	                        lip->fl.gmt = true ;
	                        if (vl > 0) {
	                            rs = optbool(vp,vl) ;
	                            lip->fl.gmt = (rs > 0) ;
	                        }
	                    }
	                    break ;
	                case akoname_ttl:
	                    if (lip->ttl < 0) {
	                        if (vl > 0) {
	                            rs = optvalue(vp,vl) ;
	                            lip->ttl = rs ;
	                        }
	                    }
	                    break ;
	                case akoname_intrun:
	                    if (lip->intrun < 0) {
	                        if (vl > 0) {
	                            int	v ;
	                            rs = cfdecti(vp,vl,&v) ;
	                            lip->intrun = v ;
	                        }
	                    }
	                    break ;
	                case akoname_empty:
	                    break ;
	                default:
	                    rs = SR_INVALID ;
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
/* end subroutine (procopts) */


local int procargs(PROGINFO *pip,ARGINFO *aip,bits *bop,cchar *afn,cchar *ofn)
{
	SHIO		ofile, *ofp = &ofile ;
	const int	to_open = pip->to_open ;
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	cchar		*pn = pip->progname ;
	cchar		*fmt ;

	if ((ofn == NULL) || (ofn[0] == '\0') || (ofn[0] == '-'))
	    ofn = STDFNOUT ;

	if ((rs = shio_opene(ofp,ofn,"wct",0666,to_open)) >= 0) {
	    int		pan = 0 ;
	    int		cl ;
	    const char	*cp ;

	    if (pip->have.bufnone)
	        shio_control(ofp,SHIO_CSETBUFNONE,true) ;

	    if (pip->have.bufline)
	        shio_control(ofp,SHIO_CSETBUFLINE,pip->fl.bufline) ;

	    if (pip->have.bufwhole)
	        shio_control(ofp,SHIO_CSETBUFWHOLE,pip->fl.bufwhole) ;

/* go through the loops */

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
	                    rs = procquery(pip,ofp,cp,-1) ;
	                    wlen += rs ;
	                }
	            }

	            if (rs >= 0) rs = lib_sigterm() ;
	            if (rs >= 0) rs = lib_sigintr() ;
	            if (rs < 0) break ;
	        } /* end for */
	    } /* end if */

	    if ((rs >= 0) && (afn != NULL) && (afn[0] != '\0')) {
	        SHIO	afile, *afp = &afile ;

	        if (strcmp(afn,"-") == 0) afn = STDFNIN ;

	        if ((rs = shio_open(&afile,afn,"r",0666)) >= 0) {
	            const int	llen = LINEBUFLEN ;
	            char	lbuf[LINEBUFLEN + 1] ;

	            while ((rs = shio_readline(afp,lbuf,llen)) > 0) {
	                int	len = rs ;

	                if (lbuf[len - 1] == '\n') len -= 1 ;
	                lbuf[len] = '\0' ;

	                if ((cl = sfskipwhite(lbuf,len,&cp)) > 0) {
	                    if (cp[0] != '#') {
	                        pan += 1 ;
	                        rs = procquery(pip,ofp,cp,cl) ;
	                        wlen += rs ;
	                    }
	                }

	                if (rs >= 0) rs = lib_sigterm() ;
	                if (rs >= 0) rs = lib_sigintr() ;
	                if (rs < 0) break ;
	            } /* end while (reading lines) */

	            rs1 = shio_close(afp) ;
	            if (rs >= 0) rs = rs1 ;
	        } else {
	            fmt = "%s: inaccessible argument-list (%d)\n" ;
	            shio_printf(pip->efp,fmt,pn,rs) ;
	            shio_printf(pip->efp,"%s: afile=%s\n",pn,afn) ;
	        } /* end if */

	    } /* end if (procesing file argument file list) */

	    if ((rs >= 0) && (pan == 0)) {

	        cp = "-" ;
	        pan += 1 ;
	        rs = procquery(pip,ofp,cp,-1) ;
	        wlen += rs ;

	    } /* end if (standard-input) */

	    rs1 = shio_close(ofp) ;
	    if (rs >= 0) rs = rs1 ;
	} else {
	    fmt = "%s: inaccessible output (%d)\n" ;
	    shio_printf(pip->efp,fmt,pn,rs) ;
	    shio_printf(pip->efp,"%s: ofile=%s\n",pn,ofn) ;
	}

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (procargs) */


local int procquery(PROGINFO *pip,void *ofp,cchar qp[],int ql)
{
	LOCINFO		*lip = pip->lip ;
	int		rs = SR_OK ;
	int		mjd ;
	int		wlen = 0 ;

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("b_mjd/procquery: q=%s\n",qp) ;
#endif

	if (qp == NULL) return SR_FAULT ;

	if (ql < 0) ql = strlen(qp) ;

	if (lip->fl.mjd && hasalldig(qp,ql)) {
	    if (uint uv ; (rs = cfdecui(qp,ql,&uv)) >= 0) {
	        mjd = (int) uv ;
	    }
	} else if ((rs = ourmjd(qp,ql)) > 0) {
	    mjd = rs ;
	} else {
	    dayspec	ds ;
	    if ((qp[0] == '+') || (qp[0] == '-')) {
	        rs = dayspec_def(&ds) ;
	    } else {
	        rs = dayspec_load(&ds,qp,-1) ;
	    }
	    if (rs >= 0) {
	        if ((rs = locinfo_defspec(lip,&ds)) >= 0) {
		    if (ds.y >= TMTIME_YEARBASE) {
	                rs = getmjd(ds.y,ds.m,ds.d) ;
	                mjd = rs ;
		    } else {
			rs = SR_DOM ;
		    }
	        } /* end if (locinfo_defspec) */
	    } /* end if (ok) */
	} /* end if */

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("b_mjd/procquery: mid rs=%d mjd=%u\n",rs,mjd) ;
#endif

	if (rs >= 0) {
	    rs = shio_printf(ofp,"%u\n",mjd) ;
	    wlen += rs ;
	} /* end if */

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (procquery) */


local int locinfo_start(LOCINFO *lip,PROGINFO *pip)
{
	int		rs = SR_OK ;
	const char	*varterm = VARTERM ;

	if (lip == NULL) return SR_FAULT ;

	memset(lip,0,sizeof(LOCINFO)) ;
	lip->pip = pip ;
	lip->to = -1 ;
	lip->ttl = -1 ;
	lip->intrun = -1 ;
	lip->termtype = getourenv(pip->envv,varterm) ;

	return rs ;
}
/* end subroutine (locinfo_start) */


local int locinfo_finish(LOCINFO *lip)
{
	int		rs = SR_OK ;
	int		rs1 ;

	if (lip == NULL) return SR_FAULT ;

	if (lip->open.stores) {
	    lip->open.stores = false ;
	    rs1 = vecstr_finish(&lip->stores) ;
	    if (rs >= 0) rs = rs1 ;
	}

	return rs ;
}
/* end subroutine (locinfo_finish) */


local int locinfo_setentry(LOCINFO *lip,cchar **epp,cchar *vp,int vl)
{
	VECSTR		*slp ;
	int		rs = SR_OK ;
	int		len = 0 ;

	if (lip == NULL) return SR_FAULT ;
	if (epp == NULL) return SR_FAULT ;

	slp = &lip->stores ;
	if (! lip->open.stores) {
	    rs = vecstr_start(slp,4,0) ;
	    lip->open.stores = (rs >= 0) ;
	}

	if (rs >= 0) {
	    int	oi = -1 ;
	    if (*epp != NULL) {
		oi = vecstr_findaddr(slp,*epp) ;
	    }
	    if (vp != NULL) {
	        len = strnlen(vp,vl) ;
	        rs = vecstr_store(slp,vp,len,epp) ;
	    } else {
	        *epp = NULL ;
	    }
	    if ((rs >= 0) && (oi >= 0)) {
	        vecstr_del(slp,oi) ;
	    }
	} /* end if (ok) */

	return (rs >= 0) ? len : rs ;
}
/* end subroutine (locinfo_setentry) */


local int locinfo_getdefyear(LOCINFO *lip)
{
	int		rs = SR_OK ;
	if (lip->fl.year) {
	    lip->fl.year = true ;
	    if (lip->year > 0) {
	        if (lip->year < NYEARS_CENTURY) {
	            if ((rs = locinfo_getcentury(lip)) >= 0) {
		        lip->year += (rs*NYEARS_CENTURY) ;
	            }
		}
	    }
	} else {
	    rs = (lip->year >= 0) ? lip->year : SR_OK ;
	}
	return rs ;
}
/* end subroutine (locinfo_getdefyear) */


local int locinfo_qfname(LOCINFO *lip,const char *qfname)
{
	int		rs = SR_OK ;

	if (qfname != NULL) {
	    cchar	**vpp = &lip->qfname ;
	    rs = locinfo_setentry(lip,vpp,qfname,-1) ;
	}

	return rs ;
} /* end subroutine (locinfo_qfname) */

local int locinfo_defspec(LOCINFO *lip,dayspec *dsp) noex {
	int		rs = SR_OK ;
	int		f = false ;

	f = f || (dsp->y < 0) || (dsp->y < NYEARS_CENTURY) ;
	f = f || (dsp->m < 0) ;
	f = f || (dsp->d < 0) ;
	if (f) {
	    if ((rs = locinfo_curspec(lip)) >= 0) {
	        if (dsp->y < 0) {
		    if ((rs = locinfo_getdefyear(lip)) > 0) {
			dsp->y = lip->year ;
		    } else {
			dsp->y = lip->curspec.y ;
		    }
		} else if (dsp->y < NYEARS_CENTURY) {
		    if ((rs = locinfo_getcentury(lip)) >= 0) {
		        dsp->y += (rs*NYEARS_CENTURY) ;
		    }
		}
	        if (dsp->m < 0) dsp->m = lip->curspec.m ;
	        if (dsp->d < 0) dsp->d = lip->curspec.d ;
	    } /* end if (locinfo_curspec) */
	} /* end if (needed) */

	return rs ;
} /* end subroutine (locinfo_defspec) */

local int locinfo_curspec(LOCINFO *lip) noex {
	int		rs = SR_OK ;
	if (! lip->fl.curspec) {
	    lip->fl.curspec = true ;
	    if ((rs = locinfo_curdate(lip)) >= 0) {
	        lip->curspec.y = (lip->curdate.year + TMTIME_YEARBASE) ;
	        lip->curspec.m = lip->curdate.mon ;
	        lip->curspec.d = lip->curdate.mday ;
	    }
	}
	return rs ;
} /* end subroutine (locinfo_curspec) */

local int locinfo_getcentury(LOCINFO *lip) noex {
	int		rs ;
	if ((rs = locinfo_curdate(lip)) >= 0) {
	    rs = ((lip->curdate.year + TMTIME_YEARBASE) / NYEARS_CENTURY) ;
	}
	return rs ;
} /* end subroutine (locinfo_getcentury) */

local int locinfo_curdate(LOCINFO *lip) noex {
	PROGINFO	*pip = lip->pip ;
	int		rs = SR_OK ;
	if (! lip->fl.curdate) {
	    TMTIME	*ctp = &lip->curdate ;
	    lip->fl.curdate = true ;
	    if (lip->fl.gmt) {
	        rs = tmtime_timegm(ctp,pip->daytime) ;
	    } else {
	        rs = tmtime_timelocal(ctp,pip->daytime) ;
	    }
	} /* end if */
	return rs ;
}
/* end subroutine (locinfo_curdate) */



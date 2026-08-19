/* b_qotd SUPPORT (KSH builtin) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* this is a SHELL built-in version of |qotd(1ksh)| */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* switchable at invocation */
#define	CF_DEBUGMALL	1		/* debug memory allocation */
#define	CF_BUFLINEIN	1		/* line-buffering for STDIN */

/* revision history:

	= 2004-03-01, David A­D­ Morano
	This code was originally written as a KSH built-in command.

*/

/* Copyright © 2004 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	b_qotd

	Synopsis:
	$ qotd [<day>|<mjd> -m] [-af <afile>] [-r] [-e] [-l <qfile>] [-V]

	Arguments:
	<day>		quote for this day (default today): <mon><mday>
	-af <afile>	argument file of <day(s)>
	-m		the day(s) are MJDs in decimal
	-r		flush locallly cached quote-of-the-day
	-e		request expiration maintenance of local cache
	-l <qfile>	load file into local cache
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
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>		/* CSYD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<bits.h>
#include	<keyopt.h>
#include	<vecstr.h>
#include	<userinfo.h>
#include	<tmtime.hh>
#include	<dayspec.h>
#include	<openqotd.h>
#include	<filer.h>
#include	<termout.h>
#include	<ourmjd.h>
#include	<exitcodes.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"shio.h"
#include	"kshlib.h"
#include	"msgdata.h"
#include	"proglog.h"
#include	"b_qotd.h"
#include	"defs.h"


/* local defines */

#ifndef	LINEBUFLEN
#ifdef	LINE_MAX
#define	LINEBUFLEN	MAX(2048,LINE_MAX)
#else
#define	LINEBUFLEN	2048
#endif
#endif /* LINEBUFLEN */

#ifndef	QBUFLEN
#define	QBUFLEN		LINEBUFLEN
#endif

#define	LOCINFO		struct locinfo
#define	LOCINFO_FL	struct locinfo_flags


/* external subroutines */


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
	uint		mjd:1 ;
	uint		gmt:1 ;
	uint		year:1 ;
	uint		ttl:1 ;
	uint		expire:1 ;
	uint		del:1 ;
	uint		intrun:1 ;
	uint		separate:1 ;
	uint		dgram:1 ;
	uint		rate:1 ;
} ;

struct locinfo {
	LOCINFO_FL	have, f, changed, finval ;
	LOCINFO_FL	open ;
	TERMOUT		outer ;
	DAYSPEC		ds ;
	vecstr		stores ;
	PROGINFO	*pip ;
	cchar		*termtype ;
	cchar		*qfname ;
	cchar		*hostspec ;
	int		year ;
	int		ttl ;
	int		intrun ;
	int		af ;
	int		rate ;
} ;


/* forward references */

local int	mainsub(int,cchar **,cchar **,void *) ;

local int	usage(PROGINFO *) ;

local int	procopts(PROGINFO *,keyopt *) ;
local int	procdgram(PROGINFO *,int) ;
local int	procdgramer(PROGINFO *,MSGDATA *) ;
local int	procargs(PROGINFO *,ARGINFO *,bits *,cchar *,cchar *) ;
local int	procquery(PROGINFO *,void *,cchar *,int) ;
local int	procqueryload(PROGINFO *,int) ;
local int	procqueryout(PROGINFO *,void *,int) ;
local int	procqueryout_remote(PROGINFO *,void *,int) ;
local int	procqueryout_local(PROGINFO *,void *,int) ;
local int	procopenquery(PROGINFO *,int) ;
local int	procopenqueryhost(PROGINFO *,int) ;
local int	procqueryouter(PROGINFO *,void *,int) ;
local int	procquerytermout(PROGINFO *,void *,int) ;

local int	procuserinfo_begin(PROGINFO *,USERINFO *) ;
local int	procuserinfo_end(PROGINFO *) ;
local int	procuserinfo_logid(PROGINFO *) ;

local int	locinfo_start(LOCINFO *,PROGINFO *) ;
local int	locinfo_finish(LOCINFO *) ;
local int	locinfo_qfname(LOCINFO *,cchar *) ;
local int	locinfo_termoutbegin(LOCINFO *,void *) ;
local int	locinfo_termoutend(LOCINFO *) ;
local int	locinfo_termoutprint(LOCINFO *,void *,cchar *,int) ;
local int	locinfo_defspec(LOCINFO *,DAYSPEC *) ;
local int	locinfo_curdate(LOCINFO *) ;
local int	locinfo_setentry(LOCINFO *,cchar **,cchar *,int) ;
local int	locinfo_netparse(LOCINFO *,cchar *,int) ;
local int	locinfo_mjd(LOCINFO *) ;


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
	"to",
	"tr",
	"dgram",
	"rate",
	nullptr
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
	argopt_to,
	argopt_tr,
	argopt_dgram,
	argopt_rate,
	argopt_overlast
} ;

static const PIVARS		initvars = {
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
	"separate",
	"",
	nullptr
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
	akoname_separate,
	akoname_empty,
	akoname_overlast
} ;


/* exported subroutines */


int b_qotd(int argc,cchar *argv[],void *contextp)
{
	int		rs ;
	int		rs1 ;
	int		ex = EX_OK ;

	if ((rs = lib_kshbegin(contextp,nullptr)) >= 0) {
	    cchar	**envv = (cchar **) environ ;
	    ex = mainsub(argc,argv,envv,contextp) ;
	    rs1 = lib_kshend() ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (ksh) */

	if ((rs < 0) && (ex == EX_OK)) ex = EX_DATAERR ;

	return ex ;
}
/* end subroutine (b_qotd) */


int p_qotd(int argc,cchar *argv[],cchar *envv[],void *contextp)
{
	return mainsub(argc,argv,envv,contextp) ;
}
/* end subroutine (p_qotd) */


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

	cchar		*argp, *aop, *akp, *avp ;
	cchar		*argval = nullptr ;
	cchar		*pr = nullptr ;
	cchar		*sn = nullptr ;
	cchar		*afname = nullptr ;
	cchar		*ofname = nullptr ;
	cchar		*efname = nullptr ;
	cchar		*qfname = nullptr ;
	cchar		*tos_open = nullptr ;
	cchar		*tos_read = nullptr ;
	cchar		*cp ;


#if	CF_DEBUG || CF_DEBUG
	if ((cp = getourenv(envv,VARDEBUGFNAME)) != nullptr) {
	    rs = debugopen(cp) ;
	    debugprintf("b_qotd: starting DFD=%u\n",rs) ;
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

	if ((cp = getourenv(envv,VARBANNER)) == nullptr) cp = BANNER ;
	rs = proginfo_setbanner(pip,cp) ;

/* initialize */

	pip->verboselevel = 1 ;
	pip->daytime = time(nullptr) ;
	pip->fl.logprog = OPT_LOGPROG ;

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
	for (ai = 0 ; (ai < argc) && (argv[ai] != nullptr) ; ai += 1) {
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
	            if ((avp = strchr(aop,'=')) != nullptr) {
	                f_optequal = true ;
	                akl = avp - aop ;
	                avp += 1 ;
	                avl = aop + argl - 1 - avp ;
	                aol = akl ;
	            } else {
	                avp = nullptr ;
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

/* time-out */
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

/* rate control mode */
	                case argopt_rate:
	                    lip->finval.rate = true ;
	                    lip->have.rate = true ;
	                    if (f_optequal) {
	                        f_optequal = false ;
	                        if (avl) {
	                            rs = cfdecti(avp,avl,&v) ;
	                            lip->rate = v ;
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

/* query a specified host */
	                    case 'h':
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl) {
	                                lip->hostspec = argp ;
				    }
	                        } else
	                            rs = SR_INVALID ;
	                        break ;

	                    case 'i':
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl) {
	                                rs = cfdecti(argp,argl,&v) ;
					lip->intrun = v ;
				    }
	                        } else
	                            rs = SR_INVALID ;
	                        break ;

/* specify that queries are MJDs */
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
	                                rs = cfdecti(argp,argl,&v) ;
	                                pip->to = v ;
	                            }
	                        } else {
	                            rs = SR_INVALID ;
				}
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

/* use GMT */
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

	if (efname == nullptr) efname = getourenv(envv,VAREFNAME) ;
	if (efname == nullptr) efname = STDFNERR ;
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
	    debugprintf("b_qotd: debuglevel=%u\n",pip->debuglevel) ;
#endif

	if (f_version) {
	    shio_printf(pip->efp,"%s: version %s\n",
	        pip->progname,VERSION) ;
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
	    printhelp(nullptr,pip->pr,pip->searchname,HELPFNAME) ;
#endif
	}

	if (f_version || f_help || f_usage)
	    goto retearly ;


	ex = EX_OK ;

/* some initialization */

	if ((rs >= 0) && (pip->n == 0) && (argval != nullptr)) {
	    rs = optvalue(argval,-1) ;
	    pip->n = rs ;
	}

	if (rs >= 0) {
	    rs = procopts(pip,&akopts) ;
	}

	if (afname == nullptr) afname = getourenv(envv,VARAFNAME) ;

#ifdef	COMMENT
	if (pip->tmpdname == nullptr) pip->tmpdname = getourenv(envv,VARTMPDNAME) ;
	if (pip->tmpdname == nullptr) pip->tmpdname = TMPDNAME ;
#endif

	if ((rs >= 0) && (lip->intrun == 0)) {
	    if ((cp = getourenv(envv,VARINTRUN)) != nullptr) {
		rs = cfdecti(cp,-1,&v) ;
		lip->intrun = v ;
	    }
	}

	if (lip->intrun == 0) lip->intrun = INT_RUN ;

	if (pip->to <= 0) pip->to = TO_RECVMSG ;

	if ((rs >= 0) && (pip->to_open == 0) && (tos_open != nullptr)) {
	    rs = cfdecti(tos_open,-1,&v) ;
	    pip->to_open = v ;
	}

	if ((rs >= 0) && (pip->to_read == 0) && (tos_read != nullptr)) {
	    rs = cfdecti(tos_read,-1,&v) ;
	    pip->to_read = v ;
	}

	if (pip->to_open == 0) pip->to_open = TO_OPEN ;

	if (pip->to_read == 0) pip->to_read = TO_READ ;

	if (pip->debuglevel > 0) {
	    cchar	*pn = pip->progname ;
	    cchar	*fmt ;
	    if ((pip->to_open >= 0) || (pip->to_read >= 0)) {
		fmt = "%s: to_open=%d\n" ;
	        shio_printf(pip->efp,fmt,pn,pip->to_open) ;
		fmt = "%s: to_read=%d\n" ;
	        shio_printf(pip->efp,fmt,pn,pip->to_read) ;
	    }
	}

	if ((rs >= 0) && (qfname != nullptr)) {
	    rs = locinfo_qfname(lip,qfname) ;
	}

#if	CF_DEBUG
	if (DEBUGLEVEL(2)) {
	    debugprintf("b_qotd: to_open=%d\n",pip->to_open) ;
	    debugprintf("b_qotd: to_read=%d\n",pip->to_read) ;
	    debugprintf("b_qotd: f_bufline=%u\n",pip->fl.bufline) ;
	    debugprintf("b_qotd: f_bufnone=%u\n",pip->fl.bufnone) ;
	}
#endif /* CF_DEBUG */

/* go */

	memset(&ainfo,0,sizeof(ARGINFO)) ;
	ainfo.argc = argc ;
	ainfo.ai = ai ;
	ainfo.argv = argv ;
	ainfo.ai_max = ai_max ;
	ainfo.ai_pos = ai_pos ;

	if (rs >= 0) {
	    USERINFO	u ;
	    if ((rs = userinfo_start(&u,nullptr)) >= 0) {
	        if ((rs = procuserinfo_begin(pip,&u)) >= 0) {
	 	    if ((rs = proglog_begin(pip,&u)) >= 0) {
	    	        if (lip->fl.dgram) {
	    	            const int	nfd = FD_STDIN ;
			    rs = procdgram(pip,nfd) ;
			    wlen = rs ;
	    	        } else {
	    	            cchar	*ofn = ofname ;
	    	            cchar	*afn = afname ;
			    rs = procargs(pip,&ainfo,&pargs,ofn,afn) ;
			    wlen = rs ;
	    	        }
	 		rs1 = proglog_end(pip) ;
	  		if (rs >= 0) rs = rs1 ;
	  	    } /* end if (proglog) */
		    rs1 = procuserinfo_end(pip) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (procuserinfo) */
	        rs1 = userinfo_finish(&u) ;
	        if (rs >= 0) rs = rs1 ;
	    } else {
	        cchar	*pn = pip->progname ;
	        cchar	*fmt = "%s: userinfo failure (%d)\n" ;
	        ex = EX_NOUSER ;
	        shio_printf(pip->efp,fmt,pn,rs) ;
	    } /* end if (userinfo) */
	} else if (ex == EX_OK) {
	    cchar	*pn = pip->progname ;
	    cchar	*fmt = "%s: invalid argument or configuration (%d)\n" ;
	    shio_printf(pip->efp,fmt,pn,rs) ;
	    ex = EX_USAGE ;
	    usage(pip) ;
	} /* end if (ok) */

	if ((rs >= 0) && (pip->debuglevel > 0)) {
	    shio_printf(pip->efp,"%s: bytes=%u\n",
	        pip->progname,wlen) ;
	}

/* done */
	if ((rs < 0) && (ex == EX_OK)) {
	    if (! pip->fl.quiet) {
	        cchar	*pn = pip->progname ;
	        cchar	*fmt = "%s: could not process (%d)\n" ;
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
	    debugprintf("b_qotd: exiting ex=%u (%d)\n",ex,rs) ;
#endif

	if (pip->efp != nullptr) {
	    pip->open.errfile = false ;
	    shio_close(pip->efp) ;
	    pip->efp = nullptr ;
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
	    debugprintf("b_qotd: final mallout=%u\n",mo-mo_start) ;
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
	cchar		*pn = pip->progname ;
	cchar		*fmt ;

	fmt = "%s: USAGE> %s [<day(s)>|-m <mjd(s)>] [-af <afile>] [-r]\n" ;
	if (rs >= 0) rs = shio_printf(pip->efp,fmt,pn,pn) ;
	wlen += rs ;

	fmt = "%s:  [-of <ofile>] [-e[=<ttl>]] [-l <qfile>] [-y <year>]\n" ;
	if (rs >= 0) rs = shio_printf(pip->efp,fmt,pn) ;
	wlen += rs ;

	fmt = "%s:  [-to <to_open>] [-tr <to_read>] [-h <host>]:<port>]\n" ;
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
	cchar		*cp ;

	if ((cp = getourenv(pip->envv,VAROPTS)) != nullptr) {
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

	                vl = keyopt_fetch(kop,kp,nullptr,&vp) ;

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
	                case akoname_separate:
	                    if (! lip->finval.separate) {
	                        lip->have.separate = true ;
	                        lip->finval.separate = true ;
	                        lip->fl.separate = true ;
	                        if (vl > 0) {
	                            rs = optbool(vp,vl) ;
	                            lip->fl.separate = (rs > 0) ;
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


/* this is a UDP server (it eventually times out) */
local int procdgram(PROGINFO *pip,int nfd)
{
	LOCINFO		*lip = pip->lip ;
	MSGDATA		m ;
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	int		c = 0 ;
	if ((rs = msgdata_init(&m,0)) >= 0) {
	    const int	to = lip->intrun ;
	    while ((rs = msgdata_recvto(&m,nfd,to)) >= 0) {
	        if ((rs = procdgramer(pip,&m)) >= 0) {
		    if (lip->rate > 0) sleep(lip->rate) ;
		    c += 1 ;
	            rs = msgdata_send(&m,nfd,rs,0) ;
	            wlen += rs ;
	        }
	        if (rs >= 0) rs = lib_sigterm() ;
	        if (rs >= 0) rs = lib_sigintr() ;
	        if (rs < 0) break ;
	    } /* end while */
	    rs1 = msgdata_fini(&m) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (msgdata) */
	proglog_printf(pip,"reverse c=%u wl=%u",c,wlen) ;
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (procdgram) */


local int procdgramer(PROGINFO *pip,MSGDATA *mip)
{
	LOCINFO		*lip = pip->lip ;
	int		rs ;
	int		rs1 ;
	int		tlen = 0 ;
	int		mjd = 0 ;
	char		*mbuf ;
	if ((rs = msgdata_getdata(mip,&mbuf)) > 0) {
	    int		ql = rs ;
	    cchar	*tp ;
	    cchar	*qp  = mbuf ;
	    if ((tp = strnchr(qp,ql,'\n')) != nullptr) {
	        ql = (tp-qp) ;
	    }
	    while (ql && (qp[ql-1] == '\0')) ql -= 1 ;
	    if ((rs = locinfo_netparse(lip,qp,ql)) >= 0) {
	        mjd = rs ;
	    } else if (isNotValid(rs)) {
	        rs = SR_OK ;
	    }
	} /* end if (msgdata_getdata) */
	if ((rs >= 0) && (mjd == 0)) {
	    rs = locinfo_mjd(lip) ;
	    mjd = rs ;
	}
	if (rs >= 0) {
	    const int	of = O_RDONLY ;
	    const int	to_open = pip->to_open ;
	    if (pip->debuglevel > 0) {
	        cchar	*pn = pip->progname ;
	        cchar	*fmt = "%s: mjd=%u\n" ;
	        shio_printf(pip->efp,fmt,pn,mjd) ;
	    }
	    proglog_printf(pip,"mjd=%u",mjd) ;
	    if ((rs = openqotd(pip->pr,mjd,of,to_open)) >= 0) {
	        const int	qfd = rs ;
		if ((rs = msgdata_getbuf(mip,&mbuf)) >= 0) {
		    int		ml = rs ;
		    char	*mp = mbuf ;
	            while ((ml > 0) && ((rs = u_read(qfd,mp,ml)) > 0)) {
	                int	rlen = rs ;
			tlen += rlen ;
	                mp += rlen ;
	                ml -= rlen ;
	            } /* end while (reading) */
		    msgdata_setdatalen(mip,tlen) ;
		} /* end if (msgdata) */
	        rs1 = u_close(qfd) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (openqotd) */
	} /* end if (ok) */
	return (rs >= 0) ? tlen : rs ;
}
/* end subroutine (procdgramer) */


local int procargs(PROGINFO *pip,ARGINFO *aip,bits *bop,cchar *afn,cchar *ofn)
{
	LOCINFO		*lip = pip->lip ;
	SHIO		ofile, *ofp = &ofile ;
	const int	to_open = pip->to_open ;
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	cchar		*pn = pip->progname ;
	cchar		*fmt ;

	if ((ofn == nullptr) || (ofn[0] == '\0') || (ofn[0] == '-'))
	    ofn = STDFNOUT ;

	if ((rs = shio_opene(ofp,ofn,"wct",0666,to_open)) >= 0) {
	    int	pan = 0 ;

	    if (pip->have.bufnone)
	        shio_control(ofp,SHIO_CSETBUFNONE,true) ;

	    if (pip->have.bufline)
	        shio_control(ofp,SHIO_CSETBUFLINE,pip->fl.bufline) ;

	    if (pip->have.bufwhole)
	        shio_control(ofp,SHIO_CSETBUFWHOLE,pip->fl.bufwhole) ;

/* go through the loops */

	    if ((rs = locinfo_termoutbegin(lip,ofp)) >= 0) {
	        int	cl ;
	        cchar	*cp ;

	        if (rs >= 0) {
	            const int	argc = aip->argc ;
	            int		ai ;
	            int		f ;
	            cchar	**argv = aip->argv ;
	            for (ai = 1 ; ai < argc ; ai += 1) {

	                f = (ai <= aip->ai_max) && (bits_test(bop,ai) > 0) ;
	                f = f || ((ai > aip->ai_pos) && (argv[ai] != nullptr)) ;
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
	        } /* end if (ok) */

	        if ((rs >= 0) && (afn != nullptr) && (afn[0] != '\0')) {
	            SHIO	afile, *afp = &afile ;

	            if (strcmp(afn,"-") == 0) afn = STDFNIN ;

	            if ((rs = shio_open(&afile,afn,"r",0666)) >= 0) {
	                const int	llen = LINEBUFLEN ;
	                char		lbuf[LINEBUFLEN + 1] ;

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
	                fmt = "%s: inaccessible argument-list (%d)\n",
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

	        rs1 = locinfo_termoutend(lip) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (termout) */

	    rs1 = shio_close(ofp) ;
	    if (rs >= 0) rs = rs1 ;
	} else {
	    fmt = "%s: inaccessible output (%d)\n" ;
	    shio_printf(pip->efp,fmt,pn,rs) ;
	    shio_printf(pip->efp,"%s: ofile=%s\n",pn,ofn) ;
	}
	proglog_printf(pip,"forward wl=%u",wlen) ;

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
	    debugprintf("b_qotd/procquery: q=%s\n",qp) ;
#endif

	if (qp == nullptr)
	    return SR_FAULT ;

	if (ql < 0) ql = strlen(qp) ;

	if (lip->fl.mjd && hasalldig(qp,ql)) {
	    if (uint uv ; (rs = cfdecui(qp,ql,&uv)) >= 0) {
	        mjd = (int) uv ;
	    }
	} else if ((rs = ourmjd(qp,ql)) > 0) {
	    mjd = rs ;
	} else {
	    DAYSPEC	ds ;
	    if ((qp[0] == '+') || (qp[0] == '-')) {
	        rs = dayspec_def(&ds) ;
	    } else {
	        rs = dayspec_load(&ds,qp,ql) ;
	    }
	    if (rs >= 0) {
	        if ((rs = locinfo_defspec(lip,&ds)) >= 0) {
	            rs = getmjd(ds.y,ds.m,ds.d) ;
	            mjd = rs ;
	        }
	    }
	} /* end if */

	if (rs >= 0) {
	    if (pip->debuglevel > 0) {
	        cchar	*pn = pip->progname ;
	        cchar	*fmt = "%s: mjd=%u\n" ;
	        shio_printf(pip->efp,fmt,pn,mjd) ;
	    }
	    if (lip->qfname != nullptr) {
	        rs = procqueryload(pip,mjd) ;
	    } else {
	        const int	n = MAX(pip->n,1) ;
	        int		i ;
	        for (i = 0 ; i < n ; i += 1) {
	            if (lip->fl.separate && (i > 0)) {
	                char	obuf[2] = { '÷', 0 } ;
	                rs = shio_print(ofp,obuf,1) ;
	                wlen += rs ;
	            }
	            if (rs >= 0) {
	                rs = procqueryout(pip,ofp,mjd) ;
	                wlen += rs ;
	                mjd += 1 ;
	            }
	            if (rs < 0) break ;
	        } /* end for */
	    } /* end if (program mode) */
	} /* end if (ok) */

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (procquery) */


local int procqueryload(PROGINFO *pip,int mjd)
{
	LOCINFO		*lip = pip->lip ;
	const int	to_open = pip->to_open ;
	const int	of = (O_CREAT|O_WRONLY|O_TRUNC) ;
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;

	if ((rs = openqotd(pip->pr,mjd,of,to_open)) >= 0) {
	    SHIO	ifile, *ifp = &ifile ;
	    const int	qfd = rs ;
	    cchar	*qfname = lip->qfname ;

	    if (qfname[0] == '-') qfname = STDFNIN ;

	    if ((rs = shio_open(ifp,qfname,"r",0666)) >= 0) {
	        const int	llen = LINEBUFLEN ;
	        char		lbuf[LINEBUFLEN+1] ;

	        while ((rs = shio_read(ifp,lbuf,llen)) > 0) {
	            int	len = rs ;

	            rs = u_write(qfd,lbuf,len) ;
	            wlen += rs ;

	            if (rs < 0) break ;
	        } /* end while */

	        rs1 = shio_close(ifp) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (input-file) */

	    rs1 = u_close(qfd) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (quote-file) */

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (procqueryload) */


local int procqueryout(PROGINFO *pip,void *ofp,int mjd)
{
	LOCINFO		*lip = pip->lip ;
	int		rs ;
	int		wlen = 0 ;

	if (lip->hostspec != nullptr) {
	    rs = procqueryout_remote(pip,ofp,mjd) ;
	    wlen += rs ;
	} else {
	    rs = procqueryout_local(pip,ofp,mjd) ;
	    wlen += rs ;
	}

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (procqueryout) */


local int procqueryout_remote(PROGINFO *pip,void *ofp,int mjd)
{
	LOCINFO		*lip = pip->lip ;
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("b_qotd/procqueryout_remote: ent mjd=%d\n",mjd) ;
#endif

	if ((rs = procopenquery(pip,mjd)) >= 0) {
	    const int	to = pip->to_read ;
	    const int	qlen = QBUFLEN ;
	    const int	qfd = rs ;
	    char	qbuf[QBUFLEN+1] ;
#if	CF_DEBUG
	    if (DEBUGLEVEL(4))
	        debugprintf("b_qotd/procqueryout_remote: uc_recve()\n") ;
#endif
	    if ((rs = uc_recve(qfd,qbuf,qlen,0,to,0)) >= 0) {
		const int	ql = rs ;
	        if (pip->verboselevel > 0) {
	            if (lip->open.outer) {
	                if (ql > 0) {
	                    rs = locinfo_termoutprint(lip,ofp,qbuf,ql) ;
	                    wlen += rs ;
	                } else {
	                    rs = shio_print(ofp,qbuf,ql) ;
	                    wlen += rs ;
	                }
		    } else {
	    		rs = shio_write(ofp,qbuf,ql) ;
	    		wlen += rs ;
		    }
		}
	    } /* end if (uc_recve) */
	    rs1 = u_close(qfd) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (procopenquery) */

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("b_qotd/procqueryout_remote: ret rs=%d wlen=%u\n",
		rs,wlen) ;
#endif

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (procqueryout_remote) */


local int procqueryout_local(PROGINFO *pip,void *ofp,int mjd)
{
	LOCINFO		*lip = pip->lip ;
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("b_qotd/procqueryout_local: ent mjd=%d\n",mjd) ;
#endif

	if ((rs = procopenquery(pip,mjd)) >= 0) {
	    const int	qfd = rs ;

	    if (pip->verboselevel > 0) {
	        if (lip->open.outer) {
	            rs = procquerytermout(pip,ofp,qfd) ;
	            wlen = rs ;
	        } else {
	            rs = procqueryouter(pip,ofp,qfd) ;
	            wlen = rs ;
	        }
	    } /* end if (verbose enough) */

	    rs1 = u_close(qfd) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (open) */

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (procqueryout_local) */


local int procopenquery(PROGINFO *pip,int mjd)
{
	LOCINFO		*lip = pip->lip ;
	int		rs ;
	int		fd = -1 ;

	if (lip->hostspec != nullptr) {
	    rs = procopenqueryhost(pip,mjd) ;
	    fd = rs ;
	} else {
	    int		to_open = pip->to_open ;
	    int		of = O_RDONLY ;
	    if (lip->fl.expire) {
	        of |= O_EXCL ;
	        to_open = lip->ttl ;
	    }
	    if (lip->fl.del) {
	        of |= O_NOCTTY ;
	    }
	    rs = openqotd(pip->pr,mjd,of,to_open) ;
	    fd = rs ;
	} /* end if (local or remote) */

	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (procopenquery) */


local int procopenqueryhost(PROGINFO *pip,int mjd)
{
	LOCINFO		*lip = pip->lip ;
	const int	hlen = MAXHOSTNAMELEN ;
	int		rs = SR_OK ;
	int		fd = -1 ;
	cchar		*tp ;
	cchar		*hs, *ps ;
	char		hbuf[MAXHOSTNAMELEN+1] ;
	if ((tp = strchr(lip->hostspec,':')) != nullptr) {
	    hs = hbuf ;
	    strdcpy1w(hbuf,hlen,lip->hostspec,(tp-lip->hostspec)) ;
	    ps = (tp+1) ;
	} else {
	    hs = lip->hostspec ;
	    ps = PORTSPEC_QUOTE ;
	}
	if (hs[0] != '\0') {
	    const int	to_open = pip->to_open ;
	    const int	af = lip->af ;
	    if ((rs = dialudp(hs,ps,af,to_open,0)) >= 0) {
	        fd = rs ;
	        if ((rs = ctdeci(hbuf,hlen,mjd)) >= 0) {
		    rs = u_send(fd,hbuf,rs,0) ;
		    if (rs < 0) u_close(fd) ;
	        }
	    } /* end if (dialudp) */
	} else {
	    rs = SR_INVALID ;
	}
	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (procopenqueryhost) */


local int procqueryouter(PROGINFO *pip,void *ofp,int qfd)
{
	const int	llen = LINEBUFLEN ;
	int		rs ;
	int		len ;
	int		wlen = 0 ;
	char		lbuf[LINEBUFLEN + 1] ;

	if (pip == nullptr) return SR_FAULT ;

	while ((rs = u_read(qfd,lbuf,llen)) > 0) {
	    len = rs ;

	    rs = shio_write(ofp,lbuf,len) ;
	    wlen += rs ;

	    if (rs >= 0) rs = lib_sigterm() ;
	    if (rs >= 0) rs = lib_sigintr() ;
	    if (rs < 0) break ;
	} /* end while */

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (procqueryouter) */

local int procquerytermout(PROGINFO *pip,void *ofp,int qfd) noex {
	LOCINFO		*lip = pip->lip ;
	cint		to = pip->to_read ;
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	if (filer b ; (rs = filer_start(&b,qfd,0z,0,0)) >= 0) {
	    cint	llen = LINEBUFLEN ;
	    int		len ;
	    char	lbuf[LINEBUFLEN + 1] ;

	    while ((rs = filer_readlns(&b,lbuf,llen,to,nullptr)) > 0) {
	        len = rs ;

	            if (len > 0) {
	                rs = locinfo_termoutprint(lip,ofp,lbuf,len) ;
	                wlen += rs ;
	            } else {
	                rs = shio_print(ofp,lbuf,len) ;
	                wlen += rs ;
	            }

	        if (rs >= 0) rs = lib_sigterm() ;
	        if (rs >= 0) rs = lib_sigintr() ;
	        if (rs < 0) break ;
	    } /* end while */

	    rs1 = filer_finish(&b) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (filer) */

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("b_qotd/procquerytermout: ret rs=%d wlen=%u\n",
	        rs,wlen) ;
#endif

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (procquerytermout) */


local int procuserinfo_begin(PROGINFO *pip,USERINFO *uip)
{
	int		rs = SR_OK ;

	pip->nodename = uip->nodename ;
	pip->domainname = uip->domainname ;
	pip->username = uip->username ;
	pip->gecosname = uip->gecosname ;
	pip->realname = uip->realname ;
	pip->name = uip->name ;
	pip->fullname = uip->fullname ;
	pip->mailname = uip->mailname ;
	pip->org = uip->organization ;
	pip->logid = uip->logid ;
	pip->pid = uip->pid ;
	pip->uid = uip->uid ;
	pip->euid = uip->euid ;
	pip->gid = uip->gid ;
	pip->egid = uip->egid ;

	if (rs >= 0) {
	    const int	hlen = MAXHOSTNAMELEN ;
	    char	hbuf[MAXHOSTNAMELEN+1] ;
	    cchar	*nn = pip->nodename ;
	    cchar	*dn = pip->domainname ;
	    if ((rs = snsds(hbuf,hlen,nn,dn)) >= 0) {
	        cchar	**vpp = &pip->hostname ;
	        rs = proginfo_setentry(pip,vpp,hbuf,rs) ;
	    }
	}

	if (rs >= 0) {
	    rs = procuserinfo_logid(pip) ;
	} /* end if (ok) */

	return rs ;
}
/* end subroutine (procuserinfo_begin) */


local int procuserinfo_end(PROGINFO *pip)
{
	int		rs = SR_OK ;

	if (pip == nullptr) return SR_FAULT ;

	return rs ;
}
/* end subroutine (procuserinfo_end) */


local int procuserinfo_logid(PROGINFO *pip)
{
	int		rs ;
	if ((rs = lib_runmode()) >= 0) {
#if	CF_DEBUG
	    if (DEBUGLEVEL(4))
	        debugprintf("procuserinfo_logid: rm=%08ß\n",rs) ;
#endif
	    if (rs & KSHLIB_RMKSH) {
	        if ((rs = lib_serial()) >= 0) {
	            const int	s = rs ;
	            const int	plen = LOGIDLEN ;
	            const int	pv = pip->pid ;
	            cchar	*nn = pip->nodename ;
	            char	pbuf[LOGIDLEN+1] ;
	            if ((rs = mklogidpre(pbuf,plen,nn,pv)) >= 0) {
	                const int	slen = LOGIDLEN ;
	                char		sbuf[LOGIDLEN+1] ;
	                if ((rs = mklogidsub(sbuf,slen,pbuf,s)) >= 0) {
	                    cchar	**vpp = &pip->logid ;
	                    rs = proginfo_setentry(pip,vpp,sbuf,rs) ;
	                }
	            }
	        } /* end if (lib_serial) */
	    } /* end if (runmode-KSH) */
	} /* end if (lib_runmode) */
	return rs ;
}
/* end subroutine (procuserinfo_logid) */


local int locinfo_start(LOCINFO *lip,PROGINFO *pip)
{
	int		rs = SR_OK ;
	cchar		*varterm = VARTERM ;

	if (lip == nullptr) return SR_FAULT ;

	memset(lip,0,sizeof(LOCINFO)) ;
	lip->pip = pip ;
	lip->termtype = getourenv(pip->envv,varterm) ;

	lip->fl.separate = true ;

	return rs ;
}
/* end subroutine (locinfo_start) */


local int locinfo_finish(LOCINFO *lip)
{
	int		rs = SR_OK ;
	int		rs1 ;

	if (lip == nullptr) return SR_FAULT ;

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

	if (lip == nullptr) return SR_FAULT ;
	if (epp == nullptr) return SR_FAULT ;

	slp = &lip->stores ;
	if (! lip->open.stores) {
	    rs = vecstr_start(slp,4,0) ;
	    lip->open.stores = (rs >= 0) ;
	}

	if (rs >= 0) {
	    int	oi = -1 ;
	    if (*epp != nullptr) {
		oi = vecstr_findaddr(slp,*epp) ;
	    }
	    if (vp != nullptr) {
	        len = strnlen(vp,vl) ;
	        rs = vecstr_store(slp,vp,len,epp) ;
	    } else {
	        *epp = nullptr ;
	    }
	    if ((rs >= 0) && (oi >= 0)) {
	        vecstr_del(slp,oi) ;
	    }
	} /* end if (ok) */

	return (rs >= 0) ? len : rs ;
}
/* end subroutine (locinfo_setentry) */


local int locinfo_qfname(LOCINFO *lip,cchar *qfname)
{
	int		rs = SR_OK ;
	cchar		**vpp ;

	if (qfname != nullptr) {
	    vpp = &lip->qfname ;
	    rs = locinfo_setentry(lip,vpp,qfname,-1) ;
	}

	return rs ;
}
/* end subroutine (locinfo_qfname) */


local int locinfo_termoutbegin(LOCINFO *lip,void *ofp)
{
	PROGINFO	*pip = lip->pip ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		f_termout = false ;
	cchar		*tstr = lip->termtype ;

	if (lip->fl.termout || ((rs = shio_isterm(ofp)) > 0)) {
	    int		ncols = COLUMNS ;
	    cchar	*vp ;
	    if ((vp = getourenv(pip->envv,VARCOLUMNS)) != nullptr) {
	        if (int v ; (rs1 = cfdeci(vp,-1,&v)) >= 0) {
	            ncols = v ;
		}
	    }
	    if (rs >= 0) {
	        rs = termout_start(&lip->outer,tstr,-1,ncols) ;
	        lip->open.outer = (rs >= 0) ;
	    }
	} /* end if */

	if ((rs >= 0) && (pip->debuglevel > 0)) {
	    cchar	*pn = pip->progname ;
	    f_termout = lip->open.outer ;
	    shio_printf(pip->efp,"%s: termout=%u\n",pn,f_termout) ;
	    if (f_termout) {
	        shio_printf(pip->efp,"%s: termtype=%s\n",pn,tstr) ;
	    }
	}

	return (rs >= 0) ? f_termout : rs ;
}
/* end subroutine (locinfo_termoutbegin) */


local int locinfo_termoutend(LOCINFO *lip)
{
	int		rs = SR_OK ;
	int		rs1 ;

	if (lip->open.outer) {
	    lip->open.outer = false ;
	    rs1 = termout_finish(&lip->outer) ;
	    if (rs >= 0) rs = rs1 ;
	}

	return rs ;
}
/* end subroutine (locinfo_termoutend) */


local int locinfo_termoutprint(LOCINFO *lip,void *ofp,cchar *lbuf,int llen)
{
	PROGINFO	*pip = lip->pip ;
	TERMOUT		*top = &lip->outer ;
	int		rs ;
	int		wlen = 0 ;

	if (pip == nullptr) return SR_FAULT ;

	if ((rs = termout_load(top,lbuf,llen)) >= 0) {
	    int		ln = rs ;
	    int		ll ;
	    cchar	*lp ;
	    for (int i = 0 ; i < ln ; i += 1) {
	        ll = termout_getline(top,i,&lp) ;
	        if (ll == SR_NOTFOUND) break ;
	        rs = ll ;

#if	CF_DEBUG
	        if (DEBUGLEVEL(4)) {
	            debugprintf("b_qotd/locinfo_termoutprint: ll=%u\n",ll) ;
	            debugprintf("b_qotd/locinfo_termoutprint: l=>%r<\n",
	                lp,strlinelen(lp,ll,40)) ;
	        }
#endif

	        if (rs >= 0) {
	            rs = shio_print(ofp,lp,ll) ;
	            wlen += rs ;
	        }

	        if (rs < 0) break ;
	    } /* end for */
	} /* end if (termout_load) */

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (locinfo_termoutprint) */


local int locinfo_defspec(LOCINFO *lip,DAYSPEC *dsp)
{
	int		rs = SR_OK ;
	int		f = false ;

	f = f || (dsp->y < 0) ;
	f = f || (dsp->m < 0)  ;
	f = f || (dsp->d < 0) ;
	if (f) {
	    if (! lip->fl.curdate) rs = locinfo_curdate(lip) ;
	    if (dsp->y < 0) dsp->y = lip->ds.y ;
	    if (dsp->m < 0) dsp->m = lip->ds.m ;
	    if (dsp->d < 0) dsp->d = lip->ds.d ;
	}

	return rs ;
}
/* end subroutine (locinfo_defspec) */

local int locinfo_curdate(LOCINFO *lip) noex {
	PROGINFO	*pip = lip->pip ;
	int		rs = SR_OK ;
	if (! lip->fl.curdate) {
	    tmtime ct ;
	    lip->fl.curdate = true ;
	    if (lip->fl.gmt) {
	        rs = tmtime_timegm(&ct,pip->daytime) ;
	    } else {
	        rs = tmtime_timelocal(&ct,pip->daytime) ;
	    }
	    if (lip->year > 0) {
	        lip->ds.y = lip->year ;
	    } else {
	        lip->ds.y = (ct.year + TMTIME_YEARBASE) ;
	    }
	    lip->ds.m = ct.mon ;
	    lip->ds.d = ct.mday ;
	} /* end if (needed) */
	return rs ;
}
/* end subroutine (locinfo_curdate) */

local int locinfo_netparse(LOCINFO *lip,cchar *qp,int ql) noex {
	int		rs = SR_OK ;
	int		mjd = 0 ;
	if (ql < 0) ql = strlen(qp) ;
	if (hasalldig(qp,ql)) {
	    if (uint uv ; (rs = cfdecui(qp,ql,&uv)) >= 0) {
	        mjd = (int) uv ;
	    }
	} else if ((rs = ourmjd(qp,ql)) > 0) {
	    mjd = rs ;
	} else {
	    DAYSPEC	ds ;
	    if ((qp[0] == '+') || (qp[0] == '-')) {
	        rs = dayspec_default(&ds) ;
	    } else {
	        rs = dayspec_load(&ds,qp,ql) ;
	    }
	    if (rs >= 0) {
	        if ((rs = locinfo_defspec(lip,&ds)) >= 0) {
	            rs = getmjd(ds.y,ds.m,ds.d) ;
	            mjd = rs ;
	        }
	    }
	} /* end if */
	return (rs >= 0) ? mjd : rs ;
} /* end subroutine (locinfo_netparse) */

local int locinfo_mjd(LOCINFO *lip) noex {
	int		rs ;
	int		mjd = 0 ;
	if ((rs = locinfo_curdate(lip)) >= 0) {
	    rs = getmjd(lip->ds.y,lip->ds.m,lip->ds.d) ;
	    mjd = rs ;
	}
	return (rs >= 0) ? mjd : rs ;
}
/* end subroutine (locinfo_mjd) */



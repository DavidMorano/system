/* b_numbers */
/* lang=C++11 */

/* SHELL built-in to return load averages */
/* version %I% last-modified %G% */


#define	CF_DEBUGS	0		/* non-switchable debug print-outs */
#define	CF_DEBUG	0		/* switchable at invocation */
#define	CF_DEBUGMALL	1		/* debug memory-allocations */


/* revision history:

	= 2004-03-01, David A�D� Morano
	This subroutine was originally written.  

*/

/* Copyright � 2004 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	Synopsis:

	$ numbers <num-n>[,<num-k>] ...


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
#include	<limits.h>
#include	<unistd.h>
#include	<stdlib.h>
#include	<string.h>
#include	<time.h>

#include	<usystem.h>
#include	<bits.h>
#include	<keyopt.h>
#include	<field.h>
#include	<estrings.h>
#include	<cfdec.h>
#include	<exitcodes.h>
#include	<localmisc.h>

#include	"shio.h"
#include	"kshlib.h"
#include	"b_numbers.h"
#include	"defs.h"


/* local defines */

#define	LOCINFO		struct locinfo
#define	LOCINFO_FL	struct locinfo_flags


/* external subroutines */

/* in C++ this is required (like this in this order)! */
extern "C" int b_numbers(int,cchar **,void *) ;
extern "C" int p_numbers(int,cchar **,cchar **,void *) ;

extern "C" LONG	factorial(int) ;
extern "C" LONG	permutations(int,int) ;
extern "C" LONG	combinations(int,int) ;
extern "C" LONG	multicombinations(int,int) ;
extern "C" LONG	ipowell(int,int) ;

extern "C" int	ipow(int,int) ;
extern "C" int	matostr(cchar **,int,cchar *,int) ;
extern "C" int	matstr(cchar **,cchar *,int) ;
extern "C" int	matcasestr(cchar **,cchar *,int) ;
extern "C" int	optbool(cchar *,int) ;
extern "C" int	optvalue(cchar *,int) ;
extern "C" int	sialpha(cchar *,int) ;
extern "C" int	hasalpha(cchar *,int) ;
extern "C" int	isalphalatin(int) ;
extern "C" int	isdigitlatin(int) ;
extern "C" int	isFailOpen(int) ;
extern "C" int	isNotPresent(int) ;

extern "C" int	printhelp(void *,cchar *,cchar *,cchar *) ;
extern "C" int	proginfo_setpiv(PROGINFO *,cchar *,const struct pivars *) ;

#if	CF_DEBUGS || CF_DEBUG
extern "C" int	debugopen(cchar *) ;
extern "C" int	debugprintf(cchar *,...) ;
extern "C" int	debugclose() ;
extern "C" int	strlinelen(cchar *,int,int) ;
#endif

extern "C" cchar	*getourenv(cchar **,cchar *) ;

extern "C" char	*strwcpy(char *,cchar *,int) ;
extern "C" char	*strnpbrk(cchar *,int,cchar *) ;
extern "C" char	*timestr_log(time_t,char *) ;
extern "C" char	*timestr_elapsed(time_t,char *) ;


/* external variables */

extern char	**environ ;		/* definition required by AT&T AST */


/* local structures */

enum numtypes {
	numtype_default,
	numtype_permutations,
	numtype_multipermutations,
	numtype_combinations,
	numtype_multicombinations,
	numtype_factorial,
	numtype_exponetial,
	numtype_overlast
} ;

static const char	*keys[] = {
	"e",
	"p",
	"mp",
	"pm",
	"c",
	"mc",
	"cm",
	"f",
	NULL
} ;

/* these map the above keys to their "numtypes" ('enum' farther above) */
static const int	numkeys[] = {
	numtype_exponetial,
	numtype_permutations,
	numtype_multipermutations,
	numtype_multipermutations,
	numtype_combinations,
	numtype_multicombinations,
	numtype_multicombinations,
	numtype_factorial,
	numtype_overlast
} ;

struct locinfo_flags {
	uint		init:1 ;
	uint		numtype:1 ;
	uint		otype:1 ;
	uint		hex:1 ;
	uint		multi:1 ;	/* multi-set variant */
	uint		with:1 ;	/* *with* repitition */
} ;

struct locinfo {
	LOCINFO_FL	f, have, changed, final ;
	PROGINFO	*pip ;
	int		numtype ;
	int		otype ;
} ;

struct numpair {
	int		n = 0 ;
	int		k = 0 ;
	int		numtype = 0 ;
	int loadkey(cchar *,int) ;
	int load(cchar *sp,int sl) {
	    int		rs = SR_OK ;
	    int		nl = sl ;
	    int		kl = 0 ;
	    cchar	*tp ;
	    cchar	*np = sp ;
	    cchar	*kp = NULL ;
	    if (sl < 0) sl = strlen(sp) ;
	    if (hasalpha(sp,sl)) {
		rs = loadkey(sp,sl) ;
	    } else {
	        if ((tp = strnpbrk(sp,sl,",:-���� \t")) != NULL) {
		    kp = (tp+1) ;
		    kl = ((sp+sl)-(tp+1)) ;
		    nl = (tp-sp) ;
	        }
	        if ((rs = cfdeci(np,nl,&n)) >= 0) {
		    if (kp != NULL) {
		        rs = cfdeci(kp,kl,&k) ;
		    }
	        }
	    }
	    return (rs >= 0) ? numtype : rs ;
	} ;
} ;

int numpair::loadkey(cchar *sp,int sl) {
	int	rs = SR_OK ;
	int	si ;
#if	CF_DEBUGS
	debugprintf("b_numbers/numpair::loadkey: ent s=%t\n",sp,sl) ;
#endif
	if ((si = sialpha(sp,sl)) >= 0) {
	    int		ch ;
	    int		ki ;
	    int		nl = si ;
	    int		al = 0 ;
	    cchar	*np = sp ;
	    cchar	*ap = (sp+si) ;
	    sp += si ;
	    sl -= si ;
#if	CF_DEBUGS
	debugprintf("b_numbers/numpair::loadkey: n=%t\n",np,nl) ;
	debugprintf("b_numbers/numpair::loadkey: s=%t\n",sp,sl) ;
#endif
	    while (sl && *sp) {
	        ch = MKCHAR(*sp) ;
		if (! isalphalatin(ch)) break ;
	 	sl -= 1 ;
		al += 1 ;
		sp += 1 ;
	    }
#if	CF_DEBUGS
	debugprintf("b_numbers/numpair::loadkey: a=%t\n",ap,al) ;
#endif
	    if ((rs = cfdeci(np,nl,&n)) >= 0) {
	            if ((ki = matcasestr(keys,ap,al)) >= 0) {
		        numtype = numkeys[ki] ;
		        if (sl > 0) {
		            rs = cfdeci(sp,sl,&k) ;
		        }
	            } else {
	                rs = SR_INVALID ;
	            }
	    }
	} else {
	    rs = SR_INVALID ;
	}
	return (rs >= 0) ? numtype : rs ;
} ; /* end method (numpair::loadkey) */


/* forward references */

static int	mainsub(int,cchar **,cchar **,void *) ;

static int	usage(PROGINFO *) ;

static int	procopts(PROGINFO *,KEYOPT *) ;
static int	procargs(PROGINFO *,ARGINFO *,BITS *,cchar *,cchar *) ;
static int	procspecs(PROGINFO *,SHIO *,cchar *,int) ;
static int	procspec(PROGINFO *,SHIO *,cchar *,int) ;

static int	locinfo_start(LOCINFO *,PROGINFO *) ;
static int	locinfo_finish(LOCINFO *) ;
static int	locinfo_setotype(LOCINFO *,cchar *,int) ;


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
	argopt_overlast
} ;

static const char	*akonames[] = {
	"otype",
	"output",
	"hex",
	NULL
} ;

enum akonames {
	akoname_output,
	akoname_otype,
	akoname_hex,
	akoname_overlast
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

static const uchar	aterms[] = {
	0x00, 0x2E, 0x00, 0x00,
	0x09, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
} ;

enum otypes {
	otype_dec,
	otype_hex,
	otype_overlast
} ;

static const char	*otypes[] = {
	"decimal",
	"hexadecimal",
	NULL
} ;


/* exported subroutines */


int b_numbers(int argc,cchar *argv[],void *contextp)
{
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
/* end subroutine (b_numbers) */


int p_numbers(int argc,cchar *argv[],cchar *envv[],void *contextp)
{
	return mainsub(argc,argv,envv,contextp) ;
}
/* end subroutine (p_numbers) */


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
	int		rs, rs1 ;
	int		ex = EX_INFO ;
	int		f_optminus, f_optplus, f_optequal ;
	int		f_version = FALSE ;
	int		f_usage = FALSE ;
	int		f_help = FALSE ;

	cchar		*argp, *aop, *akp, *avp ;
	cchar		*argval = NULL ;
	cchar		*pr = NULL ;
	cchar		*sn = NULL ;
	cchar		*afname = NULL ;
	cchar		*efname = NULL ;
	cchar		*ofname = NULL ;
	cchar		*cp ;


#if	CF_DEBUGS || CF_DEBUG
	if ((cp = getourenv(envv,VARDEBUGFNAME)) != NULL) {
	    rs = debugopen(cp) ;
	    debugprintf("b_numbers: starting DFD=%d\n",rs) ;
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

/* keyword match or only key letters? */

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

			    case 'c':
				if (! lip->f.numtype) {
				    lip->numtype = numtype_combinations ;
				    lip->f.numtype = TRUE ;
				}
	                        break ;

			    case 'p':
				if (! lip->f.numtype) {
				    lip->numtype = numtype_permutations ;
				    lip->f.numtype = TRUE ;
				}
	                        break ;

			    case 'f':
				if (! lip->f.numtype) {
				    lip->numtype = numtype_factorial ;
				    lip->f.numtype = TRUE ;
				}
	                        break ;

/* multi-set variant */
			    case 'm':
				lip->f.multi = TRUE ;
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

/* specify with or without repititions */
	                    case 'w':
	                        lip->f.with = TRUE ;
	                        if (f_optequal) {
	                            f_optequal = FALSE ;
	                            if (avl) {
	                                rs = optbool(avp,avl) ;
	                                lip->f.with = (rs > 0) ;
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

	if (rs < 0)
	    goto badarg ;

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    debugprintf("b_numbers: debuglevel=%u\n",pip->debuglevel) ;
#endif

	if (f_version) {
	    SHIO	*efp = (SHIO *) pip->efp ;
	    cchar	*pn = pip->progname ;
	    shio_printf(efp,"%s: version %s\n",pn,VERSION) ;
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
	    SHIO	*efp = (SHIO *) pip->efp ;
	    shio_printf(efp,"%s: pr=%s\n",pip->progname,pip->pr) ;
	    shio_printf(efp,"%s: sn=%s\n",pip->progname,pip->searchname) ;
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

/* initialize */

	if ((rs >= 0) && (pip->n == 0) && (argval != NULL)) {
	    rs = optvalue(argval,-1) ;
	    pip->n = rs ;
	}

	if (afname == NULL) afname = getourenv(envv,VARAFNAME) ;

	if (rs >= 0) {
	    rs = procopts(pip,&akopts) ;
	}

	if (lip->f.multi) {
	    switch (lip->numtype) {
	    case numtype_permutations:
		lip->numtype = numtype_multipermutations ;
		break ;
	    case numtype_combinations:
		lip->numtype = numtype_multicombinations ;
		break ;
	    }
	} /* end if (handling multi-set variants) */

/* process */

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
	} else if (ex == EX_OK) {
	    SHIO	*efp = (SHIO *) pip->efp ;
	    cchar	*pn = pip->progname ;
	    cchar	*fmt = "%s: invalid argument or configuration (%d)\n" ;
	    shio_printf(efp,fmt,pn,rs) ;
	    ex = EX_USAGE ;
	    usage(pip) ;
	}

/* done */
	if ((rs < 0) && (ex == EX_OK)) {
	    switch (rs) {
	    case SR_INVALID:
	        ex = EX_USAGE ;
	        if (! pip->f.quiet) {
	    	    SHIO	*efp = (SHIO *) pip->efp ;
		    cchar	*pn = pip->progname ;
	            shio_printf(efp,"%s: invalid query (%d)\n",pn,rs) ;
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
	    SHIO	*efp = (SHIO *) pip->efp ;
	    cchar	*pn = pip->progname ;
	    shio_printf(efp,"%s: exiting ex=%u (%d)\n",pn,ex,rs) ;
	}

	if (pip->efp != NULL) {
	    SHIO	*efp = (SHIO *) pip->efp ;
	    pip->open.errfile = FALSE ;
	    shio_close(efp) ;
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

#if	(CF_DEBUGS || CF_DEBUG)
	debugclose() ;
#endif

	return ex ;

/* the bad things */
badarg:
	{
	    SHIO	*efp = (SHIO *) pip->efp ;
	    cchar	*pn = pip->progname ;
	    ex = EX_USAGE ;
	    shio_printf(efp,"%s: invalid argument specified (%d)\n",pn,rs) ;
	    usage(pip) ;
	}
	goto retearly ;

}
/* end subroutine (mainsub) */


static int usage(PROGINFO *pip)
{
	SHIO		*efp = (SHIO *) pip->efp ;
	int		rs = SR_OK ;
	int		wlen = 0 ;
	cchar	*pn = pip->progname ;
	cchar	*fmt ;

	fmt = "%s: USAGE> %s [<spec(s)> ...] [-af <afile>]\n" ;
	if (rs >= 0) rs = shio_printf(efp,fmt,pn,pn) ;
	wlen += rs ;

	fmt = "%s:  [-Q] [-D] [-v[=<n>]] [-HELP] [-V]\n" ;
	if (rs >= 0) rs = shio_printf(efp,fmt,pn) ;
	wlen += rs ;

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (usage) */


/* process the program ako-options */
static int procopts(PROGINFO *pip,KEYOPT *kop)
{
	LOCINFO		*lip = (LOCINFO *) pip->lip ;
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

	            if ((oi = matostr(akonames,2,kp,kl)) >= 0) {

	                vl = keyopt_fetch(kop,kp,NULL,&vp) ;

	                switch (oi) {
	                case akoname_output:
	                case akoname_otype:
	                    if (! lip->final.otype) {
	                        lip->have.otype = TRUE ;
	                        lip->final.otype = TRUE ;
	                        lip->f.otype = TRUE ;
	                        if (vl > 0) {
	                            rs = locinfo_setotype(lip,vp,vl) ;
				}
	                    }
	                    break ;
	                case akoname_hex:
	                    if (! lip->final.hex) {
	                        lip->have.hex = TRUE ;
	                        lip->final.hex = TRUE ;
	                        lip->f.hex = TRUE ;
				lip->otype = otype_hex ;
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
	        } /* end while (looping through key options) */

	        keyopt_curend(kop,&kcur) ;
	    } /* end if (keyopt-cur) */
	} /* end if (ok) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (procopts) */


static int procargs(PROGINFO *pip,ARGINFO *aip,BITS *bop,cchar *ofn,cchar *afn)
{
	SHIO		ofile, *ofp = &ofile ;
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	cchar		*pn = pip->progname ;
	cchar		*fmt ;

	if ((ofn == NULL) || (ofn[0] == '\0') || (ofn[0] == '-'))
	    ofn = STDFNOUT ;

	if ((rs = shio_open(ofp,ofn,"wct",0666)) >= 0) {
	    int		pan = 0 ;
	    int		cl ;
	    cchar	*cp ;

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
	                    rs = procspec(pip,ofp,cp,-1) ;
			    wlen += rs ;
	                }
	            }

	            if (rs >= 0) rs = lib_sigterm() ;
	            if (rs >= 0) rs = lib_sigintr() ;
	            if (rs < 0) break ;
	        } /* end for (handling positional arguments) */
	    } /* end if (ok) */

	    if ((rs >= 0) && (afn != NULL) && (afn[0] != '\0')) {
	        SHIO	afile, *afp = &afile ;

	        if (strcmp(afn ,"-") == 0)
	            afn = STDFNIN ;

	        if ((rs = shio_open(afp,afn,"r",0666)) >= 0) {
	            const int	llen = LINEBUFLEN ;
	            int		len ;
	            char	lbuf[LINEBUFLEN + 1] ;

	            while ((rs = shio_readline(afp,lbuf,llen)) > 0) {
	                len = rs ;

	                if (lbuf[len - 1] == '\n') len -= 1 ;
	                lbuf[len] = '\0' ;

	                if ((cl = sfskipwhite(lbuf,len,&cp)) > 0) {
	                    if (cp[0] != '#') {
	                        pan += 1 ;
	                        rs = procspecs(pip,ofp,cp,cl) ;
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
	            if (! pip->f.quiet) {
	    	        SHIO	*efp = (SHIO *) pip->efp ;
	                fmt = "%s: inacessible argument-list (%d)\n",
	                shio_printf(efp,fmt,rs) ;
	                fmt = "%s: afile=%s\n" ;
	                shio_printf(efp,fmt,pn,afn) ;
	            } /* end if */
	        } /* end if */

	    } /* end if (processing file argument file list) */

#ifdef	COMMENT
	    if ((rs >= 0) && (pan == 0)) {
	        SHIO	*efp = (SHIO *) pip->efp ;
	        rs = SR_INVALID ;
	        fmt = "%s: no specifications given\n" ;
	        shio_printf(efp,fmt,pn) ;
	    } /* end if */
#endif /* COMMENT */

	    rs1 = shio_close(ofp) ;
	    if (rs >= 0) rs = rs1 ;
	} else {
	    SHIO	*efp = (SHIO *) pip->efp ;
	    fmt = "%s: inaccessible output (%d)\n" ;
	    shio_printf(efp,fmt,pn,rs) ;
	    fmt = "%s: ofn=%s\n" ;
	    shio_printf(efp,fmt,pn,ofn) ;
	}

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (procargs) */


static int procspecs(PROGINFO *pip,SHIO *ofp,cchar *lbuf,int llen)
{
	FIELD		fsb ;
	int		rs ;
	int		wlen = 0 ;
	if ((rs = field_start(&fsb,lbuf,llen)) >= 0) {
	    int		fl ;
	    cchar	*fp ;
	    while ((fl = field_get(&fsb,aterms,&fp)) >= 0) {
	        if (fl > 0) {
	            rs = procspec(pip,ofp,fp,fl) ;
	            wlen += rs ;
	        }
	        if (fsb.term == '#') break ;
	        if (rs < 0) break ;
	    } /* end while */
	    field_finish(&fsb) ;
	} /* end if (field) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (procspecs) */


/* process a specification name */
static int procspec(PROGINFO *pip,SHIO *ofp,cchar *np,int nl)
{
	LOCINFO		*lip = (LOCINFO *) pip->lip ;
	numpair		query ;
	int		numtype = lip->numtype ;
	int		rs ;
	int		wlen = 0 ;

	if (np == NULL) return SR_FAULT ;

#if	CF_DEBUG
	if (DEBUGLEVEL(4)) {
	    debugprintf("b_numbers/procspec: ent spec=%t\n",np,nl) ;
	    debugprintf("b_numbers/procspec: def numtype=%d\n",numtype) ;
	}
#endif

	if (nl < 0) nl = strlen(np) ;

	if ((rs = query.load(np,nl)) >= 0) {
	    LONG	v = 0 ;
	    int		n = query.n ;
	    int		k = query.k ;
	    if (rs > 0) numtype = rs ;
#if	CF_DEBUG
	    if (DEBUGLEVEL(4)) {
	        debugprintf("b_numbers/procspec: rs=%d n=%d k=%d nt=%d\n",
		    rs,n,k,numtype) ;
	        debugprintf("b_numbers/procspec: switch nt=%d\n",numtype) ;
	    }
#endif
	    switch (numtype) {
	    case numtype_exponetial:
		v = ipowell(n,k) ;
		break ;
	    case numtype_permutations:
	        if (lip->f.with) {
		    v = ipowell(n,k) ;
		} else {
		    v = permutations(n,k) ;
		}
		break ;
	    case numtype_combinations:
	        if (lip->f.with) {
		    v = multicombinations(n,k) ;
		} else {
		    v = combinations(n,k) ;
		}
		break ;
	    case numtype_multicombinations:
		v = multicombinations(n,k) ;
		break ;
	    case numtype_factorial:
	    case numtype_default:
		v = factorial(query.n) ;
		break ;
	    default:
		rs = SR_INVALID ;
		break ;
	    } /* end switch */
	    if ((rs >= 0) && (v < 0)) rs = SR_RANGE ;
#if	CF_DEBUG
	    if (DEBUGLEVEL(4)) {
	        debugprintf("b_numbers/procspec: switch-out rs=%d\n",rs) ;
	    }
#endif
	    if (rs >= 0) {
		cchar	*fmt ;
		switch (lip->otype) {
		default:
		case otype_dec:
		    fmt = "%lld\n" ;
		    break ;
		case otype_hex:
		    fmt = "%016llx\n" ;
		    break ;
		} /* end switch */
	        rs = shio_printf(ofp,fmt,v) ;
	        wlen += rs ;
	    } else if (rs == SR_INVALID) {
	        rs = shio_printf(ofp,"*invalid*\n") ;
	        wlen += rs ;
	    } else if (rs == SR_RANGE) {
	        rs = shio_printf(ofp,"*overflow*\n") ;
	        wlen += rs ;
	    }
	} /* end if (query.load) */

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("b_numbers/procspec: ret rs=%d wlen=%u\n",rs,wlen) ;
#endif

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (procspec) */


static int locinfo_start(LOCINFO *lip,PROGINFO *pip)
{

	if (lip == NULL) return SR_FAULT ;
	memset(lip,0,sizeof(LOCINFO)) ;
	lip->pip = pip ;

	return SR_OK ;
}
/* end subroutine (locinfo_start) */


static int locinfo_finish(LOCINFO *lip)
{
	if (lip == NULL) return SR_FAULT ;
	return SR_OK ;
}
/* end subroutine (locinfo_finish) */


static int locinfo_setotype(LOCINFO *lip,cchar *vp,int vl)
{
	int		rs = SR_OK ;
	if (vp && vl) {
	    PROGINFO	*pip = lip->pip ;
	    int		mi ;
	    if ((mi = matostr(otypes,1,vp,vl)) >= 0) {
		lip->otype = mi ;
	    } else {
		SHIO	*efp = (SHIO *) pip->efp ;
		cchar	*pn = pip->progname ;
		cchar	*fmt = "%s: invalid output type\n" ;
		shio_printf(efp,fmt,pn) ;
		rs = SR_INVALID ;
	    }
	}
	return rs ;
}
/* end subroutine (locinfo_setotype) */



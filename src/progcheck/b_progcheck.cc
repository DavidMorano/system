/* b_progcheck SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* check C-language programs for some simple problems */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* non-switchable debug print-outs */
#define	CF_DEBUG	0		/* switchable at invocation */
#define	CF_DEBUGMALL	1		/* debug memory allocation */
#define	CF_LOCSETENT	0		/* allow |locinfo_setentry()| */

/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

	= 2004-03-01, David A­D­ Morano
	This was created quickly as a hack to replace the existing
	CHECKBRA program.

	= 2017-10-18, David A­D­ Morano
	I tried to update this to ignore characters within double
	quotation marks.

*/

/* Copyright © 2004 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Description:
	This program will check the specified C language files for
	problems.

	Synopsis:
	$ progcheck [<file(s)> ...] [<opt(s)>]

	Notes:
	This program is quite inefficient.  It calls LANGSTATE four
	times for each character of input.  A proper implementation
	would call it just once for each character of input!  How
	did this happen?  It happened because we built the program
	in incremental stages and what should have happened was
	that a rewrite, refactor, whatever should have reorganized
	the program to call LANGSTATE only once for each input
	character.  Whatever, the program works right now.

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
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<getoutenv.h>
#include	<mallocxx.h>
#include	<bits.h>
#include	<keyopt.h>
#include	<vecstr.h>
#include	<ascii.h>
#include	<langstate.h>
#include	<optval.h>
#include	<sncpyx.h>
#include	<rmx.h>
#include	<sfx.h>
#include	<matstr.h>
#include	<linehist.h>
#include	<mkchar.h>
#include	<char.h>
#include	<ischarx.h>
#include	<isbad.h>
#include	<exitcodes.h>
#include	<localmisc.h>

#include	"shio.h"
#include	"kshlib.h"
#include	"b_progcheck.h"
#include	"defs.h"


/* local defines */

#define	PI		proginfo

#define	LI_FL		locinfo_fl

#define	KO		keyopt
#define	KO_CUR		keyopt_cur

#define	LS		langstate
#define	LS_STAT		langstate_stat

#define	AI		ARGINFO

#define	HIST		hist_head

#define	FUNTAB		fun_tab

#define	FC		fun_count

#define	PROGCHECK_NCH	3


/* external subroutines */

extern int	printhelp(void *,cchar *,cchar *,cchar *) noex ;
extern int	proginfo_setpiv(PI *,cchar *,const pivars *) noex ;

#if	CF_DEBUGS || CF_DEBUG
extern int	debugopen(cchar *) noex ;
extern int	debugprintf(cchar *,...) noex ;
extern int	debugprinthex(cchar *,int,cchar *,int) noex ;
extern int	debugclose() noex ;
extern int	strlinelen(cchar *,int,int) noex ;
#endif


/* external variables */

extern char	**environ ;		/* definition required by AT&T AST */


/* local structures */

struct locinfo_flags {
	uint		stores:1 ;
	uint		counts:1 ;
	uint		dirty:1 ;
} ;

struct locinfo {
	LI_FL		have, f, changed, final ;
	LI_FL		open ;
	vecstr		stores ;
	PI		*pip ;
	int		to ;
} ;

struct hist_head {
	linehist	types[3] ;
} ;

struct fun_tab {
	char		*funcname ;
	char		c_open ;
	char		c_close ;
} ;

struct fun_count {
	int		c_open ;
	int		c_close ;
} ;


/* forward references */

local int	mainsub(int,cchar **,cchar **,void *) noex ;

local int	usage(PI *) noex ;

local int	procopts(PI *,KO *) noex ;
local int	procargs(PI *,AI *,BITS *,cchar *,cchar *) noex ;
local int	procfile(PI *,void *,cchar *) noex ;
local int	procln(PI *,LS *,FC *,int,cchar *,int) noex ;
local int	procout(PI *,void *,cchar *,FC *) noex ;
local int	procouthist(PI *,void *,HIST *) noex ;
local int	procoutlang(PI *,void *,LS *) noex ;

local int	locinfo_start(LI *,PI *) noex ;
local int	locinfo_finish(LI *) noex ;

#if	CF_LOCSETENT
local int	locinfo_setentry(LI *,cchar **,cchar *,int) noex ;
#endif

local int	hist_start(HIST *) noex ;
local int	hist_proc(HIST *,int,cchar *,int) noex ;
local int	hist_count(HIST *,int) noex ;
local int	hist_get(HIST *,int,int,int *) noex ;
local int	hist_finish(HIST *) noex ;

local int	funcount_clear(FC *) noex ;

static cchar	*chartype(int) noex ;


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
	argopt_if,
	argopt_to,
	argopt_tr,
	argopt_c,
	argopt_overlast
} ;

constexpr cpcchar	argopts[] = {
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
	"c",
	nullptr
} ;

constexpr PIVARS	initvars = {
	VARPROGRAMROOT1,
	VARPROGRAMROOT2,
	VARPROGRAMROOT3,
	PROGRAMROOT,
	VARPRNAME
} ;

constexpr MAPEX		mapexs[] = {
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

enum akonames {
	akoname_bufwhole,
	akoname_bufline,
	akoname_bufnone,
	akoname_whole,
	akoname_line,
	akoname_none,
	akoname_un,
	akoname_counts,
	akoname_empty,
	akoname_overlast
} ;

constexpr cpcchar	akonames[] = {
	"bufwhole",
	"bufline",
	"bufnone",
	"whole",
	"line",
	"none",
	"un",
	"counts",
	"",
	nullptr
} ;

constexpr fun_tab	cca[] = {
	{ "paren", CH_LPAREN, CH_RPAREN },
	{ "brace", CH_LBRACE, CH_RBRACE },
	{ "brack", CH_LBRACK, CH_RBRACK }
} ;

constexpr cpcchar	balstrs[] = {
	"()",
	"{}",
	"[]",
	nullptr
} ;

constexpr cpcchar	chartypes[] = {
	"unspec",
	"open",
	"close",
	nullptr
} ;

constexpr cpcchar	langstatetypes[] = {
	"clear",
	"comment",
	"quote",
	"literal",
	nullptr
} ;


/* exported variables */


/* exported subroutines */

int b_progcheck(int argc,mainv argv,void *contextp) noex {
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
/* end subroutine (b_progcheck) */

int p_progcheck(int argc,mainv argv,mainv envv,void *contextp) noex {
	return mainsub(argc,argv,envv,contextp) ;
}
/* end subroutine (p_progcheck) */


/* local subroutines */

/* ARGSUSED */
local int mainsub(int argc,mainv argv,mainv envv,void *contextp) noex {
	PI		pi, *pip = &pi ;
	LI		li, *lip = &li ;
	AI		ainfo ;
	BITS		pargs ;
	LO		akopts ;
	SHIO		errfile ;

#if	(CF_DEBUGS || CF_DEBUG) && CF_DEBUGMALL
	uint		mo_start = 0 ;
#endif

	int		argr, argl, aol, akl, avl, kwi ;
	int		ai, ai_max, ai_pos ;
	int		rs, rs1 ;
	int		v ;
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
	cchar		*efname = nullptr ;
	cchar		*ofname = nullptr ;
	cchar		*tos_open = nullptr ;
	cchar		*tos_read = nullptr ;
	cchar		*cp ;

	(void) envv ;
	(void) contextp ;

#if	CF_DEBUGS || CF_DEBUG
	if ((cp = getourenv(envv,VARDEBUGFNAME)) != nullptr) {
	    rs = debugopen(cp) ;
	    debugprintf("b_progcheck: starting DFD=%u\n",rs) ;
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

	if ((cp = getourenv(envv,VARBANNER)) == nullptr) cp = BANNER ;
	rs = proginfo_setbanner(pip,cp) ;

/* initialize */

	pip->verboselevel = 1 ;

	pip->lip = lip ;
	if (rs >= 0) rs = locinfo_start(lip,pip) ;
	if (rs < 0) {
	    ex = EX_OSERR ;
	    goto badlocstart ;
	}

/* start parsing the arguments */

#if	CF_DEBUGS
	debugprintf("b_progcheck: args\n") ;
#endif

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
	    argl = lenstr(argp) ;

#if	CF_DEBUGS
	    debugprintf("b_progcheck: a=>%r<\n",argp,argl) ;
#endif

	    f_optminus = (*argp == '-') ;
	    f_optplus = (*argp == '+') ;
	    if ((argl > 1) && (f_optminus || f_optplus)) {
	        cint	ach = MKCHAR(argp[1]) ;

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

#if	CF_DEBUGS
	            debugprintf("b_progcheck: ak=>%r<\n",akp,akl) ;
#endif

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
	                            argl = lenstr(argp) ;
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
	                            argl = lenstr(argp) ;
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
	                            argl = lenstr(argp) ;
	                            if (argl)
	                                tos_read = argp ;
	                        } else
	                            rs = SR_INVALID ;
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
	                            argl = lenstr(argp) ;
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
	                            argl = lenstr(argp) ;
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
	                            argl = lenstr(argp) ;
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
	                            argl = lenstr(argp) ;
	                            if (argl)
	                                ofname = argp ;
	                        } else
	                            rs = SR_INVALID ;
	                    }
	                    break ;

/* specify counts */
	                case argopt_c:
	                    lip->fl.counts = true ;
	                    lip->final.counts = true ;
	                    lip->have.counts = true ;
	                    if (f_optequal) {
	                        f_optequal = false ;
	                        if (avl) {
	                            rs = optbool(avp,avl) ;
	                            lip->fl.counts = (rs > 0) ;
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
	                    cint	kc = MKCHAR(*akp) ;

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
	                            argl = lenstr(argp) ;
	                            if (argl)
	                                pr = argp ;
	                        } else
	                            rs = SR_INVALID ;
	                        break ;

/* version */
	                    case 'V':
	                        f_version = true ;
	                        break ;

/* options */
	                    case 'o':
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = lenstr(argp) ;
	                            if (argl) {
	                                LO	*kop = &akopts ;
	                                rs = keyopt_loads(kop,argp,argl) ;
	                            }
	                        } else
	                            rs = SR_INVALID ;
	                        break ;

/* quiet mode */
	                    case 'q':
	                        pip->verboselevel = 0 ;
	                        break ;

	                    case 't':
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = lenstr(argp) ;
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
	                        pip->final.bufnone = true ;
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

#if	CF_DEBUGS
	debugprintf("b_progcheck: args-out rs=%d\n",rs) ;
#endif

	if (rs < 0)
	    goto badarg ;

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    debugprintf("b_progcheck: debuglevel=%u\n",pip->debuglevel) ;
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

#if	CF_DEBUG
	if (DEBUGLEVEL(2)) {
	    debugprintf("b_progcheck: pr=%s\n",pip->pr) ;
	    debugprintf("b_progcheck: sn=%s\n",pip->searchname) ;
	}
#endif

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

	if (afname == nullptr) afname = getourenv(pip->envv,VARAFNAME) ;

	if (rs >= 0) {
	    rs = procopts(pip,&akopts) ;
	}

#ifdef	COMMENT
	if (pip->tmpdname == nullptr) pip->tmpdname = getourenv(envv,VARTMPDNAME) ;
	if (pip->tmpdname == nullptr) pip->tmpdname = TMPDNAME ;
#endif

	pip->to_open = -1 ;
	pip->to_read = -1 ;
	if ((rs >= 0) && (tos_open != nullptr)) {
	    rs = cfdecti(tos_open,-1,&v) ;
	    pip->to_open = v ;
	}

	if ((rs >= 0) && (tos_read != nullptr)) {
	    rs = cfdecti(tos_read,-1,&v) ;
	    pip->to_read = v ;
	}

	if (pip->debuglevel > 0) {
	    cchar	*pn = pip->progname ;
	    if ((pip->to_open >= 0) || (pip->to_read >= 0)) {
	        shio_printf(pip->efp,"%s: to_open=%d\n",pn,pip->to_open) ;
	        shio_printf(pip->efp,"%s: to_read=%d\n",pn,pip->to_read) ;
	    }
	}

#if	CF_DEBUG
	if (DEBUGLEVEL(2)) {
	    debugprintf("b_progcheck: to_open=%d\n",pip->to_open) ;
	    debugprintf("b_progcheck: to_read=%d\n",pip->to_read) ;
	    debugprintf("b_progcheck: have.bufline=%u\n",pip->have.bufline) ;
	    debugprintf("b_progcheck: have.bufnone=%u\n",pip->have.bufnone) ;
	    debugprintf("b_progcheck: f_bufline=%u\n",pip->fl.bufline) ;
	    debugprintf("b_progcheck: f_bufnone=%u\n",pip->fl.bufnone) ;
	}
#endif /* CF_DEBUG */

	if (pip->to_open == 0)
	    pip->to_open = 1 ;

	if (pip->to_read == 0)
	    pip->to_read = 1 ;

/* go */

	memclear(&ainfo) ;
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
	    cchar	*pn = pip->progname ;
	    cchar	*fmt = "%s: invalid argument or configuration (%d)\n" ;
	    shio_printf(pip->efp,fmt,pn,rs) ;
	    ex = EX_USAGE ;
	    usage(pip) ;
	}

/* done! */
	if ((rs < 0) && (ex == EX_OK)) {
	    switch (rs) {
	    default:
	        if (! pip->fl.quiet) {
	            cchar	*pn = pip->progname ;
	            cchar	*fmt = "%s: could not process (%d)\n" ;
	            shio_printf(pip->efp,fmt,pn,rs) ;
	        }
	        break ;
	    case SR_PIPE:
	        break ;
	    } /* end switch */
	    ex = mapex(mapexs,rs) ;
	} else if ((rs >= 0) && (ex == EX_OK)) {
	    if ((rs = lib_sigterm()) < 0) {
	        ex = EX_TERM ;
	    } else if ((rs = lib_sigintr()) < 0) {
	        ex = EX_INTR ;
	    } else if (lip->fl.dirty) {
	        ex = EX_DATAERR ;
	    }
	} /* end if */

retearly:
	if (pip->debuglevel > 0) {
	    shio_printf(pip->efp,"%s: exiting ex=%u (%d)\n",
	        pip->progname,ex,rs) ;
	}

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("b_progcheck: exiting ex=%u (%d)\n",ex,rs) ;
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

#if	(CF_DEBUGS || CF_DEBUG) && CF_DEBUGMALL
	{
	    uint	mo ;
	    uc_mallout(&mo) ;
	    debugprintf("b_progcheck: final mallout=%u\n",(mo-mo_start)) ;
	    uc_mallset(0) ;
	}
#endif

#if	(CF_DEBUGS || CF_DEBUG)
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

local int usage(PI *pip) noex {
	int		rs = SR_OK ;
	int		wlen = 0 ;
	cchar	*pn = pip->progname ;
	cchar	*fmt ;

	fmt = "%s: USAGE> %s <file(s)>\n" ;
	if (rs >= 0) rs = shio_printf(pip->efp,fmt,pn,pn) ;
	wlen += rs ;

	fmt = "%s:  [-Q] [-D] [-v[=<n>]] [-HELP] [-V]\n" ;
	if (rs >= 0) rs = shio_printf(pip->efp,fmt,pn) ;
	wlen += rs ;

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (usage) */

/* process the program ako-options */
local int procopts(PI *pip,KO *kop) noex {
	LI		*lip = pip->lip ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		c = 0 ;
	cchar		*cp ;

	if (lip == nullptr) return SR_FAULT ;

	if ((cp = getourenv(pip->envv,VAROPTS)) != nullptr) {
	    rs = keyopt_loads(kop,cp,-1) ;
	}

	if (rs >= 0) {
	    if (KO_CUR kcur ; (rs = keyopt_curbegin(kop,&kcur)) >= 0) {
	        int	oi ;
	        int	kl, vl ;
	        cchar	*kp, *vp ;

	        while ((kl = keyopt_enumkeys(kop,&kcur,&kp)) >= 0) {

	            if ((oi = matostr(akonames,2,kp,kl)) >= 0) {

	                vl = keyopt_fetch(kop,kp,nullptr,&vp) ;

	                switch (oi) {
	                case akoname_bufwhole:
	                case akoname_whole:
	                    if (! pip->final.bufwhole) {
	                        pip->have.bufwhole = true ;
	                        pip->final.bufwhole = true ;
	                        pip->fl.bufwhole = true ;
	                        if (vl > 0) {
	                            rs = optbool(vp,vl) ;
	                            pip->fl.bufwhole = (rs > 0) ;
	                        }
	                    }
	                    break ;
	                case akoname_bufline:
	                case akoname_line:
	                    if (! pip->final.bufline) {
	                        pip->have.bufline = true ;
	                        pip->final.bufline = true ;
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
	                    if (! pip->final.bufnone) {
	                        pip->have.bufnone = true ;
	                        pip->final.bufnone = true ;
	                        pip->fl.bufnone = true ;
	                        if (vl > 0) {
	                            rs = optbool(vp,vl) ;
	                            pip->fl.bufnone = (rs > 0) ;
	                        }
	                    }
	                    break ;
	                case akoname_counts:
	                    if (! lip->final.counts) {
	                        lip->have.counts = true ;
	                        lip->final.counts = true ;
	                        lip->fl.counts = true ;
	                        if (vl > 0) {
	                            rs = optbool(vp,vl) ;
	                            lip->fl.counts = (rs > 0) ;
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

	        rs1 = keyopt_curend(kop,&kcur) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (keyopt-cur) */
	} /* end if (ok) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (procopts) */

local int procargs(PI *pip,AI *aip,BITS *bop,cchar *ofn,cchar *afn) noex {
	SHIO		ofile, *ofp = &ofile ;
	int		rs ;
	int		rs1 ;
	cchar		*pn = pip->progname ;
	cchar		*fmt ;

	if ((ofn == nullptr) || (ofn[0] == '\0') || (ofn[0] == '-'))
	    ofn = STDFNOUT ;

	if ((rs = shio_open(ofp,ofn,"wct",0666)) >= 0) {
	    int		pan = 0 ;
	    int		cl ;
	    cchar	*cp ;

	    if (rs >= 0) {
	        bool	f ;
	        cchar	**argv = aip->argv ;
	        for (int ai = 1 ; ai < aip->argc ; ai += 1) {

	            f = (ai <= aip->ai_max) && (bits_test(bop,ai) > 0) ;
	            f = f || ((ai > aip->ai_pos) && (argv[ai] != nullptr)) ;
	            if (f) {
	                cp = aip->argv[ai] ;
	                if (cp[0] != '\0') {
	                    pan += 1 ;
	                    rs = procfile(pip,ofp,cp) ;
	                }
	            }

	            if (rs >= 0) rs = lib_sigterm() ;
	            if (rs >= 0) rs = lib_sigintr() ;
	            if (rs < 0) break ;
	        } /* end for */
	    } /* end if (ok) */

	    if ((rs >= 0) && (afn != nullptr) && (afn[0] != '\0')) {
		if (char *lbuf ; (rs = malloc_ml(&lbuf)) >= 0) {
	            SHIO	afile, *afp = &afile ;
		    cint	llen = rs ;
	            if (strcmp(afn,"-") == 0) afn = STDFNIN ;
	            if ((rs = shio_open(afp,afn,"r",0666)) >= 0) {
	                while ((rs = shio_readln(afp,lbuf,llen)) > 0) {
	                    int	len = rs ;
	                    if (lbuf[len - 1] == '\n') len -= 1 ;
	                    lbuf[len] = '\0' ;
	                    if ((cl = sfshrink(lbuf,len,&cp)) > 0) {
	                        if (cp[0] != '#') {
	                            lbuf[(cp+cl)-lbuf] = '\0' ;
	                            pan += 1 ;
	                            rs = procfile(pip,ofp,lbuf) ;
	                        }
	                    }
	                    if (rs >= 0) rs = lib_sigterm() ;
	                    if (rs >= 0) rs = lib_sigintr() ;
	                    if (rs < 0) break ;
	                } /* end while (reading lines) */
	                rs1 = shio_close(afp) ;
	                if (rs >= 0) rs = rs1 ;
	        } else {
	            if (! pip->fl.quiet) {
	                fmt = "%s: inaccessible argument-list (%d)\n" ;
	                shio_printf(pip->efp,fmt,pn,rs) ;
	                shio_printf(pip->efp,"%s: afile=%s\n",pn,afn) ;
	            }
	        } /* end if */
		    rs1 = uc_free(lbuf) ;
		    if (rs >= 0) rs = rs1 ;
		    } /* end if (m-a-f) */
	    } /* end if (procesing file argument file list) */

	    rs1 = shio_close(ofp) ;
	    if (rs >= 0) rs = rs1 ;
	} else {
	    fmt = "%s: inaccessible output (%d)\n" ;
	    shio_printf(pip->efp,fmt,pn,rs) ;
	    shio_printf(pip->efp,"%s: ofile=%s\n",pn,ofn) ;
	}

	return rs ;
}
/* end subroutine (procargs) */

local int procfile(PI *pip,void *ofp,cchar *fn) noex {
	int		rs ;
	int		rs1 ;
	cchar		*pn = pip->progname ;
#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("progcheck/procfile: ent fn=%s\n",fn) ;
#endif
	if (char *lbuf ; (rs = malloc_ml(&lbuf)) >= 0) {
	    if (HIST h ; (rs = hist_start(&h)) >= 0) {
	        if (LS ls ; (rs = langstate_start(&ls)) >= 0) {
	            cint	ncca = nelem(cca) ;
	            SHIO	cfile, *cfp = &cfile ;
	            if ((rs = shio_open(cfp,fn,"r",0666)) >= 0) {
	                FC	counts[ncca + 1] ;
	                int	ln = 1 ;	/* line-number */
	                funcount_clear(counts) ;
	                while ((rs = shio_readln(cfp,lbuf,llen)) > 0) {
			    cchar *lp = lbuf ;
			    if (cint l = rmeol(lbuf,rs) ; ll > 0) {
	                        FC *c = counts ;
	                        if ((rs = procln(pip,&ls,c,ln,lp,ll)) >= 0) {
	                            rs = hist_proc(&h,ln,lp,ll) ;
	                        }
	                    } /* end if (rmeol) */
	                    ln += 1 ;
	                    if (rs < 0) break ;
	                } /* end while (reading lines) */
	                if (rs >= 0) {
	                    if ((rs = procout(pip,ofp,fn,counts)) >= 0) {
	                        if ((rs = procouthist(pip,ofp,&h)) >= 0) {
	                            rs = procoutlang(pip,ofp,&ls) ;
	                        }
	                    }
	                } /* end if (ok) */
	                rs1 = shio_close(cfp) ;
	                if (rs >= 0) rs = rs1 ;
	            } else {
	                cchar *fmt = "%s: inaccessible file (%d)\n" ;
	                shio_printf(pip->efp,fmt,pn,rs) ;
	                shio_printf(pip->efp,"%s: file=%s\n",pn,fn) ;
	            } /* end if */
	            rs1 = langstate_finish(&ls) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (langstate) */
	        rs1 = hist_finish(&h) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (hist) */
	    rs1 = uc_free(lbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("progcheck/procfile: ret rs=%d\n",rs) ;
#endif
	return rs ;
}
/* end subroutine (procfile) */

local int procln(PI *pip,LS *lsp,FC *cntp,int ln,cc *lp,int ll) noex {
	int		rs = SR_FAULT ;
	if (pip && lp) {
	    rs = SR_OK ;
	    for (int j = 0 ; j < ll ; j += 1) {
	        cint	ch = MKCHAR(lp[j]) ;
#if	CF_DEBUG
	        if (DEBUGLEVEL(4))
	            debugprintf("progcheck/procln: ln=%u\n",ln) ;
#endif
	        if ((rs = langstate_proc(lsp,ln,ch)) > 0) {
		    cint	ncca = nelem(cca) ;
	            for (int k = 0 ; k < ncca ; k += 1) {
	                if (ch == cca[k].c_open) {
	                    cntp[k].c_open += 1 ;
	                } else if (ch == cca[k].c_close) {
	                    cntp[k].c_close += 1 ;
	                }
	            } /* end for */
	        } /* end if (langstate_proc) */
	    } /* end for */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (procln) */

local int procout(PI *pip,void *ofp,cchar *fn,FC *counts) noex {
	LI		*lip = pip->lip ;
	cint		ncca = nelem(cca) ;
	int		rs = SR_OK ;
	if (lip->fl.counts) {
	    cchar	*pn = pip->progname ;
	    cchar	*fmt ;
	    for (int k = 0 ; k < ncca ; k += 1) {
	        if (counts[k].c_open != counts[k].c_close) {
	            fmt = "file \"%s\" %s> open=%-3d close=%-3d\n" ;

	            lip->fl.dirty = true ;
	            shio_printf(ofp,fmt,
	                fn,cca[k].funcname,
	                counts[k].c_open,counts[k].c_close) ;

	        } else {

	            if (pip->verboselevel > 0) {
	                fmt = "file \"%s\" %s> is clean\n" ;
	                shio_printf(ofp,fmt, fn,cca[k].funcname) ;
	            }

	            if (pip->debuglevel > 0) {
	                fmt = "%s: file \"%s\" %s> is clean\n" ;
	                shio_printf(pip->efp,fmt,pn,fn,cca[k].funcname) ;
	            }

	        } /* end if */
	    } /* end for */
	} /* end if (counts) */
	return rs ;
}
/* end subroutine (procout) */

local int procouthist(PI *pip,void *ofp,HIST *hlp) noex {
	cint		ncca = nelem(cca) ;
	int		rs = SR_OK ;
	for (int w = 0 ; w < ncca ; w += 1) {
	    if ((rs = hist_count(hlp,w)) > 0) {
	        LI	*lip = pip->lip ;
	        int	ln ;
	        int	t ;
	        cchar	*fmt = nullptr ;
	        lip->fl.dirty = true ;
	        switch (w) {
	        case 0:
	            fmt = "parentheses¬\n" ;
	            break ;
	        case 1:
	            fmt = "braces¬\n" ;
	            break ;
	        case 2:
	            fmt = "brackets¬\n" ;
	            break ;
	        } /* end switch */
	        if (fmt != nullptr) {
	            shio_printf(ofp,fmt) ;
	        }
	        for (int i = 0 ; (t = hist_get(hlp,w,i,&ln)) > 0 ; i += 1) {
	            cchar	*ts = chartype(t) ;
	            rs = shio_printf(ofp,"%s %u\n",ts,ln) ;
	            if (rs < 0) break ;
	        } /* end for */
	    } /* end if (positive) */
	    if (rs < 0) break ;
	} /* end for */
	return rs ;
}
/* end subroutine (procouthist) */

local int procoutlang(PI *pip,void *ofp,LS *lsp) noex {
	int		rs ;
#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("progcheck/procoutlang: ent\n") ;
#endif
	if (LS_STAT stat ; (rs = langstate_getstat(lsp,&stat)) > 0) {
	    LI		*lip = pip->lip ;
	    cchar	*cp = langstatetypes[stat.type] ;
	    cchar	*fmt = "unbalanced »%s« at line=%u\n" ;
#if	CF_DEBUG
	    if (DEBUGLEVEL(4))
	        debugprintf("progcheck/procoutlang: type=%u\n",stat.type) ;
#endif
	    lip->fl.dirty = true ;
	    rs = shio_printf(ofp,fmt,cp,stat.line) ;
	} /* end if (langstate_getstat) */
	return rs ;
}
/* end subroutine (procoutlang) */

local int locinfo_start(LI *lip,PI *pip) noex {
	int		rs = SR_OK ;

	if (lip == nullptr) return SR_FAULT ;

	memset(lip,0,sizeof(LI)) ;
	lip->pip = pip ;
	lip->to = -1 ;

	return rs ;
}
/* end subroutine (locinfo_start) */

local int locinfo_finish(LI *lip) noex {
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

#if	CF_LOCSETENT
local int locinfo_setentry(LI *lip,cchar **epp,cchar *vp,int vl) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (lip && epp) {
	    vecstr	*slp = &lip->stores ;
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
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (locinfo_setentry) */
#endif /* CF_LOCSETENT */

local int hist_start(HIST *hlp) noex {
	cint		n = PROGCHECK_NCH ;
	int		rs = SR_OK ;
	int		i ; /* used-afterwards */
	for (i = 0 ; (i < n) ; i += 1) {
	    linehist	*lhp = (hlp->types + i) ;
	    rs = linehist_start(lhp,balstrs[i]) ;
	    if (rs < 0) break ;
	} /* end for */
	if (rs < 0) {
	    for (int j = 0 ; j < i ; j += 1) {
	        linehist	*lhp = (hlp->types + j) ;
	        linehist_finish(lhp) ;
	    } /* end for */
	} /* end if (error) */
	return rs ;
}
/* end subroutine (hist_start) */

local int hist_finish(HIST *hlp) noex {
	cint		n = PROGCHECK_NCH ;
	int		rs = SR_OK ;
	int		rs1 ;
	for (int j = 0 ; j < n ; j += 1) {
	    linehist	*lhp = (hlp->types + j) ;
	    rs1 = linehist_finish(lhp) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end for */
	return rs ;
}
/* end subroutine (hist_finish) */

local int hist_proc(HIST *hlp,int ln,cchar *sp,int sl) noex {
	cint		n = PROGCHECK_NCH ;
	int		rs = SR_OK ;
	for (int i = 0 ; (rs >= 0) && (i < n) ; i += 1) {
	    linehist	*lhp = (hlp->types + i) ;
	    rs = linehist_proc(lhp,ln,sp,sl) ;
	} /* end for */
	return rs ;
}
/* end subroutine (hist_proc) */

local int hist_count(HIST *hlp,int w) noex {
	int		rs = SR_INVALID ;
	if (w < PROGCHECK_NCH) {
	    linehist	*lhp = (hlp->types + w) ;
	    rs = linehist_count(lhp) ;
	}
	return rs ;
}
/* end if (hist_count) */

local int hist_get(HIST *hlp,int w,int i,int *lnp) noex {
	int		rs = SR_INVALID ;
	if (w < PROGCHECK_NCH) {
	    linehist	*lhp = (hlp->types + w) ;
	    rs = linehist_get(lhp,i,lnp) ;
	}
	return rs ;
}
/* end if (hist_get) */

local int funcount_clear(FC *counts) noex {
	cint	ncca = nelem(cca) ;
	for (int i = 0 ; i < ncca ; i += 1) {
	    counts[i].c_open = 0 ;
	    counts[i].c_close = 0 ;
	} /* end for */
	return SR_OK ;
}
/* end subroutine (funcount_clear) */

static cchar *chartype(int w) noex {
	cchar		*cs = "unknown" ;
	if ((w >= 0) && (w < 3)) {
	    cs = chartypes[w] ;
	} /* end if */
	return cs ;
}
/* end subroutine (chartype) */



/* b_strerror SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* this is a SHELL built-in version of 'strerror(1)' */
/* last modified %G% version %I% */

#define	CF_DEBUGS	0		/* non-switchable debug print-outs */
#define	CF_DEBUG	0		/* switchable at invocation */
#define	CF_DEBUGMALL	1		/* debug memory allocation */
#define	CF_DEBUGHEX	0		/* debug w/ HEX */
#define	CF_BUFLINEIN	1		/* line-buffering for STDIN */
#define	CF_READINTR	0		/* |shio_readintr(3shio)| */
#define	CF_SPECIALFIFO	0		/* employ special FIFO handling */
#define	CF_LOCSETENT	0		/* allow |locinfo_setentry()| */

/* revision history:

	= 2004-03-01, David A­D­ Morano
	This subroutine was originally written as a KSH built-in command.

*/

/* Copyright © 2004 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	strerror

	Description:
	Display the string-code related to an error number.

	Synopsis:
	$ strerror <num(s)>

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
#include	<climits>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<bits.h>
#include	<keyopt.h>
#include	<vecstr.h>
#include	<char.h>
#include	<field.h>
#include	<strx.h>
#include	<mapex.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */
#include	<libdebug.h>		/* LIBDEBUG |DEBUGPRINTF(3debug)| */

#include	"shio.h"
#include	"kshlib.h"
#include	"b_strerror.h"
#include	"defs.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#ifndef	LINEBUFLEN
#ifdef	LINE_MAX
#define	LINEBUFLEN	MAX(2048,LINE_MAX)
#else
#define	LINEBUFLEN	2048
#endif
#endif /* LINEBUFLEN */

#ifndef	PI
#define	PI		proginfo
#endif

#define	LI		locinfo
#define	LI_FL		locinfo_fl


/* external subroutines */

extern int	printhelp(void *,cchar *,cchar *,cchar *) ;
extern int	proginfo_setpiv(PI *,cchar *,const pivars *) ;


/* external variables */

extern char	**environ ;		/* definition required by AT&T AST */


/* local structures */

struct locinfo_fl {
	uint		stores:1 ;
	uint		outer:1 ;
} ; /* end struct */

struct locinfo {
	vecstr		stores ;
	PI		*pip ;
	LI_FL		have, fl, changed, finval ;
	LI_FL		open ;
	int		to ;
} ; /* end struct */


/* forward references */

local int	mainsub(int,cchar **,cchar **,void *) ;

local int	usage(PI *) ;

local int	procopts(PI *,keyopt *) ;
local int	procargs(PI *,ARGINFO *,bits *,cchar *,cchar *) ;
local int	procspecs(PI *,void *,cchar *,int) ;
local int	procspec(PI *,void *,cchar *,int) ;

local int	locinfo_start(LI *,PI *) ;
local int	locinfo_finish(LI *) ;

#if	CF_LOCSETENT
local int	locinfo_setentry(LI *,cchar **,cchar *,int) ;
#endif


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
	argopt_overlast
} ; /* end enum */

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
	nullptr
} ; /* end array */

constexpr pivars	initvars = {
	VARPROGRAMROOT1,
	VARPROGRAMROOT2,
	VARPROGRAMROOT3,
	PROGRAMROOT,
	VARPRNAME
} ; /* end array */

constexpr MAXEX		mapexs[] = {
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
} ; /* end array */

enum akonames {
	akoname_bufwhole,
	akoname_bufline,
	akoname_bufnone,
	akoname_whole,
	akoname_line,
	akoname_none,
	akoname_un,
	akoname_empty,
	akoname_overlast
} ; /* end enum */

constexpr cpcchar	akonames[] = {
	"bufwhole",
	"bufline",
	"bufnone",
	"whole",
	"line",
	"none",
	"un",
	"",
	nullptr
} ; /* end array */

constexpr cpcchar	aterms[] = {
	0x00, 0x2E, 0x00, 0x00,
	0x09, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
} ; /* end array */


/* exported variables */


/* exported subroutines */

int b_strerror(int argc,con mainv argv,void *contextp) noex {
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
} /* end subroutine (b_strerror) */

int p_strerror(int argc,cchar *argv[],cchar *envv[],void *contextp) noex {
	return mainsub(argc,argv,envv,contextp) ;
} /* end subroutine (p_strerror) */


/* local subroutines */

/* ARGSUSED */
local int mainsub(int argc,con mainv argv,con mainv envv,void *contextp) noex {
	PI	pi, *pip = &pi ;
	LI		li, *lip = &li ;
	ARGINFO		ainfo ;
	bits		pargs ;
	keyopt		akopts ;
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

	cchar	*argp, *aop, *akp, *avp ;
	cchar	*argval = nullptr ;
	cchar	*pr = nullptr ;
	cchar	*sn = nullptr ;
	cchar	*afname = nullptr ;
	cchar	*efname = nullptr ;
	cchar	*ofname = nullptr ;
	cchar	*tos_open = nullptr ;
	cchar	*tos_read = nullptr ;
	cchar	*cp ;


#if	CF_DEBUGS || CF_DEBUG
	if ((cp = getourenv(envv,VARDEBUGFNAME)) != nullptr) {
	    rs = debugopen(cp) ;
	    debugprintf("b_strerror: starting DFD=%u\n",rs) ;
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
	        cint ach = MKCHAR(argp[1]) ;

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
	                        pip->f.quiet = true ;
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
	                        f_version = true ;
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

/* quiet mode */
	                    case 'q':
	                        pip->verboselevel = 0 ;
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
	                        pip->f.bufnone = true ;
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
	if (efname == nullptr) efname = STDERRFNAME ;
	if ((rs1 = shio_open(&errfile,efname,"wca",0666)) >= 0) {
	    pip->efp = &errfile ;
	    pip->open.errfile = true ;
	    shio_control(&errfile,SHIO_CSETBUFLINE,true) ;
	} else if (! isFailOpen(rs1)) {
	    if (rs >= 0) rs = rs1 ;
	}

#if	CF_DEBUGS
	debugprintf("b_strerror: args-out rs=%d\n",rs) ;
#endif

	if (rs < 0)
	    goto badarg ;

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    debugprintf("b_strerror: debuglevel=%u\n",pip->debuglevel) ;
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

#if	CF_DEBUG
	if (DEBUGLEVEL(2)) {
	    debugprintf("b_strerror: pr=%s\n",pip->pr) ;
	    debugprintf("b_strerror: sn=%s\n",pip->searchname) ;
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

	if (afname == nullptr) afname = getourenv(pip->envv,VARAFNAME) ;

	rs = procopts(pip,&akopts) ;

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

	if ((pip->debuglevel > 0) && 
	    ((pip->to_open >= 0) || (pip->to_read >= 0))) {
	    shio_printf(pip->efp,"%s: to_open=%d\n",
	        pip->progname,pip->to_open) ;
	    shio_printf(pip->efp,"%s: to_read=%d\n",
	        pip->progname,pip->to_read) ;
	}

#if	CF_DEBUG
	if (DEBUGLEVEL(2)) {
	    debugprintf("b_strerror: to_open=%d\n",pip->to_open) ;
	    debugprintf("b_strerror: to_read=%d\n",pip->to_read) ;
	    debugprintf("b_strerror: have.bufline=%u\n",pip->have.bufline) ;
	    debugprintf("b_strerror: have.bufnone=%u\n",pip->have.bufnone) ;
	    debugprintf("b_strerror: f_bufline=%u\n",pip->f.bufline) ;
	    debugprintf("b_strerror: f_bufnone=%u\n",pip->f.bufnone) ;
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
	} else if (ex == EX_OK) {
	    cchar	*pn = pip->progname ;
	    cchar	*fmt = "%s: invalid argument or configuration (%d)\n" ;
	    shio_printf(pip->efp,fmt,pn,rs) ;
	    ex = EX_USAGE ;
	    usage(pip) ;
	}

/* done */
	if ((rs < 0) && (ex == EX_OK)) {
	    if (! pip->f.quiet) {
	        cchar	*pn = pip->progname ;
	        cchar	*fmt = "%s: could not process (%d)\n" ;
	        shio_printf(pip->efp,fmt,pn,rs) ;
	    }
	    ex = mapex(mapexs,rs) ;
	} else if (rs >= 0) {
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
	    debugprintf("b_strerror: exiting ex=%u (%d)\n",ex,rs) ;
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
	    debugprintf("b_strerror: final mallout=%u\n",(mo-mo_start)) ;
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

} /* end subroutine (mainsub) */

local int usage(PI *pip) noex {
	int		rs = SR_OK ;
	int		wlen = 0 ;
	cchar	*pn = pip->progname ;
	cchar	*fmt ;

	fmt = "%s: USAGE> %s [<num(s)> ...] [-af <afile>] [-of <ofile>]\n" ;
	if (rs >= 0) rs = shio_printf(pip->efp,fmt,pn,pn) ;
	wlen += rs ;

	fmt = "%s:  [-Q] [-D] [-v[=<n>]] [-HELP] [-V]\n" ;
	if (rs >= 0) rs = shio_printf(pip->efp,fmt,pn) ;
	wlen += rs ;

	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (usage) */

/* process the program ako-options */
local int procopts(PI *pip,keyopt *kop) noex {
	int		rs = SR_OK ;
	int		c = 0 ;
	cchar	*cp ;

	if ((cp = getourenv(pip->envv,VAROPTS)) != nullptr) {
	    rs = keyopt_loads(kop,cp,-1) ;
	}

	if (rs >= 0) {
	    keyopt_cur	kcur ;
	    if ((rs = keyopt_curbegin(kop,&kcur)) >= 0) {
	        int	oi ;
	        int	kl, vl ;
	        cchar	*kp, *vp ;

	        while ((kl = keyopt_enumkeys(kop,&kcur,&kp)) >= 0) {

	            if ((oi = matostr(akonames,2,kp,kl)) >= 0) {

	                vl = keyopt_fetch(kop,kp,nullptr,&vp) ;

	                switch (oi) {

	                case akoname_bufwhole:
	                case akoname_whole:
	                    if (! pip->finval.bufwhole) {
	                        pip->have.bufwhole = true ;
	                        pip->finval.bufwhole = true ;
	                        pip->f.bufwhole = true ;
	                        if (vl > 0) {
	                            rs = optbool(vp,vl) ;
	                            pip->f.bufwhole = (rs > 0) ;
	                        }
	                    }
	                    break ;

	                case akoname_bufline:
	                case akoname_line:
	                    if (! pip->finval.bufline) {
	                        pip->have.bufline = true ;
	                        pip->finval.bufline = true ;
	                        pip->f.bufline = true ;
	                        if (vl > 0) {
	                            rs = optbool(vp,vl) ;
	                            pip->f.bufline = (rs > 0) ;
	                        }
	                    }
	                    break ;

	                case akoname_bufnone:
	                case akoname_none:
	                case akoname_un:
	                    if (! pip->finval.bufnone) {
	                        pip->have.bufnone = true ;
	                        pip->finval.bufnone = true ;
	                        pip->f.bufnone = true ;
	                        if (vl > 0) {
	                            rs = optbool(vp,vl) ;
	                            pip->f.bufnone = (rs > 0) ;
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
} /* end subroutine (procopts) */

local int procargs(PI *pip,ARGINFO *aip,bits *bop,cchar *ofn,cchar *afn) noex {
	SHIO		ofile, *ofp = &ofile ;
	cint		to = pip->to_open ;
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	cchar		*pn = pip->progname ;
	cchar		*fmt ;

	if ((ofn == nullptr) || (ofn[0] == '\0') || (ofn[0] == '-'))
	    ofn = STDOUTFNAME ;

	if ((rs = shio_opene(ofp,ofn,"wct",0666,to)) >= 0) {
	    int		pan = 0 ;
	    int		cl ;
	    cchar	*cp ;

	    if (rs >= 0) {
	        int	ai ;
	        int	f ;
	        cchar	**argv = aip->argv ;
	        for (ai = 1 ; ai < aip->argc ; ai += 1) {

	            f = (ai <= aip->ai_max) && (bits_test(bop,ai) > 0) ;
	            f = f || ((ai > aip->ai_pos) && (argv[ai] != nullptr)) ;
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
	        } /* end for */
	    } /* end if (ok) */

	    if ((rs >= 0) && (afn != nullptr) && (afn[0] != '\0')) {
	        SHIO	afile, *afp = &afile ;

	        if (strcmp(afn,"-") == 0) afn = STDINFNAME ;

	        if ((rs = shio_open(afp,afn,"r",0666)) >= 0) {
	            cint	llen = LINEBUFLEN ;
	            char	lbuf[LINEBUFLEN + 1] ;

	            while ((rs = shio_readline(afp,lbuf,llen)) > 0) {
	                int	len = rs ;

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
			fmt = "%s: inaccessible argument-list (%d)\n",
	                shio_printf(pip->efp,fmt,pn,rs) ;
	                shio_printf(pip->efp,"%s: afile=%s\n",pn,afn) ;
	            }
	        } /* end if */

	    } /* end if (procesing file argument file list) */

	    rs1 = shio_close(ofp) ;
	    if (rs >= 0) rs = rs1 ;
	} else {
	    fmt = "%s: inaccessible output (%d)\n" ;
	    shio_printf(pip->efp,fmt,pn,rs) ;
	    shio_printf(pip->efp,"%s: ofile=%s\n",pn,ofn) ;
	}

	if ((pip->debuglevel > 0) && (rs >= 0)) {
	    shio_printf(pip->efp,"%s: written=%u\n",
	        pip->progname,wlen) ;
	}

	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (procargs) */

local int procspecs(PI *pip,void *ofp,cchar *lbuf,int llen) noex {
	int		rs ;
	int		wlen = 0 ;
	if (field fsb ; (rs = field_start(&fsb,lbuf,llen)) >= 0) {
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
} /* end subroutine (procspecs) */

/* process a specification name */
local int procspec(PI *pip,void *ofp,cchar *namp,int nl) noex {
	int		rs = SR_OK ;
	int		wlen = 0 ;

	if (pip == nullptr) return SR_FAULT ;

	if (nl < 0) nl = strlen(namp) ;

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    debugprintf("b_strerror/procspec: namp=%r\n",namp,nl) ;
#endif

	if (nl > 0) {
	    int		en ;
	    if ((rs = cfdeci(namp,nl,&en)) >= 0) {
	        cchar	*se = strerror(en) ;
	        cchar	*sr = strabbrerr(-en) ;
	        if (se == nullptr) se = "*unknown*" ;
	        rs = shio_printf(ofp,"%4u %s ­ %s\n",en,sr,se) ;
	        wlen += rs ;
	    }
	}

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    debugprintf("b_strerror/procspec: ret rs=%d wlen=%u\n",rs,wlen) ;
#endif

	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (procspec) */

local int locinfo_start(LI *lip,PI *pip) noex {
	int		rs = SR_OK ;
	if (lip == nullptr) return SR_FAULT ;
	memclear(lip) ;
	lip->pip = pip ;
	lip->to = -1 ;
	return rs ;
} /* end subroutine (locinfo_start) */

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
} /* end subroutine (locinfo_finish) */

#if	CF_LOCSETENT
int locinfo_setentry(LI *lip,cchar **epp,cchar *vp,int vl) noex {
	vecstr		*slp ;
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
} /* end subroutine (locinfo_setentry) */
#endif /* CF_LOCSETENT */



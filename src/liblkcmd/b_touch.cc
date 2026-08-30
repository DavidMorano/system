/* b_touch SUPPORT (KSH builtin) */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* this is a SHELL built-in version of 'touch(1)' */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* switchable at invocation */
#define	CF_DEBUGMALL	1		/* debug memory-allocations */
#define	CF_tmtime	0		/* use |tmtime(3dam)| */

/* revision history:

	= 2004-03-01, David A­D­ Morano
	This was written when we discovered that the SHELL (KSH)
	can be enhanced with plugin built-in functions.  Many thanks
	are due to David Korn for his shell!

*/

/* Copyright © 2004 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	b_touch

	Synopsis:
	$ touch [-acm] [{-t datespec}|datespec] files(s) [-af <afile>]

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */

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
#include	<sys/time.h>		/* for |utimes(2)| */
#include	<climits>
#include	<utime.h>		/* for |utime(2)| */
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<bits.h>
#include	<keyopt.h>
#include	<tmz.hh>
#include	<tmtime.hh>
#include	<exitcodes.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */
#include	<libdebug.h>		/* LIBDEBUG |DEBUGPRINTF(3debug)| */

#include	"shio.h"
#include	"kshlib.h"
#include	"b_touch.h"
#include	"defs.h"


/* local defines */

#ifndef	LOGNAMELEN
#define	LOGNAMELEN	32
#endif

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

#define	TOUCH_INFO	struct touch_info

#define	LOCINFO		struct locinfo
#define	LOCINFO_FL	struct locinfo_flags


/* external subroutines */

extern int	printhelp(void *,cchar *,cchar *,cchar *) ;
extern int	proginfo_setpiv(PROGINFO *,cchar *,const struct pivars *) ;


/* external variables */

extern char	**environ ;		/* definition required by AT&T AST */


/* local structures */

struct locinfo_flags {
	uint		toucht:1 ;
	uint		access:1 ;
	uint		nocreate:1 ;
	uint		modify:1 ;
	uint		gmt:1 ;
} ;

struct locinfo {
	LOCINFO_FL	have, f, changed, finval ;
	PROGINFO	*pip ;
} ;

struct touch_info {
	time_t		atime ;
	time_t		mtime ;
	int		f_current ;
} ;


/* forward references */

local int	mainsub(int,cchar **,cchar **,void *) ;

local int	usage(PROGINFO *) ;

local int	gettimes(PROGINFO *,TOUCH_INFO *,cchar *) ;
local int	procargs(PROGINFO *,ARGINFO *,bits *,TOUCH_INFO *,cchar *) ;
local int	procfile(PROGINFO *,TOUCH_INFO *,cchar *) ;

local int	locinfo_start(LOCINFO *,PROGINFO *) ;
local int	locinfo_finish(LOCINFO *) ;


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
	argopt_if,
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


/* exported subroutines */


int b_touch(int argc,cchar *argv[],void *contextp)
{
	int		rs ;
	int		rs1 ;
	int		ex = EX_OK ;

	if ((rs = lib_kshbegin(contextp,nullptr)) >= 0) {
	    cchar	**envv = (const char **) environ ;
	    ex = mainsub(argc,argv,envv,contextp) ;
	    rs1 = lib_kshend() ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (ksh) */

	if ((rs < 0) && (ex == EX_OK)) ex = EX_DATAERR ;

	return ex ;
}
/* end subroutine (b_touch) */


int p_touch(int argc,cchar *argv[],cchar *envv[],void *contextp)
{
	return mainsub(argc,argv,envv,contextp) ;
}
/* end subroutine (p_touch) */


/* local subroutines */


/* ARGSUSED */
local int mainsub(int argc,cchar *argv[],cchar *envv[],void *contextp)
{
	PROGINFO	pi, *pip = &pi ;
	LOCINFO		li, *lip = &li ;
	ARGINFO		ainfo ;
	TOUCH_INFO	spi ;
	bits		pargs ;
	keyopt		akopts ;
	SHIO		errfile ;

#if	(CF_DEBUG || CF_DEBUG) && CF_DEBUGMALL
	uint		mo_start = 0 ;
#endif

	int		argr, argl, aol, akl, avl, kwi ;
	int		ai, ai_max, ai_pos, ai_continue ;
	int		rs, rs1 ;
	int		ex = EX_INFO ;
	int		f_optminus, f_optplus, f_optequal ;
	int		f_version = false ;
	int		f_usage = false ;
	int		f_help = false ;
	int		f ;

	const char	*argp, *aop, *akp, *avp ;
	const char	*argval = nullptr ;
	const char	*pr = nullptr ;
	const char	*sn = nullptr ;
	const char	*afname = nullptr ;
	const char	*efname = nullptr ;
	const char	*ofname = nullptr ;
	const char	*reffname = nullptr ;
	const char	*datespec = nullptr ;
	const char	*cp ;


#if	CF_DEBUG || CF_DEBUG
	if ((cp = getourenv(envv,VARDEBUGFNAME)) != nullptr) {
	    rs = debugopen(cp) ;
	    DEBUGPRINTF("b_touch: starting DFD=%d\n",rs) ;
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

	pip->lip = &li ;
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

/* keyword match or only key letters ? */

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

	                case argopt_of:
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

/* version */
	                    case 'V':
	                        f_version = true ;
	                        break ;

/* access time */
	                    case 'a':
	                        lip->fl.access = true ;
	                        break ;

/* do NOT create file */
	                    case 'c':
	                        lip->fl.nocreate = true ;
	                        break ;

/* the old BSD "force" option (ignored) */
	                    case 'f':
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

/* modification time */
	                    case 'm':
	                        lip->fl.modify = true ;
	                        break ;

/* reference file */
	                    case 'r':
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl)
	                                reffname = argp ;
	                        } else
	                            rs = SR_INVALID ;
	                        break ;

/* time specification */
	                    case 't':
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl) {
	                                lip->fl.toucht = true ;
	                                datespec = argp ;
	                            }
	                        } else
	                            rs = SR_INVALID ;
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

/* use GMT */
	                    case 'z':
	                        lip->finval.gmt = true ;
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
	    DEBUGPRINTF("b_touch: debuglevel=%u\n",pip->debuglevel) ;
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
	    printhelp(nullptr,pip->pr,pip->searchname,HELPFNAME) ;
#endif
	}

	if (f_version || f_help || f_usage)
	    goto retearly ;


	ex = EX_OK ;

/* some initialization */

	if (afname == nullptr) afname = getourenv(envv,VARAFNAME) ;

	if (ofname == nullptr) ofname = getourenv(envv,VAROFNAME) ;

	memset(&spi,0,sizeof(TOUCH_INFO)) ;

/* use the first positional argument as the date-spec if we do not have it */

	ai_continue = 1 ;
	if (reffname == nullptr) {
	    time_t	t = 0 ;

	    if (datespec == nullptr) {

#if	CF_DEBUG
	        if (DEBUGLEVEL(4))
	            DEBUGPRINTF("b_touch: no datespec\n") ;
#endif

	        for (ai = ai_continue ; ai < argc ; ai += 1) {

	            f = (ai <= ai_max) && (bits_test(&pargs,ai) > 0) ;
	            f = f || ((ai > ai_pos) && (argv[ai] != nullptr)) ;
	            if (f) {
	                cp = argv[ai] ;
	                if (cp[0] != '\0') {
	                    cint	ch = MKCHAR(cp[0]) ;
	                    if (isdigitlatin(ch)) {
	                        datespec = cp ;
	                        ai_continue = (ai + 1) ;
	                        bits_clear(&pargs,ai) ;
	                    }
	                }
	                break ;
	            }

	        } /* end for */

	    } /* end if (no date specification) */

#if	CF_DEBUG
	    if (DEBUGLEVEL(4))
	        DEBUGPRINTF("b_touch: datespec=>%s<\n",datespec) ;
#endif

	    if (datespec != nullptr) {
	        tmz	stz ;
	        tmtime	tmt ;

	        if (lip->fl.toucht) {
	            rs = tmz_xtoucht(&stz,datespec,-1) ;
	        } else {
	            rs = tmz_xtouch(&stz,datespec,-1) ;
	        }

#if	CF_DEBUG
	        if (DEBUGLEVEL(4)) {
	            DEBUGPRINTF("b_touch: tmz_xxx() rs=%d\n",rs) ;
	            DEBUGPRINTF("b_touch: f_year=%u\n",stz.f.year) ;
	            DEBUGPRINTF("b_touch: f_zoff=%u\n",stz.f.zoff) ;
	            DEBUGPRINTF("b_touch: year=%d\n",stz.st.tm_year) ;
	            DEBUGPRINTF("b_touch: mon=%d\n",stz.st.tm_mon) ;
	            DEBUGPRINTF("b_touch: mday=%d\n",stz.st.tm_mday) ;
	            DEBUGPRINTF("b_touch: hour=%d\n",stz.st.tm_hour) ;
	            DEBUGPRINTF("b_touch: min=%d\n",stz.st.tm_min) ;
	            DEBUGPRINTF("b_touch: sec=%d\n",stz.st.tm_sec) ;
	            DEBUGPRINTF("b_touch: isdst=%d\n",stz.st.tm_isdst) ;
	            DEBUGPRINTF("b_touch: zo=%d\n",stz.zoff) ;
	            DEBUGPRINTF("b_touch: zn=%s\n",stz.zname) ;
	        }
#endif /* CF_DEBUG */

	        if ((rs >= 0) && (tmz.hhasyear(&stz) == 0)) {
	            t = time(nullptr) ;
	            rs = tmtime_timelocal(&tmt,t) ;
	            tmz_setyear(&stz,tmt.year) ;
	        } /* end if (getting the current year) */

	        if (rs >= 0) {

#if	CF_tmtime
	            tmtime_insert(&tmt,&stz.st) ;
	            rs = tmtime_mktime(&tmt,&t) ;
#if	CF_DEBUG
	            if (DEBUGLEVEL(4))
	                DEBUGPRINTF("b_touch: tmtime_mktime() rs=%d\n",rs) ;
#endif
#else /* CF_tmtime */
	            rs = uc_mktime(&stz.st,&t) ;
#if	CF_DEBUG
	            if (DEBUGLEVEL(4))
	                DEBUGPRINTF("b_touch: uc_mktime() rs=%d\n",rs) ;
#endif
#endif /* CF_tmtime */

#if	CF_DEBUG
	            if (DEBUGLEVEL(4)) {
	                DEBUGPRINTF("b_touch: isdst=%d\n",stz.st.tm_isdst) ;
	            }
#endif

	        } /* end if */

	        spi.atime = t ;
	        spi.mtime = t ;

	    } else {

#ifdef	COMMENT
	        t = time(nullptr) ;
	        spi.atime = t ;
	        spi.mtime = t ;
#else /* COMMENT */
	        spi.f_current = true ;
#endif /* COMMENT */

	    } /* end if */

	} else {
	    rs = gettimes(pip,&spi,reffname) ;
	}

#if	CF_DEBUG
	if (DEBUGLEVEL(4)) {
	    char	timebuf[TIMEBUFLEN + 1] ;
	    DEBUGPRINTF("b_touch: atime=%s\n",
	        timestr_log(spi.atime,timebuf)) ;
	    DEBUGPRINTF("b_touch: mtime=%s\n",
	        timestr_log(spi.mtime,timebuf)) ;
	}
#endif /* CF_DEBUG */

/* continue and pop everything */

	memset(&ainfo,0,sizeof(ARGINFO)) ;
	ainfo.argc = argc ;
	ainfo.ai = ai ;
	ainfo.argv = argv ;
	ainfo.ai_max = ai_max ;
	ainfo.ai_pos = ai_pos ;
	ainfo.ai_continue = ai_continue ;

	if (rs >= 0) {
	    const char	*afn = afname ;
	    rs = procargs(pip,&ainfo,&pargs,&spi,afn) ;
	} else if (ex == EX_OK) {
	    cchar	*pn = pip->progname ;
	    cchar	*fmt = "%s: invalid argument or configuration (%d)\n" ;
	    shio_printf(pip->efp,fmt,pn,rs) ;
	    ex = EX_USAGE ;
	    usage(pip) ;
	}

/* done */
	if ((rs < 0) && (ex == EX_OK)) {
	    ex = mapex(mapexs,rs) ;
	    if (! pip->fl.quiet) {
	        shio_printf(pip->efp,
	            "%s: processing error (%d)\n",
	            pip->progname,rs) ;
	    }
	} else if ((rs >= 0) && (ex == EX_OK)) {
	    if ((rs = lib_sigterm()) < 0) {
	        ex = EX_TERM ;
	    } else if ((rs = lib_sigintr()) < 0) {
	        ex = EX_INTR ;
	    }
	} /* end if */

/* we are done */
retearly:
	if (pip->debuglevel > 0) {
	    shio_printf(pip->efp,"%s: exiting ex=%u (%d)\n",
	        pip->progname,ex,rs) ;
	}

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
	    DEBUGPRINTF("b_touch: final mallout=%u\n",(mo-mo_start)) ;
	    uc_mallset(0) ;
	}
#endif /* CF_DEBUGMALL */

#if	(CF_DEBUG || CF_DEBUG)
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


local int usage(PROGINFO *pip)
{
	int		rs = SR_OK ;
	int		wlen = 0 ;
	const char	*pn = pip->progname ;
	const char	*fmt ;

	fmt = "%s: USAGE> %s [{-t [[CC]YY]MMDDhhmm[.ss]}|MMDDhhmm[YY]] "
	    "<files(s)>\n" ;
	if (rs >= 0) rs = shio_printf(pip->efp,fmt,pn,pn) ;
	wlen += rs ;

	fmt = "%s:  [-af <afile>] [-acm] [-r <reffile>] \n" ;
	if (rs >= 0) rs = shio_printf(pip->efp,fmt,pn) ;
	wlen += rs ;

	fmt = "%s:  [-Q] [-D] [-v[=<n>]] [-HELP] [-V]\n" ;
	if (rs >= 0) rs = shio_printf(pip->efp,fmt,pn) ;
	wlen += rs ;

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (usage) */


local int locinfo_start(LOCINFO *lip,PROGINFO *pip)
{

	memset(lip,0,sizeof(LOCINFO)) ;
	lip->pip = pip ;

	return SR_OK ;
}
/* end subroutine (locinfo_start) */


local int locinfo_finish(LOCINFO *lip)
{
	int		rs = SR_OK ;

	if (lip == nullptr) return SR_FAULT ;

	return rs ;
}
/* end subroutine (locinfo_finish) */


local int gettimes(PROGINFO *pip,TOUCH_INFO *spip,cchar reffname[])
{
	ustat	sb ;
	int		rs ;

	if ((rs = u_stat(reffname,&sb)) >= 0) {
	    spip->atime = sb.st_atime ;
	    spip->mtime = sb.st_mtime ;
	} else {
	    spip->atime = time(nullptr) ;
	    spip->mtime = spip->atime ;
	}

	return rs ;
}
/* end subroutine (gettimes) */


local int procargs(PROGINFO *pip,ARGINFO *aip,bits *bop,TOUCH_INFO *spip,
		cchar *afn)
{
	int		rs = SR_OK ;
	int		rs1 ;
	int		pan = 0 ;
	cchar		*pn = pip->progname ;
	cchar		*fmt ;
	cchar		*cp ;

	if (rs >= 0) {
	    int	ai ;
	    int	f ;
	    for (ai = aip->ai_continue ; ai < aip->argc ; ai += 1) {

	        f = (ai <= aip->ai_max) && (bits_test(bop,ai) > 0) ;
	        f = f || ((ai > aip->ai_pos) && (aip->argv[ai] != nullptr)) ;
	        if (f) {
	            cp = aip->argv[ai] ;
	            if (cp[0] != '\0') {
	                pan += 1 ;
	                rs = procfile(pip,spip,cp) ;
	            }
	        }

	        if (rs < 0) break ;
	    } /* end for */
	} /* end if (ok) */

	if ((rs >= 0) && (afn != nullptr) && (afn[0] != '\0')) {
	    SHIO	afile, *afp = &afile ;

	    if (strcmp(afn,"-") == 0) afn = STDFNIN ;

	    if ((rs = shio_open(afp,afn,"r",0666)) >= 0) {
	        cint	llen = LINEBUFLEN ;
	        int		len ;
	        char		lbuf[LINEBUFLEN + 1] ;

	        while ((rs = shio_readline(afp,lbuf,llen)) > 0) {
	            len = rs ;

	            if (lbuf[len - 1] == '\n') len -= 1 ;
	            lbuf[len] = '\0' ;

	            if (len > 0) {
	                pan += 1 ;
	                rs = procfile(pip,spip,lbuf) ;
	            }

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

	if ((rs >= 0) && (pan == 0)) {
	    rs = SR_INVALID ;
	    shio_printf(pip->efp,"%s: no files specified\n",pn) ;
	} /* end if */

	return rs ;
} /* end subroutine (procargs) */

local int procfile(PROGINFO *pip,TOUCH_INFO *spip,cchar *fname) noex {
	LOCINFO		*lip = pip->lip ;
	cint	nrs = SR_NOENT ;
	int		rs ;
	int		f_continue = true ;

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    DEBUGPRINTF("touch/procfile: fname=%s\n",fname) ;
#endif

	if (fname == nullptr)
	    return SR_FAULT ;

	if (ustat sb ; (rs = u_stat(fname,&sb)) == nrs) {
	    if (! lip->fl.nocreate) {
	        if ((rs = u_creat(fname,0666)) >= 0) {
	            int	fd = rs ;
	            rs = u_fstat(fd,&sb) ;
	            u_close(fd) ;
	        } /* end if (creating file) */
	    } else {
	        rs = SR_OK ;
	        f_continue = false ;
	    }
	} /* end if (attempt to create) */

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    DEBUGPRINTF("touch/procfile: final u_stat() rs=%d\n",rs) ;
#endif

	if ((rs >= 0) && f_continue) {
	    UTIMBUF	ut{} ;
	    ut.actime = sb.st_atime ;
	    ut.modtime = sb.st_mtime ;
	    if (lip->fl.access) ut.actime = spip->atime ;
	    if (lip->fl.modify) ut.modtime = spip->mtime ;
	    if ((! lip->fl.access) && (! lip->fl.modify)) {
	        ut.actime = spip->atime ;
	        ut.modtime = spip->mtime ;
	    }
	    if (spip->f_current) {
	        rs = uc_filetime(fname,nullptr) ;
	    } else {
	        rs = uc_filetime(fname,&ut) ;
	    }
	} /* end if (ok) */

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    DEBUGPRINTF("touch/procfile: ret rs=%d \n",rs) ;
#endif

	return rs ;
} /* end subroutine (procfile) */



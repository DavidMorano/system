/* b_lastlog SUPPORT (KSH builtin) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* (fairly) generic front-end subroutine */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* non-switchable debug print-outs */
#define	CF_DEBUG	0		/* switchable debug print-outs */
#define	CF_DEBUGMALL	1		/* debug memory-allocations */
#define	CF_UGETPW	1		/* use |ugetpw(3uc)| */
#define	CF_LOCSETENT	0		/* |locinfo_setentry()| */

/* revision history:

	= 1998-03-01, David A­D­ Morano
	The program was written from scratch to do what the previous
	program by the same name did.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	b_lastlog

	Description:
	Display the Last-Log entry for given user(s).

	Synopsis:
	$ lastlog [<username(s)>] [-af <afile>] [-a] [-llf <lastlog>] [-V]

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
#include	<cstdlib>
#include	<cstring>
#include	<pwd.h>
#include	<usystem.h>
#include	<getpwentry.h>
#include	<sysusernames.h>
#include	<bits.h>
#include	<keyopt.h>
#include	<paramopt.h>
#include	<vecstr.h>
#include	<setostr.h>
#include	<field.h>
#include	<getax.h>
#include	<getpwx.h>
#include	<userinfo.h>
#include	<logfile.h>
#include	<field.h>
#include	<lastlogfile.h>
#include	<pwfile.h>
#include	<grmems.h>
#include	<sysrealname.h>
#include	<exitcodes.h>
#include	<localmisc.h>
#include	<libdebug.h>		/* LIBDEBUG |DEBUGPRINTF(3debug)| */

#include	"shio.h"
#include	"kshlib.h"
#include	"b_lastlog.h"
#include	"defs.h"
#include	"proglog.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#define	LOCINFO		struct locinfo
#define	LOCINFO_FL	struct locinfo_flags
#define	LOCINFO_GMCUR	struct locinfo_gmcur
#define	LOCINFO_RNCUR	struct locinfo_rncur

#ifndef	BUFLEN
#define	BUFLEN		LINEBUFLEN
#endif


/* external subroutines */

extern int	printhelp(void *,cchar *,cchar *,cchar *) ;
extern int	proginfo_setpiv(PROGINFO *,cchar *,const struct pivars *) ;

extern int	proguserlist_begin(PROGINFO *) ;
extern int	proguserlist_end(PROGINFO *) ;


/* external variables */

extern char	**environ ;		/* definition required by AT&T AST */


/* local structures */

struct locinfo_gmcur {
	GRMEMS_CUR	gmcur ;
} ; /* end struct */

struct locinfo_rncur {
	SYSREALNAME_CUR	rncur ;
} ; /* end struct */

struct locinfo_flags {
	uint		stores:1 ;
	uint		nouser:1 ;
	uint		linebuf:1 ;
	uint		all:1 ;
	uint		realname:1 ;
	uint		name:1 ;
	uint		fullname:1 ;
	uint		gm:1 ;
	uint		rn:1 ;
	uint		sysuser:1 ;
	uint		reguser:1 ;
	uint		speuser:1 ;
	uint		hdr:1 ;
	uint		sort:1 ;
	uint		reverse:1 ;
} ; /* end struct */

struct locinfo {
	LOCINFO_FL	have, f, changed, finval ;
	LOCINFO_FL	open ;
	PROGINFO	*pip ;
	vecstr		stores ;
	GRMEMS		gm ;
	SYSREALNAME	rn ;
	char		unbuf[USERNAMELEN+1] ;
	char		gnbuf[GROUPNAMELEN+1] ;
} ; /* end struct */


/* forward references */

local int	mainsub(int,cchar **,cchar **,void *) ;

local int	usage(PROGINFO *) ;

local int	procopts(PROGINFO *,keyopt *) ;

local int	procuserinfo_begin(PROGINFO *,USERINFO *) ;
local int	procuserinfo_end(PROGINFO *) ;
local int	procuserinfo_logid(PROGINFO *) ;

local int	process(PROGINFO *,ARGINFO *,bits *,cchar *,cchar *,cchar *) ;
local int	procer(PROGINFO *,ARGINFO *,bits *,void *,cchar *,cchar *) ;
local int	procloadnames(PROGINFO *,OSETSTR *,cchar *,int) ;
local int	procloadname(PROGINFO *,OSETSTR *,cchar *,int) ;
local int	procerall(PROGINFO *,void *,LASTLOGFILE *) ;
local int	procerargs(PROGINFO *,ARGINFO *,bits *,void *,
			LASTLOGFILE *,cchar *) ;
local int	procerarg(PROGINFO *,void *,LASTLOGFILE *,cchar *) ;
local int	display(PROGINFO *,void *,cchar *,time_t,cchar *,cchar *) ;

local int	locinfo_start(LOCINFO *,PROGINFO *) ;
local int	locinfo_finish(LOCINFO *) ;
local int	locinfo_username(LOCINFO *) ;
local int	locinfo_groupname(LOCINFO *) ;
local int	locinfo_gmcurbegin(LOCINFO *,LOCINFO_GMCUR *) ;
local int	locinfo_gmcurend(LOCINFO *,LOCINFO_GMCUR *) ;
local int	locinfo_gmlook(LOCINFO *,LOCINFO_GMCUR *,cchar *,int) ;
local int	locinfo_gmread(LOCINFO *,LOCINFO_GMCUR *,char *,int) ;
local int	locinfo_rncurbegin(LOCINFO *,LOCINFO_RNCUR *) ;
local int	locinfo_rncurend(LOCINFO *,LOCINFO_RNCUR *) ;
local int	locinfo_rnlook(LOCINFO *,LOCINFO_RNCUR *,cchar *,int) ;
local int	locinfo_rnread(LOCINFO *,LOCINFO_RNCUR *,char *,int) ;

#if	CF_LOCSETENT
local int	locinfo_setentry(LOCINFO *,cchar **,cchar *,int) ;
#endif


/* local variables */

enum argopts {
	argopt_root,
	argopt_version,
	argopt_verbose,
	argopt_tmpdname,
	argopt_config,
	argopt_help,
	argopt_all,
	argopt_sn,
	argopt_af,
	argopt_ef,
	argopt_of,
	argopt_lf,
	argopt_db,
	argopt_llf,
	argopt_overlast
} ; /* end enum */

constexpr cpcchar	argopts[] = {
	"ROOT",
	"VERSION",
	"VERBOSE",
	"TMPDIR",
	"CONFIG",
	"HELP",
	"all",
	"sn",
	"af",
	"ef",
	"of",
	"lf",
	"db",
	"llf",
	nullptr
} ; /* end array */

constexpr PIVARS	initvars = {
	VARPROGRAMROOT1,
	VARPROGRAMROOT2,
	VARPROGRAMROOT3,
	PROGRAMROOT,
	VARPRLOCAL
} ; /* end array */

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
} ; /* end array */

enum akonames {
	akoname_linebuf,
	akoname_all,
	akoname_realname,
	akoname_pcsname,
	akoname_name,
	akoname_fullname,
	akoname_sysuser,
	akoname_reguser,
	akoname_speuser,
	akoname_hdr,
	akoname_overlast
} ; /* end enum (alonames) */

constexpr cpcchar	akonames[] = {
	"linebuf",
	"all",
	"realname",
	"pcsname",
	"name",
	"fullname",
	"sysuser",
	"reguser",
	"speuser",
	"hdr",
	nullptr
} ; /* end array */

constexpr char		aterms[] = {
	0x00, 0x2E, 0x00, 0x00,
	0x09, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
} ; /* end array */

#ifdef	COMMENT
constexpr cpcchar	outopts[] = {
	"host",
	"line",
	"time",
	nullptr
} ; /* end array */
#endif /* COMMENT */

#ifdef	COMMENT
enum outopts {
	outopt_host,
	outopt_line,
	outopt_time,
	outopt_overlast
} ; /* end enum */
#endif /* COMMENT */


/* exported variables */


/* exported subroutines */

int b_lastlog(int argc,mainv argv,void *contextp) noex {
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
} /* end subroutine (b_lastlog) */

int p_lastlog(int argc,mainv argv,mainv envv,void *contextp) noex {
	return mainsub(argc,argv,envv,contextp) ;
} /* end subroutine (p_lastlog) */


/* local subroutines */

local int mainsub(int argc,mainv argv,mainv envv,void *contextp) noex {
	PROGINFO	pi, *pip = &pi ;
	LOCINFO		li, *lip = &li ;
	ARGINFO		ainfo ;
	bits		pargs ;
	keyopt		akopts ;
	PARAMOPT	aparams ;
	SHIO		errfile ;

#if	(CF_DEBUGS || CF_DEBUG) && CF_DEBUGMALL
	uint		mo_start = 0 ;
#endif

	int		argr, argl, aol, akl, avl, kwi ;
	int		ai, ai_max, ai_pos ;
	int		rs, rs1 ;
	int		ex = EX_INFO ;
	int		f_optminus, f_optplus, f_optequal ;
	int		f_usage = false ;
	int		f_version = false ;
	int		f_help = false ;

	cchar		*argp, *aop, *akp, *avp ;
	cchar		*argval = nullptr ;
	cchar		*pr = nullptr ;
	cchar		*sn = nullptr ;
	cchar		*afname = nullptr ;
	cchar		*efname = nullptr ;
	cchar		*ofname = nullptr ;
	cchar		*llfname = nullptr ;
	cchar		*cp ;


#if	CF_DEBUGS || CF_DEBUG
	if ((cp = getourenv(envv,VARDEBUGFNAME)) != nullptr) {
	    rs = debugopen(cp) ;
	    debugprintf("main: starting DFD=%d\n",rs) ;
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

/* early things to initialize */

	pip->verboselevel = 1 ;
	pip->fl.logprog = OPT_LOGPROG ;

	pip->lip = lip ;
	if (rs >= 0) rs = locinfo_start(lip,pip) ;
	if (rs < 0) {
	    ex = EX_OSERR ;
	    goto badlocstart ;
	}

/* process program arguments */

	if (rs >= 0) rs = bits_start(&pargs,0) ;
	if (rs < 0) goto badpargs ;

	rs = keyopt_start(&akopts) ;
	pip->open.akopts = (rs >= 0) ;

	if (rs >= 0) {
	    rs = paramopt_start(&aparams) ;
	    pip->open.aparams = (rs >= 0) ;
	}

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

	            argval = (argp + 1) ;

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
	                    break ;

/* verbose */
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

/* temporary directory */
	                case argopt_tmpdname:
	                    if (f_optequal) {
	                        f_optequal = false ;
	                        if (avl)
	                            pip->tmpdname = avp ;
	                    } else {
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl)
	                                pip->tmpdname = argp ;
	                        } else
	                            rs = SR_INVALID ;
	                    }
	                    break ;

/* get a program root */
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

	                case argopt_all:
	                    pip->fl.all = true ;
	                    if (f_optequal) {
	                        f_optequal = false ;
	                        if (avl) {
	                            rs = optbool(avp,avl) ;
	                            pip->fl.all = (rs > 0) ;
	                        }
	                    }
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

/* log-file */
	                case argopt_lf:
	                    if (f_optequal) {
	                        f_optequal = false ;
	                        if (avl)
	                            pip->lfname = avp ;
	                    } else {
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl)
	                                pip->lfname = argp ;
	                        } else
	                            rs = SR_INVALID ;
	                    }
	                    break ;

/* database file */
	                case argopt_llf:
	                case argopt_db:
	                    if (f_optequal) {
	                        f_optequal = false ;
	                        if (avl)
	                            llfname = avp ;
	                    } else {
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl)
	                                llfname = argp ;
	                        } else
	                            rs = SR_INVALID ;
	                    }
	                    break ;

/* default action and user specified help */
	                default:
	                    rs = SR_INVALID ;
	                    f_usage = true ;
	                    break ;

	                } /* end switch (key words) */

	            } else {

	                while (akl--) {
	                    cint	kc = MKCHAR(*akp) ;

	                    switch (kc) {

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

	                    case 'V':
	                        f_version = true ;
	                        break ;

	                    case 'a':
	                        pip->fl.all = true ;
	                        break ;

/* LASTLOG DB file */
	                    case 'f':
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl)
	                                llfname = argp ;
	                        } else
	                            rs = SR_INVALID ;
	                        break ;

	                    case 'h':
	                        lip->finval.hdr = true ;
	                        lip->fl.hdr = true ;
	                        if (f_optequal) {
	                            f_optequal = false ;
	                            if (avl) {
	                                rs = optbool(avp,avl) ;
	                                lip->fl.hdr = (rs > 0) ;
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
	                        lip->fl.reverse = true ;
	                        break ;

	                    case 's':
	                        lip->fl.sort = true ;
	                        break ;

/* verbose output */
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
	                        f_usage = true ;
	                        rs = SR_INVALID ;
	                        break ;

	                    } /* end switch */
	                    akp += 1 ;

	                    if (rs < 0) break ;
	                } /* end while */

	            } /* end if (individual option key letters) */

	        } /* end if */

	    } else {

	        rs = bits_set(&pargs,ai) ;
	        ai_max = ai ;

	    } /* end if (key letter/word or positional) */

	    ai_pos = ai ;

	} /* end while (all command line argument processing) */

	if (efname == nullptr) efname = getourenv(envv,VARERRORFNAME) ;
	if (efname == nullptr) efname = BFILE_STDERR ;
	if ((rs1 = shio_open(&errfile,efname,"wca",0666)) >= 0) {
	    pip->efp = &errfile ;
	    pip->open.errfile = true ;
	    shio_control(&errfile,SHIO_CSETBUFLINE,true) ;
	} else if (! isFailOpen(rs1)) {
	    if (rs >= 0) rs = rs1 ;
	}

	if (rs < 0) goto badarg ;

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    debugprintf("main: debuglevel=%u\n",pip->debuglevel) ;
#endif

	if (f_version) {
	    shio_printf(pip->efp,"%s: version %s\n",
	        pip->progname,VERSION) ;
	}

/* check arguments */

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
	}

	if (f_usage)
	    usage(pip) ;

/* user specified help only */

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

/* OK on the the mundane stuff */

	if (afname == nullptr) afname = getourenv(envv,VARAFNAME) ;

	if (pip->tmpdname == nullptr) pip->tmpdname = getourenv(envv,VARTMPDNAME) ;
	if (pip->tmpdname == nullptr) pip->tmpdname = TMPDNAME ;

	pip->daytime = time(nullptr) ;

	rs = procopts(pip,&akopts) ;

	if (pip->lfname == nullptr) pip->lfname = getourenv(envv,VARLFNAME) ;

	if (llfname == nullptr) llfname = getourenv(envv,VARDFNAME) ;
	if (llfname == nullptr) llfname = LASTLOGFNAME ;

	if (pip->debuglevel > 0) {
	    shio_printf(pip->efp,"%s: lastlog=%s\n",
	        pip->progname,llfname) ;
	}

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
	                if ((rs = proguserlist_begin(pip)) >= 0) {
	                    cchar	*lfn = llfname ;
	                    cchar	*ofn = ofname ;
	                    cchar	*afn = afname ;
	                    {
	                        rs = process(pip,&ainfo,&pargs,ofn,lfn,afn) ;
	                    }
	                    rs1 = proguserlist_end(pip) ;
	                    if (rs >= 0) rs = rs1 ;
	                } /* end if (proguserlist) */
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
		cchar	*fmt ;
	        ex = EX_NOUSER ;
		fmt = "%s: userinfo failure (%d)\n" ;
	        shio_printf(pip->efp,fmt,pn,rs) ;
	    } /* end if */
	} else if (ex == EX_OK) {
	    cchar	*pn = pip->progname ;
	    cchar	*fmt = "%s: invalid argument or configuration (%d)\n" ;
	    ex = EX_USAGE ;
	    shio_printf(pip->efp,fmt,pn,rs) ;
	    usage(pip) ;
	} /* end if (ok) */

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

	if (pip->efp != nullptr) {
	    pip->open.errfile = false ;
	    shio_close(pip->efp) ;
	    pip->efp = nullptr ;
	}

	if (pip->open.aparams) {
	    pip->open.aparams = false ;
	    paramopt_finish(&aparams) ;
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
	    debugprintf("b_lastlog: final mallout=%u\n",(mo-mo_start)) ;
	    uc_mallset(0) ;
	}
#endif

#if	(CF_DEBUGS || CF_DEBUG)
	debugclose() ;
#endif

	return ex ;

/* bad usage */
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
	cchar		*pn = pip->progname ;
	cchar		*fmt ;

	fmt = "%s: USAGE> %s [<username(s)>] " ;
	if (rs >= 0) rs = shio_printf(pip->efp,fmt,pn,pn) ;
	wlen += rs ;

	fmt = "[-af <afile>] [-a]\n" ;
	if (rs >= 0) rs = shio_printf(pip->efp,fmt) ;
	wlen += rs ;

	fmt = "%s:  [-llf <lastlog>]\n" ;
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
	                case akoname_linebuf:
	                    if (! lip->finval.linebuf) {
	                        lip->have.linebuf = true ;
	                        lip->finval.linebuf = true ;
	                        lip->fl.linebuf = true ;
	                        if (vl > 0) {
	                            rs = optbool(vp,vl) ;
	                            lip->fl.linebuf = (rs > 0) ;
	                        }
	                    }
	                    break ;
	                case akoname_all:
	                    if (! lip->finval.all) {
	                        lip->have.all = true ;
	                        lip->finval.all = true ;
	                        lip->fl.all = true ;
	                        if (vl > 0) {
	                            rs = optbool(vp,vl) ;
	                            lip->fl.all = (rs > 0) ;
	                        }
	                    }
	                    break ;
	                case akoname_realname:
	                    if (! lip->finval.realname) {
	                        lip->have.realname = true ;
	                        lip->finval.realname = true ;
	                        lip->fl.realname = true ;
	                        if (vl > 0) {
	                            rs = optbool(vp,vl) ;
	                            lip->fl.realname = (rs > 0) ;
	                        }
	                    }
	                    break ;
	                case akoname_pcsname:
	                case akoname_name:
	                    if (! lip->finval.name) {
	                        lip->have.name = true ;
	                        lip->finval.name = true ;
	                        lip->fl.name = true ;
	                        if (vl > 0) {
	                            rs = optbool(vp,vl) ;
	                            lip->fl.name = (rs > 0) ;
	                        }
	                    }
	                    break ;
	                case akoname_fullname:
	                    if (! lip->finval.fullname) {
	                        lip->have.fullname = true ;
	                        lip->finval.fullname = true ;
	                        lip->fl.fullname = true ;
	                        if (vl > 0) {
	                            rs = optbool(vp,vl) ;
	                            lip->fl.fullname = (rs > 0) ;
	                        }
	                    }
	                    break ;
	                case akoname_sysuser:
	                    if (! lip->finval.sysuser) {
	                        lip->have.sysuser = true ;
	                        lip->finval.sysuser = true ;
	                        lip->fl.sysuser = true ;
	                        if (vl > 0) {
	                            rs = optbool(vp,vl) ;
	                            lip->fl.sysuser = (rs > 0) ;
	                        }
	                    }
	                    break ;
	                case akoname_reguser:
	                    if (! lip->finval.reguser) {
	                        lip->have.reguser = true ;
	                        lip->finval.reguser = true ;
	                        lip->fl.reguser = true ;
	                        if (vl > 0) {
	                            rs = optbool(vp,vl) ;
	                            lip->fl.reguser = (rs > 0) ;
	                        }
	                    }
	                    break ;
	                case akoname_speuser:
	                    if (! lip->finval.speuser) {
	                        lip->have.speuser = true ;
	                        lip->finval.speuser = true ;
	                        lip->fl.speuser = true ;
	                        if (vl > 0) {
	                            rs = optbool(vp,vl) ;
	                            lip->fl.speuser = (rs > 0) ;
	                        }
	                    }
	                    break ;
	                case akoname_hdr:
	                    if (! lip->finval.hdr) {
	                        lip->have.hdr = true ;
	                        lip->finval.hdr = true ;
	                        lip->fl.hdr = true ;
	                        if (vl > 0) {
	                            rs = optbool(vp,vl) ;
	                            lip->fl.hdr = (rs > 0) ;
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
/* end subroutine (procopts) */


local int procuserinfo_begin(PROGINFO *pip,USERINFO *uip)
{
	int		rs = SR_OK ;

	pip->nodename = uip->nodename ;
	pip->domainname = uip->domainname ;
	pip->username = uip->username ;
	pip->org = uip->organization ;
	pip->gecosname = uip->gecosname ;
	pip->realname = uip->realname ;
	pip->name = uip->name ;
	pip->homedname = uip->homedname ;
	pip->fullname = uip->fullname ;
	pip->mailname = uip->mailname ;
	pip->logid = uip->logid ;
	pip->pid = uip->pid ;
	pip->uid = uip->uid ;
	pip->euid = uip->euid ;
	pip->gid = uip->gid ;
	pip->egid = uip->egid ;

	if (rs >= 0) {
	    cint	hlen = MAXHOSTNAMELEN ;
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
	    if (rs & KSHLIB_RMKSH) {
	        if ((rs = lib_serial()) >= 0) {
	            cint	s = rs ;
	            cint	plen = LOGIDLEN ;
	            cint	pv = pip->pid ;
	            cchar	*nn = pip->nodename ;
	            char	pbuf[LOGIDLEN+1] ;
	            if ((rs = mklogidpre(pbuf,plen,nn,pv)) >= 0) {
	                cint	slen = LOGIDLEN ;
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


local int process(PROGINFO *pip,ARGINFO *aip,bits *bop,
		cchar *ofn,cchar *llfn,cchar *afn)
{
	SHIO		ofile, *ofp = &ofile ;
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;

	if ((ofn == nullptr) || (ofn[0] == '\0') || (ofn[0] == '-'))
	    ofn = STDFNOUT ;

	if ((rs = shio_open(ofp,ofn,"wct",0666)) >= 0) {

	    rs = procer(pip,aip,bop,ofp,llfn,afn) ;
	    wlen += rs ;

	    rs1 = shio_close(ofp) ;
	    if (rs >= 0) rs = rs1 ;
	} else {
	    cchar	*pn = pip->progname ;
	    cchar	*fmt ;
	    fmt = "%s: inaccessible output (%d)\n" ;
	    shio_printf(pip->efp,fmt,pn,rs) ;
	} /* end if */

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (process) */


local int procer(PROGINFO *pip,ARGINFO *aip,bits *bop,void *ofp,
		cchar *llfn,cchar *afn)
{
	LOCINFO		*lip = pip->lip ;
	LASTLOGFILE	sll ;		/* system last-log */
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	cchar		*fmt ;

	fmt = "LOGNAME  DATE                    LINE                 HOST\n" ;
	if ((rs = lastlogfile_open(&sll,llfn,O_RDONLY)) >= 0) {

	    if (lip->fl.hdr) {
	        shio_printf(ofp,fmt) ;
	    }

	    if (pip->fl.all) {
	        rs = procerall(pip,ofp,&sll) ;
	        wlen += rs ;
	    } else {
	        rs = procerargs(pip,aip,bop,ofp,&sll,afn) ;
	        wlen += rs ;
	    } /* end if */

	    rs1 = lastlogfile_close(&sll) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (opened LASTLOG file) */

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (process) */


local int procerall(PROGINFO *pip,void *ofp,LASTLOGFILE *slp)
{
	LASTLOGFILE_CUR	cur ;
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;

	if ((rs = lastlogfile_curbegin(slp,&cur)) >= 0) {
	    PWFILE_ENT	pw ;
	    time_t	t ;
	    uid_t	uid ;
	    cint	pwlen = PWFILE_ENTLEN ;
	    cint	nrs = SR_NOTFOUND ;
	    cchar	*cp ;
	    char	pwbuf[PWFILE_ENTLEN + 1] ;
	    char	line[LASTLOGFILE_LLINE + 1] ;
	    char	hbuf[LASTLOGFILE_LHOST + 1] ;
	    while (rs >= 0) {
	        rs1 = lastlogfile_curenum(slp,&cur,&uid,&t,line,hbuf) ;
	        if (rs1 == nrs) break ;
	        rs = rs1 ;
	        if (rs >= 0) {
	            if ((rs = getpwentry_uid(&pw,pwbuf,pwlen,uid)) == nrs) {
	                uint	v = (uint) uid ;
	                cp = pwbuf ;
	                rs = snsd(pwbuf,PWFILE_ENTLEN,"U",v) ;
	            } else {
	                cp = pw.username ;
	            }
	            if (rs >= 0) {
	                rs = display(pip,ofp,cp,t,line,hbuf) ;
	                wlen += rs ;
	            }
	        } /* end if (ok) */
	    } /* end while */
	    lastlogfile_curend(slp,&cur) ;
	} /* end if (lastlogfile-cur) */

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (procerall) */


local int procerargs(pip,aip,bop,ofp,slp,afn)
PROGINFO	*pip ;
ARGINFO		*aip ;
bits		*bop ;
void		*ofp ;
LASTLOGFILE	*slp ;
cchar		*afn ;
{
	OSETSTR		ss ;
	cint	n = 20 ;
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;

	if ((rs = setostr_start(&ss,n)) >= 0) {
	    int		pan = 0 ;
	    int		cl ;
	    cchar	*cp ;

	    if (rs >= 0) {
	        int	ai ;
	        int	f ;
	        for (ai = 1 ; ai < aip->argc ; ai += 1) {
	            f = (ai <= aip->ai_max) && (bits_test(bop,ai) > 0) ;
	            f = f || ((ai > aip->ai_pos) && (aip->argv[ai] != nullptr)) ;

	            if (f) {
	                cp = aip->argv[ai] ;
	                if (cp[0] != '\0') {
	                    if (strcmp(cp,"-") == 0) cp = pip->username ;
	                    pan += 1 ;
	                    rs = procloadname(pip,&ss,cp,-1) ;
	                }
	            }

	            if (rs >= 0) rs = lib_sigterm() ;
	            if (rs >= 0) rs = lib_sigintr() ;
	            if (rs < 0) break ;
	        } /* end for (looping through requested circuits) */
	    } /* end if (ok) */

/* process any arguments in the argument list file */

	    if ((rs >= 0) && (afn != nullptr) && (afn[0] != '\0')) {
	        SHIO	afile, *afp = &afile ;

	        if (afn[0] == '-') afn = STDFNIN ;

	        if ((rs = shio_open(afp,afn,"r",0666)) >= 0) {
	            cint	llen = LINEBUFLEN ;
	            int		len ;
	            char	lbuf[LINEBUFLEN + 1] ;

	            while ((rs = shio_readline(afp,lbuf,llen)) > 0) {
	                len = rs ;

	                if (lbuf[len - 1] == '\n') len -= 1 ;
	                lbuf[len] = '\0' ;

	                if ((cl = sfskipwhite(lbuf,len,&cp)) > 0) {
	                    if (cp[0] != '#') {
	                        pan += 1 ;
	                        rs = procloadnames(pip,&ss,cp,cl) ;
	                    }
	                }

	                if (rs >= 0) rs = lib_sigterm() ;
	                if (rs >= 0) rs = lib_sigintr() ;
	                if (rs < 0) break ;
	            } /* end while (reading lines) */

	            rs1 = shio_close(afp) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (arg-file) */

	    } /* end if (processing argument-list) */

	    if ((rs >= 0) && (pan == 0)) {

	        cp = "-" ;
	        pan += 1 ;
	        rs = procloadname(pip,&ss,cp,-1) ;

	    } /* end if (default) */

	    if (rs >= 0) {
	        OSETSTR_CUR	cur ;
	        if ((rs = setostr_curbegin(&ss,&cur)) >= 0) {
	            while ((rs1 = setostr_curenum(&ss,&cur,&cp)) >= 0) {
	                if (strcmp(cp,"--") != 0) {
	                    rs = procerarg(pip,ofp,slp,cp) ;
	                    wlen += rs ;
	                }
	                if (rs < 0) break ;
	            } /* end while */
	            if ((rs >= 0) && (rs1 != SR_NOTFOUND)) rs = rs1 ;
	            rs1 = setostr_curend(&ss,&cur) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (setostr-cursor) */
	    } /* end if (ok) */

	    rs1 = setostr_finish(&ss) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (setostr) */

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (procerargs) */


local int procerarg(PROGINFO *pip,void *ofp,LASTLOGFILE *slp,cchar *arg)
{
	PWFILE_ENT	pw ;
	time_t		t ;
	uid_t		uid = -1 ;
	cint	pwlen = PWFILE_ENTLEN ;
	cint	nrs = SR_NOTFOUND ;
	int		rs ;
	int		wlen = 0 ;
	char		pwbuf[PWFILE_ENTLEN + 1] ;
	char		line[LASTLOGFILE_LLINE + 1] ;
	char		hbuf[LASTLOGFILE_LHOST + 1] ;

#if	CF_DEBUG
	if (DEBUGLEVEL(5))
	    debugprintf("main: ent name=%s\n",arg) ;
#endif

	if ((rs = getpwentry_name(&pw,pwbuf,pwlen,arg)) == nrs) {
	    if (hasalldig(arg,-1)) {
	        int	v ;
	        if ((rs = cfdeci(arg,-1,&v)) >= 0) {
	            uid = v ;
	            if ((rs = getpwentry_uid(&pw,pwbuf,pwlen,uid)) >= 0) {
	                arg = pw.username ;
	            } else if (isNotPresent(rs))
	                rs = SR_OK ;
	        } /* end if (cfdeci) */
	    } /* end if (hasalldig) */
	} else
	    uid = pw.uid ;

	if (rs >= 0) {
	    int	f_norec = false ;
	    if (uid >= 0) {
	        if ((rs = lastlogfile_readinfo(slp,uid,&t,line,hbuf)) > 0) {
	            rs = display(pip,ofp,arg,t,line,hbuf) ;
	            wlen += rs ;
	        } else {
	            f_norec = true ;
		}
	    } else {
	        f_norec = true ;
	    }
	    if ((rs >= 0) && f_norec) {
	        rs = shio_printf(ofp,"%-8s ** no record **\n",arg) ;
	        wlen += rs ;
	    }
	} /* end if (ok) */

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (procerarg) */


local int procloadnames(PROGINFO *pip,OSETSTR *nlp,cchar *sp,int sl)
{
	FIELD		fsb ;
	int		rs ;
	int		c = 0 ;

	if (nlp == nullptr) return SR_FAULT ;

	if ((rs = field_start(&fsb,sp,sl)) >= 0) {
	    int		fl ;
	    cchar	*fp ;

	    while ((fl = field_get(&fsb,aterms,&fp)) >= 0) {
	        if (fl > 0) {
	            rs = procloadname(pip,nlp,fp,fl) ;
	            c += rs ;
	        }
	        if (fsb.term == '#') break ;
	        if (rs < 0) break ;
	    } /* end while */

	    field_finish(&fsb) ;
	} /* end if (field) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (procloadnames) */


local int procloadname(PROGINFO *pip,OSETSTR *nlp,cchar np[],int nl)
{
	LOCINFO		*lip = pip->lip ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		c = 0 ;

	if (np == nullptr) return SR_FAULT ;

	if (np[0] == '\0') return SR_INVALID ;

#if	CF_DEBUG
	if (DEBUGLEVEL(5))
	    debugprintf("main/procloadname: ent rn=%r\n",np,nl) ;
#endif

	if (nl < 0) nl = strlen(np) ;

	if ((np[0] == '\0') || hasonlyme(np,nl)) {
	    if ((rs = locinfo_username(lip)) >= 0) {
	        np = lip->unbuf ;
	        nl = rs ;
	        rs = setostr_add(nlp,np,nl) ;
	        c += rs ;
	    } /* end if (locinfo_username) */
	} else {
	    cint	nch = MKCHAR(np[0]) ;
	    cchar	*tp ;
	    if ((tp = strnchr(np,nl,'+')) != nullptr) {
	        nl = (tp-np) ;
	    }
	    if (strnchr(np,nl,'.') != nullptr) {
	        LOCINFO_RNCUR	rnc ;
	        if ((rs = locinfo_rncurbegin(lip,&rnc)) >= 0) {
	            if ((rs = locinfo_rnlook(lip,&rnc,np,nl)) > 0) {
	                cint	ul = USERNAMELEN ;
	                char		ub[USERNAMELEN+1] ;
	                while ((rs = locinfo_rnread(lip,&rnc,ub,ul)) > 0) {
	                    rs = setostr_add(nlp,ub,rs) ;
	                    c += rs ;
	                    if (rs < 0) break ;
	                } /* end while (reading entries) */
	            } /* end if (locinfo_rnlook) */
	            rs1 = locinfo_rncurend(lip,&rnc) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (srncursor) */
	    } else if (nch == MKCHAR('¡')) {
	        LOCINFO_GMCUR	gc ;
	        cchar		*gnp = (np+1) ;
	        int		gnl = (nl-1) ;
		if (gnl == 0) {
		    rs = locinfo_groupname(lip) ;
		    gnl = rs ;
		    gnp = lip->gnbuf ;
		}
		if (rs >= 0) {
	            if ((rs = locinfo_gmcurbegin(lip,&gc)) >= 0) {
	                if ((rs = locinfo_gmlook(lip,&gc,gnp,gnl)) > 0) {
	                    cint	ul = USERNAMELEN ;
	                    char	ub[USERNAMELEN+1] ;
	                    while ((rs = locinfo_gmread(lip,&gc,ub,ul)) > 0) {
	                        rs = setostr_add(nlp,ub,rs) ;
	                        c += rs ;
	                        if (rs < 0) break ;
	                    } /* end while */
	                } /* end if */
	                rs1 = locinfo_gmcurend(lip,&gc) ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (gmcursor) */
		} /* end if (ok) */
	    } else {
	        if (nch == '!') {
	            np += 1 ;
	            nl -= 1 ;
	        }
		if (nl == 0) {
	    	    if ((rs = locinfo_username(lip)) >= 0) {
	        	np = lip->unbuf ;
			nl = rs ;
		    }
		}
	        if ((rs >= 0) && (nl > 0)) {
	            rs = setostr_add(nlp,np,nl) ;
	            c += rs ;
		}
	    } /* end if */
	} /* end if */

#if	CF_DEBUG
	if (DEBUGLEVEL(5))
	    debugprintf("main/procloadname: ret rs=%d c=%u\n",rs,c) ;
#endif

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (procloadname) */


local int display(pip,ofp,ubuf,ti_log,line,hbuf)
PROGINFO	*pip ;
void		*ofp ;
time_t		ti_log ;
cchar		ubuf[] ;
cchar		line[] ;
cchar		hbuf[] ;
{
	int		rs ;
	int		wlen = 0 ;
	cchar		*fmt = "%-8s %-23s %-8s %16s" ;
	char		timebuf[TIMEBUFLEN + 1] ;

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    debugprintf("display: ubuf=%s\n",ubuf) ;
#endif

	timestr_logz(ti_log,timebuf) ;
	rs = shio_printf(ofp,fmt,ubuf,timebuf,line,hbuf) ;
	wlen += rs ;

	if ((rs >= 0) && (pip->verboselevel > 0)) {
	    time_t	ti_diff = (time_t) (pip->daytime - ti_log) ;
	    timestr_elapsed(ti_diff,timebuf) ;
	    rs = shio_printf(ofp," (%s)",timebuf) ;
	    wlen += rs ;
	}

	if (rs >= 0) {
	    rs = shio_printf(ofp,"\n") ;
	    wlen += rs ;
	}

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    debugprintf("display: ret rs=%d wlen=%u\n",rs,wlen) ;
#endif

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (display) */

local int locinfo_start(LOCINFO *lip,PROGINFO *pip) noex {
	memclear(lip) ;
	lip->pip = pip ;
	pip->fl.logprog = true ;
	return SR_OK ;
} /* end subroutine (locinfo_start) */

local int locinfo_finish(LOCINFO *lip) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (lip == nullptr) return SR_FAULT ;

	lip->unbuf[0] = '\0' ;
	if (lip->open.rn) {
	    lip->open.rn = false ;
	    rs1 = sysrealname_close(&lip->rn) ;
	    if (rs >= 0) rs = rs1 ;
	}

	if (lip->open.gm) {
	    lip->open.gm = false ;
	    rs1 = grmems_finish(&lip->gm) ;
	    if (rs >= 0) rs = rs1 ;
	}

	if (lip->open.stores) {
	    lip->open.stores = false ;
	    rs1 = vecstr_finish(&lip->stores) ;
	    if (rs >= 0) rs = rs1 ;
	}

	return rs ;
}
/* end subroutine (locinfo_finish) */


#if	CF_LOCSETENT
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
#endif /* CF_LOCSETENT */


local int locinfo_username(LOCINFO *lip)
{
	int		rs ;
	if (lip == nullptr) return SR_FAULT ;
	if (lip->unbuf[0] == '\0') {
	    rs = getusername(lip->unbuf,USERNAMELEN,-1) ;
	} else {
	    rs = strlen(lip->unbuf) ;
	}
	return rs ;
}
/* end subroutine (locinfo_username) */

local int locinfo_groupname(LOCINFO *lip) noex {
	int		rs ;
	if (lip == nullptr) return SR_FAULT ;
	if (lip->gnbuf[0] == '\0') {
	    rs = getgroupname(lip->gnbuf,GROUPNAMELEN,-1) ;
	} else {
	    rs = strlen(lip->gnbuf) ;
	}
	return rs ;
} /* end subroutine (locinfo_groupname) */

int locinfo_gmcurbegin(LOCINFO *lip,LOCINFO_GMCUR *curp) noex {
	int		rs = SR_OK ;

	if (curp == nullptr) return SR_FAULT ;

	if (! lip->open.gm) {
	    cint	max = 20 ;
	    cint	ttl = (12*3600) ;
	    rs = grmems_start(&lip->gm,max,ttl) ;
	    lip->open.gm = (rs >= 0) ;
	}

	if (rs >= 0)
	    rs = grmems_curbegin(&lip->gm,&curp->gmcur) ;

	return rs ;
} /* end subroutine (locinfo_gmcurbegin) */

int locinfo_gmcurend(LOCINFO *lip,LOCINFO_GMCUR *curp) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (curp == nullptr) return SR_FAULT ;

	rs1 = grmems_curend(&lip->gm,&curp->gmcur) ;
	if (rs >= 0) rs = rs1 ;

	return rs ;
} /* end subroutine (locinfo_gmcurend) */

int locinfo_gmlook(LOCINFO *lip,LOCINFO_GMCUR *curp,cchar *gnp,int gnl) noex {
	cint	rsn = SR_NOTFOUND ;
	int		rs ;

	if (curp == nullptr) return SR_FAULT ;
	if (gnp == nullptr) return SR_FAULT ;

	if ((rs = grmems_lookup(&lip->gm,&curp->gmcur,gnp,gnl)) >= 0) {
	    rs = 1 ;
	} else if (rs == rsn) {
	    rs = SR_OK ;
	}

	return rs ;
} /* end subroutine (locinfo_gmlook) */

int locinfo_gmread(LOCINFO *lip,LOCINFO_GMCUR *curp,char *ubuf,int ulen) noex {
	cint	rsn = SR_NOTFOUND ;
	int		rs ;

	if (curp == nullptr) return SR_FAULT ;
	if (ubuf == nullptr) return SR_FAULT ;

	if ((rs = grmems_lookread(&lip->gm,&curp->gmcur,ubuf,ulen)) == rsn) {
	    rs = SR_OK ;
	}

	return rs ;
} /* end subroutine (locinfo_gmread) */

int locinfo_rncurbegin(LOCINFO *lip,LOCINFO_RNCUR *curp) noex {
	PROGINFO	*pip = lip->pip ;
	int		rs = SR_OK ;

	if (curp == nullptr) return SR_FAULT ;

#if	CF_DEBUG
	if (DEBUGLEVEL(5))
	    debugprintf("main/locinfo_rncurbegin: ent\n") ;
#endif
	if (! lip->open.rn) {
	    rs = sysrealname_open(&lip->rn,nullptr) ;
	    lip->open.rn = (rs >= 0) ;
	}

	if (rs >= 0)
	    rs = sysrealname_curbegin(&lip->rn,&curp->rncur) ;

#if	CF_DEBUG
	if (DEBUGLEVEL(5))
	    debugprintf("main/locinfo_rncurbegin: ret rs=%d\n",rs) ;
#endif
	return rs ;
} /* end subroutine (locinfo_rncurbegin) */

int locinfo_rncurend(LOCINFO *lip,LOCINFO_RNCUR *curp) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (curp == nullptr) return SR_FAULT ;

	rs1 = sysrealname_curend(&lip->rn,&curp->rncur) ;
	if (rs >= 0) rs = rs1 ;

	return rs ;
} /* end subroutine (locinfo_rncurend) */

int locinfo_rnlook(LOCINFO *lip,LOCINFO_RNCUR *curp,cchar *gnp,int gnl) noex {
	PROGINFO	*pip = lip->pip ;
	cint	rsn = SR_NOTFOUND ;
	cint	fo = 0 ;
	int		rs ;

	if (curp == nullptr) return SR_FAULT ;
	if (gnp == nullptr) return SR_FAULT ;

	if ((rs = sysrealname_look(&lip->rn,&curp->rncur,fo,gnp,gnl)) >= 0) {
	    rs = 1 ;
	} else if (rs == rsn) {
	    rs = SR_OK ;
	}

#if	CF_DEBUG
	if (DEBUGLEVEL(5))
	    debugprintf("main/locinfo_rnlook: sysrealname_look() rs=%d\n",
	        rs) ;
#endif

	return rs ;
} /* end subroutine (locinfo_rnlook) */

int locinfo_rnread(LOCINFO *lip,LOCINFO_RNCUR *curp,char ubuf,int ulen) noex {
	PROGINFO	*pip = lip->pip ;
	cint	rsn = SR_NOTFOUND ;
	int		rs ;

	if (curp == nullptr) return SR_FAULT ;
	if (ubuf == nullptr) return SR_FAULT ;
	if (pip == nullptr) return SR_FAULT ;

	if ((ulen >= 0) && (ulen < USERNAMELEN)) return SR_OVERFLOW ;

	if ((rs = sysrealname_lookread(&lip->rn,&curp->rncur,ubuf)) == rsn) {
	    rs = SR_OK ;
	}

#if	CF_DEBUG
	if (DEBUGLEVEL(5))
	    debugprintf("main/locinfo_rnread: sysrealname_lookread() rs=%d\n",
	        rs) ;
#endif

	return rs ;
} /* end subroutine (locinfo_rnread) */



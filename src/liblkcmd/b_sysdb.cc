/* b_sysdb SUPPORT */
/* lang=C++20 */

/* SHELL built-in to query system databases */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* non-switchable debug print-outs */
#define	CF_DEBUG	0		/* switchable at invocation */
#define	CF_DEBUGMALL	1		/* debug memory allocation */

/* revision history:

	= 2004-03-01, David A�D� Morano
	This code was originally written.  

*/

/* Copyright � 2004 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	Synopsis:
	$ sysdb <db> [<queries>]

*******************************************************************************/

#include	<envstandards.h>	/* must be first to configure */

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
#include	<fcntl.h>
#include	<stdlib.h>
#include	<string.h>
#include	<netdb.h>

#include	<usystem.h>
#include	<exitcodes.h>
#include	<getbufsize.h>
#include	<bits.h>
#include	<keyopt.h>
#include	<vecstr.h>
#include	<setstr.h>
#include	<field.h>
#include	<nulstr.h>
#include	<getax.h>
#include	<getpe.h>
#include	<getne.h>
#include	<gethe.h>
#include	<getse.h>
#include	<getua.h>
#include	<getus.h>
#include	<isnot.h>
#include	<exitcodes.h>
#include	<localmisc.h>

#include	"shio.h"
#include	"kshlib.h"
#include	"b_sysdb.h"
#include	"defs.h"


/* local defines */

#define	PI		proginfo
#define	LI		locinfo
#define	LI_FL		locinfo_flags

#define	LOCINFO		struct locinfo
#define	LOCINFO_FL	struct locinfo_flags

#define	SYSMODE		0664


/* external subroutines */


/* external variables */

extern char	**environ ;		/* definition required by AT&T AST */


/* local structures */

struct locinfo_flags {
	uint		stores:1 ;
	uint		wargs:1 ;
	uint		dbfname:1 ;
	uint		to:1 ;
	uint		all:1 ;
	uint		proto:1 ;
} ;

struct locinfo {
	PI		*pip ;
	cchar		*dbfname ;
	cchar		*fname ;
	cchar		*proto ;
	LOCINFO_FL	have, f, changed, final ;
	LOCINFO_FL	init, open ;
	vecstr		stores ;
	uint		ndb ;
	int		pagesize ;
	int		to ;
} ;

struct procusers {
	bfile		utfile ;
	bfile		ptfile ;
	int		unl ;
	char		uhbuf[MAXPATHLEN+1] ;
	char		utfname[MAXPATHLEN+1] ;
	char		ptfname[MAXPATHLEN+1] ;
} ;


/* forward references */

static int	mainsub(int,cchar **,cchar **,void *) ;

static int	usage(PI *) ;

static int	locinfo_start(LOCINFO *,PI *) ;
static int	locinfo_dbfname(LOCINFO *,cchar *) ;
static int	locinfo_to(LOCINFO *,int) ;
static int	locinfo_setentry(LOCINFO *,cchar **,cchar *,int) ;
static int	locinfo_defaults(LOCINFO *) ;
static int	locinfo_finish(LOCINFO *) ;

static int	procopts(PI *,KEYOPT *) ;
static int	process(PI *,ARGINFO *,BITS *,cchar *,cchar *,int) ;
static int	procall(PI *,int,void *) ;
static int	procsome(PI *,int,void *,ARGINFO *,BITS *,cchar *) ;
static int	procspecs(PI *,void *,int,cchar *,int) ;
static int	procspec(PI *,void *,int,cchar *,int) ;

static int process_begin(PI *,int) ;
static int process_end(PI *,int) ;

static int procshells_begin(PI *,int) ;
static int procshells_end(PI *,int) ;
static int procshells_all(PI *,int,void *) ;
static int procshells_query(PI *,int,void *,cchar *,int) ;

static int procprotos_begin(PI *,int) ;
static int procprotos_all(PI *,int,void *) ;
static int procprotos_query(PI *,int,void *,cchar *,int) ;
static int procprotos_end(PI *,int) ;

static int procnets_begin(PI *,int) ;
static int procnets_all(PI *,int,void *) ;
static int procnets_query(PI *,int,void *,cchar *,int) ;
static int procnets_end(PI *,int) ;

static int prochosts_begin(PI *,int) ;
static int prochosts_all(PI *,int,void *) ;
static int prochosts_query(PI *,int,void *,cchar *,int) ;
static int prochosts_end(PI *,int) ;

static int procsvcs_begin(PI *,int) ;
static int procsvcs_all(PI *,int,void *) ;
static int procsvcs_query(PI *,int,void *,cchar *,int) ;
static int procsvcs_end(PI *,int) ;

static int procuas_begin(PI *,int) ;
static int procuas_all(PI *,int,void *) ;
static int procuas_query(PI *,int,void *,cchar *,int) ;
static int procuas_end(PI *,int) ;


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
	argopt_db,
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
	"db",
	nullptr
} ;

constexpr  PIVARS	initvars = {
	VARPROGRAMROOT1,
	VARPROGRAMROOT2,
	VARPROGRAMROOT3,
	PROGRAMROOT,
	VARPRNAME
} ;

constexpr MAPEX	mapexs[] = {
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
	akoname_utf,
	akoname_db,
	akoname_overlast
} ;

constexpr cpcchar	akonames[] = {
	"utf",
	"db",
	nullptr
} ;

enum dbnames {
	dbname_shells,
	dbname_protocols,
	dbname_networks,
	dbname_hosts,
	dbname_services,
	dbname_userattrs,
	dbname_overlast
} ;

constexpr cpcchar	dbnames[] = {
	"shells",
	"protocols",
	"networks",
	"hosts",
	"services",
	"userattrs",
	nullptr
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


/* exported variables */


/* exported subroutines */

int b_sysdb(int argc,cchar *argv[],void *contextp) noex {
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
/* end subroutine (b_sysdb) */

int p_sysdb(int argc,mainv argv,mainv envv,void *contextp) noex {
	return mainsub(argc,argv,envv,contextp) ;
}
/* end subroutine (p_sysdb) */


/* local subroutines */

static int mainsub(int argc,mainv argv,mainv envv,void *contextp) noex {
	PI		pi, *pip = &pi ;
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
	int		to = -1 ;
	int		v ;
	int		w = -1 ;
	int		ex = EX_INFO ;
	int		f_optminus, f_optplus, f_optequal ;
	int		f_version = FALSE ;
	int		f_usage = FALSE ;
	int		f_help = FALSE ;
	int		f ;

	cchar		*argp, *aop, *akp, *avp ;
	cchar		*argval = nullptr ;
	cchar		*pr = nullptr ;
	cchar		*sn = nullptr ;
	cchar		*efname = nullptr ;
	cchar		*afname = nullptr ;
	cchar		*ofname = nullptr ;
	cchar		*dbfname = nullptr ;
	cchar		*cp ;


#if	CF_DEBUGS || CF_DEBUG
	if ((cp = getourenv(envv,VARDEBUGFNAME)) != nullptr) {
	    rs = debugopen(cp) ;
	    debugprintf("b_sysdb: starting DFD=%d\n",rs) ;
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
	pip->daytime = time(nullptr) ;

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
	            f_optequal = FALSE ;
	            if ((avp = strchr(aop,'=')) != nullptr) {
	                f_optequal = TRUE ;
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

	                case argopt_if:
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
			    break ;

/* DB file */
	                case argopt_db:
	                    if (f_optequal) {
	                        f_optequal = FALSE ;
	                        if (avl)
	                            dbfname = avp ;
	                    } else {
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl)
	                                dbfname = argp ;
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

	                    case 'a':
	                        lip->f.all = TRUE ;
	                        if (f_optequal) {
	                            f_optequal = FALSE ;
	                            if (avl) {
	                                rs = optbool(avp,avl) ;
	                                lip->f.all = (rs > 0) ;
	                            }
	                        }
	                        break ;

/* file-name (for FS operations) */
	                    case 'f':
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl)
	                                lip->fname = argp ;
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
	                        break ;

/* time-out */
	                    case 't':
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl) {
	                                rs = cfdecti(argp,argl,&v) ;
	                                to = v ;
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

	if (efname == nullptr) efname = getourenv(envv,VAREFNAME) ;
	if (efname == nullptr) efname = STDFNERR ;
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
	    debugprintf("b_sysdb: debuglevel=%u\n",pip->debuglevel) ;
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
	    rs = printhelp(sfstdout,pip->pr,pip->searchname,HELPFNAME) ;
#else
	    rs = printhelp(nullptr,pip->pr,pip->searchname,HELPFNAME) ;
#endif
	} /* end if */

	if (f_version || f_help || f_usage)
	    goto retearly ;


	ex = EX_OK ;

/* initialization */

	if ((rs >= 0) && (pip->n == 0) && (argval != nullptr)) {
	    rs = optvalue(argval,-1) ;
	    pip->n = rs ;
	}

	if (afname == nullptr) afname = getourenv(envv,VARAFNAME) ;

	if (rs >= 0) {
	    if ((rs = locinfo_dbfname(lip,dbfname)) >= 0) {
	        if ((rs = locinfo_to(lip,to)) >= 0) {
	            if ((rs = procopts(pip,&akopts)) >= 0) {
	    	        rs = locinfo_defaults(lip) ;
	 	    }
	        }
	    }
	}

	if (rs >= 0) {
	    for (ai = ai_continue ; ai < argc ; ai += 1) {
	        f = (ai <= ai_max) && (bits_test(&pargs,ai) > 0) ;
	        f = f || ((ai > ai_pos) && (argv[ai] != nullptr)) ;
	        if (f) {
	            cp = argv[ai] ;
	            ai_continue = (ai+1) ;
		    if (cp[0] != '\0') {
	                w = matostr(dbnames,2,cp,-1) ;
		    } else {
		        w = 0 ;
		    }
	            break ;
	        }
	    } /* end for */
	} /* end if (ok) */

	if (rs < 0) {
	    ex = EX_USAGE ;
	    goto retearly ;
	}

/* process */

#if	CF_DEBUG
	if (DEBUGLEVEL(2)) {
	    debugprintf("b_sysdb: f_all=%u\n",lip->f.all) ;
	    debugprintf("b_sysdb: w=%d\n",w) ;
	}
#endif

	if (pip->debuglevel > 0) {
	    cchar	*pn = pip->progname ;
	    if (w >= 0) {
	        shio_printf(pip->efp,"%s: db=%s (%u)\n",pn,dbnames[w],w) ;
	    } else {
	        shio_printf(pip->efp,"%s: db=*none*\n",pn) ;
	    }
	}

/* goto */

	memset(&ainfo,0,sizeof(ARGINFO)) ;
	ainfo.argc = argc ;
	ainfo.ai = ai ;
	ainfo.argv = argv ;
	ainfo.ai_max = ai_max ;
	ainfo.ai_pos = ai_pos ;
	ainfo.ai_continue = ai_continue ;

	if (rs >= 0) {
	    if (w >= 0) {
	        rs = process(pip,&ainfo,&pargs,ofname,afname,w) ;
	    } else {
		ex = EX_USAGE ;
		rs = SR_INVALID ;
	    }
	} else if (ex == EX_OK) {
	    cchar	*pn = pip->progname ;
	    cchar	*fmt = "%s: invalid argument or configuration (%d)\n" ;
	    shio_printf(pip->efp,fmt,pn,rs) ;
	    ex = EX_USAGE ;
	    usage(pip) ;
	} /* end if (ok) */

/* done */
	if ((rs < 0) && (ex == EX_OK)) {
	    switch (rs) {
	    case SR_INVALID:
	        ex = EX_USAGE ;
	        if (! pip->f.quiet) {
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

	if (pip->efp != nullptr) {
	    pip->open.errfile = FALSE ;
	    shio_close(pip->efp) ;
	    pip->efp = nullptr ;
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
	    debugprintf("b_sysdb: final mallout=%u\n",mdiff) ;
	    if (mdiff > 0) {
	        UCMALLREG_CUR	cur ;
	        UCMALLREG_REG	reg ;
	        const int	size = (10*sizeof(uint)) ;
	        cchar		*ids = "main" ;
	        uc_mallinfo(mi,size) ;
	        debugprintf("b_sysdb: MIoutnum=%u\n",mi[ucmallreg_outnum]) ;
	        debugprintf("b_sysdb: MIoutnummax=%u\n",
	            mi[ucmallreg_outnummax]) ;
	        debugprintf("b_sysdb: MIoutsize=%u\n",mi[ucmallreg_outsize]) ;
	        debugprintf("b_sysdb: MIoutsizemax=%u\n",
	            mi[ucmallreg_outsizemax]) ;
	        debugprintf("b_sysdb: MIused=%u\n",mi[ucmallreg_used]) ;
	        debugprintf("b_sysdb: MIusedmax=%u\n",mi[ucmallreg_usedmax]) ;
	        debugprintf("b_sysdb: MIunder=%u\n",mi[ucmallreg_under]) ;
	        debugprintf("b_sysdb: MIover=%u\n",mi[ucmallreg_over]) ;
	        debugprintf("b_sysdb: MInotalloc=%u\n",mi[ucmallreg_notalloc]) ;
	        debugprintf("b_sysdb: MInotfree=%u\n",mi[ucmallreg_notfree]) ;
	        ucmallreg_curbegin(&cur) ;
	        while (ucmallreg_curenum(&cur,&reg) >= 0) {
	            debugprintf("b_sysdb: MIreg.addr=%p\n",reg.addr) ;
	            debugprintf("b_sysdb: MIreg.size=%u\n",reg.size) ;
	            debugprinthexblock(ids,80,reg.addr,reg.size) ;
	        }
	        ucmallreg_curend(&cur) ;
	    }
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


static int usage(PI *pip)
{
	int		rs = SR_OK ;
	int		wlen = 0 ;
	cchar		*pn = pip->progname ;
	cchar		*fmt ;

	fmt = "%s: USAGE> %s <db> {<queries> [-r] |-a}\n" ;
	if (rs >= 0) rs = shio_printf(pip->efp,fmt,pn,pn) ;
	wlen += rs ;

	fmt = "%s:  [-Q] [-D] [-v[=<n>]] [-HELP] [-V]\n" ;
	if (rs >= 0) rs = shio_printf(pip->efp,fmt,pn) ;
	wlen += rs ;

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (usage) */


/* process the program ako-options */
static int procopts(PI *pip,KEYOPT *kop)
{
	LOCINFO		*lip = pip->lip ;
	int		rs = SR_OK ;
	int		c = 0 ;
	cchar		*cp ;

	if ((cp = getourenv(pip->envv,VAROPTS)) != nullptr) {
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

	                vl = keyopt_fetch(kop,kp,nullptr,&vp) ;

	                switch (oi) {
	                case akoname_utf:
	                case akoname_db:
	                    if (! lip->final.dbfname) {
	                        lip->have.dbfname = TRUE ;
	                        lip->final.dbfname = TRUE ;
	                        if (vl > 0) {
	                            cchar	**vpp = &lip->dbfname ;
	                            rs = locinfo_setentry(lip,vpp,vp,vl) ;
	                        }
	                    }
	                    break ;
	                } /* end switch */

	                c += 1 ;
	            } else {
	                rs = SR_INVALID ;
		    }

	            if (rs < 0) break ;
	        } /* end while (looping through key options) */

	        keyopt_curend(kop,&kcur) ;
	    } /* end if (keyope-cur) */
	} /* end if (ok) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (procopts) */


static int process(PI *pip,ARGINFO *aip,BITS *bop,cchar *ofn,cchar *afn,
		int w)
{
	SHIO		ofile, *ofp = &ofile ;
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;

	if ((ofn == nullptr) || (ofn[0] == '\0') || (ofn[0] == '-'))
	    ofn = STDFNOUT ;

	if ((rs = shio_open(ofp,ofn,"wct",0666)) >= 0) {
	    LOCINFO	*lip = pip->lip ;
	    if ((rs = process_begin(pip,w)) >= 0) {
	        if (lip->f.all) {
	            rs = procall(pip,w,ofp) ;
	            wlen += rs ;
	        } else {
	            rs = procsome(pip,w,ofp,aip,bop,afn) ;
	            wlen += rs ;
	        }
	        rs1 = process_end(pip,w) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (proc) */
	    rs1 = shio_close(ofp) ;
	    if (rs >= 0) rs = rs1 ;
	} else {
	    cchar	*pn = pip->progname ;
	    cchar	*fmt = "%s: inaccessible output (%d)\n" ;
	    shio_printf(pip->efp,fmt,pn,rs) ;
	    shio_printf(pip->efp,"%s: ofile=%s\n",pn,ofn) ;
	} /* end if (open-file) */
	return rs ;
}
/* end subroutine (process) */


static int process_begin(PI *pip,int w)
{
	int		rs ;

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	    debugprintf("b_sysdb/process_begin: w=%u\n",w) ;
#endif

	switch (w) {
	case dbname_shells:
	    rs = procshells_begin(pip,w) ;
	    break ;
	case dbname_protocols:
	    rs = procprotos_begin(pip,w) ;
	    break ;
	case dbname_networks:
	    rs = procnets_begin(pip,w) ;
	    break ;
	case dbname_hosts:
	    rs = prochosts_begin(pip,w) ;
	    break ;
	case dbname_services:
	    rs = procsvcs_begin(pip,w) ;
	    break ;
	case dbname_userattrs:
	    rs = procuas_begin(pip,w) ;
	    break ;
	default:
	    rs = SR_INVALID ;
	    break ;
	} /* end switch */

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	    debugprintf("b_sysdb/process_begin: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (process_begin) */


static int process_end(PI *pip,int w)
{
	int		rs = SR_OK ;

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	    debugprintf("b_sysdb/process_end: w=%u\n",w) ;
#endif

	switch (w) {
	case dbname_shells:
	    rs = procshells_end(pip,w) ;
	    break ;
	case dbname_protocols:
	    rs = procprotos_end(pip,w) ;
	    break ;
	case dbname_networks:
	    rs = procnets_end(pip,w) ;
	    break ;
	case dbname_hosts:
	    rs = prochosts_end(pip,w) ;
	    break ;
	case dbname_services:
	    rs = procsvcs_end(pip,w) ;
	    break ;
	case dbname_userattrs:
	    rs = procuas_end(pip,w) ;
	    break ;
	default:
	    rs = SR_INVALID ;
	    break ;
	} /* end switch */

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	    debugprintf("b_sysdb/process_end: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subrotine (process_end) */


static int procall(PI *pip,int w,void *ofp)
{
	int		rs = SR_OK ;

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	    debugprintf("b_sysdb/procall: ent w=%u\n",w) ;
#endif

	switch (w) {
	case dbname_shells:
	    rs = procshells_all(pip,w,ofp) ;
	    break ;
	case dbname_protocols:
	    rs = procprotos_all(pip,w,ofp) ;
	    break ;
	case dbname_networks:
	    rs = procnets_all(pip,w,ofp) ;
	    break ;
	case dbname_hosts:
	    rs = prochosts_all(pip,w,ofp) ;
	    break ;
	case dbname_services:
	    rs = procsvcs_all(pip,w,ofp) ;
	    break ;
	case dbname_userattrs:
	    rs = procuas_all(pip,w,ofp) ;
	    break ;
	default:
	    rs = SR_INVALID ;
	    break ;
	} /* end switch */

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	    debugprintf("b_sysdb/procall: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (procall) */


static int procsome(PI *pip,int w,void *ofp,ARGINFO *aip,BITS *bop,
		cchar *afn)
{
	int		rs = SR_OK ;
	int		rs1 ;
	int		pan = 0 ;
	int		wlen = 0 ;
	int		cl ;
	cchar		*pn = pip->progname ;
	cchar		*fmt ;
	cchar		*cp ;

	if (rs >= 0) {
	    int		ai ;
	    int		f ;
	    for (ai = aip->ai_continue ; ai < aip->argc ; ai += 1) {

	        f = (ai <= aip->ai_max) && (bits_test(bop,ai) > 0) ;
	        f = f || ((ai > aip->ai_pos) && (aip->argv[ai] != nullptr)) ;
	        if (f) {
	            cp = aip->argv[ai] ;
	            if (cp[0] != '\0') {
	                pan += 1 ;
	                rs = procspec(pip,ofp,w,cp,-1) ;
	                wlen += rs ;
	            }
	        }

	        if (rs >= 0) rs = lib_sigterm() ;
	        if (rs >= 0) rs = lib_sigintr() ;
	        if (rs < 0) break ;
	    } /* end for */
	} /* end if */

	if ((rs >= 0) && (afn != nullptr) && (afn[0] != '\0')) {
	    SHIO	afile, *afp = &afile ;

	    if (strcmp(afn,"-") == 0)
	        afn = STDFNIN ;

	    if ((rs = shio_open(afp,afn,"r",0666)) >= 0) {
	        const int	llen = LINEBUFLEN ;
	        char		lbuf[LINEBUFLEN + 1] ;

	        while ((rs = shio_readline(afp,lbuf,llen)) > 0) {
	            int	len = rs ;

	            if (lbuf[len - 1] == '\n') len -= 1 ;
	            lbuf[len] = '\0' ;

	            if ((cl = sfskipwhite(lbuf,len,&cp)) > 0) {
	                if (cp[0] != '#') {
	                    pan += 1 ;
	                    rs = procspecs(pip,ofp,w,cp,cl) ;
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

	} /* end if (processing file argument file list) */

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (procsome) */

static int procspecs(PI *pip,void *ofp,int w,cchar *lbuf,int llen) noex {
	FIELD		fsb ;
	int		rs ;
	int		wlen = 0 ;
	if ((rs = field_start(&fsb,lbuf,llen)) >= 0) {
	    int		fl ;
	    cchar	*fp ;
	    while ((fl = field_get(&fsb,aterms,&fp)) >= 0) {
	        if (fl > 0) {
	            rs = procspec(pip,ofp,w,fp,fl) ;
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

static int procspec(PI *pip,void *ofp,int w,cchar *sp,int sl) noex {
	int		rs = SR_OK ;

	switch (w) {
	case dbname_shells:
	    rs = procshells_query(pip,w,ofp,sp,sl) ;
	    break ;
	case dbname_protocols:
	    rs = procprotos_query(pip,w,ofp,sp,sl) ;
	    break ;
	
	    rs = procnets_query(pip,w,ofp,sp,sl) ;
	    break ;
	case dbname_hosts:
	    rs = prochosts_query(pip,w,ofp,sp,sl) ;
	    break ;
	case dbname_services:
	    rs = procsvcs_query(pip,w,ofp,sp,sl) ;
	    break ;
	case dbname_userattrs:
	    rs = procuas_query(pip,w,ofp,sp,sl) ;
	    break ;
	default:
	    rs = SR_INVALID ;
	    break ;
	} /* end switch */

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	    debugprintf("b_sysdb/procspec: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (procspec) */

/* ARGSUSED */
static int procshells_begin(PI *pip,int w) noex {
#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	    debugprintf("b_sysdb/procshells_begin: ent\n") ;
#endif
	return getus_begin() ;
}

/* ARGSUSED */
static int procshells_end(PI *pip,int w) noex {
#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	    debugprintf("b_sysdb/procshells_end: ent\n") ;
#endif
	return getus_end() ;
}

/* ARGSUSED */
static int procshells_all(PI *pip,int w,void *ofp) noex {
	const int	uslen = MAXPATHLEN ;
	int		rs ;
	char		usbuf[MAXPATHLEN+1] ;
	while ((rs = getus_ent(usbuf,uslen)) > 0) {
	    rs = shio_print(ofp,usbuf,rs) ;
	    if (rs < 0) break ;
	} /* end while */
	return rs ;
}

/* ARGSUSED */
static int procshells_query(PI *pip,int w,void *ofp,cc *sp,int sl) noex {
	return SR_INVALID ;
}

/* ARGSUSED */
static int procprotos_begin(PI *pip,int w) noex {
#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	    debugprintf("b_sysdb/procprotos_begin: ent\n") ;
#endif
	return getpe_begin(FALSE) ;
}

/* ARGSUSED */
static int procprotos_end(PI *pip,int w) noex {
#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	    debugprintf("b_sysdb/procprotos_end: ent\n") ;
#endif
	return getpe_end() ;
}

/* ARGSUSED */
static int procprotos_all(PI *pip,int w,void *ofp) noex {
	struct protoent	pe ;
	const int	pelen = getbufsize(getbufsize_pe) ;
	int		rs ;
	int		rs1 ;
	char		*pebuf ;
#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	    debugprintf("b_sysdb/procprotos_all: ent pelen=%d\n",pelen) ;
#endif
	if ((rs = uc_malloc((pelen+1),&pebuf)) >= 0) {
	    SETSTR	ss ;
	    const int	n = 20 ;
	    if ((rs = setstr_start(&ss,n)) >= 0) {
		cchar	*np ;
	        while ((rs = getpe_ent(&pe,pebuf,pelen)) > 0) {
		    np = pe.p_name ;
		    if ((rs = setstr_add(&ss,np,-1)) > 0) {
	        	rs = shio_print(ofp,np,-1) ;
		    }
		    if (rs < 0) break ;
	        } /* end while */
		rs1 = setstr_finish(&ss) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (setstr) */
	    uc_free(pebuf) ;
	} /* end if (m-a) */
#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	    debugprintf("b_sysdb/procprotos_end: ret rs=%d\n",rs) ;
#endif
	return rs ;
}

/* ARGSUSED */
static int procprotos_query(PI *pip,int w,void *ofp,cc *sp,int sl) noex {
	nulstr		s ;
	int		rs ;
	int		rs1 ;
	cchar		*name ;
	if ((rs = nulstr_start(&s,sp,sl,&name)) >= 0) {
	    struct protoent	pe ;
	    const int		pelen = getbufsize(getbufsize_pe) ;
	    char		*pebuf ;
	    if ((rs = uc_malloc((pelen+1),&pebuf)) >= 0) {
	        if ((rs = getpe_name(&pe,pebuf,pelen,name)) > 0) {
	            rs = shio_print(ofp,pe.p_name,-1) ;
	        }
	        uc_free(pebuf) ;
	    } /* end if (m-a) */
	    rs1 = nulstr_finish(&s) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (nulstr) */
	return rs ;
}

/* ARGSUSED */
static int procnets_begin(PI *pip,int w) noex {
	return getne_begin(TRUE) ;
}

/* ARGSUSED */
static int procnets_end(PI *pip,int w) noex {
	return getne_end() ;
}

/* ARGSUSED */
static int procnets_all(PI *pip,int w,void *ofp) noex {
	struct netent	ne ;
	const int	nelen = getbufsize(getbufsize_ne) ;
	int		rs ;
	int		rs1 ;
	char		*nebuf ;
	if ((rs = uc_malloc((nelen+1),&nebuf)) >= 0) {
	    SETSTR	ss ;
	    const int	n = 20 ;
	    if ((rs = setstr_start(&ss,n)) >= 0) {
		cchar	*np ;
	        while ((rs = getne_ent(&ne,nebuf,nelen)) > 0) {
		    np = ne.n_name ;
		    if ((rs = setstr_add(&ss,np,-1)) > 0) {
	                rs = shio_print(ofp,np,-1) ;
		    }
	            if (rs < 0) break ;
	        } /* end while */
		rs1 = setstr_finish(&ss) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (setstr) */
	    uc_free(nebuf) ;
	} /* end if (m-a) */
	return rs ;
}

/* ARGSUSED */
static int procnets_query(PI *pip,int w,void *ofp,cc *sp,int sl) noex {
	nulstr		s ;
	int		rs ;
	int		rs1 ;
	cchar		*name ;
	if ((rs = nulstr_start(&s,sp,sl,&name)) >= 0) {
	    struct netent	ne ;
	    const int		nelen = getbufsize(getbufsize_ne) ;
	    char		*nebuf ;
	    if ((rs = uc_malloc((nelen+1),&nebuf)) >= 0) {
	        if ((rs = getne_name(&ne,nebuf,nelen,name)) > 0) {
	            rs = shio_print(ofp,ne.n_name,-1) ;
	        }
	        uc_free(nebuf) ;
	    } /* end if (m-a) */
	    rs1 = nulstr_finish(&s) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (nulstr) */
	return rs ;
}

/* ARGSUSED */
static int prochosts_begin(PI *pip,int w) noex {
	return gethe_begin(FALSE) ;
}

/* ARGSUSED */
static int prochosts_end(PI *pip,int w) noex {
	return gethe_end() ;
}

/* ARGSUSED */
static int prochosts_all(PI *pip,int w,void *ofp) noex {
	struct hostent	he ;
	const int	helen = getbufsize(getbufsize_he) ;
	int		rs ;
	int		rs1 ;
	char		*hebuf ;
	if ((rs = uc_malloc((helen+1),&hebuf)) >= 0) {
	    SETSTR	ss ;
	    const int	n = 20 ;
	    if ((rs = setstr_start(&ss,n)) >= 0) {
		cchar	*np ;
	        while ((rs = gethe_ent(&he,hebuf,helen)) > 0) {
		    np = he.h_name ;
		    if ((rs = setstr_add(&ss,np,-1)) > 0) {
	                rs = shio_print(ofp,np,-1) ;
		    }
	            if (rs < 0) break ;
	        } /* end while */
		rs1 = setstr_finish(&ss) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (setstr) */
	    uc_free(hebuf) ;
	} /* end if (m-a) */
	return rs ;
}

/* ARGSUSED */
static int prochosts_query(PI *pip,int w,void *ofp,cc *sp,int sl) noex {
	nulstr		s ;
	int		rs ;
	int		rs1 ;
	cchar		*name ;
	if ((rs = nulstr_start(&s,sp,sl,&name)) >= 0) {
	    struct hostent	he ;
	    const int		helen = getbufsize(getbufsize_he) ;
	    char		*hebuf ;
	    if ((rs = uc_malloc((helen+1),&hebuf)) >= 0) {
	        if ((rs = gethe_name(&he,hebuf,helen,name)) > 0) {
	            rs = shio_print(ofp,he.h_name,-1) ;
	        }
	        uc_free(hebuf) ;
	    } /* end if (m-a) */
	    rs1 = nulstr_finish(&s) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (nulstr) */
	return rs ;
}

/* ARGSUSED */
static int procsvcs_begin(PI *pip,int w) noex {
	return getse_begin(FALSE) ;
}

/* ARGSUSED */
static int procsvcs_end(PI *pip,int w) noex {
	return getse_end() ;
}

/* ARGSUSED */
static int procsvcs_all(PI *pip,int w,void *ofp) noex {
	struct servent	se ;
	const int	selen = getbufsize(getbufsize_se) ;
	int		rs ;
	int		rs1 ;
	char		*sebuf ;
	if ((rs = uc_malloc((selen+1),&sebuf)) >= 0) {
	    SETSTR	ss ;
	    const int	n = 20 ;
	    if ((rs = setstr_start(&ss,n)) >= 0) {
		cchar	*np ;
	        while ((rs = getse_ent(&se,sebuf,selen)) > 0) {
		    np = se.s_name ;
		    if ((rs = setstr_add(&ss,np,-1)) > 0) {
	                rs = shio_print(ofp,np,-1) ;
		    }
	            if (rs < 0) break ;
	        } /* end while */
		rs1 = setstr_finish(&ss) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (setstr) */
	    uc_free(sebuf) ;
	} /* end if (m-a) */
	return rs ;
}

/* ARGSUSED */
static int procsvcs_query(PI *pip,int w,void *ofp,cc *sp,int sl) noex {
	LOCINFO		*lip = pip->lip ;
	nulstr		s ;
	int		rs ;
	int		rs1 ;
	cchar		*name ;
	if ((rs = nulstr_start(&s,sp,sl,&name)) >= 0) {
	    struct servent	se ;
	    const int		selen = getbufsize(getbufsize_se) ;
	    cchar		*proto = lip->proto ;
	    char		*sebuf ;
	    if ((rs = uc_malloc((selen+1),&sebuf)) >= 0) {
	        if ((rs = getse_name(&se,sebuf,selen,proto,name)) > 0) {
	            rs = shio_print(ofp,se.s_name,-1) ;
	        }
	        uc_free(sebuf) ;
	    } /* end if (m-a) */
	    rs1 = nulstr_finish(&s) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (nulstr) */
	return rs ;
}

/* ARGSUSED */
static int procuas_begin(PI *pip,int w) noex {
	return getua_begin(FALSE) ;
}

/* ARGSUSED */
static int procuas_end(PI *pip,int w) noex {
	return getua_end() ;
}

/* ARGSUSED */
static int procuas_all(PI *pip,int w,void *ofp) noex {
	userattr_t	ua ;
	const int	ualen = getbufsize(getbufsize_ua) ;
	int		rs ;
	int		rs1 ;
	char		*uabuf ;
	if ((rs = uc_malloc((ualen+1),&uabuf)) >= 0) {
	    SETSTR	ss ;
	    const int	n = 20 ;
	    if ((rs = setstr_start(&ss,n)) >= 0) {
		int	nl ;
	        cchar	*fmt = "%s %d\n" ;
		cchar	*np ;
	        while ((rs = getua_ent(&ua,uabuf,ualen)) > 0) {
		    nl = ua.attr->length ;
		    np = ua.name ;
		    if ((rs = setstr_add(&ss,np,nl)) > 0) {
	        	rs = shio_printf(ofp,fmt,np,nl) ;
		     }
	            if (rs < 0) break ;
	        } /* end while */
		rs1 = setstr_finish(&ss) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (setstr) */
	    uc_free(uabuf) ;
	} /* end if (m-a) */
	return rs ;
}

/* ARGSUSED */
static int procuas_query(PI *pip,int w,void *ofp,cchar *sp,int sl) noex {
	nulstr		s ;
	int		rs ;
	int		rs1 ;
	cchar		*name ;
	if ((rs = nulstr_start(&s,sp,sl,&name)) >= 0) {
	    userattr_t	ua ;
	    const int	ualen = getbufsize(getbufsize_ua) ;
	    char	*uabuf ;
	    if ((rs = uc_malloc((ualen+1),&uabuf)) >= 0) {
	        if ((rs = getua_name(&ua,uabuf,ualen,name)) > 0) {
	            rs = shio_print(ofp,ua.name,-1) ;
	        }
	        uc_free(uabuf) ;
	    } /* end if (m-a) */
	    rs1 = nulstr_finish(&s) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (nulstr) */
	return rs ;
}
/* end subroutine (procus_query) */

static int locinfo_start(LOCINFO *lip,PI *pip) noex {
	int		rs = SR_OK ;

	if (lip == nullptr)
	    return SR_FAULT ;

	memset(lip,0,sizeof(LOCINFO)) ;
	lip->pip = pip ;
	lip->to = -1 ;
	lip->proto = DEFPROTO ;

	return rs ;
}
/* end subroutine (locinfo_start) */

static int locinfo_finish(LOCINFO *lip) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (lip == nullptr)
	    return SR_FAULT ;

	if (lip->open.stores) {
	    lip->open.stores = FALSE ;
	    rs1 = vecstr_finish(&lip->stores) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
}
/* end subroutine (locinfo_finish) */

static int locinfo_setentry(LOCINFO *lip,cchar **epp,cchar *vp,int vl) noex {
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
}
/* end subroutine (locinfo_setentry) */

static int locinfo_dbfname(LOCINFO *lip,cchar *dbfname) noex {
	if (lip == nullptr)
	    return SR_FAULT ;

	if (dbfname != nullptr) {
	    lip->have.dbfname = TRUE ;
	    lip->final.dbfname = TRUE ;
	    lip->dbfname = dbfname ;
	}

	return SR_OK ;
}
/* end subroutine (locinfo_dbfname) */

static int locinfo_to(LOCINFO *lip,int to) noex {
	if (to < 0) to = TO_CACHE ;

	lip->to = to ;
	return SR_OK ;
}
/* end subroutine (locinfo_to) */

static int locinfo_defaults(LOCINFO *lip) noex {
	PI		*pip = lip->pip ;
	int		rs = SR_OK ;

	if (lip == nullptr)
	    return SR_FAULT ;

	if ((lip->dbfname == nullptr) && (! lip->final.dbfname)) {
	    cchar	*cp = getourenv(pip->envv,VARDBFNAME) ;
	    if (cp != nullptr) {
	        cchar	**vpp = &lip->dbfname ;
	        rs = locinfo_setentry(lip,vpp,cp,-1) ;
	    }
	}

	return rs ;
}
/* end subroutine (locinfo_defaults) */



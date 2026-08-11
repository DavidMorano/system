/* b_bibleq SUPPORT (KSH builtin) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* query the bible database using words as the query keys */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* non-switchable debug print-outs */
#define	CF_DEBUG	0		/* switchable at invocation */
#define	CF_DEBUGMALL	1		/* debug memory allocation */
#define	CF_COOKIE	0		/* use cookie as separator */

/* revision history:

	= 2008-10-01, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	b_bibleq

	Description:
	This is a built-in command to the KSH shell.  This little
	program looks up a number in a database and returns the
	corresponding string.

	Synopsis:
	$ bibleq <word(s)>

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

#include	<sys/param.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<bits.h>		/* LIBUC */
#include	<keyopt.h>		/* LIBUC */
#include	<estrings.h>		/* LIBUC */
#include	<field.h>		/* LIBUC */
#include	<vecstr.h>		/* LIBUC */
#include	<wordfill.h>		/* LIBUC */
#include	<exitcodes.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */
#include	<libdebug.h>		/* LIBDEBUG |DEBUGPRINTF(3debug)| */

#include	<biblebook.h>		/* LIBDAM */
#include	<biblepara.h>		/* LIBDAM */
#include	<bibleq.h>		/* LIBDAM */

#include	<shio.h>
#include	<kshlib.h>

#include	"b_bibleq.h"
#include	"defs.h"


/* local defines */

#ifndef	LINEBUFLEN
#define	LINEBUFLEN	MAX((MAXPATHLEN + 2),2048)
#endif

#ifndef	BVBUFLEN
#define	BVBUFLEN	512		/* maximum bibleq length (?) */
#endif

#define	COLBUFLEN	(COLUMNS + 10)

#define	NBLANKS		20

#define	PI		proginfo

#define	LI		locinfo
#define	LI_FL		locinfo_fl


/* external subroutines */

extern int	printhelp(void *,cchar *,cchar *,cchar *) noex ;
extern int	proginfo_setpiv(PI *,cchar *,const pivars *) noex ;


/* external variables */

extern char	**environ ;		/* definition required by AT&T AST */


/* local structures */

struct locinfo_fl {
	uint		n:1 ;
	uint		audit:1 ;
	uint		linelen:1 ;
	uint		indent:1 ;
	uint		nverses:1 ;
	uint		ndb:1 ;
	uint		bookname:1 ;
	uint		interactive:1 ;
	uint		prefix:1 ;
	uint		separate:1 ;
	uint		para:1 ;
	uint		clump:1 ;
} ; /* end struct */

struct locinfo {
	bibleq		*dbp ;
	biblebook	ndb ;		/* bible-book-name DB */
	biblepara	pdb ;
	void		*ofp ;
	cchar		*ndbname ;	/* name-db */
	cchar		*vdbname ;	/* verse-db name */
	cchar		*pdbname ;	/* paragraph-db name */
	PI		*pip ;
	LI_FL		have, fl, changed, finval ;
	LI_FL		open ;
	int		linelen ;
	int		indent ;
	int		nverses ;
	int		count, max, precision ;
	int		cout ;
} ; /* end struct */


/* forward references */

local int	mainsub(int,con mainv,con mainv,void *) noex ;

local int	locinfo_start(LI *,PI *) noex ;
local int	locinfo_nlookup(LI *,int,char *,int) noex ;
local int	locinfo_finish(LI *) noex ;
local int	locinfo_deflinelen(LI *) noex ;
local int	locinfo_ispara(LI *,bibleq_q *) noex ;

local int	usage(PI *) noex ;

local int	procopts(PI *,keyopt *) noex ;
local int	procargs(PI *,ARGINFO *,bits *,cchar *,cchar *) noex ;
local int	procspecs(PI *,cchar *,int) noex ;
local int	procspec(PI *,vecstr *) noex ;
local int	procoutcite(PI *,bibleq_q *,int) noex ;
local int	procout(PI *,bibleq_q *,cchar *,int) noex ;
local int	procoutline(PI *,cchar *,int) noex ;


/* local variables */

enum argopts {
	argopt_root,
	argopt_version,
	argopt_help,
	argopt_sn,
	argopt_af,
	argopt_ef,
	argopt_of,
	argopt_ndb,
	argopt_pdb,
	argopt_vdb,
	argopt_bookname,
	argopt_overlast
} ; /* end enum (argopts) */

constexpr cpcchar	argopts[] = {
	"ROOT",
	"VERSION",
	"HELP",
	"sn",
	"af",
	"ef",
	"of",
	"ndb",
	"pdb",
	"vdb",
	"bookname",
	nullptr
} ; /* end array */

constexpr PIVARS	initvars = {
	VARPROGRAMROOT1,
	VARPROGRAMROOT2,
	VARPROGRAMROOT3,
	PROGRAMROOT,
	VARPRNAME
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
	akoname_audit,
	akoname_linelen,
	akoname_indent,
	akoname_bookname,
	akoname_interactive,
	akoname_prefix,
	akoname_separate,
	akoname_para,
	akoname_clump,
	akoname_overlast
} ; /* end enum */

constexpr cpcchar	akonames[] = {
	"audit",
	"linelen",
	"indent",
	"bookname",
	"interactive",
	"prefix",
	"separate",
	"para",
	"clump",
	nullptr
} ; /* end array */

constexpr char		aterms[] = {
	0x00, 0x0A, 0x00, 0x00,
	0x09, 0x10, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
} ; /* end array */

constexpr char		blanks[NBLANKS+1] = "                    " ;


/* exported variables */


/* exported subroutines */

int b_bibleq(int argc,con mainv argv,void *contextp) noex {
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
} /* end subroutine (b_bibleq) */

int p_bibleq(int argc,con mainv argv,con mainv envv,void *contextp) noex {
	return mainsub(argc,argv,envv,contextp) ;
} /* end subroutine (p_bibleq) */


/* local subroutines */

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
	cchar		*ndbname = nullptr ;
	cchar		*pdbname = nullptr ;
	cchar		*vdbname = nullptr ;
	cchar		*cp ;


#if	CF_DEBUGS || CF_DEBUG
	if ((cp = getourenv(envv,VARDEBUGFNAME)) != nullptr) {
	    rs = debugopen(cp) ;
	    debugprintf("bibleq: starting DFD=%d\n",rs) ;
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

/* local information */

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

/* keyword match or only key letters? */

	            if ((kwi = matostr(argopts,2,akp,akl)) >= 0) {

	                switch (kwi) {

/* version */
	                case argopt_version:
	                    f_version = true ;
	                    if (f_optequal)
	                        rs = SR_INVALID ;
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

/* argument file */
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

/* output file */
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

/* BibleName DB name */
	                case argopt_ndb:
	                    if (f_optequal) {
	                        f_optequal = false ;
	                        if (avl)
	                            ndbname = avp ;
	                    } else {
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl)
	                                ndbname = argp ;
	                        } else
	                            rs = SR_INVALID ;
	                    }
	                    break ;

/* paragraph-db name */
	                case argopt_pdb:
	                    if (f_optequal) {
	                        f_optequal = false ;
	                        if (avl)
	                            pdbname = avp ;
	                    } else {
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl)
	                                pdbname = argp ;
	                        } else
	                            rs = SR_INVALID ;
	                    }
	                    break ;

/* BibleVverse DB name */
	                case argopt_vdb:
	                    if (f_optequal) {
	                        f_optequal = false ;
	                        if (avl)
	                            vdbname = avp ;
	                    } else {
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl)
	                                vdbname = argp ;
	                        } else
	                            rs = SR_INVALID ;
	                    }
	                    break ;

	                case argopt_bookname:
	                    lip->have.bookname = true ;
	                    lip->finval.bookname = true ;
	                    lip->fl.bookname = true ;
	                    if (f_optequal) {
	                        f_optequal = false ;
	                        if (avl) {
	                            rs = optbool(avp,avl) ;
	                            lip->fl.bookname = (rs > 0) ;
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

	                    case 'n':
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl) {
	                                lip->have.nverses = true ;
	                                lip->finval.nverses = true ;
	                                rs = optvalue(argp,argl) ;
	                                lip->nverses = rs ;
	                            }
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
					keyopt	*kop = &akopts ;
	                                rs = keyopt_loads(kop,argp,argl) ;
				    }
	                        } else
	                            rs = SR_INVALID ;
	                        break ;

	                    case 'p':
	                        lip->fl.prefix = true ;
	                        break ;

	                    case 'q':
	                        pip->verboselevel = 0 ;
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

/* line-width (columns) */
	                    case 'w':
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl) {
	                                lip->have.linelen = true ;
	                                lip->finval.linelen = true ;
	                                rs = optvalue(argp,argl) ;
	                                lip->linelen = rs ;
	                            }
	                        } else
	                            rs = SR_INVALID ;
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

	if (rs < 0) goto badarg ;

#if	CF_DEBUGS
	debugprintf("bibleq: debuglevel=%u\n",pip->debuglevel) ;
#endif

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    debugprintf("bibleq: debuglevel=%u\n",pip->debuglevel) ;
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

/* argument processing */

	if ((rs >= 0) && (lip->nverses <= 0) && (argval != nullptr)) {
	    if ((rs = optvalue(argval,-1)) >= 0) {
	        lip->have.nverses = true ;
	        lip->finval.nverses = true ;
	        lip->nverses = rs ;
	    }
	}

/* load up the environment options */

	if (rs >= 0) {
	    rs = procopts(pip,&akopts) ;
	}

/* argument defaults */

	if (afname == nullptr) afname = getourenv(envv,VARAFNAME) ;

	if (ndbname == nullptr) ndbname = getourenv(envv,VARNDB) ;
	lip->ndbname = ndbname ;

/* paragraph-db name */

	if (pdbname == nullptr) pdbname = getourenv(envv,VARPDB) ;
	lip->pdbname = pdbname ;

/* verse-db name */

	if (vdbname == nullptr) vdbname = getourenv(envv,VARVDB) ;
	if (vdbname == nullptr) vdbname = VDBNAME ;

/* display some answers */

	if (pip->debuglevel > 0) {
	    cchar	*pn = pip->progname ;
	    cchar	*fmt ;
	    fmt = "%s: ndb=%s\n" ;
	    shio_printf(pip->efp,fmt,pn,
		((ndbname != nullptr) ? ndbname : "nullptr")) ;
	    fmt = "%s: pdb=%s\n" ;
	    shio_printf(pip->efp,fmt,pn,
	        ((pdbname != nullptr) ? pdbname : "nullptr")) ;
	    fmt = "%s: vdb=%s\n" ;
	    shio_printf(pip->efp,fmt,pn,
	        ((vdbname != nullptr) ? vdbname : "nullptr")) ;
	}

	if (rs >= 0) {
	    rs = locinfo_deflinelen(lip) ;
	}

	if ((lip->nverses < 1) && (! lip->have.nverses)) lip->nverses = 1 ;

	if (lip->nverses < 0) lip->nverses = 1 ;

/* process */

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    debugprintf("bibleq: bibleq_open() vdbname=%s\n",
	        vdbname) ;
#endif

	memset(&ainfo,0,sizeof(ARGINFO)) ;
	ainfo.argc = argc ;
	ainfo.ai = ai ;
	ainfo.argv = argv ;
	ainfo.ai_max = ai_max ;
	ainfo.ai_pos = ai_pos ;

	if (rs >= 0) {
	    bibleq	vdb ;
	    lip->dbp = &vdb ;
	    if ((rs = bibleq_open(&vdb,pip->pr,vdbname)) >= 0) {

#if	CF_DEBUG
	    if (DEBUGLEVEL(2))
	        debugprintf("bibleq: bibleq_open() rs=%d\n",rs) ;
#endif

	        if (lip->fl.audit) {
	            rs = bibleq_audit(&vdb) ;
	            if (pip->debuglevel > 0) {
			cchar	*pn = pip->progname ;
			cchar	*fmt = "%s: bibleq DB audit (%d)\n" ;
	                shio_printf(pip->efp,fmt,pn,rs) ;
		    }
	        }

	        if (rs >= 0) {
	            rs = procargs(pip,&ainfo,&pargs,ofname,afname) ;
	        }

	        rs1 = bibleq_close(&vdb) ;
	        if (rs >= 0) rs = rs1 ;
	    } else {
	        cchar	*pn = pip->progname ;
		cchar	*fmt ;
		fmt = "%s: could not load bibleq DB (%d)\n" ;
	        shio_printf(pip->efp,fmt,pn,rs) ;
	        shio_printf(pip->efp,"%s: vdb=%s\n",pn,vdbname) ;
	        ex = EX_CONFIG ;
	    }
	    lip->dbp = nullptr ;
	} else if (ex == EX_OK) {
	    cchar	*pn = pip->progname ;
	    cchar	*fmt = "%s: invalid argument or configuration (%d)\n" ;
	    ex = EX_USAGE ;
	    shio_printf(pip->efp,fmt,pn,rs) ;
	    usage(pip) ;
	}

/* finish up */
	if ((rs < 0) && (ex == EX_OK)) {
	    ex = mapex(mapexs,rs) ;
	    if (! pip->fl.quiet) {
		cchar	*pn = pip->progname ;
		cchar	*fmt = "%s: could not perform function (%d)\n" ;
	        shio_printf(pip->efp,fmt,pn,rs) ;
	    }
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
	    debugprintf("bibleq: exiting ex=%u (%d)\n",ex,rs) ;
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
	    debugprintf("bibleq: mallout=%u\n",mo-mo_start) ;
	    uc_mallset(0) ;
	}
#endif

#if	(CF_DEBUGS || CF_DEBUG)
	debugclose() ;
#endif

	return ex ;

/* bad stuff */
badarg:
	ex = EX_USAGE ;
	shio_printf(pip->efp,"%s: invalid argument specified (%d)\n",
	    pip->progname,rs) ;
	usage(pip) ;
	goto retearly ;

} /* end subroutine (mainsub) */

local int usage(PI *pip) noex {
	int		rs = SR_OK ;
	int		wlen = 0 ;
	cchar		*pn = pip->progname ;
	cchar		*fmt ;

	fmt = "%s: USAGE> %s [<word(s)> ...] [-w <width>]\n" ;
	if (rs >= 0) rs = shio_printf(pip->efp,fmt,pn,pn) ;
	wlen += rs ;

	fmt = "%s:  [-ndb <booknamedb>] [-vdb <versedb>]\n" ;
	if (rs >= 0) rs = shio_printf(pip->efp,fmt,pn) ;
	wlen += rs ;

	fmt = "%s:  [-Q] [-D] [-v[=<n>]] [-HELP] [-V]\n" ;
	if (rs >= 0) rs = shio_printf(pip->efp,fmt,pn) ;
	wlen += rs ;

	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (usage) */

/* process the program ako-names */
local int procopts(PI *pip,keyopt *kop) noex {
	LI		*lip = pip->lip ;
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

	                case akoname_audit:
	                    if (! lip->finval.audit) {
	                        lip->have.audit = true ;
	                        lip->finval.audit = true ;
	                        lip->fl.audit = true ;
	                        if (vl > 0) {
	                            rs = optbool(vp,vl) ;
	                            lip->fl.audit = (rs > 0) ;
	                        }
	                    }
	                    break ;

	                case akoname_linelen:
	                    if (! lip->finval.linelen) {
	                        lip->have.linelen = true ;
	                        lip->finval.linelen = true ;
	                        lip->fl.linelen = true ;
	                        if (vl > 0) {
	                            rs = optvalue(vp,vl) ;
	                            lip->linelen = rs ;
	                        }
	                    }
	                    break ;

	                case akoname_indent:
	                    if (! lip->finval.indent) {
	                        lip->have.indent = true ;
	                        lip->finval.indent = true ;
	                        lip->indent = 1 ;
	                        if (vl > 0) {
	                            rs = optvalue(vp,vl) ;
	                            lip->indent = rs ;
	                        }
	                    }
	                    break ;

	                case akoname_bookname:
	                    if (! lip->finval.bookname) {
	                        lip->have.bookname = true ;
	                        lip->finval.bookname = true ;
	                        lip->fl.bookname = true ;
	                        if (vl > 0) {
	                            rs = optbool(vp,vl) ;
	                            lip->fl.bookname = (rs > 0) ;
	                        }
	                    }
	                    break ;

	                case akoname_interactive:
	                    if (! lip->finval.interactive) {
	                        lip->have.interactive = true ;
	                        lip->finval.interactive = true ;
	                        lip->fl.interactive = true ;
	                        if (vl > 0) {
	                            rs = optbool(vp,vl) ;
	                            lip->fl.interactive = (rs > 0) ;
	                        }
	                    }
	                    break ;

	                case akoname_prefix:
	                    if (! lip->finval.prefix) {
	                        lip->have.prefix = true ;
	                        lip->finval.prefix = true ;
	                        lip->fl.prefix = true ;
	                        if (vl > 0) {
	                            rs = optbool(vp,vl) ;
	                            lip->fl.prefix = (rs > 0) ;
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

	                case akoname_para:
	                    if (! lip->finval.para) {
	                        lip->have.para = true ;
	                        lip->finval.para = true ;
	                        lip->fl.para = true ;
	                        if (vl > 0) {
	                            rs = optbool(vp,vl) ;
	                            lip->fl.para = (rs > 0) ;
	                        }
	                    }
	                    break ;

	                case akoname_clump:
	                    if (! lip->finval.clump) {
	                        lip->have.clump = true ;
	                        lip->finval.clump = true ;
	                        lip->fl.clump = true ;
	                        if (vl > 0) {
	                            rs = optbool(vp,vl) ;
	                            lip->fl.clump = (rs > 0) ;
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
} /* end subroutine (procopts) */

local int procargs(PI *pip,ARGINFO *aip,bits *bop,cchar *ofn,cchar *afn) noex {
	LI		*lip = pip->lip ;
	SHIO		ofile, *ofp = &ofile ;
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	cchar		*pn = pip->progname ;
	cchar		*fmt ;

	if ((ofn == nullptr) || (ofn[0] == '\0') || (ofn[0] == '-'))
	    ofn = STDFNOUT ;

	if ((rs = shio_open(ofp,ofn,"wct",0666)) >= 0) {
	    vecstr	qstr ;
	    cint	vo = vecstrm.compact ;
	    int		cl ;
	    int		pan = 0 ;
	    cchar	*cp ;
	    lip->ofp = ofp ;

	    if ((rs = vecstr_start(&qstr,5,opts)) >= 0) {

	        if (rs >= 0) {
	            cint	argc = aip->argc ;
	            int		ai ;
	            int		f ;
	            cchar	**argv = aip->argv ;
	            for (ai = 1 ; ai < argc ; ai += 1) {
	                f = (ai <= aip->ai_max) && (bits_test(bop,ai) > 0) ;
	                f = f || ((ai > aip->ai_pos) && (argv[ai] != nullptr)) ;
	                if (f) {
	                    cp = argv[ai] ;
			    if (cp[0] != '\0') {
	                        pan += 1 ;
	                        if (lip->fl.clump) {
	                            rs = vecstr_adduniqs(&qstr,cp,-1) ;
	                        } else {
	                            rs = vecstr_adduniq(&qstr,cp,-1) ;
				}
	                    }
	                } /* end if (non-zero) */
	                if (rs >= 0) rs = lib_sigterm() ;
	                if (rs >= 0) rs = lib_sigintr() ;
	                if (rs < 0) break ;
	            } /* end for (looping through positional arguments) */
	        } /* end if (ok) */

	        if ((rs >= 0) && (pan > 0)) {
	            if ((rs = vecstr_count(&qstr)) > 0) {
	                rs = procspec(pip,&qstr) ;
	                wlen += rs ;
	            }
	        }

	        rs1 = vecstr_finish(&qstr) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (qstr) */

	    if ((rs >= 0) && (afn != nullptr) && (afn[0] != '\0')) {
	        SHIO	afile, *afp = &afile ;

	        if (strcmp(afn,"-") == 0) afn = STDFNIN ;

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
	                        rs = procspecs(pip,cp,cl) ;
	                        wlen += rs ;
	                    }
	                }

	                if (rs >= 0) rs = lib_sigterm() ;
	                if (rs >= 0) rs = lib_sigintr() ;
	                if (rs < 0) break ;
	            } /* end while */

	            rs1 = shio_close(afp) ;
		    if (rs >= 0) rs = rs1 ;
	        } else {
		    fmt = "%s: inaccesible argument-list (%d)\n" ;
	            shio_printf(pip->efp,fmt,pn,rs) ;
	            shio_printf(pip->efp,"%s: afile=%s\n",pn,afn) ;
	        } /* end if */

	    } /* end if (argument-file arguments) */

	    lip->ofp = nullptr ;
	    rs1 = shio_close(ofp) ;
	    if (rs >= 0) rs = rs1 ;
	} else {
	    fmt = "%s: inaccessible output (%d)\n" ;
	    shio_printf(pip->efp,fmt,pn,rs) ;
	    shio_printf(pip->efp,"%s: ofile=%s\n",pn,ofn) ;
	}

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	    debugprintf("bibleq/procargs: ret rs=%d\n",rs) ;
#endif

	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (procargs) */

local int procspecs(PI *pip,cchar *sp,int sl) noex {
	LI		*lip = pip->lip ;
	vecstr		qstr ;
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;

	if (sp == nullptr) return SR_FAULT ;

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("bibleq/procspecs: ent sl=%d\n",sl) ;
#endif

	if (lip->fl.interactive) lip->cout = 0 ;

	if (sl < 0) sl = strlen(sp) ;

	if ((rs = vecstr_start(&qstr,5,0)) >= 0) {
	    FIELD	fsb ;
	    int		c = 0 ;

	    if ((rs = field_start(&fsb,sp,sl)) >= 0) {
	        cint	flen = sl ;
	        const uchar	*at = aterms ;
	        char		*fbuf ;
	        if ((rs = uc_malloc((flen+1),&fbuf)) >= 0) {
	            int		fl ;
	            while ((fl = field_wordphrase(&fsb,at,fbuf,flen)) >= 0) {
	                if (fl > 0) {
	                    rs = vecstr_adduniq(&qstr,fbuf,fl) ;
	                    c += ((rs < INT_MAX) ? 1 : 0) ;
	                }
	                if (fsb.term == '#') break ;
	                if (rs < 0) break ;
	            } /* end while */
	            uc_free(fbuf) ;
	        } /* end if (m-a) */
	        field_finish(&fsb) ;
	    } /* end if (field) */

	    if ((rs >= 0) && (c > 0)) {
	        rs = procspec(pip,&qstr) ;
	        wlen += rs ;
	    }

	    rs1 = vecstr_finish(&qstr) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (query-strings) */

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("bibleq/procspecs: ret rs=%d wlen=%u\n",rs,wlen) ;
#endif

	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (procspecs) */

local int procspec(PI *pip,vecstr *qsp) noex {
	LI		*lip = pip->lip ;
	bibleq		*bqp ;
	bibleq_cur	cur ;
	bibleq_q	q ;
	int		rs ;
	int		rs1 ;
	int		qopts = 0 ;
	int		wlen = 0 ;

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("bibleq/procspec: ent\n") ;
#endif

	if (qsp == nullptr) return SR_FAULT ;

	if (lip->fl.prefix) qopts |= BIBLEQ_OPREFIX ;

	bqp = lip->dbp ;
	if ((rs = bibleq_curbegin(bqp,&cur)) >= 0) {
	    cint	comlen = BVBUFLEN ;
	    int		cbl ;
	    cchar	**qkeya ;
	    char	combuf[BVBUFLEN + 1] ;

	    vecstr_getvec(qsp,&qkeya) ;

#if	CF_DEBUG
	    if (DEBUGLEVEL(3)) {
	        for (int i = 0 ; qkeya[i] != nullptr ; i += 1) {
	            debugprintf("bibleq/procspec: sk=>%s<\n",qkeya[i]) ;
		}
	    }
#endif /* CF_DEBUG */

	    if ((rs = bibleq_lookup(bqp,&cur,qopts,qkeya)) >= 0) {
	        cint	ntags = rs ;

	        while ((rs >= 0) && (ntags > 0)) {

	            cbl = bibleq_read(bqp,&cur,&q,combuf,comlen) ;
	            if (cbl == SR_NOTFOUND) break ;
	            rs = cbl ;
	            if (rs >= 0) {
	                rs = procoutcite(pip,&q,0) ;
	                wlen += rs ;
	            }

	            if (rs >= 0) {
	                rs = procout(pip,&q,combuf,cbl) ;
	                wlen += rs ;
	            }

	            if (rs >= 0) rs = lib_sigterm() ;
	            if (rs >= 0) rs = lib_sigintr() ;
	        } /* end while */

	    } /* end if (lookup) */

	    rs1 = bibleq_curend(bqp,&cur) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (cursor) */

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("bibleq/procspec: ret rs=%d wlen=%u\n",rs,wlen) ;
#endif

	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (procspec) */

local int procoutcite(PI *pip,bibleq_q *qp,int edays) noex {
	LI		*lip = pip->lip ;
	cint	clen = COLBUFLEN ;
	int		rs = SR_OK ;
	int		cl ;
	int		b = qp->b ;
	int		c = qp->c ;
	int		v = qp->v ;
	int		wlen = 0 ;
	int		f_havebook = false ;
	cchar		*fmt ;
	char		cbuf[COLBUFLEN + 1] ;

#if	CF_COOKIE
	fmt = "%%\n" ;
#else
	fmt = "\n" ;
#endif

/* print out any necessary separator */

	if (lip->fl.separate && (lip->cout++ > 0)) {
	    rs = shio_printf(lip->ofp,fmt) ;
	    wlen += rs ;
	} /* end if (separator) */

	if (rs >= 0) {
	    /* print out the text-data itself */
	    if (lip->fl.bookname) {
	        cint	blen = BIBLEBOOK_LEN ;
	        int	bbl ;
	        char	bbuf[BIBLEBOOK_LEN + 1] ;
	        if ((bbl = locinfo_nlookup(lip,qp->b,bbuf,blen)) > 0) {
	            f_havebook = true ;
	            fmt = (edays > 0) ? "%r %u:%u (%u)" : "%r %u:%u" ;
	            rs = bufprintf(cbuf,clen,fmt,bbuf,bbl,c,v,(edays+1)) ;
	            cl = rs ;
	            if (rs >= 0) {
	                rs = shio_print(lip->ofp,cbuf,cl) ;
	                wlen += rs ;
	            }
	        } /* end if (nlookup) */
	    } /* end if (book-name) */
	    if ((rs >= 0) && (! f_havebook)) {
	        fmt = (edays > 0) ? "%u:%u:%u (%u)" : "%u:%u:%u" ;
	        rs = bufprintf(cbuf,clen,fmt,b,c,v,(edays+1)) ;
	        cl = rs ;
	        if (rs >= 0) {
	            rs = shio_print(lip->ofp,cbuf,cl) ;
	            wlen += rs ;
	        }
	    } /* end if (type of book-name display) */
	} /* end if (ok) */
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (procoutcite) */

local int procout(PI *pip,bibleq_q *qp,cchar *vp,int vl) noex {
	LI		*lip = pip->lip ;
	wordfill	w ;
	cint		clen = COLBUFLEN ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		sl = vl ;
	int		cl ;
	int		cbl ;
	int		line = 0 ;
	int		wlen = 0 ;
	int		f_p = false ;
	cchar		*sp = vp ;
	char		cbuf[COLBUFLEN + 1] ;

	cbl = MIN((lip->linelen - lip->indent),clen) ;

	if ((rs >= 0) && lip->fl.para) {
	    rs = locinfo_ispara(lip,qp) ;
	    f_p = (rs > 0) ;
	}

/* print out the text-data itself */

	if (f_p) sp = nullptr ;

	if (rs >= 0) {
	    if ((rs = wordfill_start(&w,sp,sl)) >= 0) {

	        if (f_p) {
	            rs = wordfill_addword(&w,"¶",1) ;
	            if (rs >= 0)
	                rs = wordfill_addlines(&w,vp,vl) ;
	        }

	        while (rs >= 0) {
	            cl = wordfill_mklinefull(&w,cbuf,cbl) ;
	            if ((cl == 0) || (cl == SR_NOTFOUND)) break ;
	            rs = cl ;
	            if (rs >= 0) {
	                rs = procoutline(pip,cbuf,cl) ;
	                wlen += rs ;
	                line += 1 ;
	            }
	        } /* end while (full lines) */

	        if (rs >= 0) {
	            if ((cl = wordfill_mklinepart(&w,cbuf,cbl)) > 0) {
	                rs = procoutline(pip,cbuf,cl) ;
	                wlen += rs ;
	                line += 1 ;
	            } else if (cl != SR_NOTFOUND) {
	                rs = cl ;
		    }
	        } /* end if (partial lines) */

	        rs1 = wordfill_finish(&w) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (word-fill) */
	} /* end if (ok) */

	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (procout) */

local int procoutline(PI *pip,cchar *lp,int ll) noex {
	LI		*lip = pip->lip ;
	int		rs ;
	int		indent ;
	int		wlen = 0 ;
	{
	indent = MIN(lip->indent,NBLANKS) ;
	rs = shio_printf(lip->ofp,"%r%r\n",blanks,indent,lp,ll) ;
	wlen += rs ;
	}
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (procoutline) */

local int locinfo_start(LI *lip,PI *pip) noex {
	int		rs = SR_OK ;
	memclear(lip) ;
	lip->pip = pip ;
	lip->count = -1 ;
	lip->max = -1 ;
	lip->fl.separate = true ;
	lip->indent = OPT_INDENT ;
	lip->fl.bookname = OPT_BOOKNAME ;
	return rs ;
} /* end subroutine (locinfo_start) */

local int locinfo_finish(LI *lip) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (lip->open.para) {
	    lip->open.para = false ;
	    rs1 = biblepara_close(&lip->pdb) ;
	    if (rs >= 0) rs = rs1 ;
	}
	if (lip->have.ndb && lip->fl.ndb) {
	    lip->have.ndb = false ;
	    lip->fl.ndb = false ;
	    rs1 = biblebook_close(&lip->ndb) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
} /* end subroutine (locinfo_finish) */

local int locinfo_deflinelen(LI *lip) noex {
	cint	def = (DEFPRECISION + 2) ;
	int		rs = SR_OK ;
	if (lip->linelen < def) {
	    PI	*pip = lip->pip ;
	    cchar	*cp = nullptr ;
	    if (isStrEmpty(cp,-1)) {
		cp = getourenv(pip->envv,VARLINELEN) ;
	    }
	    if (isStrEmpty(cp,-1)) {
		cp = getourenv(pip->envv,VARCOLUMNS) ;
	    }
	    if (hasnonwhite(cp,-1)) {
	        if ((rs = optvalue(cp,-1)) >= 0) {
		    if (rs >= def) {
	                lip->have.linelen = true ;
	                lip->finval.linelen = true ;
	                lip->linelen = rs ;
		    }
	        }
	    }
	}
	if (lip->linelen < def) {
	    lip->linelen = COLUMNS ;
	}
	return rs ;
} /* end subroutine (locinfo_deflinelen) */

local int locinfo_nlookup(LI *lip,int bi,char *nbuf,int nlen) noex {
	PI	*pip = lip->pip ;
	int		rs = SR_OK ;
	int		len = 0 ;

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	    debugprintf("bibleq/locinfo_nlookup: bi=%u\n",bi) ;
#endif

	nbuf[0] = '\0' ;
	if (! lip->have.ndb) {
	    lip->have.ndb = true ;
	    rs = biblebook_open(&lip->ndb,pip->pr,lip->ndbname) ;
	    lip->fl.ndb = (rs >= 0) ;

#if	CF_DEBUG
	    if (DEBUGLEVEL(3))
	        debugprintf("bibleq/locinfo_nlookup: "
	            "biblebook_open() rs=%d\n", rs) ;
#endif

	} /* end if */

	if (rs >= 0) {
	    if (lip->fl.ndb) {
	        rs = biblebook_get(&lip->ndb,bi,nbuf,nlen) ;
	        len = rs ;
	    } else
	        rs = SR_NOTFOUND ;
	}

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	    debugprintf("bibleq/locinfo_nlookup: ret rs=%d len=%u\n",rs,len) ;
#endif

	return (rs >= 0) ? len : rs ;
} /* end subroutine (locinfo_nlookup) */

local int locinfo_ispara(LI *lip,bibleq_q *qp) noex {
	PI		*pip = lip->pip ;
	int		rs = SR_OK ;
	int		f = false ;

	if (qp == nullptr) return SR_FAULT ;

	if (lip->fl.para) {

	    if (! lip->open.para) {
	        rs = biblepara_open(&lip->pdb,pip->pr,lip->pdbname) ;
	        lip->open.para = (rs >= 0) ;
#if	CF_DEBUG
	        if (DEBUGLEVEL(5))
	            debugprintf("b_bibleverse/locinfo_ispara: "
	                "biblepara_open() rs=%d\n",rs) ;
#endif
	        if (isNotPresent(rs)) rs = SR_OK ;
	    }

	    if (rs >= 0) {
	        biblepara_q	pq ;
	        pq.b = qp->b ;
	        pq.c = qp->c ;
	        pq.v = qp->v ;
	        rs = biblepara_ispara(&lip->pdb,&pq) ;
	        f = (rs > 0) ;
#if	CF_DEBUG
	        if (DEBUGLEVEL(5))
	            debugprintf("b_bibleverse/locinfo_ispara: "
	                "biblepara_ispara() rs=%d\n",rs) ;
#endif
	    }

	} /* end if (ispara) */

	return (rs >= 0) ? f : rs ;
} /* end subroutine (locinfo_ispara) */



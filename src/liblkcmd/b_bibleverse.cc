/* b_bibleverse SUPPORT (KSH builtin) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* retrieve bible verses by citation */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* switchable at invocation */
#define	CF_DEBUGMALL	1		/* debug memory allocation */
#define	CF_DEBUGN	0		/* special */
#define	CF_COOKIE	0		/* use cookie as separator */
#define	CF_LOCNDAYS	0		/* use |locinfo_ndays()| */

/* revision history:

	= 2008-10-01, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	b_bibleverse

	Description:
	This is a built-in command to the KSH shell.  This little
	program looks up a number in a database and returns the
	corresponding string.

	Synopsis:
	$ bibleverse <bcspec(s)>

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

#include	<sys/types.h>		/* POSIX */
#include	<sys/param.h>		/* POSIX */
#include	<climits>		/* POSIX */
#include	<unistd.h>		/* POSIX */
#include	<fcntl.h>		/* POSIX */
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
#include	<tmtime.hh>		/* LIBUC */
#include	<dayspec.h>		/* LIBUC */
#include	<bcspec.h>		/* LIBUC */
#include	<ourmjd.h>		/* LIBUC */
#include	<exitcodes.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"shio.h"
#include	"kshlib.h"
#include	"b_bibleverse.h"
#include	"defs.h"
#include	"biblebook.h"
#include	"biblepara.h"
#include	"bibleverse.h"
#include	"bvs.h"
#include	"bvsmk.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#ifndef	LINEBUFLEN
#define	LINEBUFLEN	MAX((MAXPATHLEN + 2),2048)
#endif

#ifndef	BVBUFLEN
#define	BVBUFLEN	512		/* maximum bibleverse length (?) */
#endif

#ifndef	SPECBUFLEN
#define	SPECBUFLEN	40
#endif

#define	COLBUFLEN	(COLUMNS + 10)
#define	NDAYS		256		/* maximum verses per chapter */
#define	NBLANKS		20
#define	TIMECOUNT	5

#ifndef	TO_MKWAIT
#define	TO_MKWAIT	(1 * 50)
#endif

#ifndef	TO_TIME
#define	TO_TIME		5		/* time-out */
#endif

#define	TO_MJD		5		/* time-out for MJD */

#define	PI		proginfo

#define	LI		locinfo
#define	LI_FL		locinfo_fl

#define	NDF		"/tmp/bibleverse.nd"


/* external subroutines */


/* external variables */

extern char	**environ ;		/* definition required by AT&T AST */


/* local structures */

struct locinfo_fl {
	uint		audit:1 ;
	uint		linelen:1 ;
	uint		indent:1 ;
	uint		bookname:1 ;
	uint		separate:1 ;
	uint		interactive:1 ;
	uint		ndb:1 ;		/* DB name (book) */
	uint		vdb:1 ;		/* DB verse */
	uint		pdb:1 ;		/* DB paragraphs */
	uint		sdb:1 ;		/* DB verse-structure */
	uint		tmtime:1 ;
	uint		bvsmk:1 ;
	uint		mjd:1 ;
	uint		defnull:1 ;
	uint		nitems:1 ;
	uint		para:1 ;
	uint		gmt:1 ;
	uint		all:1 ;
	uint		apm:1 ;
} ; /* end struct */

struct locinfo {
	tmtime		tm ;		/* holds today's date, when set */
	BIBLEBOOK	ndb ;		/* bible-book-name DB */
	BIBLEVERSE	vdb ;
	BVS		sdb ;
	BVSMK		bsmk ;
	BIBLEPARA	pdb ;
	PI	*pip ;
	void		*ofp ;
	cchar	*ndbname ;	/* name-db name */
	cchar	*pdbname ;	/* paragraph-db name */
	cchar	*vdbname ;	/* verse-db name */
	cchar	*sdbname ;	/* structure-db name */
	LI_FL	have, fl, changed, finval ;
	LI_FL	open ;
	time_t		ti_tmtime ;
	time_t		ti_mjd ;
	int		timecount ;
	int		linelen ;
	int		indent ;
	int		nitems ;
	int		count, max, precision ;
	int		cout ;
	int		mjd ;
	int		ncites ;
	int		year ;
	int		qtype ;		/* query type */
} ; /* end struct */

struct vrbuf {
	int		vl ;
	char		vbuf[BVBUFLEN+1] ;
} ; /* end struct */


/* forward references */

local int	mainsub(int,cchar **,cchar **,void *) ;

local int	usage(PI *) ;

local int	procopts(PI *,keyopt *) ;
local int	process(PI *,ARGINFO *,bits *,cchar *,cchar *) ;
local int	procsome(PI *,ARGINFO *,bits *,cchar *) ;
local int	procspecs(PI *,cchar *,int) ;
local int	procspec(PI *,cchar *,int) ;
local int	procall(PI *) ;
local int	procparse(PI *,BIBLEVERSE_Q *,cchar *,int) ;
local int	procmulti(PI *,BIBLEVERSE_Q *,int) ;
local int	procload(PI *,vrbuf *,int,BIBLEVERSE_Q *) ;
local int	proctoday(PI *,int,int) ;
local int	procmjds(PI *,int,int) ;
local int	procoutcite(PI *,BIBLEVERSE_Q *,int) ;
local int	procout(PI *,BIBLEVERSE_Q *,cchar *,int) ;
local int	procoutline(PI *,int,cchar *,int) ;

local int	locinfo_start(LI *,PI *) ;
local int	locinfo_finish(LI *) ;
local int	locinfo_deflinelen(LI *) ;
local int	locinfo_booklookup(LI *,char *,int,int) ;
local int	locinfo_bookmatch(LI *,cchar *,int) ;
local int	locinfo_book(LI *) ;
local int	locinfo_today(LI *) ;
local int	locinfo_defdayspec(LI *,DAYSPEC *) ;
local int	locinfo_year(LI *) ;
local int	locinfo_tmtime(LI *) ;
local int	locinfo_mkmodquery(LI *,BIBLEVERSE_Q *,int) ;
local int	locinfo_bvs(LI *) ;
local int	locinfo_bvsbuild(LI *) ;
local int	locinfo_bvsbuilder(LI *) ;
local int	locinfo_ispara(LI *,BIBLEVERSE_Q *) ;

#if	CF_LOCNDAYS
local int	locinfo_ndays(LI *,BIBLEVERSE_Q *,int) ;
#endif

local bool	isNotGoodCite(int) noex ;


/* local variables */

enum argopts {
	argopt_root,
	argopt_version,
	argopt_help,
	argopt_sn,
	argopt_af,
	argopt_ef,
	argopt_of,
	argopt_book,
	argopt_pdb,
	argopt_vdb,
	argopt_sdb,
	argopt_bookname,
	argopt_overlast
} ; /* end enum (argopts) */

constexpr cpcchar	aonames[] = {
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
	"sdb",
	"bookname",
	nullptr
} ; /* end array (aonames) */

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
	{ SR_FAULT, EX_SOFTWARE },
	{ SR_NOTOPEN, EX_SOFTWARE },
	{ 0, 0 }
} ; /* end array */

enum akopts {
	akopt_audit,
	akopt_linelen,
	akopt_indent,
	akopt_bookname,
	akopt_separate,
	akopt_interactive,
	akopt_defnull,
	akopt_default,
	akopt_qtype,
	akopt_atype,
	akopt_para,
	akopt_gmt,
	akopt_overlast
} ; /* end enum (akopts) */

constexpr cpcchar	akonames[] = {
	"audit",
	"linelen",
	"indent",
	"bookname",
	"separate",
	"interactive",
	"defnull",
	"default",
	"qtype",
	"atype",
	"para",
	"gmt",
	nullptr
} ; /* end array (akonames) */

constexpr char		aterms[] = {
	0x00, 0x2E, 0x00, 0x00,
	0x09, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
} ; /* end array (aterms) */

enum qtypes {
	qtype_verse,
	qtype_day,
	qtype_mjd,
	qtype_overlast
} ; /* end enum (qtypes) */

static cchar	*qtnames[] = {
	"verses",
	"days",
	"mjds",
	nullptr
} ; /* end array (qtnames) */

constexpr char		blanks[] = "                    " ;


/* exported variables */


/* exported subroutines */

int b_bibleverse(int argc,con mainv argv,void *contextp) noex {
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
} /* end subroutine (b_bibleverse) */

int p_bibleverse(int argc,con mainv argv,con mainv envv,void *contextp) noex {
	return mainsub(argc,argv,envv,contextp) ;
} /* end subroutine (p_bibleverse) */


/* local subroutines */

local int mainsub(int argc,con mainv argv,con mainv envv,void *contextp) noex {
	PI	pi, *pip = &pi ;
	LI		li, *lip = &li ;
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
	cchar	*ofname = nullptr ;
	cchar	*efname = nullptr ;
	cchar	*ndbname = nullptr ;
	cchar	*pdbname = nullptr ;
	cchar	*vdbname = nullptr ;
	cchar	*sdbname = nullptr ;
	cchar	*qtypestr = nullptr ;
	cchar	*cp ;


#if	CF_DEBUG || CF_DEBUG
	if ((cp = getourenv(envv,VARDEBUGFNAME)) != nullptr) {
	    rs = debugopen(cp) ;
	    debugprintf("b_bibleverse: starting DFD=%d\n",rs) ;
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
	    argl = lenstr(argp) ;

	    f_optminus = (*argp == '-') ;
	    f_optplus = (*argp == '+') ;
	    if ((argl > 1) && (f_optminus || f_optplus)) {
	        cint	ach = MKCHAR(argp[1]) ;

	        if (isdigitlatin(ach)) {

	            if (f_optplus) lip->fl.apm = true ;
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

	            if ((kwi = matostr(aonames,2,akp,akl)) >= 0) {

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
	                            argl = lenstr(argp) ;
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
	                            argl = lenstr(argp) ;
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
	                            argl = lenstr(argp) ;
	                            if (argl)
	                                ofname = argp ;
	                        } else
	                            rs = SR_INVALID ;
	                    }
	                    break ;

/* BibleBook-name DB name */
	                case argopt_book:
	                    if (f_optequal) {
	                        f_optequal = false ;
	                        if (avl)
	                            ndbname = avp ;
	                    } else {
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = lenstr(argp) ;
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
	                            argl = lenstr(argp) ;
	                            if (argl)
	                                pdbname = argp ;
	                        } else
	                            rs = SR_INVALID ;
	                    }
	                    break ;

/* BibleBook-verse DB name */
	                case argopt_vdb:
	                    if (f_optequal) {
	                        f_optequal = false ;
	                        if (avl)
	                            vdbname = avp ;
	                    } else {
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = lenstr(argp) ;
	                            if (argl)
	                                vdbname = argp ;
	                        } else
	                            rs = SR_INVALID ;
	                    }
	                    break ;

/* structure-db name */
	                case argopt_sdb:
	                    if (f_optequal) {
	                        f_optequal = false ;
	                        if (avl)
	                            sdbname = avp ;
	                    } else {
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = lenstr(argp) ;
	                            if (argl)
	                                sdbname = argp ;
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

	                    case 'a':
	                        lip->fl.all = true ;
	                        if (f_optequal) {
	                            f_optequal = false ;
	                            if (avl) {
	                                rs = optbool(avp,avl) ;
	                                lip->fl.all = (rs > 0) ;
	                            }
	                        }
	                        break ;

/* type of argument-input */
	                    case 'i':
	                    case 't':
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = lenstr(argp) ;
	                            if (argl)
	                                qtypestr = argp ;
	                        } else
	                            rs = SR_INVALID ;
	                        break ;

/* number of verses to print */
	                    case 'n':
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = lenstr(argp) ;
	                            if (argl) {
	                                lip->have.nitems = true ;
	                                lip->finval.nitems = true ;
	                                rs = optvalue(argp,argl) ;
	                                lip->nitems = rs ;
	                            }
	                        } else
	                            rs = SR_INVALID ;
	                        break ;

/* options */
	                    case 'o':
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = lenstr(argp) ;
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
	                            argl = lenstr(argp) ;
	                            if (argl) {
	                                lip->have.linelen = true ;
	                                lip->finval.linelen = true ;
	                                rs = optvalue(argp,argl) ;
	                                lip->linelen = rs ;
	                            }
	                        } else
	                            rs = SR_INVALID ;
	                        break ;

/* default year */
	                    case 'y':
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = lenstr(argp) ;
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

	if (rs < 0) goto badarg ;

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    debugprintf("b_bibleverse: debuglevel=%u\n",pip->debuglevel) ;
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
	} /* end if */

	if (f_version || f_help || f_usage)
	    goto retearly ;


	ex = EX_OK ;

/* argument processing */

	if ((lip->nitems <= 0) && (argval != nullptr)) {
	    lip->have.nitems = true ;
	    lip->finval.nitems = true ;
	    rs = optvalue(argval,-1) ;
	    lip->nitems = rs ;
	}

/* load up the environment options */

	if (rs >= 0) {
	    rs = procopts(pip,&akopts) ;
	}

/* argument defaults */

	if (afname == nullptr) afname = getourenv(pip->envv,VARAFNAME) ;

/* name-db name */

	if (ndbname == nullptr) ndbname = getourenv(envv,VARNDB) ;
	lip->ndbname = ndbname ;

/* verse-db name */

	if (vdbname == nullptr) vdbname = getourenv(envv,VARVDB) ;
	if (vdbname == nullptr) vdbname = VDBNAME ;
	lip->vdbname = vdbname ;

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    debugprintf("b_bibleverse: vdbname=%s\n",
	        ((vdbname != nullptr) ? vdbname : "nullptr")) ;
#endif

/* structure-db name */

	if (sdbname == nullptr) sdbname = getourenv(envv,VARSDB) ;
	if (sdbname == nullptr) sdbname = SDBNAME ;
	lip->sdbname = sdbname ;

/* paragraph-db name */

	if (pdbname == nullptr) pdbname = getourenv(envv,VARPDB) ;
	lip->pdbname = pdbname ;

/* type of argument-input */

	if ((rs >= 0) && (qtypestr != nullptr)) {
	    if ((v = matostr(qtnames,1,qtypestr,-1)) >= 0) {
	        lip->qtype = v ;
	        if (pip->debuglevel > 0) {
	            shio_printf(pip->efp,"%s: qtype=%s(%u)\n",
	                pip->progname,qtnames[v],v) ;
	        }
	    } else {
		rs = SR_INVALID ;
	    }
	}

#if	CF_DEBUG
	if (DEBUGLEVEL(2) && (qtypestr != nullptr))
	    debugprintf("b_bibleverse: qtype=%s(%u)\n",qtypestr,lip->qtype) ;
#endif

/* debugging */

	if (pip->debuglevel > 0) {
	    cchar	*pn = pip->progname ;
	    shio_printf(pip->efp,"%s: ndb=%s\n",
	        pn,((ndbname != nullptr) ? ndbname : "nullptr")) ;
	    shio_printf(pip->efp,"%s: pdb=%s\n",
	        pn,((pdbname != nullptr) ? pdbname : "nullptr")) ;
	    shio_printf(pip->efp,"%s: vdb=%s\n",
	        pn,((vdbname != nullptr) ? vdbname : "nullptr")) ;
	    shio_printf(pip->efp,"%s: sdb=%s\n",
	        pn,((sdbname != nullptr) ? sdbname : "nullptr")) ;
	}

	if (rs >= 0) {
	    rs = locinfo_deflinelen(lip) ;
	}

#if	CF_DEBUGN
	nprintf(NDF,"b_bibleverse: 3 ll=%d\n",lip->linelen) ;
#endif

	if ((lip->nitems < 1) && (! lip->have.nitems)) lip->nitems = 1 ;

	if (lip->nitems < 0) lip->nitems = 1 ;

	if (pip->debuglevel > 0) {
	    cchar	*pn = pip->progname ;
	    shio_printf(pip->efp,"%s: linelen=%u\n",pn,lip->linelen) ;
	    shio_printf(pip->efp,"%s: indent=%u\n",pn,lip->indent) ;
	    shio_printf(pip->efp,"%s: nitems=%u\n",pn,lip->nitems) ;
	}

/* process */

	memset(&ainfo,0,sizeof(ARGINFO)) ;
	ainfo.argc = argc ;
	ainfo.ai = ai ;
	ainfo.argv = argv ;
	ainfo.ai_max = ai_max ;
	ainfo.ai_pos = ai_pos ;

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    debugprintf("b_bibleverse: bibleverse_open() vdbname=%s\n",
	        vdbname) ;
#endif

	if (rs >= 0) {
	    BIBLEVERSE	*bvp = &lip->vdb ;
	    cchar	*pn = pip->progname ;
	    cchar	*fmt ;
	    if ((rs = bibleverse_open(bvp,pip->pr,vdbname)) >= 0) {
	        cint	nverses = rs ;
	        cchar	*ofn = ofname ;
	        cchar	*afn = afname ;
	        lip->open.vdb = true ;

#if	CF_DEBUG
	        if (DEBUGLEVEL(2))
	            debugprintf("b_bibleverse: bibleverse_open() rs=%d\n",rs) ;
#endif


	        if (lip->fl.audit) {
	            rs = bibleverse_audit(bvp) ;
	            if (pip->debuglevel > 0) {
		        fmt = "%s: bibleverse DB audit (%d)\n" ;
	                shio_printf(pip->efp,fmt,pn,rs) ;
	            }
	        }

	        if (pip->debuglevel > 0) {
		    fmt = "%s: total verses=%u\n" ;
	            shio_printf(pip->efp,fmt,pn,nverses) ;
	        }

#if	CF_DEBUG
	        if (DEBUGLEVEL(2)) {
	            BIBLEVERSE_INFO	bi ;
	            rs = bibleverse_info(bvp,&bi) ;
	            debugprintf("b_bibleverse: bibleverse_info() rs=%d\n",rs) ;
	            debugprintf("b_bibleverse: maxbook=%u\n",bi.maxbook) ;
	            debugprintf("b_bibleverse: maxchapter=%u\n",bi.maxchapter) ;
	            debugprintf("b_bibleverse: nverses=%u\n",bi.nverses) ;
	            debugprintf("b_bibleverse: nzverses=%u\n",bi.nzverses) ;
	        }
#endif /* CF_DEBUG */

	        if (rs >= 0) {
	            rs = process(pip,&ainfo,&pargs,ofn,afn) ;
	        }

	        lip->open.vdb = false ;
	        rs1 = bibleverse_close(bvp) ;
	        if (rs >= 0) rs = rs1 ;
	    } else {
#if	CF_DEBUG
	        if (DEBUGLEVEL(2))
	            debugprintf("b_bibleverse: bibleverse-out rs=%d\n",rs) ;
#endif
	        fmt = "%s: could not load bibleverse DB (%d)\n" ;
	        shio_printf(pip->efp,fmt,pn,rs) ;
	        shio_printf(pip->efp,"%s: vdbname=%s\n",pn,vdbname) ;
	    } /* end if (bibleverse) */
	} else if (ex == EX_OK) {
	    cchar	*pn = pip->progname ;
	    cchar	*fmt = "%s: invalid argument or configuration (%d)\n" ;
	    ex = EX_USAGE ;
	    shio_printf(pip->efp,fmt,pn,rs) ;
	    usage(pip) ;
	}

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    debugprintf("bibleverse: done rs=%d\n",rs) ;
#endif

/* done */
	if ((rs < 0) && (ex == EX_OK)) {
	    ex = mapex(mapexs,rs) ;
	    if (! pip->fl.quiet) {
	        shio_printf(pip->efp,
	            "%s: could not perform function (%d)\n",
	            pip->progname,rs) ;
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
	rs1 = locinfo_finish(lip) ;
	if (rs >= 0) rs = rs1 ;

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	debugprintf("b_bibleverse: locinfo_finish() rs=%d\n",rs) ;
#endif

badlocstart:
	rs1 = proginfo_finish(pip) ;
	if (rs >= 0) rs = rs1 ;

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	debugprintf("b_bibleverse: proginfo_finish() rs=%d\n",rs) ;
#endif

badprogstart:

#if	(CF_DEBUG || CF_DEBUG) && CF_DEBUGMALL
	{
	    uint	mo ;
	    uc_mallout(&mo) ;
	    debugprintf("b_bibleverse/mainsub: fin mallout=%u\n",mo-mo_start) ;
	    uc_mallset(0) ;
	}
#endif

#if	(CF_DEBUG || CF_DEBUG)
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
	cchar	*pn = pip->progname ;
	cchar	*fmt ;

	fmt = "%s: USAGE> %s [<number(s)>|<cite(s)> ...] [-af <afile>]\n" ;
	if (rs >= 0) rs = shio_printf(pip->efp,fmt,pn,pn) ;
	wlen += rs ;

	fmt = "%s:  [-<n>] [-n <nverses>] [-y <defyear>] [-t <querytype>]\n" ;
	if (rs >= 0) rs = shio_printf(pip->efp,fmt,pn) ;
	wlen += rs ;

	fmt = "%s:  [-ndb <booknamedb>] [-vdb <versedb>] [-a] [-w <width>]\n" ;
	if (rs >= 0) rs = shio_printf(pip->efp,fmt,pn) ;
	wlen += rs ;

	fmt = "%s:  [-Q] [-D] [-v[=<n>]] [-HELP] [-V]\n" ;
	if (rs >= 0) rs = shio_printf(pip->efp,fmt,pn) ;
	wlen += rs ;

	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (usage) */

local int procopts(PI *pip,keyopt *kop) noex {
	LI		*lip = pip->lip ;
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

	        while ((kl = keyopt_curenumkeys(kop,&kcur,&kp)) >= 0) {

	            if ((oi = matostr(akonames,2,kp,kl)) >= 0) {

	                vl = keyopt_fetch(kop,kp,nullptr,&vp) ;

	                switch (oi) {
	                case akopt_audit:
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
	                case akopt_linelen:
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
	                case akopt_indent:
	                    if (! lip->finval.indent) {
	                        lip->have.indent = true ;
	                        lip->finval.indent = true ;
	                        lip->fl.indent = true ;
	                        lip->indent = 1 ;
	                        if (vl > 0) {
	                            rs = optvalue(vp,vl) ;
	                            lip->indent = rs ;
	                        }
	                    }
	                    break ;
	                case akopt_bookname:
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
	                case akopt_separate:
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
	                case akopt_interactive:
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
	                case akopt_default:
	                case akopt_defnull:
	                    if (! lip->finval.defnull) {
	                        lip->have.defnull = true ;
	                        lip->finval.defnull = true ;
	                        lip->fl.defnull = true ;
	                        if (vl > 0) {
	                            rs = optbool(vp,vl) ;
	                            lip->fl.defnull = (rs > 0) ;
	                        }
	                    }
	                    break ;
	                case akopt_para:
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
	                case akopt_gmt:
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
	                case akopt_atype:
	                case akopt_qtype:
	                    if (vl) {
	                        rs = matostr(qtnames,1,vp,vl) ;
	                        lip->qtype = rs ;
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

local int process(PI *pip,ARGINFO *aip,bits *bop,cchar *ofn,cchar *afn) noex {
	LI		*lip = pip->lip ;
	SHIO		ofile, *ofp = &ofile ;
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;

	if ((ofn == nullptr) || (ofn[0] == '\0') || (ofn[0] == '-'))
	    ofn = STDFNOUT ;

	if ((rs = shio_open(ofp,ofn,"wct",0666)) >= 0) {
	    lip->ofp = ofp ;

	    if (lip->fl.all) {
	        rs = procall(pip) ;
	        wlen += rs ;
	    } else {
	        rs = procsome(pip,aip,bop,afn) ;
	        wlen += rs ;
	    }

	    lip->ofp = nullptr ;
	    rs1 = shio_close(ofp) ;
	    if (rs >= 0) rs = rs1 ;
	} else {
	    cchar	*pn = pip->progname ;
	    cchar	*fmt ;
	    fmt = "%s: inaccessible output (%d)\n" ;
	    shio_printf(pip->efp,fmt,pn,rs) ;
	    shio_printf(pip->efp,"%s: ofile=%s\n",pn,ofn) ;
	}

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	debugprintf("b_bibleverse/process: ret rs=%d\n",rs) ;
#endif

	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (process) */

local int procsome(PI *pip,ARGINFO *aip,bits *bop,cchar *afn) noex {
	LI		*lip = pip->lip ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		wlen = 0 ;
	int		pan = 0 ;
	int		cl ;
	cchar	*cp ;

	if (rs >= 0) {
	    int		ai ;
	    int		f ;
	    cchar	**argv = aip->argv ;

	    for (ai = 1 ; ai < aip->argc ; ai += 1) {

	        f = (ai <= aip->ai_max) && (bits_test(bop,ai) > 0) ;
	        f = f || ((ai > aip->ai_pos) && (argv[ai] != nullptr)) ;
	        if (f) {
	            cp = argv[ai] ;
	            if (cp[0] != '\0') {
	                pan += 1 ;
	                rs = procspec(pip,cp,-1) ;
	                wlen += rs ;
	            }
	        }

	        if (rs >= 0) rs = lib_sigterm() ;
	        if (rs >= 0) rs = lib_sigintr() ;
	        if (rs < 0) break ;
	    } /* end for (looping through positional arguments) */

	} /* end if (positional arguments) */

	if ((rs >= 0) && (afn != nullptr) && (afn[0] != '\0')) {
	    SHIO	afile, *afp = &afile ;

	    if (strcmp(afn,"-") == 0)
	        afn = STDFNIN ;

	    if ((rs = shio_open(afp,afn,"r",0666)) >= 0) {
	        cint	llen = LINEBUFLEN ;
	        int		len ;
	        char		lbuf[LINEBUFLEN + 1] ;

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
		cchar	*pn = pip->progname ;
		cchar	*fmt ;
	        fmt = "%s: inaccessible argument-list (%d)\n" ;
	        shio_printf(pip->efp,fmt,pn,rs) ;
	        shio_printf(pip->efp,"%s: afile=%s\n",pn,afn) ;
	    } /* end if */

	} /* end if (afile arguments) */

	if ((rs >= 0) && lip->fl.apm) {

#if	CF_DEBUG
	    if (DEBUGLEVEL(2))
	        debugprintf("b_bibleverse/procsome: +<n>\n") ;
#endif

	    pan += 1 ;
	    rs = proctoday(pip,lip->fl.apm,(lip->nitems+1)) ;
	    wlen += rs ;

	} /* end if */

	if ((rs >= 0) && (pan == 0) && lip->fl.defnull) {
	    int	ndays = 1 ;

	    if (lip->nitems > 1) ndays = lip->nitems ;
	    rs = proctoday(pip,true,ndays) ;
	    wlen += rs ;

	} /* end if */

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    debugprintf("b_bibleverse/procsome: ret rs=%d\n",rs) ;
#endif

	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (procsome) */

local int procspecs(PI *pip,cchar *sp,int sl) noex {
	LI		*lip = pip->lip ;
	int		rs ;
	int		wlen = 0 ;

	if (lip->fl.interactive) lip->cout = 0 ;

	if (sl < 0) sl = lenstr(sp) ;

	if (field fsb ; (rs = field_start(&fsb,sp,sl)) >= 0) {
	    cint	flen = sl ;
	    char	*fbuf ;
	    if ((rs = uc_malloc((flen+1),&fbuf)) >= 0) {
	        int	fl ;
	        while ((fl = field_sharg(&fsb,aterms,fbuf,flen)) >= 0) {
	            if (fl > 0) {
	                rs = procspec(pip,fbuf,fl) ;
	                wlen += rs ;
	            }
	            if (fsb.term == '#') break ;
	            if (rs < 0) break ;
	        } /* end while */
	        uc_free(fbuf) ;
	    } /* end if (m-a) */
	    field_finish(&fsb) ;
	} /* end if (field) */

	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (procspecs) */

local int procspec(PI *pip,cchar *sp,int sl) noex {
	LI		*lip = pip->lip ;
	int		rs = SR_OK ;
	int		ndays = lip->nitems ;
	int		wlen = 0 ;
	cchar		*pn = pip->progname ;
	cchar		*fmt ;

	if (sp == nullptr) return SR_FAULT ;

	if (sl < 0) sl = lenstr(sp) ;

	if ((sl > 0) && (sp[0] != '\0')) {

#if	CF_DEBUG
	    if (DEBUGLEVEL(3))
	        debugprintf("b_bibleverse/procspec: spec>%r<\n",sp,sl) ;
#endif

	    if (pip->debuglevel > 0) {
	        shio_printf(pip->efp,"%s: spec=\"%r\"\n",pn,sp,sl) ;
	    }

	    if ((sp[0] == '+') || (sp[0] == '-')) {
	        cint	f_plus = (sp[0] == '+') ;

	        rs = proctoday(pip,f_plus,ndays) ;
	        wlen += rs ;

	    } else if (lip->qtype == qtype_verse) {
	        BIBLEVERSE_Q	q ;

	        if ((rs = procparse(pip,&q,sp,sl)) > 0) {
	            rs = procmulti(pip,&q,ndays) ;
	            wlen += rs ;
	        } else if (rs == 0) {
	            if (lip->fl.interactive) {
			fmt = "citation=>%r< invalid\n" ;
	                rs = shio_printf(lip->ofp,fmt,sp,sl) ;
		    }
	            if (pip->debuglevel > 0) {
			fmt = "%s: citation=>%r< invalid (%d)\n" ;
	                shio_printf(pip->efp,fmt,pn,sp,sl,rs) ;
	            }
	        } /* end if */

#if	CF_DEBUG
	        if (DEBUGLEVEL(3) && (rs >= 0))
	            debugprintf("b_bibleverse/procspec: get q=%u:%u:%u\n",
	                q.b,q.c,q.v) ;
#endif

	    } else {
	        int	mjd = -1 ;

	        if (lip->qtype == qtype_day) {
	            if ((rs = ourmjd(sp,sl)) > 0) {
	                mjd = rs ;
	            } else {
	                DAYSPEC	ds ;
	                if ((rs = dayspec_load(&ds,sp,sl)) >= 0) {
	                    if ((rs = locinfo_defdayspec(lip,&ds)) >= 0) {
	                        rs = getmjd(ds.y,ds.m,ds.d) ;
	                        mjd = rs ;
	                    }
	                } /* end if (dayspec) */
	            } /* end if (type-day) */
	        } else if (lip->qtype == qtype_mjd) {
	            if ((rs = ourmjd(sp,sl)) > 0) {
	                mjd = rs ;
	            } else {
	                rs = optvalue(sp,sl) ;
	                mjd = rs ;
	            }
	        } else {
	            rs = SR_INVALID ;
		}

	        if ((rs >= 0) && (mjd >= 0)) {
	            rs = procmjds(pip,mjd,ndays) ;
	            wlen += rs ;
	        }

	    } /* end if (handling different query types) */

	    if ((rs < 0) && isNotGoodCite(rs) && lip->fl.interactive) {
		fmt = "invalid citation=>%r< (%d)\n" ;
	        rs = shio_printf(lip->ofp,fmt,sp,sl,rs) ;
	    }

	} /* end if (ok) */

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	    debugprintf("b_bibleverse/procspec: ret rs=%d wlen=%u\n",rs,wlen) ;
#endif

	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (procspec) */

local int procall(PI *pip) noex {
	LI		*lip = pip->lip ;
	BIBLEVERSE	*vdbp ;
	BIBLEVERSE_CUR	cur ;
	BIBLEVERSE_Q	q ;	/* result */
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;

	vdbp = &lip->vdb ;
	if ((rs = bibleverse_curbegin(vdbp,&cur)) >= 0) {
	    cint	bvlen = BVBUFLEN ;
	    int		bvl ;
	    char	bvbuf[BVBUFLEN + 1] ;

	    while (rs >= 0) {
	        bvl = bibleverse_curenum(vdbp,&cur,&q,bvbuf,bvlen) ;
	        if ((bvl == SR_NOTFOUND) || (bvl == 0)) break ;
	        rs = bvl ;

#if	CF_DEBUG
	        if (DEBUGLEVEL(2)) {
	            debugprintf("bibleverse: bvl=%u\n",bvl) ;
	            debugprintf("bibleverse: q=%u:%u:%u\n",q.b,q.c,q.v) ;
	        }
#endif

	        if (rs >= 0) {
	            rs = procout(pip,&q,bvbuf,bvl) ;
	            wlen += rs ;
	        }

	        if (rs >= 0) rs = lib_sigterm() ;
	        if (rs >= 0) rs = lib_sigintr() ;
	    } /* end while */

	    rs1 = bibleverse_curend(vdbp,&cur) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (printing all book titles) */

	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (procall) */

local int procparse(PI *pip,BIBLEVERSE_Q *qp,cchar *sp,int sl) noex {
	BCSPEC		bb ;
	int		rs ;

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	debugprintf("b_bibleverse/procparse: ent q=>%r<\n",sp,sl) ;
#endif

	if ((rs = bcspec_load(&bb,sp,sl)) >= 0) {
	    cint	nl = bb.nl ;
	    cchar	*np = bb.np ;
	    qp->b = bb.b ;
	    qp->c = bb.c ;
	    qp->v = bb.v ;
	    if (np != nullptr) {
	        LI		*lip = pip->lip ;
	        rs = locinfo_bookmatch(lip,np,nl) ;
	        qp->b = (uchar) rs ;
	    }
	} /* end if (bcspec_load) */

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	debugprintf("b_bibleverse/procparse: ret rs=%d\n",rs) ;
#endif

	return rs ;
} /* end subroutine (procparse) */

local int proctoday(PI *pip,int f_plus,int ndays) noex {
	LI		*lip = pip->lip ;
	int		rs ;
	int		wlen = 0 ;

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	    debugprintf("b_bibleverse/proctoday: ent f_plus=%u ndays=%d\n",
	        f_plus,ndays) ;
#endif

	if ((rs = locinfo_today(lip)) >= 0) {
	    int	mjd = rs ;
	    rs = procmjds(pip,mjd,ndays) ;
	    wlen += rs ;
	}

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	    debugprintf("b_bibleverse/proctoday: ret rs=%d\n",rs) ;
#endif

	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (proctoday) */

local int procmjds(PI *pip,int mjd,int ndays) noex {
	LI		*lip = pip->lip ;
	BIBLEVERSE_Q	q ;
	int		rs ;
	int		wlen = 0 ;

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	    debugprintf("b_bibleverse/procmjds: mjd=%u ndays=%d\n",mjd,ndays) ;
#endif

	if ((rs = locinfo_mkmodquery(lip,&q,mjd)) >= 0) {
	    rs = procmulti(pip,&q,ndays) ;
	    wlen += rs ;
	}

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	    debugprintf("b_bibleverse/procmjds: ret rs=%d\n",rs) ;
#endif

	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (procmjds) */

local int procmulti(PI *pip,BIBLEVERSE_Q *qp,int ndays) noex {
	LI		*lip = pip->lip ;
	vrbuf		*vrp ;
	int		rs ;
	int		rs1 ;
	int		size ;
	int		wlen = 0 ;

	if (ndays < 1) {
	    ndays = 1 ;
	} else if (ndays > NDAYS) {
	    ndays = NDAYS ;
	}

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("b_bibleverse/procmulti: q=%u:%u:%u ndays=%d\n",
	        qp->b,qp->c,qp->v,ndays) ;
#endif

	size = (ndays * sizeof(vrbuf)) ;
	if ((rs = uc_malloc(size,&vrp)) >= 0) {
	    if ((rs = procload(pip,vrp,ndays,qp)) > 0) {
		cint	nv = rs ;
	        int		i ;

	        lip->ncites += nv ;

		if (rs >= 0) {
	            rs = procoutcite(pip,qp,nv) ;
	            wlen += rs ;
		}

	        if (rs >= 0) {
	            rs = procout(pip,qp,vrp->vbuf,vrp->vl) ;
	            wlen += rs ;
	            qp->v += 1 ;
	        }

	        for (i = 1 ; (rs >= 0) && (i < nv) ; i += 1) {
		    char	*vbuf = vrp[i].vbuf ;
		    cint	vl = vrp[i].vl ;
	            rs = procout(pip,qp,vbuf,vl) ;
	            wlen += rs ;
	            qp->v += 1 ;
	        } /* end for */

            } else if (rs == SR_NOTFOUND) {
		cint	speclen = SPECBUFLEN ;
		int		sl ;
		cchar		*pn = pip->progname ;
		cchar		*fmt = "%u:%u:%u" ;
		char		specbuf[SPECBUFLEN + 1], *sp = specbuf ;
		sl = bufprintf(specbuf,speclen,fmt,qp->b,qp->c,qp->v) ;
		if (lip->fl.interactive) {
	    	    fmt = "citation=%r not_found\n" ;
	    	    rs = shio_printf(lip->ofp,fmt,sp,sl) ;
		}
		if (pip->debuglevel > 0) {
	    	    fmt = "%s: citation=%r not_found\n" ;
	    	    shio_printf(pip->efp,fmt,pn,sp,sl) ;
		}
    	    } /* end if (procload) */
	    rs1 = uc_free(vrp) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a) */

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("b_bibleverse/procmulti: ret rs=%d\n",rs) ;
#endif

	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (procmulti) */

local int procload(PI *pip,vrbuf *vrp,int nbuf,BIBLEVERSE_Q *qp) noex {
	LI		*lip = pip->lip ;
	BIBLEVERSE	*dbp ;
	BIBLEVERSE_Q	q = *qp ;
	cint	bvlen = BVBUFLEN ;
	int		rs = SR_OK ;
	int		vl ;
	int		nb = 0 ;
	dbp = &lip->vdb ;
#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("b_bibleverse/procload: nbuf=%u\n",nbuf) ;
#endif
	while ((rs >= 0) && (nb < nbuf)) {
	    vl = bibleverse_read(dbp,vrp->vbuf,bvlen,&q) ;
	    if (vl == SR_NOTFOUND) break ;
	    rs = vl ;
	    if (rs < 0) break ;
	    vrp->vl = vl ;
	    vrp += 1 ;
	    nb += 1 ;
	    if (q.v == 255U) break ;
	    q.v += 1 ;
	} /* end while */
#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("b_bibleverse/procload: ret rs=%d\n",rs) ;
#endif
	return (rs >= 0) ? nb : rs ;
} /* end subroutine (procload) */

local int procoutcite(PI *pip,BIBLEVERSE_Q *qp,int ndays) noex {
	LI		*lip = pip->lip ;
	int		rs = SR_OK ;
	int		wlen = 0 ;
	int		f_havebook = false ;
	cchar	*fmt ;

#if	CF_COOKIE
	fmt = "%%\n" ;
#else
	fmt = "\n" ;
#endif

/* print out any necessary separator */

	if ((rs >= 0) && lip->fl.separate && (lip->cout++ > 0)) {
	    if (pip->verboselevel > 0) {
	        rs = shio_printf(lip->ofp,fmt) ;
	        wlen += rs ;
	    }
	} /* end if (separator) */

/* print out the text-data itself */

	if (rs >= 0) {
	    cint	clen = COLBUFLEN ;
	    int		cl ;
	    int		b = qp->b ;
	    int		c = qp->c ;
	    int		v = qp->v ;
	    char	cbuf[COLBUFLEN + 1] ;

	    if (lip->fl.bookname) {
	        cint	blen = BIBLEBOOK_LEN ;
	        int		bbl ;
	        char		bbuf[BIBLEBOOK_LEN + 1] ;

	        if ((bbl = locinfo_booklookup(lip,bbuf,blen,qp->b)) > 0) {

	            f_havebook = true ;
	            fmt = (ndays > 1) ? "%r %u:%u (%u)" : "%r %u:%u" ;
	            rs = bufprintf(cbuf,clen,fmt,bbuf,bbl,c,v,ndays) ;
	            cl = rs ;
	            if ((rs >= 0) && (pip->verboselevel > 0)) {
	                rs = shio_print(lip->ofp,cbuf,cl) ;
	                wlen += rs ;
	            }

	        } /* end if (nlookup) */

	    } /* end if (book-name) */

	    if ((rs >= 0) && (! f_havebook)) {

	        fmt = (ndays > 1) ? "%u:%u:%u (%u)" : "%u:%u:%u" ;
	        rs = bufprintf(cbuf,clen,fmt,b,c,v,ndays) ;
	        cl = rs ;
	        if ((rs >= 0) && (pip->verboselevel > 0)) {
	            rs = shio_print(lip->ofp,cbuf,cl) ;
	            wlen += rs ;
	        }

	    } /* end if (type of book-name display) */

	} /* end if (ok) */

	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (procoutcite) */

local int procout(PI *pip,BIBLEVERSE_Q *qp,cchar *vp,int vl) noex {
	LI		*lip = pip->lip ;
	WORDFILL	w ;
	cint	clen = COLBUFLEN ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		sl = vl ;
	int		cl ;
	int		cbl ;
	int		line = 0 ;
	int		wlen = 0 ;
	int		f_p = false ;
	cchar	*sp = vp ;
	char		cbuf[COLBUFLEN + 1] ;

#if	CF_DEBUG
	if (DEBUGLEVEL(5))
	    debugprintf("b_bibleverse/procout: ent vl=%d\n",vl) ;
#endif

	cbl = MIN((lip->linelen - lip->indent),clen) ;

#if	CF_DEBUGN
	nprintf(NDF,"b_bibleverse/procout: clen=%d ll=%d ind=%d cbl=%d\n",
		clen,lip->linelen,lip->indent,cbl) ;
#endif

	if ((rs >= 0) && lip->fl.para) {
	    rs = locinfo_ispara(lip,qp) ;
	    f_p = (rs > 0) ;
	}

/* print out the text-data itself */

	if ((rs >= 0) && (vl > 0)) {
	    if (f_p) sp = nullptr ;
	    if ((rs = wordfill_start(&w,sp,sl)) >= 0) {

	        if (f_p) {
	            if ((rs = wordfill_addword(&w,"¶",1)) >= 0) {
	                rs = wordfill_addlines(&w,vp,vl) ;
		    }
	        }

	        while (rs >= 0) {
	            cl = wordfill_mklinefull(&w,cbuf,cbl) ;
	            if ((cl == 0) || (cl == SR_NOTFOUND)) break ;
	            rs = cl ;

#if	CF_DEBUGN
	nprintf(NDF,"b_bibleverse/procout: 1 cl=%d\n",cl) ;
#endif

	            if (rs >= 0) {
	                rs = procoutline(pip,line,cbuf,cl) ;
	                wlen += rs ;
	                line += 1 ;
	            }

	        } /* end while (full lines) */

	        if (rs >= 0) {
	            if ((cl = wordfill_mklinepart(&w,cbuf,cbl)) > 0) {
#if	CF_DEBUGN
	nprintf(NDF,"b_bibleverse/procout: 2 cl=%d\n",cl) ;
#endif
	                rs = procoutline(pip,line,cbuf,cl) ;
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

local int procoutline(PI *pip,int line,cchar *lp,int ll) noex {
	LI		*lip = pip->lip ;
	int		rs = SR_OK ;
	int		indent ;
	int		wlen = 0 ;
	cchar	*fmt ;

	if (pip->verboselevel > 0) {
	    indent = MIN(lip->indent,NBLANKS) ;
	    fmt = "%r%r\n" ;
	    rs = shio_printf(lip->ofp,fmt,blanks,indent,lp,ll) ;
	    wlen += rs ;
	}

	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (procoutline) */

local int locinfo_start(LI *lip,PI *pip) noex {
	int		rs = SR_OK ;

	memclear(lip) ;
	lip->pip = pip ;
	lip->indent = 1 ;
	lip->count = -1 ;
	lip->max = -1 ;
	lip->nitems = -1 ;

	lip->fl.separate = OPT_SEPARATE ;
	lip->fl.bookname = OPT_BOOKNAME ;

	return rs ;
} /* end subroutine (locinfo_start) */

local int locinfo_finish(LI *lip) noex {
	PI	*pip = lip->pip ;
	int		rs = SR_OK ;
	int		rs1 ;

	if (pip == nullptr) return SR_FAULT ;

	if (lip->open.pdb) {
	    lip->open.pdb = false ;
	    rs1 = biblepara_close(&lip->pdb) ;
	    if (rs >= 0) rs = rs1 ;
	}

	if (lip->open.sdb) {
	    lip->open.sdb = false ;
	    rs1 = bvs_close(&lip->sdb) ;
	    if (rs >= 0) rs = rs1 ;
	}

	if (lip->open.ndb) {
	    lip->open.ndb = false ;
	    rs1 = biblebook_close(&lip->ndb) ;
	    if (rs >= 0) rs = rs1 ;
	}

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	    debugprintf("locinfo_finish: ret rs=%d\n",rs) ;
#endif

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
	if (lip->linelen < def ) lip->linelen = COLUMNS ;
	return rs ;
} /* end subroutine (locinfo_deflinelen) */

local int locinfo_booklookup(LI *lip,char *rbuf,int rlen,int bi) noex {
	PI	*pip = lip->pip ;
	int		rs ;
	int		len = 0 ;

	if (pip == nullptr) return SR_FAULT ;
	rbuf[0] = '\0' ;
	if ((rs = locinfo_book(lip)) >= 0) {
	    BIBLEBOOK	*bbp = &lip->ndb ;
	    rs = biblebook_lookup(bbp,rbuf,rlen,bi) ;
	    len = rs ;
	}

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    debugprintf("b_bibleverse/locinfo_booklookup: ret rs=%d len=%u\n",
	        rs,len) ;
#endif

	return (rs >= 0) ? len : rs ;
} /* end subroutine (locinfo_booklookup) */

local int locinfo_bookmatch(LI *lip,cchar *mbuf,int mlen) noex {
	PI	*pip = lip->pip ;
	int		rs ;
	int		bi = 0 ;

	if (pip == nullptr) return SR_FAULT ;
	if ((rs = locinfo_book(lip)) >= 0) {
	    BIBLEBOOK	*bbp = &lip->ndb ;
	    rs = biblebook_match(bbp,mbuf,mlen) ;
	    bi = rs ;
	}

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    debugprintf("b_bibleverse/locinfo_bookmatch: ret rs=%d\n",rs) ;
#endif

	return (rs >= 0) ? bi : rs ;
} /* end subroutine (locinfo_bookmatch) */

local int locinfo_book(LI *lip) noex {
	PI	*pip = lip->pip ;
	int		rs = SR_OK ;
	if (! lip->open.ndb) {
	    BIBLEBOOK	*bbp = &lip->ndb ;
	    rs = biblebook_open(bbp,pip->pr,lip->ndbname) ;
	    lip->open.ndb = (rs >= 0) ;
	}
	return rs ;
} /* end subroutine (locinfo_book) */

local int locinfo_today(LI *lip) noex {
	PI	*pip = lip->pip ;
	cint	to = TO_MJD ;
	int		rs ;
	int		yr ;
	int		mjd = lip->mjd ;

	if ((rs = locinfo_tmtime(lip)) >= 0) {
	    if ((! lip->fl.mjd) || ((pip->daytime - lip->ti_mjd) >= to)) {
	        tmtime	*tmp = &lip->tm ;
	        lip->ti_mjd = pip->daytime ;
	        yr = (tmp->year + TMTIME_YEARBASE) ;
	        rs = getmjd(yr,tmp->mon,tmp->mday) ;
	        mjd = rs ;
	        lip->fl.mjd = true ;
	        lip->mjd = mjd ;
	    }
	} /* end if (locinfo-tmtime) */

	return (rs >= 0) ? mjd : rs ;
} /* end subroutine (locinfo_today) */

local int locinfo_defdayspec(LI *lip,DAYSPEC *dsp) noex {
	int		rs = SR_OK ;
	if ((rs >= 0) && (dsp->y <= 0)) {
	    rs = locinfo_year(lip) ;
	    dsp->y = lip->year ;
	}
	if ((rs >= 0) && (dsp->m < 0)) {
	    rs = locinfo_tmtime(lip) ;
	    dsp->m = lip->tm.mon ;
	}
	return rs ;
}
/* end subroutine (locinfo_defdayspec) */

local int locinfo_year(LI *lip) noex {
	int		rs = SR_OK ;

	if (lip->year == 0) {
	    if ((rs = locinfo_tmtime(lip)) >= 0) {
	        lip->year = (lip->tm.year + TMTIME_YEARBASE) ;
	    }
	}

	return rs ;
} /* end subroutine (locinfo_year) */

local int locinfo_tmtime(LI *lip) noex {
	PI	*pip = lip->pip ;
	int		rs = SR_OK ;
	int		tc = TIMECOUNT ;
	int		to = TO_TIME ;

	if ((! lip->fl.tmtime) || (lip->timecount++ >= tc)) {
	    if ((pip->daytime == 0) || (lip->timecount == tc)) {
	        pip->daytime = time(nullptr) ;
	    }
	    lip->timecount = 0 ;
	    if ((! lip->fl.tmtime) || ((pip->daytime - lip->ti_tmtime) >= to)) {
	        lip->ti_tmtime = pip->daytime ;
	        lip->fl.tmtime = true ;
	        if (lip->fl.gmt) {
	            rs = tmtime_timegm(&lip->tm,pip->daytime) ;
	        } else {
	            rs = tmtime_timelocal(&lip->tm,pip->daytime) ;
	        }
	    }
	} /* end if */

	return rs ;
} /* end subroutine (locinfo_tmtime) */

local int locinfo_mkmodquery(LI *lip,BIBLEVERSE_Q *qp,int mjd) noex {
	PI	*pip = lip->pip ;
	int		rs = SR_OK ;

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	    debugprintf("b_bibleverse/locinfo_mkmodquery: mjd=%u\n",mjd) ;
#endif

	if (pip == nullptr) return SR_FAULT ;
	if (! lip->open.sdb) {
	    rs = locinfo_bvs(lip) ;
	}

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	    debugprintf("b_bibleverse/locinfo_mkmodquery: "
	        "locinfo_bvs() rs=%d\n",rs) ;
#endif

	if (rs >= 0) {
	    BVS		*bsp = &lip->sdb ;
	    BVS_VERSE	bsq ;
	    rs = bvs_mkmodquery(bsp,&bsq,mjd) ;
	    qp->b = bsq.b ;
	    qp->c = bsq.c ;
	    qp->v = bsq.v ;
	}

#if	CF_DEBUG
	if (DEBUGLEVEL(3)) {
	    debugprintf("b_bibleverse/locinfo_mkmodquery: "
	        "ret rs=%d\n",rs) ;
	    debugprintf("b_bibleverse/locinfo_mkmodquery: b=%u",qp->b) ;
	    debugprintf("b_bibleverse/locinfo_mkmodquery: c=%u",qp->c) ;
	    debugprintf("b_bibleverse/locinfo_mkmodquery: v=%u",qp->v) ;
	}
#endif

	return rs ;
} /* end subroutine (locinfo_mkmodquery) */

local int locinfo_bvs(LI *lip) noex {
	PI	*pip = lip->pip ;
	BVS		*bsp = &lip->sdb ;
	int		rs = SR_OK ;

	if (! lip->open.sdb) {
	    rs = bvs_open(bsp,pip->pr,lip->sdbname) ;
	    lip->open.sdb = (rs >= 0) ;
	    if (rs == SR_NOENT) {
	        if ((rs = locinfo_bvsbuild(lip)) >= 0) {
	            rs = bvs_open(bsp,pip->pr,lip->sdbname) ;
	            lip->open.sdb = (rs >= 0) ;
	        }
	    }
	    if ((rs >= 0) && lip->fl.audit && lip->open.sdb) {
	         rs = bvs_audit(bsp) ;
	         if (pip->debuglevel > 0) {
	             shio_printf(pip->efp,"%s: bible-structure audit (%d)\n",
	                 pip->progname,rs) ;
	         }
	    } /* end if */
	} /* end if (initialization needed) */

#if	CF_DEBUG
	if (DEBUGLEVEL(5))
	    debugprintf("b_bibleverse/locinfo_bvs: ret rs=%d\n",rs) ;
#endif

	return rs ;
} /* end subroutine (locinfo_bvs) */

/* build the BVS database */
local int locinfo_bvsbuild(LI *lip) noex {
	PI	*pip = lip->pip ;
	int		rs = SR_OK ;

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	    debugprintf("b_bibleverse/locinfo_bvsbuild: ent\n") ;
#endif

	if (pip == nullptr) return SR_FAULT ;
	if (lip->open.vdb) {
	    rs = locinfo_bvsbuilder(lip) ;
	} else {
	    rs = SR_NOTOPEN ;
	}

	return rs ;
} /* end subroutine (locinfo_bvsbuild) */

local int locinfo_bvsbuilder(LI *lip) noex {
	PI	*pip = lip->pip ;
	BVSMK		*bmp = &lip->bsmk ;
	cmode	om = 0666 ;
	cint	of = 0 ;
	int		rs ;
	int		rs1 ;
	cchar	*pr = pip->pr ;
	cchar	*sdbname = lip->sdbname ;

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	    debugprintf("b_bibleverse/locinfo_bvsbuilder: ent\n") ;
#endif

	if ((rs = bvsmk_open(bmp,pr,sdbname,of,om)) >= 0) {
	    lip->open.bvsmk = true ;
	    if (rs == 0) {
	        BIBLEVERSE	*vdbp = &lip->vdb ;
	        BIBLEVERSE_INFO	binfo ;
	        if ((rs = bibleverse_info(vdbp,&binfo)) >= 0) {
	            cint	maxbook = binfo.maxbook ;
	            cint	maxchapter = binfo.maxchapter ;
	            int		bal ;
	            int		size ;
		    uchar	*bap = nullptr ;
	            bal = (maxchapter + 1) ;
	            size = (bal * sizeof(uchar)) ;
	            if ((rs = uc_malloc(size,&bap)) >= 0) {
		        int	b ;
		        int	nc ;

	                for (b = 0 ; (rs >= 0) && (b <= maxbook) ; b += 1) {

#if	CF_DEBUG
	                    if (DEBUGLEVEL(3))
	                        debugprintf("b_bibleverse/locinfo_bvsbuild: "
				    "b=%u\n", b) ;
#endif

	                    rs1 = bibleverse_chapters(vdbp,b,bap,bal) ;
	                    nc = rs1 ;

#if	CF_DEBUG
	                    if (DEBUGLEVEL(3))
	                        debugprintf("b_bibleverse/locinfo_bvsbuild: "
	                            "bibleverses_chapters() rs=%d\n",rs1) ;
#endif

	                    if (rs1 == SR_NOTFOUND) {
	                        nc = 0 ;
	                    } else
	                        rs = rs1 ;

	                    if (rs >= 0)
	                        rs = bvsmk_add(bmp,b,bap,nc) ;

#if	CF_DEBUG
	                    if (DEBUGLEVEL(3))
	                        debugprintf("b_bibleverse/locinfo_bvsbuild: "
	                            "bvsmk_add() rs=%d\n",rs) ;
#endif

	                } /* end for (looping through the books) */

	                uc_free(bap) ;
	                bap = nullptr ;
	            } /* end if (temporary memory allocation) */
	        } /* end if (bibleverse_info) */
	    } /* end if (majing) */
	    lip->open.bvsmk = false ;
	    rs1 = bvsmk_close(bmp) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (bvsmk) */

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	    debugprintf("b_bibleverse/locinfo_bvsbuild: ret rs=%d\n",rs) ;
#endif

	return rs ;
} /* end subroutine (locinfo_bvsbuilder) */

/* return maximum days possible for this citation */
#if	CF_LOCNDAYS
local int locinfo_ndays(LI *lip,BIBLEVERSE_Q *qp,int ndays) noex {
	PI	*pip = lip->pip ;
	int		rs = SR_OK ;
	int		rs1 ;

#if	CF_DEBUG
	if (DEBUGLEVEL(5))
	    debugprintf("b_bibleverse/locinfo_ndays: ndays=%u\n",ndays) ;
#endif

	if (ndays > 1) {
	    BIBLEVERSE_Q	q = *qp ;
	    const uint		vmax = UCHAR_MAX ;
	    int			c = 0 ;
	    while ((rs >= 0) && (ndays > 0)) {
	        rs1 = bibleverse_read(&lip->vdb,nullptr,0,&q) ;
		if (rs1 == SR_NOTFOUND) break ;
		rs = rs1 ;
		c += 1 ;
		ndays -= 1 ;
		if (q.v == vmax) break ;
		q.v += 1 ;
	    } /* end while */
	    ndays = c ;
	} /* end if (positive) */

	return (rs >= 0) ? ndays : rs ;
} /* end subroutine (locinfo_ndays) */
#endif /* CF_LOCNDAYS */

local int locinfo_ispara(LI *lip,BIBLEVERSE_Q *qp) noex {
	PI	*pip = lip->pip ;
	int		rs = SR_OK ;
	int		f = false ;

	if (qp == nullptr) return SR_FAULT ;

	if (lip->fl.para) {

#if	CF_DEBUG
	    if (DEBUGLEVEL(5))
	        debugprintf("b_bibleverse/locinfo_ispara: active\n") ;
#endif

	    if (! lip->open.pdb) {
	        cchar	*pr = pip->pr ;
	        cchar	*pdbname = lip->pdbname ;
	        if ((rs = biblepara_open(&lip->pdb,pr,pdbname)) >= 0) {
	            lip->open.pdb = true ;
	        } else if (isNotPresent(rs)) {
	            rs = SR_OK ;
		}
	    }

	    if ((rs >= 0) && lip->open.pdb) {
	        BIBLEPARA_QUERY	pq ;
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

	} /* end if (ok) */

	return (rs >= 0) ? f : rs ;
} /* end subroutine (locinfo_ispara) */

local bool isNotGoodCite(int rs) noex {
	bool		f = false ;
	f = f || (rs == SR_NOTFOUND) ;
	f = f || isNotValid(rs) ;
	return f ;
} /* end subroutine (isNotGoodCite) */



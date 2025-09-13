/* opensvc_votd SUPPORT */
/* lang=C++20 */

/* LOCAL facility open-service (votd) */
/* version %I% last-modified %G% */

#define	CF_TEST1	0		/* test-1 */
#define	CF_SUBNDAYS	0		/* use |subinfo_ndays()| */

/* revision history:

	= 2003-11-04, David A­D­ Morano
	This code was started by taking the corresponding code from
	the TCP-family module.  In retrospect, that was a mistake.
	Rather I should have started this code by using the
	corresponding UUX dialer module.

*/

/* Copyright © 2003 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	votd

	Description:
	This is a facility-open-service module.

	File-name:
	votd[­-sn][­-af][­-ndb][­-pdb][­-vdb][­-sdb][­-bookname][­-lang]
		[­o­<option(s)>]

	File-name options:
	audit		perform a recursive audit
	linelen=<v>	number of columns		
	indent=<v>	number of columns to indent
	bookname[=<b>]	use names rather than numbers (default)	
	interactive	run in interactive mode
	separate	separate entries with a blank line
	defnull		?
	default		perform a default operation
	qtype <s>	specify Q-type as string <s>
	atype <s>	specigy A-type as string <s>
	para		?
	gmt		use GMT time
	lang=<lang>	specify language <lang>
	allcache[=<b>]	dump entire cache (as there might be)


	Synopsis:
	int opensvc_votd(pr,prn,of,om,argv,envv,to)
	cchar		*pr ;
	cchar		*prn ;
	int		of ;
	mode_t		om ;
	cchar		**argv ;
	cchar		**envv ;
	int		to ;

	Arguments:
	pr		program-root
	prn		facility name
	of		open-flags
	om		open-mode
	argv		argument array
	envv		environment array
	to		time-out

	Returns:
	>=0		file-descriptor
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<climits>
#include	<unistd.h>
#include	<fcntl.h>
#include	<dlfcn.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<tzfile.h>		/* for TM_YEAR_BASE */
#include	<usystem.h>
#include	<estrings.h>
#include	<bits.h>
#include	<keyopt.h>
#include	<field.h>
#include	<vecstr.h>
#include	<wordfill.h>
#include	<tmtime.hh>
#include	<dayspec.h>
#include	<bcspec.h>
#include	<filer.h>
#include	<bfile.h>
#include	<biblebook.h>
#include	<biblepara.h>
#include	<bibleverse.h>
#include	<bvs.h>
#include	<bvsmk.h>
#include	<votdc.h>
#include	<ourmjd.h>
#include	<exitcodes.h>
#include	<localmisc.h>

#include	"opensvc_votd.h"
#include	"defs.h"


/* local defines */

#ifndef	LOGICVAL
#define	LOGICVAL(v)	((v)!=0)
#endif

#ifndef	NYEARS_CENTURY
#define	NYEARS_CENTURY	100
#endif

#ifndef	LINEBUFLEN
#define	LINEBUFLEN	MAX((MAXPATHLEN + 2),2048)
#endif

#ifndef	COMBUFLEN
#define	COMBUFLEN	1024		/* maximum bibleverse length (?) */
#endif

#ifndef	BVBUFLEN
#define	BVBUFLEN	512		/* maximum bibleverse length (?) */
#endif

#ifndef	SPECBUFLEN
#define	SPECBUFLEN	40
#endif

#ifndef	COLUMNS
#define	COLUMNS		80		/* output columns (should be 80) */
#endif

#define	BNBUFLEN	100
#define	COLBUFLEN	(COLUMNS + 10)

#define	NBLANKS		20

#define	NDAYS		256		/* maximum verses per chapter */

#define	TIMECOUNT	5

#ifndef	TO_MKWAIT
#define	TO_MKWAIT	(1 * 50)
#endif

#ifndef	TO_TMTIME
#define	TO_TMTIME	5		/* time-out for TMTIME */
#endif

#define	TO_MJD		5		/* time-out for MJD */

#define	MAXOUT		(PIPE_BUF-256)

#define	OUTCOOKIE	"\n"

#define	SUBINFO		struct subinfo
#define	SUBINFO_FL	struct subinfo_flags

#define	VCINFO		struct vcinfo

#define	VRBUF		struct vrbuf

#define	NDF		"bibleverse.nd"


/* external subroutines */

extern "C" {
    extern cchar	*getenv(cchar *) noex ;
}


/* external variables */

extern cchar	**environ ;


/* local structures */

struct vcinfo {
	bibleverse_q	q ;
	cchar		*a ;
	char		*nbuf ;
	char		*vbuf ;
	int		vlen ;
	int		nlen ;
} ;

struct subinfo_flags {
	uint		akopts:1 ;
	uint		audit:1 ;
	uint		linelen:1 ;
	uint		bookname:1 ;		/* option: want book-names */
	uint		para:1 ;		/* option: want paragraphs */
	uint		indent:1 ;		/* option: want indent */
	uint		separate:1 ;		/* option: want separation */
	uint		trycache:1 ;		/* option: try the cache */
	uint		lang:1 ;		/* option: language */
	uint		interactive:1 ;
	uint		ndb:1 ;			/* name (book) DB */
	uint		vdb:1 ;			/* verse DB */
	uint		sdb:1 ;			/* verse-structure DB */
	uint		pdb:1 ;			/* paragraph DB */
	uint		vcache:1 ;		/* V-cache */
	uint		tmtime:1 ;
	uint		bvsmk:1 ;
	uint		mjd:1 ;
	uint		defnull:1 ;
	uint		nitems:1 ;
	uint		gmt:1 ;
	uint		all:1 ;
	uint		allcache:1 ;
} ;

struct subinfo {
	TMTIME		tm ;		/* holds today's date, when set */
	BIBLEBOOK	ndb ;		/* bible-book-name DB */
	bibleverse	vdb ;
	BVS		sdb ;
	BVSMK		bsmk ;
	BIBLEPARA	pdb ;
	VOTDC		vcache ;
	VCINFO		vinfo ;
	void		*ofp ;
	cchar		**envv ;
	cchar		*pr ;
	cchar		*sn ;
	cchar		*lang ;
	cchar		*ndbname ;	/* name-db name */
	cchar		*pdbname ;	/* paragraph-db name */
	cchar		*vdbname ;	/* verse-db name */
	cchar		*sdbname ;	/* structure-db name */
	SUBINFO_FL	have, f, changed, final ;
	SUBINFO_FL	open ;
	time_t		dt ;
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
	int		ncache ;
	int		maxout ;
	int		wfd, fd ;
} ;

struct vrbuf {
	int		vl ;
	char		vbuf[BVBUFLEN+1] ;
} ;


/* forward references */

static int	subinfo_start(SUBINFO *,cchar **) noex ;
static int	subinfo_setroot(SUBINFO *,cchar *,cchar *) noex ;
static int	subinfo_outbegin(SUBINFO *) noex ;
static int	subinfo_outend(SUBINFO *,int) noex ;
static int	subinfo_book(SUBINFO *) noex ;
static int	subinfo_booksize(SUBINFO *) noex ;
static int	subinfo_booklookup(SUBINFO *,char *,int,int) noex ;
static int	subinfo_bookmatch(SUBINFO *,cchar *,int) noex ;
static int	subinfo_today(SUBINFO *) noex ;
static int	subinfo_defdayspec(SUBINFO *,dayspec *) noex ;
static int	subinfo_year(SUBINFO *) noex ;
static int	subinfo_tmtime(SUBINFO *) noex ;
static int	subinfo_finish(SUBINFO *) noex ;
static int	subinfo_mkmodquery(SUBINFO *,bibleverse_q *,int) noex ;
static int	subinfo_bvs(SUBINFO *) noex ;
static int	subinfo_bvsbuild(SUBINFO *) noex ;
static int	subinfo_bvsbuilder(SUBINFO *) noex ;
static int	subinfo_ispara(SUBINFO *,bibleverse_q *) noex ;
static int	subinfo_text(SUBINFO *) noex ;
static int	subinfo_textquery(SUBINFO *,char *,int,bibleverse_q *) noex ;
static int	subinfo_vc(SUBINFO *) noex ;
static int	subinfo_vcbegin(SUBINFO *) noex ;
static int	subinfo_vcfetch(SUBINFO *,VCINFO *,int) noex ;
static int	subinfo_vcmjd(SUBINFO *,VCINFO *,int) noex ;
static int	subinfo_vcload(SUBINFO *,VCINFO *,cchar *,int,int) noex ;
static int	subinfo_vcbookname(SUBINFO *,VCINFO *,cchar *) noex ;
static int	subinfo_vcend(SUBINFO *) noex ;
static int	subinfo_titlelang(SUBINFO *) noex ;

#if	CF_SUBNDATS
static int	subinfo_ndays(SUBINFO *,bibleverse_q *,int) noex ;
#endif

static int	procopts(SUBINFO *,keyopt *) noex ;
static int	process(SUBINFO *,ARGINFO *,BITS *,cchar *,int) noex ;
static int	procsome(SUBINFO *,ARGINFO *,BITS *,cchar *,int) noex ;
static int	procspecs(SUBINFO *,cchar *,int) noex ;
static int	procspec(SUBINFO *,cchar *,int) noex ;
static int	procallcache(SUBINFO *) noex ;
static int	procallcacheout(SUBINFO *,VOTDC *,VOTDC_CITE *,
			char *,int) noex ;
static int	procallcacheoutcite(SUBINFO *,VOTDC *,VOTDC_CITE *) noex ;
static int	procall(SUBINFO *) noex ;
static int	procparse(SUBINFO *,bibleverse_q *,cchar *,int) noex ;
static int	procmulti(SUBINFO *,bibleverse_q *,int,int) noex ;
static int	procload(SUBINFO *,VRBUF *,int,bibleverse_q *) noex ;
static int	proctoday(SUBINFO *,int,int) noex ;
static int	procmjds(SUBINFO *,int,int,int) noex ;
static int	procvcache(SUBINFO *,int,int) noex ;
static int	procvcacher(SUBINFO *,VCINFO *,int) noex ;
static int	procvoutcite(SUBINFO *,VCINFO *,int) noex ;
static int	procvoutverse(SUBINFO *,VCINFO *) noex ;
static int	procoutcite(SUBINFO *,bibleverse_q *,int) noex ;
static int	procoutverse(SUBINFO *,bibleverse_q *,cchar *,int) noex ;
static int	procoutline(SUBINFO *,int,cchar *,int) noex ;

static int	vcinfo_start(VCINFO *) noex ;
static int	vcinfo_finish(VCINFO *) noex ;

static int	votdsq_load(VOTDC_Q *,bibleverse_q *) noex ;


/* local variables */

enum argopts {
	argopt_root,
	argopt_help,
	argopt_sn,
	argopt_af,
	argopt_book,
	argopt_pdb,
	argopt_vdb,
	argopt_sdb,
	argopt_bookname,
	argopt_lang,
	argopt_overlast
} ;

static constexpr cchar	*argopts[] = {
	"ROOT",
	"HELP",
	"sn",
	"af",
	"ndb",
	"pdb",
	"vdb",
	"sdb",
	"bookname",
	"lang",
	nullptr
} ;

enum akonames {
	akoname_audit,
	akoname_linelen,
	akoname_indent,
	akoname_bookname,
	akoname_interactive,
	akoname_separate,
	akoname_defnull,
	akoname_default,
	akoname_qtype,
	akoname_atype,
	akoname_para,
	akoname_gmt,
	akoname_lang,
	akoname_allcache,
	akoname_overlast
} ;

static constexpr cchar	*akonames[] = {
	"audit",
	"linelen",
	"indent",
	"bookname",
	"interactive",
	"separate",
	"defnull",
	"default",
	"qtype",
	"atype",
	"para",
	"gmt",
	"lang",
	"allcache",
	nullptr
} ;

static constexpr cchar	aterms[] = {
	0x00, 0x2E, 0x00, 0x00,
	0x09, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
} ;

enum qtypes {
	qtype_verse,
	qtype_day,
	qtype_mjd,
	qtype_overlast
} ;

static constexpr cchar	*qtypes[] = {
	"verses",
	"days",
	"mjds",
	nullptr
} ;

static constexpr cchar	*langs[] = {
	"english",
	"spanish",
	nullptr
} ;

static constexpr cchar	*vers[] = {
	"av",
	"rvv",
	nullptr
} ;

static constexpr cchar	blanks[NBLANKS+1] = "                    " ;


/* exported variables */


/* exported subroutines */

/* ARGSUSED */
int opensvc_votd(pr,prn,of,om,argv,envv,to)
cchar		*pr ;
cchar		*prn ;
int		of ;
mode_t		om ;
cchar		**argv ;
cchar		**envv ;
int		to ;
{
	SUBINFO		si, *sip = &si ;
	ARGINFO		ainfo ;
	BITS		pargs ;
	keyopt		akopts ;
	int		argr, argl, aol, akl, avl, kwi ;
	int		ai, ai_max, ai_pos ;
	int		argc = 0 ;
	int		rs, rs1 ;
	int		n ;
	int		v ;
	int		fd = -1 ;
	int		f_optminus, f_optplus, f_optequal ;
	int		f_apm = false ;
	cchar		*argp, *aop, *akp, *avp ;
	cchar		*argval = nullptr ;
	cchar		*sn = nullptr ;
	cchar		*afname = nullptr ;
	cchar		*ndbname = nullptr ;
	cchar		*pdbname = nullptr ;
	cchar		*vdbname = nullptr ;
	cchar		*sdbname = nullptr ;
	cchar		*qtypestr = nullptr ;
	cchar		*lang = nullptr ;
	cchar		*cp ;

	if (argv != nullptr) {
	    for (argc = 0 ; argv[argc] != nullptr ; argc += 1) ;
	}

/* local information */

	rs = subinfo_start(sip,envv) ;
	if (rs < 0) goto badsubstart ;

/* start parsing the arguments */

	if (rs >= 0) rs = bits_start(&pargs,1) ;
	if (rs < 0) goto badpargs ;

	rs = keyopt_start(&akopts) ;
	sip->open.akopts = (rs >= 0) ;

	ai_max = 0 ;
	ai_pos = 0 ;
	argr = argc ;
	for (int ai = 0 ; (ai < argc) && (argv[ai] != nullptr) ; ai += 1) {
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

	            if (f_optplus) f_apm = true ;
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
	                            argl = strlen(argp) ;
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
	                            argl = strlen(argp) ;
	                            if (argl)
	                                sdbname = argp ;
	                        } else
	                            rs = SR_INVALID ;
	                    }
	                    break ;

	                case argopt_bookname:
	                    sip->have.bookname = true ;
	                    sip->final.bookname = true ;
	                    sip->fl.bookname = true ;
	                    if (f_optequal) {
	                        f_optequal = false ;
	                        if (avl) {
	                            rs = optbool(avp,avl) ;
	                            sip->fl.bookname = (rs > 0) ;
	                        }
	                    }
	                    break ;

	                case argopt_lang:
	                    sip->have.lang = true ;
	                    sip->final.bookname = true ;
	                    if (argr > 0) {
	                        argp = argv[++ai] ;
	                        argr -= 1 ;
	                        argl = strlen(argp) ;
	                        if (argl) {
	                            lang = argp ;
	                        }
	                    } else
	                        rs = SR_INVALID ;
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

	                    case 'a':
	                        sip->fl.all = true ;
	                        break ;

/* type of argument-input */
	                    case 'i':
	                    case 't':
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
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
	                            argl = strlen(argp) ;
	                            if (argl) {
	                                sip->have.nitems = true ;
	                                sip->final.nitems = true ;
	                                rs = optvalue(argp,argl) ;
	                                sip->nitems = rs ;
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

/* line-width (columns) */
	                    case 'w':
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl) {
	                                sip->have.linelen = true ;
	                                sip->final.linelen = true ;
	                                rs = optvalue(argp,argl) ;
	                                sip->linelen = rs ;
	                            }
	                        } else
	                            rs = SR_INVALID ;
	                        break ;

/* default year */
	                    case 'y':
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl) {
	                                rs = optvalue(argp,argl) ;
	                                sip->year = rs ;
	                            }
	                        } else
	                            rs = SR_INVALID ;
	                        break ;

/* use GMT */
	                    case 'z':
	                        sip->final.gmt = true ;
	                        sip->fl.gmt = true ;
	                        if (f_optequal) {
	                            f_optequal = false ;
	                            if (avl) {
	                                rs = optbool(avp,avl) ;
	                                sip->fl.gmt = (rs > 0) ;
	                            }
	                        }
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

	if (rs >= 0) {
	    rs = subinfo_setroot(sip,pr,sn) ;
	}

	if ((rs >= 0) && (sip->nitems <= 0) && (argval != nullptr)) {
	    rs = optvalue(argval,-1) ;
	    sip->nitems = rs ;
	    sip->have.nitems = true ;
	    sip->final.nitems = true ;
	}

/* load up the environment options */

	if (rs >= 0) {
	    rs = procopts(sip,&akopts) ;
	}

/* argument defaults */

	if (afname == nullptr) afname = getourenv(sip->envv,VARAFNAME) ;

	if (lang == nullptr) lang = getourenv(sip->envv,VARLANG) ;
	if (lang == nullptr) lang = DEFLANG ;

/* check for valid language */

	if ((lang != nullptr) && (lang[0] != '\0')) {
	    int		li = matostr(langs,1,lang,-1) ;
	    ndbname = lang ;
	    if (li < 0) li = 0 ;
	    vdbname = vers[li] ;
	} /* end if (language was specified) */

/* name-db name */

	if (ndbname == nullptr) ndbname = getourenv(envv,VARNDB) ;
	if (ndbname == nullptr) ndbname = NDBNAME ;

/* verse-db name */

	if (vdbname == nullptr) vdbname = getourenv(envv,VARVDB) ;
	if (vdbname == nullptr) vdbname = VDBNAME ;

/* structure-db name */

	if (sdbname == nullptr) sdbname = getourenv(envv,VARSDB) ;
	if (sdbname == nullptr) sdbname = SDBNAME ;

/* paragraph-db name */

	if (pdbname == nullptr) pdbname = getourenv(envv,VARPDB) ;
	if (pdbname == nullptr) pdbname = PDBNAME ;

/* defaults */

	sip->lang = lang ;
	sip->ndbname = ndbname ;
	sip->vdbname = vdbname ;
	sip->sdbname = sdbname ;
	sip->pdbname = pdbname ;

/* type of argument-input */

	if ((rs >= 0) && (qtypestr != nullptr)) {
	    v = matostr(qtypes,1,qtypestr,-1) ;
	    if (v < 0) rs = SR_INVALID ;
	    sip->qtype = v ;
	}

/* debugging */

	rs1 = (DEFPRECISION + 2) ;
	if ((rs >= 0) && (sip->linelen < rs1)) {
	    cp = getourenv(envv,VARLINELEN) ;
	    if (cp == nullptr) cp = getourenv(envv,VARCOLUMNS) ;
	    if (cp != nullptr) {
	        if ((rs = optvalue(cp,-1)) >= 0) {
		    if (v >= rs1) {
	                sip->have.linelen = true ;
	                sip->final.linelen = true ;
	                sip->linelen = rs ;
		    }
	        }
	    }
	}

	if (sip->linelen < rs1)
	    sip->linelen = COLUMNS ;

	if ((sip->nitems < 1) && (! sip->have.nitems)) {
	    sip->nitems = 1 ;
	}

/* process */

	memset(&ainfo,0,sizeof(ARGINFO)) ;
	ainfo.argc = argc ;
	ainfo.ai = ai ;
	ainfo.argv = argv ;
	ainfo.ai_max = ai_max ;
	ainfo.ai_pos = ai_pos ;

	if (rs >= 0) {
	    if ((rs = subinfo_outbegin(sip)) >= 0) {
	        {
	            cchar	*af = afname ;
	            rs = process(sip,&ainfo,&pargs,af,f_apm) ;
	        }
	        fd = subinfo_outend(sip,rs) ;
	        if (rs >= 0) rs = fd ;
	    } /* end if (subinfo_outend) */
	} /* end if (ok) */

/* done */
	if (sip->open.akopts) {
	    sip->open.akopts = false ;
	    keyopt_finish(&akopts) ;
	}

	bits_finish(&pargs) ;

badpargs:
	subinfo_finish(sip) ;

	if ((rs < 0) && (fd >= 0)) u_close(fd) ;

badsubstart:
	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (opensvc_votd) */


/* local subroutines */

/* process the program ako-names */
static int procopts(SUBINFO *sip,keyopt *kop) noex {
	int		rs = SR_OK ;
	int		c = 0 ;
	cchar		*cp ;

	if ((cp = getourenv(sip->envv,VAROPTS)) != nullptr) {
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
	                case akoname_audit:
	                    if (! sip->final.audit) {
	                        sip->have.audit = true ;
	                        sip->final.audit = true ;
	                        sip->fl.audit = true ;
	                        if (vl > 0) {
	                            rs = optbool(vp,vl) ;
	                            sip->fl.audit = (rs > 0) ;
	                        }
	                    }
	                    break ;
	                case akoname_linelen:
	                    if (! sip->final.linelen) {
	                        sip->have.linelen = true ;
	                        sip->final.linelen = true ;
	                        sip->fl.linelen = true ;
	                        if (vl > 0) {
	                            rs = optvalue(vp,vl) ;
	                            sip->linelen = rs ;
	                        }
	                    }
	                    break ;
	                case akoname_indent:
	                    if (! sip->final.indent) {
	                        sip->have.indent = true ;
	                        sip->final.indent = true ;
	                        sip->fl.indent = true ;
	                        sip->indent = 1 ;
	                        if (vl > 0) {
	                            rs = optvalue(vp,vl) ;
	                            sip->indent = rs ;
	                        }
	                    }
	                    break ;
	                case akoname_bookname:
	                    if (! sip->final.bookname) {
	                        sip->have.bookname = true ;
	                        sip->final.bookname = true ;
	                        sip->fl.bookname = true ;
	                        if (vl > 0) {
	                            rs = optbool(vp,vl) ;
	                            sip->fl.bookname = (rs > 0) ;
	                        }
	                    }
	                    break ;
	                case akoname_interactive:
	                    if (! sip->final.interactive) {
	                        sip->have.interactive = true ;
	                        sip->final.interactive = true ;
	                        sip->fl.interactive = true ;
	                        if (vl > 0) {
	                            rs = optbool(vp,vl) ;
	                            sip->fl.interactive = (rs > 0) ;
	                        }
	                    }
	                    break ;
	                case akoname_separate:
	                    if (! sip->final.separate) {
	                        sip->have.separate = true ;
	                        sip->final.separate = true ;
	                        sip->fl.separate = true ;
	                        if (vl > 0) {
	                            rs = optbool(vp,vl) ;
	                            sip->fl.separate = (rs > 0) ;
	                        }
	                    }
	                    break ;
	                case akoname_default:
	                case akoname_defnull:
	                    if (! sip->final.defnull) {
	                        sip->have.defnull = true ;
	                        sip->final.defnull = true ;
	                        sip->fl.defnull = true ;
	                        if (vl > 0) {
	                            rs = optbool(vp,vl) ;
	                            sip->fl.defnull = (rs > 0) ;
	                        }
	                    }
	                    break ;
	                case akoname_para:
	                    if (! sip->final.para) {
	                        sip->have.para = true ;
	                        sip->final.para = true ;
	                        sip->fl.para = true ;
	                        if (vl > 0) {
	                            rs = optbool(vp,vl) ;
	                            sip->fl.para = (rs > 0) ;
	                        }
	                    }
	                    break ;
	                case akoname_gmt:
	                    if (! sip->final.gmt) {
	                        sip->have.gmt = true ;
	                        sip->final.gmt = true ;
	                        sip->fl.gmt = true ;
	                        if (vl > 0) {
	                            rs = optbool(vp,vl) ;
	                            sip->fl.gmt = (rs > 0) ;
	                        }
	                    }
	                    break ;
	                case akoname_allcache:
	                    if (! sip->final.allcache) {
	                        sip->have.allcache = true ;
	                        sip->final.allcache = true ;
	                        sip->fl.allcache = true ;
	                        if (vl > 0) {
	                            rs = optbool(vp,vl) ;
	                            sip->fl.allcache = (rs > 0) ;
	                        }
	                    }
	                    break ;
	                case akoname_atype:
	                case akoname_qtype:
	                    if (vl) {
	                        rs = matostr(qtypes,1,vp,vl) ;
	                        sip->qtype = rs ;
	                    }
	                    break ;
	                } /* end switch */

	                c += 1 ;
	            } /* end if (valid option) */

	            if (rs < 0) break ;
	        } /* end while (looping through key options) */

	        keyopt_curend(kop,&kcur) ;
	    } /* end if (keyopt-cur) */
	} /* end if (ok) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (procopts) */

static int process(SUBINFO *sip,ARGINFO *aip,BITS *bop,cchar *afn,
		int f_apm) noex {
	cint		fd = sip->wfd ;
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	if (filer b ; (rs = filer_start(&b,fd,0z,512,0)) >= 0) {
	    sip->ofp = &b ;

	    if (sip->fl.allcache) {
	        rs = procallcache(sip) ;
	        wlen += rs ;
	    } else if (sip->fl.all) {
	        rs = procall(sip) ;
	        wlen += rs ;
	    } else {
	        rs = procsome(sip,aip,bop,afn,f_apm) ;
	        wlen += rs ;
	    }

	    sip->ofp = nullptr ;
	    rs1 = filer_finish(&b) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (filer) */

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (process) */

static int procsome(SUBINFO *sip,ARGINFO *aip,BITS *bop,cchar *afn,
		int f_apm) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		pan = 0 ;
	int		cl ;
	int		wlen = 0 ;
	cchar		*cp ;

	if (rs >= 0) {
	    for (int ai = 1 ; ai < aip->argc ; ai += 1) {

	        bool	f = (ai <= aip->ai_max) && (bits_test(bop,ai) > 0) ;
	        f = f || ((ai > aip->ai_pos) && (aip->argv[ai] != nullptr)) ;
	        if (f) {
	            cp = aip->argv[ai] ;
	            if (cp[0] != '\0') {
	                pan += 1 ;
	                rs = procspec(sip,cp,-1) ;
	                wlen += rs ;
	            }
	        }

	        if (wlen >= MAXOUT) break ;
	        if (rs < 0) break ;
	    } /* end for (looping through positional arguments) */
	} /* end if (positional arguments) */

	if (rs >= 0) {
	    if ((afn != nullptr) && (afn[0] != '\0')) {
	        bfile	afile, *afp = &afile ;

	        if ((rs = bopen(afp,afn,"r",0666)) >= 0) {
	            cint	llen = LINEBUFLEN ;
	            int		len ;
	            char	lbuf[LINEBUFLEN + 1] ;

	            while ((rs = breadln(afp,lbuf,llen)) > 0) {
	                len = rs ;

	                if (lbuf[len - 1] == '\n') len -= 1 ;
	                lbuf[len] = '\0' ;

	                if ((cl = sfskipwhite(lbuf,len,&cp)) > 0) {
	                    if (cp[0] != '#') {
	                        pan += 1 ;
	                        rs = procspecs(sip,cp,cl) ;
	                        wlen += rs ;
	                    }
	                }

	                if (wlen >= MAXOUT) break ;
	                if (rs < 0) break ;
	            } /* end while */

	            rs1 = bclose(afp) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (bfile) */

	    } /* end if (afile arguments) */
	} /* end if (ok) */

	if ((rs >= 0) && f_apm) {
	    int		ndays = 1 ;

	    pan += 1 ;
	    if (sip->nitems > 0) ndays = (sip->nitems+1) ;
	    rs = proctoday(sip,true,ndays) ;
	    wlen += rs ;

	} /* end if */

	if ((rs >= 0) && (pan == 0) && sip->fl.defnull) {
	    int		ndays = 1 ;

	    if (sip->nitems > 1) ndays = sip->nitems ;
	    rs = proctoday(sip,true,ndays) ;
	    wlen += rs ;

	} /* end if */

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (procsome) */

static int procspecs(SUBINFO *sip,cchar *sp,int sl) noex {
	FIELD		fsb ;
	int		rs ;
	int		wlen = 0 ;

	if (sp == nullptr) return SR_FAULT ;

	if (sip->fl.interactive) sip->cout = 0 ;

	if (sl < 0) sl = strlen(sp) ;

	if ((rs = field_start(&fsb,sp,sl)) >= 0) {
	    cint	flen = sl ;
	    char	*fbuf ;
	    if ((rs = uc_malloc((flen+1),&fbuf)) >= 0) {
	        int	fl ;
	        while ((fl = field_sharg(&fsb,aterms,fbuf,flen)) >= 0) {
	            if (fl > 0) {
	                rs = procspec(sip,fbuf,fl) ;
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
}
/* end subroutine (procspecs) */

static int procspec(SUBINFO *sip,cchar sp[],int sl) noex {
	int		rs = SR_OK ;
	int		ndays = sip->nitems ;
	int		wlen = 0 ;

	if (sp == nullptr) return SR_FAULT ;

	if (sl < 0) sl = strlen(sp) ;

	if ((sl > 0) && (sp[0] != '\0')) {

	    if ((sp[0] == '+') || (sp[0] == '-')) {
	        int		v = sip->nitems ;

	        if (sl > 1) { /* cannot now happen!? */
	            sl -= 1 ;
	            sp += 1 ;
	            rs = cfdeci(sp,sl,&v) ;
	        }

	        if (rs >= 0) {
	            rs = proctoday(sip,true,v) ;
	            wlen += rs ;
	        }

	    } else if (sip->qtype == qtype_verse) {
	        bibleverse_q	q ;

	        if ((rs = procparse(sip,&q,sp,sl)) >= 0) {
	            rs = procmulti(sip,&q,true,ndays) ;
	            wlen += rs ;
	        }

	    } else {
	        int	mjd = -1 ;
	        if (sip->qtype == qtype_day) {
	            if ((rs = ourmjd(sp,sl)) > 0) {
	                mjd = rs ;
	            } else {
	                dayspec	ds ;
	                if ((rs = dayspec_load(&ds,sp,sl)) >= 0) {
	                    if ((rs = subinfo_defdayspec(sip,&ds)) >= 0) {
	                        rs = getmjd(ds.y,ds.m,ds.d) ;
	                        mjd = rs ;
	                    }
	                } /* end if (dayspec) */
	            } /* end if (type-day) */
	        } else if (sip->qtype == qtype_mjd) {
	            if ((rs = ourmjd(sp,sl)) > 0) {
	                mjd = rs ;
	            } else {
	                uint	uv ;
	                rs = cfdecui(sp,sl,&uv) ;
	                mjd = (int) uv ;
	            }
	        } else {
	            rs = SR_INVALID ;
	        }

	        if ((rs >= 0) && (mjd >= 0)) {
	            rs = procmjds(sip,mjd,true,ndays) ;
	            wlen += rs ;
	        }

	    } /* end if (handling different query types) */

	} /* end if (positive) */

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (procspec) */

static int procallcache(SUBINFO *sip) noex {
	VOTDC		*vcp = &sip->vcache ;
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	cchar		*pr = sip->pr ;
#if	CF_TEST1
	rs = filer_printf(sip->ofp,"hello world\n") ;
	wlen += rs ;
#else /* CF_TEST1 */
	if ((rs = votdc_open(vcp,pr,nullptr,0)) >= 0) {
	    VOTDC_VCUR	vc ;
	    sip->open.vcache = true ;
	    if ((rs = votdc_vcurbegin(vcp,&vc)) >= 0) {
	        VOTDC_CITE	vcite ;
	        cint	vlen = BVBUFLEN ;
	        int		vl ;
	        char		vbuf[BVBUFLEN+1] ;
	        while ((vl = votdc_vcurenum(vcp,&vc,&vcite,vbuf,vlen)) > 0) {
	            rs = procallcacheout(sip,vcp,&vcite,vbuf,vl) ;
	            wlen += rs ;
	        } /* end while */
	        if ((rs >= 0) && (vl != SR_NOTFOUND)) rs = vl ;
	        rs1 = votdc_vcurend(&sip->vcache,&vc) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (votdc-cur) */
	    sip->open.vcache = false ;
	    rs1 = votdc_close(&sip->vcache) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (votdc) */
#endif /* CF_TEST1 */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (procallcache) */

static int procallcacheout(SUBINFO *sip,VOTDC *vcp,VOTDC_CITE *citep,
		char *vbuf,int vl) noex {
	int		rs ;
	int		wlen = 0 ;

	if ((rs = procallcacheoutcite(sip,vcp,citep)) >= 0) {
	    bibleverse_q	q ;
	    wlen += rs ;
	    q.b = citep->b ;
	    q.c = citep->c ;
	    q.v = citep->v ;
	    rs = procoutverse(sip,&q,vbuf,vl) ;
	    wlen += rs ;
	}

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (procallcacheout) */

static int procallcacheoutcite(SUBINFO *sip,VOTDC *vcp,VOTDC_CITE *citep) noex {
	int		rs = SR_OK ;
	int		wlen = 0 ;
	cchar		*fmt ;
	if (sip->fl.separate && (sip->cout++ > 0)) {
	    fmt = OUTCOOKIE ;
	    rs = filer_printf(sip->ofp,fmt) ;
	    wlen += rs ;
	} /* end if (separator) */
	if (rs >= 0) {
	    cint	b = citep->b ;
	    cint	c = citep->c ;
	    cint	v = citep->v ;
	    cint	l = citep->l ;
	    if (sip->fl.bookname) {
	        cint	rsn = SR_NOTFOUND ;
	        cint	nlen = BNBUFLEN ;
	        char		nbuf[BNBUFLEN+1] ;
	        if ((rs = votdc_titleget(vcp,nbuf,nlen,l,b)) == rsn) {
	            rs = subinfo_booklookup(sip,nbuf,nlen,b) ;
	        }
	        if (rs >= 0) {
	            fmt = "%r %u:%u\n" ;
	            rs = filer_printf(sip->ofp,fmt,nbuf,rs,c,v) ;
	            wlen += rs ;
	        }
	    } else {
	        fmt = "%u:%u:%u\n" ;
	        rs = filer_printf(sip->ofp,fmt,b,c,v) ;
	        wlen += rs ;
	    }
	} /* end if (ok) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (procallcacheoutcite) */

static int procall(SUBINFO *sip) noex {
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;

	if ((rs = subinfo_text(sip)) >= 0) {
	    bibleverse		*vdbp = &sip->vdb ;
	    bibleverse_cur	cur ;
	    bibleverse_q	q ;	/* result */
	    if ((rs = bibleverse_curbegin(vdbp,&cur)) >= 0) {
	        cint	bvlen = BVBUFLEN ;
	        int		bvl ;
	        int		c = 0 ;
	        char		bvbuf[BVBUFLEN + 1] ;

	        while (rs >= 0) {
	            bvl = bibleverse_curenum(vdbp,&cur,&q,bvbuf,bvlen) ;
	            if ((bvl == SR_NOTFOUND) || (bvl == 0)) break ;
	            rs = bvl ;
	            if (rs >= 0) {
	                rs = procoutverse(sip,&q,bvbuf,bvl) ;
	                wlen += rs ;
	            }

	            if ((sip->nitems > 0) && (++c >= sip->nitems)) break ;
	        } /* end while */

	        rs1 = bibleverse_curend(vdbp,&cur) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (printing all book titles) */
	} /* end if (subinfo_text) */

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (procall) */

static int procmulti(SUBINFO *sip,bibleverse_q *qp,int f_cite,int ndays) noex {
	VRBUF		*vrp ;
	int		rs ;
	int		rs1 ;
	int		sz ;
	int		wlen = 0 ;

	if (ndays < 1) {
	    ndays = 1 ;
	} else if (ndays > NDAYS) {
	    ndays = NDAYS ;
	}

	sz = (ndays * sizeof(VRBUF)) ;
	if ((rs = uc_malloc(sz,&vrp)) >= 0) {
	    if ((rs = procload(sip,vrp,ndays,qp)) > 0) {
	        cint	nv = rs ;

	        sip->ncites += nv ;

	        if (f_cite) {
	            rs = procoutcite(sip,qp,nv) ;
	            wlen += rs ;
	        }

	        if (rs >= 0) {
	            rs = procoutverse(sip,qp,vrp->vbuf,vrp->vl) ;
	            wlen += rs ;
	            qp->v += 1 ;
	        }

	        for (int i = 1 ; (rs >= 0) && (i < nv) ; i += 1) {
	            cint	vl = vrp[i].vl ;
	            char	*vbuf = vrp[i].vbuf ;
	            rs = procoutverse(sip,qp,vbuf,vl) ;
	            wlen += rs ;
	            qp->v += 1 ;
	        } /* end for */

	    } /* end if (procload) */
	    rs1 = uc_free(vrp) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a) */

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (procmulti) */

static int procload(SUBINFO *sip,VRBUF *vrp,int nbuf,bibleverse_q *qp) noex {
	int		rs ;
	int		nb = 0 ;
	if ((rs = subinfo_text(sip)) >= 0) {
	    bibleverse		*dbp = &sip->vdb ;
	    bibleverse_q	q = *qp ;
	    cint		bvlen = BVBUFLEN ;
	    int			vl ;
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
	} /* end if (subinfo_text) */
	return (rs >= 0) ? nb : rs ;
}
/* end subroutine (procload) */

static int procparse(SUBINFO *sip,bibleverse_q *qp,cchar sp[],int sl) noex {
	BCSPEC		bb ;
	int		rs ;

	if ((rs = bcspec_load(&bb,sp,sl)) >= 0) {
	    cchar	*np = bb.np ;
	    cint	nl = bb.nl ;
	    qp->b = bb.b ;
	    qp->c = bb.c ;
	    qp->v = bb.v ;
	    if (np != nullptr) {
	        rs = subinfo_bookmatch(sip,np,nl) ;
	        qp->b = (uchar) rs ;
	    }
	} /* end if (bcspec_load) */

	return rs ;
}
/* end subroutine (procparse) */

static int proctoday(SUBINFO *sip,int f_cite,int ndays) noex {
	int		rs ;
	int		wlen = 0 ;

	if ((rs = subinfo_today(sip)) >= 0) {
	    cint	f_c = sip->fl.trycache ;
	    int		mjd = rs ;
	    int		f_got = false ;
	    if (f_c) {
	        if ((rs = procvcache(sip,mjd,ndays)) > 0) {
	            wlen += rs ;
	            f_got = true ;
	            if (ndays > 0) ndays -= 1 ;
	        }
	    }
	    if ((rs >= 0) && ((! f_got) || (ndays > 0))) {
	        if (f_got) {
	            f_cite = false ;
	            mjd += 1 ;
	        }
	        rs = procmjds(sip,mjd,f_cite,ndays) ;
	        wlen += rs ;
	    }
	} /* end if (subinfo_today) */

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (proctoday) */

static int procmjds(SUBINFO *sip,int mjd,int f_cite,int ndays) noex {
	bibleverse_q	q ;
	int		rs ;
	int		wlen = 0 ;

	if ((rs = subinfo_mkmodquery(sip,&q,mjd)) >= 0) {
	    rs = procmulti(sip,&q,f_cite,ndays) ;
	    wlen += rs ;
	}

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (procmjds) */

static int procvcache(SUBINFO *sip,int mjd,int ndays) noex {
	VCINFO		vci ;
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	if ((rs = vcinfo_start(&vci)) >= 0) {
	    if ((rs = procvcacher(sip,&vci,mjd)) >= 0) {
	        wlen = rs ;
	        if ((rs = procvoutcite(sip,&vci,ndays)) >= 0) {
	            rs = procvoutverse(sip,&vci) ;
	            wlen += rs ;
	        }
	    } /* end if (procvcacher) */
	    rs1 = vcinfo_finish(&vci) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (vcinfo) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (procvcache) */

static int procvcacher(SUBINFO *sip,VCINFO *vip,int mjd) noex {
	int		rs ;
	int		vl = 0 ;
	if ((rs = subinfo_vc(sip)) >= 0) {
	    cint	rsn = SR_NOTFOUND ;
	    if ((rs = subinfo_vcfetch(sip,vip,mjd)) >= 0) {
	        vl = rs ;
	    } else if (rs == rsn) {
	        if ((rs = subinfo_vcmjd(sip,vip,mjd)) >= 0) {
	            cchar	*lang = sip->lang ;
	            vl = rs ;
	            rs = subinfo_vcload(sip,vip,lang,mjd,vl) ;
	        }
	    }
	    if (rs >= 0) {
	        cchar	*lang = sip->lang ;
	        rs = subinfo_vcbookname(sip,vip,lang) ;
	    }
	} /* end if (subinfo_vc) */
	return (rs >= 0) ? vl : rs ;
}
/* end subroutine (procvcacher) */

static int procvoutcite(SUBINFO *sip,VCINFO *vip,int ndays) noex {
	bibleverse_q	*qp = &vip->q ;
	int		rs = SR_OK ;
	int		wlen = 0 ;
	cchar		*fmt = nullptr ;

	if (sip->fl.separate && (sip->cout++ > 0)) {
	    fmt = OUTCOOKIE ;
	    rs = filer_printf(sip->ofp,fmt) ;
	    wlen += rs ;
	} /* end if (separator) */

	if (rs >= 0) {
	    cint	clen = COLBUFLEN ;
	    cint	b = qp->b ;
	    cint	c = qp->c ;
	    cint	v = qp->v ;
	    int		cl ;
	    int		f_havebook = false ;
	    cchar	*fmt ;
	    char	cbuf[COLBUFLEN + 1] ;

	    if (sip->fl.bookname && (vip->nbuf != nullptr)) {
	        if (vip->nbuf[0] != '\0') {
	            cint	nlen = vip->nlen ;
	            cchar	*nbuf = vip->nbuf ;
	            f_havebook = true ;
	            fmt = (ndays > 1) ? "%r %u:%u (%u)" : "%r %u:%u" ;
	            rs = bufprintf(cbuf,clen,fmt,nbuf,nlen,c,v,ndays) ;
	            cl = rs ;
	            if (rs >= 0) {
	                rs = filer_println(sip->ofp,cbuf,cl) ;
	                wlen += rs ;
	            }
	        } /* end if (nin-nul) */
	    } /* end if (book-name) */

	    if ((rs >= 0) && (! f_havebook)) {
	        fmt = (ndays > 1) ? "%u:%u:%u (%u)" : "%u:%u:%u" ;
	        rs = bufprintf(cbuf,clen,fmt,b,c,v,ndays) ;
	        cl = rs ;
	        if (rs >= 0) {
	            rs = filer_println(sip->ofp,cbuf,cl) ;
	            wlen += rs ;
	        }
	    } /* end if (type of book-name display) */

	} /* end if (ok) */

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (procvoutcite) */

static int procvoutverse(SUBINFO *sip,VCINFO *vcip) noex {
	bibleverse_q	*qp = &vcip->q ;
	int		rs = SR_OK ;
	int		vlen = vcip->vlen ;
	int		wlen = 0 ;
	cchar		*vbuf = vcip->vbuf ;

	rs = procoutverse(sip,qp,vbuf,vlen) ;
	wlen += rs ;

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (procvoutverse) */

static int procoutcite(SUBINFO *sip,bibleverse_q *qp,int ndays) noex {
	cint	clen = COLBUFLEN ;
	int		rs = SR_OK ;
	int		cl ;
	int		wlen = 0 ;
	cchar		*fmt ;
	char		cbuf[COLBUFLEN + 1] ;

/* print out any necessary separator */

	if (sip->fl.separate && (sip->cout++ > 0)) {
	    fmt = OUTCOOKIE ;
	    rs = filer_printf(sip->ofp,fmt) ;
	    wlen += rs ;
	} /* end if (separator) */

/* print out the text-data itself */

	if (rs >= 0) {
	    int		b = qp->b ;
	    int		c = qp->c ;
	    int		v = qp->v ;
	    int		f_havebook = false ;

	    if (sip->fl.bookname) {
	        cint	blen = BNBUFLEN ;
	        int		bbl ;
	        char		bbuf[BNBUFLEN + 1] ;

	        if ((bbl = subinfo_booklookup(sip,bbuf,blen,b)) > 0) {

	            f_havebook = true ;
	            fmt = (ndays > 1) ? "%r %u:%u (%u)" : "%r %u:%u" ;
	            rs = bufprintf(cbuf,clen,fmt,bbuf,bbl,c,v,ndays) ;
	            cl = rs ;
	            if (rs >= 0) {
	                rs = filer_println(sip->ofp,cbuf,cl) ;
	                wlen += rs ;
	            }

	        } /* end if (nlookup) */

	    } /* end if (book-name) */

	    if ((rs >= 0) && (! f_havebook)) {

	        fmt = (ndays > 1) ? "%u:%u:%u (%u)" : "%u:%u:%u" ;
	        rs = bufprintf(cbuf,clen,fmt,b,c,v,ndays) ;
	        cl = rs ;
	        if (rs >= 0) {
	            rs = filer_println(sip->ofp,cbuf,cl) ;
	            wlen += rs ;
	        }

	    } /* end if (type of book-name display) */

	} /* end if (ok) */

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (procoutcite) */

static int procoutverse(SUBINFO *sip,bibleverse_q *qp,cchar vp[],int vl) noex {
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
	cchar		*sp = vp ;
	char		cbuf[COLBUFLEN + 1] ;

	cbl = MIN((sip->linelen - sip->indent),clen) ;

	if ((rs >= 0) && sip->fl.para) {
	    rs = subinfo_ispara(sip,qp) ;
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

	            if (rs >= 0) {
	                rs = procoutline(sip,line,cbuf,cl) ;
	                wlen += rs ;
	                line += 1 ;
	            }

	        } /* end while (full lines) */

	        if (rs >= 0) {
	            if ((cl = wordfill_mklinepart(&w,cbuf,cbl)) > 0) {
	                rs = procoutline(sip,line,cbuf,cl) ;
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
}
/* end subroutine (procoutverse) */

/* ARGSUSED */
static int procoutline(SUBINFO *sip,int line,cchar *lp,int ll) noex {
	int		rs ;
	int		indent ;
	int		wlen = 0 ;
	cchar		*fmt ;

	indent = MIN(sip->indent,NBLANKS) ;
	fmt = "%r%r\n" ;
	rs = filer_printf(sip->ofp,fmt,blanks,indent,lp,ll) ;
	wlen += rs ;

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (procoutline) */

static int subinfo_start(SUBINFO *sip,cchar **envv) noex {
	int		rs = SR_OK ;

	if (envv == nullptr) envv = environ ;

	memset(sip,0,sizeof(SUBINFO)) ;
	sip->envv = envv ;
	sip->dt = time(nullptr) ;
	sip->indent = 1 ;
	sip->year = -1 ;
	sip->linelen = 0 ;
	sip->count = -1 ;
	sip->max = -1 ;

	sip->fl.defnull = OPT_DEFnullptr ;
	sip->fl.separate = OPT_SEPARATE ;
	sip->fl.bookname = OPT_BOOKNAME ;
	sip->fl.trycache = OPT_TRYCACHE ;

	return rs ;
}
/* end subroutine (subinfo_start) */

static int subinfo_finish(SUBINFO *sip) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (sip->open.vcache) {
	    rs1 = subinfo_vcend(sip) ;
	    if (rs >= 0) rs = rs1 ;
	}

	if (sip->open.pdb) {
	    sip->open.pdb = false ;
	    rs1 = biblepara_close(&sip->pdb) ;
	    if (rs >= 0) rs = rs1 ;
	}

	if (sip->open.sdb) {
	    sip->open.sdb = false ;
	    rs1 = bvs_close(&sip->sdb) ;
	    if (rs >= 0) rs = rs1 ;
	}

	if (sip->open.ndb) {
	    sip->open.ndb = false ;
	    rs1 = biblebook_close(&sip->ndb) ;
	    if (rs >= 0) rs = rs1 ;
	}

	if (sip->open.vdb) {
	    sip->open.vdb = false ;
	    rs1 = bibleverse_close(&sip->vdb) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (bibleverse) */

	return rs ;
}
/* end subroutine (subinfo_finish) */

static int subinfo_setroot(SUBINFO *sip,cchar *pr,cchar *sn) noex {
	if (pr == nullptr) return SR_FAULT ;
	sip->pr = pr ;
	if (sn == nullptr) sn = getourenv(sip->envv,VARSEARCHNAME) ;
	if (sn == nullptr) sn = SEARCHNAME ;
	sip->sn = sn ;
	return SR_OK ;
}
/* end subroutine (subinfo_setroot) */

static int subinfo_outbegin(SUBINFO *sip) noex {
	int		rs = SR_OK ;
	if (sip->fl.all || sip->fl.allcache) {
	    if ((rs = opentmp(nullptr,0,0)) >= 0) {
	        sip->fd = rs ;
	        sip->wfd = rs ;
	    }
	} else {
	    int		pipes[2] ;
	    if ((rs = u_pipe(pipes)) >= 0) {
	        sip->fd = pipes[0] ;
	        sip->wfd = pipes[1] ;
	    }
	}
	return (rs >= 0) ? sip->wfd : rs ;
}
/* end subroutine (subinfo_outbegin) */

static int subinfo_outend(SUBINFO *sip,int ars) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (sip->fl.all || sip->fl.allcache) {
	    rs = u_rewind(sip->fd) ;
	} else {
	    rs = u_close(sip->wfd) ;
	}
	if (ars < 0) {
	    rs1 = u_close(sip->fd) ;
	    if (rs >= 0) rs = rs1 ;
	    sip->fd = SR_NOENT ;
	}
	return (rs >= 0) ? sip->fd : rs ;
}
/* end subroutine (subinfo_outend) */

static int subinfo_booksize(SUBINFO *sip) noex {
	int		rs = SR_OK ;
	if ((rs = subinfo_book(sip)) >= 0) {
	    rs = biblebook_size(&sip->ndb) ;
	}
	return rs ;
}
/* end subroutine (subinfo_booksize) */

static int subinfo_booklookup(SUBINFO *sip,char *nbuf,int nlen,int bi) noex {
	int		rs ;
	int		len = 0 ;

	nbuf[0] = '\0' ;
	if ((rs = subinfo_book(sip)) >= 0) {
	    BIBLEBOOK	*bbp = &sip->ndb ;
	    rs = biblebook_lookup(bbp,nbuf,nlen,bi) ;
	    len = rs ;
	}

	return (rs >= 0) ? len : rs ;
}
/* end subroutine (subinfo_booklookup) */

static int subinfo_bookmatch(SUBINFO *sip,cchar *mbuf,int mlen) noex {
	int		rs ;
	int		bi = 0 ;

	if ((rs = subinfo_book(sip)) >= 0) {
	    BIBLEBOOK	*bbp = &sip->ndb ;
	    rs = biblebook_match(bbp,mbuf,mlen) ;
	    bi = rs ;
	}

	return (rs >= 0) ? bi : rs ;
}
/* end subroutine (subinfo_bookmatch) */

static int subinfo_book(SUBINFO *sip) noex {
	int		rs = SR_OK ;
	if (! sip->open.ndb) {
	    BIBLEBOOK	*bbp = &sip->ndb ;
	    rs = biblebook_open(bbp,sip->pr,sip->ndbname) ;
	    sip->open.ndb = (rs >= 0) ;
	}
	return rs ;
}
/* end subroutine (subinfo_book) */

/* get (find out) the current MJD (for today) */
static int subinfo_today(SUBINFO *sip) noex {
	cint	to = TO_MJD ;
	int		rs ;
	int		mjd = sip->mjd ;

	if ((rs = subinfo_tmtime(sip)) >= 0) {
	    if ((! sip->fl.mjd) || ((sip->dt - sip->ti_mjd) >= to)) {
	        TMTIME	*tmp = &sip->tm ;
	        int	yr ;
	        sip->ti_mjd = sip->dt ;
	        yr = (tmp->year + TM_YEAR_BASE) ;
	        rs = getmjd(yr,tmp->mon,tmp->mday) ;
	        mjd = rs ;
	        sip->fl.mjd = true ;
	        sip->mjd = mjd ;
	    }
	} /* end if (subinfo-tmtime) */

	return (rs >= 0) ? mjd : rs ;
}
/* end subroutine (subinfo_today) */

static int subinfo_defdayspec(SUBINFO *sip,dayspec *dsp) noex {
	int		rs = SR_OK ;
	if ((rs >= 0) && (dsp->y <= 0)) {
	    rs = subinfo_year(sip) ;
	    dsp->y = sip->year ;
	}
	if ((rs >= 0) && (dsp->m < 0)) {
	    rs = subinfo_tmtime(sip) ;
	    dsp->m = sip->tm.mon ;
	}
	return rs ;
}
/* end subroutine (subinfo_defdayspec) */

/* get the current year (if necessary) */
static int subinfo_year(SUBINFO *sip) noex {
	int		rs = SR_OK ;
	if (sip->year <= 0) {
	    if ((rs = subinfo_tmtime(sip)) >= 0) {
	        sip->year = (sip->tm.year + TM_YEAR_BASE) ;
	    }
	}
	return rs ;
}
/* end subroutine (subinfo_year) */

static int subinfo_tmtime(SUBINFO *sip) noex {
	cint	tc = TIMECOUNT ;
	cint	to = TO_TMTIME ;
	int		rs = SR_OK ;

	if ((! sip->fl.tmtime) || (sip->timecount++ >= tc)) {
	    if ((sip->dt == 0) || (sip->timecount == tc)) {
	        sip->dt = time(nullptr) ;
	    }
	    sip->timecount = 0 ;
	    if ((! sip->fl.tmtime) || ((sip->dt - sip->ti_tmtime) >= to)) {
	        sip->ti_tmtime = sip->dt ;
	        sip->fl.tmtime = true ;
	        if (sip->fl.gmt) {
	            rs = tmtime_gmtime(&sip->tm,sip->dt) ;
	        } else {
	            rs = tmtime_localtime(&sip->tm,sip->dt) ;
	        }
	    }
	} /* end if */

	return rs ;
}
/* end subroutine (subinfo_tmtime) */

static int subinfo_mkmodquery(SUBINFO *sip,bibleverse_q *qp,int mjd) noex {
	int		rs = SR_OK ;

	if (! sip->open.sdb) {
	    rs = subinfo_bvs(sip) ;
	}

	if (rs >= 0) {
	    BVS		*bsp = &sip->sdb ;
	    BVS_VERSE	bsq ;
	    rs = bvs_mkmodquery(bsp,&bsq,mjd) ;
	    qp->b = bsq.b ;
	    qp->c = bsq.c ;
	    qp->v = bsq.v ;
	}

	return rs ;
}
/* end subroutine (subinfo_mkmodquery) */

static int subinfo_bvs(SUBINFO *sip) noex {
	int		rs = SR_OK ;
	if (! sip->open.sdb) {
	    BVS		*bsp = &sip->sdb ;
	    cint	rsn = SR_NOENT ;
	    cchar	*pr = sip->pr ;
	    cchar	*sdbname = sip->sdbname ;
	    if ((rs = bvs_open(bsp,pr,sdbname)) >= 0) {
	        sip->open.sdb = true ;
	    } else if (rs == rsn) {
	        if ((rs = subinfo_bvsbuild(sip)) >= 0) {
	            rs = bvs_open(bsp,sip->pr,sip->sdbname) ;
	            sip->open.sdb = (rs >= 0) ;
	        }
	    }
	    if ((rs >= 0) && sip->fl.audit) {
	        rs = bvs_audit(bsp) ;
	    } /* end if (audit) */
	} /* end if (not open) */
	return rs ;
}
/* end subroutine (subinfo_bvs) */

/* build the BVS database */
static int subinfo_bvsbuild(SUBINFO *sip) noex {
{
	int		rs = SR_OK ;
	if (sip->open.vdb) {
	    rs = subinfo_bvsbuilder(sip) ;
	} else {
	    rs = SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (subinfo_bvsbuild) */

static int subinfo_bvsbuilder(SUBINFO *sip) noex {
	BVSMK		*bmp = &sip->bsmk ;
	cmode		om = 0666 ;
	cint		of = 0 ;
	int		rs = SR_OK ;
	int		rs1 ;
	cchar		*pr = sip->pr ;
	cchar		*sdbname = sip->sdbname ;

	if ((rs = bvsmk_open(bmp,pr,sdbname,of,om)) >= 0) {
	    sip->open.bvsmk = true ;
	    if (rs == 0) {
	        bibleverse	*vdbp = &sip->vdb ;
	        bibleverse_info	binfo ;
	        if ((rs = bibleverse_info(vdbp,&binfo)) >= 0) {
	            cint	maxbook = binfo.maxbook ;
	            cint	maxchapter = binfo.maxchapter ;
	            int		bal ;
	            int		sz ;
	            uchar		*bap = nullptr ;
	            bal = (maxchapter + 1) ;
	            sz = (bal * sizeof(uchar)) ;
	            if ((rs = uc_malloc(sz,&bap)) >= 0) {
	                int		nc ;
	                for (int b = 0 ; (rs >= 0) && (b <= maxbook) ; b += 1) {

	                    rs1 = bibleverse_chapters(vdbp,b,bap,bal) ;
	                    nc = rs1 ;

	                    if (rs1 == SR_NOTFOUND) {
	                        nc = 0 ;
	                    } else {
	                        rs = rs1 ;
			    }

	                    if (rs >= 0) {
	                        rs = bvsmk_add(bmp,b,bap,nc) ;
			    }

	                } /* end for (looping through the books) */

	                uc_free(bap) ;
	                bap = nullptr ;
	            } /* end if (temporary memory allocation) */
	        } /* end if (bibleverse_info) */
	    } /* end if (majing) */
	    sip->open.bvsmk = false ;
	    rs1 = bvsmk_close(bmp) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (bvsmk) */

	return rs ;
}
/* end subroutine (subinfo_bvsbuilder) */

/* return maximum days possible for this citation */
#if	CF_SUBNDATS
static int subinfo_ndays(SUBINFO *sip,bibleverse_q *qp,int ndays) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (ndays > 1) {
	    bibleverse_q	q = *qp ;
	    uint		v ;

	    rs1 = SR_NOTFOUND ;
	    while ((rs1 == SR_NOTFOUND) && (ndays > 1)) {

	        v = qp->v ;
	        v += (ndays-1) ;
	        if (v <= UCHAR_MAX) {

	            q.v = v ;
	            rs1 = bibleverse_read(&sip->vdb,nullptr,0,&q) ;

	        } /* end if */

	        if (rs1 == SR_NOTFOUND)
	            ndays -= 1 ;


	    } /* end while */

	} /* end if (have some) */

	return (rs >= 0) ? ndays : rs ;
}
/* end subroutine (subinfo_ndays) */
#endif /* CF_SUBNDATS */

static int subinfo_ispara(SUBINFO *sip,bibleverse_q *qp) noex {
	int		rs = SR_OK ;
	int		f = false ;

	if (qp == nullptr) return SR_FAULT ;

	if (! sip->open.pdb) {
	    if ((rs = biblepara_open(&sip->pdb,sip->pr,sip->pdbname)) >= 0) {
	        sip->open.pdb = true ;
	    } else if (isNotPresent(rs)) {
	        rs = SR_OK ;
	    }
	}

	if ((rs >= 0) && sip->open.pdb) {
	    BIBLEPARA_QUERY	pq ;
	    pq.b = qp->b ;
	    pq.c = qp->c ;
	    pq.v = qp->v ;
	    rs = biblepara_ispara(&sip->pdb,&pq) ;
	    f = (rs > 0) ;
	}

	return (rs >= 0) ? f : rs ;
}
/* end subroutine (subinfo_ispara) */

static int subinfo_vc(SUBINFO *sip) noex {
	int		rs = SR_OK ;
	if (sip->fl.trycache && (! sip->open.vcache)) {
	    rs = subinfo_vcbegin(sip) ;
	}
	return rs ;
}
/* end subroutine (subinfo_vc) */

static int subinfo_vcbegin(SUBINFO *sip) noex {
	int		rs = SR_OK ;
	if (sip->fl.trycache && (! sip->open.vcache)) {
	    cint	of = O_CREAT ;
	    cchar	*pr = sip->pr ;
	    if ((rs = votdc_open(&sip->vcache,pr,nullptr,of)) >= 0) {
	        sip->open.vcache = true ;
	    }
	}
	return rs ;
}
/* end subroutine (subinfo_vcbegin) */

static int subinfo_vcend(SUBINFO *sip) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (sip->open.vcache) {
	    rs1 = votdc_close(&sip->vcache) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
}
/* end subroutine (subinfo_vcend) */

static int subinfo_vcfetch(SUBINFO *sip,VCINFO *vcip,int mjd) noex {
	VOTDC		*vcp = &sip->vcache ;
	VOTDC_Q		vq ;
	cint	vlen = vcip->vlen ;
	int		rs ;
	int		vl = 0 ;
	cchar		*lang = sip->lang ;
	char		*vbuf = vcip->vbuf ;
	if (lang == nullptr) lang = DEFLANG ;
	if ((rs = votdc_versefetch(vcp,&vq,vbuf,vlen,lang,mjd)) >= 0) {
	    bibleverse_q	*qp = &vcip->q ;
	    vl = rs ;
	    qp->b = vq.b ;
	    qp->c = vq.c ;
	    qp->v = vq.v ;
	} /* end if (votdc_versefetch) */
	return (rs >= 0) ? vl : rs ;
}
/* end subroutine (subinfo_vcfetch) */

static int subinfo_vcmjd(SUBINFO *sip,VCINFO *vcip,int mjd) noex {
	bibleverse_q	*qp = &vcip->q ;
	int		rs ;
	int		vl = 0 ;
	if ((rs = subinfo_mkmodquery(sip,qp,mjd)) >= 0) {
	    if ((rs = subinfo_text(sip)) >= 0) {
	        cint	vlen = vcip->vlen ;
	        char		*vbuf = vcip->vbuf ;
	        vcip->q.b = qp->b ;
	        vcip->q.c = qp->c ;
	        vcip->q.v = qp->v ;
	        rs = subinfo_textquery(sip,vbuf,vlen,qp) ;
	        vl = rs ;
	    } /* end if (subinfo_text) */
	} /* end if (subinfo_mkmodquery) */
	return (rs >= 0) ? vl : rs ;
}
/* end subroutine (subinfo_vcmjd) */

static int subinfo_vcload(SUBINFO *sip,VCINFO *vcip,cchar *lang,int mjd,
		int vl) noex {
	int		rs ;
	if ((rs = subinfo_titlelang(sip)) >= 0) {
	    VOTDC		*vcp = &sip->vcache ;
	    VOTDC_Q		vq ;
	    bibleverse_q	*qp = &vcip->q ;
	    char		*vbuf = vcip->vbuf ;
	    votdsq_load(&vq,qp) ;
	    rs = votdc_verseload(vcp,lang,&vq,mjd,vbuf,vl) ;
	}
	return rs ;
}
/* end subroutine (subinfo_vcload) */

static int subinfo_vcbookname(SUBINFO *sip,VCINFO *vip,cchar *lang) noex {
	int		rs = SR_OK ;
	if (vip->nbuf[0] == '\0') {
	    VOTDC	*vcp = &sip->vcache ;
	    cint	nlen = vip->nlen ;
	    cint	b = vip->q.b ;
	    char	*nbuf = vip->nbuf ;
	    rs = votdc_titlefetch(vcp,nbuf,nlen,lang,b) ;
	} /* end if (name needed) */
	return rs ;
}
/* end subroutine (subinfo_vcbookname) */

static int subinfo_text(SUBINFO *sip) noex {
	int		rs = SR_OK ;
	if (! sip->open.vdb) {
	    cchar	*pr = sip->pr ;
	    cchar	*vdb = sip->vdbname ;
	    if ((rs = bibleverse_open(&sip->vdb,pr,vdb)) >= 0) {
	        sip->open.vdb = true ;
	        rs = 1 ;
	    }
	}
	return rs ;
}
/* end subroutine (subinfo_text) */

static int subinfo_textquery(SUBINFO *sip,char *vbuf,int vlen,
		bibleverse_q *qp) noex {
	return bibleverse_read(&sip->vdb,vbuf,vlen,qp) ;
	return rs ;
}
/* end subroutine (subinfo_textquery) */

static int subinfo_titlelang(SUBINFO *sip) noex {
	VOTDC		*vcp = &sip->vcache ;
	int		rs ;
	cchar		*lang = sip->lang ;
	if ((rs = votdc_titlelang(vcp,lang)) == 0) {
	    if ((rs = subinfo_booksize(sip)) >= 0) {
	        cint	sz = rs ;
	        cchar		*titles[VOTDC_NTITLES+1] ;
	        char		*bp ;
	        if ((rs = uc_malloc(sz,&bp)) >= 0) {
	            BIBLEBOOK	*bbp = &sip->ndb ;
	            cint	n = VOTDC_NTITLES ;
	            int		bl = 0 ;
	            cchar	*bstr = bp ;
	            for (int i = 0 ; i < n ; i += 1) {
	                rs = biblebook_read(bbp,bp,(sz-bl),i) ;
	                titles[i] = bp ;
	                bp[rs] = '\0' ;
	                bl += (rs+1) ;
	                bp += (rs+1) ;
	                if (rs < 0) break ;
	            } /* end for */
	            titles[i] = nullptr ;
	            if (rs >= 0) {
	                rs = votdc_titleloads(vcp,lang,titles) ;
	            }
	            uc_free(bstr) ;
	        } /* end if (m-a) */
	    } /* end if (subinfo_booksize) */
	} /* end if (votdc_titlelang) */
	return rs ;
}
/* end subroutine (subinfo_titlelang) */

static int vcinfo_start(VCINFO *op) noex {
	cint	nlen = BNBUFLEN ;
	cint	vlen = BVBUFLEN ;
	int		rs ;
	int		sz = 0 ;
	char		*bp ;
	memset(op,0,sizeof(VCINFO)) ;
	sz += (nlen+1) ;
	sz += (vlen+1) ;
	if ((rs = uc_malloc(sz,&bp)) >= 0) {
	    op->a = bp ;
	    op->nbuf = bp + (0) ;
	    op->nlen = nlen ;
	    op->vbuf = bp + (nlen+1) ;
	    op->vlen = vlen ;
	    {
	        op->nbuf[0] = '\0' ;
	        op->vbuf[0] = '\0' ;
	    }
	}
	return rs ;
}
/* end subroutine (vcinfo_start) */

static int vcinfo_finish(VCINFO *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->a != nullptr) {
	    rs1 = uc_free(op->a) ;
	    if (rs >= 0) rs = rs1 ;
	    op->a = nullptr ;
	    op->vbuf = nullptr ;
	    op->nbuf = nullptr ;
	    op->vlen = 0 ;
	    op->nlen = 0 ;
	}
	return rs ;
}
/* end subroutine (vcinfo_finish) */

static int votdsq_load(VOTDC_Q *vqp,bibleverse_q *qp) noex {
	vqp->b = qp->b ;
	vqp->c = qp->c ;
	vqp->v = qp->v ;
	return SR_OK ;
}
/* end subroutine (votdsq_load) */



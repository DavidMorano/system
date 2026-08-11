/* b_rfinger SUPPORT (KSH builtin) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* SHELL built-in FINGER client */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* compile-time debugging */
#define	CF_DEBUG	0		/* run-time debug print-outs */
#define	CF_DEBUGMALL	1		/* debug memory-allocations */
#define	CF_DEBUGN	0		/* special debugging */
#define	CF_MKFINGERARGS	1		/* use 'mkfingerquery()' */
#define	CF_CLEANLINES	0		/* clean lines */
#define	CF_LOCSETENT	0		/* |locinfo_setentry()| */
#define	CF_SPECIAL1	0		/* special-1 */
#define	CF_SPECIAL2	0		/* special-2 */
#define	CF_SPECIAL3	0		/* special-3 */
#define	CF_SPECIAL4	0		/* special-4 */

/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	b_rfinger

	Synopsis:
	$ rfinger <user>@<host> [-d <dialerspec>]



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
#include	<netdb.h>
#include	<ctime>
#include	<climites>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<uclibmem.h>
#include	<bits.h>
#include	<vecstr.h>
#include	<keyopt.h>
#include	<userinfo.h>
#include	<schedvar.h>
#include	<ascii.h>
#include	<vechand.h>
#include	<strpack.h>
#include	<field.h>
#include	<filer.h>
#include	<termout.h>
#include	<opendial.h>
#include	<snwcpy.h>
#include	<ischarx.h>
#include	<iserror.h>
#include	<isnot.h>
#include	<exitcodes.h>
#include	<localmisc.h>

#include	"shio.h"
#include	"kshlib.h"
#include	"b_rfinger.h"
#include	"defs.h"
#include	"proglog.h"
#include	"systems.h"
#include	"sysdialer.h"
#include	"cm.h"


/* local defines */

#define	LINEDISPLAYLEN	80

#ifndef	linebufLEN
#define	linebufLEN	2048
#endif

#define	QUERY_HOSTPARTLEN	MAXHOSTNAMELEN
#define	QUERY_USERPARTLEN	linebufLEN

#define	LOCINFO		struct locinfo
#define	LOCINFO_FL	struct locinfo_flags

#define	linebuf		struct linebuf

#define	NDF		"rfinger.deb"


/* external subroutines */

extern int	printhelp(void *,cchar *,cchar *,cchar *) noex ;
extern int	proginfo_setpiv(PROGINFO *,cchar *,const pivars *) noex ;
extern int	proginfo_rootname(PROGINFO *) noex ;

extern int	proguserlist_begin(PROGINFO *) noex ;
extern int	proguserlist_end(PROGINFO *) noex ;

#if	CF_DEBUGS || CF_DEBUG
extern int	debugopen(cchar *) noex ;
extern int	debugprintf(cchar *,...) noex ;
extern int	debugprinthex(cchar *,int,cchar *,int) noex ;
extern int	debugclose() noex ;
extern int	strlinelen(cchar *,int,int) noex ;
#endif /* CF_DEBUG */

#if	CF_DEBUGN
extern int	nprintf(cchar *,cchar *,...) noex ;
#endif


/* external variables */

extern char	**environ ;		/* definition required by AT&T AST */


/* local structures */

struct locinfo_flags {
	uint		stores:1 ;
	uint		to:1 ;
	uint		linelen:1 ;
	uint		systems:1 ;
	uint		args:1 ;
	uint		strs:1 ;
	uint		longer:1 ;
	uint		termout:1 ;
	uint		shutdown:1 ;
	uint		outer:1 ;
} ; /* end struct */

struct locinfo {
	LOCINFO_FL	have, f, changed, finval ;
	LOCINFO_FL	open ;
	vecstr		stores ;
	STRPACK		strs ;
	VECHAND		args ;
	TERMOUT		outer ;
	PROGINFO	*pip ;
	cchar		*dialerspec ;
	cchar		*afspec ;
	cchar		*portspec ;
	cchar		*svcspec ;
	cchar		*argspec ;
	cchar		*dialbuf ;
	cchar		**av ;		/* service-arguments */
	cchar		*argstab ;
	cchar		*termtype ;
	int		linelen ;
	int		dialer ;	/* open-dial dial-code */
	int		af ;		/* socket address-family */
	int		nav ;
} ; /* end struct */

struct linebuf {
	int		llen ;
	int		ll ;
	char		*lbuf ;
} ; /* end struct */

struct query {
	char		hpart[QUERY_HOSTPARTLEN+1] ;
	char		upart[QUERY_USERPARTLEN+1] ;
} ; /* end struct */


/* forward references */

local int	mainsub(int,cchar **,cchar **,void *) noex ;

local int	usage(PROGINFO *) noex ;

local int	procopts(PROGINFO *,keyopt *) noex ;
local int	procdefs(PROGINFO *) noex ;
local int	procargs(PROGINFO *,ARGINFO *,bits *,cc *,cc*,cc *) noex ;

local int	procuserinfo_begin(PROGINFO *,USERINFO *) noex ;
local int	procuserinfo_end(PROGINFO *) noex ;
local int	procuserinfo_logid(PROGINFO *) noex ;

local int	procdials(PROGINFO *,void *) noex ;
local int	procdial(PROGINFO *,void *,cchar *) noex ;
local int	procdialread(PROGINFO *,void *,int,linebuf *) noex ;

local int	procsystems(PROGINFO *,void *,cchar *) noex ;
local int	procsystem(PROGINFO *,void *,CM_ARGS *,cchar *) noex ;
local int	procsystemcm(PROGINFO *,void *,CM_ARGS *,cc *,linebuf *) noex ;
local int	procsysteminfo(PROGINFO *,CM *) noex ;
local int	procsystemread(PROGINFO *,void *,CM *,linebuf *) noex ;
local int	procsystemout(PROGINFO *,void *,int,linebuf *) noex ;

local int	locinfo_start(LOCINFO *,PROGINFO *) noex ;
local int	locinfo_finish(LOCINFO *) noex ;
local int	locinfo_deflinelen(LOCINFO *) noex ;
local int	locinfo_argload(LOCINFO *,cchar *,int) noex ;
local int	locinfo_argenum(LOCINFO *,int,cchar **) noex ;
local int	locinfo_logdialer(LOCINFO *) noex ;
local int	locinfo_setdialer(LOCINFO *,cchar *) noex ;
local int	locinfo_setsvcargs(LOCINFO *,cchar *) noex ;
local int	locinfo_setsvcarger(LOCINFO *,vecstr *,cchar *) noex ;
local int	locinfo_termoutbegin(LOCINFO *,void *) noex ;
local int	locinfo_termoutend(LOCINFO *) noex ;
local int	locinfo_termoutprint(LOCINFO *,void *,cchar *,int) noex ;

#if	CF_LOCSETENT
local int	locinfo_setentry(LOCINFO *,cchar **,cchar *,int) noex ;
#endif

local int	linebuf_start(linebuf *) noex ;
local int	linebuf_finish(linebuf *) noex ;

local int	loadsysfiles(PROGINFO *,systems *) noex ;
local int	query_parse(struct query *,cchar *) noex ;


/* local variables */

enum argopts {
	argopt_root,
	argopt_debug,
	argopt_version,
	argopt_verbose,
	argopt_help,
	argopt_log,
	argopt_sn,
	argopt_af,
	argopt_ef,
	argopt_of,
	argopt_sf,
	argopt_lf,
	argopt_to,
	argopt_overlast
} ; /* end enum (argopts) */

constexpr cpcchar	argopts[] = {
	"ROOT",
	"DEBUG",
	"VERSION",
	"VERBOSE",
	"HELP",
	"LOG",
	"sn",
	"af",
	"ef",
	"of",
	"sf",
	"lf",
	"to",
	nullptr
} ; /* end array (argopts) */

constexpr PIVARS	initvars = {
	VARPROGRAMROOT1,
	VARPROGRAMROOT2,
	VARPROGRAMROOT3,
	PROGRAMROOT,
	VARPRNAME
} ; /* end array */

constexpr MAPEX		mapexs[] = {
	{ SR_CONNREFUSED, EX_UNAVAILABLE },
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
	akoname_quiet,
	akoname_termout,
	akoname_shutdown,
	akoname_intopen,
	akoname_intread,
	akoname_overlast
} ; /* end enum (akonames) */

constexpr cpcchar	akonames[] = {
	"quiet",
	"termout",
	"shutdown",
	"intopen",
	"intread",
	nullptr
} ; /* end array */

constexpr cpcchar	sysfiles[] = {
	"%p/etc/%n/%n.%f",
	"%p/etc/%n/%f",
	"%p/etc/%n.%f",
	"%p/%n.%f",
	"%n.%f",
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


/* exported variables */


/* exported subroutines */

int b_rfinger(int argc,cchar *argv[],void *contextp) noex {
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
/* end subroutine (b_rfinger) */

int p_rfinger(int argc,cchar *argv[],cchar *envv[],void *contextp) noex {
#if	CF_DEBUGN
	nprintf(NDF,"b_rfinger/p_rfinger: ent\n") ;
#endif
	return mainsub(argc,argv,envv,contextp) ;
}
/* end subroutine (p_rfinger) */


/* local subroutines */

/* ARGSUSED */
local int mainsub(int argc,mainv argv,mainv envv,void *contextp) noex {
	PROGINFO	pi, *pip = &pi ;
	LOCINFO		li, *lip = &li ;
	ARGINFO		ainfo ;
	bits		pargs ;
	keyopt		akopts ;
	shio		efile ;

#if	(CF_DEBUGS || CF_DEBUG) && CF_DEBUGMALL
	uint		mo_start = 0 ;
#endif

	int		argr, argl, aol, akl, avl, kwi ;
	int		ai, ai_max, ai_pos ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		v ;
	int		ex = EX_INFO ;
	int		f_optminus, f_optplus, f_optequal ;
	int		f_version = false ;
	int		f_usage = false ;
	int		f_help = false ;
	int		f_long = false ;

	cchar		*argp, *aop, *akp, *avp ;
	cchar		*argval = nullptr ;
	cchar		*pr = nullptr ;
	cchar		*sn = nullptr ;
	cchar		*afname = nullptr ;
	cchar		*ofname = nullptr ;
	cchar		*efname = nullptr ;
	cchar		*sfname = nullptr ;
	cchar		*dialerspec = nullptr ;
	cchar		*afspec = nullptr ;
	cchar		*portspec = nullptr ;
	cchar		*argspec = nullptr ;
	cchar		*cp ;


#if	CF_DEBUGS || CF_DEBUG
	if ((cp = getourenv(envv,VARDEBUGFNAME)) != nullptr) {
	    rs = debugopen(cp) ;
	    debugprintf("b_rfinger: starting DFD=%d\n",rs) ;
	}
#endif /* CF_DEBUGS */

#if	(CF_DEBUGS || CF_DEBUG) && CF_DEBUGMALL
	uc_mallset(1) ;
	uc_mallout(&mo_start) ;
#endif

#if	CF_SPECIAL1
	if (rs >= 0) goto badprogstart ;
#endif

	rs = proginfo_start(pip,envv,argv[0],VERSION) ;
	if (rs < 0) {
	    ex = EX_OSERR ;
	    goto badprogstart ;
	}

#if	CF_SPECIAL2
	if (rs >= 0) goto badlocstart ;
#endif

	if ((cp = getourenv(envv,VARBANNER)) == nullptr) cp = BANNER ;
	rs = proginfo_setbanner(pip,cp) ;

/* initialization */

	pip->verboselevel = 1 ;
	pip->to_open = -1 ;
	pip->to_read = -1 ;
	pip->to = -1 ;
	pip->daytime = time(nullptr) ;

	pip->fl.logprog = true ;

#if	CF_SPECIAL3
	if (rs >= 0) goto badlocstart ;
#endif

	pip->lip = lip ;
	if (rs >= 0) rs = locinfo_start(lip,pip) ;
	if (rs < 0) {
	    ex = EX_OSERR ;
	    goto badlocstart ;
	}

#if	CF_SPECIAL4
	if (rs >= 0) goto badpargs ;
#endif

/* process program arguments */

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

/* debug level */
	                case argopt_debug:
	                    pip->debuglevel = 1 ;
	                    if (f_optequal) {
	                        f_optequal = false ;
	                        if (avl) {
	                            rs = optvalue(avp,avl) ;
	                            pip->debuglevel = rs ;
	                        }
	                    }
	                    break ;

	                case argopt_version:
	                    f_version = true ;
	                    break ;

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

/* help file */
	                case argopt_help:
	                    f_help  = true ;
	                    if (f_optequal) {
	                        f_optequal = false ;
	                        if (avl)
	                            pip->hfname = avp ;
	                    }
	                    break ;

/* log file */
	                case argopt_log:
	                case argopt_lf:
	                    if (f_optequal) {
	                        f_optequal = false ;
	                        if (avl) {
	                            pip->lfname = avp ;
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

/* argument files */
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

/* system file */
	                case argopt_sf:
	                    if (f_optequal) {
	                        f_optequal = false ;
	                        if (avl)
	                            sfname = avp ;
	                    } else {
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl)
	                                sfname = argp ;
	                        } else
	                            rs = SR_INVALID ;
	                    }
	                    break ;

/* timeout */
	                case argopt_to:
	                    if (argr > 0) {
	                        argp = argv[++ai] ;
	                        argr -= 1 ;
	                        argl = strlen(argp) ;
	                        if (argl) {
	                            rs = cfdecti(argp,argl,&v) ;
	                            pip->to = v ;
	                        }
	                    } else
	                        rs = SR_INVALID ;
	                    break ;

	                default:
	                    rs = SR_INVALID ;
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

	                    case 'V':
	                        f_version = true ;
	                        break ;

	                    case 'a':
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl)
	                                argspec = argp ;
	                        } else
	                            rs = SR_INVALID ;
	                        break ;

	                    case 'd':
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl)
	                                dialerspec = argp ;
	                        } else
	                            rs = SR_INVALID ;
	                        break ;

	                    case 'f':
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl)
	                                afspec = argp ;
	                        } else
	                            rs = SR_INVALID ;
	                        break ;

	                    case 'l':
	                        f_long = true ;
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

/* port specification */
	                    case 'p':
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl)
	                                portspec = argp ;
	                        } else
	                            rs = SR_INVALID ;
	                        break ;

	                    case 'q':
	                        pip->verboselevel = 0 ;
	                        break ;

/* service name */
	                    case 's':
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl)
	                                lip->svcspec = argp ;
	                        } else
	                            rs = SR_INVALID ;
	                        break ;

/* timeout */
	                    case 't':
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl) {
	                                rs = cfdecti(argp,argl,&v) ;
	                                pip->to = v ;
	                            }
	                        } else
	                            rs = SR_INVALID ;
	                        break ;

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

/* width (columns) */
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

/* fall through from above */
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
	if ((rs1 = shio_open(&efile,efname,"wca",0666)) >= 0) {
	    pip->efp = &efile ;
	    pip->open.errfile = true ;
	    shio_control(pip->efp,SHIO_CSETBUFLINE,true) ;
	} else if (! isFailOpen(rs1)) {
	    if (rs >= 0) rs = rs1 ;
	}

	if (rs < 0)
	    goto badarg ;

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    debugprintf("b_rfinger: debuglevel=%u\n",pip->debuglevel) ;
#endif

	if (f_version) {
	    shio_printf(pip->efp,"%s: version %s\n",pip->progname,VERSION) ;
	}

/* program root */

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
	    debugprintf("b_rfinger: pr=%s\n",pip->pr) ;
	    debugprintf("b_rfinger: sn=%s\n",pip->searchname) ;
	}
#endif

	if (pip->debuglevel > 0) {
	    shio_printf(pip->efp,"%s: pr=%s\n",pip->progname,pip->pr) ;
	    shio_printf(pip->efp,"%s: sn=%s\n",pip->progname,pip->searchname) ;
	}

	if (f_usage)
	    usage(pip) ;

/* help */

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

/* load up the argument and environment options */

	if (rs >= 0) {
	    if ((rs = proginfo_rootname(pip)) >= 0) {
	        if ((rs = procopts(pip,&akopts)) >= 0) {
		    rs = procdefs(pip) ;
		}
	    }
	}

	if (afname == nullptr) afname = getourenv(envv,VARAFNAME) ;

	if (pip->tmpdname == nullptr) pip->tmpdname = getourenv(envv,VARTMPDNAME) ;
	if (pip->tmpdname == nullptr) pip->tmpdname = TMPDNAME ;

/* argument processing */

	rs1 = (DEFPRECISION + 2) ;
	if ((rs >= 0) && (lip->linelen < rs1) && (argval != nullptr)) {
	    lip->have.linelen = true ;
	    lip->finval.linelen = true ;
	    rs = optvalue(argval,-1) ;
	    lip->linelen = rs ;
	}

#if	CF_DEBUG
	if (DEBUGLEVEL(2)) {
	    debugprintf("b_rfinger: dialerspec=%s\n",dialerspec) ;
	    debugprintf("b_rfinger: argspec=%s\n",argspec) ;
	    debugprintf("b_rfinger: linewidth=%u\n",lip->linelen) ;
	}
#endif

	if (rs >= 0) {
	    rs = locinfo_setdialer(lip,dialerspec) ;
	}

	lip->afspec = afspec ;
	if ((pip->debuglevel > 0) && (afspec != nullptr)) {
	    shio_printf(pip->efp,"%s: af=%s\n", pip->progname,afspec) ;
	}

	if (portspec != nullptr) lip->portspec = portspec ;

	if ((lip->svcspec == nullptr) || (lip->svcspec[0] == '\0')) {
	    lip->svcspec = SVCSPEC_RFINGER ;
	}

	if (rs >= 0) {
	    if ((rs = locinfo_setsvcargs(lip,argspec)) >= 0) {
	        rs = locinfo_deflinelen(lip) ;
	    }
	}

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    debugprintf("b_rfinger: linelen=%d\n",lip->linelen) ;
#endif

	if (pip->debuglevel > 0) {
	    cchar	*pn = pip->progname ;
	    cchar	*fmt = "%s: linelen=%u\n" ;
	    shio_printf(pip->efp,fmt,pn,lip->linelen) ;
	}

	lip->fl.longer = f_long ;

/* loop through the arguments */

	memclear(&ainfo) ;
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
	                    {
	                        cchar	*sfn = sfname ;
	                        cchar	*ofn = ofname ;
	                        cchar	*afn = afname ;
	                        rs = procargs(pip,&ainfo,&pargs,sfn,ofn,afn) ;
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
	        char	*fmt ;
	        ex = EX_NOUSER ;
	        fmt = "%s: userinfo failure (%d)\n" ;
	        shio_printf(pip->efp,fmt,pn,rs) ;
	    } /* end if (userinfo) */
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
	        if (! pip->fl.quiet) {
	            cchar	*pn = pip->progname ;
	            cchar	*fmt ;
	            fmt = "%s: invalid query (%d)\n" ;
	            shio_printf(pip->efp,fmt,pn,rs) ;
	        }
	        break ;
	    case SR_NOENT:
	        ex = EX_CANTCREAT ;
	        break ;
	    case SR_AGAIN:
	        ex = EX_TEMPFAIL ;
	        break ;
	    case SR_PIPE:
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

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    debugprintf("b_rfinger: exiting ex=%u (%d)\n",ex,rs) ;
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
	    uint	mo_finish ;
	    uc_mallout(&mo_finish) ;
	    uc_mallset(0) ;
	    debugprintf("b_rfinger: final mallout=%u\n",(mo_finish-mo_start)) ;
	}
#endif /* CF_DEBUGMALL */

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

}
/* end subroutine (mainsub) */

local int usage(PROGINFO *pip) noex {
	int		rs = SR_OK ;
	int		wlen = 0 ;
	cchar		*pn = pip->progname ;
	cchar		*fmt ;

	fmt = "%s: USAGE> %s [<spec(s)> [...]] [-d <dialer>[:<port>]]\n" ;
	if (rs >= 0) rs = shio_printf(pip->efp,fmt,pn,pn) ;
	wlen += rs ;

	fmt = "%s:  [-p <port>] [-s <service>] [-f <af>] [-l]\n" ;
	if (rs >= 0) rs = shio_printf(pip->efp,fmt,pn) ;
	wlen += rs ;

	fmt = "%s:  [-Q] [-D] [-v[=<n>]] [-HELP] [-V]\n" ;
	if (rs >= 0) rs = shio_printf(pip->efp,fmt,pn) ;
	wlen += rs ;

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (usage) */

/* process the program ako-options */
local int procopts(PROGINFO *pip,keyopt *kop) noex {
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
	                case akoname_quiet:
	                    if (! pip->finval.quiet) {
	                        pip->have.quiet = true ;
	                        pip->finval.quiet = true ;
	                        pip->fl.quiet = true ;
	                        if (vl > 0) {
	                            rs = optbool(vp,vl) ;
	                            pip->fl.quiet = (rs > 0) ;
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
	                case akoname_shutdown:
	                    if (! lip->finval.shutdown) {
	                        lip->have.shutdown = true ;
	                        lip->finval.shutdown = true ;
	                        lip->fl.shutdown = true ;
	                        if (vl > 0) {
	                            rs = optbool(vp,vl) ;
	                            lip->fl.shutdown = (rs > 0) ;
	                        }
	                    }
	                    break ;
	                case akoname_intopen:
	                    if (pip->to_open < 0) {
	                        if (vl > 0) {
				    int	v ;
	                            rs = cfdecti(vp,vl,&v) ;
	                            pip->to_open = v ;
	                        }
	                    }
	                    break ;
	                case akoname_intread:
	                    if (pip->to_read < 0) {
	                        if (vl > 0) {
				    int	v ;
	                            rs = cfdecti(vp,vl,&v) ;
	                            pip->to_read = v ;
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

local int procdefs(PROGINFO *pip) noex {
	int		rs = SR_OK ;

	if (pip->to_open < 0) {
	    pip->to_open = (pip->to >= 0) ? pip->to : TO_OPEN ;
	}

	if (pip->to_read < 0) {
	    pip->to_read = (pip->to >= 0) ? pip->to : TO_READ ;
	}

	return rs ;
}
/* end subroutine (procdefs) */

local int procargs(PROGINFO *pip,ARGINFO *aip,bits *bop,cchar *sfn,
		cchar *ofn,cchar *afn) noex {
	LOCINFO		*lip = pip->lip ;
	shio		ofile, *ofp = &ofile ;
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	cchar		*pn = pip->progname ;
	cchar		*fmt ;

	if ((ofn == nullptr) || (ofn[0] == '\0') || (ofn[0] == '-'))
	    ofn = STDFNOUT ;

	if ((rs = shio_open(ofp,ofn,"wct",0666)) >= 0) {
	    int		pan = 0 ;
	    int		cl ;
	    cchar	*cp ;
	    pip->open.outfile = true ;

	    if (rs >= 0) {
	        int	ai ;
	        int	f ;
	        for (ai = 1 ; ai < aip->argc ; ai += 1) {

	            f = (ai <= aip->ai_max) && (bits_test(bop,ai) > 0) ;
	            f = f || ((ai > aip->ai_pos) && (aip->argv[ai] != nullptr)) ;
	            if (f) {
	                cp = aip->argv[ai] ;
	                if (cp[0] != '\0') {
	                    pan += 1 ;
	                    rs = locinfo_argload(lip,cp,-1) ;
	                }
	            }

	            if (rs < 0) {
	                fmt = "%s: processing error (%d) addr=%s\n" ;
	                shio_printf(pip->efp,fmt,pn,rs,cp) ;
	                break ;
	            }
	        } /* end if (got a positional argument) */
	    } /* end if */

	    if ((rs >= 0) && (afn != nullptr) && (afn[0] != '\0')) {
	        shio	afile, *afp = &afile ;

	        if (afn[0] == '-') afn = STDFNIN ;

	        if ((rs = shio_open(afp,afn,"r",0666)) >= 0) {
	            cint	llen = linebufLEN ;
	            int		len ;
	            char	lbuf[linebufLEN + 1] ;

	            while ((rs = shio_readline(afp,lbuf,llen)) > 0) {
	                len = rs ;

	                if (lbuf[len - 1] == '\n') len -= 1 ;
	                lbuf[len] = '\0' ;

	                if ((cl = sfskipwhite(lbuf,len,&cp)) > 0) {
	                    if (cp[0] != '#') {
	                        pan += 1 ;
	                        rs = locinfo_argload(lip,cp,cl) ;
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

	    } /* end if (processing file argument file list) */

	    if (rs >= 0) {
	        if ((rs = locinfo_termoutbegin(lip,ofp)) >= 0) {

	            if (lip->dialerspec != nullptr) {
	                rs = procdials(pip,ofp) ;
	                wlen += rs ;
	            } else {
	                rs = procsystems(pip,ofp,sfn) ;
	                wlen += rs ;
	            }

	            rs1 = locinfo_termoutend(lip) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (termout) */
	    } /* end if (ok) */

	    pip->open.outfile = false ;
	    rs1 = shio_close(ofp) ;
	    if (rs >= 0) rs = rs1 ;
	} else {
	    fmt = "%s: inaccessible output (%d)\n" ;
	    shio_printf(pip->efp,fmt,pn,rs) ;
	    shio_printf(pip->efp,"%s: ofile=%s\n",pn,ofn) ;
	} /* end if (opened output) */

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (procargs) */

local int procdials(PROGINFO *pip,void *ofp) noex {
	LOCINFO		*lip = pip->lip ;
	int		rs = SR_OK ;

	lip->af = AF_UNSPEC ;
	if ((lip->afspec != nullptr) && (lip->afspec[0] != '\0')) {
	    rs = getaf(lip->afspec,-1) ;
	    lip->af = rs ;
	}

	if (rs >= 0) {
	    if ((rs = locinfo_logdialer(lip)) >= 0) {
		int	i = 0 ;
		int	al ;
		cchar	*ap ;
	        while (rs >= 0) {
	            al = locinfo_argenum(lip,i++,&ap) ;
	            if (al == SR_NOTFOUND) break ;
	            rs = al ;
	            if (rs >= 0) {
	                if (ap == nullptr) continue ;
	                rs = procdial(pip,ofp,ap) ;
	            }
		} /* end while */
	    } /* end if (locinfo_logdialer) */
	} /* end if (ok) */

	return rs ;
}
/* end subroutine (procdials) */

local int procdial(PROGINFO *pip,void *ofp,cchar *ap) noex {
	LOCINFO		*lip = pip->lip ;
	struct query	q ;
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	    debugprintf("main/procdial: ap=>%s<\n",ap) ;
#endif

	if ((rs = query_parse(&q,ap)) >= 0) {
	    linebuf	b ;
	    if ((rs = linebuf_start(&b)) >= 0) {
		cint	f_long = lip->fl.longer ;
		cint	llen = b.llen ;
		cchar		**av = lip->av ;
		cchar		*un = q.upart ;
		char		*lbuf = b.lbuf ;
		if ((rs = mkfingerquery(lbuf,llen,f_long,un,av)) >= 0) {
		    cint	ql = rs ;
		    cint	di = lip->dialer ;
		    cint	af = lip->af ;
		    cint	to = pip->to_open ;
		    cint	oo = 0 ;
		    cchar	**ev = pip->envv ;
		    cchar	*hn = q.hpart ;
		    cchar	*ps = lip->portspec ;
		    cchar	*ss = lip->svcspec ;
		    cchar	*qp = lbuf ;
		    if ((rs = opendial(di,af,hn,ps,ss,nullptr,ev,to,oo)) >= 0) {
	    		cint	s = rs ;
	    		if ((rs = uc_writen(s,qp,ql)) >= 0) {
	    		    if (lip->fl.shutdown && isasocket(s)) {
	        		rs = u_shutdown(s,SHUT_WR) ;
			    }
			    if (rs >= 0) {
			        rs = procdialread(pip,ofp,s,&b) ;
				wlen = rs ;
			    }
			} /* end if (uc_writen) */
	    		rs1 = u_close(s) ;
	    		if (rs >= 0) rs = rs1 ;
		    } /* end if (opendial) */
		} /* end if (mkfingerquery) */
		rs1 = linebuf_finish(&b) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (linebuf) */
	} /* end if (query_parse) */

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (procdial) */

local int procdialread(PROGINFO *pip,void *ofp,int s,linebuf *lbp) noex {
	LOCINFO		*lip = pip->lip ;
	cint		opts = (FILER_ONET&0) ;
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	if (filer b ; (rs = filer_start(&b,s,0z,512,opts)) >= 0) {
	    cint	to = pip->to_read ;
	    cint	llen = lbp->llen ;
	    char	*lbuf = lbp->lbuf ;
	    while ((rs = filer_readlns(&b,lbuf,llen,to,nullptr)) > 0) {
		cchar	*lp = lbuf ;
		int	ll = rs ;

		if ((ll > 0) && (lp[ll-1] == '\n')) ll -= 1 ;

#if	CF_DEBUG
	        if (DEBUGLEVEL(3))
	            debugprintf("main/procdialread: ll=%d l=>%r<\n",ll,lp,ll) ;
#endif

		if (rs >= 0) rs = lib_sigterm() ;
		if (rs >= 0) rs = lib_sigintr() ;

		if (rs >= 0) {
		    cint	clen = linebufLEN ;
		    int		cl ;
		    char	cbuf[linebufLEN+1] ;
		    if (ll > clen) ll = clen ;
		    if ((rs = snwcpyclean(cbuf,clen,'¿',lp,ll)) >= 0) {
			cl = rs ;
			if (lip->open.outer) {
			    rs = locinfo_termoutprint(lip,ofp,cbuf,cl) ;
			    wlen += rs ;
			} else {
			    rs = shio_print(ofp,cbuf,cl) ;
			    wlen += rs ;
			}
		    } /* end if (snwcpyclean) */
#if	CF_DEBUG
		    if (DEBUGLEVEL(3))
	    	        debugprintf("main/procdialread: out4 rs=%d\n",rs) ;
#endif
		} /* end if (ok) */

	        if (rs < 0) break ;
	    } /* end while */
	    rs1 = filer_finish(&b) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (filer) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (procdialread) */

local int procsystems(PROGINFO *pip,void *ofp,cchar *sfname) noex {
    	cnullptr	np{} ;
	LOCINFO		*lip = pip->lip ;
	SYSDIALER	d ;
	int		rs ;
	int		rs1 ;
	if ((rs = sysdialer_start(&d,pip->pr,np,np)) >= 0) {
	    CM_ARGS	ca{} ;
	    systems	sysdb ;
	    int		al ;
	    cchar	*ap ;
	    ca.pr = pip->pr ;
	    ca.prn = pip->rootname ;
	    ca.searchname = pip->searchname ;
	    ca.nodename = pip->nodename ;
	    ca.domainname = pip->domainname ;
	    ca.username = pip->username ;
	    ca.sp = &sysdb ;
	    ca.dp = &d ;
	    ca.timeout = pip->to_open ;
	    ca.options = (SYSDIALER_MFULL | SYSDIALER_MCO) ;

/* do it */

	    if ((rs = systems_open(&sysdb,sfname)) >= 0) {

#if	CF_DEBUG
	        if (DEBUGLEVEL(5))
	            debugprintf("b_rfinger: systems_open() rs=%d\n",rs) ;
#endif

	        if (sfname == nullptr) {
	            rs = loadsysfiles(pip,&sysdb) ;
	        } /* end if (loadfiles) */

#if	CF_DEBUG && 0
	        if (DEBUGLEVEL(5)) {
	            systems_cur	cur ;
	            systems_ent	*sep ;
	            debugprintf("b_rfinger: sysnames: \n") ;
	            systems_curbegin(&sysdb,&cur) ;
	            while (systems_curenum(&sysdb,&cur,&sep) >= 0) {
	                debugprintf("b_rfinger: sysname=%s\n",sep->sysname) ;
	            }
	            systems_curend(&sysdb,&cur) ;
	        }
#endif /* CF_DEBUG */

	        if (rs >= 0) {
	            int	i = 0 ;
	            while (rs >= 0) {
	                al = locinfo_argenum(lip,i++,&ap) ;
	                if (al == SR_NOTFOUND) break ;
	                rs = al ;
	                if ((rs >= 0) && (ap != nullptr)) {
	                    rs = procsystem(pip,ofp,&ca,ap) ;
	                }
	            } /* end while */
	        } /* end if (ok) */

	        rs1 = systems_close(&sysdb) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (systems) */
	    rs1 = sysdialer_finish(&d) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (sysdialer) */
	return rs ;
}
/* end subroutine (procsystems) */

local int procsystem(PROGINFO *pip,void *ofp,CM_ARGS *cap,cchar *ap) noex {
	LOCINFO		*lip = pip->lip ;
	struct query	q ;
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	int		f_long ;

#if	CF_DEBUG
	if (DEBUGLEVEL(3)) {
	    debugprintf("main/procsystem: ap=>%s<\n",ap) ;
	    debugprintf("main/procsystem: svc=%s\n",lip->svcspec) ;
	}
#endif

	f_long = lip->fl.longer ;
	if ((rs = query_parse(&q,ap)) >= 0) {
	    linebuf	b ;
	    if ((rs = linebuf_start(&b)) >= 0) {
	        cint	ll = rs ;
	        cchar		**av = lip->av ;
	        char		*lp = b.lbuf ;
	        if ((rs = mkfingerquery(lp,ll,f_long,q.upart,av)) >= 0) {
		    b.ll = rs ;
		    rs = procsystemcm(pip,ofp,cap,q.hpart,&b) ;
		    wlen = rs ;
	        }
		rs1 = linebuf_finish(&b) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (linebuf) */
	} /* end if (query_parse) */

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	    debugprintf("main/procsystem: ret rs=%d wlen=%u\n",rs,wlen) ;
#endif

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (procsystem) */

local int procsystemcm(PROGINFO *pip,void *ofp,CM_ARGS *cap,cchar *hn,
		linebuf *lbp) noex {
	LOCINFO		*lip = pip->lip ;
	CM		con ;
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	    debugprintf("main/procsystemcm: ent\n") ;
#endif
	if ((rs = cm_open(&con,cap,hn,lip->svcspec,nullptr)) >= 0) {
	    if ((rs = procsysteminfo(pip,&con)) >= 0) {
		cint	ql = lbp->ll ;
		cchar		*qp = lbp->lbuf ;
		if ((rs = cm_write(&con,qp,ql)) >= 0) {
	   	    if (lip->fl.shutdown) {
	                rs = cm_shutdown(&con,SHUT_WR) ;
	            }
	            if (rs >= 0) {
	                rs = procsystemread(pip,ofp,&con,lbp) ;
	                wlen = rs ;
	            }
		} /* end if (cm_write) */
	    } /* end if (procsysteminfo) */
	    rs1 = cm_close(&con) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (cm) */
#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	    debugprintf("main/procsystemcm: ret rs=%d wlen=%u\n",rs,wlen) ;
#endif
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (procsystemcm) */

local int procsysteminfo(PROGINFO *pip,CM *conp) noex {
	int		rs ;
	CM_INFO		ci ;
	if ((rs = cm_info(conp,&ci)) >= 0) {
	    cchar	*fmt ;

#if	CF_DEBUG
	    if (DEBUGLEVEL(3)) {
	        debugprintf("main/procsystem: cm_info() rs=%d\n",rs) ;
	        debugprintf("main/procsystem: dialer=%s\n",ci.dname) ;
	    }
#endif

	    if (pip->debuglevel > 0) {
		cchar	*pn = pip->progname ;
	        fmt = "%s: systems dialer=%s\n" ;
	        shio_printf(pip->efp,fmt,pn,ci.dname) ;
	    }

	    if (pip->open.logprog) {
		cchar	*ds = ((rs >= 0) ? ci.dname : "*") ;
		fmt = "systems dialer=%s" ;
	        proglog_printf(pip,fmt,ds) ;
	    }

	} /* end if (cm_info) */
	return rs ;
}
/* end subroutine (procsysteminfo) */

local int procsystemread(PROGINFO *pip,void *ofp,CM *conp,linebuf *lbp) noex {
	const mode_t	om = 0664 ;
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	if ((rs = openshmtmp(nullptr,0,om)) >= 0) {
	    cint	ropts = 0 ;
	    cint	llen = lbp->llen ;
	    cint	fd = rs ;
	    cint	to = pip->to_read ;
	    char	*lbuf = lbp->lbuf ;

	    while ((rs = cm_reade(conp,lbuf,llen,to,ropts)) > 0) {
	        rs = u_write(fd,lbuf,rs) ;
	        if (rs < 0) break ;
	    } /* end while */

	    if (rs >= 0) {
	        if ((rs = u_rewind(fd)) >= 0) {
	            rs = procsystemout(pip,ofp,fd,lbp) ;
	            wlen = rs ;
	        }
	    } /* end if (ok) */

	    rs1 = u_close(fd) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (opentmp) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (procsystemread) */

local int procsystemout(PROGINFO *pip,void *ofp,int fd,linebuf *lbp) noex {
	LOCINFO		*lip = pip->lip ;
	cint		opts = FILER_ONET ;
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	if (filer b ; (rs = filer_start(&b,fd,0z,512,opts)) >= 0) {
	    cint	llen = lbp->llen ;
	    cint	to = pip->to_read ;
	    char	*lbuf = lbp->lbuf ;
	    while ((rs = filer_readlns(&b,lbuf,llen,to,nullptr)) > 0) {
	        cchar	*lp = lbuf ;
	        int	ll = rs ;

	        if ((ll > 0) && (lp[ll-1] == '\n')) ll -= 1 ;

	        if (rs >= 0) rs = lib_sigterm() ;
	        if (rs >= 0) rs = lib_sigintr() ;

	        if (rs >= 0) {
		    cint	clen = linebufLEN ;
	            char	cbuf[linebufLEN+1] ;
		    if (ll > clen) ll = clen ;
	            if ((rs = snwcpyclean(cbuf,clen,'¿',lp,ll)) >= 0) {
	                cint	cl = rs ;
	                if (lip->open.outer && (cl > 0)) {
	                    rs = locinfo_termoutprint(lip,ofp,cbuf,cl) ;
	                    wlen += rs ;
	                } else {
	                    rs = shio_print(ofp,cbuf,cl) ;
	                    wlen += rs ;
	                }
		    } /* end if (cleaning) */
	        } /* end if (ok) */

	    } /* end while (reading lines) */
	    rs1 = filer_finish(&b) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (filer) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (procsystemout) */

local int procuserinfo_begin(PROGINFO *pip,USERINFO *uip) noex {
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

local int procuserinfo_end(PROGINFO *pip) noex {
	int		rs = SR_OK ;

	if (pip == nullptr) return SR_FAULT ;

	return rs ;
}
/* end subroutine (procuserinfo_end) */

local int procuserinfo_logid(PROGINFO *pip) noex {
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

local int loadsysfiles(PROGINFO *pip,systems *sdbp) noex {
	schedvar	sf ;
	int		rs ;
	int		rs1 ;
	int		n = 0 ;

#if	CF_DEBUG
	if (DEBUGLEVEL(5))
	    debugprintf("b_rfinger/loadsysfiles: ent\n") ;
#endif

	if ((rs = schedvar_start(&sf)) >= 0) {
	    cint	tlen = MAXPATHLEN ;
	    int		i, j ;
	    char	tbuf[MAXPATHLEN + 1] ;

	    schedvar_add(&sf,"p",pip->pr,-1) ;

	    schedvar_add(&sf,"n",pip->searchname,-1) ;

	    for (j = 0 ; j < 2 ; j += 1) {

	        if (j == 0) {
	            schedvar_add(&sf,"f",SYSFNAME1,-1) ;
	        } else {
	            schedvar_add(&sf,"f",SYSFNAME2,-1) ;
	        }

	        for (i = 0 ; sysfiles[i] != nullptr ; i += 1) {

	            rs1 = SR_NOENT ;
	            rs = schedvar_expand(&sf,tbuf,tlen,sysfiles[i],-1) ;
	            if (rs >= 0)
	                rs1 = u_access(tbuf,R_OK) ;

	            if (rs1 >= 0) {

	                n += 1 ;
	                rs = systems_fileadd(sdbp,tbuf) ;

#if	CF_DEBUG
	                if (DEBUGLEVEL(5)) {
	                    debugprintf("b_rfinger/loadsysfiles: "
	                        "systems_fileadd() rs=%d\n", rs) ;
	                    debugprintf("b_rfinger/loadsysfiles: "
	                        "fname=%s\n",tbuf) ;
	                }
#endif /* CF_DEBUGS */

	            } /* end if */

	            if (rs < 0) break ;
	        } /* end for */

	        if (rs < 0) break ;
	    } /* end for */

	    rs1 = schedvar_finish(&sf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (schedvar) */

#if	CF_DEBUG
	if (DEBUGLEVEL(5))
	    debugprintf("b_rfinger/loadsysfiles: ret rs=%d n=%u\n",rs,n) ;
#endif

	return (rs >= 0) ? n : rs ;
}
/* end subroutine (loadsysfiles) */

local int locinfo_start(LOCINFO *lip,PROGINFO *pip) noex {
	int		rs ;
	cchar		*varterm = VARTERM ;

	if (lip == nullptr) return SR_FAULT ;

	memclear(lip) ;
	lip->pip = pip ;
	lip->dialer = -1 ;
	lip->termtype = getourenv(pip->envv,varterm) ;
	lip->fl.shutdown = true ;

	if ((rs = strpack_start(&lip->strs,0)) >= 0) {
	    rs = vechand_start(&lip->args,0,0) ;
	    if (rs < 0) {
	        strpack_finish(&lip->strs) ;
	    }
	}

	return rs ;
}
/* end subroutine (locinfo_start) */

local int locinfo_finish(LOCINFO *lip) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (lip == nullptr) return SR_FAULT ;

	if (lip->av != nullptr) {
	    rs1 = lm_free(lip->av) ;
	    if (rs >= 0) rs = rs1 ;
	    lip->av = nullptr ;
	}

	if (lip->argstab != nullptr) {
	    rs1 = lm_free(lip->argstab) ;
	    if (rs >= 0) rs = rs1 ;
	    lip->argstab = nullptr ;
	}

	if (lip->dialbuf != nullptr) {
	    rs1 = lm_free(lip->dialbuf) ;
	    if (rs >= 0) rs = rs1 ;
	    lip->dialbuf = nullptr ;
	}

	rs1 = vechand_finish(&lip->args) ;
	if (rs >= 0) rs = rs1 ;

	rs1 = strpack_finish(&lip->strs) ;
	if (rs >= 0) rs = rs1 ;

	if (lip->open.stores) {
	    lip->open.stores = false ;
	    rs1 = vecstr_finish(&lip->stores) ;
	    if (rs >= 0) rs = rs1 ;
	}

	return rs ;
}
/* end subroutine (locinfo_finish) */

#if	CF_LOCSETENT
local int locinfo_setentry(LOCINFO *lip,cchar **epp,cchar *vp,int vl) noex {
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

local int locinfo_deflinelen(LOCINFO *lip) noex {
	cint	def = (DEFPRECISION + 2) ;
	int		rs = SR_OK ;
	if (lip->linelen < def) {
	    PROGINFO	*pip = lip->pip ;
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
}
/* end subroutine (locinfo_deflinelen) */


int locinfo_argload(LOCINFO *lip,cchar *ap,int al)
{
	int		rs = SR_OK ;

	if (al >= 0) {
	    cchar	*rp ;
	    rs = strpack_store(&lip->strs,ap,al,&rp) ;
	    ap = rp ;
	}

	if (rs >= 0) {
	    rs = vechand_add(&lip->args,ap) ;
	}

	return rs ;
}
/* end subroutine (locinfo_argload) */


int locinfo_argenum(LOCINFO *lip,int i,cchar **rpp)
{
	int		rs ;

	rs = vechand_get(&lip->args,i,rpp) ;

	return rs ;
}
/* end subroutine (locinfo_argenum) */

local int locinfo_setdialer(LOCINFO *lip,cchar *ds) noex {
	PROGINFO	*pip = lip->pip ;
	int		rs = SR_OK ;

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    debugprintf("b_rfinger: dialerspec=%s\n",ds) ;
#endif

	if (ds != nullptr) {
	    cchar	*tp ;

/* does this dialer specification have a port-like part */

	    if ((tp = strchr(ds,':')) != nullptr) {
	        lip->portspec = (tp + 1) ;
	        if (cchar *cp ; (rs = lm_strw(ds,(tp-ds),&cp)) >= 0) {
	            lip->dialbuf = cp ;
	            lip->dialerspec = cp ;
	        }
	    } else {
	        lip->dialerspec = ds ;
	    }

#if	CF_DEBUG
	    if (DEBUGLEVEL(3)) {
	        debugprintf("b_rfinger: 2 dialerspec=%s\n",lip->dialerspec) ;
	        debugprintf("b_rfinger: 2 portspec=%s\n",lip->portspec) ;
	    }
#endif /* CF_DEBUG */

/* find the open-dial "dial-code" */

	    if (rs >= 0) {
		cchar	*pn = pip->progname ;
		cchar	*ds = lip->dialerspec ;
		cchar	*fmt ;
	        if ((rs = getopendial(ds)) >= 0) {
	            lip->dialer = rs ;
	            if (pip->debuglevel > 0) {
		        cint	di = rs ;
			fmt = "%s: specified dialer=%s(%u)\n" ;
	                shio_printf(pip->efp,fmt,pn,ds,di) ;
	            }
	        } else {
	            fmt = "%s: unknown dialer=%s (%d)\n" ;
	            shio_printf(pip->efp,fmt,pn,ds,rs) ;
	        }
	        if (rs < 0) {
	            if (lip->dialbuf != nullptr) {
	                lm_free(lip->dialbuf) ;
	                lip->dialbuf = nullptr ;
	            }
	        }
	    } /* end if (ok) */

	} else {
	    lip->dialer = -1 ;
	} /* end if (non-null) */

	return rs ;
}
/* end subroutine (locinfo_setdialer) */

local int locinfo_logdialer(LOCINFO *lip) noex {
	PROGINFO	*pip = lip->pip ;
	int		rs = SR_OK ;
	if (pip->open.logprog) {
	    cint	di = lip->dialer ;
	    if (di >= 0) {
		cchar	*ds = lip->dialerspec ;
		cchar	*fmt = "specified dialer=%s(%u)" ;
		proglog_printf(pip,fmt,ds,di) ;
	    }
	}
	return rs ;
}
/* end subroutine (locinfo_logdialer) */

local int locinfo_setsvcargs(LOCINFO *lip,cchar *argspec) noex {
	PROGINFO	*pip = lip->pip ;
	int		rs = SR_OK ;
	int		c = 0 ;

	if (pip == nullptr) return SR_FAULT ;

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	    debugprintf("b_rfinger: argspec=>%s<\n",argspec) ;
#endif

	if (argspec != nullptr) {
	    VECSTR	al, *alp = &al ;
	    if ((rs = vecstr_start(alp,0,0)) >= 0) {
	        if ((rs = locinfo_setsvcarger(lip,alp,argspec)) >= 0) {
	            cint	ts = vecstr_strsize(alp) ;
	            int		c = rs ;
	            int		as ;

	            as = ((c + 2) * szof(cchar *)) ;
	            if (mainv av ; (rs = lm_mall(as,&av)) >= 0) {
	                char	*at = nullptr ;
	                if ((rs = lm_mall(ts,&at)) >= 0) {

	                    if ((rs = vecstr_avmkstr(alp,av,as,at,ts)) >= 0) {
	                        lip->nav = (c+1) ;
	                        lip->av = av ;
	                        lip->argstab = at ;
	                    }

	                    if (rs < 0) lm_free(at) ;
	                } /* end if (memory-allocation) */
	                if (rs < 0) {
	                    lm_free(av) ;
			}
	            } /* end if (memory-allocation) */

	        } /* end if (arger) */
	        vecstr_finish(alp) ;
	    } /* end if (vecstr) */
	} /* end if (non-null) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (locinfo_setsvcargs) */

local int locinfo_setsvcarger(LOCINFO *lip,vecstr *alp,cchar *argspec) noex {
	cint	alen = strlen(argspec) ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if (lip == nullptr) return SR_FAULT ;
	if (field fsb ; (rs = field_start(&fsb,argspec,alen)) >= 0) {
	    cint	flen = alen ;
	    if (char *fbuf ; (rs = lm_mall((flen+1),&fbuf)) >= 0) {
	        int	fl ;
	        while ((fl = field_sharg(&fsb,aterms,fbuf,flen)) >= 0) {
	            c += 1 ;
	            rs = vecstr_add(alp,fbuf,fl) ;
	            if (fsb.term == '#') break ;
	            if (rs < 0) break ;
	        } /* end while */
	        rs1 = lm_free(fbuf) ;
	        rs1 = lm_free(vp) ;
	    } /* end if (m-a) */
	    rs1 = field_finish(&fsb) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (field) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (locinfo_setsvcarger) */

local int locinfo_termoutbegin(LOCINFO *lip,void *ofp) noex {
	PROGINFO	*pip = lip->pip ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		f_termout = false ;
	cchar		*tstr = lip->termtype ;

	if (lip->fl.termout || ((rs = shio_isterm(ofp)) > 0)) {
	    int		ncols = COLUMNS ;
	    cchar	*vp ;
	    if ((vp = getourenv(pip->envv,VARCOLUMNS)) != nullptr) {
	        int	v ;
	        rs1 = cfdeci(vp,-1,&v) ;
	        if (rs1 >= 0) ncols = v ;
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
	    if (f_termout)
	        shio_printf(pip->efp,"%s: termtype=%s\n",pn,tstr) ;
	}

	return (rs >= 0) ? f_termout : rs ;
}
/* end subroutine (locinfo_termoutbegin) */

local int locinfo_termoutend(LOCINFO *lip) noex {
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

local int locinfo_termoutprint(LOCINFO *lip,void *ofp,cc *lbuf,int llen) noex {
	PROGINFO	*pip = lip->pip ;
	TERMOUT		*top = &lip->outer ;
	int		rs ;
	int		wlen = 0 ;

	if (pip == nullptr) return SR_FAULT ;

	if (llen > 0) {
	    if ((rs = termout_load(top,lbuf,llen)) >= 0) {
	        int	ln = rs ;
	        int	i ;
	        int	ll ;
	        cchar	*lp ;
	        for (i = 0 ; i < ln ; i += 1) {
	            ll = termout_getline(top,i,&lp) ;
	            if (ll < 0) break ;

#if	CF_DEBUG
	            if (DEBUGLEVEL(4)) {
	                debugprintf("b_rfinger/locinfo_termoutprint: ll=%u\n",
	                    ll) ;
	                debugprintf("b_rfinger/locinfo_termoutprint: l=>%r<\n",
	                    lp,strlinelen(lp,ll,40)) ;
	            }
#endif

	            rs = shio_print(ofp,lp,ll) ;
	            wlen += rs ;
	            if (rs < 0) break ;

	        } /* end for */
	        if ((rs >= 0) && (ll != SR_NOTFOUND)) rs = ll ;
	    } /* end if (termoutprint) */
	} else {
	    rs = shio_print(ofp,lbuf,llen) ;
	    wlen += rs ;
	}

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (locinfo_termoutprint) */

local int linebuf_start(linebuf *lbp) noex {
	cint	llen = linebufLEN ;
	int		rs ;
	if (lbp == nullptr) return SR_FAULT ;
	memclear(lbp) ;
	if (char *lbuf ; (rs = lm_mall((llen+1),&lbuf)) >= 0) {
	    lbp->lbuf = lbuf ;
	    lbp->llen = llen ;
	}
	return (rs >= 0) ? llen : rs ;
}
/* end subroutine (linebuf_start) */

local int linebuf_finish(linebuf *lbp) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (lbp == nullptr) return SR_FAULT ;
	if (lbp->lbuf != nullptr) {
	    rs1 = lm_free(lbp->lbuf) ;
	    if (rs >= 0) rs = rs1 ;
	    lbp->lbuf = nullptr ;
	}
	return rs ;
}
/* end subroutine (linebuf_finish) */

local int query_parse(struct query *qp,cchar *query) noex {
	cint	ulen = QUERY_USERPARTLEN ;
	cint	hlen = QUERY_HOSTPARTLEN ;
	int		rs = SR_OK ;
	int		cl ;
	int		ql = -1 ;
	int		ul = 0 ;
	cchar		*tp, *cp ;

	if (qp == nullptr) return SR_FAULT ;
	if (query == nullptr) return SR_FAULT ;

#if	CF_DEBUGS
	debugprintf("query_part: q=>%s<\n",query) ;
#endif

	qp->hpart[0] = '\0' ;
	qp->upart[0] = '\0' ;
	if ((tp = strchr(query,'@')) != nullptr) {
	    ql = (tp-query) ;
	    if ((cl = sfshrink((tp + 1),-1,&cp)) > 0) {
	        rs = snwcpy(qp->hpart,hlen,cp,cl) ;
	    }
	}

	if (rs >= 0) {
	    if ((cl = sfshrink(query,ql,&cp)) > 0) {
	        rs = snwcpy(qp->upart,ulen,cp,cl) ;
	        ul = rs ;
	    }
	} /* end if */

	if ((rs >= 0) && (qp->hpart[0] == '\0')) {
	    rs = sncpy1(qp->hpart,hlen,LOCALHOST) ;
	}

#if	CF_DEBUGS
	debugprintf("query_part: hpart=>%s<\n",qp->hpart) ;
	debugprintf("query_part: upart=>%s<\n",qp->upart) ;
	debugprintf("query_part: ret rs=%d ul=%u\n",rs,ul) ;
#endif

	return (rs >= 0) ? ul : rs ;
}
/* end subroutine (query_parse) */



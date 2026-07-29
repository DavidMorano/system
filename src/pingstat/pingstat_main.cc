/* pingstat_main SUPPORT (pingstat) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* generic (pretty much) front end program subroutine */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* compile-time debugging */
#define	CF_DEBUG	0		/* run-time debugging */
#define	CF_DEBUGMALL	1		/* debug memory-allocations */

/* revision history:

	= 2000-05-14, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Description:
	This subroutine forms the front-end part of a generic PCS
	type of program.  This front-end is used in a variety of
	PCS programs.

	This code was originally part of the Personal
	Communications Services (PCS) package but can also be used
	independently from it.  Historically, this was developed
	as part of an effort to maintain high function (and reliable)
	email communications in the face of increasingly draconian
	security restrictions imposed on the computers in the
	DEFINITY development organization.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/param.h>		/* POSIX® */
#include	<sys/stat.h>		/* POSIX® */
#include	<sys/socket.h>		/* POSIX® */
#include	<netinet/in.h>		/* POSIX® */
#include	<arpa/inet.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<netdb.h>		/* POSIX® */
#include	<ctime>			/* CSTD */
#include	<climits>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ascii.h>		/* LIBU */
#include	<getportnum.h>		/* LIBUC */
#include	<gethe.h>		/* LIBUC */
#include	<bits.h>		/* LIBUC */
#include	<keyopt.h>		/* LIBUC */
#include	<field.h>		/* LIBUC */
#include	<userinfo.h>		/* LIBUC */
#include	<vecstr.h>		/* LIBUC */
#include	<char.h>		/* LIBUC */
#include	<hostent.h>		/* LIBUC */
#include	<dater.h>		/* LIBUC */
#include	<sockaddress.h>		/* LIBUC */
#include	<lfm.h>			/* LIBUC */
#include	<exitcodes.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */
#include	<bfile.h>		/* LIBB */
#include	<libdebug.h>		/* LIBDEBUG */

#include	<proguserlist.h>
#include	<pinghost.h>
#include	<pingstatdb.h">
#include	<pingtab.h>

#include	"pingstat_config.h"
#include	"defs.h"
#include	"proglog.h"
#include	"configfile.h"


/* local defines */

#define	PI	proginfo
#define	AI	arginfo
#define	UI	userinfo

#define	NDF	"/tmp/pingstat.deb"


/* external subroutines */

extern int	printhelp(void *,cchar *,cchar *,cchar *) noex ;
extern int	proginfo_setpiv(PI *,cchar *,const PIVARS *) noex ;

extern int	progpingtabadd(PI *,cchar *,int) noex ;

extern int	progconf_begin(PI *) noex ;
extern int	progconf_end(PI *) noex ;
extern int	progconf_check(PI *) noex ;
extern int	progconf_read(PI *) noex ;

extern int	proginput(PI *,int) noex ;
extern int	progoutput(PI *,AI *,bits *) noex ;


/* externals variables */

extern cchar	pingstat_makedate[] ;


/* local structures */


/* forward references */

local int	usage(PI *) noex ;

local int	makedate_get(cchar *,cchar **) noex ;
local int	procopts(PI *,keyopt *) noex ;
local int	process(PI *,AI *,bits *,cchar *,cchar *) noex ;
local int	processor(PI *,AI *,bits *,cchar *,cchar *) noex ;
local int	processing(PI *,AI *,bits *,cchar *,cchar *) noex ;

local int	procpfname(PI *) noex ;
local int	procdfname(PI *) noex ;
local int	procpsdname(PI *) noex ;
local int	procdefportspec(PI *,char *,int,cchar *,cchar *,int) noex ;

local int	procuserinfo_begin(PI *,UI *) noex ;
local int	procuserinfo_end(PI *) noex ;

local int	proclocnames_begin(PI *) noex ;
local int	proclocnames_end(PI *) noex ;
local int	proclocnames_load(PI *) noex ;

local int	procsvars_begin(PI *) noex ;
local int	procsvars_end(PI *) noex ;
local int	procsvars_load(PI *) noex ;

local int	procourconf_begin(PI *) noex ;
local int	procourconf_end(PI *) noex ;

local int	procpid_begin(PI *) noex ;
local int	procpid_end(PI *) noex ;

#ifdef	COMMENT
local int	procuserinfo_logid(PI *) noex ;
#endif /* COMMENT */

local int	procdefs_begin(PI *) noex ;
local int	procdefs_end(PI *) noex ;
local int	procdefs_vardname(PI *) noex ;

local int	procsum_begin(PI *) noex ;
local int	procsum_end(PI *) noex ;


/* local variables */

enum argopts {
	argopt_tmpdir,
	argopt_version,
	argopt_verbose,
	argopt_root,
	argopt_logfile,
	argopt_config,
	argopt_help,
	argopt_sn,
	argopt_af,
	argopt_ef,
	argopt_of,
	argopt_if,
	argopt_lf,
	argopt_db,
	argopt_dgram,
	argopt_host,
	argopt_port,
	argopt_logextra,
	argopt_mu,
	argopt_overlast
} ; /* end enum */

constexpr cpcchar	argopts[] = {
	"ROOT",
	"TMPDIR",
	"VERSION",
	"VERBOSE",
	"LOGFILE",
	"CONFIG",
	"HELP",
	"sn",
	"af",
	"ef",
	"of",
	"if",
	"lf",
	"db",
	"dgram",
	"host",
	"port",
	"logextra",
	"mu",
	nullptr
} ; /* end array */

constexpr pivars	initvars = {
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
	{ 0, 0 },
} ; /* end array */

enum procopts {
	progopt_defintminping,
	progopt_intminping,
	progopt_intminupdate,
	progopt_marktime,
	progopt_logfile,
	progopt_sumfile,
	progopt_overlast
} ; /* end enum */

/* exported subroutines */
constexpr cpcchar	progopts[] = {
	"defintminping",
	"intminping",
	"intminupdate",
	"marktime",
	"logfile",
	"sumfile",
	nullptr
} ; /* end array */


/* exported variables */


/* exported subroutines */

int main(int argc,con mainv argv,con mainv envv) {
	PI		pi, *pip = &pi ;
	AI		ainfo, *aip = &ainfo ;
	bits		pargs ;
	keyopt		akopts ;
	bfile		errfile ;

#if	(CF_DEBUGS || CF_DEBUG) && CF_DEBUGMALL
	uint		mo_start = 0 ;
#endif

	int		argr, argl, aol, akl, avl, kwi ;
	int		ai, ai_max, ai_pos ;
	int		rs, rs1 ;
	int		v ;
	int		cl ;
	int		ex = EX_INFO ;
	int		f_optminus, f_optplus, f_optequal ;
	int		f_version = false ;
	int		f_makedate = false ;
	int		f_usage = false ;
	int		f_help = false ;

	cchar	*argp, *aop, *akp, *avp ;
	cchar	*argval = nullptr ;
	cchar	*pr = nullptr ;
	cchar	*sn = nullptr ;
	cchar	*afname = nullptr ;
	cchar	*efname = nullptr ;
	cchar	*ofname = nullptr ;
	cchar	*hfname = nullptr ;
	cchar	*cp ;

	opendefstds(3) ;

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

	if ((cp = getenv(VARBANNER)) == nullptr) cp = BANNER ;
	proginfo_setbanner(pip,cp) ;

/* initialize */

	pip->verboselevel = 1 ;
	pip->pagesize = getpagesize() ;

	pip->intrun = -1 ;
	pip->intmark = -1 ;
	pip->defintminping = -1 ;
	pip->intminping = -1 ;
	pip->intminupdate = -1 ;
	pip->intmininput = -1 ;
	pip->toping = -1 ;

	pip->fl.logprog = OPT_LOGPROG ;

	rs = pip->nowinit ;
	if (rs < 0) goto badinitnow ;

	if (rs >= 0)
	    rs = vecstr_start(&pip->pingtabs,0,0) ;

	if (rs < 0) {
	    ex = EX_OSERR ;
	    goto badtabstart ;
	}

/* start parsing the arguments */

	if (rs >= 0) rs = bits_start(&pargs,1) ;
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

#if	CF_DEBUGS
	    debugprintf("main: a=>%r<\n",argp,argl) ;
#endif

	    f_optminus = (*argp == '-') ;
	    f_optplus = (*argp == '+') ;
	    if ((argl > 1) && (f_optminus || f_optplus)) {
	        cint	ach = MKCHAR(argp[1]) ;

	        if (isdigitlatin(ach)) {

	            argval = (argp + 1) ;

	        } else if (ach == '-') {

	            aip->ai_pos = ai ;
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
	            debugprintf("main: ak=>%r<\n",akp,akl) ;
#endif

	            if ((kwi = matostr(argopts,2,akp,akl)) >= 0) {

	                switch (kwi) {

	                case argopt_tmpdir:
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

/* version */
	                case argopt_version:
	                    f_makedate = f_version ;
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

/* configuration file */
	                case argopt_config:
	                    if (f_optequal) {
	                        f_optequal = false ;
	                        if (avl)
	                            pip->cfname = avp ;
	                    } else {
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl)
	                                pip->cfname = argp ;
	                        } else
	                            rs = SR_INVALID ;
	                    }
	                    break ;

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

/* argument-list filename */
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

/* output filename */
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

	                case argopt_db:
	                    if (argr > 0) {
	                        argp = argv[++ai] ;
	                        argr -= 1 ;
	                        argl = strlen(argp) ;
	                        if (argl)
	                            pip->dfname = argp ;
	                    } else
	                        rs = SR_INVALID ;
	                    break ;

/* log file */
	                case argopt_logfile:
	                    if (f_optequal) {
	                        f_optequal = false ;
	                        if (avl) {
	                            pip->lfname = avp ;
	                        }
	                    } else {
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl) {
	                                pip->lfname = argp ;
	                            }
	                        } else
	                            rs = SR_INVALID ;
	                    }
	                    break ;

	                case argopt_dgram:
	                    pip->fval.dgram = true ;
	                    pip->fl.dgram = true ;
	                    if (f_optequal) {
	                        f_optequal = false ;
	                        if (avl) {
	                            rs = optbool(avp,avl) ;
	                            pip->fl.dgram = (rs > 0) ;
	                        }
	                    }
	                    break ;

	                case argopt_help:
	                    f_help = true ;
	                    break ;

	                case argopt_host:
	                    if (argr > 0) {
	                        argp = argv[++ai] ;
	                        argr -= 1 ;
	                        argl = strlen(argp) ;
	                        if (argl)
	                            pip->hostspec = argp ;
	                    } else
	                        rs = SR_INVALID ;
	                    break ;

	                case argopt_port:
	                    if (argr > 0) {
	                        argp = argv[++ai] ;
	                        argr -= 1 ;
	                        argl = strlen(argp) ;
	                        if (argl)
	                            pip->portspec = argp ;
	                    } else
	                        rs = SR_INVALID ;
	                    break ;

	                case argopt_logextra:
	                    pip->fl.logextra = true ;
	                    if (f_optequal) {
	                        f_optequal = false ;
	                        if (avl) {
	                            rs = optbool(avp,avl) ;
	                            pip->fl.logextra = (rs > 0) ;
	                        }
	                    }
	                    break ;

	                case argopt_mu:
	                    if (argr > 0) {
	                        argp = argv[++ai] ;
	                        argr -= 1 ;
	                        argl = strlen(argp) ;
	                        if (argl) {
	                            rs = cfdecti(argp,argl,&v) ;
	                            pip->intminupdate = v ;
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

#if	CF_DEBUGS
	                    debugprintf("main: kc=>%c< (%u)\n",kc,kc) ;
#endif

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

/* configuration file */
	                    case 'C':
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl)
	                                pip->cfname = argp ;
	                        } else
	                            rs = SR_INVALID ;
	                        break ;

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

/* mutex lock PID file */
	                    case 'P':
	                        pip->have.pfname = true ;
	                        pip->fval.pfname = true ;
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl)
	                                pip->pfname = argp ;
	                        } else
	                            rs = SR_INVALID ;
	                        break ;

	                    case 'Q':
	                        pip->fl.quiet = true ;
	                        break ;

/* version */
	                    case 'V':
	                        f_makedate = f_version ;
	                        f_version = true ;
	                        break ;

/* binary dump output file */
	                    case 'b':
	                        pip->fl.binary = true ;
	                        break ;

	                    case 'd':
	                        pip->fl.daemon = true ;
	                        if (f_optequal) {
	                            f_optequal = false ;
	                            if (avl) {
	                                rs = cfdecti(avp,avl,&v) ;
	                                pip->intrun = v ;
	                            }
	                        }
	                        break ;

/* hostspec */
	                    case 'h':
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl)
	                                pip->hostspec = argp ;
	                        } else
	                            rs = SR_INVALID ;
	                        break ;

/* input mode */
	                    case 'i':
	                        pip->fl.input = true ;
	                        if (f_optequal) {
	                            f_optequal = false ;
	                            if (avl) {
	                                rs = cfdecti(avp,avl,&v) ;
	                                pip->intmininput = v ;
	                            }
	                        }
	                        break ;

/* default ping-interval */
	                    case 'm':
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl) {
	                                rs = cfdecti(argp,argl,&v) ;
	                                pip->intminping = v ;
	                            }
	                        } else
	                            rs = SR_INVALID ;
	                        break ;

/* no output */
	                    case 'n':
	                        pip->fl.nooutput = true ;
	                        break ;

/* options */
	                    case 'o':
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl) {
	                                rs = keyopt_loads(&akopts,argp,argl) ;
	                            }
	                        } else
	                            rs = SR_INVALID ;
	                        break ;

/* accummulate the 'pingtab' files */
	                    case 'p':
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl) {
	                                rs = progpingtabadd(pip,argp,argl) ;
	                            }
	                        } else
	                            rs = SR_INVALID ;
	                        break ;

/* quiet mode */
	                    case 'q':
	                        pip->verboselevel = 0 ;
	                        break ;

/* receive mode */
	                    case 'r':
	                        pip->fl.receive = true ;
	                        break ;

/* 'pingstat' file name */
	                    case 's':
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl)
	                                pip->dfname = argp ;
	                        } else
	                            rs = SR_INVALID ;
	                        break ;

/* over-ride ping-timeout */
	                    case 't':
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl) {
	                                rs = cfdecti(argp,argl,&v) ;
	                                if (v > 0) {
	                                    pip->have.toping = true ;
	                                    pip->fval.toping = true ;
	                                    pip->toping = v ;
	                                }
	                            }
	                        } else
	                            rs = SR_INVALID ;
	                        break ;

/* update mode */
	                    case 'u':
	                        pip->fl.update = true ;
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

#if	CF_DEBUGS
	debugprintf("main: ai=%u\n",ai) ;
	debugprintf("main: ai_max=%u\n",ai_max) ;
	debugprintf("main: ai_pos=%u\n",ai_pos) ;
#endif

	if (efname == nullptr) efname = getenv(VAREFNAME) ;
	if (efname == nullptr) efname = getenv(VARERRORFNAME) ;
	if (efname == nullptr) efname = BFILE_STDERR ;
	if ((rs1 = bopen(&errfile,efname,"wca",0666)) >= 0) {
	    pip->efp = &errfile ;
	    pip->open.errfile = true ;
	    bcontrol(&errfile,BC_SETBUFLINE,true) ;
	} else if (! isFailOpen(rs1)) {
	    if (rs >= 0) rs = rs1 ;
	}

	if (rs < 0)
	    goto badarg ;

#if	CF_DEBUGS
	debugprintf("main: debuglevel=%u¹\n",pip->debuglevel) ;
#endif

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    debugprintf("main: debuglevel=%u²\n",pip->debuglevel) ;
#endif

	if (f_version) {
	    cchar	*pn = pip->progname ;
	    bprintf(pip->efp,"%s: version %s\n",pn,VERSION) ;
	    if (f_makedate) {
	        cl = makedate_get(pingstat_makedate,&cp) ;
	        bprintf(pip->efp,"%s: makedate %r\n",pn,cp,cl) ;
	    }
	}

/* get our program root */

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    debugprintf("main: con0\n") ;
#endif

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
	    bprintf(pip->efp,"%s: pr=%s\n", pip->progname,pip->pr) ;
	    bprintf(pip->efp,"%s: sn=%s\n", pip->progname,pip->searchname) ;
	} /* end if */

	if (f_usage)
	    usage(pip) ;

/* help */

	if (f_help) {
	    if ((hfname == nullptr) || (hfname[0] == '\0')) {
	        hfname = HELPFNAME ;
	    }
	    printhelp(nullptr,pip->pr,pip->searchname,hfname) ;
	} /* end if (help) */

	if (f_version || f_help || f_usage)
	    goto retearly ;


	ex = EX_OK ;

/* more option initialization */

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    debugprintf("main: con1\n") ;
#endif

	if ((rs >= 0) && (pip->n == 0) && (argval != nullptr)) {
	    rs = optvalue(argval,-1) ;
	    pip->n = rs ;
	}

	if (afname == nullptr) afname = getenv(VARAFNAME) ;

	if (rs >= 0) {
	    rs = procopts(pip,&akopts) ;
	}

	if (rs < 0) {
	    ex = EX_USAGE ;
	    goto retearly ;
	}

/* argument defaults */

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    debugprintf("main: con2\n") ;
#endif

/* get some host/user information */

	uc_sigignore(SIGPIPE) ;

	memset(&ainfo,0,sizeof(ARGINFO)) ;
	ainfo.argc = argc ;
	ainfo.ai = ai ;
	ainfo.argv = argv ;
	ainfo.ai_max = ai_max ;
	ainfo.ai_pos = ai_pos ;

	if (rs >= 0) {
	    UI	u ;
	    if ((rs = userinfo_start(&u,nullptr)) >= 0) {
	        if ((rs = procuserinfo_begin(pip,&u)) >= 0) {
	            if ((rs = proglog_begin(pip,&u)) >= 0) {
	                if ((rs = proguserlist_begin(pip)) >= 0) {
	                    {
	                        ARGINFO	*aip = &ainfo ;
	                        bits	*bop = &pargs ;
	                        cchar	*afn = afname ;
	                        cchar	*ofn = ofname ;
	                        rs = process(pip,aip,bop,ofn,afn) ;
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
	        cchar	*fmt = "%s: userinfo failure (%d)\n" ;
	        ex = EX_NOUSER ;
	        bprintf(pip->efp,fmt,pn,rs) ;
	    } /* end if (userinfo) */
	} else if (ex == EX_OK) {
	    cchar	*pn = pip->progname ;
	    cchar	*fmt = "%s: invalid argument or configuration (%d)\n" ;
	    ex = EX_USAGE ;
	    bprintf(pip->efp,fmt,pn,rs) ;
	    usage(pip) ;
	}

/* done */
	if ((rs < 0) && (ex == EX_OK)) {
	    switch (rs) {
	    case SR_INVALID:
	        ex = EX_USAGE ;
	        if (! pip->fl.quiet) {
	            bprintf(pip->efp,"%s: invalid query (%d)\n",
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
	    if (pip->fl.hostdown) ex = EX_TEMPFAIL ;
	} /* end if */

retearly:
	if (pip->debuglevel > 0) {
	    bprintf(pip->efp,"%s: exiting ex=%u (%d)\n",
	        pip->progname,ex,rs) ;
	}

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    debugprintf("main: exiting ex=%u rs=%d\n",ex,rs) ;
#endif

	if (pip->efp != nullptr) {
	    pip->open.errfile = false ;
	    bclose(pip->efp) ;
	    pip->efp = nullptr ;
	}

	if (pip->open.akopts) {
	    pip->open.akopts = false ;
	    keyopt_finish(&akopts) ;
	}

	bits_finish(&pargs) ;

badpargs:

#if	CF_DEBUG
	if (DEBUGLEVEL(2)) {
	    int		i ;
	    cchar	*cp ;
		    debugprintf("main: pingtabs¬\n") ;
	    for (i = 0 ; vecstr_get(&pip->pingtabs,i,&cp) >= 0 ; i += 1) {
		if (cp != nullptr) {
		    debugprintf("main: pt=%s\n",cp) ;
		}
	    }
	}
#endif /* CF_DEBUG */

	vecstr_finish(&pip->pingtabs) ;

badtabstart:
badinitnow:
	proginfo_finish(pip) ;

badprogstart:

#if	(CF_DEBUGS || CF_DEBUG) && CF_DEBUGMALL
	{
	    uint	mi[12] ;
	    uint	mo ;
	    uint	mdiff ;
	    uc_mallout(&mo) ;
	    mdiff = (mo-mo_start) ;
	    debugprintf("main: fval mallout=%u\n",mdiff) ;
	    if (mdiff > 0) {
	        UCMALLREG_CUR	cur ;
	        UCMALLREG_REG	reg ;
	        cint	size = (10*sizeof(uint)) ;
	        cchar		*ids = "main" ;
	        uc_mallinfo(mi,size) ;
	        debugprintf("main: MIoutnum=%u\n",mi[ucmallreg_outnum]) ;
	        debugprintf("main: MIoutnummax=%u\n",
	            mi[ucmallreg_outnummax]) ;
	        debugprintf("main: MIoutsize=%u\n",mi[ucmallreg_outsize]) ;
	        debugprintf("main: MIoutsizemax=%u\n",
	            mi[ucmallreg_outsizemax]) ;
	        debugprintf("main: MIused=%u\n",mi[ucmallreg_used]) ;
	        debugprintf("main: MIusedmax=%u\n",mi[ucmallreg_usedmax]) ;
	        debugprintf("main: MIunder=%u\n",mi[ucmallreg_under]) ;
	        debugprintf("main: MIover=%u\n",mi[ucmallreg_over]) ;
	        debugprintf("main: MInotalloc=%u\n",mi[ucmallreg_notalloc]) ;
	        debugprintf("main: MInotfree=%u\n",mi[ucmallreg_notfree]) ;
	        ucmallreg_curbegin(&cur) ;
	        while (ucmallreg_curenum(&cur,&reg) >= 0) {
	            debugprintf("main: MIreg.addr=%p\n",reg.addr) ;
	            debugprintf("main: MIreg.size=%u\n",reg.size) ;
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

/* bad argument usage */
badarg:
	ex = EX_USAGE ;
	bprintf(pip->efp,"%s: invalid argument specified (%d)\n",
	    pip->progname,rs) ;
	usage(pip) ;
	goto retearly ;

}
/* end subroutine (main) */


/* local subroutines */

local int usage(PI *pip) noex {
	int		rs = SR_OK ;
	int		wlen = 0 ;
	cchar	*pn = pip->progname ;
	cchar	*fmt ;

	fmt = "%s: USAGE> %s [<host(s)>] [-p <pingtab(s)>] [-q]\n" ;
	if (rs >= 0) rs = bprintf(pip->efp,fmt,pn,pn) ;
	wlen += rs ;

	fmt = "%s:  [-u] [-R <pr>] [-C <conf>] [-P <pidmutex>] [-db <db>]\n" ;
	if (rs >= 0) rs = bprintf(pip->efp,fmt,pn) ;
	wlen += rs ;

	fmt = "%s:  [-Q] [-D] [-v[=<n>]] [-HELP] [-V]\n" ;
	if (rs >= 0) rs = bprintf(pip->efp,fmt,pn) ;
	wlen += rs ;

	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (usage) */

/* get the date out of the ID string */
local int makedate_get(cchar *md,cchar **rpp) noex {
	int		ch ;
	cchar	*sp ;
	cchar	*cp ;

	if (rpp != nullptr) *rpp = nullptr ;

	if ((cp = strchr(md,CH_RPAREN)) == nullptr)
	    return SR_NOENT ;

	while (CHAR_ISWHITE(*cp))
	    cp += 1 ;

	ch = MKCHAR(*cp) ;
	if (! isdigitlatin(ch)) {
	    while (*cp && (! CHAR_ISWHITE(*cp))) cp += 1 ;
	    while (CHAR_ISWHITE(*cp)) cp += 1 ;
	} /* end if (skip over the name) */

	sp = cp ;
	if (rpp != nullptr)
	    *rpp = cp ;

	while (*cp && (! CHAR_ISWHITE(*cp)))
	    cp += 1 ;

	return (cp - sp) ;
} /* end subroutine (makedate_get) */

/* process the program ako-options */
local int procopts(PI *pip,keyopt *kop) noex {
	int		rs = SR_OK ;
	int		c = 0 ;
	cchar	*cp ;

	if ((cp = getenv(VAROPTS)) != nullptr) {
	    rs = keyopt_loads(kop,cp,-1) ;
	}

	if (rs >= 0) {
	    keyopt_cur	kcur ;
	    if ((rs = keyopt_curbegin(kop,&kcur)) >= 0) {
	        int	v ;
	        int	oi ;
	        int	kl, vl ;
	        cchar	*kp, *vp ;

	        while ((kl = keyopt_enumkeys(kop,&kcur,&kp)) >= 0) {

	            if ((oi = matostr(progopts,2,kp,kl)) >= 0) {

	                vl = keyopt_fetch(kop,kp,nullptr,&vp) ;

	                switch (oi) {
	                case progopt_marktime:
	                    if (! pip->fval.marktime) {
	                        pip->have.marktime = true ;
	                        pip->fval.marktime = true ;
	                        pip->fl.marktime = true ;
	                        if (vl > 0) {
	                            rs = optbool(vp,vl) ;
	                            pip->fl.marktime = (rs > 0) ;
	                        }
	                    }
	                    break ;
	                case progopt_defintminping:
	                case progopt_intminping:
	                case progopt_intminupdate:
	                    if (vl > 0) {
	                        if ((rs = cfdecti(vp,vl,&v)) >= 0) {
	                            switch (oi) {
	                            case progopt_defintminping:
	                                if (! pip->fval.defintminping) {
	                                    pip->have.defintminping = true ;
	                                    pip->fval.defintminping = true ;
	                                    pip->fl.defintminping = true ;
	                                    pip->defintminping = v ;
	                                }
	                                break ;
	                            case progopt_intminping:
	                                if (! pip->fval.intminping) {
	                                    pip->have.intminping = true ;
	                                    pip->fval.intminping = true ;
	                                    pip->fl.intminping = true ;
	                                    pip->intminping = v ;
	                                }
	                                break ;
	                            case progopt_intminupdate:
	                                if (! pip->fval.intminupdate) {
	                                    pip->have.intminupdate = true ;
	                                    pip->fval.intminupdate = true ;
	                                    pip->fl.intminupdate = true ;
	                                    pip->intminupdate = v ;
	                                }
	                                break ;
	                            } /* end switch */
	                        } /* end if (cfdecti) */
	                    } /* end if (positive) */
	                    break ;
	                case progopt_sumfile:
	                    break ;
	                case progopt_logfile:
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
} /* end subroutine (procopts) */

local int process(PI *pip,AI *aip,bits *bop,cchar *ofn,cchar *afn) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = proclocnames_begin(pip)) >= 0) {
	    if ((rs = procsvars_begin(pip)) >= 0) {
	        if ((rs = progconf_begin(pip)) >= 0) {
	            if ((rs = procourconf_begin(pip)) >= 0) {
	                {
	                    rs = processor(pip,aip,bop,ofn,afn) ;
	                }
	                rs1 = procourconf_end(pip) ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (procoutconf) */
	            rs1 = progconf_end(pip) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (progconf_end) */
	        rs1 = procsvars_end(pip) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (procsvars) */
	    rs1 = proclocnames_end(pip) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (proclocnames) */
	return rs ;
} /* end subroutine (process) */

local int processor(PI *pip,AI *aip,bits *bop,cchar *ofn,cchar *afn) noex {
	int		rs ;
	int		rs1 ;

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("main/processor: ent\n") ;
#endif

	if ((rs = procdefs_begin(pip)) >= 0) {
	    if ((rs = procpid_begin(pip)) >= 0) {
	        if ((rs = procsum_begin(pip)) >= 0) {
	            const mode_t	om = 0666 ;
	            cint		of = (O_RDWR | O_CREAT) ;
	            cchar		*dfname = pip->dfname ;
#if	CF_DEBUG
	            if (DEBUGLEVEL(4))
	                debugprintf("main/processor: dfn=%s\n",dfname) ;
#endif
	            if ((rs = pingstatdb_open(&pip->ps,dfname,of,om)) >= 0) {
	                {
	                    rs = processing(pip,aip,bop,ofn,afn) ;
	                }
	                rs1 = pingstatdb_close(&pip->ps) ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (pingstatdb) */
	            rs1 = procsum_end(pip) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (procsum) */
	        rs1 = procpid_end(pip) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (procpid) */
	    rs1 = procdefs_end(pip) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (procdefs) */

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("main/processor: ret rs=%d\n",rs) ;
#endif

	return rs ;
} /* end subroutine (processor) */

local int processing(PI *pip,AI *aip,bits *bop,cc *ofn,cchar *afn) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	cchar		*fmt ;

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("main/processing: ent\n") ;
#endif

/* OK, now do whatever needed for the particular program mode we're in */

	if (pip->fl.input) {
	    int		fd = FD_STDIN ;
	    if ((! pip->fl.daemon) && (! pip->fl.dgram) && isasocket(fd)) {
		if ((rs = uc_getsocktype(fd)) >= 0) {
	            pip->fl.dgram = (rs == SOCK_DGRAM) ;
		}
	    } /* end if (data-gram determination) */

	    if (pip->debuglevel > 0)
	        bprintf(pip->efp,"%s: mode=input\n", pip->progname) ;

	    if (pip->open.logprog) {
	        fmt = (pip->fl.dgram) ? "mode=input (dgram)" : "mode=input" ;
	        proglog_printf(pip,fmt) ;
	    }

	    if ((rs >= 0) && pip->fl.daemon) {
	        cchar	*dps = PORTSPEC_PINGSTAT ;
	        cchar	*protoname = "udp" ;
	        cint	af = AF_INET ;
	        cint	dpn = PORT_PINGSTAT ;

	        if (pip->debuglevel > 0)
	            bprintf(pip->efp,"%s: server\n", pip->progname) ;

	        if (pip->open.logprog)
	            logfile_printf(&pip->lh,"server") ;

	        pip->fl.dgram = true ;
	        if (pip->debuglevel == 0) {

	            bflush(pip->efp) ;

	            logfile_flush(&pip->lh) ;

	            if ((rs = uc_fork()) == 0) {
	                int	i ;
	                for (i = 0 ; i < 3 ; i += 1) u_close(i) ;
	                u_setsid() ;
	            } else if (rs > 0) {
	                uc_exit(EX_OK) ;
		    }

	        } /* end if */

	        if (rs >= 0) {
	            char	psbuf[SVCNAMELEN + 1] ;

	            if ((pip->portspec == nullptr) || (pip->portspec[0] == '\0'))
	                pip->portspec = dps ;

	            rs = procdefportspec(pip,psbuf,SVCNAMELEN,protoname,
	                pip->portspec,dpn) ;

	            if (rs >= 0) {
	                rs = listenudp(af,pip->hostspec,psbuf,0) ;
	                fd = rs ;
	            }
	        } /* end if */

	    } /* end if (daemon) */

#if	CF_DEBUG
	    if (DEBUGLEVEL(4))
	        debugprintf("main: proginput() \n") ;
#endif

	    if (rs >= 0) {
	        rs = proginput(pip,fd) ;
	    }

#if	CF_DEBUG
	    if (DEBUGLEVEL(4))
	        debugprintf("main: proginput() rs=%d\n",rs) ;
#endif

	    if (pip->open.logprog)
	        logfile_printf(&pip->lh,"exiting (%d)",rs) ;

	} else {

#if	CF_DEBUG
	    if (DEBUGLEVEL(4))
	        debugprintf("main: progoutput() \n") ;
#endif

	    pip->afname = afn ;
	    pip->ofname = ofn ;
	    rs = progoutput(pip,aip,bop) ;

#if	CF_DEBUG
	    if (DEBUGLEVEL(4))
	        debugprintf("main: progoutput() rs=%d\n",rs) ;
#endif

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    debugprintf("main: total hosts=%u processed=%u up=%u\n",
	        pip->c_hosts,pip->c_processed,pip->c_up) ;
#endif

	    if (rs >= 0) {
		cchar	*pn = pip->progname ;
		cchar	*fmt ;

	        if (pip->debuglevel > 0) {
		    fmt = "%s: total hosts=%u processed=%u up=%u\n" ;
	            bprintf(pip->efp,fmt,pn,
	                pip->c_hosts,pip->c_processed,pip->c_up) ;
		    fmt = "%s: pingtabs=%u\n" ;
	            bprintf(pip->efp,fmt,pn,pip->c_pingtabs) ;
	        } /* end if */

		fmt = "hosts=%u processed=%u up=%u\n" ;
	        logfile_printf(&pip->lh,fmt,
	            pip->c_hosts,pip->c_processed,pip->c_up) ;

	    } /* end if (not in input-mode) */

	} /* end if (program mode) */

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("main/processing: ret rs=%d\n",rs) ;
#endif

	return rs ;
} /* end subroutine (processing) */

local int procpfname(PI *pip) noex {
	int		rs = SR_OK ;
	if ((pip->pfname == nullptr) || (pip->pfname[0] == '\0')) {
	    cchar	*sn = pip->searchname ;
	    char	tbuf[MAXPATHLEN+1] ;
	    if ((rs = mkpath4(tbuf,pip->pr,VDNAME,sn,PIDFNAME)) >= 0) {
	        cchar	**vpp = &pip->pfname ;
	        rs = proginfo_setentry(pip,vpp,tbuf,rs) ;
	    }
	}
	return rs ;
} /* end subroutine (procpfname) */

local int procdfname(PI *pip) noex {
	int		rs = SR_OK ;
	if ((pip->dfname == nullptr) || (pip->dfname[0] == '\0')) {
	    cchar	*sn = pip->searchname ;
	    char	tbuf[MAXPATHLEN+1] ;
	    if ((rs = mkpath4(tbuf,pip->pr,VDNAME,sn,PSFNAME)) >= 0) {
	        cchar	**vpp = &pip->dfname ;
	        rs = proginfo_setentry(pip,vpp,tbuf,rs) ;
	    }
	}
#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("main/procdfname: ret rs=%d\n",rs) ;
#endif
	return rs ;
} /* end subroutine (procdfname) */

local int procpsdname(PI *pip) noex {
	int		rs = SR_OK ;
	int		cl  ;
	int		f_created = false ;
	cchar		*dfname = pip->dfname ;
	cchar	*cp ;

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("main/procpsdname: ent dfn=%s\n",dfname) ;
#endif

	if (dfname != nullptr) {
	    if ((cl = sfdirname(dfname,-1,&cp)) > 0) {
	        char	tbuf[MAXPATHLEN + 1] ;
	        if ((rs = mkpath1w(tbuf,cp,cl)) >= 0) {
	            USTAT	sb ;
	            if ((rs = u_stat(tbuf,&sb)) == SR_NOENT) {
	                const mode_t	dm = 0777 ;
	                f_created = true ;
	                if ((rs = mkdirs(tbuf,dm)) >= 0) {
	                    if ((rs = uc_minmod(tbuf,dm)) >= 0) {
	                        rs = chownsame(tbuf,pip->pr) ;
	                    }
	                }
	            }
	        }
	    }
	} else {
	    rs = SR_NOENT ;
	}

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("main/procpsdname: ret rs=%d f=%u\n",rs,f_created) ;
#endif

	return (rs >= 0) ? f_created : rs ;
} /* end subroutine (procpsdname) */

local int procdefportspec(PI *pip,char *psbuf,int pslen,
		cchar *protoname,cchar *dps,int dpn) noex {
	int		rs ;
	if (pip == nullptr) return SR_FAULT ;
	if ((rs = getportnum(protoname,dps)) >= 0) {
	    rs = sncpy1(psbuf,pslen,dps) ;
	} else if (rs == SR_NOENT) {
	    rs = ctdeci(psbuf,pslen,dpn) ;
	}
	return rs ;
} /* end subroutine (procdefportspec) */

local int proclocnames_begin(PI *pip) noex {
	vecstr		*lnp = &pip->localnames ;
	int		rs ;
	if ((rs = vecstr_start(lnp,5,0)) >= 0) {
	    rs = proclocnames_load(pip) ;
	    if (rs < 0) {
	        vecstr_finish(lnp) ;
	    }
	}
	return rs ;
} /* end subroutine (proclocnames_begin) */

local int proclocnames_end(PI *pip) noex {
	vecstr		*lnp = &pip->localnames ;
	int		rs = SR_OK ;
	int		rs1 ;
	{
	    rs1 = vecstr_finish(lnp) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
} /* end subroutine (proclocnames_end) */

local int proclocnames_load(PI *pip) noex {
	cint		hlen = MAXPATHLEN ;
	int		rs ;
	int		rs1 ;
	int		n = 0 ;
	char		hbuf[MAXHOSTNAMELEN + 1] ;
	char		*hebuf ;

	snsds(hbuf,hlen,pip->nodename,pip->domainname) ;

	if ((rs = malloc_ho(&hebuf)) >= 0) {
	    ucentho	he ;
	    vecstr	*lnp = &pip->localnames ;
	    cint	helen = rs ;
	    cchar	*hnp ;
	    for (int i = 0 ; i < 2 ; i += 1) {
	        hnp = (i == 0) ? pip->nodename : hbuf ;
	        if ((rs1 = gethename(&he,hebuf,helen,hnp)) >= 0) {
	            hostent_cur		cur ;
	            if ((rs = hostent_curbegin(&he,&cur)) >= 0) {
	                cint	rsn = SR_NOTFOUND ;
	    		cchar	*sp ;
	                while ((rs = hostent_curenumname(&he,&cur,&sp)) > 0) {
	                    if ((rs = vecstr_find(lnp,sp)) == rsn) {
	                        rs = vecstr_add(lnp,sp,-1) ;
	                    }
	                    if (rs < 0) break ;
	                } /* end while */
	                rs1 = hostent_curend(&he,&cur) ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (hostend-cur) */
	        } /* end if (gethename) */
	        if (rs < 0) break ;
	    } /* end for */
	    if (rs >= 0) {
	        n = vecstr_count(lnp) ;
	    }
	    rs1 = uc_free(hebuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (memory-allocation) */

	return (rs >= 0) ? n : rs ;
} /* end subroutine (proclocnames_load) */

local int procsvars_begin(PI *pip) noex {
	vecstr		*slp = &pip->svars ;
	int		rs ;
	if ((rs = vecstr_start(slp,3,0)) >= 0) {
	    pip->open.svars = true ;
	    rs = procsvars_load(pip) ;
	    if (rs < 0) {
		pip->open.svars = false ;
	        vecstr_finish(slp) ;
	    }
	}
	return rs ;
} /* end subroutine (procsvars_begin) */

local int procsvars_end(PI *pip) noex {
	vecstr		*slp = &pip->svars ;
	int		rs = SR_OK ;
	int		rs1 ;
	if (pip->open.svars) {
	    pip->open.svars = false ;
	    rs1 = vecstr_finish(slp) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
} /* end subroutine (procsvars_end) */

local int procsvars_load(PI *pip) noex {
	vecstr		*slp = &pip->svars ;
	int		rs = SR_OK ;
	int		i ;
	int		vl ;
	cchar		keys[] = "pen" ;
	cchar		*vp ;
	char		kbuf[2] = { 0, 0 } ;
	for (i = 0 ; keys[i] != '\0' ; i += 1) {
	    cint	kch = MKCHAR(keys[i]) ;
	    vp = nullptr ;
	    vl = -1 ;
	    switch (kch) {
	    case 'p':
	        vp = pip->pr ;
	        break ;
	    case 'e':
	        vp = "etc" ;
	        break ;
	    case 'n':
	        vp = pip->searchname ;
	        break ;
	    } /* end switch */
	    if ((rs >= 0) && (vp != nullptr)) {
	        kbuf[0] = kch ;
	        rs = vecstr_envadd(slp,kbuf,vp,vl) ;
	    }
	    if (rs < 0) break ;
	} /* end for */
	return rs ;
} /* end subroutine (procsvars_load) */

local int procourconf_begin(PI *pip) noex {
	int		rs = SR_OK ;

	if (pip->toping < 0)
	    pip->toping = TO_PING ;

	if (pip->defintminping < 0)
	    pip->defintminping = INTMINPING ;

	if (pip->intmininput < 0)
	    pip->intmininput = INTMININPUT ;

	if (pip->intminupdate < 0)
	    pip->intminupdate = INTMINUPDATE ;

#if	CF_DEBUG
	if (DEBUGLEVEL(2)) {
	    debugprintf("main: toping=%d\n",pip->toping) ;
	    debugprintf("main: intminping=%d\n",pip->intminping) ;
	    debugprintf("main: intmininput=%d\n",pip->intmininput) ;
	    debugprintf("main: intminupdate=%d\n",pip->intminupdate) ;
	}
#endif

	if (pip->debuglevel > 0) {
	    bprintf(pip->efp,"%s: toping=%d\n",
	        pip->progname,pip->toping) ;
	    bprintf(pip->efp,"%s: intminping=%d\n",
	        pip->progname,pip->intminping) ;
	    bprintf(pip->efp,"%s: intminupdate=%d\n",
	        pip->progname,pip->intminupdate) ;
	}

	if (pip->workdname == nullptr) {
	    pip->workdname = WORKDNAME ;
	} else if (pip->workdname[0] == '\0') {
	    pip->workdname = "." ;
	}

/* TMP directory */

	if ((pip->tmpdname == nullptr) || (pip->tmpdname[0] == '\0'))
	    pip->tmpdname = getenv(VARTMPDNAME) ;

	if ((pip->tmpdname == nullptr) || (pip->tmpdname[0] == '\0'))
	    pip->tmpdname = TMPDNAME ;

/* can we access the working directory? */

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    debugprintf("main: access working directory \"%s\"\n",
	        pip->workdname) ;
#endif

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	    debugprintf("main: 2 dfname=%s\n",pip->dfname) ;
#endif

	if (pip->debuglevel > 0) {
	    bprintf(pip->efp,"%s: db=%s\n",pip->progname,pip->dfname) ;
	}

	return rs ;
} /* end subroutine (procourconf_begin) */

local int procourconf_end(PI *pip) noex {
	int		rs = SR_OK ;
	if (pip == nullptr) return SR_FAULT ;
	return rs ;
} /* end subroutine (procourconf_end) */

local int procpid_begin(PI *pip) noex {
	int		rs = SR_OK ;
	if ((pip->pfname != nullptr) && (pip->pfname[0] != '\0')) {

#if	CF_DEBUG
	    if (DEBUGLEVEL(4))
	        debugprintf("main/procpid_begin: PIDFNAME=%s\n",pip->pfname) ;
#endif

	    logfile_printf(&pip->lh,"pidfile=%s\n",pip->pfname) ;

	    logfile_printf(&pip->lh,"PID mutex captured\n") ;

	} /* end if (we have a mutex PID file) */
#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("main/procpid_begin: ret rs=%d\n",rs) ;
#endif
	return rs ;
} /* end subroutine (procpid_begin) */

local int procpid_end(PI *pip) noex {
	int		rs = SR_OK ;
	if (pip == nullptr) return SR_FAULT ;
	return rs ;
} /* end subroutine (procpid_end) */

local int procuserinfo_begin(PI *pip,UI *uip) noex {
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
	pip->ppid = pip->pid ;
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
	    cint	glen = GROUPNAMELEN ;
	    char	gbuf[GROUPNAMELEN + 1] ;
	    if ((rs = getgroupname(gbuf,glen,pip->gid)) >= 0) {
	        cchar	**vpp = &pip->groupname ;
	        rs = proginfo_setentry(pip,vpp,gbuf,rs) ;
	    }
	}

#ifdef	COMMENT
	if (rs >= 0) {
	    rs = procuserinfo_logid(pip) ;
	} /* end if (ok) */
#endif /* COMMENT */

	return rs ;
} /* end subroutine (procuserinfo_begin) */

local int procuserinfo_end(PI *pip) noex {
	int		rs = SR_OK ;
	if (pip == nullptr) return SR_FAULT ;
	return rs ;
} /* end subroutine (procuserinfo_end) */

#ifdef	COMMENT
local int procuserinfo_logid(PI *pip) noex {
	int		rs ;
	if ((rs = lib_runmode()) >= 0) {
#if	CF_DEBUG
	    if (DEBUGLEVEL(4))
	        debugprintf("procuserinfo_logid: rm=%08ß\n",rs) ;
#endif
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
} /* end subroutine (procuserinfo_logid) */
#endif /* COMMENT */

local int procdefs_begin(PI *pip) noex {
	int		rs ;
	if ((rs = procdefs_vardname(pip)) >= 0) {
	    if ((rs = procpfname(pip)) >= 0) {
	        if ((rs = procdfname(pip)) >= 0) {
	            rs = procpsdname(pip) ;
	        }
	    }
	    proglog_printf(pip,"db=%s\n",pip->dfname) ;
	} /* end if (procdefs_vardname) */
	return rs ;
} /* end subroutine (procdefs_begin) */

local int procdefs_end(PI *pip) noex {
	if (pip == nullptr) return SR_FAULT ;
	return SR_OK ;
} /* end subroutine (procdefs_end) */

local int procdefs_vardname(PI *pip) noex {
	int		rs ;
	cchar		*pr = pip->pr ;
	cchar		*sn = pip->searchname ;
	char		tbuf[MAXPATHLEN+1] ;
	if ((rs = mkpath3(tbuf,pr,VDNAME,sn)) >= 0) {
	    USTAT	sb ;
	    cint	rsn = SR_NOTFOUND ;
	    if ((rs = uc_stat(tbuf,&sb)) == rsn) {
	        const mode_t	dm = 0777 ;
	        if ((rs = mkdirs(tbuf,dm)) >= 0) {
	            if ((rs = uc_minmod(tbuf,dm)) >= 0) {
	                rs = chownsame(tbuf,pip->pr) ;
	            }
	        }
	    } /* end if (uc_stat) */
	}
#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("main/procdefs_vardname: ret rs=%d\n",rs) ;
#endif
	return rs ;
} /* end subroutine (procdefs_vardname) */

/* open a summary file (if we have one) */
local int procsum_begin(PI *pip) noex {
	int		rs = SR_OK ;
#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("main/procsum_begin: ent\n") ;
#endif
	if ((pip->sumfname != nullptr) && (pip->sumfname[0] != '\0')) {
	    cint	esize = sizeof(bfile) ;
	    cchar	*pn = pip->progname ;
	    cchar	*fmt ;
	    char	*p ;
	    fmt = "%s: summary-file=%s\n" ;
	    bprintf(pip->efp,fmt,pn,pip->sumfname) ;
	    if ((rs = uc_malloc(esize,&p)) >= 0) {
	        cchar	*sfname = pip->sumfname ;
	        pip->sumfp = p ;
	        if ((rs = bopen(pip->sumfp,sfname,"wca",0666)) >= 0) {
	            pip->open.sumfile = true ;
	        }
	        if (rs < 0) {
	            uc_free(pip->sumfp) ;
	            pip->sumfp = nullptr ;
	        }
	    } /* end if (m-a) */
	}
#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("main/procsum_begin: ret rs=%d\n",rs) ;
#endif
	return rs ;
} /* end subroutine (procsum_begin) */

local int procsum_end(PI *pip) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (pip->sumfp != nullptr) {
	    rs1 = bclose(pip->sumfp) ;
	    if (rs >= 0) rs = rs1 ;
	    rs1 = uc_free(pip->sumfp) ;
	    if (rs >= 0) rs = rs1 ;
	    pip->sumfp = nullptr ;
	    pip->open.sumfile = false ;
	}
#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("main/procsum_end: ret rs=%d\n",rs) ;
#endif
	return rs ;
} /* end subroutine (procsum_end) */



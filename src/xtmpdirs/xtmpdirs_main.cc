/* xtmpdirs_main SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* XTMPDIRS (X-Windows Temporary-Directories) */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* switchable print-outs */
#define	CF_DEBUGMALL	1		/* debug memory-allocations */
#define	CF_DEFLOGSIZE	0		/* default log-size */

/* revision history:

	= 2011-03-17, David A­D­ Morano
	I created this -- after about needing it for 20 years --
	because I am tired of the little flaws in the X11 startup
	sequence.

*/

/* Copyright © 2011 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Descripion:
	This little program ensures that the proper directories are
	present, and with the correct permissions, in the system
	TMP directory in order for X11 to start up correctly.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/param.h>		/* POSIX® */
#include	<sys/stat.h>		/* POSIX® */
#include	<sys/socket.h>		/* POSIX® */
#include	<netinet/in.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<sysexits.h>		/* POSIX® */
#include	<csignal>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<getax.h>		/* LIBUC */
#include	<bits.h>		/* LIBUC */
#include	<keyopt.h>		/* LIBUC */
#include	<ids.h>			/* LIBUC */
#include	<userinfo.h>		/* LIBUC */
#include	<logfile.h>		/* LIBUC */
#include	<exitcodes.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */
#include	<libdebug.h>		/* LIBDEBUG |DEBUGPRINTF(3debug)| */
#include	<bfile.h>		/* LIBB */

#include	"xtmpdirs_config.h"
#include	"defs.h"
#include	"proglog.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#define	DIRMODE		0777

#ifndef	PI
#define	PI		progifo
#endif

#define	LI		locinfo
#define	LI_FL		locinfo_flags


/* external subroutines */

extern int	printhelp(bfile *,cchar *,cchar *,cchar *) noex ;
extern int	proginfo_setpiv(PI *,cchar *,const struct pivars *) noex ;

extern int	proguserlist_begin(PI *) noex ;
extern int	proguserlist_end(PI *) noex ;


/* external variables */


/* local structures */

struct locinfo_flags {
	uint		stores:1 ;
	uint		mids:1 ;
	uint		wl_system:1 ;
	uint		wl_local:1 ;
	uint		bl_system:1 ;
	uint		bl_local:1 ;
} ; /* end struct */

struct locinfo {
	LI_FL	have, f, changed, finval ;
	LI_FL	open ;
	PI	*pip ;
	vecstr		stores ;
} ; /* end struct */


/* local typedefs */


/* forward references */

local int	usage(PI *) noex ;

local int	procopts(PI *,keyopt *) noex ;
local int	proctmp(PI *) noex ;
local int	process(PI *) noex ;
local int	procdir(PI *,cchar *) noex ;
local int	procdirer(PI *,cchar *,mode_t) noex ;

local int	procuserinfo_begin(PI *,userinfo *) noex ;
local int	procuserinfo_end(PI *) noex ;

local int	locinfo_start(LI *,PI *) noex ;
local int	locinfo_finish(LI *) noex ;


/* local variables */

enum argopts {
	argopt_root,
	argopt_version,
	argopt_verbose,
	argopt_tmpdir,
	argopt_help,
	argopt_sn,
	argopt_af,
	argopt_ef,
	argopt_if,
	argopt_overlast
} ; /* end enum */

constexpr cpcchar	argopts[] = {
	"ROOT",
	"VERSION",
	"VERBOSE",
	"TMPDIR",
	"HELP",
	"sn",
	"af",
	"ef",
	"if",
	nullptr
} ; /* end array */

constexpr cpcchar	initvars = {
	VARPROGRAMROOT1,
	VARPROGRAMROOT2,
	VARPROGRAMROOT3,
	PROGRAMROOT,
	VARPRNAME
} ; /* end array */

static const struct mapex	mapexs[] = {
	{ SR_NOENT, EX_NOUSER },
	{ SR_AGAIN, EX_TEMPFAIL },
	{ SR_DEADLK, EX_TEMPFAIL },
	{ SR_NOLCK, EX_TEMPFAIL },
	{ SR_TXTBSY, EX_TEMPFAIL },
	{ SR_INVALID, EX_USAGE },
	{ 0, 0 }
} ; /* end array */

enum progopts {
	progopt_log,
	progopt_logsize,
	progopt_tmpdir,
	progopt_overlast
} ; /* end enum */

constexpr cpcchar	progopts[] = {
	"log",
	"logsize",
	"tmpdir",
	nullptr
} ; /* end array */

constexpr cpcchar	xdnames[] = {
	".X11-unix",
	".X11-pipe",
	nullptr
} ; /* end array */


/* exported variables */


/* exported subroutines */

int main(int argc,con mainv argv,con mainv envv) {
	PI	pi, *pip = &pi ;
	LI		li, *lip = &li ;
	bits		pargs ;
	keyopt		akopts ;
	bfile		errfile ;

#if	(CF_DEBUG || CF_DEBUG) && CF_DEBUGMALL
	uint		mo_start = 0 ;
#endif

	int		argr, argl, aol, akl, avl, kwi ;
	int		ai, ai_max, ai_pos ;
	int		rs, rs1 ;
	int		ex = EX_INFO ;
	int		f_optminus, f_optplus, f_optequal ;
	int		f_usage = false ;
	int		f_help = false ;
	int		f_version = false ;

	cchar	*argp, *aop, *akp, *avp ;
	cchar	*argval = nullptr ;
	cchar	*pr = nullptr ;
	cchar	*sn = nullptr ;
	cchar	*afname = nullptr ;
	cchar	*efname = nullptr ;
	cchar	*cp ;

#if	CF_DEBUG || CF_DEBUG
	if ((cp = getourenv(envv,VARDEBUGFNAME)) != nullptr) {
	    rs = debugopen(cp) ;
	    debugprintf("main: starting DFD=%d\n",rs) ;
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

	if ((cp = getenv(VARBANNER)) == nullptr) cp = BANNER ;
	proginfo_setbanner(pip,cp) ;

	pip->verboselevel = 1 ;
	pip->fl.logprog = true ;

	pip->lip = &li ;
	rs = locinfo_start(lip,pip) ;
	if (rs < 0) {
	    ex = EX_OSERR ;
	    goto badlocstart ;
	}

/* process program arguments */

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

	    f_optminus = (*argp == '-') ;
	    f_optplus = (*argp == '+') ;
	    if ((argl > 1) && (f_optminus || f_optplus)) {
	        cint ach = MKCHAR(argp[1]) ;

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

	                case argopt_sn:
	                    if (f_optequal) {
	                        f_optequal = false ;
	                        if (avl > 0)
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

/* default action and user specified help */
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

/* help */
	                case argopt_help:
	                    f_help = true ;
	                    break ;

/* version */
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

	                default:
	                    rs = SR_INVALID ;
	                    break ;

	                } /* end switch (key words) */

	            } else {

	                while (akl--) {
	                    cint	kc = MKCHAR(*akp) ;

	                    switch (kc) {

	                    case 'V':
	                        f_version = true ;
	                        break ;

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

/* quiet */
	                    case 'q':
	                        pip->fl.quiet = true ;
	                        break ;

/* verbose (level) */
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

	        } /* end if (digit or not) */

	    } else {

	        rs = bits_set(&pargs,ai) ;
	        ai_max = ai ;

	    } /* end if (key letter/word or positional) */

	    ai_pos = ai ;

	} /* end while (all command line argument processing) */

	if (efname == nullptr) efname = getenv(VAREFNAME) ;
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

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    debugprintf("main: debuglevel=%u\n", pip->debuglevel) ;
#endif

	if (f_version) {
	    bprintf(pip->efp,"%s: version %s\n",pip->progname,pip->version) ;
	}

/* get some program information */

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

/* help file */

	if (f_help)
	    printhelp(nullptr,pip->pr,pip->searchname,HELPFNAME) ;

	if (f_version || f_usage || f_help)
	    goto retearly ;


	ex = EX_OK ;

/* load up the environment options */

	if ((rs >= 0) && (pip->n == 0) && (argval != nullptr)) {
	    rs = optvalue(argval,-1) ;
	    pip->n = rs ;
	}

	if (rs >= 0) {
	    if ((ai_pos < 0) || (ai_max < 0)) rs = SR_BUGCHECK ;
	}

	if (pip->tmpdname != nullptr) pip->finval.tmpdir = true ;

	if (rs >= 0) {
	    rs = procopts(pip,&akopts) ;
	}

/* other initialization */

	if (afname == nullptr) afname = getenv(VARAFNAME) ;

	if (pip->tmpdname == nullptr) pip->tmpdname = getenv(VARTMPDNAME) ;
	if (pip->tmpdname == nullptr) pip->tmpdname = TMPDNAME ;

/* continue */

	if (rs >= 0) {
	    userinfo	u ;
	    if ((rs = userinfo_start(&u,nullptr)) >= 0) {
	        if ((rs = procuserinfo_begin(pip,&u)) >= 0) {
		    if ((rs = proglog_begin(pip,&u)) >= 0) {
			if ((rs = proguserlist_begin(pip)) >= 0) {
	    		    if ((rs = proctmp(pip)) >= 0) {
	        		rs = process(pip) ;
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
	} else {
	    cchar	*pn = pip->progname ;
	    cchar	*fmt = "%s: invalid argument or configuration (%d)\n" ;
	    ex = EX_USAGE ;
	    bprintf(pip->efp,fmt,pn,rs) ;
	    usage(pip) ;
	}

/* done */
	if ((ex == EX_OK) && (rs < 0)) {
	    switch (rs) {
	    case SR_NOENT:
	        ex = EX_NOUSER ;
	        break ;
	    case SR_AGAIN:
	    case SR_DEADLK:
	    case SR_NOLCK:
	    case SR_TXTBSY:
	        ex = EX_TEMPFAIL ;
	        break ;
	    default:
	        ex = mapex(mapexs,rs) ;
	        if (pip->open.logprog) {
	            logfile_printf(&pip->lh,
	                "ex=%u unknown error (%d)\n",ex,rs) ;
	        }
	        if (! pip->fl.quiet) {
	            bprintf(pip->efp,"%s: unknown bad thing (%d)\n",
	                pip->progname,rs) ;
	        }
	        break ;
	    } /* end switch */
	} /* end if */

retearly:
	if (pip->debuglevel > 0) {
	    bprintf(pip->efp,"%s: exiting ex=%d (%d)\n",
	        pip->progname,ex,rs) ;
	}

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
	locinfo_finish(lip) ;

badlocstart:
	proginfo_finish(pip) ;

badprogstart:

#if	(CF_DEBUG || CF_DEBUG) && CF_DEBUGMALL
	{
	    uint	mo ;
	    uc_mallout(&mo) ;
	    debugprintf("main: final mallout=%u\n",(mo-mo_start)) ;
	    uc_mallset(0) ;
	}
#endif /* CF_DEBUGMALL */

#if	(CF_DEBUG || CF_DEBUG)
	debugclose() ;
#endif

	return ex ;

/* bad stuff */
badarg:
	ex = EX_USAGE ;
	bprintf(pip->efp,"%s: invalid argument specified (%d)\n",
	    pip->progname,rs) ;
	usage(pip) ;
	goto retearly ;

} /* end subroutine (main) */


/* local subroutines */

local int usage(PI *pip) noex {
	int		rs = SR_OK ;
	int		wlen = 0 ;
	cchar	*pn = pip->progname ;
	cchar	*fmt ;

	fmt = "%s: USAGE> %s\n" ;
	if (rs >= 0) rs = bprintf(pip->efp,fmt,pn,pn) ;
	wlen += rs ;

	fmt = "%s:  [-Q] [-D] [-v[=<n>]] [-HELP] [-V]\n" ;
	if (rs >= 0) rs = bprintf(pip->efp,fmt,pn) ;
	wlen += rs ;

	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (usage) */

/* process program options */
local int procopts(PI *pip,keyopt *kop) noex {
	int		rs = SR_OK ;
	cchar		*cp ;

	if ((cp = getenv(VAROPTS)) != nullptr) {
	    rs = keyopt_loads(kop,cp,-1) ;
	}

	if (rs >= 0) {
	    keyopt_cur	kcur ;
	    if ((rs = keyopt_curbegin(kop,&kcur)) >= 0) {
	        uint	uv ;
	        int	oi ;
	        int	kl, vl ;
	        cchar	*kp, *vp ;

	        while ((kl = keyopt_curenumkeys(kop,&kcur,&kp)) >= 0) {

	            if ((oi = matostr(progopts,3,kp,kl)) >= 0) {

	                vl = keyopt_fetch(kop,kp,nullptr,&vp) ;

	                switch (oi) {
	                case progopt_log:
	                    if (! pip->finval.logprog) {
	                        pip->have.logprog = true ;
	                        pip->finval.logprog = true ;
	                        pip->fl.logprog = true ;
	                        if (vl > 0) {
	                            rs = optbool(vp,vl) ;
	                            pip->fl.logprog = (rs > 0) ;
	                        }
	                    }
	                    break ;
	                case progopt_logsize:
	                    if (! pip->finval.logsize) {
	                        pip->have.logsize = true ;
	                        pip->finval.logsize = true ;
	                        if (vl > 0) {
	                            rs = cfdecui(vp,vl,&uv) ;
	                            pip->logsize = uv ;
	                        }
	                    }
	                    break ;
	                case progopt_tmpdir:
	                    if (! pip->finval.tmpdir) {
	                        pip->have.tmpdir = true ;
	                        pip->finval.tmpdir = true ;
	                        if (vl > 0) {
	                            cchar	**vpp = &pip->tmpdname ;
	                            rs = proginfo_setentry(pip,vpp,vp,vl) ;
	                        }
	                    }
	                    break ;
	                } /* end switch */

	            } /* end if */

	            if (rs < 0) break ;
	        } /* end while */

	        keyopt_curend(kop,&kcur) ;
	    } /* end if (keyopt-cur) */
	} /* end if (ok) */

	return rs ;
} /* end subroutine (procopts) */

local int proctmp(PI *pip) noex {
	ustat	sb ;
	int		rs ;
	if ((rs = u_stat(pip->tmpdname,&sb)) >= 0) {
	    pip->uid_tmp = sb.st_uid ;
	    pip->gid_tmp = sb.st_gid ;
	}
	return rs ;
} /* end subroutine (proctmp) */

local int process(PI *pip) noex {
	int		rs = SR_OK ;
	int		i ;
	for (i = 0 ; (rs >= 0) && (xdnames[i] != nullptr) ; i += 1) {
	    rs = procdir(pip,xdnames[i]) ;
	}
	return rs ;
} /* end subroutine (process) */

local int procdir(PI *pip,cchar *dname) noex {
	int		rs ;
	char		xdname[MAXPATHLEN + 1] ;
	if ((rs = mkpath2(xdname,pip->tmpdname,dname)) >= 0) {
	    ustat	sb ;
	    const mode_t	dm = DIRMODE ;
	    if ((rs = u_stat(xdname,&sb)) >= 0) {
		rs = procdirer(pip,xdname,dm) ;
	    } else if (isNotPresent(rs)) {
	        if ((rs = u_mkdir(xdname,dm)) >= 0) {
		    rs = procdirer(pip,xdname,dm) ;
		}
	    }
	    if ((rs < 0) && (! pip->fl.quiet)) {
		cchar	*pn = pip->progname ;
		cchar	*fmt = "%s: could not establish dir=%s (%d)\n" ;
	        bprintf(pip->efp,fmt,pn,dname,rs) ;
	    }
	} /* end if (mkpath2) */
	return rs ;
} /* end subroutine (procdir) */

local int procdirer(PI *pip,cchar *xdname,mode_t dm) noex {
	int		rs ;
	if ((rs = uc_minmod(xdname,dm)) >= 0) {
	    if (pip->uid != pip->euid) {
	        rs = u_chown(xdname,pip->uid,pip->gid_tmp) ;
	    }
	}
	return rs ;
} /* end subroutine (procdirer) */

local int procuserinfo_begin(PI *pip,userinfo *uip) noex {
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

	return rs ;
} /* end subroutine (procuserinfo_begin) */

local int procuserinfo_end(PI *pip) noex {
	int		rs = SR_OK ;

	if (pip == nullptr) return SR_FAULT ;

	return rs ;
/* end subroutine (procuserinfo_end) */

local int locinfo_start(LI *lip,PI *pip) noex {
	int		rs = SR_OK ;

	memclear(lip) ;
	lip->pip = pip ;

	return rs ;
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
} /* end subroutine (locinfo_finish) */



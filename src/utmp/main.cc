/* main SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* UTMP program */
/* version %I% last-modified %G% */

#define	CF_LOCALWTMP	0		/* use local WTMP */
#define	CF_LOCSETENT	0		/* |locinfo_setentry()| */

/* revision history:

	= 2000-12-13, David A­D­ Morano
        This program was originally written (as a test).

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Synopsis:
	$ utmp [-y|-n] [-h <hostname>]

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<utmpx.h>
#include	<netdb.h>
#include	<usystem.h>
#include	<bfile.h>
#include	<bits.h>
#include	<tmpx.h>
#include	<exitcodes.h>
#include	<localmisc.h>

#include	"config.h"
#include	"defs.h"


/* local defines */

#ifndef	WTMPFNAME
#if	CF_LOCALWTMP
#define	WTMPFNAME	"wtmpx"
#else
#define	WTMPFNAME	"/var/adm/wtmpx"
#endif
#endif

#ifndef	UTMPFNAME
#define	UTMPFNAME	"/var/adm/utmpx"
#endif

#define	TERMBUFLEN	(TMPX_LLINE + 5)

#define	LOCINFO		struct locinfo
#define	LOCINFO_FL	struct locinfo_flags


/* external subroutines */


/* external variables */


/* local structures */

struct locinfo_flags {
	uint		stores:1 ;
	uint		searchline:1 ;
	uint		yes:1 ;
	uint		no:1 ;
	uint		zap:1 ;
} ;

struct locinfo {
	LOCINFO_FL	have, f, changed, final ;
	LOCINFO_FL	open ;
	vecstr		stores ;
	PROGINFO	*pip ;
	cchar		*searchline ;
} ;


/* forward references */

static int	usage(PROGINFO *) ;

static int	process(PROGINFO *,bfile *,pid_t) ;
static int	zaputx(UTMPX	 *) ;

static int	locinfo_start(LOCINFO *,PROGINFO *) ;
static int	locinfo_finish(LOCINFO *) ;

#if	CF_LOCSETENT
static int	locinfo_setentry(LOCINFO *,cchar **,cchar *,int) ;
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
	NULL
} ;

constexpr pivars	initvars = {
	VARPROGRAMROOT1,
	VARPROGRAMROOT2,
	VARPROGRAMROOT3,
	PROGRAMROOT,
	VARPRLOCAL
} ;

constexpr mapex		mapexs[] = {
	{ SR_NOENT, EX_NOUSER },
	{ SR_AGAIN, EX_TEMPFAIL },
	{ SR_DEADLK, EX_TEMPFAIL },
	{ SR_NOLCK, EX_TEMPFAIL },
	{ SR_TXTBSY, EX_TEMPFAIL },
	{ SR_ACCESS, EX_NOPERM },
	{ SR_REMOTE, EX_PROTOCOL },
	{ SR_NOSPC, EX_TEMPFAIL },
	{ 0, 0 }
} ;


/* exported variables */


/* exported subroutines */

int main(int argc,mainv argv,mainv envv) {
	PROGINFO	pi, *pip = &pi ;
	LOCINFO		li, *lip = &li ;
	BITS		pargs ;
	bfile		errfile ;
	pid_t		pid = 0 ;

	int		argr, argl, aol, akl, avl, kwi ;
	int		ai, ai_max, ai_pos ;
	int		pan = 0 ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		ex = EX_INFO ;
	int		f_optminus, f_optplus, f_optequal ;
	int		f_version = FALSE ;
	int		f_usage = FALSE ;
	int		f_help = FALSE ;
	int		f ;

	cchar	*argp, *aop, *akp, *avp ;
	cchar	*argval = NULL ;
	cchar	*pr = NULL ;
	cchar	*sn = NULL ;
	cchar	*afname = NULL ;
	cchar	*efname = NULL ;
	cchar	*ofname = NULL ;
	cchar	*cp ;


	rs = proginfo_start(pip,envv,argv[0],VERSION) ;
	if (rs < 0) {
	    ex = EX_OSERR ;
	    goto badprogstart ;
	}

	if ((cp = getenv(VARBANNER)) == NULL) cp = BANNER ;
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

	if (rs >= 0) rs = bits_start(&pargs,1) ;
	if (rs < 0) goto badpargs ;

	ai_max = 0 ;
	ai_pos = 0 ;
	argr = argc ;
	for (ai = 0 ; (ai < argc) && (argv[ai] != NULL) ; ai += 1) {
	    if (rs < 0) break ;
	    argr -= 1 ;
	    if (ai == 0) continue ;

	    argp = argv[ai] ;
	    argl = strlen(argp) ;

	    f_optminus = (*argp == '-') ;
	    f_optplus = (*argp == '+') ;
	    if ((argl > 0) && (f_optminus || f_optplus)) {
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
	            f_optequal = FALSE ;
	            if ((avp = strchr(aop,'=')) != NULL) {
	                f_optequal = TRUE ;
	                akl = avp - aop ;
	                avp += 1 ;
	                avl = aop + argl - 1 - avp ;
	                aol = akl ;
	            } else {
	                avp = NULL ;
	                avl = 0 ;
	                akl = aol ;
	            }

	            if ((kwi = matostr(argopts,2,akp,akl)) >= 0) {

	                switch (kwi) {

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

/* argument-list file */
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
	                            f_optequal = FALSE ;
	                            if (avl) {
	                                rs = optvalue(avp,avl) ;
	                                pip->debuglevel = rs ;
	                            }
	                        }
	                        break ;

/* quiet mode */
	                    case 'Q':
	                        pip->fl.quiet = TRUE ;
	                        break ;

/* version */
	                    case 'V':
	                        f_version = TRUE ;
	                        break ;

	                    case 'h':
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl)
	                                pip->hostname = argp ;
	                        } else
	                            rs = SR_INVALID ;
	                        break ;

	                    case 'y':
	                        lip->fl.yes = TRUE ;
	                        break ;

	                    case 'n':
	                        lip->fl.no = TRUE ;
	                        break ;

	                    case 'p':
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl) {
	                                rs = optvalue(argp,argl) ;
	                                pid = rs ;
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
	                            f_optequal = FALSE ;
	                            if (avl) {
	                                rs = optvalue(avp,avl) ;
	                                pip->verboselevel = rs ;
	                            }
	                        }
	                        break ;

	                    case 'z':
	                        lip->fl.zap = TRUE ;
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

	if (efname == NULL) efname = getenv(VAREFNAME) ;
	if (efname == NULL) efname = BFILE_STDERR ;
	if ((rs1 = bopen(&errfile,efname,"wca",0666)) >= 0) {
	    pip->efp = &errfile ;
	    pip->open.errfile = TRUE ;
	    bcontrol(&errfile,BC_SETBUFLINE,TRUE) ;
	} else if (! isNotPresent(rs1)) {
	    if (rs >= 0) rs = rs1 ;
	}

	if (rs < 0)
	    goto badarg ;

	if (f_version) {
	    bprintf(pip->efp,"%s: version %s\n",
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
	    bprintf(pip->efp,"%s: pr=%s\n", pip->progname,pip->pr) ;
	    bprintf(pip->efp,"%s: sn=%s\n", pip->progname,pip->searchname) ;
	} /* end if */

	if (f_usage)
	    usage(pip) ;

/* help file */

	if (f_help)
	    printhelp(NULL,pip->pr,pip->searchname,HELPFNAME) ;

	if (f_version || f_help || f_usage)
	    goto retearly ;


	ex = EX_OK ;

/* some initialization */

	if ((rs >= 0) && (pip->n == 0) && (argval != NULL)) {
	    rs = optvalue(argval,-1) ;
	    pip->n = rs ;
	}

	if (afname == NULL) afname = getenv(VARAFNAME) ;

	if (pip->tmpdname == NULL) pip->tmpdname = getenv(VARTMPDNAME) ;
	if (pip->tmpdname == NULL) pip->tmpdname = TMPDNAME ;

/* loop through the arguments processing them */

	for (ai = 1 ; ai < argc ; ai += 1) {
	    f = (ai <= ai_max) && (bits_test(&pargs,ai) > 0) ;
	    f = f || ((ai > ai_pos) && (argv[ai] != NULL)) ;
	    if (f) {
	        cp = argv[ai] ;
	        if (cp[0] != '\0') {
	            switch (pan) {
	            case 0:
	                lip->searchline = cp ;
	                break ;
	            } /* end switch */
	            pan += 1 ;
	        } /* end if (non-nul) */
	    }
	} /* end for */

/* open the output file */

	if ((ofname == NULL) || (ofname[0] == '\0') || (ofname[0] == '-'))
	    ofname = BFILE_STDOUT ;

	if (rs >= 0) {
	    bfile	ofile, *ofp = &ofile ;
	    if ((rs = bopen(ofp,ofname,"wct",0666)) >= 0) {
	        rs = process(pip,ofp,pid) ;
	        bclose(ofp) ;
	    } else {
	        ex = EX_CANTCREAT ;
	        bprintf(pip->efp,"%s: inaccesssible output (%d)\n",
	            pip->progname,rs) ;
	    }
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
	} /* end if */

/* we are done */
retearly:
	if (pip->debuglevel > 0) {
	    bprintf(pip->efp,"%s: exiting ex=%u (%d)\n",
	        pip->progname,ex,rs) ;
	}

	if (pip->efp != NULL) {
	    pip->open.errfile = FALSE ;
	    bclose(pip->efp) ;
	    pip->efp = NULL ;
	}

	bits_finish(&pargs) ;

badpargs:
	locinfo_finish(lip) ;

badlocstart:
	proginfo_finish(pip) ;

badprogstart:
	return ex ;

/* the bad things */
badarg:
	ex = EX_USAGE ;
	bprintf(pip->efp,"%s: bad argument(s) given (%d)\n",
	    pip->progname,rs) ;
	usage(pip) ;
	goto retearly ;

}
/* end subroutine (main) */


/* local subrouines */

static int usage(PROGINFO *pip) noex {
	int		rs = SR_OK ;
	int		wlen = 0 ;
	cchar	*pn = pip->progname ;
	cchar	*fmt ;

	fmt = "%s: USAGE> %s [-y|-n] [-h <hostname>] <searchline> [-z]\n" ;
	if (rs >= 0) rs = bprintf(pip->efp,fmt,pn,pn) ;
	wlen += rs ;

	fmt = "%s:  [-Q] [-D] [-v[=n]] [-HELP] [-V]\n" ;
	if (rs >= 0) rs = bprintf(pip->efp,fmt,pn) ;
	wlen += rs ;

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (usage) */

static int process(PROGINFO *pip,bfile *ofp,pid_t pid) noex {
	UTMPX		uc ;
	UTMPX		*up ;
	LOCINFO		*lip = pip->lip ;
	const pid_t	sid = u_getsid(pid) ;
	int		rs = SR_OK ;
	int		si ;
	int		cl ;
	char		lognamebuf[TMPX_LUSER + 1] ;
	cchar		*pn = pip->progname ;
	cchar		*fmt ;


	lognamebuf[0] = '\0' ;
	if (pip->debuglevel > 0) {
	    fmt = "%s: sid=%d\n" ;
	    bprintf(pip->efp,fmt,pn,sid) ;
	}

/* loop */

	setutxent() ;

	si = 0 ;
	while ((up = getutxent()) != NULL) {
	    if (lip->fl.zap && (lip->searchline != NULL)) {
		cchar	*searchline = lip->searchline ;

	        if (strncmp(up->ut_line,searchline,TMPX_LLINE) == 0)
	            break ;

	    } else if ((up->ut_type == TMPX_TUSERPROC) &&
	        	(up->ut_pid == sid)) {
	        break ;
	    } /* end if */

	    si += 1 ;

	} /* end while (positioning within the UTMPX file) */

	if (up != NULL) {
	    strwcpy(lognamebuf,up->ut_user,TMPX_LUSER) ;
	}

	if (lip->fl.zap && (lip->searchline != NULL)) {

	    uc = *up ;		/* copy the record found */

	    uc.ut_exit.e_termination = 0 ;
	    uc.ut_exit.e_exit = 0 ;
	    uc.ut_type = TMPX_TEMPTY ;

	    rs = zaputx(&uc) ;

	    if (rs < 0) {
		fmt = "%s: operation not allowed (%d)\n" ;
	        bprintf(pip->efp,fmt,pn,rs) ;
	    }

	} else if ((! lip->fl.yes) && (! lip->fl.no)) {
	    fmt = "id=%-4t line=%-12t user=%-12t sid=%-5u ses=%u host=%r\n" ;

	    if (up != NULL) {

	        bprintf(ofp,fmt,
	            up->ut_id,strnlen(up->ut_id,TMPX_LID),
	            up->ut_line,strnlen(up->ut_line,TMPX_LLINE),
	            up->ut_user,strnlen(up->ut_user,TMPX_LUSER),
	            up->ut_pid,
	            up->ut_session,
	            up->ut_host,strnlen(up->ut_host,TMPX_LHOST)) ;

	    } else {
	        bprintf(ofp,"not logged in\n") ;
	    }

	} else if (lip->fl.yes) {

	    if (up == NULL) {
		cint	termlen = TERMBUFLEN ;
	        int	tl ;
	        char	termbuf[TERMBUFLEN + 1] ;

	        if ((rs = termdevice(termbuf,termlen,FD_STDIN)) >= 0) {
	            char	idbuf[TMPX_LID + 1] ;
	            tl = rs ;
	            uc = {} ;
	            mkutmpid(idbuf,TMPX_LID,termbuf,tl) ;

	            strncpy(uc.ut_id,idbuf,TMPX_LID) ;

	            strncpy(uc.ut_line,(termbuf + 5),TMPX_LLINE) ;

	            if (lognamebuf[0] == '\0') {
	                getusername(lognamebuf,USERNAMELEN,-1) ;
		    }

	            strncpy(uc.ut_user,lognamebuf,TMPX_LUSER) ;

	            if (pip->hostname != NULL) {

	                strncpy(uc.ut_host,pip->hostname,TMPX_LHOST) ;

	                cl = strnlen(pip->hostname,TMPX_LHOST) ;

#if	defined(OSNAME_SunOS) && (OSNAME_SunOS > 0)
	                uc.ut_host[cl] = '\0' ;
#endif

	                uc.ut_syslen = (cl + 1) ;

	            } else {
	                uc.ut_host[0] = '\0' ;
	                uc.ut_syslen = 0 ;
	            }

	            gettimeofday(&uc.ut_tv,NULL) ;

	            uc.ut_pid = sid ;
	            uc.ut_exit.e_termination = 0 ;
	            uc.ut_exit.e_exit = 0 ;
	            uc.ut_type = TMPX_TUSERPROC ;

	            up = pututxline(&uc) ;

	            if (up == NULL) {
	                rs = SR_ACCESS ;
	                bprintf(pip->efp,
	                    "%s: operation not allowed\n",
	                    pip->progname) ;
	            }

	        } else {
	            bprintf(ofp,"no terminal on standard input\n") ;
		}

	    } else {
	        bprintf(ofp,"already logged in\n") ;
	    }

	} else {

	    if (up != NULL) {

	        uc = *up ;		/* copy the record found */

	        uc.ut_exit.e_termination = 0 ;
	        uc.ut_exit.e_exit = 0 ;
	        uc.ut_type = TMPX_TDEADPROC ;

	        if ((up = pututxline(&uc)) == nullptr) {
	            rs = SR_ACCESS ;
		    fmt = "%s: operation not allowed\n" ;
	            bprintf(pip->efp,fmt,pn) ;
	        }

	    } /* end if */

	} /* end if (what to do) */

	endutxent() ;

	return rs ;
}
/* end subroutine (process) */

/* zap the specified UTMPX entry */
static int zaputx(UTMPX	 *up) noex {
	TMPX		ut ;
	TMPX_CUR	cur ;
	TMPX_ENT	ue ;
	int		rs ;
	int		ei = 0 ;
	int		f ;

	if ((rs = tmpx_open(&ut,NULL,O_RDWR)) >= 0) {
	    if ((rs = tmpx_curbegin(&ut,&cur)) >= 0) {

	        f = FALSE ;
	        while ((ei = tmpx_curenum(&ut,&cur,&ue)) >= 0) {

	            f = ((strncmp(up->ut_id,ue.ut_id,TMPX_LID) == 0) &&
	                (strncmp(up->ut_line,ue.ut_line,TMPX_LLINE) == 0)) ;

	            if (f) break ;
	        } /* end while */

	        if (f) {

	            ue.ut_exit.e_termination = up->ut_exit.e_termination ;
	            ue.ut_exit.e_exit = up->ut_exit.e_termination ;

	            ue.ut_type = up->ut_type ;
	            ue.ut_host[0] = '\0' ;
	            ue.ut_syslen = 0 ; /* or '1' like stupid Solaris wants? */

	            rs = tmpx_write(&ut,ei,&ue) ;

	        } /* end if (got a match) */

	        tmpx_curend(&ut,&cur) ;
	    } /* end if (cursor) */
	    tmpx_close(&ut) ;
	} /* end if (tmpx) */

	return (rs >= 0) ? ei : rs ;
}
/* end subroutine (zaputx) */

static int locinfo_start(LOCINFO *lip,PROGINFO *pip) noex {
	int		rs = SR_OK ;

	memclear(lip) ; /* dangerous */
	lip->pip = pip ;

	return rs ;
}
/* end subroutine (locinfo_start) */

static int locinfo_finish(LOCINFO *lip) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (lip == NULL) return SR_FAULT ;

	if (lip->open.stores) {
	    lip->open.stores = FALSE ;
	    rs1 = vecstr_finish(&lip->stores) ;
	    if (rs >= 0) rs = rs1 ;
	}

	return rs ;
}
/* end subroutine (locinfo_finish) */

#if	CF_LOCSETENT
int locinfo_setentry(LOCINFO *lip,cchar **epp,cchar *vp,int vl) noex {
	vecstr		*slp ;
	int		rs = SR_OK ;
	int		len = 0 ;

	if (lip == NULL) return SR_FAULT ;
	if (epp == NULL) return SR_FAULT ;

	slp = &lip->stores ;
	if (! lip->open.stores) {
	    rs = vecstr_start(slp,4,0) ;
	    lip->open.stores = (rs >= 0) ;
	}

	if (rs >= 0) {
	    int	oi = -1 ;
	    if (*epp != NULL) {
		oi = vecstr_findaddr(slp,*epp) ;
	    }
	    if (vp != NULL) {
	        len = strnlen(vp,vl) ;
	        rs = vecstr_store(slp,vp,len,epp) ;
	    } else {
	        *epp = NULL ;
	    }
	    if ((rs >= 0) && (oi >= 0)) {
	        vecstr_del(slp,oi) ;
	    }
	} /* end if (ok) */

	return (rs >= 0) ? len : rs ;
}
/* end subroutine (locinfo_setentry) */
#endif /* CF_LOCSETENT */



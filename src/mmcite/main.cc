/* main */

/* part of the MMCITE program */


#define	CF_DEBUGS	0		/* compile-time debug print-outs */
#define	CF_DEBUG	1		/* run-time debug print-outs */
#define	CF_DEBUGMALL	1		/* debug memory-allocations */


/* revision history:

	= 1998-02-01, David A�D� Morano
        The program was written from scratch to do what the previous program by
        the same name did.

	= 2009-05-29, David A�D� Morano
	I enhanced this in some way.

*/

/* Copyright � 1998,2009 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	This is a fairly generic front-end subroutine for a program.


*******************************************************************************/


#include	<envstandards.h>	/* MUST be first to configure */

#include	<sys/types.h>
#include	<sys/param.h>
#include	<csignal>
#include	<unistd.h>
#include	<cstdlib>
#include	<cstring>
#include	<time.h>

#include	<usystem.h>
#include	<bits.h>
#include	<paramopt.h>
#include	<bfile.h>
#include	<vecstr.h>
#include	<ucmallreg.h>
#include	<exitcodes.h>
#include	<localmisc.h>

#include	"config.h"
#include	"defs.h"
#include	"citedb.h"
#include	"bdb.h"
#include	"keytracker.h"


/* local defines */

#ifndef	LINEBUFLEN
#define	LINEBUFLEN	MAX((MAXPATHLEN + 20),2048)
#endif

#ifndef	LINEFOLDLEN
#define	LINEFOLDLEN	76
#endif


/* external subroutines */

extern int	mkpath1(char *,const char *) ;
extern int	mkpath2(char *,const char *,const char *) ;
extern int	matstr(const char **,const char *,int) ;
extern int	matostr(const char **,int,const char *,int) ;
extern int	sfshrink(cchar *,int,cchar **) ;
extern int	cfdeci(const char *,int,int *) ;
extern int	cfdecti(const char *,int,int *) ;
extern int	optbool(const char *,int) ;
extern int	optvalue(const char *,int) ;
extern int	mktmpfile(char *,mode_t,const char *) ;
extern int	bprintlns(bfile *,int,const char *,int) ;
extern int	isdigitlatin(int) ;
extern int	isNotPresent(int) ;
extern int	isFailOpen(int) ;

extern int	printhelp(void *,const char *,const char *,const char *) ;
extern int	proginfo_setpiv(PROGINFO *,cchar *,const PIVARS *) ;
extern int	progfile(PROGINFO *,PARAMOPT *,BDB *,CITEDB *,cchar *) ;
extern int	progoutfin(PROGINFO *,BDB *,CITEDB *,cchar *) ;

#if	CF_DEBUGS || CF_DEBUG
extern int	debugopen(const char *) ;
extern int	debugprintf(const char *,...) ;
extern int	debugprinthexblock(cchar *,int,const void *,int) ;
extern int	debugclose() ;
extern int	strlinelen(const char *,int,int) ;
#endif

extern cchar	*getourenv(cchar **,cchar *) ;

extern char	*strwcpy(char *,const char *,int) ;
extern char	*strshrink(char *) ;


/* external variables */


/* local structures */


/* forward references */

int		findbibfile(PROGINFO *,PARAMOPT *,const char *,char *) ;

static int	usage(PROGINFO *) ;

static int	process(PROGINFO *,ARGINFO *,BITS *,PARAMOPT *,
			cchar *,cchar *) ;
static int	procargs(PROGINFO *,ARGINFO *,BITS *,PARAMOPT *,
			BDB *,CITEDB *,cchar *) ;
static int	procereport(PROGINFO *,int) ;

static int	loadbibfiles(PROGINFO *,PARAMOPT *,BDB *) ;


/* local variables */

static const char	*progmodes[] = {
	"mmcite",
	NULL
} ;

enum progmodes {
	progmode_mmcite,
	progmode_overlast
} ;

static const char	*argopts[] = {
	"ROOT",
	"VERSION",
	"VERBOSE",
	"TMPDIR",
	"HELP",
	"pm",
	"sn",
	"af",
	"ef",
	"of",
	"option",
	"set",
	"follow",
	NULL
} ;

enum argopts {
	argopt_root,
	argopt_version,
	argopt_verbose,
	argopt_tmpdir,
	argopt_help,
	argopt_pm,
	argopt_sn,
	argopt_af,
	argopt_ef,
	argopt_of,
	argopt_option,
	argopt_set,
	argopt_follow,
	argopt_overlast
} ;

static const struct pivars	initvars = {
	VARPROGRAMROOT1,
	VARPROGRAMROOT2,
	VARPROGRAMROOT3,
	PROGRAMROOT,
	VARPRLOCAL
} ;

static const struct mapex	mapexs[] = {
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

static const char	*progopts[] = {
	"uniq",
	"follow",
	"nofollow",
	NULL
} ;

enum progopts {
	progopt_uniq,
	progopt_follow,
	progopt_nofollow,
	progopt_overlast
} ;


/* exported subroutines */


int main(int argc,cchar **argv,cchar **envv)
{
	PROGINFO	pi, *pip = &pi ;
	ARGINFO		ainfo ;
	BITS		pargs ;
	PARAMOPT	aparams ;
	bfile		errfile ;
	bfile		outfile, *ofp = &outfile ;

#if	(CF_DEBUGS || CF_DEBUG) && CF_DEBUGMALL
	uint		mo_start = 0 ;
#endif

	int		argr, argl, aol, akl, avl, kwi ;
	int		ai, ai_max, ai_pos ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		cl ;
	int		ex = EX_INFO ;
	int		f_optminus, f_optplus, f_optequal ;
	int		f_usage = FALSE ;
	int		f_version = FALSE ;
	int		f_help = FALSE ;

	const char	*argp, *aop, *akp, *avp ;
	const char	*argval = NULL ;
	const char	*pmspec = NULL ;
	const char	*pr = NULL ;
	const char	*sn = NULL ;
	const char	*afname = NULL ;
	const char	*efname = NULL ;
	const char	*ofname = NULL ;
	const char	*cp ;

#if	CF_DEBUGS || CF_DEBUG
	if ((cp = getourenv(envv,VARDEBUGFNAME)) != NULL) {
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

	if ((cp = getenv(VARBANNER)) == NULL) cp = BANNER ;
	rs = proginfo_setbanner(pip,cp) ;

/* early things to initialize */

	pip->ofp = ofp ;

	pip->verboselevel = 1 ;
	pip->progmode = -1 ;

/* process program arguments */

	if (rs >= 0) rs = bits_start(&pargs,1) ;
	if (rs < 0) goto badpargs ;

	rs = paramopt_start(&aparams) ;
	pip->open.aparams = (rs >= 0) ;

	if ((rs >= 0) && ((cp = getenv(VARBIBFILES)) != NULL)) {
	    rs = paramopt_loads(&aparams,PO_BIBFILE, cp,-1) ;
	}

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
	    if ((argl > 1) && (f_optminus || f_optplus)) {
	        const int	ach = MKCHAR(argp[1]) ;

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

/* do we have a keyword match or should we assume only key letters? */

	            if ((kwi = matostr(argopts,2,akp,akl)) >= 0) {

	                switch (kwi) {

/* program root */
	                case argopt_root:
	                    if (f_optequal) {
	                        f_optequal = FALSE ;
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

/* program mode */
	                case argopt_pm:
	                    if (f_optequal) {
	                        f_optequal = FALSE ;
	                        if (avl)
	                            pmspec = avp ;
	                    } else {
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl)
	                                pmspec = argp ;
	                        } else
	                            rs = SR_INVALID ;
	                    }
	                    break ;

/* version */
	                case argopt_version:
	                    f_version = TRUE ;
	                    if (f_optequal)
	                        rs = SR_INVALID ;
	                    break ;

/* verbose */
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

/* temporary directory */
	                case argopt_tmpdir:
	                    if (f_optequal) {
	                        f_optequal = FALSE ;
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

	                case argopt_help:
	                    f_help = TRUE ;
	                    break ;

/* the user specified some progopts */
	                case argopt_option:
	                    if (argr > 0) {
	                        argp = argv[++ai] ;
	                        argr -= 1 ;
	                        argl = strlen(argp) ;
	                        if (argl) {
	                            PARAMOPT	*pop = &aparams ;
	                            cchar	*po = PO_OPTION ;
	                            rs = paramopt_loads(pop,po,argp,argl) ;
	                        }
	                    } else
	                        rs = SR_INVALID ;
	                    break ;

/* the user specified some progopts */
	                case argopt_set:
	                    if (argr > 0) {
	                        argp = argv[++ai] ;
	                        argr -= 1 ;
	                        argl = strlen(argp) ;
	                        if (argl) {
	                            PARAMOPT	*pop = &aparams ;
	                            rs = paramopt_loadu(pop,argp,argl) ;
	                        }
	                    } else
	                        rs = SR_INVALID ;
	                    break ;

/* search name */
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

/* argument files */
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

/* output file */
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

/* follow symbolic links */
	                case argopt_follow:
	                    pip->f.follow = TRUE ;
	                    break ;

/* default action and user specified help */
	                default:
	                    rs = SR_INVALID ;
	                    break ;

	                } /* end switch (key words) */

	            } else {

	                while (akl--) {
	                    const int	kc = MKCHAR(*akp) ;

	                    switch (kc) {

	                    case 'V':
	                        f_version = TRUE ;
	                        break ;

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

/* BIBDIR */
	                    case 'B':
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl) {
	                                PARAMOPT	*pop = &aparams ;
	                                cchar		*po = PO_BIBDIR ;
	                                rs = paramopt_loads(pop,po,argp,argl) ;
	                            }
	                        } else
	                            rs = SR_INVALID ;
	                        break ;

/* quiet */
	                    case 'Q':
	                        pip->f.quiet = TRUE ;
	                        break ;

/* options */
	                    case 'o':
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl) {
	                                PARAMOPT	*pop = &aparams ;
	                                cchar		*po = PO_OPTION ;
	                                rs = paramopt_loads(pop,po,argp,argl) ;
	                            }
	                        } else
	                            rs = SR_INVALID ;
	                        break ;

/* BIBFILE */
	                    case 'p':
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl) {
	                                PARAMOPT	*pop = &aparams ;
	                                cchar		*po = PO_BIBFILE ;
	                                rs = paramopt_loads(pop,po,argp,argl) ;
	                            }
	                        } else
	                            rs = SR_INVALID ;
	                        break ;

/* take input file arguments from STDIN */
	                    case 'f':
	                        pip->f.follow = TRUE ;
	                        break ;

/* no-change */
	                    case 'n':
	                        pip->f.nochange = TRUE ;
	                        break ;

/* require a suffix for file names */
	                    case 's':
	                        cp = NULL ;
	                        cl = -1 ;
	                        if (f_optequal) {
	                            f_optequal = FALSE ;
	                            if (avl) {
	                                cp = avp ;
	                                cl = avl ;
	                            }
	                        } else {
	                            if (argr > 0) {
	                                argp = argv[++ai] ;
	                                argr -= 1 ;
	                                argl = strlen(argp) ;
	                                if (argl) {
	                                    cp = argp ;
	                                    cl = argl ;
	                                }
	                            } else
	                                rs = SR_INVALID ;
	                        }
	                        if ((rs >= 0) && (cp != NULL)) {
	                            PARAMOPT	*pop = &aparams ;
	                            cchar	*po = PO_SUFFIX ;
	                            rs = paramopt_loads(pop,po,cp,cl) ;
	                        }
	                        break ;

	                    case 'u':
	                        pip->f.uniq = TRUE ;
	                        break ;

/* verbose output */
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

	        } /* end if */

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
	} else if (! isFailOpen(rs1)) {
	    if (rs >= 0) rs = rs1 ;
	}

	if (rs < 0)
	    goto badarg ;

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    debugprintf("main: debuglevel=%u\n",pip->debuglevel) ;
#endif

	if (f_version) {
	    bprintf(pip->efp,"%s: version %s\n",pip->progname,VERSION) ;
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

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("main: pr=%s\n",pip->pr) ;
#endif

	if (pip->debuglevel > 0) {
	    bprintf(pip->efp,"%s: pr=%s\n",pip->progname,pip->pr) ;
	    bprintf(pip->efp,"%s: sn=%s\n",pip->progname,pip->searchname) ;
	}

/* get our program mode */

	if (pmspec == NULL) pmspec = pip->progname ;

	pip->progmode = matstr(progmodes,pmspec,-1) ;

#if	CF_DEBUG
	if (DEBUGLEVEL(2)) {
	    if (pip->progmode >= 0) {
	        debugprintf("main: progmode=%s(%u)\n",
	            progmodes[pip->progmode],pip->progmode) ;
	    } else {
	        debugprintf("main: progmode=NONE\n") ;
	    }
	}
#endif /* CF_DEBUG */

	if (pip->progmode < 0)
	    pip->progmode = progmode_mmcite ;

/* help file */

	if (f_usage)
	    usage(pip) ;

	if (f_help)
	    printhelp(NULL,pip->pr,pip->searchname,HELPFNAME) ;

	if (f_help || f_version || f_usage)
	    goto retearly ;


	ex = EX_OK ;

/* check a few more things */

	if ((rs >= 0) && (pip->n == 0) && (argval != NULL)) {
	    rs = optvalue(argval,-1) ;
	    pip->n = rs ;
	}

	if (afname == NULL) afname = getenv(VARAFNAME) ;

	if (pip->tmpdname == NULL) pip->tmpdname = getenv(VARTMPDNAME) ;
	if (pip->tmpdname == NULL) pip->tmpdname = TMPDNAME ;

/* we need a PWD for later (handling non-rooted BIB files) */

	if (rs >= 0) {
	    rs = proginfo_pwd(pip) ;
	}

#if	CF_DEBUG
	if (DEBUGLEVEL(2)) {
	    debugprintf("main: proginfo_pwd() rs=%d\n",rs) ;
	    debugprintf("main: pwd=%s\n",pip->pwd) ;
	}
#endif

/* process some options */

	if (rs >= 0) {
	    if ((rs = paramopt_havekey(&aparams,PO_OPTION)) > 0) {
	        PARAMOPT_CUR	cur ;
	        if ((rs = paramopt_curbegin(&aparams,&cur)) >= 0) {
	            cchar	*po = PO_OPTION ;

	            while (paramopt_enumvalues(&aparams,po,&cur,&cp) >= 0) {
	                if (cp != NULL) {

	                    if ((kwi = matostr(progopts,2,cp,-1)) >= 0) {
	                        switch (kwi) {
	                        case progopt_follow:
	                            pip->f.follow = TRUE ;
	                            break ;
	                        case progopt_nofollow:
	                            pip->f.follow = FALSE ;
	                            break ;
	                        case progopt_uniq:
	                            pip->f.uniq = TRUE ;
	                            break ;
	                        } /* end switch */
	                    } /* end if (matostr) */

	                }
	            } /* end while */

	            paramopt_curend(&aparams,&cur) ;
	        } /* end if (progopts) */
	    } /* end if (paramopt_havekey) */
	} /* end if (ok) */

/* load up BIBDIRS */

	if ((rs >= 0) && ((cp = getenv(VARBIBDIRS)) != NULL)) {
	    rs = paramopt_loads(&aparams,PO_BIBDIR, cp,-1) ;
	}

	memset(&ainfo,0,sizeof(ARGINFO)) ;
	ainfo.argc = argc ;
	ainfo.ai = ai ;
	ainfo.argv = argv ;
	ainfo.ai_max = ai_max ;
	ainfo.ai_pos = ai_pos ;

	if (rs >= 0) {
	    ARGINFO	*aip = &ainfo ;
	    BITS	*bop = &pargs ;
	    PARAMOPT	*pop = &aparams ;
	    cchar	*ofn = ofname ;
	    cchar	*afn = afname ;
	    rs = process(pip,aip,bop,pop,ofn,afn) ;
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
	        if (! pip->f.quiet) {
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
	} /* end if */

/* we are out of here */
retearly:
	if (pip->debuglevel > 0) {
	    bprintf(pip->efp,"%s: exiting ex=%u (%d)\n",
	        pip->progname,ex,rs) ;
	}

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    debugprintf("main: exiting ex=%u (%d)\n",ex,rs) ;
#endif

	if (pip->efp != NULL) {
	    pip->open.errfile = FALSE ;
	    bclose(pip->efp) ;
	    pip->efp = NULL ;
	}

	if (pip->open.aparams) {
	    pip->open.aparams = FALSE ;
	    paramopt_finish(&aparams) ;
	}

	bits_finish(&pargs) ;

badpargs:
	proginfo_finish(pip) ;

badprogstart:

#if	(CF_DEBUGS || CF_DEBUG) && CF_DEBUGMALL
	{
	    uint	mi[12] ;
	    uint	mo ;
	    uint	mdiff ;
	    uc_mallout(&mo) ;
	    mdiff = (mo-mo_start) ;
	    debugprintf("main: final mallout=%u\n",mdiff) ;
	    if (mdiff > 0) {
	        UCMALLREG_CUR	cur ;
	        UCMALLREG_REG	reg ;
	        const int	size = (10*sizeof(uint)) ;
	        const char	*ids = "main" ;
	        uc_mallinfo(mi,size) ;
	        debugprintf("main: MIoutnum=%u\n",mi[ucmallreg_outnum]) ;
	        debugprintf("main: MIoutnummax=%u\n",mi[ucmallreg_outnummax]) ;
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

/* bad stuff */
badarg:
	ex = EX_USAGE ;
	bprintf(pip->efp,"%s: invalid argument specified (%d)\n",
	    pip->progname,rs) ;
	usage(pip) ;
	goto retearly ;

}
/* end subroutine (main) */


int findbibfile(PROGINFO *pip,PARAMOPT *app,cchar *fname,char *tmpfname)
{
	int		rs = SR_OK ;
	int		fl = 0 ;

	if (pip == NULL) return SR_FAULT ;

	if (fname[0] != '/') {
	    USTAT		sb ;
	    PARAMOPT_CUR	cur ;
	    const char		*cp ;

	    rs = SR_NOENT ;
	    tmpfname[0] = '\0' ;
	    if ((paramopt_curbegin(app,&cur)) >= 0) {
	        cchar	*po = PO_BIBDIR ;

	        while (paramopt_enumvalues(app,po,&cur,&cp) >= 0) {

	            fl = mkpath2(tmpfname,cp,fname) ;

	            if (((rs = u_stat(tmpfname,&sb)) >= 0) && 
	                S_ISREG(sb.st_mode))
	                break ;

	        } /* end while */

	        paramopt_curend(app,&cur) ;
	    } /* end if */

	} else {
	    fl = mkpath1(tmpfname,fname) ;
	}

	return (rs >= 0) ? fl : rs ;
}
/* end subroutine (findbibfile) */


/* local subroutines */


static int usage(PROGINFO *pip)
{
	int		rs = SR_OK ;
	int		wlen = 0 ;
	const char	*pn = pip->progname ;
	const char	*fmt ;

	fmt = "%s: USAGE> %s [<file(s)> ...] [-af {<afile>|-}]\n" ;
	if (rs >= 0) rs = bprintf(pip->efp,fmt,pn,pn) ;
	wlen += rs ;

	fmt = "%s:  [-B <incdir(s)>] [-p <bibfile(s)>] [-V]\n" ;
	if (rs >= 0) rs = bprintf(pip->efp,fmt,pn) ;
	wlen += rs ;

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (usage) */


static int process(PROGINFO *pip,ARGINFO *aip,BITS *bop,PARAMOPT *pop,
		cchar *ofn,cchar *afn)
{
	vecstr		*flp = &pip->filenames ;
	const int	n = DEFNFILES ;
	int		rs ;
	int		rs1 ;
	cchar		*pn = pip->progname ;
	cchar		*fmt ;
	if ((rs = vecstr_start(flp,n,0)) >= 0) {
	    BDB		bibber ;
	    int		opts = 0 ;
	    if (pip->f.uniq) opts |= BDB_OUNIQ ;
	    if ((rs = bdb_start(&bibber,"Q",opts)) >= 0) {
	        if ((rs = loadbibfiles(pip,pop,&bibber)) >= 0) {
	            CITEDB	citer ;
	            if ((rs = citedb_start(&citer)) >= 0) {
	                bfile	*tfp = &pip->tf.tfile ;
	                cchar	*txfn = TMPFX ;
	                memset(&pip->tf,0,sizeof(PROGINFO_TMP)) ;
	                if ((rs = bopentmp(tfp,txfn,"rwc",0666)) >= 0) {
	                    BDB		*b = &bibber ;
	                    CITEDB	*c = &citer ;
	                    if ((rs = procargs(pip,aip,bop,pop,b,c,afn)) >= 0) {
	                        brewind(&pip->tf.tfile) ;
	                        rs = progoutfin(pip,&bibber,&citer,ofn) ;
	                        if ((pip->debuglevel > 0) && (rs >= 0)) {
	                            fmt = "%s: files=%u\n" ;
	                            bprintf(pip->efp,fmt,pn,pip->c_files) ;
	                        }
	                        if (rs < 0) {
	                            procereport(pip,rs) ;
	                        } /* end if */
	                    } /* end if (procargs) */
	                    rs1 = bclose(tfp) ;
	                    if (rs >= 0) rs = rs1 ;
	                } /* end if (bopentmp) */
	                rs1 = citedb_finish(&citer) ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (citedb) */
	        } /* end if (loadbibfiles) */
	        rs1 = bdb_finish(&bibber) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (hdb) */
	    rs1 = vecstr_finish(&pip->filenames) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (phase-ii processing) */
	return rs ;
}
/* end subroutine (process) */


static int procargs(PROGINFO *pip,ARGINFO *aip,BITS *bop,PARAMOPT *pop,
		BDB *bdp,CITEDB *cdp,cchar *afn)
{
	int		rs = SR_OK ;
	int		rs1 ;
	int		cl ;
	int		pan = 0 ;
	cchar		*cp ;
	cchar		*pn = pip->progname ;
	cchar		*fmt ;

	if (rs >= 0) {
	    int		ai ;
	    int		f ;
	    cchar	**argv = aip->argv ;
	    for (ai = 1 ; ai < aip->argc ; ai += 1) {
	        f = (ai <= aip->ai_max) && (bits_test(bop,ai) > 0) ;
	        f = f || ((ai > aip->ai_pos) && (argv[ai] != NULL)) ;
	        if (f) {
	            cp = argv[ai] ;
	            if (cp[0] != '\0') {
	                pan += 1 ;
	                pip->c_files += 1 ;
	                rs = progfile(pip,pop,bdp,cdp,cp) ;
	            }
	        }
	        if (rs < 0) break ;
	        pip->c_processed += 1 ;
	    } /* end for (looping through requested circuits) */
	} /* end if (ok) */

/* process any files in the argument filename list file */

	if ((rs >= 0) && (afn != NULL) && (afn[0] != '\0')) {
	    bfile	afile, *afp = &afile ;

	    if (afn[0] == '-') afn = BFILE_STDIN ;

	    if ((rs = bopen(afp,afn,"r",0666)) >= 0) {
	        const int	llen = LINEBUFLEN ;
	        int		len ;
	        char		lbuf[LINEBUFLEN + 1] ;

	        while ((rs = breadln(afp,lbuf,llen)) > 0) {
	            len = rs ;

	            if (lbuf[len - 1] == '\n') len -= 1 ;
	            lbuf[len] = '\0' ;

	            if ((cl = sfshrink(lbuf,len,&cp)) > 0) {
	                lbuf[(cp+cl)-lbuf] = '\0' ;
	                if (cp[0] != '#') {
	                    pan += 1 ;
	                    pip->c_files += 1 ;
	                    rs = progfile(pip,pop,bdp,cdp,cp) ;
	                }
	            }

	            if (rs < 0) break ;
	            pip->c_processed += 1 ;
	        } /* end while (reading lines) */

	        rs1 = bclose(afp) ;
	        if (rs >= 0) rs = rs1 ;
	    } else {
	        if (! pip->f.quiet) {
	            fmt = "%s: inaccessible argument-list (%d)\n" ;
	            bprintf(pip->efp,fmt,pn,rs) ;
	            bprintf(pip->efp,"%s: afile=%s\n",pn,afn) ;
	        }
	    } /* end if */

	} /* end if (processing file argument file list) */

	if ((rs >= 0) && (pan == 0)) {

	    cp = "-" ;
	    pan += 1 ;
	    pip->c_files += 1 ;
	    rs = progfile(pip,pop,bdp,cdp,cp) ;

	    if (rs >= 0)
	        pip->c_processed += 1 ;

	} /* end if */

	if ((rs >= 0) && (pan == 0) && (afn == NULL)) {
	    fmt = "%s: no files were specified\n" ;
	    rs = SR_INVALID ;
	    bprintf(pip->efp,fmt,pn) ;
	}

	return rs ;
}
/* end subroutine (procargs) */


static int procereport(PROGINFO *pip,int prs)
{
	int		rs = SR_OK ;
	cchar		*pn = pip->progname ;
	cchar		*fmt ;
	if (pip->f.uniq && (prs == SR_NOTUNIQ)) {
	    fmt = "%s: citation was not unique in DB (%d)\n" ;
	    bprintf(pip->efp,fmt,pn,prs) ;
	} else {
	    fmt = "%s: could not process citation (%d)\n" ;
	    bprintf(pip->efp,fmt,pn,prs) ;
	}
	return rs ;
}
/* end subroutine (procereport) */


static int loadbibfiles(PROGINFO *pip,PARAMOPT *app,BDB *bdbp)
{
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	const char	*po = PO_BIBFILE ;

	if ((rs = paramopt_havekey(app,po)) > 0) {
	    PARAMOPT_CUR	cur ;
	    if ((rs = paramopt_curbegin(app,&cur)) >= 0) {
		cchar		*cp ;
		char		tmpfname[MAXPATHLEN + 1] ;
	        while (paramopt_enumvalues(app,po,&cur,&cp) >= 0) {

	            rs1 = SR_OK ;
	            if (cp[0] != '/') {
	                rs1 = findbibfile(pip,app,cp,tmpfname) ;
	                cp = tmpfname ;
	            }

	            if (rs1 >= 0) {
	                c += 1 ;
	                rs = bdb_add(bdbp,cp) ;
	            }

		    if (rs < 0) break ;
	        } /* end while */
	        paramopt_curend(app,&cur) ;
	    } /* end if (paramopt-cur) */
	} /* end if (paramopt_havekey) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (loadbibfiles) */



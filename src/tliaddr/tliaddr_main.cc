/* tliaddr_main SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* program to print out a hexadecimal TPI address */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* switchable debug print-outs */
#define	CF_DEBUGMALL	1		/* debug memory-allocations */

/* revision history:

	= 1999-08-17, David A­D­ Morano
        This subroutine was taken from the LOGDIR/LOGNAME program (fist written
        for the SunOS 4.xx environment in 1989). This whole program is replacing
        serveral that did similar things in the past.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This little program creates and prints out a TLI address
	according to the parameters given.

	Synopsis:
	$ tliaddr -t inet rca listen
	\x00020ace000000000000000000000000
	$

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<netdb.h>
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<keyopt.h>		/* LIBUC */
#include	<bits.h>		/* LIBUC */
#include	<sockaddress.h>		/* LIBUC */
#include	<exitcodes.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */
#include	<libdebug.h>		/* LIBDEBUG |DEBUGPRINTF(3debug)| */
#include	<bfile.h>		/* LIBB */

#include	"tliaddr_config.h"


/* local defines */

#define	INTERFACENAMELEN	32
#define	FAMILYNAMELEN		32


/* external subroutines */

extern int	printhelp(void *,cchar *,cchar *,cchar *) noex ;
extern int	proginfo_setpiv(PI *,cchar *,const pivars *) noex ;
extern int	prograw(PI *,cchar *,cchar *,cchar *) noex ;
extern int	progsock(PI *,cchar *,cchar *,cchar *) noex ;


/* external variables */


/* forward references */

local int	usage(PI *) noex ;


/* local structures */


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
} ; /* end enum */

constexpr cpcchar	argopts[] = {
	"ROOT",
	"VERSION",
	"VERBOSE",
	"HELP",
	"sn",
	"af",
	"ef",
	"of",
	nullptr
} ; /* end array */

constexpr pivars	initvars = {
	VARPROGRAMROOT1,
	VARPROGRAMROOT2,
	VARPROGRAMROOT3,
	PROGRAMROOT,
	VARPRLOCAL
} ; /* end array */

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
} ; /* end array */

enum interfaces {
	interface_raw,
	interface_loopback,
	interface_socket,
	interface_overlast
} ; /* end enum */

static cchar	*interfaces[] = {
	"raw",
	"loopback",
	"socket",
	nullptr
} ; /* end array */


/* exported variables */


/* exported subroutines */

int main(int argc,con mainv argv,con mainv envv) {
	PI		pi, *pip = &pi ;
	bits		pargs ;
	keyopt		akopts ;
	bfile		errfile ;
	bfile		outfile, *ofp = &outfile ;

#if	CF_DEBUG && CF_DEBUGMALL
	uint		mo_start = 0 ;
#endif

	int		argr, argl, aol, akl, avl, kwi ;
	int		ai, ai_max, ai_pos ;
	int		pan = 0 ;
	int		rs, rs1 ;
	int		sl ;
	int		interface ;
	int		ex = EX_INFO ;
	int		f_optminus, f_optplus, f_optequal ;
	int		f_version = FALSE ;
	int		f_help = FALSE ;
	int		f_usage = FALSE ;
	int		f ;

	cchar	*argp, *aop, *akp, *avp ;
	cchar	*argval = nullptr ;
	cchar	*pr = nullptr ;
	cchar	*sn = nullptr ;
	cchar	*afname = nullptr ;
	cchar	*efname = nullptr ;
	cchar	*ofname = nullptr ;
	cchar	*netaddr1 = nullptr ;
	cchar	*netaddr2 = nullptr ;
	cchar	*interfacename = nullptr ;
	cchar	*familyname = nullptr ;
	cchar	*cp ;
	char		interfacebuf[INTERFACENAMELEN + 1] ;


#if	CF_DEBUG
	if ((cp = getourenv(envv,VARDEBUGFNAME)) != nullptr) {
	    rs = DEBUGOPEN(cp) ;
	    DEBUGPRINTF("starting DFD=%d\n",rs) ;
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

/* initialize */

	pip->ofp = ofp ;
	pip->verboselevel = 1 ;

/* start parsing the arguments */

	rs = keyopt_start(&akopts) ;
	pip->open.akopts = (rs >= 0) ;

	if (rs >= 0) rs = bits_start(&pargs,1) ;
	if (rs < 0) goto badpargs ;

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

/* version */
	                case argopt_version:
	                    f_version = TRUE ;
	                    if (f_optequal)
	                        rs = SR_INVALID ;
	                    break ;

/* verbose mode */
	                case argopt_verbose:
	                    pip->verboselevel = 2 ;
	                    if (argr > 0) {
	                        argp = argv[++ai] ;
	                        argr -= 1 ;
	                        argl = strlen(argp) ;
	                        if (argl) {
	                            rs = optvalue(argp,argl) ;
	                            pip->verboselevel = rs ;
	                        }
	                    } else
	                        rs = SR_INVALID ;
	                    break ;

/* program-root */
	                case argopt_root:
	                    if (argr > 0) {
	                        argp = argv[++ai] ;
	                        argr -= 1 ;
	                        argl = strlen(argp) ;
	                        if (argl)
	                            pip->pr = argp ;
	                    } else
	                        rs = SR_INVALID ;
	                    break ;

	                case argopt_help:
	                    f_help = TRUE ;
	                    break ;

/* program search name */
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

/* argument list file */
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

	                    case 'Q':
	                        pip->fl.quiet = TRUE ;
	                        break ;

/* version */
	                    case 'V':
	                        f_version = TRUE ;
	                        break ;

	                    case 'a':
	                        pip->fl.all = TRUE ;
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

/* port (for INET) */
	                    case 'p':
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl)
	                                netaddr2 = argp ;
	                        } else
	                            rs = SR_INVALID ;
	                        break ;

/* quiet mode */
	                    case 'q':
	                        pip->verboselevel = 0 ;
	                        break ;

/* transport address family */
	                    case 't':
	                    case 'f':
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl)
	                                familyname = argp ;
	                        } else
	                            rs = SR_INVALID ;
	                        break ;

/* transport interface address type */
	                    case 'i':
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl)
	                                interfacename = argp ;
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

	if (efname == nullptr) efname = getenv(VAREFNAME) ;
	if (efname == nullptr) efname = BFILE_STDERR ;
	if ((rs1 = bopen(&errfile,efname,"wca",0666)) >= 0) {
	    pip->efp = &errfile ;
	    pip->open.errfile = TRUE ;
	    bcontrol(&errfile,BC_SETBUFLINE,TRUE) ;
	} else if (! isNotPresent(rs1)) {
	    if (rs >= 0) rs = rs1 ;
	}

	if (rs < 0)
	    goto badarg ;

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    DEBUGPRINTF("debuglevel=%u\n",pip->debuglevel) ;
#endif

	if (f_version) {
	    bprintf(pip->efp,"%s: version %s\n",
	        pip->progname,VERSION) ;
	}

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

/* initialized */

	if (afname == nullptr) afname = getenv(VARAFNAME) ;

	if ((ofname == nullptr) || (ofname[0] == '\0')) ofname = BFILE_STDOUT ;

	if (rs >= 0) {
	    rs = proginfo_setentry(pip,&pip->ofname,ofname,-1) ;
	}

/* get the first positional argument as the username to key on */

	if (rs >= 0) {
	    for (ai = 1 ; ai < argc ; ai += 1) {

	        f = (ai <= ai_max) && (bits_test(&pargs,ai) > 0) ;
	        f = f || ((ai > ai_pos) && (argv[ai] != nullptr)) ;
	        if (f) {
	            cp = argv[ai] ; /* no NUL check on purpose */
	            switch (pan) {
	            case 0:
	                netaddr1 = cp ;
	                break ;
	            case 1:
	                netaddr2 = cp ;
	                break ;
	            } /* end switch */
	            pan += 1 ;
	        } /* end if */

	    } /* end for */
	} /* end if */

#ifdef	COMMENT
	if ((netaddr1 == nullptr) || (netaddr1[0] == '\0'))
	    rs = SR_INVALID ;
#else
	if ((netaddr1 == nullptr) || (netaddr1[0] == '\0'))
	    netaddr1 = "anyhost" ;
#endif

#if	CF_DEBUG
	if (pip->debuglevel > 1) {
	    DEBUGPRINTF("netaddr1=%s\n",netaddr1) ;
	    DEBUGPRINTF("netaddr2=%s\n",netaddr2) ;
	}
#endif

/* check some stuff */

	if (interfacename == nullptr)
	    interfacename = DEFINTERFACE ;

	if ((cp = strchr(interfacename,':')) != nullptr) {

	    sl = MIN((cp - interfacename),INTERFACENAMELEN) ;
	    strwcpy(interfacebuf,interfacename,sl) ;

	    interfacename = interfacebuf ;
	    if (cp[1] != '\0')
	        familyname = (cp + 1) ;

	} /* end if */

#ifdef	COMMENT
	if (familyname == nullptr) familyname = DEFSOCKFAMILY ;
#endif

	if (pip->debuglevel > 0) {
	    bprintf(pip->efp,
	        "%s: interface=%s family=%s\n",
	        pip->progname,interfacename,familyname) ;
	}

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    DEBUGPRINTF("interface=%s family=%s\n",
	        interfacename,familyname) ;
#endif

	if (rs >= 0) {
	    if ((interface = matostr(interfaces,1,interfacename,-1)) >= 0) {
	        switch (interface) {
	        case interface_raw:
	        case interface_loopback:
	            rs = prograw(pip,familyname,netaddr1,netaddr2) ;
	            break ;
	        case interface_socket:
	            rs = progsock(pip,familyname,netaddr1,netaddr2) ;
	            break ;
	        default:
	            rs = SR_NOTSUP ;
	            break ;
	        } /* end switch */
	    } else {
	        ex = EX_USAGE ;
	        bprintf(pip->efp,"%s: unrecognized interface address type\n",
	            pip->progname) ;
	    } /* end if (interface) */
	} /* end if (ok) */

/* done */
	if ((rs < 0) && (ex == EX_OK)) {
	    switch (rs) {
	    case SR_NOTFOUND:
	        ex = EX_NOTFOUND ;
	        break ;
	    case SR_NOTSUP:
	        ex = EX_UNAVAILABLE ;
	        break ;
	    default:
	        ex = mapex(mapexs,rs) ;
	        break ;
	    } /* end switch */
	} /* end if */

retearly:
	if (pip->debuglevel > 0) {
	    bprintf(pip->efp,"%s: exiting ex=%u (%d)\n",
	        pip->progname,ex,rs) ;
	}

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    DEBUGPRINTF("exiting ex=%u (%d)\n",ex,rs) ;
#endif

	if (pip->efp != nullptr) {
	    pip->open.errfile = FALSE ;
	    bclose(pip->efp) ;
	    pip->efp = nullptr ;
	}

	if (pip->open.akopts) {
	    keyopt_finish(&akopts) ;
	    pip->open.akopts = FALSE ;
	}

	bits_finish(&pargs) ;

badpargs:
	proginfo_finish(pip) ;

badprogstart:

#if	CF_DEBUG && CF_DEBUGMALL
	{
	    uint	mo ;
	    uc_mallout(&mo) ;
	    DEBUGPRINTF("final mallout=%u\n",(mo-mo_start)) ;
	    uc_mallset(0) ;
	}
#endif /* CF_DEBUGMALL */

#if	CF_DEBUG
	DEBUGCLOSE() ;
#endif

	return ex ;

/* the bad things */
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

	fmt = "%s: USAGE> %s [<hostname> [<service>]]\n" ;
	if (rs >= 0) rs = bprintf(pip->efp,fmt,pn,pn) ;
	wlen += rs ;

	fmt = "%s:  [-i <interface>[:<family>]]\n" ;
	if (rs >= 0) rs = bprintf(pip->efp,fmt,pn) ;
	wlen += rs ;

	fmt = "%s:  [-f <family>] [-p <inetport>]\n" ;
	if (rs >= 0) rs = bprintf(pip->efp,fmt,pn) ;
	wlen += rs ;

	fmt = "%s:  [-Q] [-D] [-v[=n]] [-V]\n" ;
	if (rs >= 0) rs = bprintf(pip->efp,fmt,pn) ;
	wlen += rs ;

	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (usage) */



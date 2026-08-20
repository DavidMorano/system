/* discard_main SUPPORT (discard) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* generic (pretty much) front-end program subroutine */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* compile-time */
#define	CF_DEBUG	0		/* run-time */
#define	CF_PILOGFNAME	0		/* ? */

/* revision history:

	= 1994-09-01, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1994 David A­D­ Morano.  All rights reserved. */

/*****************************************************************************

  	Name:
	main

	Description:
	This subroutine forms the front-end part of a generic PCS
	type of program.  This front-end is used in a variety of
	PCS programs.  This code was originally part of the
	Personal Communications Services (PCS) package but can also
	be used independently from it.  Historically, this was
	developed as part of an effort to maintain high function
	(and reliable) email communications in the face of increasingly
	draconian security restrictions imposed on the computers
	in the DEFINITY development organization.

*****************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<sys/socket.h>
#include	<sys/utsname.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<netdb.h>
#include	<grp.h>
#include	<ctime>
#include	<climits>
#include	<cstdlib>
#include	<cstring>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<getax.h>
#include	<getusername.h>
#include	<mallocxx.h>
#include	<mallocstuff.h>
#include	<baops.h>
#include	<bfile.h>
#include	<field.h>
#include	<logfile.h>
#include	<vecstr.h>
#include	<userinfo.h>
#include	<ascii.h>
#include	<varsub.h>
#include	<hostent.h>
#include	<storebuf.h>
#include	<vstrcmp.h>		/* |vstrkeycmp(3uc)| */
#include	<char.h>
#include	<exitcodes.h>
#include	<localmisc.h>

#include	"config.h"
#include	"defs.h"
#include	"configfile.h"


/* local defines */

#define	MAXARGINDEX	100
#define	MAXARGGROUPS	(MAXARGINDEX/8 + 1)

#ifndef	REALNAMELEN
#define	REALNAMELEN	100
#endif

#ifndef	BUFLEN
#define	BUFLEN		((8 * 1024) + REALNAMELEN)
#endif


/* external subroutines */

extern int	expander(PROGINFO *,char *,int,char *,int) ;
extern int	procfileenv(char *,char *,vecstr *) ;
extern int	procfilepaths(char *,char *,vecstr *) ;
extern int	process_input(PROGINFO *,int) ;


/* externals variables */

extern cchar	makedate[] ;


/* local global variabes */


/* local structures */

struct openstuff {
	uint	lists : 1 ;
	uint	svars : 1 ;
} ; /* end struct */


/* forward references */

static int	getlocalnames(PROGINFO *) ;
static int	procfile(PROGINFO *,int (*)(char *,char *,vecstr *),
			char *,vecstr *,char *,vecstr *) ;


/* local variables */

enum argopts {
	argopt_tmpdir,
	argopt_version,
	argopt_verbose,
	argopt_root,
	argopt_logfile,
	argopt_config,
	argopt_help,
	argopt_af,
	argopt_dgram,
	argopt_overlast
} ; /* end enum */

constexpr cpcchar	argopts[] = {
	"TMPDIR",
	"VERSION",
	"VERBOSE",
	"ROOT",
	"LOGFILE",
	"CONFIG",
	"HELP",
	"af",
	"dgram",
	nullptr
} ; /* end array */

enum configopts {
	configopt_marktime,
	configopt_sumfile,
	configopt_minpingint,
	configopt_minupdate,
	configopt_overlast
} ; /* end enum */

constexpr cpcchar	configopts[] = {
	"marktime",
	"sumfile",
	"minpingint",
	"minupdate",
	nullptr
} ; /* end array */

/* 'conf' for most regular programs */
constexpr cpcchar	sched1[] = {
	"%p/%e/%n/%n.%f",
	"%p/%e/%n/%f",
	"%p/%e/%n.%f",
	"%p/%n.%f",
	nullptr
} ; /* end array */

/* non-'conf' ETC stuff for all regular programs */
constexpr cpcchar	sched2[] = {
	"%p/%e/%n/%n.%f",
	"%p/%e/%n/%f",
	"%p/%e/%n.%f",
	"%p/%e/%f",
	"%p/%n.%f",
	nullptr
} ; /* end array */

/* 'conf' and non-'conf' ETC stuff for local searching */
constexpr cpcchar	sched3[] = {
	"%e/%n/%n.%f",
	"%e/%n/%f",
	"%e/%n.%f",
	"%e/%f",
	"%n.%f",
	"%f",
	nullptr
} ; /* end array */

constexpr char		oterms[] = {
	0x00, 0x0B, 0x00, 0x00,
	0x09, 0x10, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
} ; /* end array */


/* exported variables */


/* exported subroutines */

int main(int argc,mainv argv,mainv envv) {
	PROGINFO	pi, *pip = &pi ;
	openstuff	oo ;
	ustat		sb ;
	GROUP		ge ;
	userinfo		u ;

	bfile		errfile, *efp = &errfile ;
	bfile		argfile, *afp = &argfile ;
	bfile		outfile ;
	bfile		pidfile ;
	bfile		logfile ;
	bfile		sumfile ;

	CONFIGFILE	cf ;

	vecstr		defines, unsets, exports ;
	vecstr		svars ;

	VARSUB		vsh_e, vsh_d ;

	time_t	daytime ;

	int	argr, argl, aol, akl, avl ;
	int	maxai, pan, npa, kwi, i ;
	int	f_optminus, f_optplus, f_optequal ;
	int	f_extra = FALSE ;
	int	len, rs ;
	int	ex = EX_INFO ;
	int	f_version = FALSE ;
	int	f_makedate = FALSE ;
	int	f_usage = FALSE ;
	int	f_input = FALSE ;
	int	loglen = -1 ;
	int	to = -1 ;
	int	l2, sl, sl2 ;
	int	iw ;
	int	val, val1, val2 ;
	int	logfile_type = -1 ;
	int	c_pingtabs = 0 ;
	int	c_hosts = 0 ;
	int	c_processed = 0 ;
	int	c_up = 0 ;
	int	f_programroot = FALSE ;
	int	f_freeconfigfname = FALSE ;
	int	f_procfileenv = FALSE ;
	int	f_procfilepaths = FALSE ;
	int	f_help = FALSE ;
	int	f_minpingint = FALSE ;
	int	argnum, cl ;
	int	fd_debug ;

	char	*argp, *aop, *akp, *avp ;
	char	argpresent[MAXARGGROUPS] ;
	char	*programroot = nullptr ;
	char	buf[BUFLEN + 1] ;
	char	buf2[BUFLEN + 1] ;
	char	userbuf[USERINFO_LEN + 1] ;
	char	nodename[NODENAMELEN + 1], domainname[MAXHOSTNAMELEN + 1] ;
	char	tmpfname[MAXPATHLEN + 1] ;
	char	pidfname[MAXPATHLEN + 1] ;
	char	lockfname[MAXPATHLEN + 1] ;
	char	logfname[MAXPATHLEN + 1] ;
	char	helpfname[MAXPATHLEN + 1] ;
	char	timebuf[TIMEBUFLEN + 1] ;
	char	*configfname = nullptr ;
	char	*afname = nullptr ;
	char	*cp ;

#if	CF_DEBUGS || CF_DEBUG
	if ((cp = getourenv(envv,VARDEBUGFNAME)) != nullptr) {
	    rs = debugopen(cp) ;
	    debugprintf("main: starting DFD=%d\n",rs) ;
	}
#endif /* CF_DEBUGS */

	memset(pip,0,sizeof(PROGINFO)) ;

	if (bopen(efp,BFILE_STDERR,"dwca",0666) >= 0) {
	    pip->efp = efp ;
	    bcontrol(efp,BC_LINEBUF,0) ;
	}

	memset(&oo,0,sizeof(struct openstuff)) ;

	pip->version = VERSION ;
	pip->progname = strbasename(argv[0]) ;

	pip->pid = getpid() ;

	pip->ppid = pip->pid ;

/* initialize */

	pip->fl.quiet = FALSE ;
	pip->fl.log = FALSE ;
	pip->fl.slog = FALSE ;
	pip->fl.update = FALSE ;
	pip->fl.dgram = FALSE ;

	pip->debuglevel = 0 ;
	pip->verboselevel = 1 ;

	pip->markint = -1 ;
	pip->minpingint = -1 ;
	pip->minupdate = -1 ;
	pip->mininputint = -1 ;

	pip->pr = nullptr ;
	pip->username = nullptr ;
	pip->groupname = nullptr ;
	pip->pidfname = nullptr ;
	pip->lockfname = nullptr ;
	pip->sumfname = nullptr ;

	pip->tmpdname = nullptr ;
	pip->workdname = nullptr ;
	pip->homedname = nullptr ;

	pip->lfp = &logfile ;
	pip->sumfp = nullptr ;

#if	CF_PILOGFNAME
	pip->logfname = nullptr ;
#endif

	pidfname[0] = '\0' ;
	lockfname[0] = '\0' ;
	logfname[0] = '\0' ;
	helpfname[0] = '\0' ;

	{
	    TM		st, *stp ;
	    int		zo ;
	    char	*tznp ;
	    uc_ftime(&pip->now) ;
	    stp = &st ;
	    uc_timelocal(&pip->now.time,&st) ;
	    zo = intconv(stp->tm_gmtoff) ;
	    pip->now.timezone = zo / 60 ;
	    pip->now.dstflag = stp->tm_idst ;
	    tznp = stp->tm_zone ;
	    rs = pip->znameset(tznp) ;
	} /* end block (getting some current time stuff) */

/* start parsing the arguments */

	for (i = 0 ; i < MAXARGGROUPS ; i += 1) argpresent[i] = 0 ;

	npa = 0 ;			/* number of positional so far */
	maxai = 0 ;
	i = 0 ;
	argr = argc - 1 ;
	while (argr > 0) {

	    argp = argv[++i] ;
	    argr -= 1 ;
	    argl = strlen(argp) ;

	    f_optminus = (*argp == '-') ;
	    f_optplus = (*argp == '+') ;
	    if ((argl > 0) && (f_optminus || f_optplus)) {

	        if (argl > 1) {
		    const int	ach = MKCHAR(argp[1]) ;

	            if (isdigitlatin(ach)) {

	                if (((argl - 1) > 0) && 
	                    (cfdeci(argp + 1,argl - 1,&argnum) < 0))
	                    goto badargval ;

	            } else {

	                aop = argp + 1 ;
	                aol = argl - 1 ;
	                akp = aop ;
	                f_optequal = FALSE ;
	                if ((avp = strchr(aop,'=')) != nullptr) {

	                    akl = avp - aop ;
	                    avp += 1 ;
	                    avl = aop + aol - avp ;
	                    f_optequal = TRUE ;

	                } else {

	                    akl = aol ;
	                    avl = 0 ;

	                } /* end if */

	                if ((kwi = matstr(argopts,akp,akl)) >= 0) {

	                    switch (kwi) {

	                    case argopt_tmpdir:
	                        if (f_optequal) {

	                            f_optequal = FALSE ;
	                            if (avl)
	                                pip->tmpdname = avp ;

	                        } else {

	                            if (argr <= 0)
	                                goto badargnum ;

	                            argp = argv[++i] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;

	                            if (argl)
	                                pip->tmpdname = argp ;

	                        }
	                        break ;

/* version */
	                    case argopt_version:
				f_makedate = f_version ;
	                        f_version = TRUE ;
	                        if (f_optequal)
	                            goto badargextra ;

	                        break ;

/* verbose mode */
	                    case argopt_verbose:
	                        pip->verboselevel = 2 ;
	                        if (f_optequal) {

	                            f_optequal = FALSE ;
	                            if (avl) {

	                                rs = cfdeci(avp,avl,
	                                    &pip->verboselevel) ;

	                                if (rs < 0)
	                                    goto badargval ;

	                            }
	                        }

	                        break ;

/* program root */
	                    case argopt_root:
	                        if (f_optequal) {

	                            f_optequal = FALSE ;
	                            if (avl)
	                                pip->pr = avp ;

	                        } else {

	                            if (argr <= 0)
	                                goto badargnum ;

	                            argp = argv[++i] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;

	                            if (argl)
	                                pip->pr = argp ;

	                        }

	                        break ;

/* configuration file */
	                    case argopt_config:
	                        if (f_optequal) {

	                            f_optequal = FALSE ;
	                            if (avl)
	                                configfname = avp ;

	                        } else {

	                            if (argr <= 0)
	                                goto badargnum ;

	                            argp = argv[++i] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;

	                            if (argl)
	                                configfname = argp ;

	                        }

	                        break ;

	                    case argopt_af:
	                        if (f_optequal) {

	                            f_optequal = FALSE ;
	                            if (avl)
	                                afname = avp ;

	                        } else {

	                            if (argr <= 0)
	                                goto badargnum ;

	                            argp = argv[++i] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;

	                            if (argl)
	                                afname = argp ;

	                        }

	                        break ;

/* log file */
	                    case argopt_logfile:
	                        if (f_optequal) {

	                            f_optequal = FALSE ;
	                            if (avl) {

	                                logfile_type = 0 ;
	                                strwcpy(logfname,avp,avl) ;

	                            }

	                        } else {

	                            if (argr <= 0)
	                                goto badargnum ;

	                            argp = argv[++i] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;

	                            if (argl)  {

	                                logfile_type = 0 ;
	                                strwcpy(logfname,argp,argl) ;

	                            }
	                        }

	                        break ;

	                    case argopt_dgram:
	                        pip->fl.dgram = TRUE ;
	                        break ;

	                    case argopt_help:
	                        f_help = TRUE ;
	                        break ;

/* handle all keyword defaults */
	                    default:
	                        bprintf(efp,"%s: option (%s) not supported\n",
	                            pip->progname,akp) ;

	                        goto badargunk ;

	                    } /* end switch */

	                } else {

#if	CF_DEBUGS
	                    debugprintf("main: got an option key letter\n") ;
#endif

	                    while (akl--) {

#if	CF_DEBUGS
	                        debugprintf("main: option key letters\n") ;
#endif

	                        switch ((int) *akp) {

/* debug */
	                        case 'D':
	                            pip->debuglevel = 1 ;
	                            if (f_optequal) {

	                                f_optequal = FALSE ;
	                                if (avl) {

	                                    rs = cfdeci(avp,avl,
	                                        &pip->debuglevel) ;

	                                    if (rs < 0)
	                                        goto badargval ;

	                                }
	                            }

	                            break ;

/* version */
	                        case 'V':
				    f_makedate = f_version ;
	                            f_version = TRUE ;
	                            break ;

/* configuration file */
	                        case 'C':
	                            if (argr <= 0)
	                                goto badargnum ;

	                            argp = argv[++i] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;

	                            if (argl)
	                                configfname = argp ;

	                            break ;

/* mutex lock PID file */
	                        case 'P':
	                            if (argr <= 0)
	                                goto badargnum ;

	                            argp = argv[++i] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;

	                            if (argl)
	                                strwcpy(pidfname,argp,argl) ;

	                            break ;

/* binary dump output file */
	                        case 'b':
	                            pip->fl.binary = TRUE ;
	                            break ;

/* take input file arguments from STDIN */
	                        case 'f':
	                            if (argr <= 0)
	                                goto badargnum ;

	                            argp = argv[++i] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;

	                            afname = argp ;
	                            break ;

/* input mode */
				case 'i':
				    f_input = TRUE ;
	                            if (f_optequal) {

	                                f_optequal = FALSE ;
	                                if (avl) {

	                                    rs = cfdeci(avp,avl,
	                                        &pip->mininputint) ;

	                                    if (rs < 0)
	                                        goto badargval ;

	                                } else
						pip->mininputint = -2 ;

	                            }

				    break ;

/* minimum ping interval */
	                        case 'm':
	                            if (argr <= 0)
	                                goto badargnum ;

#if	CF_DEBUGS
	                            debugprintf("main: got 'm'\n") ;
#endif

	                            argp = argv[++i] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;

	                            if (argl) {

	                                rs = cfdecti(argp,argl,&val) ;

	                                if (rs < 0)
	                                    goto badargval ;

	                                f_minpingint = TRUE ;
	                                pip->minpingint = val ;
	                            }

	                            break ;

/* quiet mode */
	                        case 'q':
	                            pip->fl.quiet = TRUE ;
	                            break ;

/* receive mode */
	                        case 'r':
	                            pip->fl.receive = TRUE ;
	                            break ;

/* default timeout */
	                        case 't':
	                            if (argr <= 0)
	                                goto badargnum ;

	                            argp = argv[++i] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;

	                            if (argl) {

	                                rs = cfdecti(argp,argl,&val) ;

	                                if (rs < 0)
	                                    goto badargval ;

	                                to = val ;
	                            }

	                            break ;

/* update mode */
	                        case 'u':
	                            pip->fl.update = TRUE ;
	                            break ;

/* verbose mode */
	                        case 'v':
	                            pip->verboselevel = 2 ;
	                            if (f_optequal) {

	                                f_optequal = FALSE ;
	                                if (avl) {

	                                    rs = cfdeci(avp,avl,
	                                        &pip->verboselevel) ;

	                                    if (rs < 0)
	                                        goto badargval ;

	                                }
	                            }

	                            break ;

	                        case '?':
	                            f_usage = TRUE ;
	                            break ;

	                        default:
	                            ex = EX_USAGE ;
	                            f_usage = TRUE ;
	                            bprintf(efp,"%s: unknown option - %c\n",
	                                pip->progname,*aop) ;

	                        } /* end switch */

	                        akp += 1 ;

	                    } /* end while */

	                } /* end if (individual option key letters) */

	            } /* end if (digits as argument or not) */

	        } else {

/* we have a plus or minux sign character alone on the command line */

	            if (i < MAXARGINDEX) {

	                BASET(argpresent,i) ;
	                maxai = i ;
	                npa += 1 ;	/* increment position count */

	            }

	        } /* end if */

	    } else {

	        if (i < MAXARGINDEX) {

	            BASET(argpresent,i) ;
	            maxai = i ;
	            npa += 1 ;

	        } else {

	            if (! f_extra) {

	                f_extra = TRUE ;
	                bprintf(efp,"%s: extra arguments ignored\n",
	                    pip->progname) ;

	            }
	        }

	    } /* end if (key letter/word or positional) */

	} /* end while (all command line argument processing) */

	if (pip->debuglevel > 0)
	    bprintf(efp,"%s: debuglevel=%u\n",
	        pip->progname,pip->debuglevel) ;


	if (f_version) {
	    bprintf(efp,"%s: version %s\n",pip->progname,VERSION) ;
	}

	if (f_usage)
	    goto usage ;

	if (f_version)
	    goto retearly ;

#if	CF_DEBUG
	if (pip->debuglevel >= 4) {
	    debugprintf("main: minpingint=%d to=%d\n",
	        pip->minpingint,to) ;
	}
#endif


/* get our program root */

	if (pip->pr == nullptr) {

	    programroot = getenv(VARPROGRAMROOT1) ;

	    if (programroot == nullptr)
	        programroot = getenv(VARPROGRAMROOT2) ;

	    if (programroot == nullptr)
	        programroot = getenv(VARPROGRAMROOT3) ;

	    if (programroot == nullptr)
	        programroot = PROGRAMROOT ;

	    pip->pr = programroot ;

	} else {

	    f_programroot = TRUE ;
	    programroot = pip->pr ;

	}

/* get some host/user information */

	rs = userinfo(&u,userbuf,USERINFO_LEN,nullptr) ;

#if	CF_DEBUG
	if (pip->debuglevel > 1)
	    debugprintf("main: name=\"%s\"\n",u.name) ;
#endif

	pip->nodename = u.nodename ;
	pip->domainname = u.domainname ;
	pip->username = u.username ;
	pip->homedname = u.homedname ;

	if (rs < 0) {

	    getnodedomain(nodename,domainname) ;

#if	CF_DEBUG
	    if (pip->debuglevel > 1)
	        debugprintf("main: got node/domain\n") ;
#endif

	    pip->nodename = nodename ;
	    pip->domainname = domainname ;

#if	CF_DEBUG
	    if (pip->debuglevel > 1)
	        debugprintf("main: about to get username\n") ;
#endif

	    getusername(buf,USERNAMELEN,-1) ;

#if	CF_DEBUG
	    if (pip->debuglevel > 1)
	        debugprintf("main: goto username\n") ;
#endif

	    pip->username = mallocstr(buf) ;

#if	CF_DEBUG
	    if (pip->debuglevel > 1)
	        debugprintf("main: done w/ name stuff\n") ;
#endif

	} /* end if (got some user information or not) */


/* PWD */

#ifdef	COMNMENT
	(void) getpwd(buf,BUFLEN) ;
	pip->pwd = mallocstr(buf) ;
#endif

/* get the local names for this host */

	rs = vecstr_start(&pip->localnames,5,0) ;
	if (rs >= 0)
		(void) getlocalnames(pip) ;

/* create the values for the file schedule searching */

	vecstr_start(&svars,6,0) ;

	vecstr_envset(&svars,"p",pip->pr,-1) ;

	vecstr_envset(&svars,"e","etc",-1) ;

	vecstr_envset(&svars,"n",SEARCHNAME,-1) ;


/* prepare to store configuration variable lists */

	cint vo = vecstrm.ordered ;
	if ((rs = vecstr_start(&defines,10,vo)) < 0)
	    goto badlistinit ;

	cint vo = vecstrm.compact ;
	if ((rs = vecstr_start(&unsets,10,vo)) < 0) {
	    vecstr_finish(&defines) ;
	    goto badlistinit ;
	}

	if ((rs = vecstr_start(&exports,10,vo)) < 0) {
	    vecstr_finish(&defines) ;
	    vecstr_finish(&unsets) ;
	    goto badlistinit ;
	}

	oo.lists = TRUE ;


/* load up some initial environment that everyone should have ! */

#if	CF_DEBUG
	if (pip->debuglevel > 1)
	    debugprintf("main: about to do DEFINITFNAME=%s\n",DEFINITFNAME) ;
#endif

	procfileenv(pip->pr,DEFINITFNAME,&exports) ;


/* find a configuration file if we have one */

#if	CF_DEBUG
	if (pip->debuglevel > 1) {
	    debugprintf("main: checking for configuration file\n") ;
	    debugprintf("main: 0 CF=%s\n",configfname) ;
	}
#endif /* CF_DEBUG */

	rs = SR_NOEXIST ;
	if ((configfname == nullptr) || (configfname[0] == '\0')) {

	    configfname = CONFIGFNAME ;

	    if ((sl = permsched(sched1,&svars,
	        tmpfname,MAXPATHLEN, configfname,R_OK)) < 0) {

	        if ((sl = permsched(sched3,&svars,
	            tmpfname,MAXPATHLEN configfname,R_OK)) > 0)
	            configfname = tmpfname ;

	    } else if (sl > 0)
	        configfname = tmpfname ;

	    rs = sl ;

	} else {

	    sl = getfname(pip->pr,configfname,1,tmpfname) ;

	    if (sl > 0)
	        configfname = tmpfname ;

	    rs = sl ;		/* cause an open failure later */

	} /* end if */

	if (configfname == tmpfname) {

	    f_freeconfigfname = TRUE ;
	    configfname = mallocstr(tmpfname) ;

	}

#if	CF_DEBUG
	if (pip->debuglevel > 1) {
	    debugprintf("main: find rs=%d\n",sl) ;
	    debugprintf("main: 1 CF=%s\n",configfname) ;
	}
#endif /* CF_DEBUG */

/* read in the configuration file if we have one */

	if ((rs >= 0) || (perm(configfname,-1,-1,nullptr,R_OK) >= 0)) {
	    FIELD	fsb ;

#if	CF_DEBUG
	    if (pip->debuglevel > 1)
	        debugprintf("main: configuration file \"%s\"\n",
	            configfname) ;
#endif

	    if (pip->debuglevel > 0)
	        bprintf(efp,"%s: configfile=%s\n",
	            pip->progname,configfname) ;

	    if ((rs = configfile_start(&cf,configfname)) < 0)
	        goto badconfig ;

#if	CF_DEBUG
	    if (pip->debuglevel > 1)
	        debugprintf("main: we have a good configuration file\n") ;
#endif

#if	CF_DEBUG
	    if (pip->debuglevel > 1)
	        debugprintf("main: varsub_start d\n") ;
#endif

/* we must set this mode to 'VARSUB_MBADNOKEY' so that a miss is noticed */

	    varsub_start(&vsh_d,VARSUB_MBADNOKEY) ;

#if	CF_DEBUG
	    if (pip->debuglevel > 1)
	        debugprintf("main: varsub_start e\n") ;
#endif

	    varsub_start(&vsh_e,0) ;

#if	CF_DEBUG
	    if (pip->debuglevel > 1)
	        debugprintf("main: varsub_addva\n") ;
#endif

	    varsub_addva(&vsh_e,envv) ;

#if	CF_DEBUG
	    if (pip->debuglevel > 1) {
	        debugprintf("main: 0 for\n") ;
	        varsub_dumpfd(&vsh_e,-1) ;
	    }
#endif /* CF_DEBUG */


/* program root from configuration file */

	    if ((cf.root != nullptr) && (! f_programroot)) {

#if	CF_DEBUG
	        if (pip->debuglevel > 1)
	            debugprintf("main: processing CF programroot\n") ;
#endif

	        if (((sl = varsub_subbuf(&vsh_d,&vsh_e,cf.root,
	            -1,buf,BUFLEN)) > 0) &&
	            ((l2 = expander(pip,buf,sl,buf2,BUFLEN)) > 0)) {

	            pip->pr = mallocstrw(buf2,l2) ;

	        }

	    } /* end if (configuration file program root) */


/* loop through the DEFINEd variables in the configuration file */

	    for (i = 0 ; vecstr_get(&cf.defines,i,&cp) >= 0 ; i += 1) {

#if	CF_DEBUG
	        if (pip->debuglevel > 1)
	            debugprintf( "main: 0 top, cp=%s\n",cp) ;
#endif

	        if (((sl = varsub_subbuf(&vsh_d,&vsh_e,cp,
	            -1,buf,BUFLEN)) > 0) &&
	            ((l2 = expander(pip,buf,sl,buf2,BUFLEN)) > 0)) {

#if	CF_DEBUG
	            if (pip->debuglevel > 1)
	                debugprintf("main: 0 about to merge\n") ;
#endif

	            varsub_merge(&vsh_d,&defines,buf2,l2) ;

#if	CF_DEBUG
	            if (pip->debuglevel > 1)
	                debugprintf("main: 0 out of merge\n") ;
#endif

	        } /* end if */

#if	CF_DEBUG
	        if (pip->debuglevel > 1)
	            debugprintf("main: 0 bot\n") ;
#endif

	    } /* end for (defines) */

#if	CF_DEBUG
	    if (pip->debuglevel > 1)
	        debugprintf("main: done w/ defines\n") ;
#endif


/* all of the rest of the configuration file stuff */

	    if ((cf.workdir != nullptr) && (pip->workdname == nullptr)) {

	        if (((sl = varsub_subbuf(&vsh_d,&vsh_e,cf.workdir,
	            -1,buf,BUFLEN)) > 0) &&
	            ((l2 = expander(pip,buf,sl,buf2,BUFLEN)) > 0)) {

	            pip->workdname = mallocstrw(buf2,l2) ;

	        }

	    } /* end if (config file working directory) */


	    if ((cf.pidfname != nullptr) && (pidfname[0] == '\0')) {

#if	CF_DEBUG
	        if (pip->debuglevel > 1)
	            debugprintf("main: CF pidfile=%s\n",cf.pidfname) ;
#endif

	        if ((cf.pidfname[0] != '\0') && (cf.pidfname[0] != '-')) {

	            if (((sl = varsub_subbuf(&vsh_d,&vsh_e,cf.pidfname,
	                -1,buf,BUFLEN)) > 0) &&
	                ((l2 = expander(pip,buf,sl, buf2,BUFLEN)) > 0)) {

	                strwcpy(pidfname,buf2,l2) ;

	            }

	        }

#if	CF_DEBUG
	        if (pip->debuglevel > 1)
	            debugprintf("main: pidfname=%s\n",pidfname) ;
#endif

	    } /* end if (configuration file PIDFNAME) */


	    if ((cf.lockfname != nullptr) && (lockfname[0] == '\0')) {

#if	CF_DEBUG
	        if (pip->debuglevel > 1)
	            debugprintf("main: CF lockfile=%s\n",cf.lockfname) ;
#endif

	        if ((cf.lockfname[0] != '\0') && (cf.lockfname[0] != '-')) {

	            if (((sl = varsub_subbuf(&vsh_d,&vsh_e,cf.lockfname,
	                -1,buf,BUFLEN)) > 0) &&
	                ((l2 = expander(pip,buf,sl, buf2,BUFLEN)) > 0)) {

	                strwcpy(lockfname,buf2,l2) ;

	            }

	        }

#if	CF_DEBUG
	        if (pip->debuglevel > 1)
	            debugprintf("main: lockfile=%s\n",lockfname) ;
#endif

	    } /* end if (configuration file LOCKFNAME) */


#if	CF_DEBUG
	    if (pip->debuglevel > 1) {
	        debugprintf("main: so far logfname=%s\n",logfname) ;
	        debugprintf("main: about to get config log filename\n") ;
	    }
#endif

	    if ((cf.logfname != nullptr) && (logfile_type < 0)) {

#if	CF_DEBUG
	        if (pip->debuglevel > 1)
	            debugprintf("main: CF logfilename=%s\n",cf.logfname) ;
#endif

	        if (((sl = varsub_subbuf(&vsh_d,&vsh_e,cf.logfname,
	            -1,buf,BUFLEN)) > 0) &&
	            ((l2 = expander(pip,buf,sl,buf2,BUFLEN)) > 0)) {

	            strwcpy(logfname,buf2,l2) ;

	            if (strchr(logfname,'/') != nullptr)
	                logfile_type = 1 ;

	        }

#if	CF_DEBUG
	        if (pip->debuglevel > 1)
	            debugprintf("main: processed CF logfilename=%s\n",
		logfname) ;
#endif

	    } /* end if (configuration file log filename) */


/* timeout */

	    if ((cf.timeout != nullptr) && (to < 0)) {

#if	CF_DEBUG
	        if (pip->debuglevel > 1)
	            debugprintf("main: CF timeout=%s\n",cf.timeout) ;
#endif

	        if (((sl = varsub_subbuf(&vsh_d,&vsh_e,cf.timeout,
	            -1,buf,BUFLEN)) > 0) &&
	            ((l2 = expander(pip,buf,sl,buf2,BUFLEN)) > 0)) {

	            if (cfdeci(buf2,l2,&val) >= 0)
	                to = val ;

	        }

#if	CF_DEBUG
	        if (pip->debuglevel > 1)
	            debugprintf("main: processed CF timeout=%s\n",
	                buf2) ;
#endif

	    } /* end if (configuration file timeout) */


/* what about an 'environ' file ? */

	    if (cf.envfname != nullptr) {

#if	CF_DEBUG
	        if (pip->debuglevel > 1)
	            debugprintf("main: 1 envfile=%s\n",cf.envfname) ;
#endif

	        procfileenv(pip->pr,cf.envfname,&exports) ;

	    } else {
	        procfile(pip,procfileenv,pip->pr,&svars,
	            ENVFNAME,&exports) ;
	    }

	    f_procfileenv = TRUE ;


/* "do" any 'paths' file before we process the environment variables */

	    if (cf.pathfname != nullptr) {
	        procfilepaths(pip->pr,cf.pathfname,&exports) ;
	    } else {
	        procfile(pip,procfilepaths,pip->pr,&svars,
	            PATHSFNAME,&exports) ;
	    }

	    f_procfilepaths = TRUE ;


#if	CF_DEBUG
	    if (pip->debuglevel > 1)
	        debugprintf("main: processing CF loglen\n") ;
#endif

	    if ((cf.loglen >= 0) && (loglen < 0))
	        loglen = cf.loglen ;

/* OK, now loop through any options that we have */

	    if (cf.options != nullptr) {

#if	CF_DEBUG
	        if (pip->debuglevel > 1)
	            debugprintf("main: CF options\n") ;
#endif

	        if (((sl = varsub_subbuf(&vsh_d,&vsh_e,cf.options,
	            -1,buf,BUFLEN)) > 0) &&
	            ((sl2 = expander(pip,buf,sl,buf2,BUFLEN)) > 0)) {

	            field_start(&fsb,buf2,sl2) ;

	            while (field_get(&fsb,oterms) >= 0) {

	                if (fsb.flen == 0) continue ;

#if	CF_DEBUG
	                if (pip->debuglevel > 1)
	                    debugprintf("main: CF option >%W<\n",
	                        fsb.fp,fsb.flen) ;
#endif

	                if ((sl = sfkey(fsb.fp,fsb.flen,&cp)) < 0) {

	                    cp = fsb.fp ;
	                    sl = fsb.flen ;
	                }

#if	CF_DEBUG
	                if (pip->debuglevel > 1)
	                    debugprintf("main: CF option key >%W<\n",cp,sl) ;
#endif

	                if ((i = matstr3(configopts,cp,sl)) >= 0) {

	                    char	*cp2 ;


	                    cp2 = strnchr(cp + sl,(fsb.flen - sl),'=') ;

	                    sl2 = 0 ;
	                    if (cp2 != nullptr) {

	                        cp2 += 1 ;
	                        sl2 = fsb.flen - (cp2 - fsb.fp) ;

	                    }

#if	CF_DEBUG
	                    if (pip->debuglevel > 1) {
	                        debugprintf("main: CF option match i=%d\n",i) ;
	                        debugprintf("main: cp2=%W\n",cp2,sl2) ;
	                    }
#endif

	                    switch (i) {

	                    case configopt_marktime:
	                        if ((pip->markint <= 0) && (cp2 != nullptr) &&
	                            (cfdecti(cp2,sl2,&val) >= 0)) {

#if	CF_DEBUG
	                            if (pip->debuglevel > 1)
	                                debugprintf("main: CF opt markint=%d\n",
	                                    val) ;
#endif

	                            pip->markint = val ;

	                        } /* end if */

	                        break ;

	                    case configopt_sumfile:
	                        if (pip->sumfname == nullptr) {

#if	CF_DEBUG
	                            if (pip->debuglevel > 1)
	                                debugprintf("main: CF opt sumfname=%s\n",
	                                    cp2) ;
#endif

	                            if (cp2 != nullptr)
	                                pip->sumfname = mallocstrw(cp2,sl2) ;

	                        }

	                        break ;

	                    case configopt_minpingint:
	                        if ((pip->minpingint < 0) && (cp2 != nullptr) &&
	                            (cfdecti(cp2,sl2,&val) >= 0)) {

#if	CF_DEBUG
	                            if (pip->debuglevel > 1)
	                                debugprintf("main: CF opt minpingint=%d\n",
	                                    val) ;
#endif

	                            pip->minpingint = val ;

	                        } /* end if */

	                        break ;

	                    case configopt_minupdate:
	                        if ((pip->minupdate < 0) && (cp2 != nullptr) &&
	                            (cfdecti(cp2,sl2,&val) >= 0)) {

#if	CF_DEBUG
	                            if (pip->debuglevel > 1)
	                                debugprintf("main: CF opt minupdate=%d\n",
	                                    val) ;
#endif

	                            pip->minupdate = val ;

	                        } /* end if */

	                        break ;

	                    } /* end switch */

	                } /* end if */

	            } /* end while */

	            field_finish(&fsb) ;
	        } /* end if */

	    } /* end if (options) */


/* loop through the UNSETs in the configuration file */

	    for (i = 0 ; vecstr_get(&cf.unsets,i,&cp) >= 0 ; i += 1) {

	        rs = vecstr_finder(&exports,cp,vstrkeycmp,nullptr) ;

	        if (rs >= 0)
	            vecstr_del(&exports,rs) ;

	    } /* end for */


/* loop through the EXPORTed variables in the configuration file */

	    for (i = 0 ; vecstr_get(&cf.exports,i,&cp) >= 0 ; i += 1) {

#if	CF_DEBUG
	        if (pip->debuglevel > 1)
	            debugprintf("main: 1 about to sub> %s\n",cp) ;
#endif

	        if (((sl = varsub_subbuf(&vsh_d,&vsh_e,cp,
	            -1,buf,BUFLEN)) > 0) &&
	            ((l2 = expander(pip,buf,sl,buf2,BUFLEN)) > 0)) {

#if	CF_DEBUG
	            if (pip->debuglevel > 1)
	                debugprintf("main: 1 about to merge> %W\n",buf,l2) ;
#endif

	            varsub_merge(nullptr,&exports,buf2,l2) ;

#if	CF_DEBUG
	            if (pip->debuglevel > 1)
	                debugprintf("main: 1 done merging\n") ;
#endif

#if	CF_DEBUG && 0
	            if (pip->debuglevel > 1) {
	                debugprintf("varsub_merge: VSA_D so far \n") ;
	                varsub_dump(&vsh_d) ;
	                debugprintf("varsub_merge: VSA_E so far \n") ;
	                varsub_dump(&vsh_e) ;
	            } /* end if (debuglevel) */
#endif /* CF_DEBUG */

	        } /* end if (merging) */

#if	CF_DEBUG
	        if (pip->debuglevel > 1)
	            debugprintf("main: done subbing & merging\n") ;
#endif

	    } /* end for (exports) */

#if	CF_DEBUG
	    if (pip->debuglevel > 1)
	        debugprintf("main: done w/ exports\n") ;
#endif


#if	CF_DEBUG
	    if (pip->debuglevel > 1)
	        debugprintf("main: processing CF freeing data structures\n") ;
#endif

	    varsub_finish(&vsh_d) ;

	    varsub_finish(&vsh_e) ;

#if	CF_DEBUG
	    if (pip->debuglevel > 1)
	        debugprintf("main: processing CF free\n") ;
#endif

	    configfile_finish(&cf) ;

#if	CF_DEBUG
	    if (pip->debuglevel > 1) {
	        debugprintf("main: dumping defines\n") ;
	        for (i = 0 ; vecstr_get(&defines,i,&cp) >= 0 ; i += 1)
	            debugprintf("main: define> %s\n",cp) ;
	        debugprintf("main: dumping exports\n") ;
	        for (i = 0 ; vecstr_get(&exports,i,&cp) >= 0 ; i += 1)
	            debugprintf("main: export> %s\n",cp) ;
	    } /* end if (CF_DEBUG) */
#endif /* CF_DEBUG */


	} /* end if (accessed the configuration file) */

#if	CF_DEBUG
	if (pip->debuglevel > 1)
	    debugprintf("main: finished with any configfile stuff\n") ;
#endif


	if (pip->pr == nullptr)
	    pip->pr = programroot ;

	if (pip->debuglevel > 0) {
	    bprintf(efp,"%s: pr=%s\n",
	        pip->progname,pip->pr) ;
	}

/* help */

	if (f_help) {

	    if ((helpfname == nullptr) || (helpfname[0] == '\0')) {
	        strcpy(helpfname,HELPFNAME) ;
	    }
	    printhelp(nullptr,pip->pr,SEARCHNAME,helpfname) ;
	    vecstr_finish(&svars) ;
	    goto rethelp ;

	} /* end if (help) */


/* put the final value of the program root into the schedule search variables */

	oo.svars = TRUE ;
	vecstr_envset(&svars,"p",pip->pr,-1) ;

/* load up some environment and execution paths if we have not already */

	if (! f_procfileenv)
	    procfile(pip,procfileenv,pip->pr,&svars,
	        ENVFNAME,&exports) ;


	if (! f_procfilepaths)
	    procfile(pip,procfilepaths,pip->pr,&svars,
	        PATHSFNAME,&exports) ;


/* check program parameters */

#if	CF_DEBUG
	if (pip->debuglevel > 1)
	    debugprintf("main: checking program parameters\n") ;
#endif

	if (pip->workdname == nullptr) {
	    pip->workdname = WORKDNAME ;
	} else if (pip->workdname[0] == '\0') {
	    pip->workdname = "." ;
	}

	if ((pip->tmpdname == nullptr) || (pip->tmpdname[0] == '\0')) {

	    if ((pip->tmpdname = getenv("TMPDIR")) == nullptr)
	        pip->tmpdname = TMPDNAME ;

	} /* end if (tmpdir) */

	if (lockfname[0] == '\0') {
	    strcpy(lockfname,LOCKFNAME) ;
	}

	if ((sl = getfname(pip->pr,lockfname,1,tmpfname)) > 0)
	    strwcpy(lockfname,tmpfname,sl) ;

	pip->lockfname = lockfname ;


/* can we access the working directory ? */

#if	CF_DEBUG
	if (pip->debuglevel > 1)
	    debugprintf("main: access working directory \"%s\"\n",
		pip->workdname) ;
#endif

#ifdef	COMMENT
	if ((perm(pip->workdname,-1,-1,nullptr,X_OK) < 0) || 
	    (perm(pip->workdname,-1,-1,nullptr,R_OK) < 0))
	    goto badworking ;
#endif /* COMMENT */


/* do we have an activity log file ? */

#if	CF_DEBUG
	if (pip->debuglevel > 1)
	    debugprintf("main: 0 logfname=%s\n",logfname) ;
#endif

	i = storebuf_strw(buf,BUFLEN,0,pip->nodename,-1) ;

	(void) storebuf_dec(buf,BUFLEN,i,pip->pid) ;

	pip->logid = mallocstr(buf) ;

	rs = SR_BAD ;
	if (logfname[0] == '\0') {
	    logfile_type = 1 ;
	    strcpy(logfname,LOGFNAME) ;
	}

	sl = getfname(pip->pr,logfname,logfile_type,tmpfname) ;

	if (sl > 0)
	    strwcpy(logfname,tmpfname,sl) ;

	rs = logfile_open(&pip->lh,logfname,0,0666,pip->logid) ;

#if	CF_DEBUG
	if (pip->debuglevel > 1)
	    debugprintf("main: 1 logfname=%s rs=%d\n",logfname,rs) ;
#endif

	if (rs >= 0) {

	    struct utsname	un ;


#if	CF_DEBUG
	    if (pip->debuglevel > 1)
	        debugprintf("main: we opened a logfile\n") ;
#endif

	    pip->fl.log = TRUE ;
	    if (pip->debuglevel > 0)
	        bprintf(efp,"%s: logfile=%s\n",pip->progname,logfname) ;

#if	CF_PILOGFNAME
	    pip->logfname = logfname ;
#endif

/* we opened it, maintenance this log file if we have to */

	    if (loglen < 0)
	        loglen = LOGSIZE ;

	    logfile_checksize(&pip->lh,loglen) ;

#if	CF_DEBUG
	    if (pip->debuglevel > 1)
	        debugprintf("main: we checked its length\n") ;
#endif

	    u_time(&daytime) ;

#if	CF_DEBUG
	    if (pip->debuglevel > 1)
	        debugprintf("main: making log entry\n") ;
#endif

	    logfile_printf(&pip->lh,"%s %s\n",
	        timestr_log(daytime,timebuf),
	        BANNER) ;

	    logfile_printf(&pip->lh,"%-14s %s/%s\n",
	        pip->progname,
	        VERSION,(u.f.sysv_ct) ? "SYSV" : "BSD") ;


	    u_uname(&un) ;

	    logfile_printf(&pip->lh,"ostype=%s os=%s(%s) domain=%s\n",
	        (u.f.sysv_rt ? "SYSV" : "BSD"),
	        un.sysname,un.release,
	        pip->domainname) ;


	    buf[0] = '\0' ;
	    if ((u.name != nullptr) && (u.name[0] != '\0')) {
	        sprintf(buf,"(%s)",u.name) ;

#if	CF_DEBUG
	        if (pip->debuglevel > 1)
	            debugprintf("main: 0 buf=\"%s\"\n",buf) ;
#endif

	    } else if ((u.gecosname != nullptr) && (u.gecosname[0] != '\0')) {
	        sprintf(buf,"(%s)",u.gecosname) ;

#if	CF_DEBUG
	        if (pip->debuglevel > 1)
	            debugprintf("main: 1 buf=\"%s\"\n",buf) ;
#endif

	    } else if ((u.fullname != nullptr) && (u.fullname[0] != '\0')) {
	        sprintf(buf,"(%s)",u.fullname) ;

#if	CF_DEBUG
	        if (pip->debuglevel > 1)
	            debugprintf("main: 2 buf=\"%s\"\n",buf) ;
#endif

	    }

	    logfile_printf(&pip->lh,"%s!%s %s\n",
	        u.nodename,u.username,buf) ;

	} /* end if (we have a log file or not) */


/* do we have a good default timeout value */

	if (to < 0)
	    to = RUNTIMEOUT ;



/* handle UID/GID stuff */

#if	CF_DEBUG
	if (pip->debuglevel > 1)
	    debugprintf("main: continuing with some PID stuff\n") ;
#endif

	pip->uid = u.uid ;
	pip->euid = geteuid() ;

	pip->gid = u.gid ;
	pip->egid = getegid() ;

	rs = getgr_gid(pip->gid,&ge,buf,BUFLEN) ;

	if (rs < 0) {

	    cp = buf ;
	    bufprintf(buf,BUFLEN,"GID%d",(int) pip->gid) ;

	} else
	    cp = ge.gr_name ;

	pip->groupname = mallocstr(cp) ;


/* before we go too far, are we the only one on this PID mutex ? */

	if ((pip->pidfname != nullptr) && (pip->pidfname[0] != '\0')) {

#if	CF_DEBUG
	    if (pip->debuglevel > 1)
	        debugprintf("main: we have a PIDFNAME=%s\n",pip->pidfname) ;
#endif

	    if ((rs = bopen(&pidfile,pip->pidfname,"rwc",0664)) < 0)
	        goto badpidfile1 ;

/* capture the lock (if we can) */

	    if ((rs = bcontrol(&pidfile,BC_LOCK,2)) < 0) {

	        bclose(&pidfile) ;

	        goto badpidfile2 ;
	    }

	    bcontrol(&pidfile,BC_TRUNCATE,0L) ;

	    bseek(&pidfile,0L,SEEK_SET) ;

	    bprintf(&pidfile,"%d\n",pip->pid) ;

	    bprintf(&pidfile,"%s %s\n",
	        BANNER,timestr_log(daytime,timebuf)) ;

	    if (userbuf[0] != '\0')
	        bprintf(&pidfile,"host=%s.%s user=%s pid=%d\n",
	            u.nodename,u.domainname,
	            u.username,
	            pip->pid) ;

	    else
	        bprintf(&pidfile,"host=%s.%s pid=%d\n",
	            u.nodename,u.domainname,
	            pip->pid) ;

	    bflush(&pidfile) ;

/* we leave the file open as our mutex lock ! */

	    logfile_printf(&pip->lh,"pidfile=%s\n",pip->pidfname) ;

	    logfile_printf(&pip->lh,"PID mutex captured\n") ;

	    bcontrol(&pidfile,BC_STAT,&sb) ;

	    logfile_printf(&pip->lh,"pidfile device=%ld inode=%ld\n",
	        sb.st_dev,sb.st_ino) ;

	    pip->pidfp = &pidfile ;

	} /* end if (we have a mutex PID file) */


/* clean up some stuff we will no longer need */

	vecstr_finish(&svars) ;


/* some final initialization */

#if	CF_DEBUG
	if (pip->debuglevel > 1)
	    debugprintf("main: minpingint=%d\n",pip->minpingint) ;
#endif

	if (pip->mininputint < -1)
		pip->mininputint = MININPUTINT ;


/* OK, we do it */

	varsub_start(&vsh_d,VARSUB_MBADNOKEY) ;

	varsub_start(&vsh_e,0) ;


	varsub_addvec(&vsh_d,&defines) ;

	varsub_addvec(&vsh_e,&exports) ;


	ex = EX_OK ;


/* open a summary file (if we have one) */

	if ((pip->sumfname != nullptr) && (pip->sumfname[0] != '\0')) {

	    rs = bopen(&sumfile,pip->sumfname,"wca",0666) ;

#if	CF_DEBUG
	    if (pip->debuglevel > 1)
	        debugprintf("main: sumfname=%s rs=%d\n",
	            pip->sumfname,rs) ;
#endif

	    if (rs >= 0)
	        pip->sumfp = &sumfile ;

	    if ((rs < 0) && (! pip->fl.quiet)) {

	        bprintf(efp,
	            "%s: could not open the summary file (rs=%d)\n",
	            pip->progname,rs) ;

	        logfile_printf(&pip->lh,
	            "could not open the summary file (rs=%d)\n",
	            rs) ;

	    } /* end if (announcing failure) */

	} /* end if (opening the summary file) */


/* OK, do out thing */

		cp = (pip->fl.dgram) ? "mode=input (dgram)" : "mode=input" ;
		logfile_printf(&pip->lh,cp) ;

		rs = process_input(pip,pip->minupdate) ;

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("main: process_input() rs=%d\n",rs) ;
#endif


/* close the summary file (if opened) */

	if (pip->sumfp != nullptr)
	    bclose(pip->sumfp) ;

ret5:
	varsub_finish(&vsh_d) ;

	varsub_finish(&vsh_e) ;

#if	CF_DEBUG
	if (pip->debuglevel > 1)
	    debugprintf("main: total hosts=%d processed=%d up=%d\n",
	        c_hosts,c_processed,c_up) ;
#endif

ret4:
	bclose(&pidfile) ;

ret3:

/* close some more earlier stuff */
ret2:
	if (pip->fl.log)
	    logfile_close(&pip->lh) ;

	if (pip->fl.slog)
	    bclose(pip->lfp) ;

rethelp:
ret1:
	if (f_freeconfigfname && (configfname != nullptr)) {
	    free(configfname) ;
	}

	vecstr_finish(&pip->localnames) ;

#ifdef	OPTIONAL
	vecstr_finish(&defines) ;

	vecstr_finish(&exports) ;
#endif /* COMMENT */


	if (pip->debuglevel > 0)
	    bprintf(efp,"%s: program exiting ex=%d\n",
	        pip->progname,ex) ;

ret0:
retearly:
	bclose(efp) ;
	return ex ;

/* usage */
usage:
	bprintf(efp,
	    "%s: USAGE> %s [-C conf] [-?v]\n",
	    pip->progname,pip->progname) ;

	bprintf(efp,"%s: \t[-ROOT programroot] [-P pidmutex]\n",
		pip->progname) ;

	goto retearly ;

badargnum:
	bprintf(efp,"%s: not enough arguments specified\n",
	    pip->progname) ;

	goto badarg ;

badargextra:
	bprintf(efp,"%s: no value associated with this option key\n",
	    pip->progname) ;

	goto badarg ;

badargunk:
	bprintf(efp,"%s: unknown argument(s) given\n",
	    pip->progname) ;

	goto badarg ;

badargval:
	bprintf(efp,"%s: bad argument value was specified\n",
	    pip->progname) ;

	goto badarg ;

badarg:
	ex = EX_USAGE ;
	goto ret0 ;

#ifdef	COMMENT
badworking:
	ex = EX_USAGE ;
	bprintf(efp,"%s: could not access the working directory \"%s\"\n",
	    pip->progname,pip->workdname) ;

	goto ret0 ;
#endif /* COMMENT */

badlistinit:
	ex = EX_DATAERR ;
	bprintf(efp,"%s: could not initialize list structures (rs %d)\n",
	    pip->progname,rs) ;

	goto badmore0 ;

badconfig:
	ex = EX_DATAERR ;
	bprintf(efp,
	    "%s: configfile=%s\n",
	    pip->progname,configfname) ;

	bprintf(efp,
	    "%s: error (rs %d) in configuration file starting at line %d\n",
	    pip->progname,rs,cf.badline) ;

	goto badmore1 ;


badpidfile1:
	ex = EX_DATAERR ;
	logfile_printf(&pip->lh,
	    "could not open the PID mutex file (rs=%d)\n",
	    rs) ;

	logfile_printf(&pip->lh,"pidfile=%s\n", pip->pidfname) ;

	goto badmore3 ;

badpidfile2:
	ex = EX_DATAERR ;
	logfile_printf(&pip->lh,
	    "there was a daemon already on the mutex file, PID=%d\n",
	    pip->pid) ;

	goto badmore3 ;

/* extra cleanup */
badmore0:
	vecstr_finish(&svars) ;

	goto ret0 ;

badmore1:
	vecstr_finish(&svars) ;

	goto ret1 ;

badmore2:
	vecstr_finish(&svars) ;

	goto ret2 ;

badmore3:
	vecstr_finish(&svars) ;

	goto ret3 ;

}
/* end subroutine (main) */


/* local subroutines */


static int procfile(pip,func,pr,svp,fname,elp)
PROGINFO	*pip ;
int		(*func)(char *,char *,vecstr *) ;
char		pr[] ;
vecstr		*svp ;
char		fname[] ;
vecstr		*elp ;
{
	int	sl ;

	char	tmpfname[MAXPATHLEN + 1] ;


#if	CF_DEBUG
	if (pip->debuglevel > 1)
	    debugprintf("procfile: 1 fname=%s\n",fname) ;
#endif

	if ((sl = permsched(sched2,svp,
	    tmpfname,MAXPATHLEN, fname,R_OK)) < 0) {

	    if ((sl = permsched(sched3,svp,
	        tmpfname,MAXPATHLEN, fname,R_OK)) > 0)
	        fname = tmpfname ;

	} else if (sl > 0)
	    fname = tmpfname ;

#if	CF_DEBUG
	if (pip->debuglevel > 1)
	    debugprintf("procfile: 2 fname=%s\n",fname) ;
#endif

	if (sl >= 0)
	    return (*func)(pr,fname,elp) ;

	return SR_NOEXIST ;
}
/* end subroutine (procfile) */

static int getlocalnames(PROGINFO *pip) noex {
	int		rs ;
	int		rs1 ;
	int		n ;
	char		hostnamebuf[MAXHOSTNAMELEN + 1], *hnp ;
	char		*hebuf ;
	char		*np ;
	bufprintf(hostnamebuf,MAXHOSTNAMELEN,"%s.%s",
	    pip->nodename,pip->domainname) ;

	if ((rs = malloc_ho(&hebuf)) >= 0) {
	    ucentho	he ;
	    vecstr	*lnp = &pip->localnames ;
	    cint	helen = rs ;
	    for (int i = 0 ; i < 2 ; i += 1) {
	        hnp = (i == 0) ? pip->nodename : hostnamebuf ;
	        if ((rs = gethename(&he,hebuf,helen,hnp)) >= 0) {
	            HOSTENT_CUR	cur ;
	            if ((rs = hostent_curbegin(&he,&cur)) >= 0) {
		        cchar	*hp ;
	                while ((rs = hostent_curenumname(&he,&cur,&hp) > 0) {
		            cint	rsn = SR_NOTFOUND ;
	                    if ((rs = vecstr_find(lnp,hp)) == rsn) {
	                        vecstr_add(lnp,hp,-1) ;
		            }
	                } /* end while */
	                hostent_curend(&he,&cur) ;
		    } /* end if (hostent-cur) */
	        } /* end if (gethename) */
	    } /* end for */
	    rs1 = uc_free(hebuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (memory-allocation) */

	if (rs >= 0) {
	    n = vecstr_count(&pip->localnames) ;
	}

	return (rs >= 0) ? n : rs ;
}
/* end subroutine (getlocalnames) */



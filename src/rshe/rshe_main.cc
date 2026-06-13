/* rshe_main SUPPORT (RSHE) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* program to run a program remotely inheriting the local environment */
/* version %I% last-modified %G% */


/* revision history:

	= 1995-08-22, David A­D­ Morano
	This program was originally written.

	= 1995-10-12, David A­D­ Morano
	This operation of the 'x' option was modified to
	reflect modern usage of the same option in similar programs.

*/

/* Copyright © 1995 David A­D­ Morano.  All rights reserved. */

/**************************************************************************

  	Name:
	main

	Description:
	This is a remove-execution program.

	Synopsis:
	$ rshe [-n] [-l user] [-d] [-e=file] remotehost command [args]

	Arguments:
		-n		do not read input
		-l user		become other 'user' on other side
		-d		do not change directory on the other side
		-x		only export those variables in RXPORT
		-x=[list]	only export those variables in "list"
		-D[=level]	set debug level
		-V		print version of program and exit
		-?		print brief help

	The program normally sends all of the local environment
	over to the remote machine.  If the user provides
	the '-e' option, then only that environment along with
	any as a result of specifying the '-x' option are
	established.  The default environment variables :

		HOME
		PATH
		USER

	are always established (not necessarily by direct actions
	of this program).  The program will try to change the remote
	current working directory to that of the local machine if
	the '-d' option is specified.  The environment variable
	'DISPLAY' is possibly adjusted for name space shifts between
	the local and remote machines.

**************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<sys/wait.h>
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>
#include	<rpc/rpc.h>
#include	<rpcsvc/rstat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<netdb.h>
#include	<pwd.h>
#include	<grp.h>
#include	<ctime>
#include	<csignal>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<cstring>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<getfiledirs.h>
#include	<getusername.h>
#include	<getchostname.h>
#include	<bfile.h>
#include	<baops.h>
#include	<vecstr.h>
#include	<userinfo.h>
#include	<logfile.h>
#include	<bufstr.h>
#include	<mallocstuff.h>
#include	<strx.h>		/* |strnncmp(3uc)| */
#include	<prognamevar.hh>
#include	<exitcodes.h>
#include	<localmisc.h>

#include	"config.h"
#include	"defs.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#define		MAXARGINDEX	100
#define		MAXARGGROUPS	(MAXARGINDEX/8 + 1)

#define		BUFLEN		(MAXPATHLEN + (LINEBUFLEN * 2))
#define		CMDBUFLEN	(MAXPATHLEN * 2)
#define		DISBUFLEN	300


/* external subroutines */


/* external variables */


/* local structures */

struct jobinfo {
	bfile	*jfp ;
	char	*nodename ;
	char	*domainname ;
	int	f_remotedomain ;
} ;


/* forward references */

local int	quotevalue(proginfo *,cchar *,bufstr *) noex ;
local int	send_rxenv(proginfo *,jobinfo *,char *) noex ;
local int	proc_rxenv(proginfo *,jobinfo *,char *,int) noex ;
local int	send_procenv(proginfo *,jobinfo *) noex ;
local int	proc_env(proginfo *,jobinfo *,cchar *,int,cchar *) noex ;

static void	bdump() ;


/* local variables */

enum argopts {
	argopt_tmpdir,
	argopt_version,
	argopt_verbose,
	argopt_root,
	argopt_logfile,
	argopt_overlast
} ; /* end enum */

constexpr cpcchar	argopts[] = {
	"TMPDIR",
	"VERSION",
	"VERBOSE",
	"ROOT",
	"LOGFILE",
	nullptr
} ; /* end array (argopts) */


/* exported variables */


/* exported subroutines */

int main(int argc,mainv argv,mainv envv) {
    	prognamevar	progname(argc,argv,envv) ;
	proginfo	g{}, *pip = &g ;
	jobinfo		ji ;
	userinfo	u ;
	bfile		errfile, *efp = &errfile ;
	bfile		jobfile, *jfp = &jobfile ;
	bfile		file0, file1, file2, *fpa[3] ;
	vecstr		argvec ;
	logfile		lh ;
	pid_t		cpid ;
	time_t		daytime ;
	sigset_t	signalmask ;

	int	argr, argl, aol, akl, avl ;
	int	rai = -1, npa, kwi, i ;
	int	ex = EX_INFO ;
	int	f_optminus, f_optplus, f_optequal ;
	int	f_verbose = false ;
	int	f_userinfo = false ;
	int	f_log = true ;
	int	f_extra = false ;
	int	f_version = false ;
	int	f_usage = false ;
	int	f_exitargs = false ;
	int	f_sysvct, f_sysvrt ;
	int	f_usessh, f_ssh ;
	int	f_cd = false, f_x = false, f_n = false ;
	int	len, l, rs ;
	int	argnum ;
	int	n ;
	int	childstat ;

	char	*argp, *aop, *akp, *avp ;
	char	*logfname = nullptr ;
	char	argpresent[MAXARGGROUPS] ;
	char	*tmpdir = "/tmp" ;
	char	buf[BUFLEN + 1] ;
	char	cmd[CMDBUFLEN + 1] ;
	char	jobfname[MAXPATHLEN + 1] ;
	char	userbuf[USERINFO_LEN + 1] ;
	char	nodename[NODENAMELEN + 1], domainname[MAXHOSTNAMELEN + 1] ;
	char	hostname[MAXHOSTNAMELEN + 1] ;
	char	rnodename[NODENAMELEN + 1], rdomainname[MAXHOSTNAMELEN + 1] ;
	char	rhost[MAXHOSTNAMELEN + 1] ;
	char	*cwd = nullptr ;
	char	*cmd_rcp, *cmd_rsh ;
	char	*nnp, *dnp ;
	char	*remotehost = nullptr ;
	char	*remotecommand = nullptr ;
	char	*remoteuser = "" ;
	char	*envfile = "" ;
	char	*opt_n = "" ;
	char	*rxport = nullptr ;
	char	*cp ;

	memclear(pip) ;

#if	CF_DEBUGS || CF_DEBUG
	if ((cp = getourenv(envv,VARDEBUGFNAME)) != nullptr) {
	    rs = debugopen(cp) ;
	    debugprintf("main: starting DFD=%d\n",rs) ;
	}
#endif /* CF_DEBUGS */

	pip->envv = (cchar **) envv ;
	pip->progname = progname ;
	if (bopen(efp,BFILE_STDERR,"dwca",0666) >= 0) {
		pip->efp = &errfile ;
		bcontrol(pip->efp,BC_LINEBUF,0) ;
	}

	f_usessh = false ;
	f_verbose = false ;

	pip->verboselevel = 1 ;
	g.efp = efp ;

/* some other global initialization */

	pip->pr = getenv(VARPROGRAMROOT1) ;

	if (pip->pr == nullptr)
	    pip->pr = getenv(VARPROGRAMROOT2) ;

	if (pip->pr == nullptr)
	    pip->pr = PROGRAMROOT ;

/* other initialization */

	jobfname[0] = '\0' ;

/* process program arguments */

	for (i = 0 ; i < MAXARGGROUPS ; i += 1) argpresent[i] = 0 ;

	npa = 0 ;			/* number of positional so far */
	rai = -1 ;			/* remote argument index */
	i = 0 ;
	argr = argc - 1 ;
	while ((! f_exitargs) && (argr > 0)) {

	    argp = argv[++i] ;
	    argr -= 1 ;
	    argl = lenstr(argp) ;

	    f_optminus = (*argp == '-') ;
	    f_optplus = (*argp == '+') ;
	    if ((argl > 0) && (f_optminus || f_optplus)) {

	        if (argl > 1) {

	            if (isdigit(argp[1])) {

	                argnum = 0 ;
	                if (((argl - 1) > 0) && 
	                    (cfdeci(argp + 1,argl - 1,&argnum) < 0))
	                    goto badargvalue ;

	            } else {

#if	CF_DEBUGS
	                debugprintf("main: got an option\n") ;
#endif

	                aop = argp + 1 ;
	                aol = argl - 1 ;
	                akp = aop ;
	                f_optequal = false ;
	                if ((avp = strchr(aop,'=')) != nullptr) {

#if	CF_DEBUGS
	                    debugprintf("main: got an option key w/ a value\n") ;
#endif

	                    akl = avp - aop ;
	                    avp += 1 ;
	                    avl = aop + aol - avp ;
	                    f_optequal = true ;

#if	CF_DEBUGS
	                    debugprintf("main: aol=%d avp=\"%s\" avl=%d\n",
	                        aol,avp,avl) ;

	                    debugprintf("main: akl=%d akp=\"%s\"\n",
	                        akl,akp) ;
#endif

	                } else {

	                    akl = aol ;
	                    avl = 0 ;

	                }

/* do we have a keyword match or should we assume only key letters? */

	                if ((kwi = matostr(argopts,2,akp,akl)) >= 0) {

	                    switch (kwi) {

/* version */
	                    case argopt_version:
	                        f_version = true ;
	                        if (f_optequal) 
					goto badargextra ;

	                        break ;

/* verbose mode */
	                    case argopt_verbose:
	                        f_verbose = true ;
	                        break ;

/* program root */
	                    case argopt_root:
	                        if (f_optequal) {

	                            f_optequal = false ;
	                            if (avl) 
					pip->pr = avp ;

	                        } else {

	                            if (argr <= 0) 
					goto badargnum ;

	                            argp = argv[++i] ;
	                            argr -= 1 ;
	                            argl = lenstr(argp) ;

	                            if (argl) 
					pip->pr = argp ;

	                        }

	                        break ;

/* LOGFILE */
	                    case argopt_logfile:
	                        f_log = true ;
	                        if (f_optequal) {

	                            f_optequal = false ;
	                            if (avl) 
					logfname = avp ;

	                        } else {

	                            if (argr <= 0) 
					goto badargnum ;

	                            argp = argv[++i] ;
	                            argr -= 1 ;
	                            argl = lenstr(argp) ;

	                            if (argl) 
					logfname = argp ;

	                        }

	                        break ;

/* handle all keyword defaults */
	                    default:
				ex = EX_USAGE ;
	                        f_exitargs = true ;
	                        f_usage = true ;
	                        bprintf(efp,
	                            "%s: unknown argument keyword \"%s\"\n",
	                            pip->progname,akp) ;

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

#if	CF_DEBUGS
	                                debugprintf(
	                                    "main: debug flag, avp=\"%W\"\n",
	                                    avp,avl) ;
#endif

	                                f_optequal = false ;
	                                if (avl) {

	                                    rs = cfdeci(avp,avl,
						&pip->debuglevel) ;

					    if (rs < 0)
	                                    goto badargvalue ;

					}
	                            }

	                            break ;

/* version */
	                        case 'V':
	                            f_version = true ;
	                            break ;

/* change directory on remote machine */
	                        case 'd':
	                            f_cd = true ;
	                            break ;

/* environment file */
	                        case 'e':
	                            if (f_optequal) {

	                                f_optequal = false ;
	                                if (avl)
	                                    envfile = avp ;

	                            } else {

	                                if (argr <= 0) 
						goto badargnum ;

	                                argp = argv[++i] ;
	                                argr -= 1 ;
	                                argl = lenstr(argp) ;

	                                if (argl)
	                                    envfile = argp ;

	                            }

	                            break ;

/* other user */
	                        case 'l':
	                            if (f_optequal) {

	                                f_optequal = false ;
	                                if (avl)
	                                    remoteuser = avp ;

	                            } else {

	                                if (argr <= 0) 
						goto badargnum ;

	                                argp = argv[++i] ;
	                                argr -= 1 ;
	                                argl = lenstr(argp) ;

	                                if (argl)
	                                    remoteuser = argp ;

	                            }

	                            break ;

/* do not read input */
	                        case 'n':
	                            opt_n = "-n" ;
	                            f_n = true ;
	                            break ;
/* options */
	                        case 'o':
	                            f_usessh = true ;
	                            break ;

/* export variables in RXPORT */
	                        case 'x':
	                            f_x = true ;
	                            if (f_optequal) {

	                                rxport = "" ;
	                                f_optequal = false ;
	                                if (avl)
	                                    rxport = avp ;

	                            }

	                            break ;

/* verbose mode */
	                        case 'v':
	                            f_verbose = true ;
	                            break ;

	                        case '?':
	                            f_usage = true ;
					break ;

	                        default:
				    ex = EX_USAGE ;
	                            f_usage = true ;
	                            bprintf(efp,"%s : unknown option - %c\n",
	                                pip->progname,*akp) ;

	                        } /* end switch */

	                        akp += 1 ;

	                    } /* end while */

	                } /* end if (individual option key letters) */

	            } /* end if (digits as argument or not) */

	        } else {

/* we have a plus or minux sign character alone on the command line */

	            npa += 1 ;

	        } /* end if */

	    } else {

	        switch (npa) {

	        case 0:
	            remotehost = argp ;
	            break ;

	        case 1:
	            remotecommand = argp ;
	            f_exitargs = true ;
	            rai = i + 1 ;
	            break ;

	        default:
	            bprintf(efp,
	                "%s: extra positional arguments ignored\n",
	                pip->progname) ;

	        } /* end switch */

	        npa += 1 ;

	    } /* end if (key letter/word or positional) */

	} /* end while (all command line argument processing) */


	if (pip->debuglevel > 0) 
		bprintf(efp,
	    		"%s: debuglevel=%u\n",
	    		pip->progname,pip->debuglevel) ;

	if (f_version) 
		bprintf(efp,"%s: version %s\n",
	    pip->progname,VERSION) ;

	if (f_usage) 
		goto usage ;

	if (f_version) 
		goto retearly ;


/* check some arguments */

	if (! f_x) {

	    if ((rxport = getenv("RXPORT")) == nullptr)
	        rxport = RXPORT ;

	}

	if ((remotehost == nullptr) || (remotehost[0] == '\0'))
	    goto badhost ;

	if ((remotecommand == nullptr) || (remotecommand[0] == '\0')) {

	    if ((remotecommand = getenv("SHELL")) == nullptr)
	        remotecommand = "/bin/sh" ;

	}

#if	CF_DEBUG
	if (pip->debuglevel > 2)
	    debugprintf("main: entered DL=%d host=%s cmd>\n%s\n",
	        pip->debuglevel,remotehost,remotecommand) ;
#endif


/* open up a log file if we are supposed to */

	if (f_log) {

#if	CF_DEBUG
	    if (pip->debuglevel > 2)
	        debugprintf("main: logging requeted\n") ;
#endif

	    if (logfname == nullptr) {

	        mkpath2(buf,pip->pr,LOGFNAME) ;

	        logfname = mallocstr(buf) ;

	    }

#if	CF_DEBUG
	    if (pip->debuglevel > 2)
	        debugprintf("main: log filename=\"%s\"\n",logfname) ;
#endif

	    if (pip->debuglevel)
	        bprintf(efp,"%s: logfile=%s\n",
	            pip->progname,logfname) ;

	    f_log = false ;
	    if ((logfname != nullptr) && (logfname[0] != '\0')) {
		int	rs1 ;

#if	CF_DEBUG
	        if (pip->debuglevel > 2)
	            debugprintf("main: good logfile\n") ;
#endif

		rs1 = userinfo(&u,userbuf,USERINFO_LEN,nullptr) ;
	        f_userinfo = (rs1 >= 0) ? true : false ;

	        if (f_userinfo &&
	            (logfile_open(&lh,logfname,0,0666,u.logid) >= 0))
	            f_log = true ;

#if	CF_DEBUG
	        if (pip->debuglevel > 2)
	            debugprintf("main: log=%d\n",f_log) ;
#endif

	    } /* end if */


#ifdef	SYSV
	    f_sysvct = true ;
#else
	    f_sysvct = false ;
#endif

	    f_sysvrt = false ;
	    if (access("/usr/sbin",R_OK) == 0) f_sysvrt = true ;

	    daytime = time(nullptr) ;

	    logfile_printf(&lh,"%s %-14s %s/%s\n",
	        timestr_log(daytime,buf),pip->progname,VERSION,
	        (f_sysvct) ? "SYSV" : "BSD") ;

	    logfile_printf(&lh,"os=%s %s!%s(%s) uid=%d\n",
	        (f_sysvrt) ? "SYSV" : "BSD",
	        u.nodename,u.username,u.mailname,u.uid) ;

	    logfile_printf(&lh,"cmd=%s\n",remotecommand) ;

	} /* end if (making first log entry) */

	if (pip->debuglevel > 1) 
		bprintf(efp,
	    "%s: cmd=%s\n",pip->progname,remotecommand) ;


/* OK so far, get some command names */

	f_ssh = false ;
	if (f_usessh && (getfiledirs(nullptr,"ssh","x",nullptr) > 0) &&
	    (getfiledirs(nullptr,"scp","x",nullptr) > 0)) {

	    f_ssh = true ;
	    cmd_rsh = "ssh" ;
	    cmd_rcp = "scp -o \"BatchMode yes\"" ;

	} else {

	    cmd_rsh = "/bin/rsh" ;
	    if (access(cmd_rsh,X_OK) != 0) {

	        cmd_rsh = "/usr/ucb/rsh" ;
	        if (access(cmd_rsh,X_OK) != 0) 
			return EX_OSFILE ;

	    }

	    cmd_rcp = "/bin/rcp" ;
	    if (access(cmd_rcp,X_OK) != 0) {

	        cmd_rcp = "/usr/ucb/rcp" ;
	        if (access(cmd_rcp,X_OK) != 0) 
			return EX_OSFILE ;

	    }

	} /* end if */


/* get our host (nodename) and domain name */

	if (f_userinfo) {

	    nnp = u.nodename ;
	    dnp = u.domainname ;

	} else {

	    getnodedomain(nodename,domainname) ;

	    nnp = nodename ;
	    dnp = domainname ;

	    getusername(buf,BUFLEN,-1) ;

	    u.username = mallocstr(buf) ;

	}

#if	CF_DEBUG
	if (pip->debuglevel > 2)
	    debugprintf("main: h=%s d=%s ru=%s\n",
	        nnp,dnp,
	        (remoteuser[0] == '\0') ? "**unspecified**" : remoteuser) ;
#endif

	snsds(hostname,MAXHOSTNAMELEN,nnp,dnp) ;

/* is this going over to another INET domain? */

	getcanonical(remotehost,rhost,rnodename,rdomainname) ;

/* put together the remote command file */

#if	CF_DEBUG
	if (pip->debuglevel > 1)
	    debugprintf("main: about to create the jobfile\n") ;
#endif

	jobfname[0] = '\0' ;
	rs = mkfilejob(tmpdir,0740,jobfname) ;

	if (rs < 0)
		goto badjobfile ;

#if	CF_DEBUG
	if (pip->debuglevel > 1)
	    debugprintf("main: jobfname=%s\n",jobfname) ;
#endif

	rs = bopen(jfp,jobfname,"wct",0744) ;

	if (rs < 0)
		goto badjobopen ;

#if	CF_DEBUG
	if (pip->debuglevel > 1)
	    debugprintf("main: opened the jobfname\n") ;
#endif


	bprintf(jfp," # <-- force CSH to use Bourne shell\n") ;

	bprintf(jfp,"# RSHE %s\n",timestr_log(daytime,buf)) ;


/* put some stuff together for later */

	ji.f_remotedomain = 
	    (strcmp(dnp,rdomainname) != 0) ? true : false ;

	ji.nodename = nnp ;
	ji.domainname = dnp ;
	ji.jfp = jfp ;


/* execute some stuff on the remote which is supposed to be there already */

	if (remoteuser[0] == '\0')
	    bprintf(jfp,": ${LOGNAME:=${USER}}\nexport LOGNAME\n") ;

	else
	    bprintf(jfp,"LOGNAME=%s\nexport LOGNAME\n",remoteuser) ;


/* have the remote side execute the environment file if it exists */

	if (envfile[0] != '\0') {

	    if (pip->debuglevel > 0) {

	        bprintf(efp,"%s: envfile=\"%s\"\n",
	            pip->progname,envfile) ;

	    }

	    if (f_log) {

	        logfile_printf(&lh,"envfile=\"%s\"\n",
	            envfile) ;

	    }

	    bprintf(jfp,"if [ -r %s%s ] ; then\n",
	        (envfile[0] != '/') ? "${HOME}/" : "",envfile) ;

	    bprintf(jfp,". %s%s < /dev/null > /dev/null 2>&1\n",
	        (envfile[0] != '/') ? "${HOME}/" : "",envfile) ;

	    bprintf(jfp,"fi\n") ;

	} /* end if (processing the remote environment file) */


/* OK, now we send over all (essentially) or some of our local environemnt */

	if (pip->debuglevel > 0) {
	    bprintf(efp,"%s: rhost=%s X=%d N=%d\n",
	        pip->progname,remotehost,f_x,f_n) ;
	}

	if (f_log) {
	    logfile_printf(&lh,"rhost=%s X=%d N=%d\n",
	        remotehost,f_x,f_n) ;
	}

	if (f_x && (rxport == nullptr)) {
	    send_procenv(pip,&ji) ;
	} else {
	    send_rxenv(pip,&ji,rxport) ;
	}

#if	CF_DEBUG
	if (pip->debuglevel > 1)
	    debugprintf("main: sent over the environment\n") ;
#endif


/* OK, now send over the stuff that is fixed */

	bprintf(jfp,"RUNMODE=rcmd\nexport RUNMODE\n") ;

	bprintf(jfp,"RCMD_MACHINE=%s\nexport RCMD_MACHINE\n",
	    hostname) ;

	bprintf(jfp,"RCMD_USER=%s\nexport RCMD_USER\n",
	    u.username) ;


/* should we arrange for a change of directory on the remote? */

	cwd = nullptr ;
	if (f_cd) {

#if	CF_DEBUG
	if (pip->debuglevel > 1)
	    debugprintf("main: handling a CHDIR\n") ;
#endif

	    if (getpwd(buf,BUFLEN) >= 0)
	        cwd = mallocbuf(buf,BUFLEN) ;

	    if (cwd != nullptr) {

	        if (pip->debuglevel > 0)
	            bprintf(efp,"%s: CHDIR=%s\n",
	                pip->progname,cwd) ;

	        if (f_log)
	            logfile_printf(&lh,"CHDIR=%s\n",
	                cwd) ;

	        bprintf(jfp,"if [ -d %s -a -x %s ] ; then\n",cwd,cwd) ;

	        bprintf(jfp,"cd %s 2> /dev/null\n",cwd) ;

	        bprintf(jfp,"else\n") ;

	        bprintf(jfp,"exit 128\n") ;

	        bprintf(jfp,"fi\n") ;

	    } /* end if (we have a directory) */

	} /* end if (arranging for a change of directory on remote) */


/* set the PWD environment variable properly */

	if (cwd == nullptr) {

	    bprintf(jfp,"if [ -z \"${PWD}\" ] ; then\n") ;

	    bprintf(jfp,"PWD=${HOME}\nexport PWD\n") ;

	    bprintf(jfp,"fi\n") ;

	} else 
	    bprintf(jfp,"PWD=%s\nexport PWD\n",cwd) ;


/* get ready to boogie */

	if (pip->debuglevel > 0) {

#if	CF_DEBUG
	if (pip->debuglevel > 1)
	    debugprintf("main: jobfname=%s\n",jobfname) ;
#endif

	    bprintf(efp,"%s: jobfname=%s\n",
	        pip->progname,jobfname) ;

	}

	if (f_log) {

	    logfile_printf(&lh,"jobfname=%s\n",
	        jobfname) ;

	}

	bprintf(jfp,"/bin/rm -f %s\n",jobfname) ;

	bprintf(jfp,"exec %s",remotecommand) ;

	if (npa >= 2) {

	    n = 1 ;
	    for (i = rai ; argv[i] != nullptr ; i += 1) {

	        bufstr	value ;

	        char	*vp ;


#if	CF_DEBUG
	        if (pip->debuglevel > 2)
	            debugprintf("main: argv[%d]=%s\n",i,argv[i]) ;
#endif

	        bufstr_start(&value) ;

	        rs = quotevalue(pip,argv[i],&value) ;

	        if (rs < 0) goto badalloc ;

	        l = bufstr_get(&value,&vp) ;

#if	CF_DEBUG
	        if (pip->debuglevel > 2)
	            debugprintf("main: arg%d=%s\n",n,vp) ;
#endif

	        bprintf(jfp," \"%W\"",vp,l) ;

	        bufstr_finish(&value) ;

	        n += 1 ;

	    } /* end for */

	} /* end if (we had arguments) */

	bprintf(jfp,"\n") ;

	bprintf(jfp,"exit 129\n") ;

	if ((rs = bclose(jfp)) < 0) 
		goto badhost2 ;

/* done creating the remote execution SHELL program */


/* if the target machine is really remote, send the job file over to it */

	if ((pip->debuglevel > 1) ||
		(ji.f_remotedomain || (strcmp(nnp,rnodename) != 0))) {

#if	CF_DEBUG
	    if (pip->debuglevel > 2)
	        debugprintf("main: the target is remote (usual case)\n") ;
#endif

/* put the job command file on the remote host */

	    bufprintf(cmd,CMDBUFLEN,"%s %s %s%s%s:%s",
	        cmd_rcp,
		jobfname,
	        remoteuser,(remoteuser[0] != '\0') ? "@" : "",
	        remotehost,jobfname) ;

#if	CF_DEBUG
	    if (pip->debuglevel > 2)
	        debugprintf("main: RCP cmd> %s\n",cmd) ;
#endif

	    fpa[0] = &file0 ;
	    fpa[1] = &file1 ;
	    fpa[2] = &file2 ;
	    cpid = bopencmd(fpa,cmd) ;

/* close the program's standard input */

	    bclose(&file0) ;

	    len = 0 ;
	    while ((len < BUFLEN) && (l = bread(&file2,buf + len,BUFLEN)) > 0)
	        len += l ;

	    bdump(&file1,&file2) ;

	    if (len > 0) {

#if	CF_DEBUG
	        if (pip->debuglevel > 2)
	            debugprintf("main: RCP ouput> %W\n",buf,len) ;
#endif

	        if (f_log) {

	            if (buf[len - 1] == '\n') len -= 1 ;

	            logfile_printf(&lh,"authorization failed> %W\n",
	                buf,len) ;

	        }

	        goto badperm ;
	    }

	    u_waitpid(cpid,&childstat,WUNTRACED) ;


/* arrange for it to be deleted (locally) in due course */

	    if (unlinkd(jobfname,WORMTIMEOUT) < 0) {

	        sprintf(cmd,
	            "echo rm -f %s | at now + 2 minute > /dev/null 2>&1",
	            jobfname) ;

	        system(cmd) ;

	    }

	} /* end if (handling a remote job) */


/* execute the job command file on the remote host */

	vecstr_start(&argvec,10,0) ;

	if (f_ssh) {

	    vecstr_add(&argvec,"ssh",-1) ;

	    vecstr_add(&argvec,"-o",-1) ;

	    vecstr_add(&argvec,"BatchMode yes",-1) ;

	} else
	    vecstr_add(&argvec,"rsh",-1) ;

	if (opt_n[0] != '\0') {

#if	CF_DEBUG
	    if (pip->debuglevel > 2)
	        debugprintf("main: we are in \"%s\" mode\n",opt_n) ;
#endif

	    vecstr_add(&argvec,opt_n,-1) ;

	}

	cp = nullptr ;
	if (remoteuser[0] != '\0')
	    cp = remoteuser ;

	else if (f_userinfo)
	    cp = u.username ;

	else if (getusername(buf,BUFLEN,-1) >= 0)
	    cp = buf ;

	if (cp != nullptr) {

	    vecstr_add(&argvec,"-l",2) ;

	    vecstr_add(&argvec,cp,-1) ;

	    logfile_printf(&lh,"connected ruser=%s\n",cp) ;

	}

	vecstr_add(&argvec,remotehost,-1) ;

	vecstr_add(&argvec,"/bin/sh",-1) ;

	vecstr_add(&argvec,jobfname,-1) ;

/* do it */

#if	CF_DEBUG
	if (pip->debuglevel > 1)
	    debugprintf("main: about to exec \"%s\"\n",cmd_rsh) ;
#endif

	bflush(efp) ;

	if (opt_n[0] != '\0')
	    u_close(FD_STDIN) ;

#if	CF_DEBUG
	if (pip->debuglevel > 2) {

	    debugprintf("main: final command dump\n") ;

	    for (i = 0 ; argvec.va[i] != nullptr ; i += 1)
	        debugprintf("main: arg%d=%s\n",i,argvec.va[i]) ;

	    debugprintf("main: exec'ing\n") ;

	}
#endif /* CF_DEBUG */

#if	defined(BRAINDAMAGEWAIT) && BRAINDAMAGEWAIT
	if (BRAINDAMAGEWAIT > 0)
	    sleep(BRAINDAMAGEWAIT) ;
#endif

	rs = u_execvp(cmd_rsh,argvec.va) ;

/* if we get to this point, we are bad since the 'exec' failed */

	bprintf(efp,
	    "%s: program \"%s\" would not 'exec' correctly (rs %d)\n",
	    pip->progname,cmd_rsh,rs) ;

	goto badret ;


/* return here */
retearly:
ret1:
	bclose(efp) ;

ret0:

#if	(CF_DEBUGS || CF_DEBUG)
	debugclose() ;
#endif

	return ex ;

/* information */
usage:
	bprintf(efp,
	    "%s: USAGE> %s [-nx] [-l user] host cmd [args ...]\n",
	    pip->progname,pip->progname) ;

	goto retearly ;

/* bad returns */
badperm:
	bprintf(efp,"%s: you are not authorized on the remote machine\n",
	    pip->progname) ;

	goto badret ;

badhost2:
badalloc:
	u_unlink(jobfname) ;

	bprintf(efp,"%s: bad memory allocation\n",
	    pip->progname) ;

	goto badret ;

badjobopen:
	u_unlink(jobfname) ;

	bprintf(efp,"%s: could not open the job file \"%s\" (rs %d)\n",
	    pip->progname,jobfname,rs) ;

	goto badret ;

badjobfile:
	bprintf(efp,"%s: could not create a job file (rs %d)\n",
	    pip->progname,rs) ;

	goto badret ;

badhost:
badcmd:
	ex = EX_DATAERR ;
	goto badret ;

notenough:
	bprintf(efp,"%s: not enough arguments given\n",pip->progname) ;

	goto badret ;

badargextra:
	bprintf(efp,"%s: no value associated with this option key\n",
	    pip->progname) ;

	goto badret ;

badargvalue:
	bprintf(efp,"%s: bad argument value was specified\n",
	    pip->progname) ;

	goto badret ;

badargnum:
	bprintf(efp,"%s: not enough arguments specified\n",
	    pip->progname) ;

	goto badret ;


badret:
	if (jobfname[0] != '\0') {
		u_unlink(jobfname) ;
	}

	goto ret1 ;

}
/* end subroutine (main) */


/* local subroutines */

/* get the canonical name of a host */
/*************************************************************************

	Arguements:
	name		name given to us
	rhost		result name returned (possibly fully qualified)
	rnode		result node name returned
	rdomain		result domain name returned

*******************************************************************************/

local int getcanonical(cchar *name,char *rhost,char *rnode,char *rdom) noex {
	int		rs ;
	rdom[0] = '\0' ;
	if ((rs = getchostname(rhost,name)) >= 0) {
	    cchar	*cp ; /* used-multiple */
	    if ((cp = strchr(rhost,'.')) != nullptr) {
		cint cl = intconv(cp - rhost) ;
	        strwcpy(rnode,rhost,cl) ;
	        strcpy(rdom,cp + 1) ;
	    } /* end if */
	    if (rdom[0] == '\0') {
	        if ((cp = strchr(name,'.')) != nullptr) {
		    cint cl = intconv(cp - name) ;
	            strcpy(rhost,name) ;
	            strwcpy(rnode,name,cl) ;
	            strcpy(rdom,cp + 1) ;
	        } /* end if */
	    } /* end if (still needed) */
	} /* end if (getchostname) */
	return rs ;
} /* end subroutine (getcanonical) */

/* send over the variables in RXPORT */
local int send_rxenv(proginfo *pip,jobinfo *jip,char *rxport) noex {
	int		klen ;
	int		rs ;
	char		*keyp = rxport ;
	char		*cp ;

#if	CF_DEBUG
	if (pip->debuglevel > 2)
	    debugprintf("send_rxenv: entered\n") ;
#endif

	if ((rxport == nullptr) || (*rxport == '\0')) 
		return OK ;

#if	CF_DEBUG
	if (pip->debuglevel > 2) {
	    debugprintf("send_rxenv: about to loop\n") ;
	    debugprintf("send_rxenv: rxport=\"%s\"\n",rxport) ;
	}
#endif

	while ((cp = strchr(keyp,',')) != nullptr) {
	    klen = cp - keyp ;

#if	CF_DEBUG
	    if (pip->debuglevel > 2)
	        debugprintf("send_rxenv: var=\"%W\"\n",keyp,klen) ;
#endif

	    rs = proc_rxenv(pip,jip,keyp,klen) ;
	    keyp = cp + 1 ;

	    if (rs < 0) break ;
	} /* end while */

	if (rs >= 0) {

	klen = lenstr(keyp) ;

#if	CF_DEBUG
	if (pip->debuglevel > 2)
	    debugprintf("send_rxenv: var=\"%W\"\n",keyp,klen) ;
#endif

	rs = proc_rxenv(pip,jip,keyp,klen) ;

	} /* end if (ok) */

	return rs ;
} /* end subroutine (send_rxenv) */

/* process the RXPORT type environment variables */
local int proc_rxenv(proginfo *pip,jobinfo *jip,char *key,int klen) noex {
	int		elen ;
	cchar		**ep ;
	char		*cp ;

#if	CF_DEBUG
	if (pip->debuglevel > 2)
	    debugprintf("proc_rxenv: entered var=\"%W\"\n",key,klen) ;
#endif

	if (klen <= 0) 
		return OK ;

/* search for this variable name in the process environment list */

	for (ep = pip->envv ; (*ep) != nullptr ; ep += 1) {

	    if ((cp = strchr((*ep),'=')) == nullptr) 
		continue ;

	    elen = cp - (*ep) ;
	    if ((elen == klen) && (strnncmp(*ep,elen,key,klen) == 0))
	        return proc_env(pip,jip,key,klen,cp + 1) ;

	} /* end for */

	return OK ;
} /* end subroutine (proc_rxenv) */

/* send over the current process environment */
local int send_procenv(proginfo *pip,jobinfo *jip) noex {
	int		rs = SR_OK ;
	int		i, klen ;
	cchar		**ep ;
	char		*cp ;

/* write out our process environment to the remote command */

	ep = pip->envv ;
	for (i = 0 ; ep[i] != nullptr ; i += 1) {

#if	CF_DEBUG
	    if (pip->debuglevel > 2)
	        debugprintf("send_procenv: top environment loop\n") ;
#endif

	    if ((ep[i][0] == '_') || (((unsigned int) ep[i][0]) < ' ')) 
		continue ;

/* break it up into the two pieces */

	    if ((cp = strchr(ep[i],'=')) != nullptr) {

	        klen = cp - ep[i] ;	/* length of variable name */

#if	CF_DEBUG
	        if (pip->debuglevel > 2) {

	            debugprintf("send_procenv: environment name=%W\n",
	                ep[i],klen) ;

	            debugprintf("send_procenv: environment value=%s\n",
	                cp + 1) ;

	        }
#endif

	        rs = proc_env(pip,jip,ep[i],klen,(cp + 1)) ;

	        if (rs < 0) break ;
	    } /* end if (we have a value to this variable) */

	    if (rs < 0) break ;
	} /* end for (sending over environment) */

	return OK ;
} /* end subroutine (send_procenv) */

/* process an environment variable */

/****
	Arguments:
	- kp	pointer to the keyname string
	- klen	length of keyname string
	- vp	pointer to the value string
****/

local int proc_env(proginfo *pip,jobinfo *jip,cc *kp,int klen,cc *vp) noex {
	bufstr		value ;
	int		rs ;
	int		sl ;
	char		*cp ;
	char		displaybuf[BUFLEN + 1] ;
	char		*nvp ;

	if (klen < 0) 
		klen = lenstr(kp) ;

/* perform some preprocessing on specific variables */

	if (strnncmp(kp,klen,"DISPLAY",-1) == 0) {

#if	CF_DEBUG
	    if (pip->debuglevel > 2)
	        debugprintf("proc_env: special DISPLAY value=%s\n",vp) ;
#endif

	    rs = BAD ;

	    cp = (jip->f_remotedomain) ? jip->domainname : "" ;
	    rs = qualdisplay(vp,jip->nodename,cp,displaybuf,
	        DISBUFLEN) ;

	    if (rs >= 0)
	        vp = displaybuf ;

#if	CF_DEBUG
	    if (pip->debuglevel > 2)
	        debugprintf("proc_env: DISPLAY after qualled value=%s\n",
		vp) ;
#endif

	} else if ((strnncmp(kp,klen,"HOME",-1) == 0) ||
	    (strnncmp(kp,klen,"PWD",-1) == 0) ||
	    (strnncmp(kp,klen,"LOGNAME",-1) == 0) ||
	    (strnncmp(kp,klen,"HZ",-1) == 0) ||
	    (strnncmp(kp,klen,"TERM",-1) == 0)) {

	    return OK ;

	} else if (strnncmp(kp,klen,"LOCALDOMAIN",-1) == 0) {

#if	CF_DEBUG
	    if (pip->debuglevel > 2)
	        debugprintf("proc_env: got a LOCALDOMAIN variable \"%W\"\n",
	            kp,klen) ;
#endif

	    if (jip->f_remotedomain) 
		return OK ;

	} /* end if (processing special environment variables) */

/* perform general character escape processing */

#if	CF_DEBUG
	if (pip->debuglevel > 2)
	    debugprintf("proc_env: about to call quote\n") ;
#endif

	rs = OK ;
	if (klen > 0) {
	    if ((rs = bufstr_start(&value)) >= 0) {

	    if ((rs = quotevalue(pip,vp,&value)) >= 0) {
	        sl = bufstr_get(&value,&nvp) ;
	        bprintf(jip->jfp,"%W=\"%W\"\n", kp,klen, nvp,sl) ;
	        bprintf(jip->jfp,"export %W\n", kp,klen) ;
	    }

	    bufstr_finish(&value) ;
	    } /* end if (bufstr) */
	} /* end if (we have something to send over) */

	return rs ;
} /* end subroutine (proc_env) */

/* quote a string for SHELL escaping */
local int quotevalue(proginfo *pip,cchar *vs,bufstr *bsp) noex {
	int		f_got = false ;
	int		rs = 0 ;
	cchar	*tp ;
	cchar	*cp ;

#if	CF_DEBUG
	if (pip->debuglevel > 2)
	    debugprintf("quotevalue: entered value=%s\n",vs) ;
#endif

	cp = vs ;
	while ((tp = strbrk(cp,"\\\"'$")) != nullptr) {

#if	CF_DEBUG
	    if (pip->debuglevel > 2)
	        debugprintf("quotevalue: got one\n") ;
#endif

	    f_got = true ;
	    bufstr_strw(bsp,cp,(tp - cp)) ;

	    bufstr_chr(bsp,'\\') ;

	    rs = bufstr_chr(bsp,*tp++) ;

	    cp = tp ;

#if	CF_DEBUG
	    if (pip->debuglevel > 2)
	        debugprintf("quotevalue: bottom of got one\n") ;
#endif

	} /* end while */

	if (rs >= 0) {
	if (f_got) {
	    rs = bufstr_strw(bsp,cp,-1) ;
	} else {
	    rs = bufstr_strw(bsp,vs,-1) ;
	}
	}

#if	CF_DEBUG
	if (pip->debuglevel > 2)
	    debugprintf("quotevalue: exiting value=%s len=%d\n",bsp->s,rs) ;
#endif

	return rs ;
} /* end subroutine (quotevalue) */

/* dump data from pipes */

/****
	This is a cheap way to try and read the junk coming out of two
	pipes without getting into a situation where the sending program
	blocks.
****/

#define	DUMPLEN	50

static void bdump(bfile *f1p,bfile *f2p) noex {
	int		f_done1, f_done2 ;
	char		buf[DUMPLEN + 1] ;

	f_done1 = f_done2 = false ;
	while ((! f_done1) || (! f_done2)) {

	    if (! f_done1) {
	        if (bread(f1p,buf,DUMPLEN) <= 0) f_done1 = true ;
	    }

	    if (! f_done2) {
	        if (bread(f2p,buf,DUMPLEN) <= 0) f_done2 = true ;
	    }

	} /* end while */

} /* end subroutine (bdump) */



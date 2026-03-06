/* chostname_main (CHOSTNAME) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* program to get a canonical host name */
/* version %I% last-modified %G% */


/* revision history:

	- 1998-05-23, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Synopsis:
	$ chostname name(s) [-v] [-V]

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<netdb.h>
#include	<poll.h>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strchr(3c)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<getbufsize.h>
#include	<baops.h>
#include	<bfile.h>
#include	<userinfo.h>
#include	<inetaddr.h>
#include	<getchostname.h>
#include	<exitcodes.h>
#include	<mallocstuff.h>
#include	<localmisc.h>

#include	"config.h"
#include	"defs.h"


/* local defines */

#define	PI		proginfo

#define	MAXARGINDEX	100
#define	NARGGROUPS	(MAXARGINDEX/8 + 1)

#ifndef	USERBUFLEN
#define	USERBUFLEN	(NODENAMELEN + (2 * 1024))
#endif

#define	IABUFLEN	40


/* external subroutines */

extern int	proginfo_setpiv(PI *,cchar *,const pivars *) ;
extern int	printhelp(void *,cchar *,cchar *,cchar *) ;


/* external variables */

extern char	chostname_makedate[] ;


/* local structures */


/* forward references */

local int	usage(PI *) ;
local int	procname(PI *,bfile *,cchar *) ;
local int	printinfo(PI *,int,bfile *,cchar *,int,char *) ;


/* local variables */

enum argopts {
	argopt_root,
	argopt_tmpdir,
	argopt_version,
	argopt_verbose,
	argopt_help,
	argopt_logfile,
	argopt_af,
	argopt_of,
	argopt_overlast
} ;

static cchar *argopts[] = {
	"ROOT",
	"TMPDIR",
	"VERSION",
	"VERBOSE",
	"HELP",
	"LOGFILE",
	"af",
	"of",
	NULL
} ;

static const struct pivars	initvars = {
	VARPROGRAMROOT1,
	VARPROGRAMROOT2,
	VARPROGRAMROOT3,
	PROGRAMROOT,
	VARPRLOCAL
} ;


/* exported variables */


/* exported subroutines */

int main(int argc,mainv argv,mainv envv) {
	PROGINFO	pi, *pip = &pi ;
	userinfo	u ;
	bfile		errfile ;
	bfile		outfile, *ofp = &outfile ;

	int	argr, argl, aol, akl, avl, kwi ;
	int	ai, ai_max, ai_pos ;
	int	argvalue = -1 ;
	int	pan ;
	int	rs ;
	int	ex = EX_INFO ;
	int	f_optplus, f_optminus, f_optequal ;
	int	f_version = false ;
	int	f_usage = false ;
	int	f_help = false ;
	int	f ;

	cchar	*argp, *aop, *akp, *avp ;
	char	argpresent[NARGGROUPS] ;
	char	buf[BUFLEN + 1] ;
	char	tmpfname[MAXPATHLEN + 1] ;
	char	userbuf[USERBUFLEN + 1] ;
	char	timebuf[TIMEBUFLEN + 1] ;
	cchar	*pr = NULL ;
	cchar	*afname = NULL ;
	cchar	*ofname = NULL ;
	cchar	*logfname = NULL ;
	cchar	*cp ;

	rs = proginfo_start(pip,envv,argv[0],VERSION) ;
	if (rs < 0) {
	    ex = EX_OSERR ;
	    goto badprogstart ;
	}

	if ((cp = getourenv(envv,VARBANNER)) == NULL) cp = BANNER ;
	rs = proginfo_setbanner(pip,cp) ;

	if ((cp = getenv(VARERRORFNAME)) != NULL) {
	    rs = bopen(&errfile,cp,"wca",0666) ;
	} else
	    rs = bopen(&errfile,BFILE_STDERR,"dwca",0666) ;
	if (rs >= 0) {
	    pip->efp = &errfile ;
	    bcontrol(&errfile,BC_LINEBUF,0) ;
	}

/* miscellaneous early stuff */

	pip->verboselevel = 1 ;

/* parse arguments */

	for (ai = 0 ; ai < NARGGROUPS ; ai += 1) 
		argpresent[ai] = 0 ;

	ai = 0 ;
	ai_max = 0 ;
	ai_pos = 0 ;
	argr = argc - 1 ;
	while ((rs >= 0) && (argr > 0)) {

	    argp = argv[++ai] ;
	    argr -= 1 ;
	    argl = strlen(argp) ;

	    f_optminus = (*argp == '-') ;
	    f_optplus = (*argp == '+') ;
	    if ((argl > 1) && (f_optplus || f_optminus)) {
		cint	ach = MKCHAR(argp[1]) ;

	        if (isdigitlatin(ach)) {

		    rs = cfdeci((argp + 1),(argl - 1),&argvalue) ;

		} else if (ach == '-') {

		    ai_pos = ai ;
		    break ;

		} else {

	            aop = argp + 1 ;
	            akp = aop ;
	            aol = argl - 1 ;
	            f_optequal = false ;
	            if ((avp = strchr(aop,'=')) != NULL) {
	                f_optequal = true ;
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

	                            f_optequal = false ;
	                            if (avl) 
					pr = avp ;

	                        } else {

	                            if (argr <= 0) {
					rs = SR_INVALID ;
					break ;
				    }

	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;

	                            if (argl) 
					pr = argp ;

	                        }

	                        break ;

/* temporary directory */
	                    case argopt_tmpdir:
	                        if (f_optequal) {

	                            f_optequal = false ;
	                            if (avl) 
					pip->tmpdname = avp ;

	                        } else {

	                            if (argr <= 0) {
					rs = SR_INVALID ;
					break ;
				    }

	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;

	                            if (argl) 
					pip->tmpdname = argp ;

	                        }

	                        break ;

/* version */
	                    case argopt_version:
	                        f_version = true ;
	                        if (f_optequal) 
					rs = SR_INVALID ;

	                        break ;

/* verbose mode */
	                    case argopt_verbose:
	                        pip->verboselevel = 2 ;
	                        break ;

	                case argopt_help:
	                    f_help = true ;
	                    break ;

/* LOGFILE */
	                    case argopt_logfile:
	                        if (f_optequal) {

	                            f_optequal = false ;
	                            if (avl) 
					logfname = avp ;

	                        } else {

	                            if (argr <= 0) {
					rs = SR_INVALID ;
					break ;
				    }

	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;

	                            if (argl) 
					logfname = argp ;

	                        }

	                        break ;

/* argument list file */
	                        case argopt_af:
	                            if (f_optequal) {

	                                f_optequal = false ;
	                                if (avl)
	                                    afname = avp ;

	                            } else {

	                            if (argr <= 0) {
					rs = SR_INVALID ;
					break ;
				    }

	                                argp = argv[++ai] ;
	                                argr -= 1 ;
	                                argl = strlen(argp) ;

	                                if (argl)
	                                    afname = argp ;

	                            }

	                            break ;

/* output file */
	                        case argopt_of:
	                            if (f_optequal) {

	                                f_optequal = false ;
	                                if (avl)
	                                    ofname = avp ;

	                            } else {

	                            if (argr <= 0) {
					rs = SR_INVALID ;
					break ;
				    }

	                                argp = argv[++ai] ;
	                                argr -= 1 ;
	                                argl = strlen(argp) ;

	                                if (argl)
	                                    ofname = argp ;

	                            }

	                            break ;

/* handle all keyword defaults */
	                    default:
				rs = SR_INVALID ;
	                        bprintf(pip->efp,
	                        "%s: invalid key=%r\n",
	                        pip->progname,akp,akl) ;

	                    } /* end switch */

	                } else {

	                    while (akl--) {

	                        switch ((int) *akp) {

/* debug */
	                        case 'D':
	                            pip->debuglevel = 1 ;
	                            if (f_optequal) {

	                                f_optequal = false ;
	                                if (avl)
	                                    rs = cfdeci(avp,avl,
						&pip->debuglevel) ;

	                            }

	                            break ;

	                        case 'V':
	                            f_version = true ;
	                            break ;

	                        case 'Q':
	                            pip->fl.quiet = true ;
	                            break ;

	                        case 'a':
	                            pip->fl.auth = true ;
	                            break ;

/* verbose */
	                        case 'v':
	                            pip->verboselevel = 2 ;
	                            if (f_optequal) {
	                                f_optequal = false ;
	                                if (avl)
	                                    rs = cfdeci(avp,avl,
						&pip->verboselevel) ;
	                            }
	                            break ;

				case '?':
	                            f_usage = true ;
					break ;
	                        default:
				    rs = SR_INVALID ;
	                            bprintf(pip->efp,
	                            "%s: invalid option=%c\n",
	                            pip->progname,*akp) ;
	                        } /* end switch */
	                        akp += 1 ;
				if (rs < 0) break ;
	                    } /* end while */
	                } /* end if (individual option key letters) */
	            } /* end if (digits as argument or not) */
	    } else {
	        if (ai >= MAXARGINDEX) break ;
	        BASET(argpresent,ai) ;
	        ai_max = ai ;
	    } /* end if (key letter/word or positional) */
	    ai_pos = ai ;
	} /* end while (all command line argument processing) */
	if (rs < 0) goto badarg ;

	if (pip->debuglevel > 0)
	    bprintf(pip->efp,"%s: debuglevel=%u\n",
	        pip->progname,pip->debuglevel) ;

	rs = OK ;
	if (f_version)
	    bprintf(pip->efp,"%s: version %s\n",
	        pip->progname,VERSION) ;

	if (f_usage) 
		goto usage ;

	if (f_version) 
		goto retearly ;


/* get our program root directory */

	rs = proginfo_setpiv(pip,pr,&initvars) ;

	if (rs < 0) {
	    ex = EX_USAGE ;
	    goto retearly ;
	}

/* program search name */

	proginfo_setsearchname(pip,VARSEARCHNAME,SEARCHNAME) ;

	if (pip->debuglevel > 0)
	    bprintf(pip->efp,"%s: pr=%s\n",
	        pip->progname, pip->pr) ;

/* help file */

	if (f_help)
	    goto help ;


/* check arguments */

	ex = EX_OK ;

/* get a TMPDIR */

	if (pip->tmpdname == NULL) pip->tmpdname = getenv(VARTMPDNAME) ;
	if (pip->tmpdname == NULL) pip->tmpdname = TMPDNAME ;

/* initialization */

	rs = userinfo(&u,userbuf,USERBUFLEN,NULL) ;

	if (rs < 0) {
		ex = EX_NOUSER ;
		goto baduser ;
	}

	pip->nodename = u.nodename ;
	pip->domainname = u.domainname ;

/* go */

	if ((ofname != NULL) && (ofname[0] != '-')) {
	    rs = bopen(ofp,ofname,"wct",0666) ;
	} else {
	    rs = bopen(ofp,BFILE_STDOUT,"dwct",0666) ;
	}

	if (rs < 0) {
	    ex = EX_CANTCREAT ;
	    goto badoutopen ;
	}

/* process the arguments */

	pan = 0 ;

	for (ai = 1 ; ai < argc ; ai += 1) {

	    f = (ai <= ai_max) && BATST(argpresent,ai) ;
	    f = f || (ai > ai_pos) ;
	    if (! f) continue ;

	    cp = argv[ai] ;
	    pan += 1 ;
	    rs = procname(pip,ofp,cp) ;

	    if (rs < 0)
		break ;

	} /* end for (processing positional arguments) */

	if ((rs >= 0) && (pan == 0)) {

	    char	tmphostname[MAXHOSTNAMELEN + 1] ;


	    cp = tmphostname ;
	    snsds(tmphostname,MAXHOSTNAMELEN,u.nodename,u.domainname) ;

	    pan += 1 ;
	    rs = procname(pip,ofp,cp) ;

	} /* end if (processing requests) */

	bclose(ofp) ;

badoutopen:
baduser:
done:
	ex = (rs >= 0) ? EX_OK : EX_DATAERR ;

/* finish up */
ret3:
ret2:
	if (pip->debuglevel > 0)
	    bprintf(pip->efp,"%s: exiting ex=%u (%d)\n",
		pip->progname,ex,rs) ;

retearly:
ret1:
	bclose(pip->efp) ;

ret0:
	proginfo_finish(pip) ;

badprogstart:

	return ex ;

/* usage */
usage:
	usage(pip) ;
	goto retearly ;

/* print out some help */
help:
	printhelp(NULL,pip->pr,pip->searchname,HELPFNAME) ;
	goto retearly ;

badarg:
	ex = EX_USAGE ;
	bprintf(pip->efp,"%s: invalid argument specified (%d)\n",
	    pip->progname,rs) ;
	usage(pip) ;
	goto ret1 ;

}
/* end subroutine (main) */


/* local subroutines */


local int usage(pip)
PROGINFO	*pip ;
{
	int		rs = SR_OK ;
	int		wlen = 0 ;

	rs = bprintf(pip->efp,
	    "%s: USAGE> %s [<name(s)> ...] [-v] [-V]\n",
	    pip->progname,pip->progname) ;

	wlen += rs ;
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (usage) */


local int procname(pip,ofp,name)
PROGINFO	*pip ;
bfile		*ofp ;
cchar	name[] ;
{
	int	rs = SR_OK ;
	int	rs1 ;
	int	as = 0 ;

	char	chostname[MAXHOSTNAMELEN + 1] ;


	if (name == NULL)
	    return SR_FAULT ;

	if ((name[0] == '\0') || (name[0] == '-'))
	    name = pip->nodename ;

	if (pip->debuglevel > 0) {
	    bprintf(pip->efp,"%s: query=%s\n",
		pip->progname,name) ;
	}

	if (pip->verboselevel >= 2) {

	    rs = printinfo(pip,pip->fl.auth,ofp,
		name,as,chostname) ;

	} else {

	chostname[0] = '\0' ;
	if ((rs = getchostname(chostname,name)) >= 0) {
	    rs = bprintf(ofp,"%s\n",chostname) ;
	}

	if (rs < 0) {
	    if (! pip->fl.quiet) {
		bprintf(pip->efp,"%s: not found query=%s (%d)\n",
			pip->progname,name,rs1) ;
	    }
	    rs = bprintf(ofp,"\n") ;
	} /* end if */

ret0:
	return rs ;
}
/* end subroutine (procname) */

local int printinfo(PI *pip,int f_auth,bfile *ofp,cc *name,int as
		char *,cname) noex {
	cint	helen = getbufsize(bufsize_ho) ;
	int		rs ;
	int		rs1 ;
	char		*hebuf ;
	if ((rs = uc_malloc((helen+1),&gebuf)) >= 0) {
	HOSTENT	he ;
	if ((rs1 = getheour(name,cname,&he,hebuf,helen)) >= 0) {
	    hostent_cur	hc ;
	    uchar	*ap ;

	    bprintf(ofp,"%s:\n",name) ;

	    bprintf(ofp,"\tname=%s\n",cname) ;

	    bprintf(ofp,"\tcname=%s\n",cname) ;

	    if (f_auth)
	        bprintf(ofp,"\tauthenticated=%s\n",
		(as < 0) ? "no" : "yes") ;

/* addresses */

	    hostent_curbegin(&he,&hc) ;

	    while ((rs = hostent_curenumaddr(&he,&hc,&ap)) > 0) {
		in_addr_t	iwa, *iwap ;
	        char		iabuf[IABUFLEN + 1] ;

		inetaddrs at = inetaddr_bin ;
	        if (inetaddr ia ; (rs = inetaddr_start(&ia,at,ap,-1)) >= 0) {

	        if ((rs1 = inetaddr_getdotaddr(&ia,iabuf,IABUFLEN)) >= 0) {
			iwap = (in_addr_t *) ap ;
			iwa = *iwap ;
	            rs = bprintf(ofp,"\taddr=%s (\\x%08x)\n",iabuf,
			iwa) ;
	        } else {
	            rs = bprintf(ofp,"\taddr=*\n") ;
		}

	        inetaddr_finish(&ia) ;
		} /* end if */

		if (rs < 0) break ;
	    } /* end while */

	    hostent_curend(&he,&hc) ;
	    /* alias names */
	    if ((rs = hostent_curbegin(&he,&hc)) >= 0) {
		cchar	*namp{} ;
	        while ((rs = hostent_curenumname(&he,&hc,&namp)) > 0) {
	            rs = bprintf(ofp,"\talias=%s\n",namp) ;
		    if (rs < 0) break ;
	        } /* end while */
	        rs1 = hostent_curend(&he,&hc) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (lookup) */
	    rs1 = uc_free(hebuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (memory-allocation) */

	return rs ;
}
/* end subroutine (printinfo) */



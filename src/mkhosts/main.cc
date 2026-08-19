/* main SUPPORT (mkhosts) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* convert a DNS database (in ASCII) into a 'hosts' table format */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* compile-time */
#define	CF_DEBUG	0		/* run-time */

/* revision history:

	= 1987-09-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1987 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Description:
	This program will read a DNS database (in ASCII) and convert
	it into a 'hosts' type of file.

	Synopsis:
	$ mkhosts [input_file] [-DV] 

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<fcntl.h>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<cstring>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<bfile.h>
#include	<baops.h>
#include	<mallocstuff.h>
#include	<prognamevar.hh>
#include	<exitcodes.h>
#include	<localmisc.h>

#include	"config.h"
#include	"defs.h"


/* local defines */

#define	MAXARGINDEX	100
#define	NARGGROUPS	(MAXARGINDEX/8 + 1)
#define	DEFMAXLINES	66
#define	MAXLINES	180
#define	LINELEN		200
#define	BUFLEN		(MAXPATHLEN + (2 * LINELEN))
#define	DEFPOINT	10


/* external subroutines */

extern int	procfile() ;


/* forward references */

static void	helpfile(cchar *,bfile *) ;


/* local structures */


/* forward references */


/* local variables */

enum argopts {
	argopt_root,
	argopt_debug,
	argopt_version,
	argopt_verbose,
	argopt_help,
	argopt_overlast
} ;

constexpr cpcchar	argopts[] = {
	"ROOT",
	"DEBUG",
	"VERSION",
	"VERBOSE",
	"HELP",
	nullptr
} ;


/* exported variables */

struct global		g ;


/* exported subroutines */

int main(int argc,mainv argv,mainv envv) {
    	prognamevar	progname(argc,argv,envv) ;
	global		*gp = &g ;
	bfile		outfile, *ofp = &outfile ;
	bfile		errfile, *efp = &errfile ;
	int	argr, argl, aol, avl ;
	int	maxai, pan, npa, kwi, i ;
	int	f_optminus, f_optplus, f_optequal ;
	int	rs, bl ;
	int	ex = EX_INFO ;
	int	argnum ;
	int	nhosts ;
	int	f_extra = false ;
	int	f_version = false ;
	int	f_usage = false ;
	int	f_help = false ;

	cchar	*argp, *aop, *avp ;
	char	argpresent[NARGGROUPS] ;
	char	buf[BUFLEN + 1] ;
	cchar	*ifname = nullptr ;
	cchar	*ofname = nullptr ;
	cchar	*cp ;


	g.progname = progname ;
	if (bopen(efp,BFILE_STDERR,"wca",0666) < 0) {
	    bcontrol(efp,BC_LINEBUF,0) ;
	}

	g.efp = efp ;
	g.ofp = ofp ;
	g.debuglevel = 0 ;
	g.programroot = nullptr ;
	g.helpfile = nullptr ;

	g.f.verbose = false ;
	f_help = false ;

/* process program arguments */

	rs = SR_OK ;
	for (i = 0 ; i < NARGGROUPS ; i += 1) argpresent[i] = 0 ;

	npa = 0 ;			/* number of positional so far */
	maxai = 0 ;
	i = 0 ;
	argr = argc - 1 ;
	while ((rs >= 0) && (argr > 0)) {

	    argp = argv[++i] ;
	    argr -= 1 ;
	    argl = strlen(argp) ;

	    f_optminus = (*argp == '-') ;
	    f_optplus = (*argp == '+') ;
	    if ((argl > 0) && (f_optminus || f_optplus)) {
		const int	ach = MKCHAR(argp[1]) ;

	        if (argl > 1) {

	            if (isdigitlatin(ach)) {

	                if (cfdeci(argp + 1,argl - 1,&argnum))
	                    goto badargvalue ;

	            } else {

	                aop = argp + 1 ;
	                aol = argl - 1 ;
	                f_optequal = false ;
	                if ((avp = strchr(aop,'=')) != nullptr) {

	                    aol = avp - aop ;
	                    avp += 1 ;
	                    avl = aop + argl - 1 - avp ;
	                    f_optequal = true ;

	                } else
	                    avl = 0 ;

/* do we have a keyword match or should we assume only key letters? */

	                if ((kwi = matostr(argopts,2,akp,akl)) >= 0) {

	                    switch (kwi) {

/* program root */
	                    case argopt_root:
	                        if (f_optequal) {

	                            f_optequal = false ;
	                            if (avl) g.programroot = avp ;

	                        } else {

	                            if (argr <= 0) goto badargnum ;

	                            argp = argv[++i] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;

	                            if (argl) g.programroot = argp ;

	                        }

	                        break ;

/* debug level */
	                    case argopt_debug:
	                        g.debuglevel = 1 ;
	                        if (f_optequal) {

#if	CF_DEBUGS
	                            debugprintf(
	                                "main: debug flag, avp=\"%W\"\n",
	                                avp,avl) ;
#endif

	                            f_optequal = false ;
	                            if ((avl > 0) &&
	                                (cfdeci(avp,avl,
	                                &g.debuglevel) < 0))
	                                goto badargvalue ;

	                        }

	                        break ;

	                    case argopt_version:
	                        f_version = true ;
	                        break ;

	                    case argopt_verbose:
	                        g.f.verbose = true ;
	                        break ;

/* help file */
	                    case argopt_help:
	                        if (f_optequal) {

	                            f_optequal = false ;
	                            if (avl) g.helpfile = avp ;

	                        }

	                        f_help  = true ;
	                        break ;

	                    } /* end switch (key words) */

	                } else {

	                    while (akl--) {

	                        switch ((int) *akp) {

	                        case 'D':
	                            g.debuglevel = 1 ;
	                            if (f_optequal) {

	                                f_optequal = false ;
	                                rs = cfdeci(avp,avl, &g.debuglevel) ;
					if (rs < 0) goto badargvalue ;

	                            }

	                            break ;

	                        case 'V':
	                            f_version = true ;
	                            break ;

	                        case 'v':
					g.f.verbose = true ;
	                            break ;

	                        case '?':
	                            f_usage = true ;
				    break ;

	                        default:
				    rs = SR_INVALID ;
	                            bprintf(efp,
	                            "%s: invalid option=%c\n",
	                            pip->progname,*akp) ;

	                        } /* end switch */

	                        akp += 1 ;
				if (rs < 0)
				    break ;

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
	                f_extra = true ;
	                bprintf(efp,"%s: extra arguments ignored\n",
	                    g.progname) ;
	            }
	        }

	    } /* end if (key letter/word or positional) */

	} /* end while (all command line argument processing) */


#if	CF_DEBUG
	if (g.debuglevel > 1)
	    debugprintf("main: finished parsing arguments\n") ;
#endif


/* get our program root (if we have one) */

	if (g.programroot == nullptr) {

	    if (g.programroot == nullptr)
	        g.programroot = getenv(VARPROGRAMROOT1) ;

	    if (g.programroot == nullptr)
	        g.programroot = getenv(VARPROGRAMROOT2) ;

	    if (g.programroot == nullptr)
	        g.programroot = PROGRAMROOT ;

	} /* end if */


/* continue w/ the trivia argument processing stuff */

	if (f_version) {

	    bprintf(efp,"%s: version %s\n",
	        g.progname,VERSION) ;

	}

	if (f_usage) goto usage ;

	if (f_version) goto exit ;


	if (f_help) {

	    if (g.helpfile == nullptr) {

	        bl = mkpath2(buf,
	            g.programroot,HELPFILE) ;

	        g.helpfile = (char *) mallocbuf(buf,bl) ;

	    }

	    helpfile(g.helpfile,g.efp) ;

	    goto exit ;

	}


	if (g.debuglevel > 0)
	    bprintf(efp,"%s: debuglevel %d\n",
	        g.progname,g.debuglevel) ;


/* open output file */

	if ((ofname == nullptr) || (ofname[0] == '-'))
		rs = bopen(ofp,BFILE_STDOUT,"dwct",0666) ;

	else
		rs = bopen(ofp,ofname,"wct",0666) ;

	if (rs < 0)
	    goto badoutopen ;


/* processing the input file arguments */

#if	CF_DEBUG
	if (g.debuglevel > 0) debugprintf(
	    "main: checking for positional arguments\n") ;
#endif

	nhosts = 0 ;
	pan = 0 ;
	if (npa > 0) {

	    for (i = 0 ; i <= maxai ; i += 1) {

	        if (BATST(argpresent,i)) {

#if	CF_DEBUG
	            if (g.debuglevel > 0) debugprintf(
	                "main: got a positional argument i=%d pan=%d arg=%s\n",
	                i,pan,argv[i]) ;
#endif


	            rs = procfile(&g,argv[i],pan + 1) ;

	            if (rs < 0) {

	                if (g.f.verbose)
	                    bprintf(g.efp,"%s: error processing file \"%s\"\n",
	                        g.progname,argv[i]) ;

	    } else
		nhosts += rs ;

	            pan += 1 ;

	        } /* end if (got a positional argument) */

	    } /* end for (loading positional arguments) */

	} else {

	    rs = procfile(&g,"-",pan + 1) ;

	    if (rs < 0) {

	        if (g.f.verbose)
	            bprintf(g.efp,"%s: error processing file \"%s\"\n",
	                g.progname,argv[i]) ;

	    } else
		nhosts += rs ;

	    pan += 1 ;

	} /* end if */



/* let's get out of here !! */
done:
	if ((g.debuglevel > 0) || g.f.verbose) {

	    bprintf(efp,"%s: files processed - %d\n",
	        g.progname,pan) ;

	    bprintf(efp,"%s: hosts found - %d\n",
	        g.progname,nhosts) ;

	}

	bclose(ofp) ;


/* close off and get out ! */
exit:
	bclose(efp) ;

	return OK ;

/* what are we about ? */
usage:
	bprintf(efp,
	    "%s: USAGE> %s [infile [...]]\n",
	    g.progname,g.progname) ;

	bprintf(efp,"\t\t[-DV]\n") ;

	bprintf(efp,
	    "\t-D          debugging flag\n") ;

	bprintf(efp,
	    "\t-V          program version\n") ;

	goto badret ;

badargnum:
	bprintf(efp,"%s: not enough arguments specified\n",g.progname) ;

	goto badret ;

badargvalue:
	bprintf(efp,"%s: bad argument value was specified\n",
	    g.progname) ;

	goto badret ;

badinfile:
	bprintf(efp,"%s: could not open input file \"%s\" (rs %d)\n",
	    g.progname,ifname,rs) ;

	goto badret ;

badoutopen:
	bprintf(efp,"%s: could not open output file \"%s\" (rs %d)\n",
	    g.progname,ofname,rs) ;

	goto badret ;

badret:
	bclose(ofp) ;

	bclose(efp) ;

	return BAD ;
}
/* end subroutine (main) */


/* local subroutines */

void helpfile(cchar *f,bfile *ofp) noex {
	bfile	file, *ifp = &file ;
	if ((f == nullptr) || (f[0] == '\0')) return ;
	if (bopen(ifp,f,"r",0666) >= 0) {
	    bcopyblock(ifp,ofp,-1) ;
	    bclose(ifp) ;
	}
}
/* end subroutine (helpfile) */



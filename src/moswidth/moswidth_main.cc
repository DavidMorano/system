/* miswidth_main SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* process MOSFET width specifications within ADVICE connectivity */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0

/* revision history:

	= 1992-03-01, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1992 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Description:
	This program will process width specifications for MOSFETs.
	Specifically, we will look for MOSFET width specifications
	that have an "X" in them and we will convert these to
	straight microns.

	Synopsis:
	$ moswidth [infile] [-i infile] [-o outfile]

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<ctime>
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<cstring>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<bfile.h>
#include	<baops.h>
#include	<char.h>
#include	<localmisc.h>

#include	"config.h"
#include	"defs.h"


/* local defines */

#define		MAXARGINDEX	100
#define		MAXARGGROUPS	(MAXARGINDEX/8 + 1)


/* external subroutines */

extern char	*putheap() ;


/* external variables */


/* local structures */


/* forward references */


/* local variables */

static cchar *argopts[] = {
	    "VERSION",		/* 0 */
	    "TMPDIR",		/* 1 */
	    NULL,
} ;

#define	ARGOPT_VERSION	0
#define	ARGOPT_TMPDIR	1


/* exported variables */


/* exported subroutines */

int main(int argc,cchar **argv,cchar **envv) {
    	prognamevar	progname(argc,argv,envv) ;
	global		g, *gdp = &g ;
	bfile		errfile, *efp = &errfile ;
	bfile		infile, *ifp = &infile ;
	bfile		outfile ;

	int	argr, argl, aol, akl, avl, kwi ;
	int	rs = SR_OK ;
	int	npa, i, ai ;
	int	len ;
	int	maxai ;
	int	l, pn ;
	int	blen ;
	int	f_version = false ;
	int	f_usage = false ;
	int	f_extra = false ;
	int	f_optminus, f_optplus, f_optequal ;
	int	f_bol, f_eol ;
	int	line ;

	cchar	*argp, *aop, *akp, *avp ;
	char	argpresent[MAXARGGROUPS] ;
	char	linebuf[LINELEN + 1] ;
	char	linebuf2[LINELEN + 1] ;
	cchar	*ifname = NULL ;
	cchar	*ofname = NULL ;
	cchar	*cp ;


	g.progname = progname ;
	if (bopen(efp,BERR,"wca",0664) < 0) return BAD ;

	for (i = 0 ; i < MAXARGGROUPS ; i += 1) argpresent[i] = 0 ;

	bcontrol(efp,BC_LINEBUF,0) ;

/* some very initial stuff */

	gdp->efp = efp ;
	gdp->ofp = &outfile ;
	gdp->verboselevel = 1 ;

/* process program arguments */

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

	        if (argl > 1) {

	            aop = argp + 1 ;
	            aol = argl - 1 ;
			akp = aop ;
	            f_optequal = false ;
	            if ((avp = strchr(aop,'=')) != NULL) {

			akl = avp - aop ;
	                aol = avp - aop ;
	                avp += 1 ;
	                avl = aop + argl - 1 - avp ;
	                f_optequal = true ;

	            } else {

			akl = aol ;
	                avl = 0 ;

			}

/* do we have a keyword match or should we assume only key letters? */

	            if ((kwi = matostr(argopts,2,akp,akl)) >= 0) {

	                switch (kwi) {

	                    case ARGOPT_TMPDIR:
	                        if (f_optequal) {

	                            f_optequal = false ;
	                            if (avl) g.tmpdir = avp ;

	                        } else {

	                            if (argr <= 0) goto badargnum ;

	                            argp = argv[++i] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;

	                            if (argl) g.tmpdir = argp ;

	                        }

	                        break ;

	                case ARGOPT_VERSION:
	                    f_version = true ;
	                    if (f_optequal) goto badargextra ;

	                    break ;

/* default action and user specified help */
	                default:
	                    f_usage = true ;
	                    break ;

	                } /* end switch (key words) */

	            } else {

	                while (akl--) {

	                    switch ((int) *akp) {

	                    case 'D':
	                        gdp->debuglevel = 1 ;
	                        if (f_optequal) {

	                            f_optequal = false ;
	                            if (cfdec(avp,avl,&gdp->debuglevel) != OK)
					goto badargvalue ;

	                        }

	                        break ;

	                    case 'V':
	                        f_version = true ;
	                        break ;

/* input file */
	                    case 'i':
	                        if (f_optequal) {

	                            f_optequal = false ;
	                            if (avl) 
					ifname = avp ;

	                        } else {

	                            if (argr <= 0) goto badargnum ;

	                            argp = argv[++i] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;

	                            if (argl)
					ifname = argp ;

	                        }

	                        break ;

/* output file when all subcircuit are combined */
	                    case 'o':
	                        if (f_optequal) {

	                            f_optequal = false ;
	                            if (avl) 
					ofname = avp ;

	                        } else {

	                            if (argr <= 0) goto badargnum ;

	                            argp = argv[++i] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;

	                            if (argl) 
	                        	ofname = argp ;

	                        }

	                        break ;

	                    case '?':
	                        f_usage = true ;
				break ;

	                    default:
				rs = SR_INVALID ;
	                        bprintf(efp,"%s: unknown option - %c\n",
	                            g.progname,*aop) ;

	                    } /* end switch */

	                    akp += 1 ;
			    if (rs < 0)
				break ;

	                } /* end while */

	            } /* end if (individual option key letters) */

	        } /* end if (something in addition to just the sign) */

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

	if (gdp->debuglevel > 0)
	    bprintf(gdp->efp, "%s: debuglevel=%u\n",
		gdp->progname,gdp->debuglevel) ;

	if (f_version)
	    bprintf(efp,"%s: version %s\n",
	        gdp->progname,VERSION) ;

	if (f_usage) goto usage ;

	if (f_version) goto done ;

/* where is the TMP directory */

	if (g.tmpdir == NULL) {

	    g.tmpdir = "/tmp" ;
	    if ((cp = getenv("TMPDIR")) != NULL)
	        g.tmpdir = cp ;

	}

/* initialize some basic stuff */

#if	CF_DEBUG
	if (gdp->debuglevel > 0) debugprintf(
	    "main: about to decide input\n") ;
#endif

	if (ifname == NULL) ifname = BFILE_STDIN ;

	if ((rs = bopen(ifp,ifname,"r",0666)) < 0) goto badopenin ;

#if	CF_DEBUG
	if (gdp->debuglevel > 0) debugprintf(
	    "main: about to check for copy of input\n") ;
#endif

	gdp->ifp = ifp ;

#if	CF_DEBUG
	if (gdp->debuglevel > 0) debugprintf(
	    "main: got input, about to separate\n") ;
#endif

/* go through the loops */

	line = 0 ;
	f_bol = true ;
	offset = 0 ;
	blen = 0 ;
	blockstart = 0 ;
	while ((len = breadln(gdp->ifp,linebuf,LINELEN)) > 0) {

	    linebuf[len] = '\0' ;
	    f_eol = false ;
	    if (linebuf[len - 1] == '\n') f_eol = true ;

		f_continue = false ;
	    if (f_bol) {

#if	CF_DEBUG
	        if (gdp->debuglevel > 1) {

	            debugprintf("main: beginning of line\n") ;

	            debugprintf("main: >%s<\n", gdp->buf) ;

	        }
#endif

/* skip over white space */

	        cp = linebuf ;
		if (*cp == '+') f_continue = true ;

	        while (CHAR_ISWHITE(*cp)) cp += 1 ;

/* scan for a MOSFET specification 'M' */

		if (tolower(*cp) == 'm') f_mos = true ;

	    } /* end if (BOL) */

/* do we have a MOSFET device ? */

		if (f_mos) {

/* scan for a width specification */

	        while (CHAR_ISWHITE(*cp)) cp += 1 ;

		if (strncasecmp(cp,"sw=",3) == 0) {
		/* we finally got one */
		f_width = true ;
		cp2 = cp ;
		cp3 = cp2 + 3 ;
		} /* end if (we got a width specification) */

		} /* end if (we got a MOSFET) */

/* write out */

	if (! f_width) {
		bwrite(gdp->ofp,linebuf,len) ;
	}

	if (f_eol) f_mos = false ;

#if	CF_DEBUG
	    if (gdp->debuglevel > 1)
	        debugprintf("main: bottom of while loop o=%ld\n",
	            offset) ;
#endif

	    offset += len ;
	    blen += len ;
	    if ((len > 0) && f_eol) line += 1 ;

	    f_bol = f_eol ;

	} /* end while (scanning circuits) */

/* end of looping through (cataloging) the stuff */

#if	CF_DEBUG
	if (gdp->debuglevel > 0) bprintf(gdp->efp,
	    "%s: finished scanning circuits, input file len=%ld\n",
	    gdp->progname,offset) ;
#endif

	bclose(ifp) ;

	    bclose(gdp->ofp) ;

/* done */
done:
	bclose(ifp) ;

	    bclose(gdp->ofp) ;

	bclose(efp) ;

	return OK ;

badret:
	bclose(efp) ;

	return BAD ;

usage:
	bprintf(efp,
	    "%s: USAGE> %s [input [output]] ",
	    g.progname,g.progname) ;

	bprintf(efp,
	    "[-i infile] [-o outfile] [-csVD?]\n") ;

	bprintf(efp,"\n") ;

	bprintf(efp,
	    "\t[-TMPDIR temporary_directory] \n") ;

	goto badret ;

badarg:
	bprintf(efp,"%s: bad argument given\n",g.progname) ;

	goto badret ;

badargnum:
	bprintf(efp,"%s: not enough arguments specified\n",g.progname) ;

	goto badret ;

badargvalue:
	bprintf(efp,"%s: bad argument value specified\n",
	    g.progname) ;

	goto badret ;

badargextra:
	bprintf(efp,"%s: cannot have extra argument values for option\n",
	    g.progname) ;

	goto badret ;

badopenin:
	bprintf(efp,"%s: could not open input file (rs %d)\n",
	    g.progname,rs) ;

	goto badret ;

badopenout:
	bprintf(efp,"%s: could not open output file (rs %d)\n",
	    g.progname,rs) ;

	goto badret ;

badtmpmake:
	bprintf(efp,"%s: could not create a temporary file (rs %d)\n",
	    g.progname,rs) ;

	goto badret ;

badtmpopen:
	bprintf(efp,"%s: could not open a temporary file (rs %d)\n",
	    g.progname,rs) ;

	goto badret ;

badwrite:
	bprintf(efp,"%s: bad write (rs %d) - possible full filesystem\n",
	    g.progname,rs) ;

	goto badret ;

badalloc:
	bprintf(efp,"%s: could not allocate a circuit block\n",
	    g.progname) ;

	goto badret ;

badgotcir:
	bprintf(efp,"%s: bad return from 'gotcircuit' (rs %d)\n",
	    g.progname,rs) ;

	goto badret ;

badwritecir:
	bprintf(efp,"%s: bad return from 'writecir' (rs %d)\n",
	    g.progname,rs) ;

	goto badret ;

}
/* end subroutine (main) */



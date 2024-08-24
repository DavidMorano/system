/* main SUPPORT (testbfile) */
/* lang=C++20 */

#define	CF_DEBUGS	1		/* compile-time debugging */
#define	CF_DEBUGMALL	1		/* debugging memory-allocations */
#define	CF_WRITE	0
#define	CF_LITTLE	1		/* do the little output */
#define	CF_ERRSUM	0		/* error summary */


/* revision history:

	= 1988-02-01, David A­D­ Morano
	This subroutine was originally written to do some testing
	on the BIO package.

	= 2015-02-22, David A­D­ Morano
	I am thinking about making some changes to this test.

*/

/* Copyright © 1988,2015 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This is a test program for the BFILE package.

	Synopsis for running:
	$ testbfile.x <file> > outfile

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<cstdarg>
#include	<cstdlib>
#include	<cstdio>
#include	<usystem.h>
#include	<exitcodes.h>
#include	<localmisc.h>

#include	"bfile.h"


/* local defines */

#ifndef	LINEBUFLEN
#define	LINEBUFLEN	1024
#endif

#ifndef	UEBUFLEN
#define	UEBUFLEN	UTMPACCENT_BUFLEN
#endif

#define	VARDEBUGFNAME	"TESTBFILE_DEBUGFILE"


/* external subroutines */

extern int	bufprintf(char *,int,cchar *,...) ;

#if	CF_DEBUGS
extern int	debugopen(cchar *) ;
extern int	debugprintf(cchar *,...) ;
extern int	debugclose() ;
extern int	strlinelen(cchar *,int,int) ;
#endif

extern cchar 	*getourenv(cchar **,cchar *) ;

extern char	*timestr_logz(time_t,char *) ;


/* forward references */


/* exported variables */


/* exported subroutines */

int main(int argc,mainv argv,mainv envv) {
	bfile	ofile, *ofp = &ofile ;
	bfile	ifile, *ifp = &ifile ;

#if	CF_DEBUGS && CF_DEBUGMALL
	uint	mo_start = 0 ;
#endif

	const int	llen = LINEBUFLEN ;

	int	rs = SR_OK ;
	int	pan = 0 ;
	int	i ;
	int	argl, aol ;
	int	ex = EX_USAGE ;
	int	len ;
	int	lines ;
	int	f_usage = FALSE ;

	cchar	*progname ;
	cchar	*argp, *aop ;
	cchar	*ofname = NULL ;
	cchar	*ifname = NULL ;
	cchar	*cp ;

	char		lbuf[LINEBUFLEN + 1] ;


#if	CF_DEBUGS
	{
	    cchar	*cp ;
	    if ((cp = getourenv(envv,VARDEBUGFNAME)) != NULL)
	        debugopen(cp) ;
	    debugprintf("main: starting\n") ;
	}
#endif /* CF_DEBUGS */

#if	CF_DEBUGS && CF_DEBUGMALL
	uc_mallset(1) ;
	uc_mallout(&mo_start) ;
#endif

#if	CF_DEBUGS
	debugprintf("main: ent\n") ;
#endif

	progname = argv[0] ;

	    ofname = BFILE_STDOUT ;

	if ((rs = bopen(ofp,ofname,"wct",0666)) >= 0) {

#if	CF_DEBUGS
	    debugprintf("main: about to open output\n") ;
#endif

	    if ((argc >= 2) && (argv[1][0] != '\0')) {
	        ifname = argv[1] ;
	    } else
	        ifname = BFILE_STDIN ;

	    if ((rs = bopen(ifp,ifname,"r",0666)) >= 0) {
		off_t	loff = 0 ;
	        int		ll ;
		cchar	*lp = lbuf ;

	        while ((rs = breadln(ifp,lbuf,llen)) > 0) {
	            len = rs ;
	            lines += 1 ;

		    ll = len ;
		    if (ll && (lbuf[ll-1] == '\n')) ll -= 1 ;

#if	CF_DEBUGS
	            debugprintf("main: l=>%t<\n",
			lp,strlinelen(lp,ll,50)) ;
	            debugprintf("main: before write\n") ;
#endif

	            rs = bwrite(ofp,lbuf,len) ;

#if	CF_DEBUGS
	            debugprintf("main: bwrite() rs=%d\n",rs) ;
#endif

		    loff += len ;
	            if (rs < 0) break ;
	        } /* end while (reading lines) */

#if	CF_DEBUGS
	        debugprintf("main: while-out rs=%d\n",rs) ;
#endif

	        bclose(ifp) ;
	    } /* end if (ifile) */

	    bclose(ofp) ;
	} /* end if (ofile) */

	ex = (rs >= 0) ? EX_OK : EX_SOFTWARE ;

#if	CF_DEBUGS
	debugprintf("main: all-out rs=%d\n",rs) ;
#endif

#if	CF_DEBUGS && CF_DEBUGMALL
	{
	    uint	mo ;
	    uc_mallout(&mo) ;
	    debugprintf("main: final mallout=%u\n",(mo-mo_start)) ;
	    uc_mallset(0) ;
	}
#endif

#if	CF_DEBUGS
	debugclose() ;
#endif

	return ex ;
}
/* end subroutine (main) */



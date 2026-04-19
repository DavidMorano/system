/* testfindinline */
/* charset=ISO8859-1 */
/* lang=C89 */

#define	CF_DEBUGS	1		/* compile-time debugging */
#define	CF_DEBUGMALL	1		/* debug memory-allocations */

#include	<envstandards.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<getx.h>
#include	<bfile.h>
#include	<localmisc.h>

#include	"findinline.h"

#define	VARDEBUGFNAME	"TESTFINDINLINE_DEBUGFILE"

#if	CF_DEBUGS
extern int	debugopen(cchar *) ;
extern int	debugprintf(cchar *,...) ;
extern int	debugclose() ;
extern int	strlinelen(cchar *,int,int) ;
#endif


int main(int argc,cchar **argv,cchar **envv) noex {
	FINDINLINE	fi ;

	bfile	ofile, *ofp = &ofile ;
	bfile	ifile, *ifp = &ifile ;

	cchar	*ofname = NULL ;
	cchar	*ifname = NULL ;
	cchar	*sp ;

#if	CF_DEBUGS && CF_DEBUGMALL
	uint	mo_start = 0 ;
#endif

	int	rs ;
	int	rs1 ;
	int	sl ;

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

	if ((ofname == NULL) || (ofname[0] == '\0') || (ofname[0] == '-'))
		ofname = BFILE_STDOUT ;

	if ((rs = bopen(ofp,ofname,"wct",0666)) >= 0) {

#ifdef	COMMENT
	if (argv != NULL) {
	    int	ai ;
	    for (ai = 1 ; (ai < argc) && (argv[ai] != NULL) ; ai += 1) {


#if	CF_DEBUGS
	        debugprintf("main: findinline() rs=%d\n",rs) ;
#endif

	    } /* end for */
	} /* end if (arguments) */
#endif /* COMMENT */

	if ((ifname == NULL) || (ifname[0] == '\0') || (ifname[0] == '-'))
		ifname = BFILE_STDIN ;

	if ((rs = bopen(ifp,ifname,"r",0666)) >= 0) {
	    FINDINLINE	fi ;
	    cint	llen = LINEBUFLEN ;
	    int		len ;
	    cchar	*fmt ;
	    char	lbuf[LINEBUFLEN+1] ;
	    while ((rs = breadln(ifp,lbuf,llen)) > 0) {
		len = rs ;

		bprintln(ofp,lbuf,len) ;

		if ((sl = findinline(&fi,lbuf,len)) > 0) {
		    fmt = "» sl=%u k=%r v=>%r<\n" ;
		    bprintf(ofp,fmt,sl,fi.kp,fi.kl,fi.vp,fi.vl) ;
		}

#if	CF_DEBUGS
	        debugprintf("main: findinline() rs=%d\n",sl) ;
#endif

	    } /* end while */
	    bclose(ifp) ;
	} /* end if (infile-open) */

	    bclose(ofp) ;
	} /* end if (outfile-open) */

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

	return 0 ;
}
/* end subroutine (main) */



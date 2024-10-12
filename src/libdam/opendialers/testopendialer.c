/* testopendialer */
/* lang=C89 */

#define	CF_DEBUGS	1		/* compile-time debugging */
#define	CF_DEBUGMALL	1		/* debugging memory-allocations */
#include	<envstandards.h>
#include	<stdio.h>
#include	<usystem.h>
#include	<filer.h>
#include	<localmisc.h>

#ifndef	UEBUFLEN
#define	UEBUFLEN	UTMPACCENT_BUFLEN
#endif

#ifndef FILER_RCNET
#define	FILER_RCNET	4		/* read-count for network */
#endif

#ifndef	TIMEBUFLEN
#define	TIMEBUFLEN	80
#endif

#define	VARDEBUGFNAME	"TESTOPENDIALER_DEBUGFILE"

#if	CF_DEBUGS
extern int	debugopen(const char *) ;
extern int	debugprintf(const char *,...) ;
extern int	debugclose() ;
extern int	strlinelen(const char *,int,int) ;
#endif

extern const char 	*getourenv(const char **,const char *) ;

extern char	*timestr_logz(time_t,char *) ;

/* forward references */


/* exported subroutines */


int main(int argc,const char **argv,const char **envv)
{

#if	CF_DEBUGS && CF_DEBUGMALL
	uint	mo_start = 0 ;
#endif

	int	rs = SR_OK ;
	int	rs1 ;


#if	CF_DEBUGS
	{
	    const char	*cp ;
	    if ((cp = getourenv(envv,VARDEBUGFNAME)) != NULL)
	        debugopen(cp) ;
	    debugprintf("main: starting\n") ;
	}
#endif /* CF_DEBUGS */

#if	CF_DEBUGS && CF_DEBUGMALL
	uc_mallset(1) ;
	uc_mallout(&mo_start) ;
#endif

	if (argv != NULL) {
	    cint	llen = LINEBUFLEN ;
	    int		ai ;
	    char	lbuf[LINEBUFLEN+1] ;
	    for (ai = 1 ; (ai < argc) && (argv[ai] != NULL) ; ai += 1) {
	        cchar	*fn = argv[ai] ;
	        cint	of = O_RDONLY ;
	        if ((rs = uc_open(fn,of,0666)) >= 0) {
	            cint	fo = (of | O_NETWORK) ;
	            int		fd = rs ;
	            if (filer b ; (rs = filer_start(&b,fd,0z,0,fo)) >= 0) {
	                cint	to = 5 ;
	                while ((rs = filer_read(&b,lbuf,llen,to)) > 0) {
	                    int	len = rs ;
	                    fbwrite(stdout,lbuf,len) ;
	                    if (rs < 0) break ;
	                } /* end while */
	                rs1 = filer_finish(&b) ;
			if (rs >= 0) rs = rs1 ;
	            } /* end if (filer) */
	            rs1 = u_close(fd) ;
		    if (rs >= 0) rs = rs1 ;
	        } else {
	            fbprintf(stdout,"not_found fn=%s (%d)\n",fn,rs) ;
	        }
	        if (rs < 0) break ;
	    } /* end for */
	} /* end if (arguments) */

#if	CF_DEBUGS
	debugprintf("main: out rs=%d\n",rs) ;
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

	return 0 ;
}
/* end subroutine (main) */


/* local subroutines */



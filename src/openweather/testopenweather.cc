/* testopenweather */
/* lang=C89 */

#define	CF_DEBUGS	1		/* compile-time debugging */
#define	CF_DEBUGMALL	1		/* debugging memory-allocations */
#include	<envstandards.h>
#include	<sys/types.h>
#include	<cstdarg>
#include	<cstdio>
#include	<usystem.h>
#include	<fsdir.h>
#include	<pcsopendircache.h>
#include	<filer.h>
#include	<localmisc.h>

#ifndef	VARPRLOCAL
#define	VARPRLOCAL	"LOCAL"
#endif

#ifndef	VARPRPCS
#define	VARPRPCS	"PCS"
#endif

#ifndef	UEBUFLEN
#define	UEBUFLEN	UTMPACCENT_BUFLEN
#endif

#ifndef FILER_RCNET
#define	FILER_RCNET	4		/* read-count for network */
#endif

#ifndef	TIMEBUFLEN
#define	TIMEBUFLEN	80
#endif

#define	VARDEBUGFNAME	"TESTOPENWEATHER_DEBUGFILE"

/* external subroutines */

#if	CF_DEBUGS
extern int	debugopen(const char *) ;
extern int	debugprintf(const char *,...) ;
extern int	debugclose() ;
extern int	strlinelen(const char *,int,int) ;
#endif


/* forward references */

static int procweather(int,int) ;


/* exported variables */


/* exported subroutines */

int main(int argc,const char **argv,const char **envv)
{
	const char	*pr = getourenv(envv,VARPRLOCAL) ;

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

#if	CF_DEBUGS
	        debugprintf("main: pr=%s\n",pr) ;
#endif

	if (argv != NULL) {
	    const mode_t	om = 0666 ;
	    const int	of = O_RDONLY ;
	    const int	to = -1 ;
	    const int	llen = LINEBUFLEN ;
	    int		ai ;
	    char	lbuf[LINEBUFLEN+1] ;
	    for (ai = 1 ; (ai < argc) && (argv[ai] != NULL) ; ai += 1) {
	        const char	*ws = argv[ai] ;
#if	CF_DEBUGS
	        debugprintf("main: ws=%s\n",ws) ;
#endif
	        if ((rs1 = openweather(pr,ws,of,to)) >= 0) {
	            ustat	sb ;
	            int		fd = rs1 ;
#if	CF_DEBUGS
	            debugprintf("main: openweather() rs=%d\n",rs1) ;
#endif
	            if ((rs = u_fstat(fd,&sb)) >= 0) {
#if	CF_DEBUGS
	            debugprintf("main: mode=\\x%08x\n",sb.st_mode) ;
#endif

			rs = procweather(fd,of) ;
		    }
	            
	            u_close(fd) ;
	        } else if (rs1 == SR_NOTFOUND) {
	            rs = SR_OK ;
	            printf("not_found ws=%s (%d)\n",ws,rs1) ;
	        }
#if	CF_DEBUGS
	        debugprintf("main: openweather-out rs=%d\n",rs1) ;
#endif
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

static int procweather(int fd,int of) noex {
	cint		fo = (of | O_NETWORK) ;
	int		rs ;
	if (filer b ; (rs = filer_start(&b,fd,0z,0,fo)) >= 0) {
	    cint	to = 0 ;
	    cint	llen = LINEBUFLEN ;
	    int		li ;
	    char	lbuf[LINEBUFLEN+1] ;
	    while ((rs = filer_readln(&b,lbuf,llen,to)) > 0) {
	        int	len = rs ;
#if	CF_DEBUGS
	        debugprintf("main/dumpfile: readline() len=%d\n",len) ;
#endif
	        li = strlinelen(lbuf,len,70) ;
	        lbuf[li] = '\0' ;
	        printf("l=>%s<\n",lbuf) ;
	        if (rs < 0) break ;
	    } /* end while */
	    filer_finish(&b) ;
	} /* end if (filer) */

#if	CF_DEBUGS
	debugprintf("main/dumpfile: rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (procweather) */



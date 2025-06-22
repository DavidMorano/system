/* testopendircache */
/* lang=C89 */


/* revision history:

	= 2017-09-07, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2017 David A­D­ Morano.  All rights reserved. */


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

#define	VARDEBUGFNAME	"TESTOPENDIRCACHE_DEBUGFILE"

extern int	bufprintf(char *,int,const char *,...) ;

#if	CF_DEBUGS
extern int	debugopen(const char *) ;
extern int	debugprintf(const char *,...) ;
extern int	debugclose() ;
extern int	strlinelen(const char *,int,int) ;
#endif

extern const char 	*getourenv(const char **,const char *) ;

extern char	*timestr_logz(time_t,char *) ;

/* forward references */

static int dumpfile(int,int) ;
static int dumpdir(int,int) ;

/* exported subroutines */

int main(int argc,const char **argv,const char **envv)
{
	const char	*pr = getourenv(envv,VARPRPCS) ;

#if	CF_DEBUGS && CF_DEBUGMALL
	uint		mo_start = 0 ;
#endif

	int		rs = SR_OK ;
	int		rs1 ;

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
	    const mode_t	om = 0666 ;
	    cint	to = -1 ;
	    cint	llen = LINEBUFLEN ;
	    int		ai ;
	    char	lbuf[LINEBUFLEN+1] ;
	    for (ai = 1 ; (ai < argc) && (argv[ai] != NULL) ; ai += 1) {
	        const char	*fn = argv[ai] ;
	        cint	of = O_RDONLY ;
#if	CF_DEBUGS
	        debugprintf("main: fn=%s\n",fn) ;
#endif
	        if ((rs1 = pcsopendircache(pr,fn,of,om,to)) >= 0) {
	            ustat	sb ;
	            int		fd = rs1 ;
#if	CF_DEBUGS
	            debugprintf("main: pcsopendircache() rs=%d\n",rs1) ;
#endif
	            if ((rs = u_fstat(fd,&sb)) >= 0) {
#if	CF_DEBUGS
	            debugprintf("main: mode=\\x%08x\n",sb.st_mode) ;
#endif
	                if (S_ISDIR(sb.st_mode)) {
	                    rs = dumpdir(fd,of) ;
	                } else {
	                    rs = dumpfile(fd,of) ;
	                }
	            }
	            u_close(fd) ;
	        } else if (rs1 == SR_NOTFOUND) {
	            rs = SR_OK ;
	            printf("not_found fn=%s (%d)\n",fn,rs1) ;
	        }
#if	CF_DEBUGS
	        debugprintf("main: uc_open-out rs=%d\n",rs1) ;
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


static int dumpfile(int fd,int of)
{
	FILER		b ;
	cint	fo = (of | O_NETWORK) ;
	int		rs ;
#if	CF_DEBUGS
	debugprintf("main/dumpfile: ent\n") ;
#endif
	if ((rs = filer_start(&b,fd,0z,0,fo)) >= 0) {
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
/* end subroutine (dumpfile) */


static int dumpdir(int fd,int of)
{
	FSDIR		d ;
	FSDIR_ENT	de ;
	cint	dlen = MAXPATHLEN ;
	int		rs ;
	char		dbuf[USERNAMELEN+1] ;

#if	CF_DEBUGS
	debugprintf("main/dumpdir: ent\n") ;
#endif
	if ((rs = bufprintf(dbuf,dlen,"/dev/fd/%u",fd)) >= 0) {
	    if ((rs = fsdir_open(&d,dbuf)) >= 0) {
	        while ((rs = fsdir_read(&d,&de)) > 0) {
	            printf("e=%s\n",de.name) ;
	        } /* end while */
	        fsdir_close(&d) ;
	    } /* end if (fsdir) */
	} /* end if */

#if	CF_DEBUGS
	debugprintf("main/dumpdir: ret rs=%d\n",rs) ;
#endif
	return rs ;
}
/* end subroutine (dumpdir) */



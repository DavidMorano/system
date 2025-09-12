/* testmaintqotd */
/* lang=C89 */

#define	CF_DEBUGS	1		/* compile-time debugging */
#define	CF_DEBUGMALL	1		/* debugging memory-allocations */
#include	<envstandards.h>
#include	<sys/types.h>
#include	<cstdarg>
#include	<cstdio>
#include	<tzfile.h>		/* for TM_YEAR_BASE */

#include	<usystem.h>
#include	<fsdir.h>
#include	<filer.h>
#include	<tmtime.hh>
#include	<dayspec.h>
#include	<localmisc.h>

#include	"maintqotd.h"

#ifndef FILER_RCNET
#define	FILER_RCNET	4		/* read-count for network */
#endif

#ifndef	TIMEBUFLEN
#define	TIMEBUFLEN	80
#endif

#define	VARDEBUGFNAME	"TESTMAINTQOTD_DEBUGFILE"

extern int	getmjd(int,int,int) ;
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

static int curdate(DAYSPEC *,int) ;
static int defspec(DAYSPEC *,DAYSPEC *) ;
static int cvtdate(DAYSPEC *,cchar *) ;

static int dumpfile(int,int) ;
static int dumpdir(int,int) ;

#ifdef	COMMENT
static int filer_oread(FILER *,void *,int,int) ;
static int filer_refill(FILER *,int) ;
#endif /* COMMENT */

/* exported subroutines */

int main(int argc,cchar **argv,cchar **envv)
{
	DAYSPEC		defs ;

#if	CF_DEBUGS && CF_DEBUGMALL
	uint	mo_start = 0 ;
#endif

	const int	y = 2015 ;

	int	rs = SR_OK ;
	int	rs1 ;
	int	mjd ;

	cchar	*pr = "/usr/add-on/local" ;

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


	rs = curdate(&defs,y) ;
	if (rs < 0) goto done ;


	if (argv != NULL) {
	    const int	llen = LINEBUFLEN ;
	    int		ai ;
	    char	lbuf[LINEBUFLEN+1] ;
	    for (ai = 1 ; (ai < argc) && (argv[ai] != NULL) ; ai += 1) {
	        cchar	*qp = argv[ai] ;
	        const int	of = O_RDONLY ;
#if	CF_DEBUGS
	        debugprintf("main: qp=%s\n",qp) ;
#endif
	        if ((rs = cvtdate(&defs,qp)) >= 0) {
	            const int	to = -1 ;
	            int		mjd = rs ;
	            if ((rs1 = maintqotd(pr,mjd,of,to)) >= 0) {
	                ustat	sb ;
	                int		fd = rs1 ;
#if	CF_DEBUGS
	                debugprintf("main: maintqotd() rs=%d\n",rs1) ;
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
	                printf("not_found qp=%s (%d)\n",qp,rs1) ;
	            }
#if	CF_DEBUGS
	            debugprintf("main: maintqotd-out rs=%d\n",rs1) ;
#endif
	        } /* end if (cvtdate) */
	        if (rs < 0) break ;
	    } /* end for */
	} /* end if (arguments) */

done:

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

static int dumpfile(int fd,int of) noex {
	cint		fo = (of | O_NETWORK) ;
	int		rs ;
	if (filer b ; (rs = filer_start(&b,fd,0z,0,fo)) >= 0) {
	    cint	to = 0 ;
	    cint	llen = LINEBUFLEN ;
	    char	lbuf[LINEBUFLEN+1] ;
	    while ((rs = filer_readln(&b,lbuf,llen,to)) > 0) {
	        int	len = rs ;
	        fbwrite(stdout,lbuf,len) ;
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
	const int	dlen = MAXPATHLEN ;
	int	rs ;

	char	dbuf[USERNAMELEN+1] ;

#if	CF_DEBUGS
	debugprintf("main/dumpdir: entered\n") ;
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


#ifdef	COMMENT

static int filer_oread(op,rbuf,rlen,to)
FILER		*op ;
void		*rbuf ;
int		rlen ;
int		to ;
{
	int	rs = SR_OK ;
	int	mlen ;
	int	rc ;
	int	tlen = 0 ;
	int	f_timedout = FALSE ;

	register char	*dbp = (char *) rbuf ;
	register char	*bp, *lastp ;

	if (op == NULL) return SR_FAULT ;

#if	CF_DEBUGS
	debugprintf("filer_oread: rlen=%d to=%d\n",rlen,to) ;
#endif

	rc = (op->fl.net) ? FILER_RCNET : 1 ;
	while (tlen < rlen) {

#if	CF_DEBUGS
	    debugprintf("filer_oread: 0 while-top tlen=%d op->len=%d\n", 
	        tlen,op->len) ;
#endif
	    if (op->len <= 0) {
	        rs = filer_refill(op,to) ;
	        if ((rs == SR_TIMEDOUT) && (tlen > 0)) {
	            rs = SR_OK ;
	            f_timedout = TRUE ;
	        }
	    }

#if	CF_DEBUGS
	    debugprintf("filer_oread: refilled rs=%d f_to=%u\n",
	        rs,f_timedout) ;
	    debugprintf("filer_oread: op->len=%d tlen=%d\n", op->len,tlen) ;
#endif

	    if ((op->len == 0) || f_timedout)
	        break ;

	    mlen = MIN(op->len,(rlen - tlen)) ;

	    bp = op->bp ;
	    lastp = op->bp + mlen ;
	    while (bp < lastp)
	        *dbp++ = *bp++ ;

	    op->bp += mlen ;
	    tlen += mlen ;
	    op->len -= mlen ;

	} /* end while */

	if (rs >= 0)
	    op->off += tlen ;

	if (rs == SR_TIMEDOUT) {
	    char	tbuf[10+1] = { 0 } ;
	    int	i ;
	    rs = u_write(op->fd,tbuf,0) ;
#if	CF_DEBUGS
	    debugprintf("filer_oread: timed-out? rs=%d\n",rs) ;
#endif
	    for (i = 0 ; i < 4 ; i += 1) {
	        rs = u_read(op->fd,tbuf,10) ;
#if	CF_DEBUGS
	        debugprintf("filer_oread: u_read() rs=%d\n",rs) ;
#endif
	    }
	} /* end if (timed-out) */

#if	CF_DEBUGS
	debugprintf("filer_oread: ret rs=%d tlen=%u\n",rs,tlen) ;
#endif

	return (rs >= 0) ? tlen : rs ;
}
/* end subroutine (filer_read) */


static int filer_refill(FILER *op,int to)
{
	const int	fmo = FM_TIMED ;
	int	rs = SR_OK ;
	int	rc = 4 ;
	int	tlen = 0 ;

	while ((op->len <= 0) && (rc-- > 0)) {

#if	CF_DEBUGS
	    debugprintf("filer_refill: 1 while-top tlen=%d len=%d rc=%d\n",
	        tlen,op->len,rc) ;
	    debugprintf("filer_refill: reading=%d to=%d\n",op->bufsize,to) ;
#endif
	    op->bp = op->buf ;
	    if (to >= 0) {
	        rs = uc_reade(op->fd,op->buf,op->bufsize,to,fmo) ;
	    } else
	        rs = u_read(op->fd,op->buf,op->bufsize) ;

#if	CF_DEBUGS
	    debugprintf("filer_refill: read rs=%d\n",rs) ;
#endif

	    if ((rs == SR_TIMEDOUT) && (tlen > 0)) {
	        rs = SR_OK ;
	        break ;
	    } else if (rs == 0) {
	        to = 0 ;
	    }

	    if (rs < 0) break ;
	    op->len = rs ;
	    tlen += rs ;
	} /* end while (refill) */

#if	CF_DEBUGS
	debugprintf("filer_refill: ret rs=%d tlen=%d\n",rs,tlen) ;
#endif
	return (rs >= 0) ? tlen : rs ;
}
/* end subroutine (filer_refill) */

#endif /* COMMENT */


static int curdate(DAYSPEC *dsp,int y)
{
	TMTIME	ct ;
	time_t	dt = time(NULL) ;
	int	rs ;
	rs = tmtime_localtime(&ct,dt) ;
	dsp->y = (y > 0) ? y : (ct.year + TM_YEAR_BASE) ;
	dsp->m = ct.mon ;
	dsp->d = ct.mday ;
	return rs ;
}
/* end subroutine (curdate) */


static int defspec(DAYSPEC *ddsp,DAYSPEC *dsp)
{
	int	rs = SR_OK ;

	if (dsp->y < 0) dsp->y = ddsp->y ;
	if (dsp->m < 0) dsp->m = ddsp->m ;
	if (dsp->d < 0) dsp->d = ddsp->d ;

	return rs ;
}
/* end subroutine (defspec) */

static int cvtdate(DAYSPEC *ddsp,cchar *qp) noex {
	DAYSPEC	ds ;
	int	rs = SR_OK ;
	int	mjd = 0 ;
	if ((qp[0] == '+') || (qp[0] == '-')) {
	    rs = dayspec_def(&ds) ;
	} else {
	    rs = dayspec_load(&ds,qp,-1) ;
	}
	if (rs >= 0) {
	    if ((rs = defspec(ddsp,&ds)) >= 0) {
	        rs = getmjd(ds.y,ds.m,ds.d) ;
	        mjd = rs ;
	    }
	}
	return (rs >= 0) ? mjd : rs ;
}
/* end subroutine (cvtdate) */



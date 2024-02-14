/* process */

/* send the message out to a host */


#define	CF_DEBUG	0		/* run-time debug print-outs */
#define	CF_TESTPREV	0		/* test small previous-buffer */


/* revision history:

	= 1996-03-01, David A�D� Morano

	The subroutine was adapted from others programs that
	did similar types of functions.


*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/******************************************************************************

	This subroutine sends the message out to a host.


******************************************************************************/


#include	<envstandards.h>

#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<sys/mman.h>
#include	<limits.h>
#include	<unistd.h>
#include	<signal.h>
#include	<stdlib.h>
#include	<string.h>
#include	<ctype.h>

#include	<usystem.h>
#include	<bfile.h>
#include	<vecobj.h>
#include	<paramopt.h>
#include	<localmisc.h>

#include	"config.h"
#include	"defs.h"


/* local defines */

#define	TMPFX		"liXXXXXXXXXXXX"
#define	NPAGESMAP	32

#if	CF_TESTPREV
#undef	LINEBUFLEN
#define	LINEBUFLEN	6
#endif


/* external subroutines */

extern int	mkpath2(char *,const char *,const char *) ;
extern int	isprintlatin(int) ;
extern int	opentmpfile(const char *,int,mode_t,char *) ;


/* external variables */


/* local structures */

struct fblock {
	off_t	start ;
	int		len ;
} ;


/* forward references */

ulong	ufloorl(ulong,ulong) ;
ulong	uceill(ulong,ulong) ;


/* local variables */


/* exported subroutines */


int process(pip,app,ofp,fname)
struct proginfo	*pip ;
PARAMOPT	*app ;
bfile		*ofp ;
const char	fname[] ;
{
	struct ustat	sb ;

	off_t	moff ;

	bfile	tmpfile ;

	vecobj	fblocks ;

	ulong	bstart ;
	ulong	fsize ;
	ulong	bsize ;
	ulong	osize ;

	int	rs = SR_OK, rs1 ;
	int	fd ;
	int	fd_tmp ;
	int	size ;
	int	wlen = 0 ;
	int	scanlen ;
	int	mn, mi, i, cb ;
	int	pl, ll, sl, cl ;

	char	template[MAXPATHLEN + 1] ;
	char	tmpfname[MAXPATHLEN + 1] ;
	char	prevbuf[LINEBUFLEN + 1] ;
	char	*buf, *copybuf = NULL ;
	char	*pp, *lp, *sp, *cp, *ep ;


	if (fname == NULL)
	    return SR_FAULT ;

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    debugprintf("process: entered fname=%s\n",fname) ;
#endif

	size = sizeof(struct fblock) ;
	rs = vecobj_start(&fblocks,size,0,0) ;

	if (rs < 0)
	    goto ret0 ;

	if (strcmp(fname,"-") != 0)
	    rs = u_open(fname,O_RDONLY,0666) ;

	else
	    rs = u_dup(FD_STDIN) ;

	fd = rs ;
	if (rs < 0)
	    goto badinopen ;

	rs = u_fstat(fd,&sb) ;

	if (rs < 0)
	    goto ret2 ;

	fsize = sb.st_size ;
	tmpfname[0] = '\0' ;
	if (! S_ISREG(sb.st_mode)) {

	    mkpath2(template,pip->tmpdname,TMPFX) ;

	    rs = opentmpfile(template,O_RDWR,0660,tmpfname) ;

	    fd_tmp = rs ;
	    u_unlink(tmpfname) ;

	    if (rs < 0) {

	        bprintf(pip->efp,
	            "%s: could create a temporary file (%d)\n",
	            pip->progname,rs) ;

	        goto ret2 ;
	    }

	    rs = uc_copy(fd,fd_tmp,-1) ;

	    if (rs < 0)
	        goto ret2 ;

	    u_close(fd) ;

	    fd = fd_tmp ;

	} /* end if (creating temporary file) */

/* go through the loops */

	cb = 0 ;
	bsize = NPAGESMAP * pip->pagesize ;
	osize = uceill(fsize,bsize) ;

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    debugprintf("process: mapping bsize=%lu osize=%lu\n",
	        bsize,osize) ;
#endif

	mn = osize / bsize ;
	bstart = ufloorl(fsize,bsize) ;

	if (bstart == fsize)
	    bstart -= bsize ;

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    debugprintf("process: mn=%u bstart=%lu\n",
	        mn,bstart) ;
#endif

	pp = prevbuf ;
	pl = 0 ;
	for (mi = 0 ; mi < mn ; mi += 1) {

#if	CF_DEBUG
	    if (DEBUGLEVEL(2))
	        debugprintf("process: mapping mi=%u\n",mi) ;
#endif

	    moff = bstart ;
	    rs = u_mmap(NULL,(size_t) bsize, PROT_READ,MAP_SHARED,
		fd,moff,&buf) ;

#if	CF_DEBUG
	    if (DEBUGLEVEL(2))
	        debugprintf("process: u_mmap() rs=%d\n",rs) ;
#endif

	    if (rs < 0)
	        break ;

	    scanlen = MIN((fsize - bstart),bsize) ;

#if	CF_DEBUG
	    if (DEBUGLEVEL(2))
	        debugprintf("process: scanlen=%u\n",scanlen) ;
#endif

	    ep = (buf + scanlen) ;
	    for (cp = (ep - 1) ; cp >= buf ; cp -= 1) {

#if	CF_DEBUG
	        if (DEBUGLEVEL(2))
	            debugprintf("process: cp=\\%02X %c\n",
	                *cp, ((isprintlatin(*cp)) ? *cp : ' ')) ;
#endif

	        if (*cp == '\n') {

#if	CF_DEBUG
	            if (DEBUGLEVEL(2))
	                debugprintf("process: got NL\n") ;
#endif

	            pip->f.lastnl = TRUE ;
	            lp = (cp + 1) ;
	            ll = (ep - lp) ;

#if	CF_DEBUG
	            if (DEBUGLEVEL(2))
	                debugprintf("process: ll=%u\n",ll) ;
#endif

	            if (ll > 0) {

	                rs = bwrite(ofp,lp,ll) ;

	                wlen += rs ;
	                pip->f.lastnl = (lp[ll - 1] == '\n') ;
	            }

	            if ((rs >= 0) && (cb > 0)) {

	                cb = 0 ;
	                rs = copyout(pip,ofp,&fblocks,
	                    fd,&copybuf) ;

	                wlen += rs ;
	                ll = 0 ;
	            }

	            if ((rs >= 0) && (pl > 0)) {

#if	CF_DEBUG
	                if (DEBUGLEVEL(2))
	                    debugprintf("process: previous buf pl=%u\n",pl) ;
#endif

	                lp = pp ;
	                ll = pl ;
	                rs = bwrite(ofp,lp,ll) ;

	                wlen += rs ;
	                pip->f.lastnl = (lp[ll - 1] == '\n') ;
	                pl = 0 ;

	            } /* end if (outputting previous buffer) */

	            if ((rs >= 0) && (! pip->f.lastnl)) {

#if	CF_DEBUG
	                if (DEBUGLEVEL(2))
	                    debugprintf("process: outputting extra NL\n") ;
#endif

	                pip->f.lastnl = FALSE ;
	                rs = bputc(ofp,'\n') ;

	                wlen += 1 ;
	            }

	            ep = (cp + 1) ;

#if	CF_DEBUG
	            if (DEBUGLEVEL(2))
	                debugprintf("process: new ep=\\%02X %c\n",
	                    *ep, ((isprintlatin(*ep)) ? *ep : ' ')) ;
#endif

	        } /* end if (found NL) */

	    } /* end for (scanning buffer) */

#if	CF_DEBUG
	    if (DEBUGLEVEL(2))
	        debugprintf("process: out-of-loop ep=%p cp=%p\n",ep,cp) ;
#endif

	    if ((rs >= 0) && ((ep - buf) >= 0)) {

#if	CF_DEBUG
	        if (DEBUGLEVEL(2))
	            debugprintf("process: got left over\n") ;
#endif

	        if ((pl == 0) && ((ep - buf) <= LINEBUFLEN)) {

	            pp = prevbuf ;
	            pl = (ep - buf) ;
	            memcpy(pp,buf,pl) ;

#if	CF_DEBUG
	            if (DEBUGLEVEL(2))
	                debugprintf("process: saving left over >%t<\n",pp,pl) ;
#endif

	        } else {

	            struct fblock	b ;


	            b.start = bstart ;
	            b.len = (ep - buf) ;
	            cb += 1 ;
	            rs = vecobj_add(&fblocks,&b) ;

	        }

	    }

	    rs1 = u_munmap(buf,(size_t) bsize) ;

#if	CF_DEBUG
	    if (DEBUGLEVEL(2))
	        debugprintf("process: u_munmap() rs=%d\n",rs) ;
#endif

	    if ((rs == 0) && (rs1 < 0))
	        rs = rs1 ;

	    if (rs < 0)
	        break ;

	    bstart -= bsize ;

#if	CF_DEBUG
	    if (DEBUGLEVEL(2))
	        debugprintf("process: new bstart=%lu\n",bstart) ;
#endif

	} /* end for (looping through file) */

	if ((rs >= 0) && (cb > 0)) {

	    cb = 0 ;
	    rs = copyout(pip,ofp,&fblocks,
	        fd,&copybuf) ;

	    wlen += rs ;
	    ll = 0 ;
	}

	if ((rs >= 0) && (pl > 0)) {

#if	CF_DEBUG
	    if (DEBUGLEVEL(2))
	        debugprintf("process: final left over pl=%u\n",pl) ;
#endif

	    lp = pp ;
	    ll = pl ;
	    rs = bwrite(ofp,lp,ll) ;

	    wlen += 1 ;
	    pip->f.lastnl = (lp[ll - 1] == '\n') ;

	} /* end if (previous buffer) */

	if ((rs >= 0) && (! pip->f.lastnl)) {

	    pip->f.lastnl = FALSE ;
	    rs = bputc(ofp,'\n') ;

	    wlen += 1 ;

	} /* end if (last NL) */

/* uninitialize and get out */

	if (copybuf != NULL)
	    free(copybuf) ;

ret3:
	if (tmpfname[0] != '\0')
	    u_unlink(tmpfname) ;

ret2:
	u_close(fd) ;

ret1:
	vecobj_finish(&fblocks) ;

ret0:
	return (rs >= 0) ? wlen : rs ;

/* bad stuff */
badinopen:
	bprintf(pip->efp,
	    "%s: could not open file (%d)\n",
	    pip->progname,rs) ;

	cp = (char *) fname ;
	if (strcmp(fname,"-") == 0)
	    cp = "*STDIN*" ;

	bprintf(pip->efp,
	    "%s: file=%s\n",
	    pip->progname,cp) ;

	goto ret1 ;
}
/* end subroutine (process) */



/* LOCAL SUBROUTINES */



ulong ufloorl(v,m)
ulong	v, m ;
{
	ulong	n ;


	n = v / m ;
	return (n * m) ;
}
/* end subroutine (ufloorl) */


ulong uceill(v,m)
ulong	v, m ;
{
	uint	n ;


	n = (v + (m - 1)) / m ;
	return (n * m) ;
}
/* end subroutine (uceill) */




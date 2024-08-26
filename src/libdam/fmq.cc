/* fmq SUPPORT */
/* lang=C++20 */

/* File Message Queue (FMQ) */
/* version %I% last-modified %G% */

#define	CF_SAFE		1		/* safer */
#define	CF_ALWAYSCREATE	0		/* always create file */
#define	CF_SENDCREATE	0		/* sender creates also */
#define	CF_SIGFILLSET	1		/* signal mask on interrupt */

/* revision history:

	= 1999-07-23, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	fmq

	Description:
	This module implements a file message queue facility.
	Enjoy!

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<sys/uio.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<endian.h>
#include	<stdorder.h>
#include	<localmisc.h>

#include	"fmq.h"


/* local defines */

#define	FMQ_OPENTIME	30	/* seconds */

#define	FMQ_IDLEN	(16 + 4)
#define	FMQ_HEADLEN	(8 * 4)
#define	FMQ_TOPLEN	(FMQ_IDLEN + FMQ_HEADLEN)

#define	FMQ_IDOFF	0
#define	FMQ_HEADOFF	FMQ_IDLEN
#define	FMQ_BUFOFF	(FMQ_HEADOFF + FMQ_HEADLEN)

#define	FMQ_BUFSIZE	(64 * 1024)
#define	FMQ_RBUFSIZE	((1 * 1024) + 4)
#define	FMQ_WBUFSIZE	((1 * 1024) + 4)

#define	TO_OPEN		(60 * 60)	/* maximum file-open time */
#define	TO_ACCESS	(2 * 60)	/* maximum access idle time */
#define	TO_LOCK		10		/* seconds */

#define	TI_MINUPDATE	4		/* minimum time between updates */

#ifndef	TIMEBUFLEN
#define	TIMEBUFLEN	80
#endif

#define	NENTRIES	100
#define	FBUFLEN		(FMQ_TOPLEN + 9)


/* external subroutines */

extern uint	uceil(uint,uint) ;

extern int	matstr(cchar **,cchar *,int) ;
extern int	sncpy2(char *,int,cchar *,cchar *) ;
extern int	mkpath2(char *,cchar *,cchar *) ;
extern int	lockfile(int,int,off_t,off_t,int) ;
extern int	msleep(uint) ;
extern int	isfsremote(int) ;

extern char	*strwcpy(char *,cchar *,int) ;
extern char	*timestr_log(time_t,char *) ;


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
static int fmq_ctor(fmq *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    rs = memclear(op) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (fmq_ctor) */

static int fmq_dtor(fmq *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (fmq_dtor) */

template<typename ... Args>
static inline int fmq_magic(fmq *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == FMQ_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (fmq_magic) */

static int	fmq_isend(fmq *,cvoid *,int,int) noex ;
static int	fmq_irecv(fmq *,void *,int,int) noex ;
static int	fmq_filecheck(fmq *,time_t,int,int) noex ;
static int	fmq_filebegin(fmq *,time_t) noex ;
static int	fmq_filechanged(fmq *) noex ;
static int	fmq_lockget(fmq *,time_t,int) noex ;
static int	fmq_lockrelease(fmq *) noex ;
static int	fmq_fileopen(fmq *,time_t) noex ;
static int	fmq_fileclose(fmq *) noex ;
static int	fmq_bufbegin(fmq *) noex ;
static int	fmq_bufend(fmq *) noex ;
static int	fmq_headwrite(fmq *) noex ;
static int	fmq_di(fmq *,sigset_t *) noex ;
static int	fmq_ei(fmq *,sigset_t *) noex ;

#ifdef	COMMENT
static int	fmq_headread(fmq *) noex ;
#endif

static int	filemagic(char *,int,fmq_fm *) noex ;
static int	filehead(char *,int,FMQ_FH *) noex ;


/* local variables */

constexpr bool		f_alwayscreate = CF_ALWAYSCREATE ;


/* exported variables */


/* exported subroutines */

int fmq_open(fmq *op,cchar *fname,int of,mode_t operm,int bufsize) noex {
	int		rs ;
	if ((rs = fmq_ctor(op,fname)) >= 0) {
	    rs = SR_INVALID ;
	    if (fname[0]) {
	        USTAT		sb ;
	        time_t		dt = time(nullptr) ;
	        int		amode ;
	        int		f_create = FALSE ;
	        if (bufsize < FMQ_BUFSIZE) bufsize = FMQ_BUFSIZE ;

#if	CF_ALWAYSCREATE
	of|= O_CREAT ;
#endif

	of = (of & (~ O_TRUNC)) ;

	op->f.create = (of & O_CREAT) ? TRUE : FALSE ;
	op->f.ndelay = (of & O_NDELAY) ? TRUE : FALSE ;
	op->f.nonblock = (of & O_NONBLOCK) ? TRUE : FALSE ;

	of = (of & (~ (O_NDELAY | O_NONBLOCK))) ;

	op->oflags = of ;
	op->operm = operm ;

	{
	    cchar	*cp ;
	    rs = uc_mallocstrw(fname,-1,&cp) ;
	    if (rs >= 0) op->fname = cp ;
	}
	if (rs < 0) goto bad0 ;

/* initialize the buffer structure */

	rs = fmq_bufbegin(op) ;
	if (rs < 0)
	    goto bad1 ;

/* try to open the file */

	of = (of& (~ O_CREAT)) ;
	rs = u_open(op->fname,of,operm) ;

	if ((rs < 0) && (op->oflags & O_CREAT)) {

	    f_create = TRUE ;
	    of = op->oflags ;
	    rs = u_open(op->fname,of,operm) ;

	} /* end if (creating file) */

	op->fd = rs ;
	if (rs < 0)
	    goto bad2 ;

	amode = (operm & O_ACCMODE) ;
	op->f.writable = ((amode == O_WRONLY) || (amode == O_RDWR)) ;

	op->opentime = dt ;
	op->accesstime = dt ;
	rs = u_fstat(op->fd,&sb) ;

	if (rs < 0)
	    goto bad3 ;

	op->mtime = sb.st_mtime ;
	op->filesize = sb.st_size ;
	op->pagesize = getpagesize() ;

/* local or remote */

	rs = isfsremote(op->fd) ;
	op->f.remote = (rs > 0) ;
	if (rs < 0)
	    goto bad3 ;

/* determine some operational parameters (size of buffer space) */

	op->bufsize = uceil(bufsize,sizeof(int)) ;

/* setup for disabling signals */

#if	CF_SIGFILLSET
	uc_sigsetfill(&op->sigmask) ;
#else
	uc_sigsetempty(&op->sigmask) ;
#endif

/* header processing */

	rs = fmq_filebegin(op,dt) ;

	if ((rs < 0) && (rs != SR_AGAIN))
	    goto bad3 ;

	if ((rs == SR_AGAIN) && (! op->f.create))
	    rs = SR_OK ;

/* out of here */

	op->magic = FMQ_MAGIC ;
	    if (rs < 0) {
		fmq_dtor(op) ;
	    }
	} /* end if (fmq_ctor) */
ret0:
	return (rs >= 0) ? f_create : rs ;

/* bad things */
bad4:
bad3:
	u_close(op->fd) ;

bad2:
	fmq_bufend(op) ;

bad1:
	if (op->fname != nullptr)
	    uc_free(op->fname) ;

bad0:
	goto ret0 ;
}
/* end subroutine (fmq_open) */


int fmq_close(fmq *op)
{
	int		rs = SR_OK ;
	int		rs1 ;

#if	CF_SAFE
	if (op == nullptr) return SR_FAULT ;

	if (op->magic != FMQ_MAGIC) return SR_NOTOPEN ;
#endif /* CF_SAFE */

	rs1 = fmq_bufend(op) ;
	if (rs >= 0) rs = rs1 ;

	if (op->fd >= 0) {
	    rs1 = u_close(op->fd) ;
	    if (rs >= 0) rs = rs1 ;
	    op->fd = -1 ;
	}

	rs1 = uc_free(op->fname) ;
	if (rs >= 0) rs = rs1 ;

	op->magic = 0 ;
	return rs ;
}
/* end subroutine (fmq_close) */


/* get a count of the number of entries */
int fmq_count(fmq *op)
{
	int		rs = SR_OK ;
	int		c = 0 ;

#if	CF_SAFE
	if (op == nullptr) return SR_FAULT ;

	if (op->magic != FMQ_MAGIC) return SR_NOTOPEN ;
#endif /* CF_SAFE */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (fmq_count) */


/* send a message */
int fmq_send(fmq *op,cvoid *buf,int buflen)
{
	int		rs ;
	int		to ;
	int		tlen = 0 ;

#if	CF_SAFE
	if (op == nullptr) return SR_FAULT ;

	if (op->magic != FMQ_MAGIC) return SR_NOTOPEN ;
#endif /* CF_SAFE */

	if (buf == nullptr) return SR_FAULT ;

	if (! op->f.writable)
	    return SR_RDONLY ;

/* continue with normal operation */

	to = -1 ;
	rs = fmq_sende(op,buf,buflen,to,0) ;
	tlen = rs ;

	return (rs >= 0) ? tlen : rs ;
}
/* end subroutine (fmq_send) */


/* send a message */
int fmq_sende(fmq *op,cvoid *buf,int buflen,int to,int opts)
{
	ulong		starttime, endtime, dt ;
	int		rs ;
	int		f_infinite = FALSE ;
	int		tlen = 0 ;

#if	CF_SAFE
	if (op == nullptr) return SR_FAULT ;

	if (op->magic != FMQ_MAGIC) return SR_NOTOPEN ;
#endif /* CF_SAFE */

	if (buf == nullptr) return SR_FAULT ;

	if (! op->f.writable)
	    return SR_RDONLY ;

/* continue with the real work */

	if (to < 0) {
	    f_infinite = TRUE ;
	    to = INT_MAX ;
	}

	starttime = time(nullptr) ;

	endtime = starttime + to ;
	if (endtime < starttime)
	    endtime = INT_MAX ;

/* CONSTCOND */

	while (TRUE) {

	    rs = fmq_isend(op,buf,buflen,opts) ;
	    tlen = rs ;
	    if (rs >= 0)
	        break ;

	    if (rs != SR_AGAIN)
	        break ;

	    if (f_infinite && (op->f.ndelay || op->f.nonblock))
	        break ;

	    if (to <= 0)
	        break ;

	    msleep(1000) ;

	    dt = time(nullptr) ;

	    if (dt >= endtime) break ;
	} /* end while */

	if ((rs == SR_AGAIN) && op->f.ndelay) {
	    tlen = 0 ;
	    rs = SR_OK ;
	}

ret0:
	return (rs >= 0) ? tlen : rs ;
}
/* end subroutine (fmq_sende) */


/* receive a message */
int fmq_recv(fmq *op,void *buf,int buflen)
{
	int		rs = SR_OK ;
	int		to ;
	int		tlen = 0 ;

#if	CF_SAFE
	if (op == nullptr) return SR_FAULT ;

	if (op->magic != FMQ_MAGIC) return SR_NOTOPEN ;
#endif /* CF_SAFE */

	if (buf == nullptr) return SR_FAULT ;

	if (! op->f.writable)
	    return SR_RDONLY ;

/* continue with normal operation */

	to = -1 ;
	rs = fmq_recve(op,buf,buflen,to,0) ;
	tlen = rs ;

	return (rs >= 0) ? tlen : rs ;
}
/* end subroutine (fmq_recv) */


/* receive a message */
int fmq_recve(fmq *op,void *buf,int buflen,int to,int opts)
{
	int		rs = SR_OK ;
	int		tlen = 0 ;
	int		f_infinite = FALSE ;

#if	CF_SAFE
	if (op == nullptr) return SR_FAULT ;

	if (op->magic != FMQ_MAGIC) return SR_NOTOPEN ;
#endif /* CF_SAFE */

	if (buf == nullptr) return SR_FAULT ;

	if (! op->f.writable)
	    return SR_RDONLY ;

/* continue with the real work */

	if (to < 0) {
	    f_infinite = TRUE ;
	    to = INT_MAX ;
	}

	while (to >= 0) {

	    rs = fmq_irecv(op,buf,buflen,opts) ;
	    tlen = rs ;
	    if (rs >= 0)
	        break ;

	    if (rs != SR_AGAIN)
	        break ;

	    if (f_infinite && (op->f.ndelay || op->f.nonblock))
	        break ;

	    if (to <= 0)
	        break ;

	    to -= 1 ;
	    sleep(1) ;

	} /* end while */

	if ((rs == SR_AGAIN) && op->f.ndelay) {
	    tlen = 0 ;
	    rs = SR_OK ;
	}

ret0:
	return (rs >= 0) ? tlen : rs ;
}
/* end subroutine (fmq_recve) */


/* do some checking */
int fmq_check(fmq *op,time_t dt)
{
	int		rs = SR_OK ;
	int		f = FALSE ;

#if	CF_SAFE
	if (op == nullptr) return SR_FAULT ;

	if (op->magic != FMQ_MAGIC) return SR_NOTOPEN ;
#endif /* CF_SAFE */

	if (op->fd >= 0) {
	    if ((! op->f.readlocked) && (! op->f.writelocked)) {
	        if ((dt - op->accesstime) >= TO_ACCESS) {
	            f = TRUE ;
		    rs = fmq_fileclose(op) ;
		}
	    }
	}

	return (rs >= 0) ? f : rs ;
}
/* end subroutine (fmq_check) */


/* private subroutines */


/* send a message */
/* ARGSUSED */
static int fmq_isend(fmq *op,cvoid *buf,int buflen,int opts)
{
	IOVEC		v[3] ;
	sigset_t	oldsigmask ;
	off_t	uoff ;
	time_t		dt = time(nullptr) ;
	uint		eoff ;
	uint		llen, dlen, len ;
	int		rs ;
	char		lenbuf[4 + 1] ;
	char		*cbuf = (char *) buf ;

	fmq_di(op,&oldsigmask) ;

/* do we have proper file access? */

	rs = fmq_filecheck(op,dt,0,0) ;
	if (rs < 0)
	    goto ret1 ;

/* is the file initialized? */

	if (! op->f.fileinit) {

#if	CF_SENDCREATE
	    rs = SR_ACCESS ;
	    if (! op->f.create)
	        goto ret1 ;
#endif /* CF_SENDCREATE */

	    if (dt == 0)
	        dt = time(nullptr) ;

	    rs = fmq_filebegin(op,dt) ;
	    if (rs < 0)
	        goto ret1 ;

	}

/* prepare the message header (the length) */

	llen = stdorder_wui(lenbuf,buflen) ;

/* can we even write this message? */

	if ((buflen + llen) > (op->h.size - op->h.blen)) {
	    rs = ((buflen + llen) > op->h.size) ? SR_TOOBIG : SR_AGAIN ;
	    goto ret1 ;
	}

/* prepare to write */

	if (op->h.wi < op->h.ri) {
	    dlen = op->h.ri - op->h.wi ;
	} else {
	    dlen = op->h.size - op->h.wi ;
	}

	dlen -= llen ;
	if (buflen < dlen)
	    dlen = buflen ;

/* set up the write buffers */

	v[0].iov_base = lenbuf ;
	v[0].iov_len = llen ;

	v[1].iov_base = (caddr_t) cbuf ;
	v[1].iov_len = dlen ;

	v[2].iov_base = nullptr ;
	v[2].iov_len = 0 ;

	eoff = FMQ_BUFOFF + op->h.wi ;
	uoff = eoff ;
	rs = u_seek(op->fd,uoff,SEEK_SET) ;

	if (rs >= 0)
	    rs = u_writev(op->fd,v,2) ;

	if ((rs >= 0) && (dlen < buflen)) {

	    v[0].iov_base = (caddr_t) (cbuf + dlen) ;
	    v[0].iov_len = (buflen - dlen) ;

	    v[1].iov_base = nullptr ;
	    v[1].iov_len = 0 ;

	    eoff = FMQ_BUFOFF ;
	    uoff = eoff ;
	    if ((rs = u_seek(op->fd,uoff,SEEK_SET)) >= 0) {
	        rs = u_writev(op->fd,v,1) ;
	    }

	} /* end if (wrapped around) */

	if (rs > 0) {

	    len = uceil((llen + buflen),llen) ;

	    op->h.wi = (op->h.wi + len) % op->h.size ;
	    op->h.blen += len ;
	    op->h.len += buflen ;
	    op->h.nmsg += 1 ;

	    if (dt == 0)
	        dt = time(nullptr) ;

	    op->h.wcount += 1 ;
	    op->h.wtime = dt ;

	    rs = fmq_headwrite(op) ;

	    if ((rs >= 0) && op->f.remote)
	        u_fsync(op->fd) ;

	} /* end if (data write was successful) */

ret1:
	fmq_lockrelease(op) ;

ret0:
	fmq_ei(op,&oldsigmask) ;

	return (rs >= 0) ? buflen : rs ;
}
/* end subroutine (fmq_isend) */


/* receive a message */
static int fmq_irecv(fmq *op,void *buf,int buflen,int opts)
{
	IOVEC		v[3] ;
	USTAT		sb ;
	sigset_t	oldsigmask ;
	off_t	uoff ;
	time_t		dt = 0 ;
	uint		eoff ;
	uint		llen, dlen, mlen, len ;
	int		rs = SR_OK ;
	int		tlen = 0 ;
	int		f_changed ;
	char		lenbuf[4 + 1] ;
	char		*cbuf = (char *) buf ;

/* are we in optional slow-poll mode? */

	if ((opts & FM_SLOWPOLL) && (op->h.nmsg == 0)) {

	    rs = u_fstat(op->fd,&sb) ;
	    if (rs < 0)
	        goto ret0 ;

	    f_changed = (sb.st_size != op->filesize) ||
	        (sb.st_mtime != op->mtime) ;

	    rs = SR_AGAIN ;
	    if (! f_changed)
	        goto ret0 ;

	} /* end if (optional slow-poll mode) */

/* continue with a regular operation */

	fmq_di(op,&oldsigmask) ;

/* do we have proper file access? */

	if (dt == 0)
	    dt = time(nullptr) ;

	rs = fmq_filecheck(op,dt,0,opts) ;
	if (rs < 0)
	    goto ret2 ;

/* is the file initialized? */

	if (! op->f.fileinit) {

	    if (dt == 0)
	        dt = time(nullptr) ;

	    rs = fmq_filebegin(op,dt) ;
	    if (rs < 0)
	        goto ret2 ;

	}

/* are there any messages to receive? */

	if (op->h.nmsg == 0) {
	    rs = SR_AGAIN ;
	    goto ret2 ;
	}

	llen = sizeof(uint) ;

/* prepare to read */

	if (op->h.ri >= op->h.wi) {
	    dlen = op->h.wi - op->h.ri ;
	} else {
	    dlen = op->h.size - op->h.ri ;
	}

	dlen -= llen ;
	if (buflen < dlen)
	    dlen = buflen ;

/* set up the read buffers */

	v[0].iov_base = lenbuf ;
	v[0].iov_len = llen ;

	v[1].iov_base = (caddr_t) cbuf ;
	v[1].iov_len = dlen ;

	v[2].iov_base = nullptr ;
	v[2].iov_len = 0 ;

	eoff = FMQ_BUFOFF + op->h.ri ;
	uoff = eoff ;
	rs = u_seek(op->fd,uoff,SEEK_SET) ;

	if (rs >= 0)
	    rs = u_readv(op->fd,v,2) ;

	if (rs >= llen) {

	    tlen = rs - llen ;
	    stdorder_rui(lenbuf,&mlen) ;

	    if (mlen > tlen) {

	        if (buflen >= mlen) {

	            v[0].iov_base = (caddr_t) (cbuf + tlen) ;
	            v[0].iov_len = (mlen - tlen) ;

	            v[1].iov_base = nullptr ;
	            v[1].iov_len = 0 ;

	            eoff = FMQ_BUFOFF ;
		    uoff = eoff ;
	            rs = u_seek(op->fd,uoff,SEEK_SET) ;

	            if (rs >= 0)
	                rs = u_readv(op->fd,v,1) ;

	            if (rs >= 0)
	                tlen += rs ;

	        } else
	            rs = SR_OVERFLOW ;

	    } else
	        tlen = mlen ;

	} else
	    rs = SR_BADFMT ;

	if (rs >= 0) {

	    len = uceil((llen + mlen),llen) ;

	    op->h.ri = (op->h.ri + len) % op->h.size ;
	    op->h.blen -= len ;
	    op->h.len -= mlen ;
	    op->h.nmsg -= 1 ;

	    if (dt == 0)
	        dt = time(nullptr) ;

	    op->h.wcount += 1 ;
	    op->h.wtime = dt ;

	    rs = fmq_headwrite(op) ;

	    if ((rs >= 0) && op->f.remote)
	        u_fsync(op->fd) ;

	} /* end if (we were able to turn the buffer) */

ret2:
	fmq_lockrelease(op) ;

ret1:
	fmq_ei(op,&oldsigmask) ;

ret0:
	return (rs >= 0) ? tlen : rs ;
}
/* end subroutine (fmq_irecv) */


/* check the file for coherency */
/* ARGSUSED */
static int fmq_filecheck(fmq *op,time_t dt,int f_read,int opts)
{
	int		rs = SR_OK ;
	int		f_changed = FALSE ;

/* is the file open */

	if (op->fd < 0) {

	    if (dt == 0)
	        dt = time(nullptr) ;

	    rs = fmq_fileopen(op,dt) ;
	    if (rs < 0)
	        goto ret0 ;

	}

/* check for optional slow-poll mode */

#ifdef	COMMENT
	if ((opts & FM_SLOWPOLL) && (op->h.nmsg <= 0)) {

	    rs = u_fstat(op->fd,&sb) ;
	    if (rs < 0)
	        goto bad0 ;

	    f_changed = (sb.st_size != op->filesize) ||
	        (sb.st_mtime != op->mtime) ;

	    if (! f_changed)
	        goto ret0 ;

	} /* end if (optional slow-poll mode) */
#endif /* COMMENT */

/* capture the lock if we do not already have it */

	if ((! op->f.readlocked) && (! op->f.writelocked)) {

	    if (dt == 0)
	        dt = time(nullptr) ;

	    rs = fmq_lockget(op,dt,f_read) ;
	    if (rs < 0)
	        goto ret0 ;

	    rs = fmq_filechanged(op) ;
	    if (rs < 0)
	        goto bad1 ;

	    f_changed = (rs > 0) ;

	} /* end if (capture lock) */

ret0:
	return (rs >= 0) ? f_changed : rs ;

/* bad stuff */
bad1:
	fmq_lockrelease(op) ;

bad0:
	return rs ;
}
/* end subroutine (fmq_filecheck) */


/* has the file changed at all? */
static int fmq_filechanged(fmq *op)
{
	USTAT		sb ;
	int		rs ;
	int		f_statchanged = FALSE ;
	int		f_headchanged = FALSE ;

/* has the file changed at all? */

	rs = u_fstat(op->fd,&sb) ;

#ifdef	COMMENT
	if (rs == SR_NOENT)
	    rs = SR_OK ;
#endif /* COMMENT */

	if (rs < 0)
	    goto bad2 ;

	if (sb.st_size < FMQ_BUFOFF)
	    op->f.fileinit = FALSE ;

	f_statchanged = (! op->f.fileinit) ||
	    (sb.st_size != op->filesize) ||
	    (sb.st_mtime != op->mtime) ;

/* read the file header for write indications */

	if (op->f.fileinit) {
	    FMQ_FH	h ;
	    char	hbuf[FMQ_TOPLEN + 1] ;

	    rs = u_pread(op->fd,hbuf,FMQ_TOPLEN,0L) ;

	    if (rs < 0)
	        goto bad2 ;

	    if (rs < FMQ_TOPLEN)
	        op->f.fileinit = FALSE ;

	    if (rs > 0) {

	        filehead((hbuf + FMQ_HEADOFF),1,&h) ;

	        f_headchanged = (op->h.wtime != h.wtime) ||
	            (op->h.wcount != h.wcount) ||
	            (op->h.nmsg != h.nmsg) ;

	        if (f_headchanged)
	            op->h = h ;

	    }

	} /* end if (reading file header) */

/* OK, we're done */

	if (f_statchanged) {
	    op->f.bufvalid = FALSE ;
	    op->filesize = sb.st_size ;
	    op->mtime = sb.st_mtime ;
	}

ret0:
	return (rs >= 0) ? f_headchanged : rs ;

/* bad stuff */
bad2:
bad1:
bad0:
	goto ret0 ;
}
/* end subroutine (fmq_filechanged) */

static int fmq_bufbegin(fmq *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    op->f.bufvalid = FALSE ;
	    op->b = {} ;
	    op->b.bsz = FMQ_BUFSIZE ;
	    rs = uc_malloc(op->b.bsz,&op->b.buf) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (fmq_bufbegin) */

static int fmq_bufend(fmq *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->b.buf) {
	    rs1 = uc_free(op->b.buf) ;
	    if (rs >= 0) rs = rs1 ;
	    op->b.buf = nullptr ;
	}
	op->b.bsz = 0 ;
	op->b.len = 0 ;
	op->b.i = 0 ;
	return rs ;
}
/* end subroutine (fmq_bufend) */

/* initialize the file header (either read it only or write it) */
static int fmq_filebegin(fmq *op,time_t dt) noex {
	fmq_fm		fm ;
	sigset_t	oldsigmask ;
	int		rs = SR_OK ;
	int		bl ;
	int		f_locked = FALSE ;
	char		fbuf[FBUFLEN + 1] ;

	fmq_di(op,&oldsigmask) ;

	if (op->filesize == 0) {

	    u_seek(op->fd,0L,SEEK_SET) ;

	    op->f.fileinit = FALSE ;
	    if (op->f.writable && op->f.create) {

	        if (! op->f.writelocked) {

	            rs = fmq_lockget(op,dt,0) ;
	            if (rs < 0)
	                goto ret0 ;

	            f_locked = TRUE ;
	        }

/* write the file header stuff */

	        strwcpy(fm.magic,FMQ_FILEMAGIC,14) ;

	        fm.vetu[0] = FMQ_FILEVERSION ;
	        fm.vetu[1] = FMQ_ENDIAN ;
	        fm.vetu[2] = 0 ;
	        fm.vetu[3] = 0 ;

	        bl = 0 ;
	        bl += filemagic((fbuf + bl),0,&fm) ;

	        memset(&op->h,0,sizeof(FMQ_FH)) ;

	        op->h.size = op->bufsize ;

	        bl += filehead((fbuf + bl),0,&op->h) ;

	        rs = u_pwrite(op->fd,fbuf,bl,0L) ;

	        if (rs > 0) {
	            op->filesize = rs ;
	            op->mtime = dt ;
	            if (op->f.remote) {
	                u_fsync(op->fd) ;
		    }
	        }

	        op->f.fileinit = (rs >= 0) ;

	    } /* end if (writing) */

	} else if (op->filesize >= FMQ_BUFOFF) {
	    int	f ;

/* read the file header */

	    if (! op->f.readlocked) {

	        rs = fmq_lockget(op,dt,0) ;
	        if (rs < 0)
	            goto ret0 ;

	        f_locked = TRUE ;
	    }

	    rs = u_pread(op->fd,fbuf,FBUFLEN,0L) ;

	    if (rs >= FMQ_TOPLEN) {

	        bl = 0 ;
	        bl += filemagic((fbuf + bl),1,&fm) ;

	        filehead((fbuf + bl),1,&op->h) ;

	        f = (strcmp(fm.magic,FMQ_FILEMAGIC) == 0) ;
	        f = f && (fm.vetu[0] <= FMQ_FILEVERSION) ;
	        f = f && (fm.vetu[1] == FMQ_ENDIAN) ;
	        if (! f)
	            rs = SR_BADFMT ;

	        op->f.fileinit = f ;

	    } /* end if (read was big enough) */

	} /* end if */

/* if we locked, we unlock it, otherwise leave it ! */

	if (f_locked)
	    fmq_lockrelease(op) ;

/* we're out of here */
ret0:
	fmq_ei(op,&oldsigmask) ;

	return rs ;
}
/* end subroutine (fmq_filebegin) */


/* acquire access to the file */
static int fmq_lockget(fmq *op,time_t dt,int f_read)
{
	int		rs = SR_OK ;
	int		lockcmd ;
	int		f_already = FALSE ;

	if (op->fd < 0) {

	    rs = fmq_fileopen(op,dt) ;

	    if (rs < 0)
	        goto bad0 ;

	} /* end if (needed to open the file) */

/* acquire a file record lock */

	if (f_read || (! op->f.writable)) {

	    f_already = op->f.readlocked ;
	    op->f.readlocked = TRUE ;
	    op->f.writelocked = FALSE ;
	    lockcmd = F_RLOCK ;

	} else {

	    f_already = op->f.writelocked ;
	    op->f.readlocked = FALSE ;
	    op->f.writelocked = TRUE ;
	    lockcmd = F_WLOCK ;

	}

/* get out if we have the lock that we want already */

	if (f_already) {
	    rs = SR_OK ;
	    goto ret0 ;
	}

/* we need to actually do the lock */

	{
	    off_t	fs = op->filesize ;
	    rs = lockfile(op->fd,lockcmd,0L,fs,TO_LOCK) ;
	}

ret0:
	return rs ;

/* bad stuff */
bad1:
	op->f.readlocked = FALSE ;
	op->f.writelocked = FALSE ;

bad0:
	goto ret0 ;
}
/* end subroutine (fmq_lockget) */

static int fmq_lockrelease(fmq *op) noex {
	int		rs = SR_OK ;
	if ((op->f.readlocked || op->f.writelocked)) {
	    if (op->fd >= 0) {
		off_t	fs = op->filesize ;
	        rs = lockfile(op->fd,F_ULOCK,0L,fs,TO_LOCK) ;
	    }
	    op->f.readlocked = FALSE ;
	    op->f.writelocked = FALSE ;
	}
	return rs ;
}
/* end subroutine (fmq_lockrelease) */

static int fmq_fileopen(fmq *op,time_t dt) noex {
	int		rs = SR_OK ;
	if (op->fd < 0) {
	    if ((rs = u_open(op->fname,op->oflags,op->operm)) >= 0) {
		op->fd = rs ;
		uc_closeonexec(op->fd,TRUE) ;
		op->opentime = dt ;
	    }
	}
	return (rs >= 0) ? op->fd : rs ;
}
/* end subroutine (fmq_fileopen) */

int fmq_fileclose(fmq *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->fd >= 0) {
	    rs1 = u_close(op->fd) ;
	    if (rs >= 0) rs = rs1 ;
	    op->fd = -1 ;
	}
	return rs ;
}
/* end subroutine (fmq_fileclose) */

static int fmq_headwrite(fmq *op) noex {
	off_t		uoff = FMQ_HEADOFF ;
	int		rs ;
	int		bl ;
	char		fbuf[FBUFLEN + 1] ;
	if ((rs = filehead(fbuf,0,&op->h)) >= 0) {
	    bl = rs ;
	   rs = u_pwrite(op->fd,fbuf,bl,uoff) ;
	}
	return rs ;
}
/* end subroutine (fmq_headwrite) */

static int fmq_di(fmq *op,sigset_t *smp) noex {
	int		rs = SR_BADFMT ;
	if (op && smp) {
	    rs = pthread_sigmask(SIG_BLOCK,&op->sigmask,smp) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (fmq_di) */

static int fmq_ei(fmq *op,sigset_t *smp) noex {
	int		rs = SR_BADFMT ;
	if (op && smp) {
	    rs = pthread_sigmask(SIG_SETMASK,smp,nullptr) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (fmq_ei) */

static int filemagic(char *buf,int f_read,fmq_fm *mp) noex {
	int		rs = SR_BADFMT ;
	if (buf && mp) {
	char	*bp = buf ;
	char	*cp ;
	    rs = 20 ;
	    if (f_read) {
	        bp[15] = '\0' ;
	        strncpy(mp->magic,bp,15) ;
	        if ((cp = strchr(mp->magic,'\n')) != nullptr) {
	            *cp = '\0' ;
	        }
	        bp += 16 ;
	        memcpy(mp->vetu,bp,4) ;
	    } else {
	        bp = strwcpy(bp,mp->magic,15) ;
	        *bp++ = '\n' ;
	        memset(bp,0,(16 - (bp - buf))) ;
	        bp = buf + 16 ;
	        memcpy(bp,mp->vetu,4) ;
	    } /* end if */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (filemagic) */

/* encode or decode the file header */
static int filehead(char *buf,int f_read,FMQ_FH *hp) noex {
	uint		*table = (uint *) buf ;
	int		rs = SR_BADFMT ;
	if (buf) {
	    if (f_read) {
	        stdorder_rui((table + 0),&hp->wcount) ;
	        stdorder_rui((table + 1),&hp->wtime) ;
	        stdorder_rui((table + 2),&hp->nmsg) ;
	        stdorder_rui((table + 3),&hp->bsz) ;
	        stdorder_rui((table + 4),&hp->blen) ;
	        stdorder_rui((table + 5),&hp->len) ;
	        stdorder_rui((table + 6),&hp->ri) ;
	        stdorder_rui((table + 7),&hp->wi) ;
	    } else {
	        stdorder_wui((table + 0),hp->wcount) ;
	        stdorder_wui((table + 1),hp->wtime) ;
	        stdorder_wui((table + 2),hp->nmsg) ;
	        stdorder_wui((table + 3),hp->bsz) ;
	        stdorder_wui((table + 4),hp->blen) ;
	        stdorder_wui((table + 5),hp->len) ;
	        stdorder_wui((table + 6),hp->ri) ;
	        stdorder_wui((table + 7),hp->wi) ;
	    } /* end if */
	    rs = FMQ_HEADLEN ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (filehead) */



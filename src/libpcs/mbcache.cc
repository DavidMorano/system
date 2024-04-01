/* mbcache SUPPORT (MailBox-Cache) */
/* lang=C++20 */

/* cache mailbox items */
/* version %I% last-modified %G% */

#define	CF_LOADENVADDR	0		/* compile |mbcache_loadenvaddr()| */

/* revision history:

	= 2009-01-10, David A­D­ Morano
	This module is an attempt to take some of the garbage out
	of the piece-of-total-garbage VMAIL program.

*/

/* Copyright © 2009 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This object parses out and then caches information about
	messages within a mailbox.  All information saved about
	each message is stored under the index of that message
	within the original mailbox.

	Implementation notes

	= Things to watch for (or to do in the future)

	This object is a cache of the MAILBOX object, but we provide
	some additional processing that the MAILBOX object does not
	provide.  For the most part, the MAILBOX object contains
	all read-only data (mail message data).  But the deletion
	status of a mail message is potentially dynamic -- meaning
	that it can be set or cleared after the mailbox is opened.
	Currently it is assumed that if a message is in a mailbox
	that it is "not deleted."  But this may not be the case in
	the future (messages may be deleted but still in the mailbox).
	As a cache, this object may need to be enhanced to always
	get the deletion status of a message from the MAILBOX object
	rather than assuming that it is by default "not deleted."

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>
#include	<unistd.h>
#include	<limits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<bfile.h>
#include	<sbuf.h>
#include	<char.h>
#include	<mailmsg.h>
#include	<mailmsg_enver.h>
#include	<mailmsghdrs.h>
#include	<ema.h>
#include	<vecobj.h>
#include	<tmtime.h>
#include	<mapstrint.h>
#include	<localmisc.h>

#include	"mailbox.h"
#include	"mbcache.h"


/* local defines */

#define	MSGENTRY	MBCACHE_SCAN

#ifndef	LINEBUFLEN
#ifdef	LINE_MAX
#define	LINEBUFLEN	MAX(LINE_MAX,2048)
#else
#define	LINEBUFLEN	2048
#endif
#endif

#ifndef	TMPDNAME
#define	TMPDNAME	"/tmp"
#endif

#ifndef	MAILADDRLEN
#define	MAILADDRLEN	(3 * MAXHOSTNAMELEN)
#endif

#undef	BUFLEN
#define	BUFLEN		MAX(100,MAILADDRLEN)

#ifndef	HDRBUFLEN
#define	HDRBUFLEN	(2*MAILADDRLEN)
#endif

#undef	SCANBUFLEN
#define	SCANBUFLEN	100

#ifndef	DIGBUFLEN
#define	DIGBUFLEN	40		/* can hold int128_t in decimal */
#endif

#define	RECORDFNAME	"envelopes.log"

#define	TI_LOCK		120

#ifndef	COL_SCANFROM
#define	COL_SCANFROM	2
#endif
#ifndef	COL_SCANSUBJECT
#define	COL_SCANSUBJECT	29
#endif
#ifndef	COL_SCANDATE
#define	COL_SCANDATE	60
#endif
#ifndef	COL_SCANLINES
#define	COL_SCANLINES	75
#endif


/* external subroutines */

extern int	sncpy1(char *,int,cchar *) ;
extern int	snwcpy(char *,int,cchar *,int) ;
extern int	cfdeci(cchar *,int,int *) ;
extern int	cfdecui(cchar *,uint,uint *) ;
extern int	ctdeci(char *,int,int) ;
extern int	mailmsg_loadmb(MAILMSG *,MAILBOX *,off_t) ;
extern int	hdrextid(char *,int,cchar *,int) ;
extern int	mkbestaddr(char *,int,cchar *,int) ;
extern int	mkaddrname(char *,int,cchar *,int) ;
extern int	isOneOf(cint *,int) ;
extern int	isNotPresent(int) ;

extern char	*strwcpy(char *,cchar *,int) ;
extern char	*strwcpyblanks(char *,int) ;
extern char	*timestr_scandate(time_t,char *) ;


/* external variables */


/* local structures */

struct scantitle {
	char		*name ;
	int		col ;
} ;


/* forward references */

static int mbcache_msgfins(MBCACHE *) noex ;
static int mbcache_msgframing(MBCACHE *,int,MSGENTRY **) noex ;
static int mbcache_msgtimers(MBCACHE *,int,time_t *) noex ;

#ifdef	COMMENT
static int mbcache_msgscanmk(MBCACHE *,int) noex ;
#endif

static int msgentry_start(MSGENTRY *,int) noex ;
static int msgentry_frame(MSGENTRY *,MAILBOX_MSGINFO *) noex ;
static int msgentry_load(MSGENTRY *,MBCACHE *) noex ;
static int msgentry_loadenv(MSGENTRY *,MBCACHE *,MAILMSG *) noex ;
static int msgentry_loadhdrmid(MSGENTRY *,MBCACHE *,MAILMSG *) noex ;
static int msgentry_loadhdrfrom(MSGENTRY *,MBCACHE *,MAILMSG *) noex ;
static int msgentry_loadhdrsubject(MSGENTRY *,MBCACHE *,MAILMSG *) noex ;
static int msgentry_loadhdrdate(MSGENTRY *,MBCACHE *,MAILMSG *) noex ;
static int msgentry_loadhdrstatus(MSGENTRY *,MBCACHE *,MAILMSG *) noex ;
static int msgentry_procenvdate(MSGENTRY *,MBCACHE *) noex ;
static int msgentry_prochdrdate(MSGENTRY *,MBCACHE *) noex ;
static int msgentry_msgtimes(MSGENTRY *,MBCACHE *) noex ;
static int msgentry_procscanfrom(MSGENTRY *,MBCACHE *) noex ;
static int msgentry_procscandate(MSGENTRY *,MBCACHE *) noex ;
static int msgentry_finish(MSGENTRY *) noex ;

#if	CF_LOADENVADDR
static int msgentry_loadenvaddr(MSGENTRY *,MBCACHE *,MAILMSG *,cchar **) noex ;
#endif /* CF_LOADENVADDR */

#ifdef	COMMENT
static int	headappend(char **,char *,int) noex ;
#endif

static int	isNoMsg(int) noex ;
static int	isBadDate(int) noex ;

static int	vcmpmsgentry(const void *,const void *) noex ;


/* local variables */

static constexpr int	rsdatebad[] = {
	SR_INVALID,
	SR_DOM,
	SR_NOMSG,
	0
} ;

static constexpr int	rsnomsg[] = {
	SR_NOMSG,
	SR_NOENT,
	0
} ;


#ifdef	COMMENT
static constexpr struct scantitle	scantitles[] = {
	{ "FROM", COL_SCANFROM },
	{ "SUBJECT", COL_SCANSUBJECT },
	{ "DATE", COL_SCANDATE },
	{ "LINES", COL_SCANLINES },
	{ nullptr, 0 }
} ;
#endif /* COMMENT */


/* exported variables */


/* exported subroutines */

int mbcache_start(MBCACHE *op,cchar *mbfname,int mflags,MAILBOX *mbp) noex {
	strpack		*psp ;
	int		rs = SR_OK ;
	int		nmsgs = 0 ;
	cchar	*cp ;

	if (op == nullptr) return SR_FAULT ;
	if (mbfname == nullptr) return SR_FAULT ;
	if (mbp == nullptr) return SR_FAULT ;

	if (mbfname[0] == '\0') return SR_INVALID ;

	memset(op,0,sizeof(MBCACHE)) ;
	op->mflags = mflags ;
	op->mbp = mbp ;

	op->f.readonly = (! (mflags & MBCACHE_ORDWR)) ;

	psp = &op->strs ;
	if ((rs = uc_mallocstrw(mbfname,-1,&cp)) >= 0) {
	    MAILBOX_INFO	*mip = &op->mbi ;
	    op->mbfname = cp ;
	    if ((rs = mailbox_info(mbp,mip)) >= 0) {
	        cint	mssize = sizeof(MBCACHE_SCAN **) ;
	        if (mip->nmsgs >= 0) {
	            cint	size = ((mip->nmsgs + 1) * mssize) ;
	            void	*p ;
	            nmsgs = mip->nmsgs ;
	            if ((rs = uc_malloc(size,&p)) >= 0) {
	                cint	csize = (mip->nmsgs * 6 * 20) ;
	                op->msgs = p ;
	                memset(op->msgs,0,size) ;
	                if ((rs = strpack_start(psp,csize)) >= 0) {
			    if ((rs = dater_start(&op->dm,nullptr,nullptr,0)) >= 0) {
	    			op->magic = MBCACHE_MAGIC ;
			    }
	                    if (rs < 0)
	                        strpack_finish(psp) ;
	                } /* end if (strpack_start) */
	                if (rs < 0) {
	                    uc_free(op->msgs) ;
	                    op->msgs = nullptr ;
	                }
	            } /* end if (memory-allocation) */
	        } else {
	            rs = SR_NOANODE ;
		}
	    }
	    if (rs < 0) {
	        uc_free(op->mbfname) ;
	        op->mbfname = nullptr ;
	    }
	} /* end if (memory-allocation) */

	return (rs >= 0) ? nmsgs : rs ;
}
/* end subroutine (mbcache_start) */


int mbcache_finish(MBCACHE *op)
{
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != MBCACHE_MAGIC) return SR_NOTOPEN ;

	rs1 = mbcache_msgfins(op) ;
	if (rs >= 0) rs = rs1 ;

	rs1 = dater_finish(&op->dm) ;
	if (rs >= 0) rs = rs1 ;

	rs1 = strpack_finish(&op->strs) ;
	if (rs >= 0) rs = rs1 ;

	if (op->msgs != nullptr) {
	    rs1 = uc_free(op->msgs) ;
	    if (rs >= 0) rs = rs1 ;
	    op->msgs = nullptr ;
	}

	if (op->mbfname != nullptr) {
	    rs1 = uc_free(op->mbfname) ;
	    if (rs >= 0) rs = rs1 ;
	    op->mbfname = nullptr ;
	}

	op->mbp = nullptr ;
	op->magic = 0 ;

	return rs ;
}
/* end subroutine (mbcache_finish) */


/* get the mail filename */
int mbcache_mbfile(MBCACHE *op,char *dbuf,int dlen)
{
	int		rs ;

	if (op == nullptr) return SR_FAULT ;
	if (dbuf == nullptr) return SR_FAULT ;

	if (op->magic != MBCACHE_MAGIC) return SR_NOTOPEN ;

	rs = sncpy1(dbuf,dlen,op->mbfname) ;

	return rs ;
}
/* end subroutine (mbcache_mbfile) */


/* get information */
int mbcache_mbinfo(MBCACHE *op,MBCACHE_INFO *mep)
{
	int		rs ;
	int		nmsgs = 0 ;

	if (op == nullptr) return SR_FAULT ;
	if (mep == nullptr) return SR_FAULT ;

	if (op->magic != MBCACHE_MAGIC) return SR_NOTOPEN ;

/* fill it in */

	memset(mep,0,sizeof(MBCACHE_INFO)) ;

	rs = mailbox_countdel(op->mbp) ;
	mep->nmsgdels = rs ;
	mep->nmsgs = op->mbi.nmsgs ;
	nmsgs = op->mbi.nmsgs ;

	return (rs >= 0) ? nmsgs : rs ;
}
/* end subroutine (mbcache_mbinfo) */


/* get count of messages */
int mbcache_count(MBCACHE *op)
{
	int		nmsgs ;

	if (op == nullptr) return SR_FAULT ;
	if (op->magic != MBCACHE_MAGIC) return SR_NOTOPEN ;

	nmsgs = op->mbi.nmsgs ;
	return nmsgs ;
}
/* end subroutine (mbcache_count) */


/* sort the message by date (oldest first) */
int mbcache_sort(MBCACHE *op)
{
	int		rs = SR_OK ;
	int		nmsgs ;
	int		mi ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != MBCACHE_MAGIC) return SR_NOTOPEN ;

	nmsgs = op->mbi.nmsgs ;

	for (mi = 0 ; (rs >= 0) && (mi < nmsgs) ; mi += 1) {
	    rs = mbcache_msgtimers(op,mi,nullptr) ;
	} /* end for */

	if (rs >= 0) {
	    cint	qsize = sizeof(cchar *) ;
	    void	*msgs = (void *) op->msgs ;
	    qsort(msgs,nmsgs,qsize,vcmpmsgentry) ;
	}

	return (rs >= 0) ? nmsgs : rs ;
}
/* end subroutine (mbcache_sort) */


#ifdef	COMMENT

/* check whatever */
int mbcache_check(MBCACHE *op,TIMEB *nowp,cchar *zname)
{
	int		rs = SR_OK ;

	if (op == nullptr) return SR_FAULT ;
	if (nowp == nullptr) return SR_FAULT ;

	if (op->magic != MBCACHE_MAGIC) return SR_NOTOPEN ;

	op->now = *nowp ;
	strncpy(op->zname,zname,DATER_ZNAMELEN) ;

	op->f.now = true ;

/* do anything ? */

	if (! op->f.tmpdate) {
	    op->f.tmpdate = true ;
	    rs = dater_start(&op->tmpdate,&op->now,op->zname,-1) ;
	} else {
	    op->tmpdate.cb = *nowp ;
	    strncpy(op->tmpdate.cname,zname,DATER_ZNAMELEN) ;
	} /* end if */

	return rs ;
}
/* end subroutine (mbcache_check) */

#endif /* COMMENT */


/* mark a message for deletion */
int mbcache_msgdel(MBCACHE *op,int mi,int delcmd)
{
	int		rs = SR_OK ;
	int		nmsgs ;
	int		f_delprev = false ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != MBCACHE_MAGIC) return SR_NOTOPEN ;

	nmsgs = op->mbi.nmsgs ;
	if ((mi < 0) || (mi >= nmsgs)) return SR_NOMSG ;

	if ((delcmd >= 0) && op->f.readonly)
	    return SR_ROFS ;

/* if this is only an inquiry, we may have a short cut (not really valid) */

	if ((delcmd >= 0) || (op->msgs[mi] != nullptr)) {
	    MBCACHE_SCAN	*mep ;
	    if ((rs = mbcache_msgframing(op,mi,&mep)) >= 0) {
	        f_delprev = mep->f.del ;
	        if (delcmd >= 0) {
	            if (! LEQUIV(mep->f.del,delcmd)) {
	                mep->f.del = (delcmd) ? true : false ;
	                rs = mailbox_msgdel(op->mbp,mi,delcmd) ;
	            }
	        } /* end if */
	    } /* end if */
	} /* end if (short cut) */

	return (rs >= 0) ? f_delprev : rs ;
}
/* end subroutine (mbcache_msgdel) */


/* delete all duplicate messages */
int mbcache_msgdeldup(MBCACHE *op)
{
	MAPSTRINT	mm ;
	int		rs ;
	int		nmsgs ;
	int		c = 0 ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != MBCACHE_MAGIC) return SR_NOTOPEN ;

	nmsgs = op->mbi.nmsgs ;

	if ((rs = mapstrint_start(&mm,nmsgs)) >= 0) {
	    MBCACHE_SCAN	*mep ;
	    cint		vi = mbcachemf_hdrmid ;
	    int			mi ;
	    int			midl ;
	    cchar		*midp ;
	    for (mi = 0 ; mi < nmsgs ; mi += 1) {
	        if ((rs = mbcache_msginfo(op,mi,&mep)) >= 0) {
		    if (! mep->f.del) {
	                midp = mep->vs[vi] ;
	                midl = mep->vl[vi] ;
		        if ((midp != nullptr) && (midl > 0)) {
		            if ((rs = mapstrint_already(&mm,midp,midl)) >= 0) {
			        c += 1 ;
	                        mep->f.del = true ;
	                        rs = mailbox_msgdel(op->mbp,mi,true) ;
		            } else if (rs == SR_NOTFOUND) {
		                rs = mapstrint_add(&mm,midp,midl,mi) ;
		            }
		        } /* end if (non-null) */
		    } /* end if (not already being deleted) */
	        } /* end (_msginfo) */
		if (rs < 0) break ;
	    } /* end for */
	    mapstrint_finish(&mm) ;
	} /* end if (mapstrint) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (mbcache_msgdeldup) */


/* retrieve MSG flags (dispositions) */
int mbcache_msgflags(MBCACHE *op,int mi)
{
	MBCACHE_SCAN	*mep ;
	int		rs ;
	int		nmsgs ;
	int		mf = 0 ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != MBCACHE_MAGIC) return SR_NOTOPEN ;

	nmsgs = op->mbi.nmsgs ;
	if ((mi < 0) || (mi >= nmsgs)) return SR_NOMSG ;

	if ((rs = mbcache_msgframing(op,mi,&mep)) >= 0) {
	    if (mep->f.read) mf |= MBCACHE_MFMREAD ;
	    if (mep->f.del) mf |= MBCACHE_MFMDEL ;
	    if (mep->f.spam) mf |= MBCACHE_MFMSPAM ;
	    if (mep->f.trash) mf |= MBCACHE_MFMTRASH ;
	} /* end if */

	return (rs >= 0) ? mf : rs ;
}
/* end subroutine (mbcache_msgflags) */


/* set MSG flags (dispositions) */
int mbcache_msgsetflag(MBCACHE *op,int mi,int w,int v)
{
	MBCACHE_SCAN	*mep ;
	int		rs ;
	int		nmsgs ;
	int		mf = 0 ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != MBCACHE_MAGIC) return SR_NOTOPEN ;

	nmsgs = op->mbi.nmsgs ;
	if ((mi < 0) || (mi >= nmsgs)) return SR_NOMSG ;

	if ((rs = mbcache_msgframing(op,mi,&mep)) >= 0) {
	    switch (w) {
	    case MBCACHE_MFVREAD:
	        mf = mep->f.read ;
	        mep->f.read = (v>0) ;
	        break ;
	    case MBCACHE_MFVDEL:
	        mf = mep->f.del ;
	        mep->f.del = (v>0) ;
	        break ;
	    case MBCACHE_MFVSPAM:
	        mf = mep->f.spam ;
	        mep->f.spam = (v>0) ;
	        break ;
	    case MBCACHE_MFVTRASH:
	        mf = mep->f.trash ;
	        mep->f.trash = (v>0) ;
	        break ;
	    } /* end switch */
	} /* end if */

	return (rs >= 0) ? mf : rs ;
}
/* end subroutine (mbcache_msgsetflag) */


/* get count of deleted messages */
int mbcache_countdel(MBCACHE *op)
{
	int		rs ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != MBCACHE_MAGIC) return SR_NOTOPEN ;

	rs = mailbox_countdel(op->mbp) ;

	return rs ;
}
/* end subroutine (mbcache_countdel) */


int mbcache_msgsetlines(MBCACHE *op,int mi,int vlines)
{
	MBCACHE_SCAN	*mep ;
	int		rs ;
	int		rlines = 0 ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != MBCACHE_MAGIC) return SR_NOTOPEN ;

	if ((rs = mbcache_msgframing(op,mi,&mep)) >= 0) {
	    if (vlines >= 0) {
	        if (mep->vlines < 0) {
	            mep->vlines = vlines ;
		}
	        rlines = mep->vlines ;
	    } else {
	        if (mep->vlines >= 0) {
	            rlines = mep->vlines ;
	        } else if (mep->nlines >= 0) {
	            rlines = mep->nlines ;
		}
	    }
	} /* end if (mbcache_msgframing) */

	return (rs >= 0) ? rlines : rs ;
}
/* end subroutine (mbcache_msgsetlines) */


/* get the file offset to the start-envelope of a message */
int mbcache_msgoff(MBCACHE *op,int mi,off_t *rp)
{
	MBCACHE_SCAN	*mep ;
	int		rs = SR_OK ;
	int		nmsgs ;

	if (op == nullptr) return SR_FAULT ;
	if (rp == nullptr) return SR_FAULT ;

	if (op->magic != MBCACHE_MAGIC) return SR_NOTOPEN ;

	nmsgs = op->mbi.nmsgs ;
	if ((mi < 0) || (mi >= nmsgs)) return SR_NOMSG ;

	mep = op->msgs[mi] ;
	if (mep == nullptr) {
	    rs = mbcache_msgframing(op,mi,nullptr) ;
	    mep = op->msgs[mi] ;
	}

	if (rp != nullptr)
	    *rp = (rs >= 0) ? mep->moff : 0 ;

	if (rs >= 0) rs = (mep->mlen & INT_MAX) ;

	return rs ;
}
/* end subroutine (mbcache_msgoff) */


/* get the number of lines in a message */
int mbcache_msglines(MBCACHE *op,int mi,int *rp)
{
	MBCACHE_SCAN	*mep ;
	int		rs = SR_OK ;
	int		nmsgs ;
	int		rlines = -1 ;

	if (op == nullptr) return SR_FAULT ;
	if (rp == nullptr) return SR_FAULT ;

	if (op->magic != MBCACHE_MAGIC) return SR_NOTOPEN ;

	nmsgs = op->mbi.nmsgs ;
	if ((mi < 0) || (mi >= nmsgs)) return SR_NOMSG ;

	mep = op->msgs[mi] ;
	if (mep == nullptr) {
	    rs = mbcache_msgframing(op,mi,nullptr) ;
	    mep = op->msgs[mi] ;
	}

	if (rs >= 0) {
	    if (mep->vlines >= 0) {
	        rlines = mep->vlines ;
	    } else if (mep->nlines >= 0) {
	        rlines = mep->nlines ;
	    }
	}

	*rp = rlines ;
	return rs ;
}
/* end subroutine (mbcache_msglines) */


/* get the framing information for a message */
int mbcache_msginfo(MBCACHE *op,int mi,MBCACHE_SCAN **mpp)
{
	MBCACHE_SCAN	*mep ;
	int		rs = SR_OK ;
	int		nmsgs ;

	if (op == nullptr) return SR_FAULT ;
	if (mpp == nullptr) return SR_FAULT ;

	if (op->magic != MBCACHE_MAGIC) return SR_NOTOPEN ;

	if (mpp != nullptr) *mpp = nullptr ;
	nmsgs = op->mbi.nmsgs ;
	if ((mi < 0) || (mi >= nmsgs)) return SR_NOMSG ;

	mep = op->msgs[mi] ;
	if (mep == nullptr) {
	    rs = mbcache_msgframing(op,mi,nullptr) ;
	    mep = op->msgs[mi] ;
	}

	if (rs >= 0) {
	    if ((rs = msgentry_load(mep,op)) >= 0) {
	        if (mpp != nullptr) *mpp = mep ;
	    }
	}

	return rs ;
}
/* end subroutine (mbcache_msginfo) */


/* get the scan information for a message */
int mbcache_msgscan(MBCACHE *op,int mi,MBCACHE_SCAN **mpp)
{
	int		rs ;

	if (op == nullptr) return SR_FAULT ;
	if (mpp == nullptr) return SR_FAULT ;

	if (op->magic != MBCACHE_MAGIC) return SR_NOTOPEN ;

	*mpp = nullptr ;
	if ((rs = mbcache_msginfo(op,mi,mpp)) >= 0) {
	    MBCACHE_SCAN	*mep = *mpp ;
	    if ((rs = msgentry_procscanfrom(mep,op)) >= 0) {
	        rs = msgentry_procscandate(mep,op) ;
	    }
	}

	return rs ;
}
/* end subroutine (mbcache_msgscan) */


int mbcache_msgenvtime(MBCACHE *op,int mi,time_t *timep)
{
	MSGENTRY	*mep ;
	time_t		t = 0 ;
	int		rs ;
	int		f = false ;

	if (op == nullptr) return SR_FAULT ;
	if (timep == nullptr) return SR_FAULT ;

	if ((rs = mbcache_msginfo(op,mi,&mep)) >= 0) {
	    if ((rs = msgentry_procenvdate(mep,op)) >= 0) {
	        t = mep->etime ;
	        f = true ;
	    }
	}

	if (timep != nullptr) *timep = t ;
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (mbcache_msgenvtime) */


int mbcache_msghdrtime(MBCACHE *op,int mi,time_t *timep)
{
	MSGENTRY	*mep ;
	time_t		t = 0 ;
	int		rs ;
	int		f = false ;

	if (op == nullptr) return SR_FAULT ;
	if (timep == nullptr) return SR_FAULT ;

	if ((rs = mbcache_msginfo(op,mi,&mep)) >= 0) {
	    if ((rs = msgentry_prochdrdate(mep,op)) >= 0) {
	        f = true ;
	        t = mep->htime ;
	    }
	}

	if (timep != nullptr) *timep = t ;
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (mbcache_msghdrtime) */


int mbcache_msgtimes(MBCACHE *op,int mi,time_t *timep)
{
	MSGENTRY	*mep ;
	int		rs ;

	if (op == nullptr) return SR_FAULT ;
	if (timep == nullptr) return SR_FAULT ;

	if ((rs = mbcache_msginfo(op,mi,&mep)) >= 0) {
	    if ((rs = msgentry_msgtimes(mep,op)) >= 0) {
	        timep[0] = mep->etime ;
	        timep[1] = mep->htime ;
	    }
	}

	return rs ;
}
/* end subroutine (mbcache_msgtimes) */


/* private subroutines */

static int mbcache_msgfins(MBCACHE *op) noex {
	MBCACHE_SCAN	*mep ;
	int		rs = SR_OK ;
	int		rs1 ;
	for (int mi = 0 ; mi < op->mbi.nmsgs ; mi += 1) {
	    mep = op->msgs[mi] ;
	    if (mep) {
		{
	            rs1 = msgentry_finish(mep) ;
	            if (rs >= 0) rs = rs1 ;
		}
		{
	            rs1 = uc_free(mep) ;
	            if (rs >= 0) rs = rs1 ;
		}
	        op->msgs[mi] = nullptr ;
	    }
	} /* end for */

	return rs ;
}
/* end subroutine (mbcache_msgfins) */


static int mbcache_msgframing(MBCACHE *op,int mi,MSGENTRY **mpp)
{
	int		rs = SR_OK ;

	if (op->msgs[mi] == nullptr) {
	    MAILBOX_MSGINFO	minfo ;
	    if ((rs = mailbox_msginfo(op->mbp,mi,&minfo)) >= 0) {
	        MBCACHE_SCAN	*mep = nullptr ;
	        cint	size = sizeof(MBCACHE_SCAN) ;

	        if ((rs = uc_malloc(size,&mep)) >= 0) {
	            if ((rs = msgentry_start(mep,mi)) >= 0) {
	                if ((rs = msgentry_frame(mep,&minfo)) >= 0) {
	                    op->msgs[mi] = mep ;
	                }
	                if (rs < 0)
	                    msgentry_finish(mep) ;
	            } /* end if (msgentry_start) */
	            if (rs < 0)
	                uc_free(mep) ;
	        } /* end if (memory-allocation) */

	    } /* end if (mailbox_msginfo) */
	} /* end if (needed) */

	if (mpp != nullptr)
	    *mpp = (rs >= 0) ? op->msgs[mi] : nullptr ;

	return rs ;
}
/* end subroutine (mbcache_msgframing) */


static int mbcache_msgtimers(MBCACHE *op,int mi,time_t *timep)
{
	MSGENTRY	*mep ;
	int		rs ;

	if ((rs = mbcache_msginfo(op,mi,&mep)) >= 0) {
	    if ((rs = msgentry_msgtimes(mep,op)) >= 0) {
	        if (timep != nullptr) {
	            timep[0] = mep->etime ;
	            timep[1] = mep->htime ;
	        }
	    }
	}

	return rs ;
}
/* end subroutine (mbcache_msgtimers) */


#ifdef	COMMENT

static int mbcache_msgscanmk(MBCACHE *op,int mi)
{
	MBCACHE_SCAN	*mep ;
	cint	sl = SCANBUFLEN ;
	int		rs = SR_OK ;

	mep = op->msgs[mi] ;
	if (mep != nullptr) {
	    if (mep->vs[mbcachemf_scanline] == nullptr) {
	        cint	size = (sl + 1) ;
	        char		*bp ;

	        if ((rs = uc_malloc(size,&bp)) >= 0) {
	            int		i ;
	            int		tcol ;
	            int		cl ;
	            cchar	*cp ;

	            strwcpyblanks(bp,sl) ;

	            for (i = 0 ; scantitles[i].name != nullptr ; i += 1) {
	                cp = scantitles[i].name ;
	                cl = strlen(cp) ;
	                tcol = scantitles[i].col ;
	                strncpy((bp + tcol),cp,cl) ;
	            } /* end for */

	            if (rs >= 0) {
	                mep->vs[mbcachemf_scanline] = bp ;
	            } else
	                uc_free(bp) ;

	        } /* end if (memory-allocation) */

	    } /* end if */
	} /* end if */

	return rs ;
}
/* end subroutine (mbcache_msgscanmk) */

#endif /* COMMENT */


#ifdef	COMMENT

static int mbcache_setnow(MBCACHE *op)
{
	TIMEB		*tbp = &op->now ;
	int		rs = SR_OK ;

	if (! op->f.now) {
	    if ((rs = uc_ftime(tbp)) >= 0) {
	        TMTIME	tmt ;
	        int	zo ;
	        if ((rs = tmtime_localtime(&tmt,tbp->time)) >= 0) {
	            tbp->timezone = (tmt.gmtoff / 60) ;
	            tbp->dstflag = tmt.isdst ;
	            strncpy(op->zname,tmt.zname,DATER_ZNAMELEN) ;
	            op->f.now = true ;
	        } /* end if */
	    } /* end if (uc_ftime) */
	} /* end if (have-now) */

	return rs ;
}
/* end subroutine (mbcache_setnow) */

#endif /* COMMENT */


static int msgentry_start(MBCACHE_SCAN *mep,int mi)
{

	memclear(mep) ;
	mep->msgi = mi ;
	mep->nlines = -1 ;
	mep->vlines = -1 ;
	mep->filesize = -1 ;
	mep->moff = -1 ;
	mep->hoff = -1 ;
	mep->boff = -1 ;

	return SR_OK ;
}
/* end subroutine (msgentry_start) */


static int msgentry_finish(MBCACHE_SCAN *mep)
{
	int		rs = SR_OK ;
	int		rs1 ;

	if (mep->f.lineoffs) {
	    mep->f.lineoffs = false ;
	    rs1 = vecint_finish(&mep->lineoffs) ;
	    if (rs >= 0) rs = rs1 ;
	}

	if (mep->fname != nullptr) {
	    rs1 = uc_free(mep->fname) ;
	    if (rs >= 0) rs = rs1 ;
	    mep->fname = nullptr ;
	}

	return rs ;
}
/* end subroutine (msgentry_finish) */


static int msgentry_frame(MSGENTRY *mep,MAILBOX_MSGINFO *mip)
{
	int		rs = SR_OK ;

	if (mep == nullptr) return SR_FAULT ;
	if (mip == nullptr) return SR_FAULT ;

	mep->moff = mip->moff ;
	mep->hoff = mip->hoff ;
	mep->boff = mip->boff ;
	mep->mlen = mip->mlen ;
	mep->hlen = mip->hlen ;
	mep->blen = mip->blen ;

	mep->hdr.clen = mip->hdr.clen ;
	mep->hdr.clines = mip->hdr.clines ;
	mep->hdr.lines = mip->hdr.lines ;
	mep->hdr.xlines = mip->hdr.xlines ;

	mep->hdrval.clen = mip->hdrval.clen ;
	mep->hdrval.clines = mip->hdrval.clines ;
	mep->hdrval.lines = mip->hdrval.lines ;
	mep->hdrval.xlines = mip->hdrval.xlines ;

	if ((mep->nlines < 0) && mip->hdrval.clines)
	    mep->nlines = mip->clines ;

	if ((mep->nlines < 0) && mip->hdrval.lines)
	    mep->nlines = mip->lines ;

	if ((mep->nlines < 0) && mip->hdrval.xlines)
	    mep->nlines = mip->xlines ;

	return rs ;
}
/* end subroutine (msgentry_frame) */


static int msgentry_load(MSGENTRY *mep,MBCACHE *op)
{
	int		rs = SR_OK ;

	if (! mep->f.info) {
	    MAILMSG	m ;
	    mep->f.info = true ;
	    if ((rs = mailmsg_start(&m)) >= 0) {
	        MAILBOX		*mbp = op->mbp ;
	        off_t	mbo = mep->moff ;
	        if ((rs = mailmsg_loadmb(&m,mbp,mbo)) >= 0) {

	            if (rs >= 0) {
	                rs = msgentry_loadenv(mep,op,&m) ;
	            }

/* MESSAGEID: extract the MAILMSG information that we want */

	            if (rs >= 0) {
	                rs = msgentry_loadhdrmid(mep,op,&m) ;
	            }

/* FROM: extract the MAILMSG information that we want */

	            if (rs >= 0) {
	                rs = msgentry_loadhdrfrom(mep,op,&m) ;
	            }

/* SUBJECT: extract the MAILMSG information that we want */

	            if (rs >= 0) {
	                rs = msgentry_loadhdrsubject(mep,op,&m) ;
	            }

/* DATE: extract the MAILMSG information that we want */

	            if (rs >= 0) {
	                rs = msgentry_loadhdrdate(mep,op,&m) ;
	            }

/* STATUS: extract the MAILMSG information that we want */

	            if (rs >= 0) {
	                rs = msgentry_loadhdrstatus(mep,op,&m) ;
	            }

/* set VS bit */

	            if (rs >= 0) mep->f.vs = true ;

	        } /* end if (mailmsg-loadmb) */
	        mailmsg_finish(&m) ;
	    } /* end if (mailmsg) */
	} /* end if (initialized) */

	return rs ;
}
/* end subroutine (msgentry_load) */


static int msgentry_loadenv(MSGENTRY *mep,MBCACHE *op,MAILMSG *mmp)
{
	int		rs ;

	if ((rs = mailmsg_envcount(mmp)) >= 0) {
	    MAILMSG_ENVER	e ;
	    strpack		*psp = &op->strs ;
	    cint		n = rs ;
	    if ((rs = mailmsg_enver(mmp,(n-1),&e)) >= 0) {
	        int		i ;
	        int		vl = -1 ;
	        int		*vlp ;
	        cchar	*vp ;
	        cchar	**vpp ;
	        for (i = 0 ; (rs >= 0) && (i < 3) ; i += 1) {
	            vp = nullptr ;
	            switch (i) {
	            case 0:
	                vpp = (mep->vs + mbcachemf_envaddr) ;
	                vlp = (mep->vl + mbcachemf_envaddr) ;
	                vp = e.a.ep ;
	                vl = e.a.el ;
	                break ;
	            case 1:
	                vpp = (mep->vs + mbcachemf_envdate) ;
	                vlp = (mep->vl + mbcachemf_envdate) ;
	                vp = e.d.ep ;
	                vl = e.d.el ;
	                break ;
	            case 2:
	                vpp = (mep->vs + mbcachemf_envremote) ;
	                vlp = (mep->vl + mbcachemf_envremote) ;
	                vp = e.r.ep ;
	                vl = e.r.el ;
	                break ;
	            } /* end switch */
	            if (vp != nullptr) {
	                rs = strpack_store(psp,vp,vl,vpp) ;
	                *vlp = rs ;
	            }
	        } /* end for */
	    } else if (isNoMsg(rs)) {
	        rs = SR_OK ;
	    }
	} /* end if (mailmsg_envcount) */

	return rs ;
}
/* end subroutine (msgentry_loadenv) */


#if	CF_LOADENVADDR
static int msgentry_loadenvaddr(mep,op,mmp,rpp)
MSGENTRY	*mep ;
MBCACHE		*op ;
MAILMSG		*mmp ;
cchar	**rpp ;
{
	int		rs = SR_OK ;
	int		len = 0 ;

	if (mep->vs[mbcachemf_envaddr] == nullptr) {
	    rs = msgentry_loadenv(mep,op,mmp) ;
	}
	if ((rs >= 0) && (rpp != nullptr)) {
	    cint	vi = mbcachemf_envaddr ;
	    *rpp = mep->vs[vi] ;
	    len = mep->vl[vi] ;
	} else if (isNoMsg(rs)) {
	    rs = SR_OK ;
	}

	return (rs >= 0) ? len : rs ;
}
/* end subroutine (msgentry_loadenvaddr) */
#endif /* CF_LOADENVADDR */


static int msgentry_loadhdrmid(MSGENTRY *mep,MBCACHE *op,MAILMSG *mmp)
{
	int		rs ;
	int		sl = 0 ;
	cchar	*hdr = HN_MESSAGEID ;
	cchar	*sp ;

	if ((rs = mailmsg_hdrval(mmp,hdr,&sp)) >= 0) {
	    strpack	*psp = &op->strs ;
	    cint	hlen = HDRBUFLEN ;
	    char	hbuf[HDRBUFLEN+1] ;
	    if ((rs = mkbestaddr(hbuf,hlen,sp,rs)) >= 0) {
	        cint	vi = mbcachemf_hdrmid ;
	        cchar	**rpp ;
	        sl = rs ;
	        rpp = (mep->vs + vi) ;
	        mep->vl[vi] = sl ;
	        rs = strpack_store(psp,hbuf,sl,rpp) ;
	    } /* end if (mkbestaddr) */
	} else if (isNoMsg(rs)) {
	    rs = SR_OK ;
	}

	return (rs >= 0) ? sl : rs ;
}
/* end subroutine (msgentry_loadhdrmid) */


static int msgentry_loadhdrfrom(MSGENTRY *mep,MBCACHE *op,MAILMSG *mmp)
{
	int		rs ;
	int		vl = 0 ;
	cchar		*hdr = HN_FROM ;
	cchar		*vp ;

	if ((rs = mailmsg_hdrval(mmp,hdr,&vp)) > 0) {
	    vl = rs ;
	} else if ((rs == 0) || isNoMsg(rs)) {
	    hdr = HN_RETURNPATH ;
	    if ((rs = mailmsg_hdrval(mmp,hdr,&vp)) > 0) {
	        vl = rs ;
	    }
	}
	if ((rs >= 0) && (vl >= 0)) {
	    strpack	*psp = &op->strs ;
	    cint	vi = mbcachemf_hdrfrom ;
	    cchar	**rpp ;
	    rpp = (mep->vs + vi) ;
	    mep->vl[vi] = vl ;
	    rs = strpack_store(psp,vp,vl,rpp) ;
	} else if (isNoMsg(rs)) {
	    rs = SR_OK ;
	}

	return (rs >= 0) ? vl : rs ;
}
/* end subroutine (msgentry_loadhdrfrom) */


static int msgentry_loadhdrsubject(MSGENTRY *mep,MBCACHE *op,MAILMSG *mmp)
{
	int		rs ;
	int		sl = 0 ;
	cchar	*hdr = HN_SUBJECT ;
	cchar	*sp ;

	if ((rs = mailmsg_hdrval(mmp,hdr,&sp)) >= 0) {
	    strpack	*psp = &op->strs ;
	    cint	vi = mbcachemf_hdrsubject ;
	    cchar	**rpp ;
	    sl = rs ;
	    rpp = (mep->vs + vi) ;
	    mep->vl[vi] = sl ;
	    rs = strpack_store(psp,sp,sl,rpp) ;
	} else if (isNoMsg(rs)) {
	    rs = SR_OK ;
	}

	return (rs >= 0) ? sl : rs ;
}
/* end subroutine (msgentry_loadhdrsubject) */


static int msgentry_loadhdrdate(MSGENTRY *mep,MBCACHE *op,MAILMSG *mmp)
{
	int		rs ;
	int		sl = 0 ;
	cchar	*hdr = HN_DATE ;
	cchar	*sp ;

	if ((rs = mailmsg_hdrval(mmp,hdr,&sp)) >= 0) {
	    strpack	*psp = &op->strs ;
	    cint	vi = mbcachemf_hdrdate ;
	    cchar	**rpp ;
	    sl = rs ;
	    mep->vl[vi] = sl ;
	    rpp = (mep->vs + vi) ;
	    rs = strpack_store(psp,sp,sl,rpp) ;
	} else if (isNoMsg(rs)) {
	    rs = SR_OK ;
	}

	return (rs >= 0) ? sl : rs ;
}
/* end subroutine (msgentry_loadhdrdate) */


static int msgentry_loadhdrstatus(MSGENTRY *mep,MBCACHE *op,MAILMSG *mmp)
{
	int		rs ;
	int		sl = 0 ;
	cchar	*hdr = HN_STATUS ;
	cchar	*sp ;

	if ((rs = mailmsg_hdrval(mmp,hdr,&sp)) >= 0) {
	    strpack	*psp = &op->strs ;
	    cint	vi = mbcachemf_hdrstatus ;
	    cchar	**rpp ;
	    sl = rs ;
	    rpp = (mep->vs + vi) ;
	    mep->vl[vi] = sl ;
	    rs = strpack_store(psp,sp,sl,rpp) ;
	} else if (isNoMsg(rs)) {
	    rs = SR_OK ;
	}

	return (rs >= 0) ? sl : rs ;
}
/* end subroutine (msgentry_loadhdrstatus) */


static int msgentry_procenvdate(MSGENTRY *mep,MBCACHE *op)
{
	cint	vi = mbcachemf_envdate ;
	int		rs = SR_OK ;
	int		f = false ;

	if (! mep->proc.edate) {
	    int		sl = mep->vl[vi] ;
	    cchar	*sp = mep->vs[vi] ;
	    mep->proc.edate = true ;
	    if ((sp != nullptr) && (sl > 0)) {
	        dater	*dp = &op->dm ;
	        if ((rs = dater_setstd(dp,sp,sl)) >= 0) {
	            DATE	*dop = &mep->edate ;
	            time_t	t = 0 ;
	            if ((rs = dater_getdate(dp,dop)) >= 0) {
	                mep->f.edate = true ;
	                dater_gettime(dp,&t) ;
	                mep->etime = t ;
	                f = true ;
	            }
	        } else if (isBadDate(rs)) {
	            rs = SR_OK ;
	        }
	    } /* end if (had ENV value) */
	} /* end if (processing) */

	return (rs >= 0) ? f : rs ;
}
/* end subroutine (msgentry_procenvdate) */


static int msgentry_prochdrdate(MSGENTRY *mep,MBCACHE *op)
{
	int		rs = SR_OK ;
	int		f = false ;

	if (! mep->proc.hdate) {
	    cint	vi = mbcachemf_hdrdate ;
	    int		sl = mep->vl[vi] ;
	    cchar	*sp = mep->vs[vi] ;
	    mep->proc.hdate = true ;
	    if ((sp != nullptr) && (sl > 0)) {
	        dater	*dp = &op->dm ;
	        if ((rs = dater_setmsg(dp,sp,sl)) >= 0) {
	            DATE	*dop = &mep->hdate ;
	            time_t	t = 0 ;
	            if ((rs = dater_getdate(dp,dop)) >= 0) {
	                mep->f.hdate = true ;
	                dater_gettime(dp,&t) ;
	                mep->htime = t ;
	                f = true ;
	            }
	        } else if (isBadDate(rs)) {
	            rs = SR_OK ;
	        }
	    } /* end if (had HDR value) */
	} /* end if (processing) */

	return (rs >= 0) ? f : rs ;
}
/* end subroutine (msgentry_prochdrdate) */


static int msgentry_procscanfrom(MSGENTRY *mep,MBCACHE *op)
{
	int		rs = SR_OK ;
	int		len = 0 ;

	if (! mep->proc.scanfrom) {
	    int		sl = mep->vl[mbcachemf_hdrfrom] ;
	    cchar	*sp = mep->vs[mbcachemf_hdrfrom] ;
	    mep->proc.scanfrom = true ;
	    if ((sp == nullptr) || (sl == 0)) {
	        sl = mep->vl[mbcachemf_envaddr] ;
	        sp = mep->vs[mbcachemf_envaddr] ;
	    }
	    if ((sp != nullptr) && (sl > 0)) {
	        strpack		*psp = &op->strs ;
	        cint	hlen = HDRBUFLEN ;
	        char		hbuf[HDRBUFLEN+1] ;
	        if ((rs = mkaddrname(hbuf,hlen,sp,sl)) >= 0) {
	            cint	vi = mbcachemf_scanfrom ;
	            cchar	**rpp ;
	            len = rs ;
	            rpp = (mep->vs + vi) ;
	            mep->vl[vi] = len ;
	            if ((rs = strpack_store(psp,hbuf,len,rpp)) >= 0) {
	                mep->f.scanfrom = true ;
	            }
	        }
	    } /* end if (positive) */
	} else if (mep->vs[mbcachemf_scanfrom] != nullptr) {
	    len = mep->vl[mbcachemf_scanfrom] ;
	}

	return (rs >= 0) ? len : rs ;
}
/* end subroutine (msgentry_procscanfrom) */


static int msgentry_procscandate(MSGENTRY *mep,MBCACHE *op)
{
	int		rs = SR_OK ;
	int		cl = 0 ;

	if (! mep->proc.scandate) {
	    mep->proc.scandate = true ;
	    if ((rs = msgentry_prochdrdate(mep,op)) >= 0) {
	        time_t	t = mep->htime ;
	        if (t == 0) {
	            if ((rs = msgentry_procenvdate(mep,op)) >= 0) {
	                t = mep->etime ;
	            }
	        }
	        if ((rs >= 0) && (t > 0)) {
	            cchar	*ts ;
	            char	timebuf[TIMEBUFLEN + 1] ;
	            if ((ts = timestr_scandate(t,timebuf)) != nullptr) {
	                strpack	*psp = &op->strs ;
	                cchar	*cp ;
	                if ((rs = strpack_store(psp,ts,-1,&cp)) >= 0) {
	                    cl = rs ;
	                    mep->f.scandate = true ;
	                    mep->vs[mbcachemf_scandate] = cp ;
	                    mep->vl[mbcachemf_scandate] = cl ;
	                }
	            }
	        }
	    } /* end if */
	} else if (mep->vs[mbcachemf_scandate] != nullptr) {
	    cl = mep->vl[mbcachemf_scandate] ;
	}

	return (rs >= 0) ? cl : rs ;
}
/* end subroutine (msgentry_procscandate) */


static int msgentry_msgtimes(MSGENTRY *mep,MBCACHE *op)
{
	int		rs ;
	int		c = 0 ;

	if ((rs = msgentry_procenvdate(mep,op)) >= 0) {
	    c += rs ;
	    if ((rs = msgentry_prochdrdate(mep,op)) >= 0) {
	        c += rs ;
	    }
	}

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (msgentry_msgtimes) */


static int isBadDate(int rs)
{
	return isOneOf(rsdatebad,rs) ;
}
/* end subroutine (isBadDate) */


static int isNoMsg(int rs)
{
	return isOneOf(rsnomsg,rs) ;
}
/* end subroutine (isNoMsg) */


static int vcmpmsgentry(const void *e1pp,const void *e2pp)
{
	MSGENTRY	**m1pp = (MSGENTRY **) e1pp ;
	MSGENTRY	**m2pp = (MSGENTRY **) e2pp ;
	MSGENTRY	*m1p, *m2p ;
	int		rc = 0 ;

	m1p = (*m1pp) ;
	m2p = (*m2pp) ;

	if ((m1p != nullptr) || (m2p != nullptr)) {
	    if ((rc == 0) && (m1p == nullptr)) rc = 1 ;
	    if ((rc == 0) && (m2p == nullptr)) rc = -1 ;
	    if (rc == 0) {
	        time_t	t1e = m1p->etime ;
	        time_t	t2e = m2p->etime ;
	        time_t	t1h = m1p->htime ;
	        time_t	t2h = m2p->htime ;
	        if (t1h == 0) t1h = t1e ;
	        if (t2h == 0) t2h = t2e ;
	        rc = (t1h - t2h) ;
	        if (rc == 0) rc = (t1e - t2e) ;
	    }
	}

	return rc ;
}
/* end subroutine (vcmpmsgentry) */


#ifdef	COMMENT
/* append some additional value to an existing header value */
static int headappend(cchar **pp,cchar *sp,int sl)
{
	int		rs ;
	int		cl, cl2 ;
	char		*cp, *cp2 ;

	if (*pp == nullptr)
	    return 0 ;

	cl2 = strlen(*pp) ;

	cl = sl + cl2 + 2 ;
	if ((rs = uc_malloc(cl,&cp)) >= 0) {

	    cp2 = strwcpy(cp,*pp,cl2) ;

	    *cp2++ = ',' ;
	    *cp2++ = ' ' ;
	    strwcpy(cp2,sp,sl) ;

	    uc_free(*pp) ;

	    *pp = cp ;

	} /* end if (memory-allocation) */

	return (rs >= 0) ? cl : rs ;
}
/* end subroutine (headappend) */
#endif /* COMMENT */

#ifdef	COMMENT
/* is the next non-white-space character a colon */
static int nextnonwhite(cchar *sp,int sl)
{
	int		i = 0 ;
	while (CHAR_ISWHITE(*sp) && (i++ < sl)) {
	    sp += 1 ;
	}
	return (i < sl) ? *sp : ' ' ;
}
/* end subroutine (nextnonwhite) */
#endif /* COMMENT */


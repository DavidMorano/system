/* mbcache SUPPORT (MailBox-Cache) */
/* charset=ISO8859-1 */
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

  	Object:
	mbcache

	Description:
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
#include	<unistd.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<new>			/* |nothrow(3c++)| */
#include	<usystem.h>
#include	<getbufsize.h>
#include	<bfile.h>
#include	<sbuf.h>
#include	<char.h>
#include	<mailmsg.h>
#include	<mailmsg_envget.h>
#include	<mailmsghdrs.h>
#include	<ema.h>
#include	<vecobj.h>
#include	<tmtime.hh>
#include	<mapstrint.h>
#include	<snx.h>
#include	<mkx.h>
#include	<strwcpy.h>
#include	<timestr.h>
#include	<intsat.h>
#include	<isoneof.h>
#include	<localmisc.h>		/* |DIGBUFLEN| */

#include	"mailbox.h"
#include	"mbcache.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |getlenstr(3u)| */

/* local defines */

#define	MC		mbcache
#define	ME		mbcache_scan

#define	HDRBUFMULT	4		/* only four?! */

#ifdef	SCANBUFLEN
#define	SCANBUFLEN	100
#endif

#define	RECORDFNAME	"envelopes.log"

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


/* imported namespaces */

using libuc::libmem ;			/* variable */
using std::nothrow ;			/* constant */


/* local typedefs */

typedef mbcache_scan **		mepp ;


/* external subroutines */


/* external variables */


/* local structures */

struct scantitle {
	char		*name ;
	int		col ;
} ;

namespace {
    struct vars {
	int		hdrbuflen ;
	operator int () noex ;
    } ; /* end struct (vars) */
}


/* forward references */

template<typename ... Args>
local int mbcache_ctor(mbcache *op,Args ... args) noex {
    	MBCACHE		*hop = op ;
	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    memclear(hop) ;
	    rs = SR_NOMEM ;
	    if ((op->mip = new(nothrow) mailbox_info) != np) {
	        if ((op->spp = new(nothrow) strpack) != np) {
	            if ((op->dmp = new(nothrow) dater) != np) {
			rs = SR_OK ;
		    }
		    if (rs < 0) {
		        delete op->spp ;
		        op->spp = nullptr ;
		    }
		} /* end if (new-strpack) */
		if (rs < 0) {
		    delete op->mip ;
		    op->mip = nullptr ;
		}
	    } /* end if (new-mailbox_info) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mbcache_ctor) */

local int mbcache_dtor(mbcache *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->dmp) {
		delete op->dmp ;
		op->dmp = nullptr ;
	    }
	    if (op->spp) {
		delete op->spp ;
		op->spp = nullptr ;
	    }
	    if (op->mip) {
		delete op->mip ;
		op->mip = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mbcache_dtor) */

template<typename ... Args>
local int mbcache_magic(mbcache *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == MBCACHE_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (mbcache_magic) */

local int mbcache_starter(mbcache *,cc *) noex ;
local int mbcache_msgfins(mbcache *) noex ;
local int mbcache_msgframing(mbcache *,int,ME **) noex ;
local int mbcache_msgtimers(mbcache *,int,time_t *) noex ;

#ifdef	COMMENT
local int mbcache_msgscanmk(mbcache *,int) noex ;
#endif

local int msgent_start(ME *,int) noex ;
local int msgent_frame(ME *,mailbox_mi *) noex ;
local int msgent_load(ME *,mbcache *) noex ;
local int msgent_loadenv(ME *,mbcache *,mailmsg *) noex ;
local int msgent_loadhdrmid(ME *,mbcache *,mailmsg *) noex ;
local int msgent_loadhdrfrom(ME *,mbcache *,mailmsg *) noex ;
local int msgent_loadhdrsubj(ME *,mbcache *,mailmsg *) noex ;
local int msgent_loadhdrdate(ME *,mbcache *,mailmsg *) noex ;
local int msgent_loadhdrstatus(ME *,mbcache *,mailmsg *) noex ;
local int msgent_procenvdate(ME *,mbcache *) noex ;
local int msgent_prochdrdate(ME *,mbcache *) noex ;
local int msgent_msgtimes(ME *,mbcache *) noex ;
local int msgent_procscanfrom(ME *,mbcache *) noex ;
local int msgent_procscandate(ME *,mbcache *) noex ;
local int msgent_finish(ME *) noex ;

#if	CF_LOADENVADDR
local int msgent_loadenvaddr(ME *,mbcache *,mailmsg *,cchar **) noex ;
#endif /* CF_LOADENVADDR */

#ifdef	COMMENT
local int	headappend(char **,char *,int) noex ;
#endif

static bool	isNoMsg(int) noex ;
static bool	isBadDate(int) noex ;

extern "C" {
    local int	vcmpmsgentry(cvoid *,cvoid *) noex ;
}


/* local variables */

constexpr int	rsdatebad[] = {
	SR_INVALID,
	SR_DOM,
	SR_NOMSG,
	0
} ;

constexpr int	rsnomsg[] = {
	SR_NOMSG,
	SR_NOENT,
	0
} ;


#ifdef	COMMENT
constexpr scantitle	scantitles[] = {
	{ "FROM",	COL_SCANFROM },
	{ "SUBJECT",	COL_SCANSUBJECT },
	{ "DATE",	COL_SCANDATE },
	{ "LINES",	COL_SCANLINES },
	{ nullptr, 0 }
} ;
#endif /* COMMENT */

static vars		var ;


/* exported variables */


/* exported subroutines */

int mbcache_start(mbcache *op,cchar *mbfname,int mflags,mailbox *mbp) noex {
	int		rs ;
	int		nmsgs = 0 ;
	if ((rs = mbcache_ctor(op,mbfname,mbp)) >= 0) {
	    rs = SR_INVALID ;
	    if (mbfname[0]) {
		static cint	rsv = var ;
	        op->mflags = mflags ;
	        op->mbp = mbp ;
	        op->fl.readonly = (! (mflags & MBCACHE_ORDWR)) ;
		if ((rs = rsv) >= 0) {
		    rs = mbcache_starter(op,mbfname) ;
		    nmsgs = rs ;
		}
	    } /* end if (valid) */
	    if (rs < 0) {
		mbcache_dtor(op) ;
	    }
	} /* end if (mbcache_ctor) */
	return (rs >= 0) ? nmsgs : rs ;
}
/* end subroutine (mbcache_start) */

local int mbcache_starter(mbcache *op,cc *mbfname) noex {
	cnullptr	np{} ;
	mailbox		*mbp = op->mbp ;
	strpack		*psp = op->spp ;
	int		rs ;
	int		nmsgs = 0 ;
	if (cchar *cp ; (rs = libmem.strw(mbfname,-1,&cp)) >= 0) {
	    mailbox_info	*mip = op->mip ;
	    op->mbfname = cp ;
	    if ((rs = mailbox_getinfo(mbp,mip)) >= 0) {
	        cint	mssize = szof(ME **) ;
	        if (mip->nmsgs >= 0) {
	            cint	sz = ((mip->nmsgs + 1) * mssize) ;
	            nmsgs = mip->nmsgs ;
	            if (void *vp ; (rs = libmem.mall(sz,&vp)) >= 0) {
	                cint	strsz = size_t(mip->nmsgs * 6 * 20) ;
	                op->msgs = mepp(vp) ;
	                memset(op->msgs,0,sz) ;
	                if ((rs = strpack_start(psp,strsz)) >= 0) {
			   dater	*dp = op->dmp ;
			    if ((rs = dater_start(dp,np,np,0)) >= 0) {
	    			op->magic = MBCACHE_MAGIC ;
			    }
	                    if (rs < 0) {
	                        strpack_finish(psp) ;
			    }
	                } /* end if (strpack_start) */
	                if (rs < 0) {
	                    libmem.free(op->msgs) ;
	                    op->msgs = nullptr ;
	                }
	            } /* end if (memory-allocation) */
	        } else {
	            rs = SR_NOANODE ;
		}
	    }
	    if (rs < 0) {
		void *vp = voidp(op->mbfname) ;
	        libmem.free(vp) ;
	        op->mbfname = nullptr ;
	    } /* end if (error) */
	} /* end if (memory-allocation) */
	return (rs >= 0) ? nmsgs : rs ;
}
/* end subroutine (mbcache_starter) */

int mbcache_finish(mbcache *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = mbcache_magic(op)) >= 0) {
	    {
	        rs1 = mbcache_msgfins(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = dater_finish(op->dmp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = strpack_finish(op->spp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->msgs) {
	        rs1 = libmem.free(op->msgs) ;
	        if (rs >= 0) rs = rs1 ;
	        op->msgs = nullptr ;
	    }
	    if (op->mbfname) {
		void *vp = voidp(op->mbfname) ;
	        rs1 = libmem.free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        op->mbfname = nullptr ;
	    }
	    {
		rs1 = mbcache_dtor(op) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    op->mbp = nullptr ;
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (mbcache_finish) */

int mbcache_mbfile(mbcache *op,char *dbuf,int dlen) noex {
	int		rs ;
	if ((rs = mbcache_magic(op,dbuf)) >= 0) {
	    rs = sncpy1(dbuf,dlen,op->mbfname) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (mbcache_mbfile) */

int mbcache_mbinfo(mbcache *op,MBCACHE_INFO *mep) noex {
	int		rs ;
	int		nmsgs = 0 ;
	if ((rs = mbcache_magic(op,mep)) >= 0) {
	    mailbox_info	*mip = op->mip ;
	    memclear(mep) ;
	    rs = mailbox_countdel(op->mbp) ;
	    mep->nmsgdels = rs ;
	    mep->nmsgs = mip->nmsgs ;
	    nmsgs = mip->nmsgs ;
	} /* end if (magic) */
	return (rs >= 0) ? nmsgs : rs ;
}
/* end subroutine (mbcache_mbinfo) */

int mbcache_count(mbcache *op) noex {
	int		rs ;
	int		nmsgs = 0 ;
	if ((rs = mbcache_magic(op)) >= 0) {
	    mailbox_info	*mip = op->mip ;
	    nmsgs = mip->nmsgs ;
	} /* end if (magic) */
	return (rs >= 0) ? nmsgs : rs ;
}
/* end subroutine (mbcache_count) */

int mbcache_sort(mbcache *op) noex {
	int		rs ;
	int		nmsgs = 0 ;
	if ((rs = mbcache_magic(op)) >= 0) {
	    mailbox_info	*mip = op->mip ;
	    nmsgs = mip->nmsgs ;
	    for (int mi = 0 ; (rs >= 0) && (mi < nmsgs) ; mi += 1) {
	        rs = mbcache_msgtimers(op,mi,nullptr) ;
	    } /* end for */
	    if (rs >= 0) {
	        cint	qsize = szof(cchar *) ;
	        void	*msgs = voidp(op->msgs) ;
	        qsort(msgs,nmsgs,qsize,vcmpmsgentry) ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? nmsgs : rs ;
}
/* end subroutine (mbcache_sort) */

#ifdef	COMMENT
int mbcache_check(mbcache *op,TIMEB *nowp,cchar *zname) noex {
	int		rs ;
	if ((rs = mbcache_magic(op,nowp,zname)) >= 0) {
	    op->now = *nowp ;
	    strncpy(op->zname,zname,DATER_ZNAMELEN) ;
	    op->fl.now = true ;
	    if (! op->fl.tmpdate) {
	        op->fl.tmpdate = true ;
	        rs = dater_start(&op->tmpdate,&op->now,op->zname,-1) ;
	    } else {
	        op->tmpdate.cb = *nowp ;
	        strncpy(op->tmpdate.cname,zname,DATER_ZNAMELEN) ;
	    } /* end if */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (mbcache_check) */
#endif /* COMMENT */

/* mark a message for deletion */
int mbcache_msgdel(mbcache *op,int mi,int delcmd) noex {
    	int		nmsgs ;
	int		rs ;
	int		f_delprev = false ;
	if ((rs = mbcache_magic(op)) >= 0) {
	    mailbox_info	*mip = op->mip ;
	    nmsgs = mip->nmsgs ;
	    rs = SR_NOMSG ;
	    if ((mi >= 0) && (mi < nmsgs)) {
		rs = SR_ROFS ;
	        if ((delcmd < 0) || (! op->fl.readonly)) {
	            if ((delcmd >= 0) || (op->msgs[mi] != nullptr)) {
	                ME	*mep ;
	                if ((rs = mbcache_msgframing(op,mi,&mep)) >= 0) {
	                    f_delprev = mep->fl.del ;
	                    if (delcmd >= 0) {
	                        if (! LEQUIV(mep->fl.del,delcmd)) {
	                            mep->fl.del = (delcmd) ? true : false ;
	                            rs = mailbox_msgdel(op->mbp,mi,delcmd) ;
	                        }
	                    } /* end if */
	                } /* end if */
	            } /* end if (short cut) */
	        } /* end if (not-readonly) */
	    } /* end if (have-message) */
	} /* end if (magic) */
	return (rs >= 0) ? f_delprev : rs ;
}
/* end subroutine (mbcache_msgdel) */

int mbcache_msgdeldup(mbcache *op) noex {
    	int		nmsgs ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = mbcache_magic(op)) >= 0) {
	    mailbox_info	*mip = op->mip ;
	    nmsgs = mip->nmsgs ;
	    if (mapstrint mm ; (rs = mapstrint_start(&mm,nmsgs)) >= 0) {
	        cint	vi = mbcachemf_hdrmid ;
	        for (int mi = 0 ; mi < nmsgs ; mi += 1) {
	            if (ME *mep ; (rs = mbcache_msginfo(op,mi,&mep)) >= 0) {
		        if (! mep->fl.del) {
	                    cchar	*midp = mep->vs[vi] ;
	                    cint	midl = mep->vl[vi] ;
		            if ((midp != nullptr) && (midl > 0)) {
				auto	msa = mapstrint_already ;
		                if ((rs = msa(&mm,midp,midl)) >= 0) {
			            c += 1 ;
	                            mep->fl.del = true ;
	                            rs = mailbox_msgdel(op->mbp,mi,true) ;
		                } else if (rs == SR_NOTFOUND) {
		                    rs = mapstrint_add(&mm,midp,midl,mi) ;
		                }
		            } /* end if (non-null) */
		        } /* end if (not already being deleted) */
	            } /* end (mbcache_msginfo) */
		    if (rs < 0) break ;
	        } /* end for */
	        rs1 = mapstrint_finish(&mm) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (mapstrint) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (mbcache_msgdeldup) */

/* retrieve MSG flags (dispositions) */
int mbcache_msgflags(mbcache *op,int mi) noex {
    	int		nmsgs ;
	int		rs ;
	int		mf = 0 ;
	if ((rs = mbcache_magic(op)) >= 0) {
	    mailbox_info	*mip = op->mip ;
	    nmsgs = mip->nmsgs ;
	    rs = SR_NOMSG ;
	    if ((mi >= 0) && (mi < nmsgs)) {
	        if (ME *mep ; (rs = mbcache_msgframing(op,mi,&mep)) >= 0) {
	            if (mep->fl.read) mf |= MBCACHE_MFMREAD ;
	            if (mep->fl.del) mf |= MBCACHE_MFMDEL ;
	            if (mep->fl.spam) mf |= MBCACHE_MFMSPAM ;
	            if (mep->fl.trash) mf |= MBCACHE_MFMTRASH ;
	        } /* end if */
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? mf : rs ;
}
/* end subroutine (mbcache_msgflags) */

/* set MSG flags (dispositions) */
int mbcache_msgsetflag(mbcache *op,int mi,int w,int v) noex {
    	int		nmsgs ;
	int		rs ;
	int		mf = 0 ;
	if ((rs = mbcache_magic(op)) >= 0) {
	    mailbox_info	*mip = op->mip ;
	    nmsgs = mip->nmsgs ;
	    rs = SR_NOMSG ;
	    if ((mi >= 0) && (mi < nmsgs)) {
	        if (ME *mep ; (rs = mbcache_msgframing(op,mi,&mep)) >= 0) {
	            switch (w) {
	            case MBCACHE_MFVREAD:
	                mf = mep->fl.read ;
	                mep->fl.read = (v>0) ;
	                break ;
	            case MBCACHE_MFVDEL:
	                mf = mep->fl.del ;
	                mep->fl.del = (v>0) ;
	                break ;
	            case MBCACHE_MFVSPAM:
	                mf = mep->fl.spam ;
	                mep->fl.spam = (v>0) ;
	                break ;
	            case MBCACHE_MFVTRASH:
	                mf = mep->fl.trash ;
	                mep->fl.trash = (v>0) ;
	                break ;
	            } /* end switch */
	        } /* end if */
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? mf : rs ;
}
/* end subroutine (mbcache_msgsetflag) */

int mbcache_countdel(mbcache *op) noex {
	int		rs ;
	if ((rs = mbcache_magic(op)) >= 0) {
	    rs = mailbox_countdel(op->mbp) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (mbcache_countdel) */

int mbcache_msgsetlines(mbcache *op,int mi,int vlines) noex {
    	int		nmsgs ;
	int		rs ;
	int		rlines = 0 ;
	if ((rs = mbcache_magic(op)) >= 0) {
	    mailbox_info	*mip = op->mip ;
	    nmsgs = mip->nmsgs ;
	    rs = SR_NOMSG ;
	    if ((mi >= 0) && (mi < nmsgs)) {
	        if (ME *mep ; (rs = mbcache_msgframing(op,mi,&mep)) >= 0) {
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
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? rlines : rs ;
}
/* end subroutine (mbcache_msgsetlines) */

/* get the file offset to the start-envelope of a message */
int mbcache_msgoff(mbcache *op,int mi,off_t *rp) noex {
    	int		nmsgs ;
	int		rs ;
	if ((rs = mbcache_magic(op,rp)) >= 0) {
	    mailbox_info	*mip = op->mip ;
	    nmsgs = mip->nmsgs ;
	    rs = SR_NOMSG ;
	    if ((mi >= 0) && (mi < nmsgs)) {
	        ME	*mep = op->msgs[mi] ;
	        if (mep) {
	            rs = mbcache_msgframing(op,mi,nullptr) ;
	            mep = op->msgs[mi] ;
	        }
	        *rp = (rs >= 0) ? mep->moff : 0 ;
	        if (rs >= 0) {
	            rs = mep->mlen ;
	        }
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (mbcache_msgoff) */

/* get the number of lines in a message */
int mbcache_msglines(mbcache *op,int mi,int *rp) noex {
    	int		nmsgs ;
	int		rs ;
	if ((rs = mbcache_magic(op,rp)) >= 0) {
	    mailbox_info	*mip = op->mip ;
	    nmsgs = mip->nmsgs ;
	    rs = SR_NOMSG ;
	    if ((mi >= 0) && (mi < nmsgs)) {
		ME	*mep = op->msgs[mi] ;
	        int	rlines = -1 ;
	        if (mep) {
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
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (mbcache_msglines) */

/* get the framing information for a message */
int mbcache_msginfo(mbcache *op,int mi,ME **mpp) noex {
    	int		nmsgs ;
	int		rs ;
	if ((rs = mbcache_magic(op,mpp)) >= 0) {
	    mailbox_info	*mip = op->mip ;
	    nmsgs = mip->nmsgs ;
	    *mpp = nullptr ;
	    rs = SR_NOMSG ;
	    if ((mi >= 0) && (mi < nmsgs)) {
	        ME	*mep = op->msgs[mi] ;
	        if (mep) {
	            rs = mbcache_msgframing(op,mi,nullptr) ;
	            mep = op->msgs[mi] ;
	        }
	        if (rs >= 0) {
	            if ((rs = msgent_load(mep,op)) >= 0) {
	                *mpp = mep ;
	            }
	        }
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (mbcache_msginfo) */

/* get the scan information for a message */
int mbcache_msgscan(mbcache *op,int mi,ME **mpp) noex {
	int		rs ;
	if ((rs = mbcache_msginfo(op,mi,mpp)) >= 0) {
	    ME	*mep = *mpp ;
	    if ((rs = msgent_procscanfrom(mep,op)) >= 0) {
	        rs = msgent_procscandate(mep,op) ;
	    }
	}
	return rs ;
}
/* end subroutine (mbcache_msgscan) */

int mbcache_msgenvtime(mbcache *op,int mi,time_t *timep) noex {
	time_t		t = 0 ;
	int		rs ;
	int		f = false ;
	if ((rs = mbcache_magic(op,timep)) >= 0) {
	    if (ME *mep ; (rs = mbcache_msginfo(op,mi,&mep)) >= 0) {
	        if ((rs = msgent_procenvdate(mep,op)) >= 0) {
	            t = mep->etime ;
	            f = true ;
	        }
	    }
	} /* end if (magic) */
	if (timep) *timep = t ;
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (mbcache_msgenvtime) */

int mbcache_msghdrtime(mbcache *op,int mi,time_t *timep) noex {
	time_t		t = 0 ;
	int		rs ;
	int		f = false ;
	if ((rs = mbcache_magic(op,timep)) >= 0) {
	    if (ME *mep ; (rs = mbcache_msginfo(op,mi,&mep)) >= 0) {
	        if ((rs = msgent_prochdrdate(mep,op)) >= 0) {
	            f = true ;
	            t = mep->htime ;
	        }
	    }
	} /* end if (magic) */
	if (timep) *timep = t ;
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (mbcache_msghdrtime) */

int mbcache_msgtimes(mbcache *op,int mi,time_t *timep) noex {
	int		rs ;
	if ((rs = mbcache_magic(op,timep)) >= 0) {
	    if (ME *mep ; (rs = mbcache_msginfo(op,mi,&mep)) >= 0) {
	        if ((rs = msgent_msgtimes(mep,op)) >= 0) {
	            timep[0] = mep->etime ;
	            timep[1] = mep->htime ;
	        }
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (mbcache_msgtimes) */


/* private subroutines */

local int mbcache_msgfins(mbcache *op) noex {
	mailbox_info	*mip = op->mip ;
	int		nmsgs ;
	int		rs = SR_OK ;
	int		rs1 ;
	nmsgs = mip->nmsgs ;
	for (int mi = 0 ; mi < nmsgs ; mi += 1) {
	    ME	*mep = op->msgs[mi] ;
	    if (mep) {
		{
	            rs1 = msgent_finish(mep) ;
	            if (rs >= 0) rs = rs1 ;
		}
		{
	            rs1 = libmem.free(mep) ;
	            if (rs >= 0) rs = rs1 ;
		}
	        op->msgs[mi] = nullptr ;
	    } /* end if (non-null) */
	} /* end for */
	return rs ;
}
/* end subroutine (mbcache_msgfins) */

local int mbcache_msgframing(mbcache *op,int mi,ME **mpp) noex {
	int		rs = SR_OK ;
	if (op->msgs[mi] == nullptr) {
	    mailbox_mi	*mip{} ;
	    if ((rs = mailbox_msgret(op->mbp,mi,&mip)) >= 0) {
	        cint	sz = szof(ME) ;
	        if (ME *mep ; (rs = libmem.mall(sz,&mep)) >= 0) {
	            if ((rs = msgent_start(mep,mi)) >= 0) {
	                if ((rs = msgent_frame(mep,mip)) >= 0) {
	                    op->msgs[mi] = mep ;
	                }
	                if (rs < 0) {
	                    msgent_finish(mep) ;
			}
	            } /* end if (msgent_start) */
	            if (rs < 0) {
	                libmem.free(mep) ;
		    }
	        } /* end if (memory-allocation) */
	    } /* end if (mailbox_getret) */
	} /* end if (needed) */
	if (mpp) {
	    *mpp = (rs >= 0) ? op->msgs[mi] : nullptr ;
	}
	return rs ;
}
/* end subroutine (mbcache_msgframing) */

local int mbcache_msgtimers(mbcache *op,int mi,time_t *timep) noex {
	int		rs ;
	if (ME *mep ; (rs = mbcache_msginfo(op,mi,&mep)) >= 0) {
	    if ((rs = msgent_msgtimes(mep,op)) >= 0) {
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
local int mbcache_msgscanmk(mbcache *op,int mi) noex {
	ME		*mep = op->msgs[mi] ;
	cint		sl = SCANBUFLEN ;
	int		rs = SR_OK ;
	if (mep != nullptr) {
	    if (mep->vs[mbcachemf_scanline] == nullptr) {
	        cint	sz = (sl + 1) ;
	        if (char *bp ; (rs = libmem.mall(sz,&bp)) >= 0) {
	            int		tcol ;
	            strwcpyblanks(bp,sl) ;
	            for (int i = 0 ; scantitles[i].name ; i += 1) {
	                cchar	*cp = scantitles[i].name ;
	                cint	cl = lenstr(cp) ;
	                tcol = scantitles[i].col ;
	                strncpy((bp + tcol),cp,cl) ;
	            } /* end for */
	            if (rs >= 0) {
	                mep->vs[mbcachemf_scanline] = bp ;
	            } else {
	                libmem.free(bp) ;
		    }
	        } /* end if (memory-allocation) */
	    } /* end if */
	} /* end if (not-null) */
	return rs ;
}
/* end subroutine (mbcache_msgscanmk) */
#endif /* COMMENT */

#ifdef	COMMENT
local int mbcache_setnow(mbcache *op) noex {
	int		rs = SR_OK ;
	if (! op->fl.now) {
	    TIMEB	*tbp = &op->now ;
	    if ((rs = uc_ftime(tbp)) >= 0) {
	        if (TMTIME tmt ; (rs = tmtime_localtime(&tmt,tbp->time)) >= 0) {
	            tbp->timezone = (tmt.gmtoff / 60) ;
	            tbp->dstflag = tmt.isdst ;
	            strncpy(op->zname,tmt.zname,DATER_ZNAMELEN) ;
	            op->fl.now = true ;
	        } /* end if */
	    } /* end if (uc_ftime) */
	} /* end if (have-now) */
	return rs ;
}
/* end subroutine (mbcache_setnow) */

#endif /* COMMENT */

local int msgent_start(ME *mep,int mi) noex {
	int		rs = SR_FAULT ;
	if (mep) {
	    rs = memclear(mep) ;
	    mep->msgi = mi ;
	    mep->nlines = -1 ;
	    mep->vlines = -1 ;
	    mep->filesz = -1 ;
	    mep->moff = -1 ;
	    mep->hoff = -1 ;
	    mep->boff = -1 ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (msgent_start) */

local int msgent_finish(ME *mep) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (mep) {
	    rs = SR_OK ;
	    if (mep->fl.lineoffs) {
	        mep->fl.lineoffs = false ;
	        rs1 = vecint_finish(&mep->lineoffs) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (mep->fname != nullptr) {
		void *vp = voidp(mep->fname) ;
	        rs1 = libmem.free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        mep->fname = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (msgent_finish) */

local int msgent_frame(ME *mep,mailbox_mi *mip) noex {
	int		rs = SR_FAULT ;
	if (mep && mip) {
	    rs = SR_OK ;
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
	    if ((mep->nlines < 0) && mip->hdrval.clines) {
	        mep->nlines = mip->clines ;
	    }
	    if ((mep->nlines < 0) && mip->hdrval.lines) {
	        mep->nlines = mip->lines ;
	    }
	    if ((mep->nlines < 0) && mip->hdrval.xlines) {
	        mep->nlines = mip->xlines ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (msgent_frame) */

typedef int (*msgent_f)(ME *,mbcache *,mailmsg *) noex ;

/* ENV:		extract envelope information */
/* MESSAGEID:	extract the mailmsg information that we want */
/* FROM:	extract the mailmsg information that we want */
/* SUBJECT:	extract the mailmsg information that we want */
/* DATE:	extract the mailmsg information that we want */
/* STATUS:	extract the mailmsg information that we want */
static msgent_f		msgents[] = {
	msgent_loadenv,
	msgent_loadhdrmid,
	msgent_loadhdrfrom,
	msgent_loadhdrsubj,
	msgent_loadhdrdate,
	msgent_loadhdrstatus
} ;

local int msgent_load(ME *mep,mbcache *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (! mep->fl.info) {
	    mep->fl.info = true ;
	    if (mailmsg m ; (rs = mailmsg_start(&m)) >= 0) {
	        mailbox		*mbp = op->mbp ;
	        coff		mbo = mep->moff ;
	        if ((rs = mailmsg_loadmb(&m,mbp,mbo)) >= 0) {
		    for (cauto f : msgents) {
			rs = f(mep,op,&m) ;
			if (rs < 0) break ;
		    } /* end for */
		    /* set VS bit */
	            if (rs >= 0) {
			mep->fl.vs = true ;
		    }
	        } /* end if (mailmsg-loadmb) */
	        rs1 = mailmsg_finish(&m) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (mailmsg) */
	} /* end if (initialized) */
	return rs ;
}
/* end subroutine (msgent_load) */

local int msgent_loadenv(ME *mep,mbcache *op,mailmsg *mmp) noex {
	int		rs ;
	if ((rs = mailmsg_envcount(mmp)) >= 0) {
	    strpack		*psp = op->spp ;
	    cint		n = rs ;
	    mailmsg_envdat	e ;
	    if ((rs = mailmsg_envget(mmp,(n-1),&e)) >= 0) {
	        for (int i = 0 ; (rs >= 0) && (i < 3) ; i += 1) {
	            int		vl = -1 ;
	            int		*vlp ;
	            cchar	**vpp ;
	            cchar	*vp = nullptr ;
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
	            if (vp) {
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
/* end subroutine (msgent_loadenv) */

#if	CF_LOADENVADDR
local int msgent_loadenvaddr(ME *mep,MC *op,mailmsg *mmp,cchar **rpp) noex {
	int		rs = SR_OK ;
	int		len = 0 ;
	if (mep->vs[mbcachemf_envaddr] == nullptr) {
	    rs = msgent_loadenv(mep,op,mmp) ;
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
/* end subroutine (msgent_loadenvaddr) */
#endif /* CF_LOADENVADDR */

local int msgent_loadhdrmid(ME *mep,mbcache *op,mailmsg *mmp) noex {
	int		rs ;
	int		sl = 0 ;
	cchar		*hdr = HN_MESSAGEID ;
	if (cchar *sp ; (rs = mailmsg_hdrval(mmp,hdr,&sp)) >= 0) {
	    cint	hlen = var.hdrbuflen ;
	    strpack	*psp = op->spp ;
	    if (char *hbuf ; (rs = libmem.mall((hlen+1),&hbuf)) >= 0) {
	        if ((rs = mkaddrbest(hbuf,hlen,sp,rs)) >= 0) {
	            cint	vi = mbcachemf_hdrmid ;
	            cchar	**rpp ;
	            sl = rs ;
	            rpp = (mep->vs + vi) ;
	            mep->vl[vi] = sl ;
	            rs = strpack_store(psp,hbuf,sl,rpp) ;
	        } /* end if (mkaddrbest) */
		rs = rsfree(rs,hbuf) ;
	    } /* end if (m-a-f) */
	} else if (isNoMsg(rs)) {
	    rs = SR_OK ;
	}
	return (rs >= 0) ? sl : rs ;
}
/* end subroutine (msgent_loadhdrmid) */

local int msgent_loadhdrfrom(ME *mep,mbcache *op,mailmsg *mmp) noex {
	int		rs ;
	int		vl = 0 ;
	cchar		*hdr = HN_FROM ;
	cchar		*vp ;		/* used in two blocks below */
	if ((rs = mailmsg_hdrval(mmp,hdr,&vp)) > 0) {
	    vl = rs ;
	} else if ((rs == 0) || isNoMsg(rs)) {
	    hdr = HN_RETURNPATH ;
	    if ((rs = mailmsg_hdrval(mmp,hdr,&vp)) > 0) {
	        vl = rs ;
	    }
	}
	if ((rs >= 0) && (vl >= 0)) {
	    strpack	*psp = op->spp ;
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
/* end subroutine (msgent_loadhdrfrom) */

local int msgent_loadhdrsubj(ME *mep,mbcache *op,mailmsg *mmp) noex {
	int		rs ;
	int		sl = 0 ;
	cchar		*hdr = HN_SUBJECT ;
	if (cchar *sp ; (rs = mailmsg_hdrval(mmp,hdr,&sp)) >= 0) {
	    strpack	*psp = op->spp ;
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
/* end subroutine (msgent_loadhdrsubj) */

local int msgent_loadhdrdate(ME *mep,mbcache *op,mailmsg *mmp) noex {
	int		rs ;
	int		sl = 0 ;
	cchar		*hdr = HN_DATE ;
	if (cchar *sp ; (rs = mailmsg_hdrval(mmp,hdr,&sp)) >= 0) {
	    strpack	*psp = op->spp ;
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
/* end subroutine (msgent_loadhdrdate) */

local int msgent_loadhdrstatus(ME *mep,mbcache *op,mailmsg *mmp) noex {
	int		rs ;
	int		sl = 0 ;
	cchar		*hdr = HN_STATUS ;
	if (cchar *sp ; (rs = mailmsg_hdrval(mmp,hdr,&sp)) >= 0) {
	    strpack	*psp = op->spp ;
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
/* end subroutine (msgent_loadhdrstatus) */

local int msgent_procenvdate(ME *mep,mbcache *op) noex {
	cint		vi = mbcachemf_envdate ;
	int		rs = SR_OK ;
	int		f = false ;
	if (! mep->proc.edate) {
	    cint	sl = mep->vl[vi] ;
	    cchar	*sp = mep->vs[vi] ;
	    mep->proc.edate = true ;
	    if ((sp != nullptr) && (sl > 0)) {
	        dater	*dp = op->dmp ;
	        if ((rs = dater_setstd(dp,sp,sl)) >= 0) {
	            date	*dop = &mep->edate ;
	            time_t	t = 0 ;
	            if ((rs = dater_getdate(dp,dop)) >= 0) {
	                mep->fl.edate = true ;
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
/* end subroutine (msgent_procenvdate) */

local int msgent_prochdrdate(ME *mep,mbcache *op) noex {
	int		rs = SR_OK ;
	int		f = false ;
	if (! mep->proc.hdate) {
	    cint	vi = mbcachemf_hdrdate ;
	    int		sl = mep->vl[vi] ;
	    cchar	*sp = mep->vs[vi] ;
	    mep->proc.hdate = true ;
	    if ((sp != nullptr) && (sl > 0)) {
	        dater	*dp = op->dmp ;
	        if ((rs = dater_setmsg(dp,sp,sl)) >= 0) {
	            date	*dop = &mep->hdate ;
	            time_t	t = 0 ;
	            if ((rs = dater_getdate(dp,dop)) >= 0) {
	                mep->fl.hdate = true ;
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
/* end subroutine (msgent_prochdrdate) */

local int msgent_procscanfrom(ME *mep,mbcache *op) noex {
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
	        strpack	*psp = op->spp ;
	        cint	hlen = var.hdrbuflen ;
	        if (char *hbuf ; (rs = libmem.mall((hlen+1),&hbuf)) >= 0) {
	            if ((rs = mkaddrname(hbuf,hlen,sp,sl)) >= 0) {
	                cint	vi = mbcachemf_scanfrom ;
	                cchar	**rpp ;
	                len = rs ;
	                rpp = (mep->vs + vi) ;
	                mep->vl[vi] = len ;
	                if ((rs = strpack_store(psp,hbuf,len,rpp)) >= 0) {
	                    mep->fl.scanfrom = true ;
	                }
	            } /* end if (mkaddrname) */
		    rs = rsfree(rs,hbuf) ;
		} /* end if (m-a-f) */
	    } /* end if (positive) */
	} else if (mep->vs[mbcachemf_scanfrom] != nullptr) {
	    len = mep->vl[mbcachemf_scanfrom] ;
	}
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (msgent_procscanfrom) */

local int msgent_procscandate(ME *mep,mbcache *op) noex {
    	cnullptr	np{} ;
	int		rs = SR_OK ;
	int		cl = 0 ;
	if (! mep->proc.scandate) {
	    mep->proc.scandate = true ;
	    if ((rs = msgent_prochdrdate(mep,op)) >= 0) {
	        time_t	t = mep->htime ;
	        if (t == 0) {
	            if ((rs = msgent_procenvdate(mep,op)) >= 0) {
	                t = mep->etime ;
	            }
	        }
	        if ((rs >= 0) && (t > 0)) {
	            char	timebuf[TIMEBUFLEN + 1] ;
	            if (cc *ts ; (ts = timestr_scandate(t,timebuf)) != np) {
	                strpack		*psp = op->spp ;
	                if (cc *cp ; (rs = strpack_store(psp,ts,-1,&cp)) >= 0) {
	                    cl = rs ;
	                    mep->fl.scandate = true ;
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
/* end subroutine (msgent_procscandate) */

local int msgent_msgtimes(ME *mep,mbcache *op) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = msgent_procenvdate(mep,op)) >= 0) {
	    c += rs ;
	    if ((rs = msgent_prochdrdate(mep,op)) >= 0) {
	        c += rs ;
	    }
	}
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (msgent_msgtimes) */

vars::operator int () noex {
    	int		rs ;
	if ((rs = getbufsize(getbufsize_mailaddr)) >= 0) {
	    hdrbuflen = (rs * HDRBUFMULT) ;
	}
	return rs ;
}

static bool isBadDate(int rs) noex {
	return isOneOf(rsdatebad,rs) ;
}
/* end subroutine (isBadDate) */

static bool isNoMsg(int rs) noex {
	return isOneOf(rsnomsg,rs) ;
}
/* end subroutine (isNoMsg) */

local int vcmpmsgentry(cvoid *e1pp,cvoid *e2pp) noex {
	ME		**m1pp = (ME **) e1pp ;
	ME		**m2pp = (ME **) e2pp ;
	ME		*m1p ;
	ME		*m2p ;
	int		rc = 0 ;
	m1p = (*m1pp) ;
	m2p = (*m2pp) ;
	if (m1p || m2p) {
	    if ((rc == 0) && (m1p == nullptr)) rc = +1 ;
	    if ((rc == 0) && (m2p == nullptr)) rc = -1 ;
	    if (rc == 0) {
	        const time_t	t1e = m1p->etime ;
	        const time_t	t2e = m2p->etime ;
	        time_t		t1h = m1p->htime ;
	        time_t		t2h = m2p->htime ;
	        if (t1h == 0) t1h = t1e ;
	        if (t2h == 0) t2h = t2e ;
	        if ((rc = intconv(t1h - t2h)) == 0) {
	            rc = intconv(t1e - t2e) ;
		}
	    }
	}
	return rc ;
}
/* end subroutine (vcmpmsgentry) */

#ifdef	COMMENT
/* append some additional value to an existing header value */
local int headappend(cchar **pp,cchar *sp,int sl) noex {
	int		rs = SR_OK ;
	int		cl = 0 ;
	if (*pp) {
	    cint	cl2 = lenstr(*pp) ;
	    cl = sl + cl2 + 2 ;
	    if (char *cp ; (rs = libmem.mall(cl,&cp)) >= 0) {
	        char	*cp2 = strwcpy(cp,*pp,cl2) ;
	        *cp2++ = ',' ;
	        *cp2++ = ' ' ;
	        strwcpy(cp2,sp,sl) ;
	        libmem.free(*pp) ;
	        *pp = cp ;
	    } /* end if (memory-allocation) */
	} /* end if (valid) */
	return (rs >= 0) ? cl : rs ;
}
/* end subroutine (headappend) */
#endif /* COMMENT */

#ifdef	COMMENT
/* is the next non-white-space character a colon */
local int nextnonwhite(cchar *sp,int sl) noex {
	int		i = 0 ;
	while (CHAR_ISWHITE(*sp) && (i++ < sl)) {
	    sp += 1 ;
	}
	return (i < sl) ? *sp : ' ' ;
}
/* end subroutine (nextnonwhite) */
#endif /* COMMENT */



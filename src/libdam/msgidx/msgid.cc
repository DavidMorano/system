/* msgid SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* manage message-id storage */
/* version %I% last-modified %G% */

#define	CF_SAFE		1
#define	CF_CREAT	0		/* always create the file? */
#define	CF_HASH		1		/* use hash for faster lookup */

/* revision history:

	= 2003-02-17, David A­D­ Morano
	This code module is being started to eliminate repeated
	mail messages (essentially just from ACM).  Much of this
	code was borrowed from the SRVREG object (which performs a
	somewhat similar function).

*/

/* Copyright © 2003 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	msgid

	Description:
	This object module manages the reading and writing of entries
	to or from a message identification database file.  This
	database is normally used to reject duplicate mail messages
	that have come in to the local server from remote mail
	servers.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<inttypes.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>
#include	<ctime>
#include	<cstdlib>
#include	<cstring>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<getbufsize.h>
#include	<endian.h>
#include	<lockfile.h>
#include	<serialbuf.h>
#include	<stdorder.h>
#include	<mkfname.h>
#include	<intfloor.h>
#include	<intceil.h>
#include	<hash.h>
#include	<strwcpy.h>
#include	<isfiledesc.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"msgid.h"
#include	"msgide.h"


/* local defines */

#define	MSGID_FMA	"MSGIDA"
#define	MSGID_FMB	"MSGIDB"

#define	MSGID_FS	"msgid"
#define	MSGID_FSA	"msgida"
#define	MSGID_FSB	"msgidb"

#define	MSGID_FLID	(16 + 4)
#define	MSGID_FLHEAD	(3 * 4)
#define	MSGID_FLTOP	(MSGID_FLID + MSGID_FLHEAD)

#define	MSGID_FOID	0
#define	MSGID_FOHEAD	(MSGID_FOID + MSGID_FLID)
#define	MSGID_FOTAB	(MSGID_FOHEAD + MSGID_FLHEAD)

#define	msgid_entSIZE	MSGIDE_SIZE
#define	MSGID_EBS	((MSGIDE_SIZE + 3) & (~ 3))

#define	MSGID_BUFSIZE	(64 * 1024)
#define	MSGID_READSIZE	(16 * 1024)

#define	MSGID_FBUFLEN	(MSGID_FLTOP + 9)

#define	MSGID_LWRITE	0
#define	MSGID_LREAD	1

#define	TO_OPEN		(60 * 60)	/* maximum file-open time */
#define	TO_ACCESS	(2 * 60)	/* maximum access idle time */
#define	TO_LOCK		60		/* seconds */

#ifndef	TIMEBUFLEN
#define	TIMEBUFLEN	80
#endif

/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */

extern "C" {
    int		msgid_close(msgid *) noex ;
}


/* external variables */


/* local structures */

struct oldentry {
	time_t		utime ;
	int		ei ;
} ;

namespace {
    struct vars {
	int		lcount ;
	int		lutime ;
	int		lctime ;
	int		lmtime ;
	int		lhash ;
	int		lrecip ;
	int		lmsgid ;
	int		lfrom ;
	int		lread ;
	int		mkvars() noex ;
    } ; /* end struct (vars) */
}


/* forward references */

template<typename ... Args>
static int msgid_ctor(msgid *op,Args ... args) noex {
    	MSGID		*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = memclear(hop) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (msgid_ctor) */

static int msgid_dtor(msgid *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (msgid_dtor) */

template<typename ... Args>
static inline int msgid_magic(msgid *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == MSGID_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (msgid_magic) */

static int	msgid_opener(msgid *,char *,cchar *,int,mode_t) noex ;
static int	msgid_openone(msgid *,char *,cchar *,cchar *,int,mode_t) noex ;

static int	msgid_fileopen(msgid *,time_t) noex ;
static int	msgid_fileclose(msgid *) noex ;
static int	msgid_lockget(msgid *,time_t,int) noex ;
static int	msgid_lockrelease(msgid *) noex ;
static int	msgid_fileinit(msgid *,time_t) noex ;
static int	msgid_filechanged(msgid *) noex ;
static int	msgid_filecheck(msgid *,time_t,int) noex ;
static int	msgid_searchid(msgid *,cchar *,int,char **) noex ;
static int	msgid_search(msgid *,MSGID_KEY *,uint,char **) noex ;
static int	msgid_searchempty(msgid *,struct oldentry *,char **) noex ;
static int	msgid_searchemptyrange(msgid *,int,int,
			struct oldentry *,char **) noex ;
static int	msgid_bufload(msgid *,int,int,char **) noex ;
static int	msgid_bufupdate(msgid *,int,int,cchar *) noex ;
static int	msgid_bufbegin(msgid *) noex ;
static int	msgid_bufend(msgid *) noex ;
static int	msgid_writehead(msgid *) noex ;

static int	filemagic(char *,int,MSGID_FM *) noex ;
static int	filehead(char *,int,MSGID_FH *) noex ;

static int	emat_recipid(cchar *,MSGID_KEY *) noex ;

static uint	recipidhash(MSGID_KEY *,int,int) noex ;
static int	matfield(cchar *,int,cchar *,int) noex ;
static int	extutime(char *) noex ;


/* local variables */

static vars		var ;


/* exported variables */


/* exported subroutines */

int msgid_open(msgid *op,cchar *fname,int of,mode_t om,int maxentry) noex {
	int		rs ;
	int		f_create = false ;
	if ((rs = msgid_ctor(op,fname)) >= 0) {
	    rs = SR_INVALID ;
	    if (fname[0]) {
		static cint	rsv = var.mkvars() ;
		if ((rs = rsv) >= 0) {

#if	CF_CREAT
	of |= O_CREAT ;
#endif

	of &= (~ O_TRUNC) ;

	op->fd = -1 ;
	op->oflags = of ;
	op->operm = om ;
	op->maxentry = maxentry ;
	op->ebs = uceil(msgid_entSIZE,4) ;

/* initialize the buffer structure */

	if ((rs = msgid_bufbegin(op)) >= 0) {
	    const time_t	dt = time(nullptr) ;
	    char		tbuf[MAXPATHLEN+1] ;
	    if ((rs = msgid_opener(op,tbuf,fname,of,om)) >= 0) {
	        cchar	*fn ;
	        f_create = rs ;
	        op->opentime = dt ;
	        op->accesstime = dt ;
	        if ((rs = uc_mallocstrw(tbuf,-1,&fn)) >= 0) {
	            cint	am = (of & O_ACCMODE) ;
	            op->fname = fn ;
	            op->f.writable = ((am == O_WRONLY) || (am == O_RDWR)) ;
	            if (USTAT sb ; (rs = u_fstat(op->fd,&sb)) >= 0) {
	                if (S_ISREG(sb.st_mode)) {
	                    op->mtime = sb.st_mtime ;
	                    op->filesize = sb.st_size ;
	                    op->pagesize = getpagesize() ;
	                    if ((rs = isfsremote(op->fd)) >= 0) {
	                        op->f.remote = (rs > 0) ;
	                        if ((rs = msgid_fileinit(op,dt)) >= 0) {
	                            op->magic = MSGID_MAGIC ;
	                        }
	                    } /* end if (isfsremote) */
	                } else {
	                    rs = SR_ISDIR ;
	                }
	            } /* end if (stat) */
	            if (rs < 0) {
	                uc_free(op->fname) ;
	                op->fname = nullptr ;
	            }
	        } /* end if (m-a) */
	        if (rs < 0) {
	            u_close(op->fd) ;
	            op->fd = -1 ;
	        }
	    } /* end if (open) */
	    if (rs < 0) {
	        msgid_bufend(op) ;
	    }
	} /* end if (buffer-start) */

		} /* end if (var.mkvars) */
	    } /* end if (valid) */
	    if (rs < 0) {
		msgid_dtor(op) ;
	    }
	} /* end if (msgid_ctor) */
	return (rs >= 0) ? f_create : rs ;
}
/* end subroutine (msgid_open) */

int msgid_close(msgid *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = msgid_magic(op)) >= 0) {
	    {
	        rs1 = msgid_bufend(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->fd >= 0) {
	        rs1 = u_close(op->fd) ;
	        if (rs >= 0) rs = rs1 ;
	        op->fd = -1 ;
	    }
	    if (op->fname) {
	        rs1 = uc_free(op->fname) ;
	        if (rs >= 0) rs = rs1 ;
	        op->fname = nullptr ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (msgid_close) */

int msgid_txbegin(msgid *op) noex {
    	int		rs ;
	if ((rs = msgid_magic(op)) >= 0) {
	    rs = SR_OK ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (msgid_txbegin) */

int msgid_txabort(msgid *op,int txid) noex {
    	int		rs ;
	(void) txid ;
	if ((rs = msgid_magic(op)) >= 0) {
	    rs = SR_OK ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (msgid_txabort) */

int msgid_txcommit(msgid *op,int txid) noex {
    	int		rs ;
	(void) txid ;
	if ((rs = msgid_magic(op)) >= 0) {
	    rs = SR_OK ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (msgid_txcomment) */

int msgid_count(msgid *op) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = msgid_magic(op)) >= 0) {
	    c = ((op->filesize - MSGID_FOTAB) / msgid_entSIZE) ;
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (msgid_count) */

int msgid_curbegin(msgid *op,msgid_cur *curp) noex {
    	int		rs ;
	if ((rs = msgid_magic(op,curp)) >= 0) {
	    op->cursors += 1 ;
	    op->f.cursorlockbroken = false ;
	    op->f.cursoracc = false ;
	    curp->i = -1 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (msgid_curbegin) */

int msgid_curend(msgid *op,msgid_cur *curp) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = msgid_magic(op,curp)) >= 0) {
	    if (op->f.cursoracc) {
	        op->accesstime = getustime ;
	    }
	    if (op->cursors > 0) {
	        op->cursors -= 1 ;
	    }
	    if ((op->cursors == 0) && (op->f.readlocked || op->f.writelocked)) {
	        rs1 = msgid_lockrelease(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    curp->i = -1 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (msgid_curend) */

int msgid_curenum(msgid *op,msgid_cur *curp,msgid_ent *ep) noex {
	custime		dt = getustime ;
	int		rs ;
	int		ei = 0 ;
	if ((rs = msgid_magic(op,curp)) >= 0) {
	    rs = SR_NOTFOUND ;
	    if (op->f.fileinit) {
	    	rs = SR_LOCKLOST ;
	        if (! op->f.cursorlockbroken) {
	int		eoff ;
	char		*bp ;
	if ((rs = msgid_filecheck(op,dt,var.lread)) >= 0) {
	        ei = (curp->i < 0) ? 0 : curp->i + 1 ;
	        eoff = MSGID_FOTAB + (ei * op->ebs) ;

/* form result to caller */

	        if ((eoff + op->ebs) <= op->filesize) {

/* verify sufficient file buffering */

	            if ((rs = msgid_bufload(op,eoff,op->ebs,&bp)) >= 0) {
	                if (rs >= op->ebs) {

/* copy entry to caller buffer */

	                    if (ep) {
	                        msgide_all(ep,1,bp,MSGIDE_SIZE) ;
	                    } /* end if */

/* commit the cursor movement? */

	                    if (rs >= 0) {
	                        curp->i = ei ;
			    }
	                    op->f.cursoracc = true ;

	                } else {
	                    rs = SR_EOF ;
			}
	            } /* end if (msgid_buf) */

	        } else {
	            rs = SR_NOTFOUND ;
		}
	} /* end if (msgid_filecheck) */

	        } /* end if (valid) */
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? ei : rs ;
}
/* end subroutine (msgid_curenum) */

int msgid_matchid(msgid *op,time_t dt,cchar *midp,int midl,msgid_ent *ep) noex {
	int		rs ;
	int		ei = 0 ;
	if ((rs = msgid_magic(op,midp)) >= 0) {
	    rs = SR_NOTFOUND ;
	    if (op->f.fileinit) {
	        if (dt == 0) dt = getustime ;
	        if ((rs = msgid_filecheck(op,dt,var.lread)) >= 0) {
		    char	*bep ;
	            if ((rs = msgid_searchid(op,midp,midl,&bep)) >= 0) {
	                ei = rs ;
	                if (ep) {
	                    msgide_all(ep,1,bep,MSGIDE_SIZE) ;
	                }
	            } /* end if */
	            if (dt == 0) dt = time(nullptr) ;
	            op->accesstime = dt ;
	        } /* end if (msgid_filecheck) */
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? ei : rs ;
}
/* end subroutine (msgid_matchid) */

int msgid_match(msgid *op,time_t dt,MSGID_KEY *kp,msgid_ent *ep) noex {
	int		rs ;
	int		ei = 0 ;
	if ((rs = msgid_magic(op,kp)) >= 0) {
	    rs = SR_FAULT ;
	    if (kp->recip && kp->mid) {
	    	rs = SR_NOTFOUND ;
	        if (op->f.fileinit) {
		    if (dt == 0) dt = getustime ;
	            if ((rs = msgid_filecheck(op,dt,var.lread)) >= 0) {
			uint	khash ;
			int	midlen = strnlen(kp->mid,var.lmsgid) ;
	        	khash = recipidhash(kp,-1,midlen) ;
			char	*bep ;
	        	if ((rs = msgid_search(op,kp,khash,&bep)) >= 0) {
	                    ei = rs ;
	                    if (ep) {
	                        msgide_all(ep,1,bep,MSGIDE_SIZE) ;
	                    }
	                } /* end if */
	                if (dt == 0) dt = time(nullptr) ;
	                op->accesstime = dt ;
		    } /* end if (msgid_filecheck) */
	        } /* end if (valid) */
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? ei : rs ;
}
/* end subroutine (msgid_match) */

/* update a (recipient,message-id) tuple entry match */
int msgid_update(msgid *op,time_t dt,MSGID_KEY *kp,msgid_ent *ep) noex {
	off_t		uoff ;
	uint		khash ;
	int		eoff ;
	int		rs ;
	int		wlen, midlen ;
	int		ei = 0 ;
	int		f_addition = false ;
	int		f_bufupdate = false ;
	char		ebuf[MSGIDE_SIZE + 4] ;
	char		*bep ;

#if	CF_SAFE
	if (op == nullptr) return SR_FAULT ;

	if (op->magic != MSGID_MAGIC) return SR_NOTOPEN ;
#endif /* CF_SAFE */

	if (kp == nullptr) return SR_FAULT ;
	if ((kp->recip == nullptr) || (kp->mid == nullptr)) return SR_FAULT ;

#ifdef	OPTIONAL
	if (ep == nullptr) return SR_FAULT ;
#endif

/* is the file even initialized? */

	if (! op->f.fileinit) return SR_NOTFOUND ;

/* do we have proper file access? */

	if (dt == 0) dt = time(nullptr) ;

	if ((rs = msgid_filecheck(op,dt,MSGID_LWRITE)) >= 0) {
	    if (op->f.fileinit) {

/* continue with the search */

	        midlen = strnlen(kp->mid,var.lmsgid) ;

	        khash = recipidhash(kp,-1,midlen) ;

	        if ((rs = msgid_search(op,kp,khash,&bep)) >= 0) {
	            MSGIDE_UPDATE	m1 ;
	            ei = rs ;

	            if (ep != nullptr) {
	                msgide_all(ep,1,bep,MSGIDE_SIZE) ;
	            }

/* update the file buffer */

	            msgide_update(&m1,1,bep,MSGIDE_SIZE) ;

	            m1.count += 1 ;
	            m1.utime = dt ;
	            wlen = msgide_update(&m1,0,bep,MSGIDE_SIZE) ;

	        } else if (rs == SR_NOTFOUND) {
	            struct oldentry	old ;
	            MSGIDE_ALL		m0{} ;
	            MSGIDE_UPDATE	m1{} ;

	            m0.count = 0 ;
	            m0.utime = dt ;
	            m0.mtime = kp->mtime ;
	            m0.ctime = dt ;
	            m0.hash = khash ;

	            if (kp->from != nullptr) {
	                strwcpy(m0.from,kp->from,var.lfrom) ;
	            }

	            strwcpy(m0.messageid,kp->mid,var.lmsgid) ;

	            strwcpy(m0.recipient,kp->recip,var.lrecip) ;

/* find an empty slot if there is one */

	            rs = msgid_searchempty(op,&old,&bep) ;
	            ei = rs ;
	            if (rs == SR_NOTFOUND) {
	                rs = SR_OK ;

	                bep = ebuf ;
	                f_bufupdate = true ;

	                if (op->h.nentries < op->maxentry) {
	                    f_addition = true ;
	                    ei = op->h.nentries ;
	                } else {
	                    ei = old.ei ;
	                }

	            } /* end if (entry not found) */

/* write to the buffer we have (either the file buffer or our own) */

	            wlen = msgide_all(&m0,0,bep,MSGIDE_SIZE) ;

	            if (ep != nullptr) {
	                msgide_all(ep,1,bep,MSGIDE_SIZE) ;
	            }

	            m0.count += 1 ;

/* also update the entry */

	            m1.count = 1 ;
	            m1.utime = m0.utime ;
	            msgide_update(&m1,0,bep,MSGIDE_SIZE) ;

/* update the in-core file buffer as needed or as appropriate */

	            if ((rs >= 0) && f_bufupdate && op->f.writable) {
	                eoff = MSGID_FOTAB + (ei * op->ebs) ;
	                msgid_bufupdate(op,eoff,wlen,ebuf) ;
	            }

	        } /* end if (match or not) */

	        if ((rs >= 0) && op->f.writable) {

/* write back this entry */

	            eoff = MSGID_FOTAB + (ei * op->ebs) ;
	            uoff = eoff ;
	            if ((rs = u_pwrite(op->fd,bep,wlen,uoff)) >= wlen) {

	                if (dt == 0) dt = time(nullptr) ;

	                op->h.wcount += 1 ;
	                op->h.wtime = dt ;
	                if (f_addition) {
	                    op->h.nentries += 1 ;
	                    op->filesize += wlen ;
	                }

	                rs = msgid_writehead(op) ;
	                if ((rs >= 0) && op->f.remote) {
	                    u_fsync(op->fd) ;
	                }

	            } /* end if (data write was successful) */

	        } /* end if (writing updated entry to file) */

/* update access time as appropriate */

	        if (dt == 0) dt = time(nullptr) ;
	        op->accesstime = dt ;

	    } else {
	        rs = SR_NOTOPEN ;
	    }
	} /* end if */

	return (rs >= 0) ? ei : rs ;
}
/* end subroutine (msgid_update) */

int msgid_check(msgid *op,time_t dt) noex {
	int		rs ;
	int		f = false ;
	if ((rs = msgid_magic(op)) >= 0) {
	    if (op->fd >= 0) {
	        if ((! op->f.readlocked) && (! op->f.writelocked)) {
	            f = f || ((dt - op->accesstime) > TO_ACCESS) ;
	            f = f || ((dt - op->opentime) > TO_OPEN) ;
	            if (f) {
	                rs = msgid_fileclose(op) ;
	            }
	        }
	    } /* end if (file was open) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (msgid_check) */


/* private subroutines */

static int msgid_opener(msgid *op,char *tbuf,cchar *fn,int of,mode_t om) noex {
	int		rs ;
	int		nof = (of & (~ O_CREAT)) ;
	int		f_create = false ;
	cchar		*ext = MSGID_FS ;
	rs = msgid_openone(op,tbuf,fn,ext,nof,om) ;
	if (isNotPresent(rs)) {
	    ext = MSGID_FSB ;
	    nof = (of & (~ O_CREAT)) ;
	    rs = msgid_openone(op,tbuf,fn,ext,nof,om) ;
	    if (isNotPresent(rs) && (of & O_CREAT)) {
	        ext = MSGID_FS ;
	        nof = of ;
	        f_create = true ;
	        rs = msgid_openone(op,tbuf,fn,ext,nof,om) ;
	        if (isNotPresent(rs)) {
	            ext = MSGID_FSB ;
	            nof = of ;
	            rs = msgid_openone(op,tbuf,fn,ext,nof,om) ;
	        }
	    }
	}
	return (rs >= 0) ? f_create : rs ;
}
/* end subroutine (msgid_opener) */

static int msgid_openone(msgid *op,char *tbuf,cchar *fn,cchar *ext,
		int of,mode_t om) noex {
	int		rs ;
	int		pl = 0 ;
	if ((rs = mkfnamesuf1(tbuf,fn,ext)) >= 0) {
	    pl = rs ;
	    if ((rs = u_open(tbuf,of,om)) >= 0) {
	        op->fd = rs ;
	    }
	}
	return (rs >= 0) ? pl : rs ;
}
/* end subroutine (sgid_openone) */

/* check the file for coherency */
static int msgid_filecheck(msgid *op,time_t dt,int f_read) noex {
	int		rs = SR_OK ;
	int		f_changed = false ;

/* is the file open */

	if (op->fd < 0) {
	    if (dt == 0) dt = time(nullptr) ;
	    rs = msgid_fileopen(op,dt) ;
	}

/* capture the lock if we do not already have it */

	if (rs >= 0) {
	    if ((! op->f.readlocked) && (! op->f.writelocked)) {
	        if (dt == 0) dt = time(nullptr) ;
	        if ((rs = msgid_lockget(op,dt,f_read)) >= 0) {
	            if ((rs = msgid_filechanged(op)) >= 0) {
	                f_changed = (rs > 0) ;
	            }
	            if (rs < 0) {
	                msgid_lockrelease(op) ;
		    }
	        } /* end if (capture lock) */
	    }
	} /* end if (ok) */

	return (rs >= 0) ? f_changed : rs ;
}
/* end subroutine (msgid_filecheck) */

/* initialize the file header (either read it only or write it) */
static int msgid_fileinit(msgid *op,time_t dt) noex {
	MSGID_FM	fm ;
	int		rs ;
	int		bl ;
	int		f_locked = false ;
	char		fbuf[MSGID_FBUFLEN + 1] ;

	if (op->filesize == 0) {

	    u_seek(op->fd,0L,SEEK_SET) ;

	    op->f.fileinit = false ;
	    if (op->f.writable) {

	        if (! op->f.writelocked) {
	            if ((rs = msgid_lockget(op,dt,0)) >= 0) {
	                f_locked = true ;
	            }
	        }
/* write the file magic and header stuff */
/* file magic */
	        if (rs >= 0) {

	            strwcpy(fm.magic,MSGID_FMB,14) ;
	            fm.vetu[0] = MSGID_FILEVERSION ;
	            fm.vetu[1] = MSGID_ENDIAN ;
	            fm.vetu[2] = 0 ;
	            fm.vetu[3] = 0 ;

	            bl = 0 ;
	            bl += filemagic((fbuf + bl),0,&fm) ;

/* file header */

	            memset(&op->h,0,sizeof(MSGID_FH)) ;

	            bl += filehead((fbuf + bl),0,&op->h) ;

/* write them to the file */

	            if ((rs = u_pwrite(op->fd,fbuf,bl,0L)) > 0) {
	                op->filesize = rs ;
	                op->mtime = dt ;
	                if (op->f.remote) u_fsync(op->fd) ;
	                rs = msgid_bufupdate(op,0,bl,fbuf) ;
	            }
	            op->f.fileinit = (rs >= 0) ;

	        } /* end if (ok) */

	    } /* end if (writing) */

	} else if (op->filesize >= MSGID_FOTAB) {

/* read the file header */

	    if (! op->f.readlocked) {
	        if ((rs = msgid_lockget(op,dt,1)) >= 0) {
	            f_locked = true ;
	        }
	    }

	    if (rs >= 0) {
	        cint		fltop = MSGID_FLTOP ;
	        if ((rs = u_pread(op->fd,fbuf,MSGID_FBUFLEN,0L)) >= fltop) {
		    bool	f ;
	            bl = 0 ;
	            bl += filemagic((fbuf + bl),1,&fm) ;
	            filehead((fbuf + bl),1,&op->h) ;
	            f = (strcmp(fm.magic,MSGID_FMB) == 0) ;
	            {
	                cint		v = MSGID_FILEVERSION ;
	                f = f && (fm.vetu[0] <= v) ;
	            }
	            f = f && (fm.vetu[1] == MSGID_ENDIAN) ;
	            if (! f) {
	                rs = SR_BADFMT ;
		    }
	            op->f.fileinit = f ;
	        } /* end if (u_pread) */
	    } /* end if (ok) */

	} /* end if */

/* if we locked, we unlock it, otherwise leave it! */

	if (f_locked) {
	    msgid_lockrelease(op) ;
	}

	return rs ;
}
/* end subroutine (msgid_fileinit) */

/* has the file changed at all? */
static int msgid_filechanged(msgid *op) noex {
	int		rs ;
	int		f_changed = false ;
	if (USTAT sb ; (rs = u_fstat(op->fd,&sb)) >= 0) {
	    if (sb.st_size < MSGID_FOTAB) {
	        op->f.fileinit = false ;
	    } 
	    f_changed = (! op->f.fileinit) ||
	        (sb.st_size != op->filesize) ||
	        (sb.st_mtime != op->mtime) ;

/* if it has NOT changed, read the file header for write indications */

	    if ((! f_changed) && op->f.fileinit) {
	        MSGID_FH	h ;
	        char		hbuf[MSGID_FLTOP + 1] ;

	        if ((rs = u_pread(op->fd,hbuf,MSGID_FLTOP,0L)) >= 0) {

	            if (rs < MSGID_FLTOP)
	                op->f.fileinit = false ;

	            if (rs > 0) {

	                filehead((hbuf + MSGID_FOHEAD),1,&h) ;

	                f_changed = (op->h.wtime != h.wtime) ||
	                    (op->h.wcount != h.wcount) ||
	                    (op->h.nentries != h.nentries) ;

	                if (f_changed)
	                    op->h = h ;

	            } /* end if (positive) */

	        } /* end if (u_pread) */

	    } /* end if (reading file header) */

/* OK, we're done */

	    if ((rs >= 0) && f_changed) {
	        op->b.len = 0 ;
	        op->filesize = sb.st_size ;
	        op->mtime = sb.st_mtime ;
	    }

	} /* end if (stat) */

	return (rs >= 0) ? f_changed : rs ;
}
/* end subroutine (msgid_filechanged) */

/* acquire access to the file */
static int msgid_lockget(msgid *op,time_t dt,int f_read) noex {
	int		rs = SR_OK ;

	if (op->fd < 0) {
	    rs = msgid_fileopen(op,dt) ;
	} /* end if (needed to open the file) */

/* acquire a file record lock */

	if (rs >= 0) {
	    int		lockcmd ;
	    int		f_already = false ;

	    if (f_read || (! op->f.writable)) {
	        f_already = op->f.readlocked ;
	        op->f.readlocked = true ;
	        op->f.writelocked = false ;
	        lockcmd = F_RLOCK ;
	    } else {
	        f_already = op->f.writelocked ;
	        op->f.readlocked = false ;
	        op->f.writelocked = true ;
	        lockcmd = F_WLOCK ;
	    }

/* get out if we have the lock that we want already */

	    if (! f_already) {
	        rs = lockfile(op->fd,lockcmd,0L,0L,TO_LOCK) ;
	    }

	} /* end if (ok) */

	return rs ;
}
/* end subroutine (msgid_lockget) */

static int msgid_lockrelease(msgid *op) noex {
	int		rs = SR_OK ;

	if ((op->f.readlocked || op->f.writelocked)) {
	    if (op->fd >= 0) {
	        rs = lockfile(op->fd,F_ULOCK,0L,0L,TO_LOCK) ;
	    }
	    op->f.readlocked = false ;
	    op->f.writelocked = false ;
	} /* end if */

	return rs ;
}
/* end subroutine (msgid_lockrelease) */

static int msgid_fileopen(msgid *op,time_t dt) noex {
	int		rs = SR_OK ;

	if (op->fd < 0) {
	    if ((rs = u_open(op->fname,op->oflags,op->operm)) >= 0) {
	        op->fd = rs ;
	        op->opentime = dt ;
	        rs = uc_closeonexec(op->fd,true) ;
	        if (rs < 0) {
	            u_close(op->fd) ;
	            op->fd = -1 ;
	        }
	    }
	} else {
	    rs = op->fd ;
	}

	return rs ;
}
/* end subroutine (msgid_fileopen) */

int msgid_fileclose(msgid *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->fd >= 0) {
	    rs1 = u_close(op->fd) ;
	    if (rs >= 0) rs = rs1 ;
	    op->fd = -1 ;
	}
	return rs ;
}
/* end subroutine (msgid_fileclose) */

/* search for an entry by MESSAGE-ID */
static int msgid_searchid(msgid *op,cchar *midp,int midl,char **bepp) noex {
	int		rs = SR_OK ;
	int		eoff ;
	int		len ;
	int		n ;
	int		i ;
	int		ne = 100 ;
	int		ei = 0 ;
	int		f = false ;
	char		*bp, *bep, *eidp ;

	while (! f) {

	    eoff = MSGID_FOTAB + (ei * op->ebs) ;
	    len = ne * op->ebs ;
	    rs = msgid_bufload(op,eoff,len,&bp) ;
	    if (rs < op->ebs) break ;

	    n = rs / op->ebs ;
	    for (i = 0 ; i < n ; i += 1) {
	        bep = bp + (i * op->ebs) ;
	        eidp = bep + MSGIDE_OMSGID ;
	        f = matfield(midp,midl,eidp,var.lmsgid) ;
	        if (f) break ;
	        ei += 1 ;
	    } /* end for */

	} /* end while */

	if ((rs >= 0) && f) {
	    *bepp = bep ;
	}

	if ((rs >= 0) && (! f)) {
	    rs = SR_NOTFOUND ;
	}

	return (rs >= 0) ? ei : rs ;
}
/* end subroutine (msgid_searchid) */

/* search by recipient AND message-id */
static int msgid_search(msgid *op,MSGID_KEY *kp,uint khash,char **bepp) noex {
	int		rs = SR_OK ;
	int		eoff ;
	int		len ;
	int		n ;
	int		i ;
	int		ne = 100 ;
	int		ei = 0 ;
	int		f = false ;
	char		*bp, *bep ;

	while (! f) {

	    eoff = MSGID_FOTAB + (ei * op->ebs) ;
	    len = ne * op->ebs ;
	    rs = msgid_bufload(op,eoff,len,&bp) ;

	    if (rs < op->ebs) break ;

	    bep = bp ;
	    n = rs / op->ebs ;

	    for (i = 0 ; i < n ; i += 1) {

#if	CF_HASH

	        {
	            uint	uiw ;
	            stdorder_rui((bep + MSGIDE_OHASH),&uiw) ;
	            f = (khash == uiw) ;
	        }

	        if (f) {
	            f = emat_recipid(bep,kp) ;
	            if (f) break ;
	        }

#else /* CF_HASH */

	        f = emat_recipid(bep,kp) ;
	        if (f) break ;
#endif /* CF_HASH */

	        bep += op->ebs ;
	        ei += 1 ;

	    } /* end for */

	} /* end while */

	*bepp = bep ;
	if ((rs >= 0) && (! f)) {
	    rs = SR_NOTFOUND ;
	}

	return (rs >= 0) ? ei : rs ;
}
/* end subroutine (msgid_search) */

/* find an empty slot */
static int msgid_searchempty(msgid *op,struct oldentry *oep,char **bepp) noex {
	uint		ra, rb ;
	int		rs = SR_NOTFOUND ;
	int		ei_start = 0 ;
	int		ei_mark ;
	int		n = 0 ;
	int		ei = 0 ;

	oep->utime = INT_MAX ;
	oep->ei = 0 ;

	if (op->b.off >= MSGID_FLTOP) {

	    ra = uceil((op->b.off - MSGID_FLTOP),op->ebs) ;

	    rb = ufloor((op->b.off + op->b.len - MSGID_FLTOP),op->ebs) ;

	    ei_start = ra / op->ebs ;
	    n = (rb > ra) ? ((rb - ra) / op->ebs) : 0 ;

	} /* end if */

	ei_mark = ei_start ;
	if (n > 0) {

	    rs = msgid_searchemptyrange(op,ei_start,n,oep,bepp) ;
	    ei = rs ;
	    if (rs < 0)
	        ei_start += n ;

	} /* end if */

	n = (op->h.nentries - ei_start) ;
	if ((rs == SR_NOTFOUND) && (n > 0)) {
	    rs = msgid_searchemptyrange(op,ei_start,n,oep,bepp) ;
	    ei = rs ;

	} /* end if */

	if ((rs == SR_NOTFOUND) && (ei_mark > 0)) {

	    ei_start = 0 ;
	    n = ei_mark ;

	    rs = msgid_searchemptyrange(op,ei_start,n,oep,bepp) ;
	    ei = rs ;

	} /* end if */

	return (rs >= 0) ? ei : rs ;
}
/* end subroutine (msgid_searchempty) */

/* search for an empty slot in a specified range of entries */
static int msgid_searchemptyrange(msgid *op,int ei,int nmax,
		struct oldentry *oep,char **bepp) noex {
	time_t		t ;
	int		rs = SR_OK ;
	int		eoff ;
	int		len ;
	int		i ;
	int		ne, n ;
	int		f = false ;
	char		*bp, *bep, *eidp ;

	while (! f) {

	    ne = MIN(nmax,100) ;

	    eoff = MSGID_FOTAB + (ei * op->ebs) ;
	    len = ne * op->ebs ;
	    rs = msgid_bufload(op,eoff,len,&bp) ;
	    if (rs < op->ebs) break ;

	    n = (rs / op->ebs) ;
	    for (i = 0 ; i < n ; i += 1) {
	        bep = bp + (i * op->ebs) ;
	        eidp = bep + MSGIDE_OMSGID ;
	        f = (*eidp == '\0') ;
	        if (f) break ;
	        if ((t = extutime(bep)) < oep->utime) {
	            oep->utime = t ;
	            oep->ei = ei ;
	        }
	        ei += 1 ;
	    } /* end for */

	} /* end while */

	*bepp = bep ;
	if ((rs >= 0) && (! f)) {
	    rs = SR_NOTFOUND ;
	}

	return (rs >= 0) ? ei : rs ;
}
/* end subroutine (msgid_searchemptyrange) */

/* buffer mangement stuff */
static int msgid_bufbegin(msgid *op) noex {
	cint		size =  MSGID_BUFSIZE ;
	int		rs ;
	char		*bp ;

	op->b.off = 0 ;
	op->b.len = 0 ;
	op->b.mlen = 0 ;
	op->b.mbuf = nullptr ;
	if ((rs = uc_malloc(size,&bp)) >= 0) {
	    op->b.mlen = size ;
	    op->b.mbuf = bp ;
	}

	return rs ;
}
/* end subroutine (msgid_bufbegin) */

static int msgid_bufend(msgid *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (op->b.mbuf != nullptr) {
	    rs1 = uc_free(op->b.mbuf) ;
	    if (rs >= 0) rs = rs1 ;
	    op->b.mbuf = nullptr ;
	}

	op->b.mlen = 0 ;
	op->b.len = 0 ;
	return rs ;
}
/* end subroutine (msgid_bufend) */

/* try to buffer up some of the file */
static int msgid_bufload(msgid *op,int roff,int rlen,char **rpp) noex {
	off_t		poff ;
	uint		bend, fext ;
	uint		foff ;
	uint		rext = (roff + rlen), ext ;
	int		rs = SR_OK ;
	int		len = 0 ;
	char		*rbuf ;

/* do we need to read in more data? */

	len = rlen ;
	fext = op->b.off + op->b.len ;
	if ((roff < op->b.off) || (rext > fext)) {

/* can we do an "add-on" type read operation? */

	    bend = op->b.off + op->b.mlen ;
	    if ((roff >= op->b.off) && (rext <= bend)) {

	        foff = op->b.off + op->b.len ;
	        rbuf = op->b.mbuf + op->b.len ;

	        ext = roff + MAX(rlen,MSGID_READSIZE) ;
	        fext = uceil(ext,op->pagesize) ;

	        if (fext > bend)
	            fext = bend ;

	        len = fext - foff ;
	        poff = foff ;
	        rs = u_pread(op->fd,rbuf,len,poff) ;

	        if (rs >= 0) {
	            op->b.len += rs ;
	            len = MIN(((op->b.off + op->b.len) - roff),rlen) ;
	        } /* end if */

	    } else {
	        op->b.off = roff ;
	        op->b.len = 0 ;
	        bend = roff + op->b.mlen ;
	        foff = roff ;
	        rbuf = op->b.mbuf ;

	        ext = roff + MAX(rlen,MSGID_READSIZE) ;
	        fext = uceil(ext,op->pagesize) ;

	        if (fext > bend)
	            fext = bend ;

	        len = fext - foff ;
	        if ((rs = u_pread(op->fd,op->b.mbuf,len,foff)) >= 0) {
	            op->b.len = rs ;
	            len = MIN(rs,rlen) ;
	        }

	    } /* end if */

	} /* end if (needed to read more data) */

	if (rpp) {
	    *rpp = op->b.mbuf + (roff - op->b.off) ;
	}

	return (rs >= 0) ? len : rs ;
}
/* end subroutine (msgid_buf) */

/* update the file buffer from a user supplied source */
static int msgid_bufupdate(msgid *op,int roff,int rbuflen,cchar *rbuf) noex {
	int		boff, bext ;
	int		rext = (roff + rbuflen) ;
	int		buflen, bdiff ;
	int		f_done = false ;

	buflen = op->b.len ;
	boff = op->b.off ;
	bext = boff + buflen ;

	if (roff < boff) {

	    if (rext > boff) {
	        rbuf += (boff - roff) ;
	        rbuflen -= (boff - roff) ;
	        roff = boff ;
	    } else {
		rbuflen = 0 ;
		f_done = true ;
	    }

	} /* end if */

	if ((! f_done) && (rext > bext)) {

	    if (roff < bext) {
	        rbuflen -= (rext - bext) ;
	        rext = bext ;
	    } else {
		rbuflen = 0 ;
		f_done = true ;
	    }

	} /* end if */

	if ((! f_done) && (rbuflen > 0)) {
	    bdiff = roff - boff ;
	    memcpy((op->b.mbuf + bdiff),rbuf,rbuflen) ;
	}

	return rbuflen ;
}
/* end subroutine (msgid_bufupdate) */

/* write out the file header */
static int msgid_writehead(msgid *op) noex {
	off_t		uoff = MSGID_FOHEAD ;
	int		rs ;
	int		bl ;
	char		fbuf[MSGID_FBUFLEN + 1] ;

	bl = filehead(fbuf,0,&op->h) ;
	rs = u_pwrite(op->fd,fbuf,bl,uoff) ;

	return rs ;
}
/* end subroutine (msgid_writehead) */

static int filemagic(char *buf,int f_read,MSGID_FM *mp) noex {
	int		rs = 20 ;
	char		*bp = buf ;

	if (buf == nullptr) return SR_BADFMT ;

	if (f_read) {
	    char	*ep ;
	    bp[15] = '\0' ;
	    strncpy(mp->magic,bp,15) ;
	    if ((ep = strchr(mp->magic,'\n')) != nullptr) {
	        *ep = '\0' ;
	    }
	    bp += 16 ;
	    memcpy(mp->vetu,bp,4) ;
	} else {
	    bp = strwcpy(bp,mp->magic,14) ;
	    *bp++ = '\n' ;
	    memset(bp,0,(16 - (bp - buf))) ;
	    bp = (buf + 16) ;
	    memcpy(bp,mp->vetu,4) ;
	} /* end if */

	return rs ;
}
/* end subroutine (filemagic) */

/* encode or decode the file header */
static int filehead(char *mbuf,int f_read,MSGID_FH *hp) noex {
	serialbuf	msgbuf ;
	cint		mlen = sizeof(MSGID_FH) ;
	int		rs ;
	int		rs1 ;

	if ((rs = serialbuf_start(&msgbuf,mbuf,mlen)) >= 0) {

	    if (f_read) { /* read */
	        serialbuf_rui(&msgbuf,&hp->wcount) ;
	        serialbuf_rui(&msgbuf,&hp->wtime) ;
	        serialbuf_rui(&msgbuf,&hp->nentries) ;
	    } else { /* write */
	        serialbuf_wui(&msgbuf,hp->wcount) ;
	        serialbuf_wui(&msgbuf,hp->wtime) ;
	        serialbuf_wui(&msgbuf,hp->nentries) ;
	    } /* end if */

	    rs1 = serialbuf_finish(&msgbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (serialbuf) */

	return rs ;
}
/* end subroutine (filehead) */

static int emat_recipid(cchar *ep,MSGID_KEY *kp) noex {
	int		f ;
	cchar		*fp ;

	fp = ep + MSGIDE_OMSGID ;
	f = matfield(kp->mid,-1,fp,var.lmsgid) ;

	if (f) {
	    fp = ep + MSGIDE_ORECIP ;
	    f = matfield(kp->recip,-1,fp,var.lrecip) ;
	}

	return f ;
}
/* end subroutine (emat_recipid) */

static int matfield(cchar *mp,int ml,cchar *ep,int el) noex {
	int		f = 0 ;
	if (ml >= 0) {
	    f = (strncmp(mp,ep,MIN(ml,el)) == 0) ;
	    if (f && (ml < el)) {
	        f = (ep[ml] == '\0') ;
	    }
	} else {
	    f = (strncmp(mp,ep,el) == 0) ;
	}
	return f ;
}
/* end subroutine (matfield) */

static int extutime(char *ep) noex {
	MSGIDE_UPDATE	m1 ;
	int		rs ;
	rs = msgide_update(&m1,1,ep,MSGIDE_SIZE) ;
	return (rs >= 0) ? m1.utime : rs ;
}
/* end subroutine (extutime) */

static uint recipidhash(MSGID_KEY *kp,int reciplen,int midlen) noex {
	uint		khash = hash_elf(kp->recip,reciplen) ;
	khash += hash_elf(kp->mid,midlen) ;
	return khash ;
}
/* end subroutine (recipidhash) */

int vars::mkvars() noex {
        int		rs ;
	if ((rs = getbufsize(getbufsize_mp)) >= 0) {
	    cint	hostnamelen = rs ;
	    lcount = MSGIDE_LCOUNT ;
	    lctime = MSGIDE_LUTIME ;
	    lmtime = MSGIDE_LMTIME ;
	    lhash = MSGIDE_LHASH ;
	    lrecip = MSGIDE_LRECIP ;
	    lmsgid = (2 * hostnamelen) ;
	    lfrom = (2 * hostnamelen) ;
	    lread = MSGID_LREAD ;
	} /* end if (getbufsize) */
	return rs ;
}
/* end subroutine (vars::mkvars) */



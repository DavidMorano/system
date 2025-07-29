/* msgid SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* manage message-id storage */
/* version %I% last-modified %G% */

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
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>		/* |INT_MAX| */
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<new>			/* |nothrow(3c++)| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<getbufsize.h>
#include	<mallocxx.h>
#include	<endian.h>
#include	<lockfile.h>
#include	<serialbuf.h>
#include	<stdorder.h>
#include	<mkfname.h>
#include	<mkx.h>
#include	<intfloor.h>
#include	<intceil.h>
#include	<hash.h>
#include	<strwcpy.h>
#include	<funmode.hh>
#include	<strnxcmp.h>		/* |strnncmp(3uc)| */
#include	<isfiledesc.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"msgid.h"

import libutil ;

/* local defines */

#define	MSGID_FMA	"MSGIDA"
#define	MSGID_FMB	"MSGIDB"

#define	MSGID_FS	"msgid"
#define	MSGID_FSA	"msgida"
#define	MSGID_FSB	"msgidb"

#define	MSGID_FLID	(MSGID_MAGICSIZE + 4)
#define	MSGID_FLHEAD	(3 * 4)
#define	MSGID_FLTOP	(MSGID_FLID + MSGID_FLHEAD)

#define	MSGID_FOID	0
#define	MSGID_FOHEAD	(MSGID_FOID + MSGID_FLID)
#define	MSGID_FOTAB	(MSGID_FOHEAD + MSGID_FLHEAD)

#define	MSGID_BUFSIZE	(64 * 1024)
#define	MSGID_READSIZE	(16 * 1024)

#define	MSGID_FBUFLEN	(MSGID_FLTOP + 9)

#define	MSGID_LWRITE	0
#define	MSGID_LREAD	1

#define	TO_OPEN		(60 * 60)	/* maximum file-open time */
#define	TO_ACCESS	(2 * 60)	/* maximum access idle time */
#define	TO_LOCK		60		/* seconds */

#ifndef	CF_CREAT
#define	CF_CREAT	0		/* always create the file? */
#endif
#ifndef	CF_HASH
#define	CF_HASH		1		/* use hash for faster lookup */
#endif


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */

typedef msgid_key	key ;
typedef const funmode	cfm ;


/* external subroutines */


/* external variables */


/* local structures */

struct oldent {
	time_t		utime ;
	int		ei ;
} ;

namespace {
    struct vars {
	int		hostnamelen ;
	int		pagesize ;
	int		entsz ;
	int		lrecip ;
	int		lmsgid ;
	operator int () noex ;
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

static int	msgid_opens(msgid *,cc *,int,mode_t) noex ;
static int	msgid_opener(msgid *,char *,cchar *,int,mode_t) noex ;
static int	msgid_openone(msgid *,char *,cchar *,cchar *,int,mode_t) noex ;

static int	msgid_fileopen(msgid *,time_t) noex ;
static int	msgid_fileclose(msgid *) noex ;
static int	msgid_lockacquire(msgid *,time_t,cfm) noex ;
static int	msgid_lockrelease(msgid *) noex ;
static int	msgid_fileinit(msgid *,time_t) noex ;
static int	msgid_filechanged(msgid *) noex ;
static int	msgid_filecheck(msgid *,time_t,cfm) noex ;
static int	msgid_searchid(msgid *,cchar *,int,char **) noex ;
static int	msgid_search(msgid *,msgid_key *,uint,char **) noex ;
static int	msgid_searchempty(msgid *,oldent *,char **) noex ;
static int	msgid_searchemptyrange(msgid *,int,int,oldent *,char **) noex ;
static int	msgid_bufload(msgid *,int,int,char **) noex ;
static int	msgid_bufupdate(msgid *,int,int,cchar *) noex ;
static int	msgid_bufbegin(msgid *) noex ;
static int	msgid_bufend(msgid *) noex ;
static int	msgid_writehead(msgid *) noex ;

static int	filemagic(MSGID_FM *,cfm,char *) noex ;
static int	filehead(MSGID_FH *,cfm,char *) noex ;

static bool	keymat(msgid_key *,msgide *) noex ;
static uint	keyhash(msgid_key *) noex ;


/* local variables */

static vars	var ;

cint		magsz = MSGID_MAGICSIZE ;

cbool		f_creat = CF_CREAT ;
cbool		f_hash = CF_HASH ;


/* exported variables */


/* exported subroutines */

int msgid_open(msgid *op,cchar *fname,int of,mode_t om,int maxentry) noex {
	int		rs ;
	int		f_create = false ;
	if ((rs = msgid_ctor(op,fname)) >= 0) {
	    rs = SR_INVALID ;
	    if (fname[0]) {
		static cint	rsv = var ;
		if ((rs = rsv) >= 0) {
		    if_constexpr (f_creat) {
			of |= O_CREAT ;
		    }
		    op->fd = -1 ;
		    op->oflags = of ;
		    op->operm = om ;
	            op->pagesize = var.pagesize ;
		    op->maxentry = maxentry ;
		    op->entsz = uceil(var.entsz,szof(uint)) ;
		    rs = msgid_opens(op,fname,of,om) ;
		    f_create = rs ;
		} /* end if (vars) */
	    } /* end if (valid) */
	    if (rs < 0) {
		msgid_dtor(op) ;
	    }
	} /* end if (msgid_ctor) */
	return (rs >= 0) ? f_create : rs ;
}
/* end subroutine (msgid_open) */

static int msgid_opens(msgid *op,cc *fname,int of,mode_t om) noex {
    	int		rs ;
	int		f_create = false ;
	if ((rs = msgid_bufbegin(op)) >= 0) {
	    if (char *tbuf ; (rs = malloc_mp(&tbuf)) >= 0) {
	        custime		dt = getustime ;
	        if ((rs = msgid_opener(op,tbuf,fname,of,om)) >= 0) {
	            f_create = rs ;
	            op->opentime = dt ;
	            op->accesstime = dt ;
	            if (cchar *fn ; (rs = uc_mallocstrw(tbuf,-1,&fn)) >= 0) {
			cint	am = (of & O_ACCMODE) ;
			op->fname = fn ;
			op->fl.writable = ((am == O_WRONLY) || (am == O_RDWR)) ;
	                if (USTAT sb ; (rs = u_fstat(op->fd,&sb)) >= 0) {
	                    if (S_ISREG(sb.st_mode)) {
	                        op->mtime = sb.st_mtime ;
	                        op->filesize = intsat(sb.st_size) ;
	                        if ((rs = isfsremote(op->fd)) >= 0) {
	                            op->fl.remote = (rs > 0) ;
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
	        } /* end if (msgid_opener) */
	        rs = rsfree(rs,tbuf) ;
	    } /* end if (m-a-f) */
	    if (rs < 0) {
	        msgid_bufend(op) ;
	    }
	} /* end if (buffer-start) */
	return (rs >= 0) ? f_create : rs ;
}
/* end subroutine (msgid_opens) */

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
	    c = ((op->filesize - MSGID_FOTAB) / op->entsz) ;
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (msgid_count) */

int msgid_curbegin(msgid *op,msgid_cur *curp) noex {
    	int		rs ;
	if ((rs = msgid_magic(op,curp)) >= 0) {
	    op->cursors += 1 ;
	    op->fl.cursorlockbroken = false ;
	    op->fl.cursoracc = false ;
	    curp->i = -1 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (msgid_curbegin) */

int msgid_curend(msgid *op,msgid_cur *curp) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = msgid_magic(op,curp)) >= 0) {
	    bool	f = true ;
	    if (op->fl.cursoracc) {
	        op->accesstime = getustime ;
	    }
	    if (op->cursors > 0) {
	        op->cursors -= 1 ;
	    }
	    f = f && (op->cursors == 0) ;
	    f = f && (op->fl.rdlocked || op->fl.wrlocked) ;
	    if (f) {
	        rs1 = msgid_lockrelease(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    curp->i = -1 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (msgid_curend) */

int msgid_curenum(msgid *op,msgid_cur *curp,msgide *ep) noex {
	custime		dt = getustime ;
	int		rs ;
	int		ei = 0 ;
	if ((rs = msgid_magic(op,curp)) >= 0) {
	    rs = SR_NOTFOUND ;
	    if (op->fl.fileinit) {
	    	rs = SR_LOCKLOST ;
	        if (! op->fl.cursorlockbroken) {
		    cfm		fc = funmode::rd ;
		    cint	ebs = op->entsz ;
		    int		eoff ;
		    if ((rs = msgid_filecheck(op,dt,fc)) >= 0) {
	        	ei = (curp->i < 0) ? 0 : curp->i + 1 ;
	        	eoff = MSGID_FOTAB + (ei * ebs) ;
			/* form result to caller */
	        	if ((eoff + ebs) <= op->filesize) {
		    	    char	*bep ;
	                    if ((rs = msgid_bufload(op,eoff,ebs,&bep)) >= 0) {
	                        if (rs >= ebs) {
	                            if (ep && bep) {
					rs = ep->wr(bep) ;
	                            } /* end if */
			            /* commit the cursor movement? */
	                            if (rs >= 0) {
	                                curp->i = ei ;
			            }
	                            op->fl.cursoracc = true ;
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

int msgid_matchid(msgid *op,time_t dt,cchar *midp,int midl,msgide *ep) noex {
	int		rs ;
	int		ei = 0 ;
	if ((rs = msgid_magic(op,midp)) >= 0) {
	    rs = SR_NOTFOUND ;
	    if (op->fl.fileinit) {
		cfm	fc = funmode::rd ;
	        if (dt == 0) dt = getustime ;
	        if ((rs = msgid_filecheck(op,dt,fc)) >= 0) {
		    char	*bep ;
	            if ((rs = msgid_searchid(op,midp,midl,&bep)) >= 0) {
	                ei = rs ;
	                if (ep && bep) {
	                    rs = ep->wr(bep) ;
	                }
	            } /* end if */
	            if (dt == 0) dt = getustime ;
	            op->accesstime = dt ;
	        } /* end if (msgid_filecheck) */
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? ei : rs ;
}
/* end subroutine (msgid_matchid) */

int msgid_match(msgid *op,time_t dt,msgid_key *kp,msgide *ep) noex {
	int		rs ;
	int		ei = 0 ;
	if ((rs = msgid_magic(op,kp)) >= 0) {
	    rs = SR_FAULT ;
	    if (kp->recip && kp->msgid) {
	    	rs = SR_NOTFOUND ;
	        if (op->fl.fileinit) {
		    cfm		fc = funmode::rd ;
		    if (dt == 0) dt = getustime ;
	            if ((rs = msgid_filecheck(op,dt,fc)) >= 0) {
			cuint	khash = keyhash(kp) ;
			char	*bep ;
	        	if ((rs = msgid_search(op,kp,khash,&bep)) >= 0) {
	                    ei = rs ;
	                    if (ep && bep) {
	                        rs = ep->wr(bep) ;
	                    }
	                } /* end if */
	                if (dt == 0) dt = getustime ;
	                op->accesstime = dt ;
		    } /* end if (msgid_filecheck) */
	        } /* end if (valid) */
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? ei : rs ;
}
/* end subroutine (msgid_match) */

/* update a (recipient,message-id) tuple entry match */
static int msgid_updates(msgid *,time_t,msgid_key *,msgide *) noex ;

int msgid_update(msgid *op,time_t dt,msgid_key *kp,msgide *ep) noex {
	int		rs ;
	int		ei = 0 ;
	if ((rs = msgid_magic(op,kp,ep)) >= 0) {
	    rs = SR_FAULT ;
	    if (kp->recip && kp->msgid) {
	        rs = SR_NOTFOUND ;
		if (op->fl.fileinit) {
		    cfm		fc = funmode::wr ;
        	    if (dt == 0) dt = getustime ;
        	    if ((rs = msgid_filecheck(op,dt,fc)) >= 0) {
		        rs = msgid_updates(op,dt,kp,ep) ;
			ei = rs ;
		    }
	        } /* end if (valid) */
	    } /* end if (non-null) */
	} /* end if (magic) */
	return (rs >= 0) ? ei : rs ;
}
/* end subroutine (msgid_update) */

static int msgid_updates(msgid *op,time_t dt,msgid_key *kp,msgide *ep) noex {
    	int		rs ;
	int		rs1 ;
	int		ei = 0 ;
	if (msgide ew ; (rs = ew.start) >= 0) {
	    cuint	khash = keyhash(kp) ;
	    off_t	uoff ;
	    int		eoff ;
	    int		wlen{} ;
	    bool	f_addition = false ;
	    bool	f_bufupdate = false ;
	    char	*bep ;
            if ((rs = msgid_search(op,kp,khash,&bep)) >= 0) {
                ei = rs ;
                if (ep) {
                    rs = ep->wr(bep) ;
                }
                /* update the file buffer */
	        if ((rs >= 0) && ((rs = ew.wr(bep)) >= 0)) {
                    ew.count += 1 ;
                    ew.utime = uint(dt) ;
	            rs = ew.rd(bep) ;	/* <- update */
                    wlen = rs ;
	        } /* end if (msgide_wr) */
            } else if (rs == SR_NOTFOUND) {
	        char	ebuf[op->entsz + 4] ;
                ew.count = 0 ;
                ew.utime = uint(dt) ;
                ew.mtime = uint(dt) ;
                ew.ctime = uint(dt) ;
                ew.hash = khash ;
		ew.loadrec(kp->recip) ;
		ew.loadmid(kp->msgid) ;
                /* find an empty slot if there is one */
                if (oldent old ; (rs = msgid_searchempty(op,&old,&bep)) >= 0) {
                    ei = rs ;
	        } else if (rs == SR_NOTFOUND) {
		    const uint	maxents = op->maxentry ;
                    rs = SR_OK ;
                    bep = ebuf ;
                    f_bufupdate = true ;
                    if (op->h.nentries < maxents) {
                        f_addition = true ;
                        ei = int(op->h.nentries) ;
                    } else {
                        ei = old.ei ;
                    }
                } /* end if (entry not found) */
                /* write the buffer we have (either file buffer or our own) */
		if ((rs >= 0) && ((rs = ew.rd(bep)) >= 0)) {
                    if (ep) {
                        rs = ep->wr(bep) ;
                    }
		    if (rs >= 0) {
                        ew.count += 1 ;
                        ew.utime = uint(dt) ;
                        if ((rs = ew.rd(bep)) >= 0) {
			    if (f_bufupdate && op->fl.writable) {
                                eoff = MSGID_FOTAB + (ei * op->entsz) ;
                                rs = msgid_bufupdate(op,eoff,wlen,ebuf) ;
			    }
                        }
		    } /* end if (ok) */
		} /* end if (msgide_rd) */
            } /* end if (match or not) */
            if ((rs >= 0) && op->fl.writable) {
                /* write back this entry */
                eoff = MSGID_FOTAB + (ei * op->entsz) ;
                uoff = eoff ;
                if ((rs = u_pwrite(op->fd,bep,wlen,uoff)) >= wlen) {
                    op->h.wcount += 1 ;
                    op->h.wtime = uint(dt) ;
                    if (f_addition) {
                        op->h.nentries += 1 ;
                        op->filesize += wlen ;
                    }
                    if ((rs = msgid_writehead(op)) >= 0) {
			if (op->fl.remote) {
                            rs = u_fsync(op->fd) ;
                        }
		    }
                } /* end if (data write was successful) */
            } /* end if (writing updated entry to file) */
            /* update access time as appropriate */
            op->accesstime = dt ;
	    rs1 = ew.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (msgide) */
	return (rs >= 0) ? ei : rs ;
}
/* end subroutine (msgid_updates) */

int msgid_check(msgid *op,time_t dt) noex {
	int		rs ;
	int		f = false ;
	if ((rs = msgid_magic(op)) >= 0) {
	    if (op->fd >= 0) {
	        if ((! op->fl.rdlocked) && (! op->fl.wrlocked)) {
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
	if ((rs = msgid_openone(op,tbuf,fn,ext,nof,om)) >= 0) {
	    rs = 0 ;
	} else if (isNotPresent(rs)) {
	    ext = MSGID_FSB ;
	    nof = (of & (~ O_CREAT)) ;
	    if ((rs = msgid_openone(op,tbuf,fn,ext,nof,om)) >= 0) {
		rs = 0 ;
	    } else if (isNotPresent(rs) && (of & O_CREAT)) {
	        ext = MSGID_FS ;
	        nof = of ;
	        f_create = true ;
	        if ((rs = msgid_openone(op,tbuf,fn,ext,nof,om)) >= 0) {
		    rs = 0 ;
		} else if (isNotPresent(rs)) {
	            ext = MSGID_FSB ;
	            nof = of ;
	            rs = msgid_openone(op,tbuf,fn,ext,nof,om) ;
	        }
	    }
	}
	return (rs >= 0) ? f_create : rs ;
}
/* end subroutine (msgid_opener) */

static int msgid_openone(msgid *op,char *tbuf,cc *fn,cc *ext,
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
static int msgid_filecheck(msgid *op,time_t dt,cfm fc) noex {
	int		rs = SR_OK ;
	int		f_changed = false ;
	/* is the file open */
	if (op->fd < 0) {
	    if (dt == 0) dt = getustime ;
	    rs = msgid_fileopen(op,dt) ;
	}
	/* capture the lock if we do not alrecipy have it */
	if (rs >= 0) {
	    if ((! op->fl.rdlocked) && (! op->fl.wrlocked)) {
	        if (dt == 0) dt = getustime ;
	        if ((rs = msgid_lockacquire(op,dt,fc)) >= 0) {
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
	int		rs = SR_OK ;
	int		bl ;
	int		f_locked = false ;
	char		fbuf[MSGID_FBUFLEN + 1] ;
	if (op->filesize == 0) {
	    u_rewind(op->fd) ;
	    op->fl.fileinit = false ;
	    if (op->fl.writable) {
	        if (! op->fl.wrlocked) {
		    cfm		fc = funmode::wr ;
	            if ((rs = msgid_lockacquire(op,dt,fc)) >= 0) {
	                f_locked = true ;
	            }
	        }
		/* write the file magic and header stuff */
		/* file magic */
	        if (rs >= 0) {
		    if ((rs = mkmagic(fm.magic,magsz,MSGID_FMB)) >= 0) {
		        cfm	fc = funmode::rd ;
	                fm.vetu[0] = MSGID_FILEVERSION ;
	                fm.vetu[1] = MSGID_ENDIAN ;
	                fm.vetu[2] = 0 ;
	                fm.vetu[3] = 0 ;
	                bl = 0 ;
	                if ((rs = filemagic(&fm,fc,(fbuf + bl))) >= 0) {
		            bl += rs ;
	                    op->h = {} ;
			    char	*headp = (fbuf + bl) ;
	                    if ((rs = filehead(&op->h,fc,headp)) >= 0) {
			        bl += rs ;
		    	        /* write them to the file */
	            	        if ((rs = u_pwrite(op->fd,fbuf,bl,0z)) > 0) {
				    op->filesize = rs ;
	                            op->mtime = dt ;
	                            if (op->fl.remote) u_fsync(op->fd) ;
	                            rs = msgid_bufupdate(op,0,bl,fbuf) ;
	                        }
	                        op->fl.fileinit = (rs >= 0) ;
	                    } /* end if (filehead) */
		        } /* end if (filemagic) */
		    } /* end if (mkmagic) */
		} /* end if (ok) */
	    } /* end if (writing) */
	} else if (op->filesize >= MSGID_FOTAB) {
	    /* read the file header */
	    if (! op->fl.rdlocked) {
		cfm	fc = funmode::rd ;
	        if ((rs = msgid_lockacquire(op,dt,fc)) >= 0) {
	            f_locked = true ;
	        }
	    }
	    if (rs >= 0) {
	        cint		fltop = MSGID_FLTOP ;
	        if ((rs = u_pread(op->fd,fbuf,MSGID_FBUFLEN,0z)) >= fltop) {
		    cfm		fc = funmode::wr ;
		    bl = 0 ;
	            if ((rs = filemagic(&fm,fc,(fbuf + bl))) >= 0) {
		        bool	f ;
		    	bl += rs ;
			if ((rs = filehead(&op->h,fc,(fbuf + bl))) >= 0) {
		   	    bl += rs ;
	                    f = (strcmp(fm.magic,MSGID_FMB) == 0) ;
	                    {
	                        cint	v = MSGID_FILEVERSION ;
	                        f = f && (fm.vetu[0] <= v) ;
	                    }
	                    f = f && (fm.vetu[1] == MSGID_ENDIAN) ;
	                    if (! f) {
	                        rs = SR_BADFMT ;
		            }
	                    op->fl.fileinit = f ;
			} /* end if (filehead) */
		    } /* end if (filemagic) */
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
	        op->fl.fileinit = false ;
	    } 
	    f_changed = f_changed || (! op->fl.fileinit) ;
	    f_changed = f_changed || (sb.st_size != op->filesize) ;
	    f_changed = f_changed || (sb.st_mtime != op->mtime) ;
	    /* if it has NOT changed, read file header for writes */
	    if ((! f_changed) && op->fl.fileinit) {
	        MSGID_FH	h ;
	        char		hbuf[MSGID_FLTOP + 1] ;
	        if ((rs = u_pread(op->fd,hbuf,MSGID_FLTOP,0z)) >= 0) {
	            if (rs < MSGID_FLTOP) {
	                op->fl.fileinit = false ;
		    }
	            if (rs > 0) {
	                filehead(&h,funmode::wr,(hbuf + MSGID_FOHEAD)) ;
	                f_changed = (op->h.wtime != h.wtime) ||
	                    (op->h.wcount != h.wcount) ||
	                    (op->h.nentries != h.nentries) ;

	                if (f_changed) {
	                    op->h = h ;
			}
	            } /* end if (positive) */
	        } /* end if (u_pread) */
	    } /* end if (reading file header) */
	    /* OK, we are done */
	    if ((rs >= 0) && f_changed) {
	        op->b.len = 0 ;
	        op->filesize = intsat(sb.st_size) ;
	        op->mtime = sb.st_mtime ;
	    }
	} /* end if (stat) */
	return (rs >= 0) ? f_changed : rs ;
}
/* end subroutine (msgid_filechanged) */

/* acquire access to the file */
static int msgid_lockacquire(msgid *op,time_t dt,cfm fc) noex {
	int		rs = SR_OK ;
	if (op->fd < 0) {
	    rs = msgid_fileopen(op,dt) ;
	} /* end if (needed to open the file) */
	/* acquire a file record lock */
	if (rs >= 0) {
	    int		lockcmd ;
	    bool	f_already = false ;
	    if ((fc == funmode::rd) || (! op->fl.writable)) {
	        f_already = op->fl.rdlocked ;
	        op->fl.rdlocked = true ;
	        op->fl.wrlocked = false ;
	        lockcmd = F_RLOCK ;
	    } else {
	        f_already = op->fl.wrlocked ;
	        op->fl.rdlocked = false ;
	        op->fl.wrlocked = true ;
	        lockcmd = F_WLOCK ;
	    }
	    /* get out if we have the lock that we want alrecipy */
	    if (! f_already) {
	        rs = lockfile(op->fd,lockcmd,0z,0z,TO_LOCK) ;
	    }
	} /* end if (ok) */
	return rs ;
}
/* end subroutine (msgid_lockacquire) */

static int msgid_lockrelease(msgid *op) noex {
	int		rs = SR_OK ;
	if ((op->fl.rdlocked || op->fl.wrlocked)) {
	    if (op->fd >= 0) {
	        rs = lockfile(op->fd,F_ULOCK,0L,0L,TO_LOCK) ;
	    }
	    op->fl.rdlocked = false ;
	    op->fl.wrlocked = false ;
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
	cint		ne = 100 ;
	int		rs ;
	int		rs1 ;
	int		ei = 0 ;
	bool		f = false ;
	if (msgide ma ; (rs = ma.start) >= 0) {
	    int		n ;
	    char	*bep = nullptr ;
	    while ((rs >= 0) && (! f)) {
	        cint	eoff = MSGID_FOTAB + (ei * op->entsz) ;
	        cint	len = ne * op->entsz ;
	        char	*bp ;
	        rs = msgid_bufload(op,eoff,len,&bp) ;
	        if (rs < op->entsz) break ;
	        n = (rs / op->entsz) ;
	        for (int i = 0 ; i < n ; i += 1) {
	            bep = bp + (i * op->entsz) ;
		    if ((rs = ma.wr(bep)) >= 0) {
	                cchar	*cp = ma.messageid ;
	                f = strnncmp(midp,midl,cp,var.lmsgid) ;
		    } /* end if (msgide_wr) */
	            if (f) break ;
	            ei += 1 ;
	        } /* end for */
	    } /* end while */
	    if ((rs >= 0) && f) {
	        if (bepp) *bepp = bep ;
	    }
	    rs1 = ma.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (msgide) */
	if ((rs >= 0) && (! f)) {
	    rs = SR_NOTFOUND ;
	}
	return (rs >= 0) ? ei : rs ;
}
/* end subroutine (msgid_searchid) */

/* search by recipient AND message-id */
static int msgid_search(msgid *op,msgid_key *kp,uint khash,char **bepp) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		ne = 100 ;
	int		ei = 0 ;
	bool		f = false ;
	if (msgide ew ; (rs = ew.start) >= 0) {
	    char	*bep ;
	    while ((rs >= 0) && (! f)) {
	        cint	eoff = MSGID_FOTAB + (ei * op->entsz) ;
	        cint	len = ne * op->entsz ;
	        char	*bp ;
	        rs = msgid_bufload(op,eoff,len,&bp) ;
	        if (rs < op->entsz) break ;
	        bep = bp ;
	        cint	n = (rs / op->entsz) ;
	        for (int i = 0 ; i < n ; i += 1) {
		    if ((rs = ew.wr(bep)) >= 0) {
			if (ew.hash == khash) {
	                    f = keymat(kp,&ew) ;
			}
		    }
	            if (f) break ;
	            bep += op->entsz ;
	            ei += 1 ;
		    if (rs < 0) break ;
	        } /* end for */
	    } /* end while */
	    if (bepp) *bepp = bep ;
	    rs1 = ew.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (msgide) */
	if ((rs >= 0) && (! f)) {
	    rs = SR_NOTFOUND ;
	}
	return (rs >= 0) ? ei : rs ;
}
/* end subroutine (msgid_search) */

/* find an empty slot */
static int msgid_searchempty(msgid *op,oldent *oep,char **bepp) noex {
	uint		ra ;
	uint		rb ;
	cint		rsn = SR_NOTFOUND ;
	int		rs = SR_NOTFOUND ;
	int		ei_start = 0 ;
	int		ei_mark ;
	int		n = 0 ;
	int		ei = 0 ;
	oep->utime = INT_MAX ;
	oep->ei = 0 ;
	if (op->b.off >= MSGID_FLTOP) {
	    ra = uceil((op->b.off - MSGID_FLTOP),op->entsz) ;
	    rb = ufloor((op->b.off + op->b.len - MSGID_FLTOP),op->entsz) ;
	    ei_start = ra / op->entsz ;
	    n = (rb > ra) ? ((rb - ra) / op->entsz) : 0 ;
	} /* end if */
	ei_mark = ei_start ;
	if (n > 0) {
	    if ((rs = msgid_searchemptyrange(op,ei_start,n,oep,bepp)) >= 0) {
	        ei = rs ;
	    } else if (rs == rsn) {
	        ei_start += n ;
	    }
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
		oldent *oep,char **bepp) noex {
	cint		ne = min(nmax,100) ;
	int		rs ;
	int		rs1 ;
	bool		f = false ;
	if (msgide ew ; (rs = ew.start) >= 0) {
	    char	*bep{} ;
	    while ((rs >= 0) && (! f)) {
	        cint	eoff = MSGID_FOTAB + (ei * op->entsz) ;
	        cint	len = ne * op->entsz ;
	        int	n ;
	        char	*bp ;
	        rs = msgid_bufload(op,eoff,len,&bp) ;
	        if (rs < op->entsz) break ;
	        n = (rs / op->entsz) ;
	        for (int i = 0 ; i < n ; i += 1) {
	            bep = bp + (i * op->entsz) ;
		    if ((rs = ew.wr(bep)) >= 0) {
	                if ((f = (ew.messageid[0] == '\0'))) {
	                    oep->utime = ew.utime ;
	                    oep->ei = ei ;
			}
	            }
		    if (f) break ;
	            ei += 1 ;
	        } /* end for */
	    } /* end while */
	    if (bepp) *bepp = bep ;
	    rs1 = ew.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (msgide) */
	if ((rs >= 0) && (! f)) {
	    rs = SR_NOTFOUND ;
	}
	return (rs >= 0) ? ei : rs ;
}
/* end subroutine (msgid_searchemptyrange) */

/* buffer mangement stuff */
static int msgid_bufbegin(msgid *op) noex {
	cint		bsz =  MSGID_BUFSIZE ;
	int		rs ;
	op->b.off = 0 ;
	op->b.len = 0 ;
	op->b.mlen = 0 ;
	op->b.mbuf = nullptr ;
	if (char *bp ; (rs = uc_malloc(bsz,&bp)) >= 0) {
	    op->b.mlen = bsz ;
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
	uint		fext = (op->b.off + op->b.len) ;
	uint		rext = uint(roff + rlen) ;
	uint		boff = uint(roff) ;
	int		rs = SR_OK ;
	int		len = rlen ;/* return-value */
	/* do we need to read in more data? */
	if ((boff < op->b.off) || (rext > fext)) {
	    off_t	poff ;
	    uint	foff ;
	    uint	bend = op->b.off + op->b.mlen ;
	    uint	ext ;
	    char	*rbuf ;
	    /* can we do an "add-on" type read operation? */
	    if ((boff >= op->b.off) && (rext <= bend)) {
	        foff = op->b.off + op->b.len ;
	        rbuf = op->b.mbuf + op->b.len ;
	        ext = roff + max(rlen,MSGID_READSIZE) ;
	        fext = uceil(ext,op->pagesize) ;
	        if (fext > bend) {
	            fext = bend ;
		}
	        len = fext - foff ;
	        poff = foff ;
	        if ((rs = u_pread(op->fd,rbuf,len,poff)) >= 0) {
	            op->b.len += rs ;
		    uint	t1len = ((op->b.off + op->b.len) - boff) ;
		    uint	t2len = uint(rlen) ;
	            len = min(t1len,t2len) ;
	        } /* end if */
	    } else {
	        op->b.off = roff ;
	        op->b.len = 0 ;
	        bend = roff + op->b.mlen ;
	        foff = roff ;
	        rbuf = op->b.mbuf ;
	        ext = roff + max(rlen,MSGID_READSIZE) ;
	        fext = uceil(ext,op->pagesize) ;
	        if (fext > bend) {
	            fext = bend ;
		}
	        len = fext - foff ;
	        if ((rs = u_pread(op->fd,op->b.mbuf,len,foff)) >= 0) {
	            op->b.len = rs ;
	            len = min(rs,rlen) ;
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
	int		boff = op->b.off ;
	int		bext ;
	int		rext = (roff + rbuflen) ;
	int		buflen = op->b.len ;
	int		bdiff ;
	bool		f_done = false ;
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
	coff		uoff = MSGID_FOHEAD ;
	int		rs ;
	char		fbuf[MSGID_FBUFLEN + 1] ;
	{
	    cint	bl = filehead(&op->h,funmode::rd,fbuf) ;
	    rs = u_pwrite(op->fd,fbuf,bl,uoff) ;
	}
	return rs ;
}
/* end subroutine (msgid_writehead) */

static int filemagic(MSGID_FM *mp,cfm fm, char *mbuf) noex {
    	cint		msz = MSGID_MAGICSIZE ;
	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	if (mp && mbuf) {
	    char	*bp = mbuf ;
	    rs = 20 ;
	    switch (fm) {
	    case funmode::wr:
	        {
	            bp[msz-1] = '\0' ;
	            strncpy(mp->magic,bp,msz-1) ;
	            if (char *ep ; (ep = strchr(mp->magic,'\n')) != np) {
	                *ep = '\0' ;
	            }
	            bp += msz ;
	            memcpy(mp->vetu,bp,4) ;
		}
		break ;
	    case funmode::rd:
		if ((rs = mkmagic(bp,msz,mp->magic)) >= 0) {
	            bp = (mbuf + msz) ;
	            memcpy(bp,mp->vetu,4) ;
		}
		break ;
	    default:
		rs = SR_INVALID ;
		break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (filemagic) */

/* encode or decode the file header */
static int filehead(MSGID_FH *hp,cfm fm,char *mbuf) noex {
	cint		mlen = szof(MSGID_FH) ;
	int		rs ;
	int		rs1 ;
	if (serialbuf sb ; (rs = sb.start(mbuf,mlen)) >= 0) {
	    switch (fm) {
	    case funmode::wr:
		{
	            sb >> hp->wcount ;
	            sb >> hp->wtime ;
	            sb >> hp->nentries ;
		}
		break ;
	    case funmode::rd:
		{
	            sb << hp->wcount ;
	            sb << hp->wtime ;
	            sb << hp->nentries ;
		}
	        break ;
	    default:
		rs = SR_INVALID ;
		break ;
	    } /* end switch */
	    rs1 = sb.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (serialbuf) */
	return rs ;
}
/* end subroutine (filehead) */

static uint keyhash(msgid_key *kp) noex {
	uint		khash = 0 ;
	cint		reclen = lenstr(kp->recip,var.lrecip) ;
	cint		midlen = lenstr(kp->msgid,var.lmsgid) ;
	khash += hash_elf(kp->recip,reclen) ;
	khash += hash_elf(kp->msgid,midlen) ;
	return khash ;
}
/* end subroutine (keyhash) */

static bool keymat(msgid_key *kp,msgide *ep) noex {
    	bool		f = true ;
	f = f && (strcmp(kp->msgid,ep->messageid) == 0) ;
	f = f && (strcmp(kp->recip,ep->recipient) == 0) ;
	return f ;
}
/* end subroutine (keymat) */

vars::operator int () noex {
        int		rs ;
	int		rs1 ;
	if ((rs = getbufsize(getbufsize_hn)) >= 0) {
	    hostnamelen = rs ;
	    if ((rs = ucpagesize) >= 0) {
		pagesize = rs ;
		if (msgide ma ; (rs = ma.start) >= 0) {
		    {
			entsz = ma.entsz ;
			lrecip = ma.len.recipient ;
			lmsgid = ma.len.messageid ;
		    }
		    rs1 = ma.finish ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (msgide) */
	    }
	} /* end if (getbufsize) */
	return rs ;
}
/* end method (vars::operator) */


